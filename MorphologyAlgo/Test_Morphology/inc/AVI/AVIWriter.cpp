#include "stdafx.h"

#include "AVIWriter.h"
#define VIDEO_STREAM mmioFOURCC('v','i','d','s')
#define YUY2 mmioFOURCC('Y','U','Y','2')
#define MJPG mmioFOURCC('M','J','P','G')

CAviWriter::CAviWriter()
:
m_pAVIFile(NULL),
m_pAVIStream(NULL),
m_iIndex(0)
{
	AVIFileInit();
}

CAviWriter::~CAviWriter()
{
	AVIFileExit();
}


BOOL CAviWriter::OpenFile(LPCTSTR lpszFileName, int nYUY2Width, int nYUY2Height)
{
	HRESULT ret;
	int iStandard = PAL_STANDARD;

	ret = AVIFileOpen(&m_pAVIFile, lpszFileName, OF_CREATE | OF_WRITE | OF_SHARE_DENY_NONE, NULL);

	if(ret != AVIERR_OK)
	{
		return FALSE;
	}

	BITMAPINFOHEADER YUVFmt;

	memset(&YUVFmt, 0, sizeof(BITMAPINFOHEADER));
	YUVFmt.biSize        = sizeof( BITMAPINFOHEADER);
	YUVFmt.biWidth       = nYUY2Width;
	YUVFmt.biHeight      = nYUY2Height;
	YUVFmt.biPlanes      = 1;
	YUVFmt.biBitCount    = 16;
	YUVFmt.biCompression = YUY2;

	AVISTREAMINFO sivideo;

	// Set the AVI stream information
	sivideo.fccType    = VIDEO_STREAM;
	sivideo.fccHandler = YUY2;
	sivideo.dwFlags = 0;
	sivideo.dwCaps = 0;
	sivideo.wPriority = 0;
	sivideo.wLanguage = 0;
	if ((iStandard == PAL_STANDARD) ||
	(iStandard == SECAM_STANDARD))
	{
	// PAL/SECAM 25 fps
	sivideo.dwScale = 1; 
	sivideo.dwRate = 25;
	}
	else
	{
	// NTSC 29.97 fps
	sivideo.dwScale = 1001; 
	sivideo.dwRate = 30000;
	}
	sivideo.dwStart = 0;
	sivideo.dwLength = 0;
	sivideo.dwInitialFrames = 0;
	sivideo.dwSuggestedBufferSize = 0;
	sivideo.dwQuality = (DWORD)-1;
	sivideo.dwSampleSize = 0;
	sivideo.rcFrame.left = 0;
	sivideo.rcFrame.top = 0;
	sivideo.rcFrame.right = 0;
	sivideo.rcFrame.bottom = 0;
	sivideo.dwEditCount = 0;
	sivideo.dwFormatChangeCount = 0;
	sivideo.szName[0] = 0;


	// Create the AVI stream
	ret = AVIFileCreateStream(m_pAVIFile, &m_pAVIStream, &sivideo);

	if(ret != AVIERR_OK)
	{
		return FALSE;
	}
	

	// Set the MJPEG format in the AVI stream
	ret = AVIStreamSetFormat(m_pAVIStream, 0, &YUVFmt, YUVFmt.biSize);


	if(ret != AVIERR_OK)
	{
		return FALSE;
	}
	

	m_iIndex = AVIStreamLength(m_pAVIStream);


	return TRUE;

}




BOOL CAviWriter::Close()
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


BOOL CAviWriter::Write(const BYTE* pData, LONG cbBuffer)
{
	if(!m_pAVIStream)
	{
		return FALSE;
	}
	HRESULT hr = 
	// Save this compressed frame in the AVI stream
	AVIStreamWrite(m_pAVIStream, 
		m_iIndex++, 
		1, 
		(LPVOID)pData, 
		cbBuffer, 
		AVIIF_KEYFRAME,
		NULL, 
		NULL); 

	if(hr  != AVIERR_OK)
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CAviWriter::IsOpen()const
{
	return m_pAVIStream != NULL;
}
