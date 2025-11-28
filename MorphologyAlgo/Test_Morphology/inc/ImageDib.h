#pragma once

#include "MyAPI.h"
#include "../inc/DIB/Dib.h"
#include "../inc/ImageFrame.h"
template<class T>
BOOL SaveToDib(const CImageFrameT<T>& frame, LPCTSTR lpszPath)
{
	CDib dib;
	int nImageWidth  = (int)frame.Width();
	int nImageHeight = (int)frame.Height();

	dib.Create(nImageWidth, nImageHeight);

	int nLineStride = dib.GetLineStride();
	
	BYTE* pDibCursor = dib.GetBits() + (nImageHeight - 1)*nLineStride;
	const T* pImageDataCursor = frame.GetData();
	T max = frame.Max();
	T min = frame.Min();
	T range = max - min;
    BOOL bIsConstant = (max-min == 0);
	for(int row = 0; row < nImageHeight; row++)
	{
		pDibCursor = dib.GetBits() + (nImageHeight - 1 - row)*nLineStride;
		for(int col= 0; col < nImageWidth; col++)
		{
			BYTE val;
			if(bIsConstant)
			{
				val = BYTE( 255u * max/(std::numeric_limits<T>::max)());
			}
			else
			{
				val = BYTE( 255u *(pImageDataCursor[0] - min) / range);
			}
			
			pDibCursor[0] = val;
			pDibCursor[1] = val;
			pDibCursor[2] = val;

			pDibCursor += 3;
			pImageDataCursor ++;

		}		

	}

	return dib.Save(lpszPath);

}


template<>
inline BOOL SaveToDib(const CImageFrameT<BYTE>& frame, LPCTSTR lpszPath)
{
	CDib dib;
	int nImageWidth  = (int)frame.Width();
	int nImageHeight = (int)frame.Height();

	dib.Create(nImageWidth, nImageHeight);

	int nLineStride = dib.GetLineStride();
	
	BYTE* pDibCursor = dib.GetBits() + (nImageHeight - 1)*nLineStride;
	const BYTE* pImageDataCursor = frame.GetData();


	for(int row = 0; row < nImageHeight; row++)
	{
		pDibCursor = dib.GetBits() + (nImageHeight - 1 - row)*nLineStride;
		for(int col= 0; col < nImageWidth; col++)
		{
			BYTE val;

			val = pImageDataCursor[0];	
			
			pDibCursor[0] = val;
			pDibCursor[1] = val;
			pDibCursor[2] = val;

			pDibCursor += 3;
			pImageDataCursor ++;

		}		

	}

	return dib.Save(lpszPath);

}











template<class T>
BOOL LoadToDib(const CImageFrameT<T>& frame, CDib& dib)
{
	int nImageWidth  = (UINT)frame.Width();
	int nImageHeight = (UINT)frame.Height();

	dib.Create(nImageWidth, nImageHeight);

	int nLineStride = dib.GetLineStride();
	
	BYTE* pDibCursor = dib.GetBits() + (nImageHeight - 1)*nLineStride;
	const T* pImageDataCursor = frame.GetData();
	T max = frame.Max();
	T min = frame.Min();
	T range = max - min;
    BOOL bIsConstant = (max-min == 0);
	for(int row = 0; row < nImageHeight; row++)
	{
		pDibCursor = dib.GetBits() + (nImageHeight - 1 - row)*nLineStride;
		for(int col= 0; col < nImageWidth; col++)
		{
			BYTE val;
			if(bIsConstant)
			{
				val = 255;
			}
			else
			{
				val = BYTE( T(255) *(pImageDataCursor[0] - min) / range);
			}
			
			pDibCursor[0] = val;
			pDibCursor[1] = val;
			pDibCursor[2] = val;

			pDibCursor += 3;
			pImageDataCursor ++;

		}		

	}

	return TRUE;
}


//
_declspec(selectany) CAtlString g_strAutoCalibrateDir = _T("..\\AutoCalibrate\\");
_declspec(selectany) int g_nImageIndex = 1;
inline void InitAutoCalibrateSaveDirectory()
{
	SYSTEMTIME now;
	GetLocalTime(&now);
	g_strAutoCalibrateDir.Format(
	_T(".\\AutoCalibrate\\%04d_%02d_%02d_%02d_%02d_%02d\\"),
	now.wYear,
	now.wMonth,
	now.wDay,
	now.wHour,
	now.wMinute,
	now.wSecond);


	//生成完整目录目录
	CreateFullDirectory(g_strAutoCalibrateDir);


}

template<class T>
BOOL Debug_SaveImageFrame(const CImageFrameT<T>& frame, LPCTSTR lpszFileName)
{
	
	CAtlString strFullPath;
	CAtlString strImageFileName;
	strImageFileName.Format(_T("%02d_%s"), g_nImageIndex, lpszFileName);

	strFullPath =  g_strAutoCalibrateDir;
	strFullPath += strImageFileName;

	g_nImageIndex++;

	return  SaveToDib(frame, strFullPath);
};


inline BOOL Debug_SaveDib(CDib& dib, LPCTSTR lpszFileName)
{
	CAtlString strFullPath;
	CAtlString strImageFileName;
	strImageFileName.Format(_T("%02d_%s"), g_nImageIndex, lpszFileName);

	strFullPath =  g_strAutoCalibrateDir;
	strFullPath += strImageFileName;

	g_nImageIndex++;

	return dib.Save(strFullPath);

}


//@功能:叠加图片
//@参数:bottomImg,[输入/输出], 底层图片, 保存输出图片的对象
//      topImg, [输入], 顶层图片
template<class T>
BOOL OverlapImage(CImageFrameT<T>& bottomImg,  const CImageFrameT<T>& topImg)
{
	T* pBottomImgData = bottomImg.GetData();
	const T* pTopImgData = topImg.GetData();
	int nWidth  = bottomImg.Width();
	int nHeight = bottomImg.Height(); 

	if(nWidth != topImg.Width() || nHeight != topImg.Height())
	{
		return FALSE;
	}

	for(int nRow = 0; nRow < nHeight; nRow++)
	{
		for(int nCol = 0; nCol < nWidth; nCol++)
		{
			*pBottomImgData = *pBottomImgData | *pTopImgData;

			pBottomImgData ++;
			pTopImgData ++;



		}
	}



	return TRUE;
}


//@功能:将24位位图转化为灰度图片
//@参数:dib, DIB对象
//      image, 位图帧
template<class T>
BOOL Dib2Image(const CDib& dib, CImageFrameT<T>& image)
{

	const BYTE* pDibData = dib.GetBits();
	int nWidth  = dib.GetWidth();
	int nHeight = dib.GetHeight();
	int nLineStride = dib.GetLineStride();

	image.SetSize(nWidth, nHeight, sizeof(T));
	T* pImage = image.GetData();
	for(int row=0; row<nHeight;row ++)
	{
		const BYTE* pDataCursor = pDibData + (nHeight-row-1)*nLineStride;
		for(int col=0; col<nWidth; col++)	
		{
			float fY = 0.2126f*float(pDataCursor[0]) + 0.7152f*float(pDataCursor[1]) + 0.0722f*float(pDataCursor[2]);
			
			BYTE Y = (BYTE)fY;

			*pImage = (T)Y;

			pDataCursor += 3;
			pImage ++;

		}
	}


	return TRUE;
}