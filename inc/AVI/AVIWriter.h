#pragma once
#include "vfw.h"
#define PAL_STANDARD   1
#define SECAM_STANDARD 2
#define VIDEO_STREAM mmioFOURCC('v','i','d','s')
#define YUY2 mmioFOURCC('Y','U','Y','2')
#define MJPG mmioFOURCC('M','J','P','G')
class CAviWriter
{
public:
	CAviWriter();

	~CAviWriter();

	BOOL OpenFile(LPCTSTR lpszFileName, int nYUY2Width, int nYUY2Height, DWORD dwRate = 25, DWORD dwFourCC=YUY2);

	BOOL Close();

	BOOL Write(const BYTE* pData, LONG cbBuffer);

	BOOL IsOpen()const;
protected:
	PAVIFILE   m_pAVIFile;
	PAVISTREAM m_pAVIStream;
	int        m_iIndex;


	//MJPG Compressor Handle
	HIC      m_hCompressor;
};