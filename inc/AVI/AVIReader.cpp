#include "stdafx.h"
#include "AVIReader.h"

#define VIDEO_STREAM mmioFOURCC('v','i','d','s')
#define YUY2 mmioFOURCC('Y','U','Y','2')
#define MJPG mmioFOURCC('M','J','P','G')

CAviReader::CAviReader()
:
m_pAVIFile(NULL),
m_pAVIStream(NULL),
m_iIndex(0)
{
	AVIFileInit();
}

CAviReader::~CAviReader()
{
	AVIFileExit();
}


BOOL CAviReader::OpenFile(LPCTSTR lpszFileName)
{
	HRESULT ret;
	int iStandard = PAL_STANDARD;

	ret = AVIStreamOpenFromFile(&m_pAVIStream, lpszFileName, streamtypeVIDEO,  0, OF_READ | OF_SHARE_DENY_NONE, NULL);
	
	if(ret !=  AVIERR_OK)
	{
		if(ret == AVIERR_NODATA)
		{
			AtlTrace(_T("AVI file %s not found data.\r\n"), lpszFileName);
		}
		return FALSE;
	}


	//m_iIndex = AVIStreamLength(m_pAVIStream);


	return TRUE;

}




BOOL CAviReader::Close()
{
	if(m_pAVIStream)
	{
		AVIStreamRelease(m_pAVIStream);
		m_pAVIStream = NULL;
	}

	if(m_pAVIFile)
	{
		AVIFileRelease(m_pAVIFile);
		m_pAVIFile = NULL;
	}

	m_iIndex = 0;
	return TRUE;
}


BOOL CAviReader::Read(BYTE* pData, LONG cbBuffer, LONG* plBytes, LONG* plSamples)
{
	if(!m_pAVIStream)
	{
		return FALSE;
	}
	HRESULT hr = 
	AVIStreamRead(
	    m_pAVIStream, 
		m_iIndex++, 
		1, 
		(LPVOID)pData, 
		cbBuffer, 
		plBytes,
		plSamples
		); 

	if(hr  != AVIERR_OK)
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CAviReader::IsOpen()const
{
	return m_pAVIStream != NULL;
}


const BITMAPINFOHEADER* CAviReader::GetAVIFormat()
{
	const BITMAPINFOHEADER* pFormat = NULL;

	HRESULT ret;
	if(m_pAVIStream)
	{
		long cbFormat = sizeof(BITMAPINFOHEADER);
		ret = AVIStreamReadFormat(m_pAVIStream, 0,  &m_format, &cbFormat);
		if(ret == AVIERR_OK)
		{
			pFormat = &m_format;
		}
	}

	return pFormat;
}