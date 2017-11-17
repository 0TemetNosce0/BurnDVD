// BurnDVD.h : main header file for the BurnDVD DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "BurnInterface.h"


// CBurnDVDApp
// See BurnDVD.cpp for the implementation of this class
//

class CBurnDVDApp : public CWinApp
{
public:
	CBurnDVDApp();
	~CBurnDVDApp();
	CDevices* GetDevices();
	bool InitializeDisc(CDeviceInfo* device);
	bool BurnISO(HWND hwnd, CDeviceInfo* device, wchar_t* isoFile);
	bool BurnFolder(HWND hwnd, CDeviceInfo* device, wchar_t* folder);
	void Stop();
	void Release();

	HWND GetWnd() {return hWnd;}
	CDeviceInfo* GetBurnDevice() {return Device;}
	wchar_t* GetIsoFile() {return IsoFile;}
	bool GetBurning() {return isBurning;}
	bool IMAPI2Uninstalled() {return isIMAPI2Uninstalled;}
	void SetVolume(wchar_t* volume){Volume = volume;}

	IStream* GetStream() {return Stream;}

	bool m_ejectWhenFinished;

private:
	bool InitializeDevice(CDeviceInfo* device);
	CString	GetMediaTypeString(int mediaType, CDiscInfo* discInfo);
	CString	GetPhysicalTypeString(int physicalType, CDiscInfo* discInfo);
	void GetMediaStatusString(int mediaStatus, CDiscInfo* discInfo);
	bool CreateMediaFileSystem(IMAPI_MEDIA_PHYSICAL_TYPE mediaType, IStream** ppDataStream, const CString& folder);
	CDevices* Devices;
	bool isBurning;
	bool isIMAPI2Uninstalled;
	CString Volume;
	IStream* Stream;

	HWND hWnd;
	CDeviceInfo* Device;
	wchar_t* IsoFile;
protected:
	static UINT __cdecl BurnThread(LPVOID pParam);
// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
