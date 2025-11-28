#pragma once
#include "ImageFrame.h"

////@功 能:根据Otsu（大津展之おうつのぶゆき）算法,计算前景和背景的门限值
////@参 数:pSrcData，指向黑白像素数据的首地址
////      nDataLength,数据的长度
////@返回值:返回计算得出的门限值
////@说 明:根据类间方差最大的原理来计算
////       between-class variance:
////      σB = ω0× ω1×(μ1-μ0)^2    ---(1)
////       ω0 + ω1 = 1
////             
////       ω0 = (ΣNi)/N,  i∈[1,k]
////       μ1, 前景的均值
////       μ0, 背景的均值
////算  法:
////		公式(1)两端都乘以N^2后得到新的公式为
////      N^2 × σB  = N0 × N1 ×(μ1-μ0)^2 --(2)
////      其中 
////      N0 = (ΣNi), i∈[1,k]
////      N1= (ΣNj) = N - N0;j∈[k+1,L)
////
////      N为所有像素的个数, L代表L级灰度
////
////                  
////                    
////       μ0 = 1/N0 ×Σ(i*Ni), i∈[1,k]
////                    
////
////                         
////      令U0 = N*μ0   =  Σ(i*Ni), i∈[1,k]
////                        
////                     L
////        μ1 = 1/N1* Σ(j*Nj), j∈[k+1,L]
////                    j= k+1
////
////                        L
////      令U1 = N1*μ1  = Σ(j*Nj),j∈[k+1,L]
////                       j=k+1
////
////
////       U0 + U1 = U
////
////则(2)两端再乘以(N1×N0)^2得到
////
////   Sigma = N^2 × σB × (N0×N1)^2 =  N0 × N1 ×(μ1*N0*N1-μ0*N0*N1)^2 = N0×N1 ×(U1*N0 - U0*N1)^2 --(3)
////
////  只需找出(3)右端最大时对应的k值即为所求的门限值???
////
//inline BYTE FindThreshold_Ostu(const INT (&histogram)[256])
//{
//    int L = 256;
//
//    BYTE cThreshold = 0;
//
//    int N = 0;
//    for(int i=0; i<L; i++)
//    {
//        N += histogram[i];
//    }
//    int N0 = 0;
//	int N1 = 0;
//
//	//  k
//	int U0 = 0;//U=Σ(i*Ni)
//	//  i=1
//
//
//	//  L
//	int U1 = 0;//U=Σ(i*Ni)
//	//  i=k+1
//
//
//	//  L
//	int U = 0; //U=Σ(i*Ni)
//	//  i=1
//	//__int64 SigmaMax = 0;
//	//__int64 Sigma = 0;
//
//
//	for(int i=0; i<L; i++)
//	{
//		U += i*histogram[i];
//	}
//
//    __int64  varMax = 0;
//
//	for(int i=0; i< L; i++)
//	{
//
//        int gray_count = histogram[i];
//        if(gray_count)
//        {
//            N0 += gray_count;//Weighted Backgound
//        }
//        else
//        {
//            continue;
//        }
//
//
//		N1  = N - N0;//Weighted Foreground
//        if(N1 == 0)
//        {
//            break;
//        }
//
//
//        U0 += i*histogram[i];
//
//		U1  = U - U0;
//
//
//        int mB = (U0 + (N0>>2)) /N0;//Mean Background;
//        int mF = (U1 + (N1>>2)) /N1;//Mean Foreground
//
//        // Calculate Between Class Variance
//         __int64 varBetween = (__int64)N0 * (__int64)N1 * (mB - mF) * (mB - mF);
//
//         // Check if new maximum found
//         if (varBetween > varMax) {
//              varMax = varBetween;
//              cThreshold = i;
//          }
//
//	}//for
//
//
//	return cThreshold;
//}
//



    //@功能:将YUY2格式的彩色位图转化为灰度位图
    //@参数:YUVFrame,输入, YUY2格式的原始图片帧
    //      monoFrame,输出,灰度位图
	inline void Covert2MonochromeFrame_Y(const CYUY2Frame& YUVFrame, CImageFrame& monoFrame)
	{
		UINT nImageWidth  = YUVFrame.Width();
		UINT nImageHeight = YUVFrame.Height();
		monoFrame.SetSize(nImageWidth, nImageHeight, 1);

		const WORD* pYUVData = YUVFrame.GetData();
		BYTE* pMonoData      = monoFrame.GetData();
		for(UINT row = 0; row < nImageHeight; row++)
		{
			for(UINT col = 0; col < nImageWidth; col++)
			{

				*pMonoData = (*pYUVData)& 0x00FF;

				pMonoData++;

				pYUVData ++;

			}//foreach col
		}//foreach row
	}

	//@功能:将YUY2格式的图片中的像素先转化为RGB格式, 让有取R分量,保存为灰度
	inline void Covert2MonochromeFrame_R(const CYUY2Frame& YUVFrame, CImageFrame& monoFrame)
	{
		UINT nImageWidth  = YUVFrame.Width();
		UINT nImageHeight = YUVFrame.Height();
		monoFrame.SetSize(nImageWidth, nImageHeight, 1);

		const BYTE* pYUVData = (const BYTE*)YUVFrame.GetData();
		BYTE* pMonoData      = monoFrame.GetData();
		for(UINT row = 0; row < nImageHeight; row++)
		{
			for(UINT col = 0; col < nImageWidth; col+=2)
			{

				//BYTE Y, Cr, Cb;

				short Y, Cr, Cb;
				short R = 0;
				Cb = pYUVData[1];
				Cr = pYUVData[3];

				Cb = Cb - 128;
				Cr = Cr - 128;

				//双字中的第一个像素
				Y = pYUVData[0];

				R = Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5);

				if(R>255) R = 255;
				if(R<0) R = 0;

				*pMonoData =  (BYTE)R;
				pMonoData++;


				//双字中的第二个像素
				Y = pYUVData[2];
				R = Y + Cr + (Cr >> 2) + (Cr >> 3) + (Cr >> 5);
				if(R>255) R = 255;
				if(R<0) R = 0;
				*pMonoData = (BYTE)R;
				pMonoData++;



				pYUVData += 4;

			}//foreach col
		}//foreach row
	}


   //@功能:在二值化画灰度图片中提取面积最大的目标
	//@参数:monoFrame, 黑白二值化图片,处理完毕后图片中只留下面积最大的目标
	//      pBoundary, 指向保存最大外接矩形尺寸的缓冲区首地址
	//      pPixelCount, 指向保存最大目标像素个数的缓冲区首地址的指针
	//      pPtCenter, 指向保存最大目标重心的缓冲区首地址的指针
	//@说明:
	//
	inline BOOL ExtractTheLargestObject(CImageFrame& monoFrame, RECT* pBoundary = NULL, int* pPixelCount = NULL, POINT* pPtCenter = NULL)
	{

		BOOL bRet = FALSE;


		int nImageWidth  = (int)monoFrame.Width ();
		int nImageHeight = (int)monoFrame.Height();

		int nMaxObjCount = (nImageWidth/2 + 1)* (nImageHeight /2 + 1);//最大可能的目标数目


		int* pLinkSet    = new int[nMaxObjCount];
		for(int i=0; i< nMaxObjCount; i++)
		{
			pLinkSet[i] = i;//每个标记集合只有标号自己。
		}//for(i)


		int nLabelImageSize = nImageWidth * nImageHeight;
		int* pLabelImage = new int[nImageWidth * nImageHeight];
		memset(pLabelImage, 0, sizeof(int)*nImageWidth * nImageHeight);

		const BYTE* pImageDataCursor = monoFrame.GetData();

		int nNorthWest  ;//= -1 - m_nImageWidth;
		int nNorth      ;//= - m_nImageWidth;
		int nNorthEast  ;//= 1 - m_nImageWidth;
		int nWest       ;//= -1;


		int* pLabelCursor = pLabelImage;
		int nLabelSeed = 0;//标签种子
		//对图像进行标记


		nNorthWest  = -1 - int(nImageWidth);
		nNorth      = - int(nImageWidth);
		nNorthEast  = 1 - int(nImageWidth);
		nWest       = -1;


		//第一行第一列
		if(pImageDataCursor[0] != 0)
		{
			*pLabelCursor = ++nLabelSeed;
		}

		pLabelCursor++;
		pImageDataCursor++;
		//第一行
		for(int col=1; col < nImageWidth; col++)
		{

			if(*pImageDataCursor != 0 )
			{
				//正西元素
				if(pLabelCursor[nWest] != 0)
				{
					*pLabelCursor = pLabelCursor[nWest];
				}
				else
				{
					*pLabelCursor = ++nLabelSeed;
				}
			}

			pLabelCursor++;
			pImageDataCursor++;
		}

		//第二行到最后一行
		for(int row = 1; row < nImageHeight; row++)
		{

			//第1列,没有West和Northwest邻居
			if(*pImageDataCursor != 0)
			{

				if(pLabelCursor[nNorth] != 0)
				{
					*pLabelCursor = pLabelCursor[nNorth];
				}
				else if(pLabelCursor[nNorthEast] != 0)
				{
					*pLabelCursor = pLabelCursor[nNorthEast];
				}
				else
				{
					*pLabelCursor = ++nLabelSeed;
				}

			}
			pImageDataCursor ++;
			pLabelCursor ++;

			//从第2列到倒数第2列
			for(int col=1; col < nImageWidth - 1; col++)
			{
				if(*pImageDataCursor !=0 )
				{

					if(pLabelCursor[nNorthWest] != 0)
					{
						*pLabelCursor = pLabelCursor[nNorthWest];
					}
					else if(pLabelCursor[nNorth] != 0)
					{
						*pLabelCursor = pLabelCursor[nNorth];
					}
					else if(pLabelCursor[nWest] != 0 )
					{
						*pLabelCursor = pLabelCursor[nWest];
					}

					//对东北邻居特殊处理, 可能发生两组集合的合并
					if(pLabelCursor[nNorthEast] !=0)
					{
						if(!*pLabelCursor)
						{
							*pLabelCursor = pLabelCursor[nNorthEast];
						}
						else//当前像素与东北像素的Label可能不同,不同则要调整等价类的划分
						{


							int nCurLabel       = pLabelCursor[0];
							int nNorthEastLabel = pLabelCursor[nNorthEast];



							//
							if(nCurLabel != nNorthEastLabel//Label不同
								&&
								pLinkSet[nCurLabel] != pLinkSet[nNorthEastLabel] //不属于同一等价类
							)
							{


								if(pLinkSet[nCurLabel] == nCurLabel)
								{
									pLinkSet[nCurLabel] = pLinkSet[nNorthEastLabel];

								}
								else if(pLinkSet[nNorthEastLabel] == nNorthEastLabel)
								{
									pLinkSet[nNorthEastLabel] = pLinkSet[nCurLabel];
								}
								else
								{
									//组成链表
									int nEqivalanceNo = pLinkSet[nCurLabel];
									//注意到集合链表的长度始终不超过2
									pLinkSet[nEqivalanceNo] = pLinkSet[nNorthEastLabel];

									//pLinkSet[nCurLabel]  = pLinkSet[nNorthEastLabel];


								}


								//调整集合链表的长度，保证其长度始终不超过2
								for(int n=1; n<=nLabelSeed; n++)
								{
									if(pLinkSet[n] != n)
									{
										int nNext =  pLinkSet[n] ;
										while(nNext != pLinkSet[nNext])
										{
											nNext = pLinkSet[nNext];
										}
										pLinkSet[n] = nNext;
									}

								}//for each label  equivalenc set


							}
						}

					}//东北角Label不为0


					//新的标号
					if(!*pLabelCursor )
					{
						*pLabelCursor = ++nLabelSeed;
					}

				}//此列像素不为0


				pImageDataCursor ++;
				pLabelCursor ++;

			}//for each col

			//行中的最后一列,没有东北角邻居
			if(pImageDataCursor[0] != 0)
			{

				if(pLabelCursor[nWest] != 0)
				{
					pLabelCursor[0] = pLabelCursor[nWest];
				}
				else if(pLabelCursor[nNorth] != 0)
				{
					pLabelCursor[0] = pLabelCursor[nNorth];
				}
				else
				{
					pLabelCursor[0] = ++nLabelSeed;
				}

			}

			pImageDataCursor ++;
			pLabelCursor ++;

		}//for each row


		int nObjectCount = 0;

		//重新分配目标编号
		for(int i = 1; i <= nLabelSeed; i++)
		{
			if(pLinkSet[i] == i)
			{
				int nNewObjectNo = ++ nObjectCount;

				int j = 1;
				while(j <= nLabelSeed)
				{
					if(pLinkSet[j] == i)
					{
						pLinkSet[j] = nNewObjectNo;

					}

					j++;
				}

			}
		}

		if(nObjectCount > 0 )
		{

			//给整个Label图像赋予新的Label编号
			for(int i = 0; i < nLabelImageSize; i++)
			{
				pLabelImage[i] = pLinkSet[pLabelImage[i]];
			}

			//统计所占每块区域的像素的个数
			int* pPixelStatistic = new int[nObjectCount+1];


			memset(pPixelStatistic, 0, sizeof(int)*(nObjectCount+1));
			for(int i=0; i<nLabelImageSize; i++)
			{
				pPixelStatistic[pLabelImage[i]]++;
			}


			//查找标记的最多的标记值
			int nMaxPixelCountLabel = 1;
			for(int i=2; i<= nObjectCount; i++)
			{
				if(pPixelStatistic[i] > pPixelStatistic[nMaxPixelCountLabel])
				{
					nMaxPixelCountLabel = i;
				}

			}


			int nPixelCount = 0;
			RECT rcBoundary;
			rcBoundary.left  = nImageWidth - 1;
			rcBoundary.right = 0;
			rcBoundary.top   = nImageHeight - 1;
			rcBoundary.bottom = 0;

			int nMx = 0;
			int nMy = 0;
			POINT ptCenter;

			BYTE* pImageDataCusor = monoFrame.GetData();
			for(int row = 0; row < nImageHeight; row++)
			{
				for(int  col = 0; col < nImageWidth; col++)
				{
					int nIndex = row*nImageWidth + col;
					if(pLabelImage[nIndex] == nMaxPixelCountLabel)
					{
						pImageDataCusor[nIndex] = 0xFF;

						if(rcBoundary.left   > col) rcBoundary.left   = col;
						if(rcBoundary.right  < col) rcBoundary.right  = col;
						if(rcBoundary.top    > row) rcBoundary.top    = row;
						if(rcBoundary.bottom < row) rcBoundary.bottom = row;

						nMx += row;
						nMy += col;

						nPixelCount++;
					}
					else
					{
						pImageDataCusor[nIndex] = 0x00;
					}

				}//for-each col

			}//for-each row


			ptCenter.x = nMy/nPixelCount;
			ptCenter.y = nMx/nPixelCount;

			bRet = TRUE;

			if(pBoundary)
				*pBoundary = rcBoundary;

			if(pPixelCount)
				*pPixelCount = nPixelCount;

			if(pPtCenter)
				*pPtCenter = ptCenter;
			delete [] pPixelStatistic;


		}
		else
		{

			bRet = FALSE;
		}


		delete[]  pLabelImage;
		delete[] pLinkSet;


		return bRet;
	}
	

	//@参数:frame, 灰度图片
	//@功能:对灰度图片进行补洞操作
	inline void FillHole(CImageFrame& frame)
	{
		int nImageWidth  = frame.Width();
		int nImageHeight = frame.Height();

		BYTE* pData       = frame.GetData();
		BYTE* pDataCursor = pData;

		//先进行行扫,按行补洞
		for(int nRow = 0; nRow < nImageHeight; nRow++)
		{
			int nLeftMost   = nImageWidth-1;
			int nRightMost  = 0;

			for(int nCol = 0; nCol < nImageWidth; nCol++)
			{
				if(*pDataCursor > 0)
				{
					if(nCol < nLeftMost ) nLeftMost  = nCol;
					if(nCol > nRightMost) nRightMost = nCol;
				}

				pDataCursor++;
			}

			BYTE* pFillDataCursor = pData + nRow*nImageWidth + nLeftMost;

			for(int nCol = nLeftMost; nCol <= nRightMost;  nCol++)
			{
				*pFillDataCursor = 0xFF;
				pFillDataCursor++;			
			}
		}


		//再进行列扫,按列补洞
		for(int nCol = 0; nCol < nImageWidth; nCol++)
		{
			int nTopMost     = nImageHeight-1;
			int nBottomMost  = 0;

			for(int nRow = 0; nRow < nImageHeight; nRow++)
			{
				if(*(pData + nRow*nImageWidth + nCol) > 0)
				{
					if(nRow < nTopMost   ) nTopMost    = nRow;
					if(nRow > nBottomMost) nBottomMost = nRow;
				}

			}

			BYTE* pFillDataCursor = pData + nTopMost*nImageWidth + nCol;

			for(int nRow = nTopMost; nRow <= nBottomMost;  nRow++)
			{
				*pFillDataCursor = 0xFF;
				pFillDataCursor += nImageWidth;			
			}
		}

	}

    //功能:最大化对比度
    inline void MaximizeContrastness(CImageFrame& srcImage)
    {
        BYTE cMinVal = srcImage.Min();
        BYTE cMaxVal = srcImage.Max();

        BYTE map[256];

        BYTE val = 0;


        BYTE range = cMaxVal - cMinVal;

        if(range == 0) return;//防止除零溢出

        for(int i=cMinVal;  i <= cMaxVal; i++)
        {
            map[i]= 255*(i - cMinVal)/range;
        }//for

        BYTE* pSrcData = srcImage.GetData();
        int nLoop = srcImage.Size();
        while(nLoop--)
        {
            *pSrcData = map[*pSrcData];
            pSrcData++;
        }

    }



	//@功能:利用差分原理来求取屏幕的屏蔽区域
	inline void CalcMaskFrame(const CImageFrame& whiteFrame, const CImageFrame& blackFrame,  CImageFrame& maskFrame)
	{

		UINT nImageWidth  = whiteFrame.Width();
		UINT nImageHeight = whiteFrame.Height();
		maskFrame.SetSize(nImageWidth, nImageHeight, 1);

		const BYTE* pWhiteDataCursor = whiteFrame.GetData();
		const BYTE* pBlackDataCursor = blackFrame.GetData();
		BYTE* pMaskDataCursor        = maskFrame.GetData();

		int nPixelCount = maskFrame.Size();

		//int activePixelCount   = 0;
		//int ActiveIntensitySum = 0;


		UINT hist[256];//直方图统计
		memset(hist, 0, sizeof(hist));

		for(int l=0; l < nPixelCount; l++)
		{

			BYTE cWhiteFrameVal = *pWhiteDataCursor;
			BYTE cBlackFrameVal = *pBlackDataCursor;
			BYTE cDifVal = 0;


			if(cWhiteFrameVal > cBlackFrameVal)
			{
				cDifVal = cWhiteFrameVal - cBlackFrameVal;
			}
			else
			{
				cDifVal = 0x00;//cBlackFrameVal - cWhiteFrameVal;
			}

			hist[cDifVal] ++;

			*pMaskDataCursor = cDifVal;

			pWhiteDataCursor ++;
			pBlackDataCursor ++;

			pMaskDataCursor  ++;
		}//for-each(l)



        //最大化对比度
        //MaximizeContrastness(maskFrame);


		UINT hist2[256];//滤波后的直方图统计
		memset(hist2, 0, sizeof(hist2));

		//对直方图进行均值滤波
		for(int i=0; i< _countof(hist); i++)
		{
			int nColumn = 0;
			int nCount  = 0;
			int M = 5;
			for(int j = i - M/2; j <= i+ M/2; j++)
			{
				if(j < 0)continue;

				if(j >= 256) continue;

				nCount += hist[j];
				nColumn++;
			}//for-each(j)

			hist2[i] = nCount/nColumn;

		}//for-each(i)




		//存在像素积聚在低亮度区和高亮度区的情形, 不能够使用函数FindOptimumThreshold确定最优门限
		//BYTE threshold = FindOptimumThreshold(hist2);
		const UINT FilterThreshold  = nPixelCount*1/256;
		WORD wMaxSearchRange = 0;
		//从255开始查找个数>FivePercentCount的直方图单元
		for(short c=255; c>=0; c--)
		{
			if(hist2[c] >= FilterThreshold)
			{
				wMaxSearchRange = (WORD)c;
				break;
			}
		}



		short  wPeak1, wPeak2;
        const int MIN_THRESHOLD = 5;
		//在hist2[nMinThreshold, wMaxSearchRange/2]中查找第一个直方图尖峰
		wPeak1 = 0;
		UINT nPeak1Count = 0;
		for(WORD  k=MIN_THRESHOLD; k < (wMaxSearchRange/2); k++)
		{
			if(hist2[k] > nPeak1Count)
			{
				wPeak1 = k;
				nPeak1Count = hist2[k];
			}
		};


		//在hist2[wMaxSearchRange/2, wMaxSearchRange]中查找第二个直方图尖峰
		wPeak2 = 0;
		UINT nPeak2Count = 0;
		for(WORD k= wMaxSearchRange/2; k < wMaxSearchRange; k++)
		{
			if(hist2[k] > nPeak2Count)
			{
				wPeak2 = k;
				nPeak2Count = hist2[k];
			}

		}


		//在hist2[cPeak1, cPeak2]中查找直方图谷底
		BYTE threshold = 5;//计算门限
		UINT nValleyCount  = (std::numeric_limits<int>::max)();
		for(WORD k = wPeak1; k < wPeak2; k++)
		{
			if(hist2[k] < nValleyCount)
			{
				nValleyCount = hist2[k];
				threshold = (BYTE)k;
			}

		}

        //threshold -=2;//人为降低门限,以确保目标尽能完整。


       //threshold = FindThreshold_Ostu(hist);

		pMaskDataCursor  = maskFrame.GetData();
		for(int l = 0; l < nPixelCount; l++)
		{
			if(*pMaskDataCursor >= threshold)
			{
				*pMaskDataCursor = 0xFF;
			}
			else
			{
				*pMaskDataCursor = 0x00;
			}
			pMaskDataCursor++;

		}

	}

    inline void WordFrame2ByteFrame(const CWordFrame& wordFrame, CImageFrame& byteFrame)
   {
        byteFrame.SetSize(wordFrame.Width(), wordFrame.Height(), 1);
        WORD max = wordFrame.Max();
        WORD min = wordFrame.Min();
        WORD range = max -min;
        const WORD* pWordPixel = wordFrame.GetData();

        BYTE* pPixel = byteFrame.GetData();
        int nLoopCount  = wordFrame.GetPixelCount();
        //16位灰度位图转为8位灰度位图
        while(nLoopCount)
        {
            if(range == 0)
            {
                *pPixel = (BYTE)0xFF;
            }
            else
            {
                *pPixel = BYTE( (UINT)255 * (UINT)(*pWordPixel - min)/(UINT)range);
            }
            pWordPixel++;
            pPixel ++;
            nLoopCount --;
        }

    }

