///////////////////////////////////////////////////////////////////////
// DiscFormatData.h
//
// Wrapper for IDiscFormat2Data Interface
//
// Written by Eric Haddan
//
#pragma once


class CDiscRecorder;

class CDiscFormatData
{
private:
	IDiscFormat2Data*	m_discFormatData;
	SAFEARRAY*			m_mediaTypesArray;
	HRESULT				m_hResult;
	CString				m_errorMessage;
	HWND				m_hNotificationWnd;
	bool				m_closeMedia;
	IMAPI_FORMAT2_DATA_MEDIA_STATE mediaState;
	ULONG				m_TotalSpace;
	ULONG				m_FreeSpace;
	IMAPI_MEDIA_PHYSICAL_TYPE m_PhysicalType;
	SAFEARRAY*			m_writeSpeedsArray;

public:
	CDiscFormatData(void);
	~CDiscFormatData(void);

	inline	HRESULT GetHresult() {return m_hResult;}
	inline	CString GetErrorMessage() {return m_errorMessage;}

	inline	IDiscFormat2Data* GetInterface() {return m_discFormatData;}
	bool	Initialize(CDiscRecorder* pDiscRecorder, const CString& clientName);

	bool	Burn(HWND hWnd, IStream* data);

	ULONG	GetTotalSupportedMediaTypes();
	int		GetSupportedMediaType(ULONG index);

	IMAPI_FORMAT2_DATA_MEDIA_STATE GetMediaStatus(){return mediaState;}

	ULONG GetMediaTotalSpace() {return m_TotalSpace;}
	ULONG GetMediaFreeSpace() {return m_FreeSpace;}
	IMAPI_MEDIA_PHYSICAL_TYPE GetPhysicalType() {return m_PhysicalType;}

	ULONG	GetTotalWriteSpeeds();
	double	GetWriteSpeed(ULONG index);

	inline	void SetCloseMedia(bool closeMedia){m_closeMedia = closeMedia;}

protected:
	IStream*	m_streamData;
};
