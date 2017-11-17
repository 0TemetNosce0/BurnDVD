// BurnDVD.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "BurnDVD.h"
#include "debuglog.h"
#include "BurnInterface.h"
#include "DiscMaster.h"
#include "DiscRecorder.h"
#include "DiscFormatData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CLIENT_NAME		_T("Digiarty BurnDVD")

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "shlwapi.lib")

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CBurnDVDApp

BEGIN_MESSAGE_MAP(CBurnDVDApp, CWinApp)
END_MESSAGE_MAP()


// CBurnDVDApp construction

CBurnDVDApp::CBurnDVDApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	isBurning = false;
	Devices = NULL;

	hWnd = NULL;
	Device = NULL;
	IsoFile = NULL;
	isIMAPI2Uninstalled = false;
	Volume = L"Digiarty";
	Stream = NULL;
}

CBurnDVDApp::~CBurnDVDApp() {
	Release();
}


// The one and only CBurnDVDApp object

CBurnDVDApp theApp;


// CBurnDVDApp initialization

BOOL CBurnDVDApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

CString	CBurnDVDApp::GetMediaTypeString(int mediaType, CDiscInfo* discInfo)
{
    switch (mediaType)
    {
    case IMAPI_MEDIA_TYPE_UNKNOWN:
    default:
        return _T("Unknown Media Type");

    case IMAPI_MEDIA_TYPE_CDROM:
		discInfo->isCdromSupported = true;
        return _T("CD-ROM or CD-R/RW media");

    case IMAPI_MEDIA_TYPE_CDR:
        discInfo->isCdromSupported = true;
        return _T("CD-R");

    case IMAPI_MEDIA_TYPE_CDRW:
        discInfo->isCdromSupported = true;
        return _T("CD-RW");

    case IMAPI_MEDIA_TYPE_DVDROM:
        discInfo->isDvdSupported = true;
        return _T("DVD ROM");

    case IMAPI_MEDIA_TYPE_DVDRAM:
        discInfo->isDvdSupported = true;
        return _T("DVD-RAM");

    case IMAPI_MEDIA_TYPE_DVDPLUSR:
        discInfo->isDvdSupported = true;
        return _T("DVD+R");

    case IMAPI_MEDIA_TYPE_DVDPLUSRW:
        discInfo->isDvdSupported = true;
        return _T("DVD+RW");

    case IMAPI_MEDIA_TYPE_DVDPLUSR_DUALLAYER:
        discInfo->isDualLayerDvdSupported = true;
        return _T("DVD+R Dual Layer");

    case IMAPI_MEDIA_TYPE_DVDDASHR:
        discInfo->isDvdSupported = true;
        return _T("DVD-R");

    case IMAPI_MEDIA_TYPE_DVDDASHRW:
        discInfo->isDvdSupported = true;
        return _T("DVD-RW");

    case IMAPI_MEDIA_TYPE_DVDDASHR_DUALLAYER:
        discInfo->isDualLayerDvdSupported = true;
        return _T("DVD-R Dual Layer");

    case IMAPI_MEDIA_TYPE_DISK:
        return _T("random-access writes");

    case IMAPI_MEDIA_TYPE_DVDPLUSRW_DUALLAYER:
        discInfo->isDualLayerDvdSupported = true;
        return _T("DVD+RW DL");

    case IMAPI_MEDIA_TYPE_HDDVDROM:
        return _T("HD DVD-ROM");

    case IMAPI_MEDIA_TYPE_HDDVDR:
        return _T("HD DVD-R");

    case IMAPI_MEDIA_TYPE_HDDVDRAM:
        return _T("HD DVD-RAM");

    case IMAPI_MEDIA_TYPE_BDROM:
        return _T("Blu-ray DVD (BD-ROM)");

    case IMAPI_MEDIA_TYPE_BDR:
        return _T("Blu-ray media");

    case IMAPI_MEDIA_TYPE_BDRE:
        return _T("Blu-ray Rewritable media");
    }
}


void CBurnDVDApp::GetMediaStatusString(int mediaStatus, CDiscInfo* discInfo) {
	CStringArray mediaStatusString;
	if (IMAPI_FORMAT2_DATA_MEDIA_STATE_UNKNOWN & mediaStatus) {
		mediaStatusString.Add(_T("Indicates that the interface does not know the media state."));
		//未知的状态
		discInfo->isSupported = false;
	}
	if (IMAPI_FORMAT2_DATA_MEDIA_STATE_INFORMATIONAL_MASK & mediaStatus) {
		mediaStatusString.Add(_T("Reports information (but not errors) about the media state."));
		//报告媒体状态信息(非错误信息)
		discInfo->isInitialized = true;
	}
	if (IMAPI_FORMAT2_DATA_MEDIA_STATE_UNSUPPORTED_MASK & mediaStatus) {
		mediaStatusString.Add(_T("Reports an unsupported media state."));
		//不支持
		discInfo->isSupported = false;
	}
	if (IMAPI_FORMAT2_DATA_MEDIA_STATE_OVERWRITE_ONLY & mediaStatus) {
		mediaStatusString.Add(_T("Write operations can occur on used portions of the disc."));
		//可擦写
		discInfo->isWriterable = true;
	}
	if (IMAPI_FORMAT2_DATA_MEDIA_STATE_RANDOMLY_WRITABLE & mediaStatus) {
		mediaStatusString.Add(_T("Media is randomly writable. This indicates that a single session can be written to this disc."));
		//随机写
		discInfo->isWriterable = true;
	}
	if (IMAPI_FORMAT2_DATA_MEDIA_STATE_BLANK & mediaStatus) {
		mediaStatusString.Add(_T("Media has never been used, or has been erased."));
		//空白盘
		discInfo->isBlank = true;
	}
	if (IMAPI_FORMAT2_DATA_MEDIA_STATE_APPENDABLE & mediaStatus) {
		mediaStatusString.Add(_T("Media is appendable (supports multiple sessions)."));
		//可追加
		discInfo->isWriterable = true;
	}
	if (IMAPI_FORMAT2_DATA_MEDIA_STATE_FINAL_SESSION & mediaStatus) {
		mediaStatusString.Add(_T("Media can have only one additional session added to it, or the media does not support multiple sessions."));
		//不可追加
		discInfo->isWriterable = true;
	}
	if (IMAPI_FORMAT2_DATA_MEDIA_STATE_DAMAGED & mediaStatus) {
		mediaStatusString.Add(_T("Media is not usable by this interface. The media might require an erase or other recovery."));
		//可能需要擦除或其他恢复。
		discInfo->isSupported = false;
	}
	if (IMAPI_FORMAT2_DATA_MEDIA_STATE_ERASE_REQUIRED & mediaStatus) {
		mediaStatusString.Add(_T("Media must be erased prior to use by this interface."));
		//必须擦除
		discInfo->isSupported = false;
		discInfo->isBlank = false;
	}
	if (IMAPI_FORMAT2_DATA_MEDIA_STATE_NON_EMPTY_SESSION & mediaStatus) {
		mediaStatusString.Add(_T("Media has a partially written last session, which is not supported by this interface."));
		//媒体已经写入一部分信息 并且这个接口不支持
		discInfo->isSupported = false;
	}
	if (IMAPI_FORMAT2_DATA_MEDIA_STATE_WRITE_PROTECTED & mediaStatus) {
		mediaStatusString.Add(_T("Media or drive is write-protected."));
		//写保护
		discInfo->isWriterable = false;
	}
	if (IMAPI_FORMAT2_DATA_MEDIA_STATE_FINALIZED & mediaStatus) {
		mediaStatusString.Add(_T("Media cannot be written to (finalized)."));
		//媒体不可写
		discInfo->isWriterable = false;
	}
	if (IMAPI_FORMAT2_DATA_MEDIA_STATE_UNSUPPORTED_MEDIA & mediaStatus) {
		mediaStatusString.Add(_T("Media is not supported by this interface."));
		//不支持
		discInfo->isSupported = false;
	}

	discInfo->mediaStatus = new CTexts(mediaStatusString.GetCount());
	for (int i=0; i<mediaStatusString.GetCount(); i++) {
		discInfo->mediaStatus->text[i] = _wcsdup(mediaStatusString.GetAt(i));
	}
}

CString	CBurnDVDApp::GetPhysicalTypeString(int physicalType, CDiscInfo* discInfo) {
	switch(physicalType) {
		case IMAPI_MEDIA_TYPE_UNKNOWN:            discInfo->physicalType = _wcsdup(_T("Unknown"));			break;
		case IMAPI_MEDIA_TYPE_CDROM:              discInfo->physicalType = _wcsdup(_T("CD-ROM or CD-R/RW"));break;
		case IMAPI_MEDIA_TYPE_CDR:                discInfo->physicalType = _wcsdup(_T("CD-R"));				break;
		case IMAPI_MEDIA_TYPE_CDRW:               discInfo->physicalType = _wcsdup(_T("CD-RW"));			break;
		case IMAPI_MEDIA_TYPE_DVDROM:             discInfo->physicalType = _wcsdup(_T("read-only"));		break;
		case IMAPI_MEDIA_TYPE_DVDRAM:             discInfo->physicalType = _wcsdup(_T("DVD-RAM"));			break;
		case IMAPI_MEDIA_TYPE_DVDPLUSR:           discInfo->physicalType = _wcsdup(_T("DVD+R"));			break;
		case IMAPI_MEDIA_TYPE_DVDPLUSRW:          discInfo->physicalType = _wcsdup(_T("DVD+RW"));			break;
		case IMAPI_MEDIA_TYPE_DVDPLUSR_DUALLAYER: discInfo->physicalType = _wcsdup(_T("DVD+R DL"));			break;
		case IMAPI_MEDIA_TYPE_DVDDASHR:           discInfo->physicalType = _wcsdup(_T("DVD-R"));			break;
		case IMAPI_MEDIA_TYPE_DVDDASHRW:          discInfo->physicalType = _wcsdup(_T("DVD-RW"));			break;
		case IMAPI_MEDIA_TYPE_DVDDASHR_DUALLAYER: discInfo->physicalType = _wcsdup(_T("DVD-R DL"));			break;
		case IMAPI_MEDIA_TYPE_DISK:               discInfo->physicalType = _wcsdup(_T("random-access"));	break;
		case IMAPI_MEDIA_TYPE_DVDPLUSRW_DUALLAYER:discInfo->physicalType = _wcsdup(_T("DVD+RW DL"));		break;
		case IMAPI_MEDIA_TYPE_HDDVDROM:           discInfo->physicalType = _wcsdup(_T("HD DVD-ROM"));		break;
		case IMAPI_MEDIA_TYPE_HDDVDR:             discInfo->physicalType = _wcsdup(_T("HD DVD-R"));			break;
		case IMAPI_MEDIA_TYPE_HDDVDRAM:           discInfo->physicalType = _wcsdup(_T("HD DVD-RAM"));		break;
		case IMAPI_MEDIA_TYPE_BDROM:              discInfo->physicalType = _wcsdup(_T("BD-ROM"));			break;
		case IMAPI_MEDIA_TYPE_BDR:                discInfo->physicalType = _wcsdup(_T("BD-R"));				break;
		case IMAPI_MEDIA_TYPE_BDRE:               discInfo->physicalType = _wcsdup(_T("BD-RE"));			break;
		//case IMAPI_MEDIA_TYPE_MAX:                DEBUGLOG(_T("This value is the maximum value defined in IMAPI_MEDIA_PHYSICAL_TYPE.\n"));	break;
	}
	return discInfo->physicalType;
}



bool CBurnDVDApp::CreateMediaFileSystem(IMAPI_MEDIA_PHYSICAL_TYPE mediaType, IStream** ppDataStream, const CString& folder)
{
    IFileSystemImage*		image = NULL;
    IFileSystemImageResult*	imageResult = NULL;
    IFsiDirectoryItem*		rootItem = NULL;
    CString					message;
    bool					returnVal = false;

    HRESULT hr = CoCreateInstance(CLSID_MsftFileSystemImage,
        NULL, CLSCTX_ALL, __uuidof(IFileSystemImage), (void**)&image);
    if (FAILED(hr) || (image == NULL)) {
        SendMessage(hWnd, WM_BURN_FINISHED, hr, (LPARAM)_T("Failed to create IFileSystemImage Interface"));
        return false;
    }

    SendMessage(hWnd, WM_BURN_STATUS_MESSAGE, 0, (LPARAM)_T("Creating File System..."));

    image->put_FileSystemsToCreate((FsiFileSystems)(FsiFileSystemJoliet|FsiFileSystemISO9660));
	image->put_VolumeName(Volume.AllocSysString());
    image->ChooseImageDefaultsForMediaType(mediaType);

    //
    // Get the image root
    //
    hr = image->get_Root(&rootItem);
    if (SUCCEEDED(hr)) {
        hr = rootItem->AddTree(folder.AllocSysString(), VARIANT_TRUE);

        if (FAILED(hr)) {
            // IMAPI_E_IMAGE_SIZE_LIMIT 0xc0aab120
            message.Format(_T("Failed IFsiDirectoryItem->AddTree(%s)!"), folder);
            SendMessage(hWnd, WM_BURN_FINISHED, hr, (LPARAM)(LPCTSTR)message);
            return false;
        }

        // Create the result image
        hr = image->CreateResultImage(&imageResult);
        if (SUCCEEDED(hr)) {
            //
            // Get the stream
            //
            hr = imageResult->get_ImageStream(ppDataStream);
            if (SUCCEEDED(hr)) {
                returnVal = true;
            }
            else {
                SendMessage(hWnd, WM_BURN_FINISHED, hr, (LPARAM)_T("Failed IFileSystemImageResult->get_ImageStream!"));
            }

        }
        else {
            SendMessage(hWnd, WM_BURN_FINISHED, hr, (LPARAM)_T("Failed IFileSystemImage->CreateResultImage!"));
        }
    }
    else {
        SendMessage(hWnd, WM_BURN_FINISHED, hr, (LPARAM)_T("Failed IFileSystemImage->getRoot"));
    }

    //
    // Cleanup
    //
    if (image != NULL)
        image->Release();
    if (imageResult != NULL)
        imageResult->Release();
    if (rootItem != NULL)
        rootItem->Release();

    return returnVal;
}


CDevices* CBurnDVDApp::GetDevices() {
	if (Devices)
		return Devices;

	CDiscMaster discMaster; 
	if (!discMaster.Initialize()) {
		DEBUGLOG(discMaster.GetErrorMessage());
		if (discMaster.GetHresult() == 0x80040154) {
			isIMAPI2Uninstalled = true;
		}
		return NULL;
	}

	Devices = new CDevices();
	Devices->totalDevice = discMaster.GetTotalDevices();
	Devices->Devices = new CDeviceInfo*[Devices->totalDevice];

	int initializedDevice = 0;
	for (long i=0; i<Devices->totalDevice; i++) {
		CString recorderUniqueID = discMaster.GetDeviceUniqueID(i);
		if (recorderUniqueID.IsEmpty()) {
			DEBUGLOG(discMaster.GetErrorMessage());
			Devices->Devices[i] = NULL;
			continue;
		}
		Devices->Devices[i] = new CDeviceInfo();
		Devices->Devices[i]->recorderUniqueID = _wcsdup(recorderUniqueID);
		if (InitializeDevice(Devices->Devices[i])) {
			if (InitializeDisc(Devices->Devices[i])) {
				initializedDevice++;
			}
		}
	}

	//至少有一个设备初始化成功
	//if (initializedDevice) {
	//	return Devices;
	//}
	//else {
	//	delete Devices;
	//	Devices = NULL;
	//	return NULL;
	//}

	//过滤设备, 有些虚拟光驱无法识别
	for (int i=0; i<Devices->totalDevice; i++) {
		if (Devices->Devices[i]) {
			if (Devices->Devices[i]->volumePath == NULL) {
				//没有盘符的设备
				delete Devices->Devices[i];
				Devices->Devices[i] = NULL;
			}
		}
	}

	//设备排序
	for (int i=0; i<Devices->totalDevice; i++) {
		for (int j=i+1; j<Devices->totalDevice; j++) {
			if (Devices->Devices[i] == NULL) {
				if (Devices->Devices[j] != NULL) {
					// Devices->Devices[i] == NULL && Devices->Devices[j] != NULL
					Devices->Devices[i] = Devices->Devices[j];
					Devices->Devices[j] = NULL;
				}
				else {
					// Devices->Devices[i] == NULL && Devices->Devices[j] == NULL
					continue;
				}
			}
			else {
				if (Devices->Devices[j] != NULL) {
					// Devices->Devices[i] != NULL && Devices->Devices[j] != NULL
					if (wcscmp(Devices->Devices[i]->volumePath, Devices->Devices[j]->volumePath) > 0) {
						CDeviceInfo *device = Devices->Devices[i];
						Devices->Devices[i] = Devices->Devices[j];
						Devices->Devices[j] = device;
					}
				}
				else {
					// Devices->Devices[i] != NULL && Devices->Devices[j] == NULL
					continue;
				}
			}
		}
	}

	//重新计数
	int total = 0;
	for (int i=0; i<Devices->totalDevice; i++) {
		if (Devices->Devices[i] == NULL) {
			break;
		}
		total++;
	}
	Devices->totalDevice = total;

	return Devices;
}

bool CBurnDVDApp::InitializeDevice(CDeviceInfo* device) {
    CDiscRecorder* pDiscRecorder = new CDiscRecorder();
    ASSERT(pDiscRecorder != NULL);
    if (pDiscRecorder == NULL)
        return false;

    if (!pDiscRecorder->Initialize(device->recorderUniqueID)) {
        if (FAILED(pDiscRecorder->GetHresult())) {
            DEBUGLOG(_T("Failed to initialize recorder - Error:0x%08x\n\nRecorder Unique ID:%s"),
                pDiscRecorder->GetHresult(), (LPCTSTR)device->recorderUniqueID);
        }
        delete pDiscRecorder;
		return false;
    }

    //
    // Get the volume path(s). usually just 1
    //
    CString volumeList;
    ULONG totalVolumePaths = pDiscRecorder->GetTotalVolumePaths();
    for (ULONG volIndex = 0; volIndex < totalVolumePaths; volIndex++) {
        if (volIndex)
            volumeList += _T(",");
        volumeList += pDiscRecorder->GetVolumePath(volIndex);
    }
	device->volumePath = _wcsdup(volumeList);

    //
    // Add Drive to combo and IDiscRecorder as data
    //
	device->productId = _wcsdup(pDiscRecorder->GetProductID());
	device->vendorId = _wcsdup(pDiscRecorder->GetVendorId());
	device->productRevision = _wcsdup(pDiscRecorder->GetProductRevision());
	device->discRecorder = pDiscRecorder;
	return true;
}

bool CBurnDVDApp::InitializeDisc(CDeviceInfo* device) {
	
	if (device->discInfo) {
		delete device->discInfo;
		device->discInfo = NULL;
	}
	
    CDiscRecorder* discRecorder = (CDiscRecorder*)device->discRecorder;
    if (discRecorder != NULL) {
        CDiscFormatData discFormatData;
        if  (!discFormatData.Initialize(discRecorder, CLIENT_NAME)) {
			DEBUGLOG(discFormatData.GetErrorMessage());
            return false;
        }

        //
        // Display Supported Media Types
        //
        ULONG totalMediaTypes = discFormatData.GetTotalSupportedMediaTypes();
		if (totalMediaTypes <= 0) {
			DEBUGLOG(_T("GetTotalSupportedMediaTypes(0)"));
			return false;
		}

		ULONG totalWriteSpeeds = discFormatData.GetTotalWriteSpeeds();
		if (totalWriteSpeeds <= 0) {
			DEBUGLOG(_T("totalWriteSpeeds(0)"));
			return false;
		}

		device->discInfo = new CDiscInfo();
		device->discInfo->mediaType = new CTexts(totalMediaTypes);
        for (ULONG volIndex = 0; volIndex < totalMediaTypes; volIndex++) {
            int mediaType = discFormatData.GetSupportedMediaType(volIndex);
            CString supportedMediaTypes = GetMediaTypeString(mediaType, device->discInfo);
			device->discInfo->mediaType->text[volIndex] = _wcsdup(supportedMediaTypes);
        }

		device->discInfo->writeSpeed = new CTexts(totalWriteSpeeds);
		for (ULONG index = 0; index < totalWriteSpeeds; index++) {
			CString writeSpeed;
			writeSpeed.Format(_T("%0.1fx"), discFormatData.GetWriteSpeed(index));
			device->discInfo->writeSpeed->text[index] = _wcsdup(writeSpeed);
		}


		device->discInfo->totalSpace = discFormatData.GetMediaTotalSpace();
		device->discInfo->freeSpace = discFormatData.GetMediaFreeSpace();

		GetMediaStatusString(discFormatData.GetMediaStatus(), device->discInfo);
		GetPhysicalTypeString(discFormatData.GetPhysicalType(), device->discInfo);
		return true;
    }
	return false;
}

UINT CBurnDVDApp::BurnThread(LPVOID pParam) {
	CBurnDVDApp* burnDVDApp = (CBurnDVDApp*)pParam;

	if (burnDVDApp == NULL) {
        DEBUGLOG(_T("Error: Param is NULL"));
        return 0;
	}

	CDeviceInfo*  DeviceInfo = burnDVDApp->GetBurnDevice();
	if (DeviceInfo == NULL || DeviceInfo->discRecorder == NULL) {
		SendMessage(burnDVDApp->GetWnd(), WM_BURN_FINISHED, 0, (LPARAM)_T("Error: No Data for selected device"));
        return 0;
    }
	CDiscRecorder* pOrigDiscRecorder = (CDiscRecorder*)DeviceInfo->discRecorder;

    //
    // Did user cancel?
    //
	if (burnDVDApp->GetBurning()) {
        SendMessage(burnDVDApp->GetWnd(), WM_BURN_FINISHED, 0, (LPARAM)_T("User Canceled!"));
        return 0;
    }

    SendMessage(burnDVDApp->GetWnd(), WM_BURN_STATUS_MESSAGE, 0, (LPARAM)_T("Initializing Disc Recorder..."));

    //
    // Create another disc recorder because we're in a different thread
    //
    CDiscRecorder discRecorder;

    CString errorMessage;
    if (discRecorder.Initialize(pOrigDiscRecorder->GetUniqueId())) {
        if (discRecorder.AcquireExclusiveAccess(true, CLIENT_NAME)) {
            CDiscFormatData discFormatData;
            if (discFormatData.Initialize(&discRecorder, CLIENT_NAME)) {
                //
                // Get the media type currently in the recording device
                //
                IMAPI_MEDIA_PHYSICAL_TYPE mediaType = IMAPI_MEDIA_TYPE_UNKNOWN;
                discFormatData.GetInterface()->get_CurrentPhysicalMediaType(&mediaType);

                //
                // Create the file system
                //
				IStream* Stream = burnDVDApp->GetStream();
				if (Stream == NULL)
					return 0;

                discFormatData.SetCloseMedia(true);

                //
                // Burn the data, this does all the work
                //
				discFormatData.Burn(burnDVDApp->GetWnd(), Stream);

                //
                // Eject Media if they chose
                //
                if (burnDVDApp->m_ejectWhenFinished) {
                    discRecorder.EjectMedia();
                }

            }

            discRecorder.ReleaseExclusiveAccess();

            //
            // Finished Burning, GetHresult will determine if it was successful or not
            //
			SendMessage(burnDVDApp->GetWnd(), WM_BURN_FINISHED, discFormatData.GetHresult(), (LPARAM)(LPCTSTR)discFormatData.GetErrorMessage());
        }
        else {
            errorMessage.Format(_T("Failed: %s is exclusive owner"), (LPCTSTR)discRecorder.ExclusiveAccessOwner());
            SendMessage(burnDVDApp->GetWnd(), WM_BURN_FINISHED, discRecorder.GetHresult(), (LPARAM)(LPCTSTR)errorMessage);
        }
    }
    else {
        errorMessage.Format(_T("Failed to initialize recorder - Unique ID:%s"), (LPCTSTR)pOrigDiscRecorder->GetUniqueId());
        SendMessage(burnDVDApp->GetWnd(), WM_BURN_FINISHED, discRecorder.GetHresult(), (LPARAM)(LPCTSTR)errorMessage);
    }

    return 0;
}

bool CBurnDVDApp::BurnISO(HWND hwnd, CDeviceInfo* device, wchar_t* isoFile) {
	this->hWnd = hwnd;
	this->Device = device;

	HRESULT m_hResult = SHCreateStreamOnFile(isoFile, STGM_SHARE_DENY_WRITE, &Stream);
	if (!SUCCEEDED(m_hResult)) {
		DEBUGLOG(_T("SHCreateStreamOnFile Failed - Error:0x%08x\n"), m_hResult);
		SendMessage(hWnd, WM_BURN_FINISHED, 0, (LPARAM)_T("SHCreateStreamOnFile Failed"));
		return false;
	}
	AfxBeginThread(BurnThread, this, THREAD_PRIORITY_NORMAL);
	return true;
}

bool CBurnDVDApp::BurnFolder(HWND hwnd, CDeviceInfo* device, wchar_t* folder) {
	this->hWnd = hwnd;
	this->Device = device;	

    CDiscRecorder* pOrigDiscRecorder = (CDiscRecorder*)device->discRecorder;
    if (pOrigDiscRecorder == NULL) {
        SendMessage(hWnd, WM_BURN_FINISHED, 0, (LPARAM)_T("Error: No Data for selected device"));
        return false;
    }

    SendMessage(hWnd, WM_BURN_STATUS_MESSAGE, 0, (LPARAM)_T("Initializing Disc Recorder..."));

    //
    // Create another disc recorder because we're in a different thread
    //
    CDiscRecorder discRecorder;
    CString errorMessage;
    if (discRecorder.Initialize(pOrigDiscRecorder->GetUniqueId())) {
        if (discRecorder.AcquireExclusiveAccess(true, CLIENT_NAME)) {
            CDiscFormatData discFormatData;
            if (discFormatData.Initialize(&discRecorder, CLIENT_NAME)) {
                //
                // Get the media type currently in the recording device
                //
                IMAPI_MEDIA_PHYSICAL_TYPE mediaType = IMAPI_MEDIA_TYPE_UNKNOWN;
                discFormatData.GetInterface()->get_CurrentPhysicalMediaType(&mediaType);

                //
                // Create the file system
                //
                if (!CreateMediaFileSystem(mediaType, &Stream, folder)) {	
					// CreateMediaFileSystem reported error to UI
                    return false;
                }
            }
        }
        else {
            errorMessage.Format(_T("Failed: %s is exclusive owner"), (LPCTSTR)discRecorder.ExclusiveAccessOwner());
            SendMessage(hWnd, WM_BURN_FINISHED, discRecorder.GetHresult(), (LPARAM)(LPCTSTR)errorMessage);
        }
    }
    else
    {
        errorMessage.Format(_T("Failed to initialize recorder - Unique ID:%s"), (LPCTSTR)pOrigDiscRecorder->GetUniqueId());
        SendMessage(hWnd, WM_BURN_FINISHED, discRecorder.GetHresult(), (LPARAM)(LPCTSTR)errorMessage);
    }

	AfxBeginThread(BurnThread, this, THREAD_PRIORITY_NORMAL);
	return true;
}

void CBurnDVDApp::Stop() {
}

void CBurnDVDApp::Release() {
	if (Devices) {
		for (long i=0; i<Devices->totalDevice; i++) {
			if (Devices->Devices[i] == NULL)
				continue;
			CDiscRecorder* pDiscRecorder = (CDiscRecorder*)Devices->Devices[i]->discRecorder;
			if (pDiscRecorder) {
				delete pDiscRecorder;
				Devices->Devices[i]->discRecorder = NULL;
			}
		}
		delete Devices;
		Devices = NULL;
	}
}



/////////////////////////////////////////////////////////
CDevices* GetDevices() {
	return theApp.GetDevices();
}

bool InitializeDisc(CDeviceInfo* Device) {
	return theApp.InitializeDisc(Device);
}

bool BurnISO(HWND hwnd, CDeviceInfo* Device, wchar_t* isoFile) {
	return theApp.BurnISO(hwnd, Device, isoFile);
}

bool BurnFolder(HWND hwnd, CDeviceInfo* Device, wchar_t* folder) {
	return theApp.BurnFolder(hwnd, Device, folder);
}

void Stop() {
	theApp.Stop();
}

void Release() {
	theApp.Release();
}

bool IMAPI2Uninstalled() {
	return theApp.IMAPI2Uninstalled();
}

void SetVolume(wchar_t* volume) {
	theApp.SetVolume(volume);
}
void SetEjectWhenFinished(bool eject) {
	theApp.m_ejectWhenFinished = eject;
}



//////////////////////////////////////////////////////////
//导出函数
CBurnInterface BurnInterface;
CBurnInterface* GetBurnInterface() {
	BurnInterface.GetDevices = GetDevices;
	BurnInterface.InitializeDisc = InitializeDisc;
	BurnInterface.BurnISO = BurnISO;
	BurnInterface.BurnFolder = BurnFolder;
	BurnInterface.Stop = Stop;
	BurnInterface.Release = Release;
	BurnInterface.IMAPI2Uninstalled = IMAPI2Uninstalled;
	BurnInterface.SetVolume = SetVolume;
	BurnInterface.SetEjectWhenFinished = SetEjectWhenFinished;

	return &BurnInterface;
}
