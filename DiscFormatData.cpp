///////////////////////////////////////////////////////////////////////
// DiscFormatData.cpp
//
// Wrapper for IDiscFormat2Data Interface
//
// Written by Eric Haddan
//

#include "StdAfx.h"
#include "DiscFormatData.h"
#include "DiscRecorder.h"
#include "DiscFormatDataEvent.h"

CDiscFormatData::CDiscFormatData(void)
: m_discFormatData(NULL)
, m_mediaTypesArray(NULL)
, m_hResult(0)
, m_hNotificationWnd(NULL)
, m_closeMedia(true)
, mediaState(IMAPI_FORMAT2_DATA_MEDIA_STATE_UNKNOWN)
, m_TotalSpace(0)
{
}

CDiscFormatData::~CDiscFormatData(void)
{
	if (m_discFormatData != NULL)
	{
		m_discFormatData->Release();
	}
}

///////////////////////////////////////////////////////////////////////
//
// CDiscFormatData::Initialize()
//
// Description:
//		Creates and initializes the IDiscFormat2Data interface
//
bool CDiscFormatData::Initialize(CDiscRecorder* pDiscRecorder, const CString& clientName)
{
	ASSERT(m_discFormatData == NULL);
	ASSERT(pDiscRecorder != NULL);
	if (pDiscRecorder == NULL)
	{
		m_errorMessage = _T("Error - CDiscFormatData::Initialize - pDiscRecorder is NULL");
		return false;
	}

	//
	// Initialize the IDiscFormat2Data Interface
	//
	m_hResult = CoCreateInstance(__uuidof(MsftDiscFormat2Data), NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IDiscFormat2Data), (void**)&m_discFormatData);
	if (!SUCCEEDED(m_hResult))
	{
		m_errorMessage.Format(_T("Unable to Initialize IDiscFormat2Data - Error:0x%08x"), m_hResult);
		return false;
	}

	//
	// Setup the Disc Format Information
	//
	VARIANT_BOOL isSupported = VARIANT_FALSE;
	m_hResult = m_discFormatData->IsRecorderSupported(pDiscRecorder->GetInterface(), &isSupported);
	if (isSupported == VARIANT_FALSE)
	{
		m_errorMessage = _T("Recorder not supported");
		return false;
	}

	m_hResult = m_discFormatData->put_Recorder(pDiscRecorder->GetInterface());
	if (!SUCCEEDED(m_hResult))
	{
		m_errorMessage.Format(_T("IDiscFormat2Data->put_Recorder Failed - Error:0x%08x"), m_hResult);
		return false;
	}

	m_hResult = m_discFormatData->put_ClientName(clientName.AllocSysString());
	if (!SUCCEEDED(m_hResult))
	{
		m_errorMessage.Format(_T("IDiscFormat2Data->put_ClientName Failed - Error:0x%08x"), m_hResult);
		return false;
	}

	m_hResult = m_discFormatData->get_SupportedMediaTypes(&m_mediaTypesArray);
	if (!SUCCEEDED(m_hResult))
	{
		m_errorMessage.Format(_T("IDiscFormat2Data->get_SupportedMediaTypes Failed - Error:0x%08x"), m_hResult);
		return false;
	}
	m_hResult = m_discFormatData->get_CurrentMediaStatus(&mediaState);
	if (!SUCCEEDED(m_hResult) && E_IMAPI_RECORDER_MEDIA_NO_MEDIA != m_hResult)
	{
		m_errorMessage.Format(_T("IDiscFormat2Data->get_CurrentMediaStatus Failed - Error:0x%08x"), m_hResult);
		return false;
	}

	LONG totalSector;
	m_hResult = m_discFormatData->get_TotalSectorsOnMedia(&totalSector);
	if (!SUCCEEDED(m_hResult) && E_IMAPI_RECORDER_MEDIA_NO_MEDIA != m_hResult) {
		m_errorMessage.Format(_T("IDiscFormat2Data->get_TotalSectorsOnMedia Failed - Error:0x%08x\n"), m_hResult);
		return false;
	}
	if (E_IMAPI_RECORDER_MEDIA_NO_MEDIA != m_hResult) {
		m_TotalSpace = totalSector/512;
	}
	else {
		m_TotalSpace = 0;
	}

	LONG freeSector;
	m_hResult = m_discFormatData->get_FreeSectorsOnMedia(&freeSector);
	if (!SUCCEEDED(m_hResult) && E_IMAPI_RECORDER_MEDIA_NO_MEDIA != m_hResult) {
		m_errorMessage.Format(_T("IDiscFormat2Data->get_FreeSectorsOnMedia Failed - Error:0x%08x\n"), m_hResult);
		return false;
	}
	if (E_IMAPI_RECORDER_MEDIA_NO_MEDIA != m_hResult) {
		m_FreeSpace = freeSector/512;
	}
	else {
		m_FreeSpace = 0;
	}

	m_hResult = m_discFormatData->get_CurrentPhysicalMediaType(&m_PhysicalType);
	if (!SUCCEEDED(m_hResult) && E_IMAPI_RECORDER_MEDIA_NO_MEDIA != m_hResult) {
		m_errorMessage.Format(_T("IDiscFormat2Data->get_CurrentPhysicalMediaType Failed - Error:0x%08x\n"), m_hResult);
		return false;
	}

	m_hResult = m_discFormatData->get_SupportedWriteSpeeds(&m_writeSpeedsArray);
	if (!SUCCEEDED(m_hResult) && E_IMAPI_RECORDER_MEDIA_NO_MEDIA != m_hResult) {
		m_errorMessage.Format(_T("IDiscFormat2Data->get_SupportedWriteSpeeds Failed - Error:0x%08x\n"), m_hResult);
		return false;
	}
	
	return true;
}

ULONG CDiscFormatData::GetTotalSupportedMediaTypes()
{
	if (m_mediaTypesArray == NULL)
		return 0;

	return m_mediaTypesArray->rgsabound[0].cElements;
}

int CDiscFormatData::GetSupportedMediaType(ULONG index)
{
	ASSERT(index < GetTotalSupportedMediaTypes());
	if (index < GetTotalSupportedMediaTypes())
	{
		if (m_mediaTypesArray)
		{
			return ((VARIANT*)(m_mediaTypesArray->pvData))[index].intVal;
		}
	}

	return 0;
}

ULONG CDiscFormatData::GetTotalWriteSpeeds() {
	if (m_writeSpeedsArray == NULL)
		return 0;
	return m_writeSpeedsArray->rgsabound[0].cElements;
}
double CDiscFormatData::GetWriteSpeed(ULONG index) {
	ASSERT(index < GetTotalWriteSpeeds());
	if (index < GetTotalWriteSpeeds()) {
		if (m_writeSpeedsArray) {
			if (m_TotalSpace > 1000)
				return ((VARIANT*)m_writeSpeedsArray->pvData)[index].ulVal/676.0;
			else {
				return ((VARIANT*)m_writeSpeedsArray->pvData)[index].ulVal/75;
			}
		}
	}
	return 0;
}


bool CDiscFormatData::Burn(HWND hNotificationWnd, IStream* streamData)
{
	if (m_discFormatData == NULL)
		return false;

	if (hNotificationWnd == NULL)
		return false;

	if (streamData == NULL)
		return false;

	m_streamData = streamData;
	m_hNotificationWnd = hNotificationWnd;

	// Create the event sink
	CDiscFormatDataEvent* eventSink = CDiscFormatDataEvent::CreateEventSink();
	if (eventSink == NULL)
	{
		m_errorMessage = _T("Unable to create event sink");
		return false;
	}

	if (!eventSink->ConnectDiscFormatData(this))
	{
		m_errorMessage = _T("Unable to connect event sink with interface");
		return false;
	}

	eventSink->SetHwnd(m_hNotificationWnd);

	m_discFormatData->put_ForceMediaToBeClosed(m_closeMedia ? VARIANT_TRUE : VARIANT_FALSE);

	m_hResult = m_discFormatData->Write(m_streamData);

	delete eventSink;

	if (SUCCEEDED(m_hResult))
	{
		return true;
	}

	m_errorMessage.Format(_T("IDiscFormat2Data->Write Failed! Error:0x%08x"), m_hResult);

	return false;

}

