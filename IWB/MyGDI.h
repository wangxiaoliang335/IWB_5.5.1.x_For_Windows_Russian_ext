#pragma once
#include "assert.h"
//@功能:更改位图中像素值等于某一指定值的像素为指定的新值
//@参数:hBmp, 位图句柄,
//            The bitmap identified by the hbmp parameter must not be selected into a device context when the application calls this function
//      clrOld, 原像素值
//      clrNew, 新像素值
//      deltaR, R分量素允许偏离范围
//      deltaG, G分量素允许偏离范围
//      deltaB, B分量素允许偏离范围
inline BOOL ChangeBmpPixelColor(HBITMAP hBmp, COLORREF clrOld, COLORREF clrNew, BYTE deltaR=0, BYTE deltaG=0, BYTE deltaB=0)
{
	HWND hwndDesktop = GetDesktopWindow(); 
	HDC hdcDesktop   = GetDC(hwndDesktop); 

	BITMAPINFO  bitmapInfo;
	memset(&bitmapInfo, 0, sizeof(bitmapInfo));
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER );
	BOOL bRet = FALSE;

	BYTE* lpBits  = NULL;
	BITMAPINFO* lpBitmapInfo = NULL;
	int nRet = 0;
	do
	{

		 nRet = 
			GetDIBits(
			hdcDesktop,//handle to the device context
			hBmp      ,//Handle to the bitmap. This must be a compatible bitmap(DDB)
			0         ,//Specifies the first scan line to retrieve
			0         ,// Specifies the number of scan lines to retrieve
			NULL      ,//Pointer to a buffer to receive the bitmap data.
			&bitmapInfo,//
			DIB_RGB_COLORS);

		if(nRet == 0)
		{
			break;
		}



		BITMAPINFOHEADER& bitmapInfoHeader = bitmapInfo.bmiHeader;

		//只对像素位数>=24的位图进行操作
		if(bitmapInfoHeader.biBitCount < 24)
		{
			break;
		}

		
		BYTE bitmapInfoEx[sizeof(BITMAPINFO) + 2*sizeof(DWORD)];
		if(bitmapInfo.bmiHeader.biCompression == BI_BITFIELDS)
		{
			//Specifies that the bitmap is not compressed and that the color table consists of three DWORD color masks that specify the red, 
			//green, and blue components, respectively, of each pixel. This is valid when used with 16- and 32-bpp bitmaps.
			
			lpBitmapInfo = (BITMAPINFO*)&bitmapInfoEx;
			memcpy(lpBitmapInfo, &bitmapInfo, sizeof(BITMAPINFO));
		}
		else if(bitmapInfo.bmiHeader.biCompression == BI_RGB)
		{
			lpBitmapInfo = &bitmapInfo;
		}


		LONG lLineStride   =  (bitmapInfoHeader.biBitCount >> 3) * bitmapInfoHeader.biWidth;
		int nPaddingLength = (lLineStride%4)? 4-lLineStride%4: 0;
		lLineStride += nPaddingLength;


		LONG lImageSize = lLineStride * bitmapInfoHeader.biHeight;

		lpBits = new BYTE[lImageSize];


		

		
		nRet = 
			GetDIBits(
			hdcDesktop                ,//handle to the device context
			hBmp                      ,//Handle to the bitmap. This must be a compatible bitmap(DDB)
			0                         ,//Specifies the first scan line to retrieve
			bitmapInfoHeader.biHeight ,// Specifies the number of scan lines to retrieve
			lpBits                    ,//Pointer to a buffer to receive the bitmap data.
			lpBitmapInfo             ,//
			DIB_RGB_COLORS);

		if(nRet == 0 )
		{

			break;
		}


		assert(nRet == bitmapInfoHeader.biHeight);

		BYTE cMatchRMax = 0;
		BYTE cMatchRMin = 0;

		if( short(GetRValue(clrOld)) + short(deltaR) > 255)
		{
			cMatchRMax = 255;
		}
		else
		{
			cMatchRMax = GetRValue(clrOld) + deltaR;
		}

		if((short)GetRValue(clrOld) - short(deltaR) < 0)
		{
			cMatchRMin = 0;
		}
		else
		{
			cMatchRMin = GetRValue(clrOld) - deltaR;
		}

		BYTE cMatchGMax = 0;
		BYTE cMatchGMin = 0;
		if( short(GetGValue(clrOld)) + short(deltaG) > 255)
		{
			cMatchGMax = 255;
		}
		else
		{
			cMatchGMax = GetGValue(clrOld) + deltaG;
		}

		if(short(GetGValue(clrOld)) - short(deltaG) < 0)
		{
			cMatchGMin = 0;
		}
		else
		{
			cMatchGMin = GetGValue(clrOld) - deltaG;
		}


		BYTE cMatchBMax = 0;
		BYTE cMatchBMin = 0;
		if( short(GetBValue(clrOld)) + short(deltaB) > 255)
		{
			cMatchBMax = 255;
		}
		else
		{
			cMatchBMax = GetBValue(clrOld) + deltaB;
		}

		if(short(GetBValue(clrOld)) - short(deltaB) < 0)
		{
			cMatchBMin = 0;
		}
		else
		{
			cMatchBMin = GetBValue(clrOld) - deltaB;
		}

		BYTE* pRowData = lpBits;
		BYTE bytesPerPixel = bitmapInfoHeader.biBitCount >> 3;
		for(long row = 0; row < bitmapInfoHeader.biHeight; row++)
		{
			BYTE* pColData = pRowData;
			for(long col = 0; col < bitmapInfoHeader.biWidth; col++)
			{
				BYTE B = pColData[0];
				BYTE G = pColData[1];
				BYTE R = pColData[2];

				if(  cMatchRMin <=R &&  R<=cMatchRMax
					&&
					cMatchGMin <=G &&  G<=cMatchGMax
					&&
					cMatchBMin <=B &&  B<=cMatchBMax)
				{

					pColData[0] = GetBValue(clrNew);
					pColData[1] = GetGValue(clrNew);
					pColData[2] = GetRValue(clrNew);

				}

				pColData += bytesPerPixel;

			}//for-each col

			pRowData += lLineStride;

		}//for-each row



		nRet = SetDIBits(
			hdcDesktop                ,//handle to the device context
			hBmp                      ,//Handle to the bitmap. This must be a compatible bitmap(DDB)
			0                         ,//Specifies the first scan line to retrieve
			bitmapInfoHeader.biHeight ,// Specifies the number of scan lines to retrieve
			lpBits                    ,//Pointer to a buffer to receive the bitmap data.
			lpBitmapInfo               ,//
			DIB_RGB_COLORS);
		

	}while(0);

	if(lpBits) delete[]lpBits;
	ReleaseDC(hwndDesktop, hdcDesktop);

	return bRet;

}