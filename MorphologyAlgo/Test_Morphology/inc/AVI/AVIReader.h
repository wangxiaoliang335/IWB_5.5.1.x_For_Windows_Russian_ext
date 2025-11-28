#pragma once
#include "vfw.h"
#define PAL_STANDARD   1
#define SECAM_STANDARD 2
class CAviReader
{
public:
	CAviReader();

	~CAviReader();

	BOOL OpenFile(LPCTSTR lpszFileName);

	BOOL Close();

	BOOL Read(BYTE* pData, LONG cbBuffer, LONG* plBytes, LONG* plSamples);

	BOOL IsOpen()const;

	const BITMAPINFOHEADER* GetAVIFormat();
protected:
	PAVIFILE         m_pAVIFile;
	PAVISTREAM       m_pAVIStream;
	int              m_iIndex;
	BITMAPINFOHEADER m_format;


	//MJPG Compressor Handle
	//HIC      m_hCompressor;
};