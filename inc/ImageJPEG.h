#pragma once

template <class T>
BOOL SaveToJPEG(const CImageFrameT<T>& frame, LPCTSTR lpszPath)
{
	CDib dib;
	int nImageWidth  = (int)frame.Width();
	int nImageHeight = (int)frame.Height();

    if(nImageWidth == 0 || nImageWidth == 0 ) return FALSE;

	dib.Create(nImageWidth, nImageHeight);

	int nLineStride = dib.GetLineStride();
	
	BYTE* pDibCursor = dib.GetBits() + (nImageHeight - 1)*nLineStride;
	const T* pImageDataCursor = frame.GetData();
    
    if(pImageDataCursor == NULL) return FALSE;

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

	 return Dib2JPEG(dib, lpszPath);

}


template<>
inline BOOL SaveToJPEG(const CImageFrameT<BYTE>& frame, LPCTSTR lpszPath)
{
	CDib dib;
	int nImageWidth  = (int)frame.Width();
	int nImageHeight = (int)frame.Height();
    if(nImageWidth == 0 || nImageWidth == 0 ) return FALSE;

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

    return Dib2JPEG(dib, lpszPath);

}


inline BOOL SaveBitFrameJPEG(const CBitFrame& bitFrame, LPCTSTR lpszPath)
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

    return Dib2JPEG(dib, lpszPath);

}