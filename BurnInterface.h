#ifndef BURN_INTERFACE_H
#define BURN_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#define FREE_ARRARY(str) \
	if (str) {	\
		delete []str;	\
		str = NULL;		\
	}

#define FREE_OBJECT(obj) \
	if (obj) {	\
		delete obj;	\
		obj = NULL;	\
	}

struct CTexts {
	long total;
	wchar_t** text;

	CTexts(int size) {
		total = size;
		text = new wchar_t *[size];
		memset(text, 0, sizeof(wchar_t*)*size);
	}
	~CTexts() {
		for (int i=0; i<total; i++ ) {
			FREE_ARRARY(text[i]);
		}
		FREE_ARRARY(text);
		total = 0;
	}
};


struct CDiscInfo {
	//void* discRecorder;
	bool isCdromSupported;
	bool isDvdSupported;
	bool isDualLayerDvdSupported;
	CTexts* mediaType;

	bool isWriterable;
	bool isBlank;
	bool isInitialized;
	bool isSupported;
	CTexts* mediaStatus;

	wchar_t* physicalType;
	unsigned long totalSpace;
	unsigned long freeSpace;

	CTexts* writeSpeed;

	CDiscInfo() {
		//discRecorder = NULL;
		isCdromSupported = false;
		isDvdSupported = false;
		isDualLayerDvdSupported = false;
		mediaType = NULL;

		isWriterable = false;
		isBlank = false;
		isInitialized = false;
		isSupported = true;		//默认支持
		mediaStatus = NULL;
		
		totalSpace = 0;
		physicalType = NULL;
		writeSpeed = NULL;
	}
	~CDiscInfo() {
		FREE_OBJECT(mediaType);
		FREE_OBJECT(mediaStatus);
		FREE_OBJECT(writeSpeed);
		FREE_ARRARY(physicalType);
	}
};

struct CDeviceInfo {
	wchar_t* recorderUniqueID;
	wchar_t* volumePath;
	wchar_t* productId;
	wchar_t* vendorId;
	wchar_t* productRevision;
	void* discRecorder;
	CDiscInfo* discInfo;

	CDeviceInfo() {
		recorderUniqueID = NULL;
		volumePath = NULL;
		productId = NULL;
		vendorId = NULL;
		productRevision = NULL;
		discInfo = NULL;
		discRecorder = NULL;
	}
	~CDeviceInfo() {
		FREE_ARRARY(recorderUniqueID);
		FREE_ARRARY(volumePath);
		FREE_ARRARY(productId);
		FREE_ARRARY(vendorId);
		FREE_ARRARY(productRevision);

		FREE_OBJECT(discInfo);
	}
};

struct CDevices {
	long totalDevice;
	CDeviceInfo** Devices;

	CDevices() {
		totalDevice = 0;
		Devices = NULL;
	}
	~CDevices() {
		for (int i=0; i<totalDevice; i++) {
			if (Devices[i] != NULL)
				FREE_OBJECT(Devices[i]);
		}
		FREE_ARRARY(Devices);
	}
};

//刻录消息
#define WM_BURN_STATUS_MESSAGE	WM_APP+300
#define WM_BURN_FINISHED		WM_APP+301
#define WM_IMAPI_UPDATE			WM_APP+842

typedef struct _IMAPI_STATUS {
	// IDiscFormat2DataEventArgs Interface
    LONG elapsedTime;		// Elapsed time in seconds
    LONG remainingTime;		// Remaining time in seconds
    LONG totalTime;			// total estimated time in seconds
	// IWriteEngine2EventArgs Interface
    LONG startLba;			// the starting lba of the current operation
    LONG sectorCount;		// the total sectors to write in the current operation
    LONG lastReadLba;		// the last read lba address
    LONG lastWrittenLba;	// the last written lba address
    LONG totalSystemBuffer;	// total size of the system buffer
    LONG usedSystemBuffer;	// size of used system buffer
    LONG freeSystemBuffer;	// size of the free system buffer
} IMAPI_STATUS, *PIMAPI_STATUS;

#define RETURN_CANCEL_WRITE	0
#define RETURN_CONTINUE		1


//获取设备列表
typedef CDevices* (*FGetDevices)(); 
typedef bool (*FInitializeDisc)(CDeviceInfo* Device);
typedef bool (*FBurnISO)(HWND hwnd, CDeviceInfo* Device, wchar_t* isoFile);
typedef bool (*FBurnFolder)(HWND hwnd, CDeviceInfo* Device, wchar_t* folder);
typedef void (*FStop)();
typedef void (*FRelease)();
typedef bool (*FIMAPI2Uninstalled)();
typedef void (*FSetVolume)(wchar_t* volume);
typedef void (*FSetEjectWhenFinished)(bool eject);


struct CBurnInterface {
	FGetDevices GetDevices;
	FInitializeDisc InitializeDisc;
	FBurnISO BurnISO;
	FBurnFolder BurnFolder;
	FStop Stop;
	FRelease Release;
	FIMAPI2Uninstalled IMAPI2Uninstalled;
	FSetVolume SetVolume;
	FSetEjectWhenFinished SetEjectWhenFinished;

	CBurnInterface() {
		GetDevices = NULL;
		InitializeDisc = NULL;
		BurnISO = NULL;
		BurnFolder = NULL;
		Stop = NULL;
		Release = NULL;
		IMAPI2Uninstalled = NULL;
		SetVolume = NULL;
		SetEjectWhenFinished = NULL;
	}
};





#ifdef __cplusplus
}
#endif

#endif