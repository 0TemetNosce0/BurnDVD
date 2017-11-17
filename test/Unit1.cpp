//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "DllInterface.h"
#include "../BurnInterface.h"

#include <imapi2.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

typedef CBurnInterface* (*FGetBurnInterface)();
typedef void (*FDebugLog)(const wchar_t* log);


typedef void (*FSetDebugLogFunction)(FDebugLog DebugLog);

TForm1 *Form1;
TDllInterface *DllInterface = NULL;
CBurnInterface *BurnInterface = NULL;
FGetBurnInterface GetBurnInterface = NULL;
FSetDebugLogFunction SetDebugLogFunction = NULL;

void DebugLog(const wchar_t* log) {
    Form1->Memo1->Lines->Add(log);
}
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	DllInterface = new TDllInterface();
	if (!DllInterface->LoadDll(L"BurnDVD.dll")) {
		ShowMessage(L"加载动态库失败");
	}

	GetBurnInterface = DllInterface->GetFunction<FGetBurnInterface>("GetBurnInterface");
	if (GetBurnInterface) {
		BurnInterface = GetBurnInterface();
	}
	SetDebugLogFunction = DllInterface->GetFunction<FSetDebugLogFunction>("SetDebugLogFunction");
	if (SetDebugLogFunction) {
		SetDebugLogFunction(DebugLog);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
	//光驱列表
	ListBox1->Clear();
	if (BurnInterface && BurnInterface->GetDevices) {
		CDevices* device = BurnInterface->GetDevices();
//		if (device == NULL) {
//			String msg;
//			ShowMessage(msg.sprintf(L"Unable to get the list of devices. 0x%08x", device));
//			return;
//		}

		if (device != NULL) {
			for (long i=0; i < device->totalDevice; i++) {
				String item;
				if (device->Devices[i]) {
					item.sprintf(L"%s [%s %s %s]",
						device->Devices[i]->volumePath,
						device->Devices[i]->vendorId,
						device->Devices[i]->productId,
						device->Devices[i]->productRevision);
				}
				else {
                    item = L"unknown";
                }

				ListBox1->Items->Add(item);
			}
		}
		else {
			String msg;
			if (BurnInterface->IMAPI2Uninstalled()) {
				msg.sprintf(L"IMAPIv2 libraries not installed properly.");
			}
			else {
				msg.sprintf(L"Unable to get the list of devices. 0x%08x", device);
			}
			ShowMessage(msg);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ListBox1Click(TObject *Sender)
{
	Label1->Caption = L"";
	Label2->Caption = L"";
	Label8->Caption = L"";
	Label10->Caption = L"";
	Label11->Caption = L"";

	CheckBox1->Checked = false;
	CheckBox2->Checked = false;
	CheckBox3->Checked = false;
	CheckBox4->Checked = false;
	CheckBox5->Checked = false;
	CheckBox6->Checked = false;
	CheckBox7->Checked = false;

	ComboBox1->Clear();

	//驱动器详细信息
	if (BurnInterface == NULL || BurnInterface->GetDevices == NULL) {
		return;
	}
	CDevices* device = BurnInterface->GetDevices();
	if (device == NULL) {
		return;
	}
	if (device->totalDevice <= ListBox1->ItemIndex
		&& ListBox1->ItemIndex >= 0) {
		return;
	}

	CDeviceInfo* info = device->Devices[ListBox1->ItemIndex];
	if (info == NULL || info->discInfo == NULL
		|| info->discInfo->mediaType == NULL) {
		return;
	}

	CTexts* mediaType = info->discInfo->mediaType;
	String temp;
	for (long i=0; i < mediaType->total; i++) {
		if (i > 0) {
			temp += L",";
		}
		temp += mediaType->text[i];
	}
	Label1->Caption = temp;

	CTexts* mediaStatus = info->discInfo->mediaStatus;
	temp = L"";
	for (long i=0; i < mediaStatus->total; i++) {
		if (i > 0) {
			temp += L"\n";
		}
		temp += mediaStatus->text[i];
	}
	Label2->Caption = temp;

	CTexts* writeSpeed = info->discInfo->writeSpeed;
	for (long i=0; i < writeSpeed->total; i++) {
		ComboBox1->Items->Add(writeSpeed->text[i]);
	}
	if (writeSpeed > 0) {
		ComboBox1->ItemIndex = 0;
	}

	Label8->Caption = temp.sprintf(L"Total : %d MB", info->discInfo->totalSpace);
	Label10->Caption = temp.sprintf(L"Free : %d MB", info->discInfo->freeSpace);
	Label11->Caption = info->discInfo->physicalType;

	CheckBox1->Checked = info->discInfo->isCdromSupported;
	CheckBox2->Checked = info->discInfo->isDvdSupported;
	CheckBox3->Checked = info->discInfo->isDualLayerDvdSupported;
	CheckBox4->Checked = info->discInfo->isWriterable;
	CheckBox5->Checked = info->discInfo->isBlank;
	CheckBox6->Checked = info->discInfo->isInitialized;
	CheckBox7->Checked = info->discInfo->isSupported;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
	if (BurnInterface == NULL || BurnInterface->BurnISO == NULL) {
		return;
	}
	if (BurnInterface->GetDevices == NULL) {
		return;
	}
	CDevices* device = BurnInterface->GetDevices();
	if (device == NULL) {
		return;
	}
	if (device->totalDevice <= ListBox1->ItemIndex
		&& ListBox1->ItemIndex >= 0) {
		return;
	}

	CDeviceInfo* info = device->Devices[ListBox1->ItemIndex];
	Stop = false;
	BurnInterface->BurnISO(Handle, info, Edit1->Text.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
	if (BurnInterface == NULL || BurnInterface->Release == NULL) {
		return;
	}
	BurnInterface->Release();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::WndProc(Messages::TMessage &Message) {
	TForm::WndProc(Message);

	if (WM_IMAPI_UPDATE == Message.Msg) {
		OnImapiUpdate(Message);
	}
	else if (WM_BURN_FINISHED == Message.Msg) {
		if (Message.WParam != 0) {
			String string;
			string.sprintf(L"%s Error 0x%08x", (wchar_t*)Message.LParam, Message.WParam);
			ShowMessage(string);
		}
		else {
			ShowMessage(L"Completed!");
		}
	}
	else if (WM_BURN_STATUS_MESSAGE == Message.Msg) {
    	Label9->Caption = (wchar_t*)(Message.LParam);
	}
}

//---------------------------------------------------------------------------
void __fastcall TForm1::OnImapiUpdate(Messages::TMessage &Message){
	IMAPI_FORMAT2_DATA_WRITE_ACTION currentAction = (IMAPI_FORMAT2_DATA_WRITE_ACTION)Message.WParam;
	PIMAPI_STATUS pImapiStatus = (PIMAPI_STATUS)Message.LParam;

	switch (currentAction)
	{
	case IMAPI_FORMAT2_DATA_WRITE_ACTION_VALIDATING_MEDIA:
		Label3->Caption = L"Validating current media...";
		break;

	case IMAPI_FORMAT2_DATA_WRITE_ACTION_FORMATTING_MEDIA:
		Label3->Caption = L"Formatting media...";
		break;

	case IMAPI_FORMAT2_DATA_WRITE_ACTION_INITIALIZING_HARDWARE:
		Label3->Caption = L"Initializing hardware...";
		break;

	case IMAPI_FORMAT2_DATA_WRITE_ACTION_CALIBRATING_POWER:
		Label3->Caption = L"Optimizing laser intensity...";
		break;

	case IMAPI_FORMAT2_DATA_WRITE_ACTION_WRITING_DATA:
		UpdateTimes(pImapiStatus->totalTime, pImapiStatus->remainingTime);
		UpdateBuffer(pImapiStatus->usedSystemBuffer, pImapiStatus->totalSystemBuffer);
		UpdateProgress(pImapiStatus->lastWrittenLba-pImapiStatus->startLba, pImapiStatus->sectorCount);
		break;

	case IMAPI_FORMAT2_DATA_WRITE_ACTION_FINALIZATION:
		Label3->Caption = L"Finalizing writing...";
		break;

	case IMAPI_FORMAT2_DATA_WRITE_ACTION_COMPLETED:
		Label3->Caption = L"Completed!";
		break;
	}

	Message.Result = Stop ? RETURN_CANCEL_WRITE : RETURN_CONTINUE;
}

void TForm1::UpdateTimes(LONG totalTime, LONG remainingTime) {
	String strText;
	if (totalTime > 0) {
		strText.sprintf(L"%d:%02d", totalTime / 60, totalTime % 60);
	}
	else {
		strText = L"0:00";
	}
	Label4->Caption = strText;

	if (remainingTime > 0) {
		strText.sprintf(L"%d:%02d", remainingTime / 60, remainingTime % 60);
	}
	else {
		strText = L"0:00";
	}
	Label5->Caption = strText;
}
void TForm1::UpdateBuffer(LONG usedSystemBuffer, LONG totalSystemBuffer) {
	String text;
	if (usedSystemBuffer && totalSystemBuffer) {
		text.sprintf(L"Buffer: %d%%", (100*usedSystemBuffer) / totalSystemBuffer);
	}
	else {
		text = L"Buffer Empty";
	}
	Label6->Caption = text;
}
void TForm1::UpdateProgress(LONG writtenSectors, LONG totalSectors) {
	static LONG prevTotalSector = 0;
	String text;

	if (totalSectors && (totalSectors != prevTotalSector)) {
		prevTotalSector = totalSectors;
	}

	if (writtenSectors && totalSectors) {
		text.sprintf(L"Progress: %d%%", (100*writtenSectors) / totalSectors);
	}
	else {
        text = L"Progress";
	}
	Label7->Caption = text;
}

void __fastcall TForm1::Button3Click(TObject *Sender)
{
	Stop = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
	//刻录目录
	if (BurnInterface == NULL || BurnInterface->BurnFolder == NULL) {
		return;
	}
	if (BurnInterface->GetDevices == NULL) {
		return;
	}
	CDevices* device = BurnInterface->GetDevices();
	if (device == NULL) {
		return;
	}
	if (device->totalDevice <= ListBox1->ItemIndex
		&& ListBox1->ItemIndex >= 0) {
		return;
	}

	BurnInterface->SetVolume(L"1234");
	BurnInterface->SetEjectWhenFinished(true);

	CDeviceInfo* info = device->Devices[ListBox1->ItemIndex];
	Stop = false;
	BurnInterface->BurnFolder(Handle, info, Edit1->Text.c_str());
}
//---------------------------------------------------------------------------

