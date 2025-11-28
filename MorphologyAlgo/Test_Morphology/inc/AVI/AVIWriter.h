#pragma once
#include "vfw.h"
#define PAL_STANDARD   1
#define SECAM_STANDARD 2
class CAviWriter
{
public:
	CAviWriter();

	~CAviWriter();

	BOOL OpenFile(LPCTSTR lpszFileName, int nYUY2Width, int nYUY2Height);

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