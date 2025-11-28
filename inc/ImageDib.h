#pragma once
template<class T>
BOOL SaveToBitmap(const CImageFrameT<T>& frame, LPCTSTR lpszPath)
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
inline BOOL SaveToBitmap(const CImageFrameT<BYTE>& frame, LPCTSTR lpszPath)
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


inline BOOL SaveBitFrame(const CBitFrame& bitFrame, LPCTSTR lpszPath)
{
	CDib dib;
	int nImageWidth  = (int)bitFrame.Width();
	int nImageHeight = (int)bitFrame.Height();

	dib.Create(nImageWidth, nImageHeight);

	int nLineStride = dib.GetLineStride();
	
	BYTE* pDibCursor = dib.GetBits() + (nImageHeight - 1)*nLineStride;
	const DWORD* pImageDataCursor = bitFrame.GetData();


	DWORD dwData = *pImageDataCursor++;
	for(int row = 0; row < nImageHeight; row++)
	{
		pDibCursor = dib.GetBits() + (nImageHeight - 1 - row)*nLineStride;
		int shift = 0;
		for(int col= 0; col < nImageWidth; col++)
		{
			BYTE val;
			if(dwData & 0x00000001)
			{
				val = 0xFF;
			}
			else
			{
				val = 0x00;
			}

			shift ++;
			if(32 == shift)
			{
				dwData = *pImageDataCursor++;
				shift  = 0;
			}
			else
			{
				dwData >>= 1;
			}

			
			
			pDibCursor[0] = val;
			pDibCursor[1] = val;
			pDibCursor[2] = val;

			pDibCursor += 3;


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

