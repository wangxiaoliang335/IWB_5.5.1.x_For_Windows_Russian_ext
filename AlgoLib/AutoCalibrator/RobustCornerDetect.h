#pragma once
#include <limits>
#include "ctmf.h"
#include "Histogram.h"
#include <assert.h>
#include "CornerDetect.h"
    
//#include "Profile.h"
//中值滤波
template<int R = 2>
class CMeidanFilter
{
public:
	
	CMeidanFilter()
		:
	m_oSrcBitmap()
	{

	}

	void SetSize(int nWidth, int nHeight)
	{
		m_oSrcBitmap.SetSize(nWidth, nHeight, 1);
	}


	void operator()(CImageFrame& srcBitmap)
	{

		m_oSrcBitmap = srcBitmap;

		const BYTE* pSrc  = m_oSrcBitmap.GetData();
		      BYTE* pDest = srcBitmap.GetData();

        int nImageWidth  = srcBitmap.Width();
		int nImageHeight = srcBitmap.Height();

		ctmf(
			pSrc,
			pDest,
			nImageWidth,
			nImageHeight,
			nImageWidth,
			nImageWidth,
			R,
			1,//gray scale image has only one channel
			512*1024);

	}
protected:

	CImageFrame m_oSrcBitmap;

};


//@功能:对每一个像素在(2R+1)×(2R+1)窗口中求取均值。
template<int R=1>
class CBlurOperator
{
public:
    void operator ()(size_t w, size_t h, const uint8_t* srcImage, uint8_t* dstImage)
    {
        size_t y,x;
        int j,i;
        const unsigned pixel_count = (2*R+1)*(2*R+1);
        for(y = R; y < h - R; y++)
            for( x = R; x < w - R; x++)
            {
                const unsigned offset= x + y * w;
                uint16_t sum = 0u;
                for(j = -R; j <= R; j++)
                    for( i = -R; i <= R; i++)
                    {
                        sum += srcImage[offset + j*w + i];
                    }

               dstImage[offset] = sum/pixel_count;
                
            }
    }


};

//@功能:二值化操作符
class CBinarizeOperator 
{
public:
    template<class T>
    void operator () (uint8_t threshold, size_t w, size_t h, T* image)
    {
        size_t y,x;
        for(y = 0; y < h ; y++)
            for( x = 0; x < w; x++)
            {
                const unsigned offset=  x + y * w;
                image[offset] = image[offset] > threshold?0xFF:0x00;
            }
    }



};


//基于论文
//《Robust Checkerboard Recognition for Efficient Nonplaner Geometry Registration In Project-camera Systems》
//强鲁棒性角点检测算法
//

/*
template<int  LAYER_NO = 1>
class Layer
{

//每层的像素相对于中心的位置偏移量组成的数组
int layer[LAYER_NO * 8];
};*/


#include "../inc/Algorithm/HeapSort.h"
class CLayer
{

public:
	CLayer()
		:
	m_nLayerIndex(0)
	{

	}

	//@功能:设置层的索引号
	//@参数:nLayerIndex, 从0开始的层索引号
	void SetLayerIndex(int nLayerIndex)
	{
		unsigned int nSize = (nLayerIndex +1) * 8;
		m_vecOffset.resize(nSize);

		//计算该层相对于中心的偏移量
		int nMaxOffset = nLayerIndex + 1;
		int iPhase     = 0;

		int nOffsetX = - nMaxOffset;
		int nOffsetY = - nMaxOffset;

		for(unsigned int i= 0 ; i < nSize; i++)
		{
			//从左上角开始,顺时针遍历。
			m_vecOffset[i].x = nOffsetX;
			m_vecOffset[i].y = nOffsetY;

			switch(iPhase)
			{
			case 0://上侧边

				if(nOffsetX == nMaxOffset)
				{
					iPhase = 1;
					nOffsetY ++;
				}
				else
				{
					nOffsetX ++;
				}
				break;

			case 1://右侧边

				if(nOffsetY == nMaxOffset)
				{
					iPhase = 2;
					nOffsetX--;
				}
				else
				{
					nOffsetY++;
				}


				break;

			case 2:

				if(nOffsetX == - nMaxOffset)
				{
					iPhase = 3;
					nOffsetY --;
				}
				else
				{
					nOffsetX -- ;
				}

				break;


			case 3:

				if(nOffsetY == -nMaxOffset)
				{

				}
				else
				{
					nOffsetY--;
				}

				break;

			default:
				assert(FALSE);
			}//switch(iPhase)

		}//for-each(i)

		m_nLayerIndex = nLayerIndex;
	}

	const POINT* GetOffsetList()const
	{
		return &m_vecOffset[0];
	}

	UINT GetLayerSize()const
	{
		return m_vecOffset.size();
	}


protected:
	std::vector<POINT> m_vecOffset;
	int                m_nLayerIndex;
};



//矩形探测窗
class RectScanWindow
{

public:
	//参数:矩形框的宽度是2w+1, 由w圈构成
	//       
	RectScanWindow(unsigned int  w)
	{
		m_vecLayers.resize(w);

		for(unsigned int i = 0; i < w; i++)
		{
			m_vecLayers[i].SetLayerIndex(i);
		}

	}


	const CLayer* GetLayer(unsigned int iLayerIndex)const
	{
		if(iLayerIndex >= m_vecLayers.size()) return NULL;

		return &m_vecLayers[iLayerIndex];

	};
protected:
	std::vector<CLayer> m_vecLayers;
};

//比特环
//static const int MAX_LAYER = 21;
static const int MAX_LAYER = 50;//modified by toxuke@gmail.com, 2011/12/29
static const int MIN_LAYER = 5;
struct TLineSegment
{
	UINT nStartPos;
	UINT nEndPos  ;
	UINT nLength  ;
	UINT type     ;//0:为'0'串;1:为'1'串
};
class CBitRing
{
public:
	CBitRing():
	  m_uBitLength(1)
	  {
		  Clear();
	  }
	  void SetLength(UINT  uBitLength)
	  {
		  UINT uNeedDword = uBitLength >> 5;
		  if(uNeedDword > _countof(m_aryData))
		  {
			  m_uBitLength = _countof(m_aryData)<<5;
		  }
		  else
		  {
			  m_uBitLength = uBitLength;
		  }
	  }

	  void Clear()
	  {
		  memset(m_aryData, 0, sizeof(m_aryData));
		  m_uBitLength = 0;

		  memset(m_aryLineSegments, 0, sizeof(m_aryLineSegments));
		  m_uLineSegmentCount = 0;

	  }

	  void SetBit(UINT uBitNo)
	  {
		  UINT uWordOffset  = uBitNo>>5;
		  UINT uBitOffset   = uBitNo % 32;

		  m_aryData[uWordOffset] |= 1 << uBitOffset;
	  }

	  void ClearBit(UINT uBitNo)
	  {

		  UINT uWordOffset  = uBitNo>>5;
		  UINT uBitOffset = uBitNo % 32;

		  m_aryData[uWordOffset] &= ~(1 << uBitOffset);	
	  }

	  UINT operator[](UINT uBitNo)
	  {
		  UINT uDwordIndex  = uBitNo>>5;
		  UINT uDwordOffset = uBitNo % 32;

		  return (m_aryData[uDwordIndex] & (1 << uDwordOffset))?1:0;	
	  }


	  UINT GetBit(UINT uBitNo)
	  {
		  UINT uDwordIndex  = uBitNo>>5;
		  UINT uDwordOffset = uBitNo % 32;

		  return (m_aryData[uDwordIndex] & (1 << uDwordOffset))?1:0;	
	  }

	  DWORD* GetData()
	  {
		  return m_aryData;
	  }
	  const DWORD* GetData()const
	  {
		  return m_aryData;
	  }

	  //计算差分不为0的个数
	  UINT GetPartCount()
	  {
		  UINT uPartCout = 0;

		  UINT uPrevData = GetBit(m_uBitLength - 1);
		  UINT uCurData  = m_aryData[0] & 0x01;

		  UINT count = 0;

		  int nBitOffset = 0;
		  DWORD* pdwData = m_aryData;
		  DWORD dwData   = *pdwData;
		  do
		  {

			  uCurData = (dwData & (1<<nBitOffset))?1:0;

			  if(uCurData != uPrevData)
			  {
				  uPartCout++;
			  }

			  uPrevData = uCurData;

			  nBitOffset++;
			  if(nBitOffset == 32)
			  {
				  nBitOffset = 0;
				  pdwData++;
				  dwData  = *pdwData;

			  }
			  count++;

		  }while(count < m_uBitLength);

		  return uPartCout;
	  };


	  //@功能:对环状结构进行腐蚀操作
	  //@参数:SEL, 结构元素数组长度
	  //      type, 0,删除连续长度小于SEL的0串
	  //            1,删除连续长度小于SEL的1串
	  //
	  //说明:问题实际归结为,在环状结构中剔除连续1长度小于SEL的线段。
	  //例如
	  //SEL = 2时。
	  //例如:00000100000000110000000000000001111111111111100000000
	  //          ↑剔除   ↑剔除
	  //
	  //例如:  111111111110111111111000000000000000001111
	  //                 ↑剔除
	  //设二进制'1', '0'串的下标按照顺时针方向增加。
	  //
	  //二进制环串
	  //例如
	  //0x007b1400 0xffff0000 0x0000000f
	  //
	  //二进制串(从低位到高位)
	  //0000 0000 0010 1000 1101 1110 0000 0000 0000 0000 0000 0000 1111 1111 1111 1111 1111 0000
	  //
	  //先对'1'串腐蚀后的结果为
	  //0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 1111 1111 1111 1111 1111 0000
	  //
	  //先对'0'串腐蚀后的结果为
	  //0000 0000 0011 1111 1111 1110 0000 0000 0000 0000 0000 0000 1111 1111 1111 1111 1111 0000
	  //
	  //而期望的腐蚀结果为
	  //0000 0000 0000 0000 1111 1110 0000 0000 0000 0000 0000 0000 1111 1111 1111 1111 1111 0000
	  //
	  //长度小于SEL的'0'串和'1'串, 根据其前后的'1'串和'0'串的长度来决定是否进行腐蚀操作。
	  //
	  void Erosion(UINT SEL, UINT type)
	  {
		  UINT uLength   = 0;//线段长度
		  UINT uFirst    = m_aryData[0] & 0x01;//第一个元素
		  BOOL bInc      = TRUE;//下标递增循环
		  UINT uEndPos   = 0;
		  UINT uStartPos = 0;
		  if(uFirst == type)
		  {
			  uLength  = 0;

			  //顺时针查找二进制'0'串或者二进制'1'串的结束位置
			  while(GetBit(uEndPos) == type && uEndPos < m_uBitLength)
			  {
				  uEndPos++;
				  uLength++;
			  }

			  if(uEndPos == m_uBitLength)//整个环全为1
			  {
				  return;
			  }

			  bInc = FALSE;//剩下部分按逆时针顺序处理
			  uStartPos =  m_uBitLength - 1;

		  }
		  else
		  {
			  uStartPos = 1;
			  uEndPos   = m_uBitLength - 1;
		  }


		  UINT pos = uStartPos;


		  if(bInc)//按顺时针顺序(下标递增)处理
		  {
			  UINT uBitOffset = uStartPos%32;
			  DWORD* pdwData  = &m_aryData[uStartPos>>5];
			  DWORD dwData    = *pdwData;


			  while(pos != uEndPos)
			  {

				  if((dwData & (1<<uBitOffset)) == (type << uBitOffset))
				  {
					  uLength ++;
				  }
				  else
				  {
					  if(uLength > 0  && uLength < SEL)
					  {
						  UINT uChangePos = pos - 1;

						  do
						  {

							  if(type)
							  {
								  ClearBit(uChangePos);
							  }
							  else
							  {
								  SetBit(uChangePos);
							  }

							  uChangePos--;
							  uLength --;
						  }while(uLength > 0);
					  }

					  uLength = 0;
				  }

				  uBitOffset++;

				  if(uBitOffset == 32)
				  {
					  pdwData ++;
					  dwData  = *pdwData;
					  uBitOffset = 0;
				  }

				  pos++;
			  }//while



			  //最后一个线段
			  if(uLength > 0  && uLength < SEL)
			  {
				  UINT uChangePos = pos - 1;

				  do
				  {

					  if(type)
					  {
						  ClearBit(uChangePos);
					  }
					  else
					  {
						  SetBit(uChangePos);
					  }

					  uChangePos--;
					  uLength --;
				  }while(uLength > 0);
			  }


		  }

		  else//按逆时针顺序(下标递减)处理
		  {
			  UINT uBitOffset = uStartPos%32;
			  DWORD* pdwData  = &m_aryData[uStartPos>>5];
			  DWORD dwData    = *pdwData;

			  while(pos != uEndPos)
			  {
				  if((dwData & (1<<uBitOffset)) == (type << uBitOffset) )
				  {
					  uLength ++;
				  }
				  else
				  {
					  if(uLength > 0 && uLength < SEL)
					  {
						  UINT uChangePos = pos + 1;

						  do
						  {
							  if(uChangePos == m_uBitLength)
							  {
								  uChangePos = 0;

							  }
							  if(type)
							  {
								  ClearBit(uChangePos);
							  }
							  else
							  {
								  SetBit(uChangePos);
							  }

							  uChangePos++;

							  uLength --;
						  }while(uLength > 0);

					  }

					  uLength = 0;
				  }


				  if(uBitOffset == 0)
				  {

					  pdwData --;
					  dwData  = *pdwData;
					  uBitOffset = 31;
				  }
				  else
				  {
					uBitOffset -- ;
				  }

				  pos--;
			  }//while


			  if(uLength > 0 && uLength < SEL)
			  {
				  UINT uChangePos = pos + 1;

				  do
				  {
					  if(uChangePos == m_uBitLength)
					  {
						  uChangePos = 0;

					  }
					  if(type)
					  {
						  ClearBit(uChangePos);
					  }
					  else
					  {
						  SetBit(uChangePos);
					  }

					  uChangePos++;

					  uLength --;
				  }while(uLength > 0);
			  }//if

		  }//else
	  }


	  //二进制串中的线段合并。
	  //例如二进制字符串
	  //从低位----------------->高位
	  // 0000 0111  1111 1011 1001 1111 1111 0111 0111 11 00
	  //有10段线段
	  //start   end  length type
	  //0x26    0x04  7      0
	  //0x05    0x0B  8      1
	  //0x0D    0x0D  1      0
	  //0x0E    0x10  3      1
	  //0x11    0x12  2      0 
	  //0x13    0x1B  9      1
	  //0x1C    0x1C  1      0
	  //0x1D    0x1F  3      1
	  //0x20    0x20  1      0
	  //0x21    0x25  5      1
	  //
	  //经过合并操作后,整条字符串将变为两个线段
	  //0000 0111  1111 1111 1111 1111 1111 1111 1111 11 00
	  //  
	  // (低位)        (高位)
	  // Left    Line  Right
	  //
	  //对于第一个线段的合并到Line 0
	  // Line(N-1)    Line0 Line 1
	  //
	  //对于最后一个线段的合并处理是合并到0号线段中去。
	  //Line(N-2) Line(N-1) 0
	  //
	  void Merge(UINT SEL)
	  {
		  //统计所有线段
		  StatisticLineSemgent();

		  UINT iLineIndex  = 0;
		  

		  //1.对三段相邻线段长度段小于SEL,但进行三合并后的长度大于SEL,对于这种情形,优先进行合并
		  if(m_uLineSegmentCount >2 )
		  {
			 
			  while(iLineIndex < m_uLineSegmentCount)
			  {
				  UINT uRightIndex = 0;
				  UINT uLeftIndex  = 0;

				  if(iLineIndex == 0)
				  {
						uLeftIndex = m_uLineSegmentCount - 1;
				  }
				  else
				  {
						uLeftIndex = iLineIndex - 1;
				  }

				  if(iLineIndex == m_uLineSegmentCount-1)
				  {

					  uRightIndex = 0;
				  }
				  else
				  {
					  uRightIndex = iLineIndex + 1;
				  }


				  UINT a = m_aryLineSegments[uLeftIndex].nLength;
				  UINT b = m_aryLineSegments[iLineIndex].nLength;
				  UINT c = m_aryLineSegments[uRightIndex].nLength;

				  
				  //if(a < SEL && b < SEL && c < SEL && (a + b +c) >= SEL && (a+c) > 2*b)
				  if(b < SEL  && (a + b +c) >= SEL && (a+c) > 2*b)
				  {

					  //合并三段线段。
					  //合并到当前前线段中
					 if(iLineIndex == 0)
					 {
						 m_aryLineSegments[iLineIndex].nLength   = a + b +c;
						 m_aryLineSegments[iLineIndex].nStartPos = m_aryLineSegments[uLeftIndex].nStartPos;
						 m_aryLineSegments[iLineIndex].nEndPos   = m_aryLineSegments[uRightIndex].nEndPos;
						 m_aryLineSegments[iLineIndex].type      = m_aryLineSegments[uRightIndex].type;

						 //从2...m_uLineSegmentCount-1往前挪动一个单元
						 for(UINT iLoop = 1; iLoop < m_uLineSegmentCount - 2; iLoop ++)
						 { 
							m_aryLineSegments[iLoop] = m_aryLineSegments[iLoop + 1];
						 }

					 }
					 else if(iLineIndex == m_uLineSegmentCount - 1)
					 {

						 m_aryLineSegments[0].nLength   = a + b +c;
						 m_aryLineSegments[0].nStartPos = m_aryLineSegments[uLeftIndex].nStartPos;
						 m_aryLineSegments[0].nEndPos   = m_aryLineSegments[uRightIndex].nEndPos;
						 m_aryLineSegments[0].type      = m_aryLineSegments[uRightIndex].type;

					 }
					 else//合并到右线段中
					 {
						
					 	 m_aryLineSegments[uLeftIndex].nLength   = a + b + c;
						 //m_aryLineSegments[uLeftIndex].nStartPos = m_aryLineSegments[uLeftIndex].nStartPos;
						 m_aryLineSegments[uLeftIndex].nEndPos   = m_aryLineSegments[uRightIndex].nEndPos;
						//m_aryLineSegments[uLeftIndex].type     = m_aryLineSegments[uRightIndex].type;

						 //从uRightIndex+1...m_uLineSegmentCount-1往前挪动连个单元
						 for(UINT iLoop = uRightIndex + 1; iLoop <= m_uLineSegmentCount-1; iLoop ++)
						 {
							m_aryLineSegments[iLoop-2] = m_aryLineSegments[iLoop];
						 }

					 }
					  

					 m_uLineSegmentCount -= 2;

					 if(m_uLineSegmentCount <=2 )
					 {
						break;
					 }
					  
					 //进行了合并操作,当前线段位置移移入了新的内容,除了是第一条线段的情形
					 //线段索引号不增加
					 if(iLineIndex == 0)
					 {
						iLineIndex++;
					 }

				  }
				  else
				  {
					  iLineIndex++;
				  }


			  }//while()
		  }//if(segmengLine >= 4)


		  //2.将短线合并成长线。
		  //iLineIndex = 0;
		  //if(m_uLineSegmentCount >=4 )
		  //{
			 //
			 // while(iLineIndex < m_uLineSegmentCount)
			 // {
				//  UINT uRightIndex = 0;
				//  UINT uLeftIndex  = 0;

				//  if(iLineIndex == 0)
				//  {
				//		uLeftIndex = m_uLineSegmentCount - 1;
				//  }
				//  else
				//  {
				//		uLeftIndex = iLineIndex - 1;
				//  }

				//  if(iLineIndex == m_uLineSegmentCount-1)
				//  {

				//	  uRightIndex = 0;
				//  }
				//  else
				//  {
				//	  uRightIndex = iLineIndex + 1;
				//  }

				//  


				//  UINT a = m_aryLineSegments[uLeftIndex].nLength;
				//  UINT b = m_aryLineSegments[iLineIndex].nLength;
				//  UINT c = m_aryLineSegments[uRightIndex].nLength;
				//  if(b < SEL && (a+c) > b )
				//  {

				//	  //合并三段线段。
				//	  //合并到但前线段中
				//	 if(iLineIndex == 0)
				//	 {
				//		 m_aryLineSegments[iLineIndex].nLength   = a + b +c;
				//		 m_aryLineSegments[iLineIndex].nStartPos = m_aryLineSegments[uLeftIndex].nStartPos;
				//		 m_aryLineSegments[iLineIndex].nEndPos   = m_aryLineSegments[uRightIndex].nEndPos;
				//		 m_aryLineSegments[iLineIndex].type      = m_aryLineSegments[uRightIndex].type;

				//		 //从2...m_uLineSegmentCount-1往前挪动一个单元
				//		 for(UINT iLoop = 1; iLoop < m_uLineSegmentCount - 2; iLoop ++)
				//		 { 
				//			m_aryLineSegments[iLoop] = m_aryLineSegments[iLoop + 1];							
				//		 }

				//	 }
				//	 else//合并到右线段中
				//	 {
				//		
				//	 	 m_aryLineSegments[uLeftIndex].nLength   = a + b + c;
				//		 //m_aryLineSegments[uLeftIndex].nStartPos = m_aryLineSegments[uLeftIndex].nStartPos;
				//		 m_aryLineSegments[uLeftIndex].nEndPos   = m_aryLineSegments[uRightIndex].nEndPos;
				//		//m_aryLineSegments[uLeftIndex].type     = m_aryLineSegments[uRightIndex].type;

				//		 //从uRightIndex+1...m_uLineSegmentCount-1往前挪动连个单元
				//		 for(UINT iLoop = uRightIndex + 1; iLoop <= m_uLineSegmentCount-1; iLoop ++)
				//		 {
				//			m_aryLineSegments[iLoop-2] = m_aryLineSegments[iLoop];
				//		 }

				//	 }
				//	  

				//	 m_uLineSegmentCount -= 2;

				//	 if(m_uLineSegmentCount <=4 )
				//	 {
				//		break;
				//	 }

				//  }

				//  iLineIndex++;

			 // }//while()
		  //}//if(segmengLine >= 4)


	  }


	  UINT GetLineSegmentCount()const
	  {
		return m_uLineSegmentCount;
	  }

	    UINT GetLineSegmentCount(unsigned int SEL)const
	  {

			UINT iLineIndex  = 0;
			int nLineCount = 0;
		  	 while(iLineIndex < m_uLineSegmentCount)
			 {
				if(m_aryLineSegments[iLineIndex].nLength  >= SEL)
				{
					nLineCount++;
				}
				iLineIndex++;
			 }

		return nLineCount;
	  }

   //methods
	//统计'0', '1'二进制串环中的线段条数。
	int StatisticLineSemgent()
	{

		m_uLineSegmentCount = 0;

		UINT uLineStartPos = 0;
		UINT uLoopEndPos   = 0;
		UINT uPos          = 0;
		UINT uLength       = 0;
		UINT uLineType     = 0;

		if(m_uBitLength == 0)
		{

			return 0;
		}

		if(m_uBitLength == 1)
		{
			m_aryLineSegments[0].nStartPos = 0;
			m_aryLineSegments[0].nEndPos   = 0;
			m_aryLineSegments[0].nLength   = 1;
			m_aryLineSegments[0].type      = GetBit(0);
			m_uLineSegmentCount = 1;
			return m_uLineSegmentCount;

		}

		//1.搜索第一条线段的长度
		UINT uFirstBit = GetBit(0);
		UINT uLastBit  = GetBit(m_uBitLength - 1);

		//线段跨越头尾
		if(uFirstBit ==  uLastBit)
		{
			uLength = 1;
			uLoopEndPos = m_uBitLength - 1;


			while(uLoopEndPos >0 &&   GetBit(uLoopEndPos) == uLastBit)
			{
				uLoopEndPos --;
				uLength++;
			}

			if(uLoopEndPos == 0)//只有一个线段
			{
				m_aryLineSegments[0].nStartPos = 0;
				m_aryLineSegments[0].nEndPos   = m_uBitLength - 1;
				m_aryLineSegments[0].nLength   = m_uBitLength;
				m_aryLineSegments[0].type      = uLastBit;

				m_uLineSegmentCount = 1;
				return m_uLineSegmentCount;
			}

			uLineStartPos = uLoopEndPos + 1;
			uPos          = 1;
			uLineType     = uLastBit;


		}
		else
		{ 
			uPos         = uLineStartPos = 1;
			uLength      = 1;
			uLineType    = GetBit(0);
			uLoopEndPos  = m_uBitLength - 1;
		}


		UINT uBitOffset = uPos % 32;
		DWORD* pdwData  = &m_aryData[uPos>>5];
		DWORD dwData    = *pdwData;


		while(uPos <= uLoopEndPos)
		{
			UINT val = (dwData & (1<<uBitOffset))?1:0;
			if(val == uLineType)
			{
				uLength ++;
			}
			else
			{

				m_aryLineSegments[m_uLineSegmentCount].nStartPos = uLineStartPos;
				m_aryLineSegments[m_uLineSegmentCount].nEndPos   = uPos - 1;
				m_aryLineSegments[m_uLineSegmentCount].nLength   = uLength;
				m_aryLineSegments[m_uLineSegmentCount].type      = uLineType;

				m_uLineSegmentCount++;


				uLineStartPos = uPos;
				uLineType = val;
				//if(uPos != uLoopEndPos)
				//{
				//	uLength  = 1;
				//}
				//else
				//{
					uLength  = 1;
				//}

			}

			uPos++;
			uBitOffset++;
			if(uBitOffset == 32)
			{
				uBitOffset = 0;
				dwData    = *(++pdwData);	

			}

		}//while(1);


		//最后一条线段
		if(uLength > 0)
		{
			m_aryLineSegments[m_uLineSegmentCount].nStartPos = uLineStartPos;
			m_aryLineSegments[m_uLineSegmentCount].nEndPos   = uPos - 1;
			m_aryLineSegments[m_uLineSegmentCount].nLength   = uLength;
			m_aryLineSegments[m_uLineSegmentCount].type      = uLineType;
			m_uLineSegmentCount++;
		}


		return m_uLineSegmentCount;
	}
protected://members
	DWORD m_aryData[((MAX_LAYER<<3) + 16) / 32];
	UINT  m_uBitLength;

	TLineSegment m_aryLineSegments[MAX_LAYER<<3];//线段数组,最极端的情形是1位是一个线段
	UINT m_uLineSegmentCount;
};



/* Type declarations */
#ifdef _MSC_VER
#include <basetsd.h>
typedef UINT8  uint8_t;
typedef UINT16 uint16_t;
typedef INT16  int16_t;
typedef UINT32 uint32_t;
#pragma warning( disable: 4799 )
#else
#include <stdint.h>
#endif

class CRobustCornerDetector
{
public:
	
	BOOL operator ()(const CImageFrame& refSrcFrame, const CImageFrame& refMaskFrame, UINT uExpectedCornerNumber, int nDetectRadius, ECalibDebugLevel eDebugLevel=E_CALIB_DEBUG_LEVEL_CONCISE, int nDebugTimes =1)
	{
		//window size range[WSmin, WSmax] to limit the window size
		// for extreme large or small grid.

		//Debug_SaveImageFrame(frame, _T("Source.bmp"));


		CImageFrame frame = refSrcFrame;
        
        frame = frame & refMaskFrame;

		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			Debug_SaveImageFrame(frame, _T("CheckerDetector_Raw.jpg"));
			
		}

        CImageFrame frameBlurred = frame;


        CBlurOperator<2> blur;
        blur(frame.Width(), frame.Height(), frame.GetData(), frameBlurred.GetData());
        frame = frameBlurred;

        


		UINT nWidth  = frame.Width();
		UINT nHeight = frame.Height();
		CImageFrame response;
		response.SetSize(nWidth, nHeight, 2);
        response.Clear();

		
       cross_detect5(nWidth, nHeight, frame.GetData(), response.GetData());
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			Debug_SaveImageFrame(response, _T("Corner_Response.jpg"));
			
		}
        //corner_detect5检测的目标响应图作为屏蔽图
        CImageFrame candidateMaskFrame = refMaskFrame;
        
        BYTE*    pCandidateMaskFrame = candidateMaskFrame.GetData();
        uint8_t* pResponse  = response.GetData();

        uint8_t response_Threshold = frame.Avg();
        response_Threshold >>= 1;
        for(UINT row=0; row<nHeight; row++)
		{
            for(UINT col=0; col<nWidth; col++)
			{
                if(*pCandidateMaskFrame == 0xFF)
                {
                    if(*pResponse< response_Threshold)
                    {
                      *pCandidateMaskFrame = 0x00;
                    }
                }

                pResponse++;
                pCandidateMaskFrame++;
            }
        }
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			Debug_SaveImageFrame(candidateMaskFrame, _T("CheckerDetector_Mask.jpg"));
		}



		
		//CMeidanFilter<> medianFilter;

		//medianFilter(frame);
		//medianFilter(frame);
		//Debug_SaveImageFrame(frame, _T("source_after_median_filter.bmp"));
		//if(nDetectRadius <= MIN_LAYER)
		{
			//medianFilter(frame);
			//Debug_SaveImageFrame(frame, _T("source_after_median_filter.bmp"));
		}
		


		//CHistogram<> histo(0,255);
		//CImageFrame histoImage;

		//histo.DoStatistic(frame, maskFrame);
		//histo.AvgFilter(3);		
		//histo.SaveToImage(histoImage);
		//Debug_SaveImageFrame(histoImage, _T("histo_calibrate_pattern.bmp"));

		const int WSmin = MIN_LAYER;//
		int WSmax = MAX_LAYER;// PROFILE::GetAutoCalibrateMaxDetectLayer();//MAX_LAYER;//

		//if(WSmax > MAX_LAYER)
		//{
		//	WSmax = MAX_LAYER;
		//}

		int nW = nDetectRadius;
		if(nW < WSmin)
		{
			nW = WSmin;
		}

		if(nW > WSmax)
		{
			nW = WSmax;
		}


		BOOL bRet = TRUE;

		RectScanWindow scanWindow(nW);

		std::vector<int> scanResults;
		scanResults.resize(nW);



		UINT nImageWidth  = frame.Width();
		UINT nImageHeight = frame.Height();

		//调试输出灰度图
		//<<
		CImageFrame debugFrame;
		debugFrame.SetSize(nImageWidth, nImageHeight, 1);
		BYTE* pDebug  = debugFrame.GetData();
		//>>

		//位图数据
		CBitFrame bitFrame;
		bitFrame.SetSize(nImageWidth, nImageHeight);
		bitFrame.Clear();
		//指向二指化位图的首地址的指针
		DWORD* p32BitData   = bitFrame.GetData();
		UINT uBitOffset     = 0;

		//
		const BYTE* pSrcData = frame.GetData();		
      

		const BYTE* pMaskData            = refMaskFrame.GetData();
		const BYTE* pMaskDataCursor      = pMaskData;
        const BYTE* pCandidateData       = candidateMaskFrame.GetData();
        const BYTE* pCandidateDataCursor = pCandidateData;

		//std::vector<BYTE> layerData;
		//std::vector<BYTE> diffData;
		BYTE layerData[MAX_LAYER * 8];

		//最大
		//layerData.resize(nW * 8 * 2 );
		//diffData.resize (nGridWidth * 8 * 2);

		//std::vector<BYTE> layerData;

		//int n4RegionThreshold = nW * 3/5;
		//int n4RegionThreshold = nW * 7/10;

        //2012/08/13,由于已经通过corner_detect5函数计算出屏蔽图了
        //干扰点已经很小了
        //<<
		int n4RegionThreshold = nW * 7/10;
        //>>

		//int n4RegionThreshold = nW * 4/5;//modified by toxuke@gmail.com, 2011/12/26
		//int n4RegionThreshold = nW * 1/2;

		CBitRing ring;


		UINT SEL = 0;
		//对每行循环,
		//由于计算量巨大, 因此需要考虑算法的优化
		for(UINT row=0; row<nImageHeight; row++)
		{
			
			//对每列循环
			for(UINT col=0; col<nImageWidth; col++)
			{

				if( *pCandidateDataCursor == 0)
				{
					goto NEXT;

				}

				int nDebug = 0;
				//if(col == 95 && row == 257) 
				//{
				//	nDebug = 1;
				//}
				//else if(row ==401 && col == 314)
				//{
				//	 nDebug = 2;

				//}

				//if(col == 176 && row == 302)
				//{
				//	int nDebug = 0;	
				//}

				for(int layer=0; layer<nW; layer++)
				{

					//if(row == 477 && col == 527 && layer == 9)
					//{
					//	AtlTrace(_T("col=%d\r\n"), col);
					//}

					ring.Clear();
					ring.SetLength((layer+1)<<3);

					const CLayer* pLayer = scanWindow.GetLayer(layer);
					//assert(pLayer);

					int layerSize  = pLayer->GetLayerSize();
					const POINT* pOffset = pLayer->GetOffsetList();


					int nSum = 0;
					int nCount = 0;
					BOOL bLayerIsNotComplete = false;//圈层不完整
					for(int i=0;  i <layerSize; i++)
					{

						int nX = col + pOffset[i].x;
						int nY = row + pOffset[i].y;

						//圈层部分在图像以外
						if(nX < 0 || nX >= int(nImageWidth) )
						{
							bLayerIsNotComplete = TRUE;
							break;
							//continue;
						}
						//圈层部分在图像以外
						if(nY < 0 || nY >= int(nImageHeight))  
						{
							
							bLayerIsNotComplete = TRUE;
							break;
							//continue;
						}


						//圈层在屏蔽区域以外
						if(pMaskData[nImageWidth*nY + nX] == 0x00)
						{
							bLayerIsNotComplete = TRUE;
							break;
						}


						layerData[i] = pSrcData[nImageWidth*nY + nX];
						nSum += pSrcData[nImageWidth*nY + nX];
						nCount ++;

					}//for-each(i);



					if(bLayerIsNotComplete)//圈层在屏蔽区域以外
					{
						scanResults[layer] = 0;
					}
					else//圈层在屏蔽区域内
					{

						//统计像素均值	
						BYTE avgIntensity = 0;

						if(nCount != 0)
						{
							avgIntensity = (nSum + (nCount>>1))/nCount;
						}

						//avgIntensity = (avgIntensity + middleIntensity)/2;

						//if(avgIntensity < 250)
						//{
						//	avgIntensity += 2;
						//}
						
						//角点像素亮度与平均值的最低偏离移量
						//static const int MIN_OFFSET_FROM_AVG_VAL = 0;
						static const int MIN_OFFSET_FROM_AVG_VAL = 4;

						//对一维Layer进行二值化处理
						for(int i=0;  i <layerSize; i++)
						{
							if(layerData[i] >= avgIntensity + MIN_OFFSET_FROM_AVG_VAL)
							{
								ring.SetBit(i);
								//layerData[i]  = 1;
							}
							//else
							//{
							//	layerData[i]  = 0;
							//}

						}//for-each(i)


						int nDifCount = 0;

						if(layer > 1)
						{
						
							
								SEL = ((layer+1)<<3)/10;


                                ring.Merge(SEL>3?3:SEL);
								//ring.Erosion(SEL, 1);
								//ring.Erosion(SEL, 0);
								//ring.Dilation(SEL);
								scanResults[layer] = ring.GetLineSegmentCount(SEL);
	

						}
						else
						{
							ring.StatisticLineSemgent();
							scanResults[layer] = ring.GetLineSegmentCount();
						}

					}
					//else
					//{
					   //scanResults[layer] = ring.GetPartCount();
					//}

				}//for-each(layer)


				int n4RegionLayerCount = 0;
				for(int layer=0; layer<nW; layer++)
				{
					if(scanResults[layer] == 4)
					{
						n4RegionLayerCount ++;
					}
				}

				if(n4RegionLayerCount >= n4RegionThreshold)
				{
					pDebug[row * nImageWidth + col] = 0xFF;

					*p32BitData |= (1<<uBitOffset);
				}

NEXT:
				uBitOffset++;
				if(uBitOffset == 32)
				{
					uBitOffset = 0;
					p32BitData++;

				}
                pCandidateDataCursor ++;
                pMaskDataCursor++;

			}//for-each(col);
		}//for-each(row)

		//
		//输出
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			Debug_SaveImageFrame(debugFrame, _T("Checker_Candidates.jpg"));
		}


		//<<debug
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			CImageFrame overlapFrame;
			overlapFrame = frame;
			OverlapImage(overlapFrame,debugFrame);
			Debug_SaveImageFrame(overlapFrame, _T("Checker_Overlap_Source.jpg"));
		}
		//debug>>


		//
		CBitFrame resultFrame;


		//噪声消除
        //消除孤立的目标点
		Erosion(bitFrame, 1, 0, resultFrame);

		
		//<<debug
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			pDebug      = debugFrame.GetData();
			p32BitData  = resultFrame.GetData();
			uBitOffset  = 0;


			for(UINT row=0; row<nImageHeight; row++)
			{
				//对每列循环
				for(UINT col=0; col<nImageWidth; col++)
				{


					*pDebug = (*p32BitData & (1<< uBitOffset))?0xFF:0x00;

					pDebug ++;

					uBitOffset ++;

					if(uBitOffset == 32)
					{
						uBitOffset = 0;
						p32BitData++;

					}
				}
			}



			Debug_SaveImageFrame(debugFrame, _T("Checkers_NoiseReduction.jpg"));
		}



		bitFrame = resultFrame;


        /*
		//膨胀操作
		//Dilate(bitFrame, 1, 1, resultFrame);


		//<<debug
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			pDebug      = debugFrame.GetData();
			p32BitData  = resultFrame.GetData();
			uBitOffset  = 0;


			for(UINT row=0; row<nImageHeight; row++)
			{
				//对每列循环
				for(UINT col=0; col<nImageWidth; col++)
				{


					*pDebug = (*p32BitData & (1<< uBitOffset))?0xFF:0x00;

					pDebug ++;

					uBitOffset ++;

					if(uBitOffset == 32)
					{
						uBitOffset = 0;
						p32BitData++;

					}
				}
			}

			Debug_SaveImageFrame(debugFrame, _T("Checkers_Dilation.bmp"));
		}*/



		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			CImageFrame overlapFrame;
			overlapFrame = frame;
			OverlapImage(overlapFrame,debugFrame);
			Debug_SaveImageFrame(overlapFrame, _T("Checker_Overlap_Source_After_Noise_Reduction.jpg"));
		}


		CConnectedComponentScanlineAlogrithm<> blobDetect;
		blobDetect.ProcessImage((const BYTE*)resultFrame.GetData(), resultFrame.Width(), resultFrame.Height());


		//
		CBitFrame edgeFrame;
		GetEdge(resultFrame, edgeFrame);


		//<<debug
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			pDebug       = debugFrame.GetData();
			p32BitData   = edgeFrame .GetData();
			uBitOffset   = 0;


			for(UINT row=0; row<nImageHeight; row++)
			{
				//对每列循环
				for(UINT col=0; col<nImageWidth; col++)
				{

					*pDebug = (*p32BitData & (1<< uBitOffset))? 0xFF:0x00;
					pDebug ++;

					uBitOffset ++;
					if(uBitOffset == 32)
					{
						uBitOffset = 0;
						p32BitData++;
					}
				}
			}

			Debug_SaveImageFrame(debugFrame, _T("Checker_Edge.jpg"));
		}
		//debug>>



		std::vector<TBlobObject> blobObjs;
		
		const TBlobObject* pObj = blobDetect.GetObjs();

		int nObjCount = blobDetect.GetObjCount();
		int nCandidateNum = nObjCount;
		for(int i= 0; i < nObjCount; i++)
		{
			//剔除长宽之比 > 2的目标
			int nWidth  = pObj[i].rcArea.right  - pObj[i].rcArea.left;
			int nHeight = pObj[i].rcArea.bottom - pObj[i].rcArea.top;

            
			//if(nCandidateNum > uExpectedCornerNumber)
			{
				if(nWidth == 0 || nHeight == 0)
				{
					nCandidateNum -- ;
					continue;
				}
				else if(nWidth > nHeight && nWidth*2 > nHeight*5)//W/L > 5/2
				{
					nCandidateNum -- ;
					continue;
				}
				else if(nWidth < nHeight && nHeight*2 > nWidth*5)//L/W > 5/2
				{
					nCandidateNum -- ;
					continue;
				}
			}

			blobObjs.push_back(pObj[i]);
			nCandidateNum -- ;
		}

		if(blobObjs.size() == 0)
		{
			return FALSE;
		}


		//<<
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			CImageFrame debugFrame2;
			debugFrame2 = debugFrame;
			for(UINT i= 0; i < blobObjs.size(); i++)
			{
				debugFrame2.DrawRectange(blobObjs[i].rcArea, 128);
			}
			Debug_SaveImageFrame(debugFrame2, _T("Checkers_Before_Merge.jpg"));
		}
		//>>

		//Merge(blobObjs, nGridWidth/2);
        Merge(blobObjs, edgeFrame, 3);//Modified by toxuke@gmail.com, 2012/08/16

		//Merge(blobObjs, edgeFrame, nDetectRadius/4);
		//Merge(blobObjs, edgeFrame,  5);//Modified by toxuke@gmail.com, 2011/12/26

        //

        //根据目标之间的相互距离进行比较来过滤虚目标
        FilterFalseObject(blobObjs, nDetectRadius);

        if(blobObjs.size() == 0) return FALSE;

		CHeapSortAlgorithm<TBlobObject> heapSort;


		//降序排列
		heapSort(&blobObjs[0], blobObjs.size(), false);


		if(blobObjs.size() < uExpectedCornerNumber)
		{
			bRet = FALSE;


    		for(UINT i=0; i<blobObjs.size(); i++)
			{
				debugFrame.DrawRectange(blobObjs[i].rcArea, 125);
                debugFrame.SetPixel(blobObjs[i].GPos, 0xFF);
			}


		}
		else
		{

			
            //几何中心
            m_vecCentroids.resize(uExpectedCornerNumber);
            m_vecCenters.resize(uExpectedCornerNumber);

			for(UINT i=0; i<uExpectedCornerNumber; i++)
			{

				//Modified by toxuke@gmail.com, 2013/04/11
				/*
                m_vecCentroids[i].x = blobObjs[i].GPos.x;//blobObjs[i].my/blobObjs[i].mass;
				m_vecCentroids[i].y = blobObjs[i].GPos.y;//blobObjs[i].mx/blobObjs[i].mass;

				*/
				m_vecCentroids[i].x =  (blobObjs[i].my << INT_SCALE_SHIFT_SIZE)/blobObjs[i].mass;
				m_vecCentroids[i].y  = (blobObjs[i].mx << INT_SCALE_SHIFT_SIZE)/blobObjs[i].mass;


               m_vecCenters[i].x = (blobObjs[i].rcArea.left  + blobObjs[i].rcArea.right ) /2;
               m_vecCenters[i].y = (blobObjs[i].rcArea.top   + blobObjs[i].rcArea.bottom) /2;

               //取几何中心和重心的加权平均值作为校正点坐标
               //m_vecCenters[i].x = (m_vecCenters[i].x + m_vecCentroids[i].x + 1)>>1;
               //m_vecCenters[i].y = (m_vecCenters[i].y + m_vecCentroids[i].y + 1)>>1;
               //测试结论: 加权中心不如几何中心。
               

				debugFrame.DrawRectange(blobObjs[i].rcArea, 125);
				//debugFrame.SetPixel(m_vecCentroids[i], 0xFF);
				debugFrame.SetPixel(blobObjs[i].GPos, 0xFF);
			}

		}

		//<<debug
		if(eDebugLevel == E_CALIB_DEBUG_LEVEL_DEBUG)
		{
			Debug_SaveImageFrame(debugFrame, _T("Checkers_Merged_And_Filtered.jpg"));
		}
		//debug>>

		return bRet;
	}




	//@参数:srcFrame,原始图片
	//       W, 侦测窗口半径
	//       nThreshold, 腐蚀标准,当像素的邻接点数目小于该值,则属于应该被腐蚀的像素。
	//       resultFrame, [输出],腐蚀后的结果图片
	//@说明:在2W+1尺寸的窗体中检测每个象素的邻居的个数, 个数小于门限值值则认为该象素为噪声点，在图像中剔除。
	void Erosion(const CBitFrame& srcFrame, int W, int nThreshold,  CBitFrame& resultFrame)
	{
		int nWidth  = srcFrame.Width();
		int nHeight = srcFrame.Height();

		resultFrame.SetSize(nWidth, nHeight);
		resultFrame.Clear();


		const DWORD* pSrcData     = srcFrame.GetData();
		const int dwordSizePerRow = nWidth>>5;
		DWORD* pResultData        = resultFrame.GetData();
		int nSrcBitOffset         = 0;


		for(int nRow=0; nRow<nHeight; nRow++)
		{
			for(int nCol = 0; nCol<nWidth; nCol++)
			{
				int nWindowPixelCount = 0;

				//像素值为1
				if(*pSrcData & (1<<nSrcBitOffset))
				{
					for(int r=-W; r <= W; r++)
					{
						int y = nRow + r;

						if(y<0 || y>=nHeight) continue;

						const DWORD* pRowCenter = pSrcData + r*dwordSizePerRow;

						for(int c=-W; c <= W; c++)
						{
							if(r == 0 && c ==0) continue;
							int x = nCol + c;
							if(x<0 || x>=nWidth) continue;

							const DWORD* pdwData = pRowCenter;
							int nWindowBitOffset = nSrcBitOffset + c;//双字内偏移

							if(nWindowBitOffset < 0)
							{
								nWindowBitOffset += 32;
								pdwData -- ;

							}
							else if(nWindowBitOffset >= 32)
							{
								nWindowBitOffset -=32;
								pdwData ++ ;
							}

							if(*pdwData & (1<<nWindowBitOffset))
							{
								nWindowPixelCount++;
							}

						}//for(c)
					}//for(r)

					if(nWindowPixelCount > nThreshold)
					{
						//只有原图像素为1的位置, 噪声消减后的图片的对应位置才为1。
						*pResultData |= 1<<nSrcBitOffset;

					}

				}


				nSrcBitOffset++;
				if(nSrcBitOffset == 32)
				{
					nSrcBitOffset = 0;
					pSrcData++;
					pResultData++;

				}


			}//for-each(col)
		}//for-each(row)

	}


	//@参数:srcFrame,原始图片
	//       W, 侦测窗口半径
	//       nThreshold,  膨胀标准,当非目标像素的邻接点数目大于等于该值，则像素为目标。
	//       resultFrame, [输出],膨胀后的结果图片
	//@说明:在2W+1尺寸的窗体中检测每个象素的邻居的个数, 个数小于门限值值则认为该象素为噪声点，在图像中剔除。
	void Dilate(const CBitFrame& srcFrame, int W, int nThreshold,  CBitFrame& resultFrame)
	{
		int nWidth  = srcFrame.Width();
		int nHeight = srcFrame.Height();

		
		resultFrame	= srcFrame;


		const DWORD* pSrcData     = srcFrame.GetData();
		const int dwordSizePerRow = nWidth>>5;
		DWORD* pResultData        = resultFrame.GetData();
		int nSrcBitOffset         = 0;


		for(int nRow=0; nRow<nHeight; nRow++)
		{
			for(int nCol = 0; nCol<nWidth; nCol++)
			{
				int nWindowPixelCount = 0;

				//像素值为0
				if((*pSrcData & (1<<nSrcBitOffset)) == 0)						
				{
					for(int r=-W; r <= W; r++)
					{
						int y = nRow + r;

						if(y<0 || y>=nHeight) continue;

						const DWORD* pRowCenter = pSrcData + r*dwordSizePerRow;

						for(int c=-W; c <= W; c++)
						{
							if(r == 0 && c ==0) continue;
							int x = nCol + c;
							if(x<0 || x>=nWidth) continue;							

							const DWORD* pdwData = pRowCenter;
							int nWindowBitOffset = nSrcBitOffset + c;//双字内偏移

							if(nWindowBitOffset < 0)
							{
								nWindowBitOffset += 32;
								pdwData -- ;

							}
							else if(nWindowBitOffset >= 32)
							{
								nWindowBitOffset -=32;
								pdwData ++ ;
							}

							if(*pdwData & (1<<nWindowBitOffset))
							{
								nWindowPixelCount++;
							}

						}//for(c)
					}//for(r)

					if(nWindowPixelCount >= nThreshold)
					{
						//只有原图像素为1的位置, 噪声消减后的图片的对应位置才为1。
						*pResultData |= 1<<nSrcBitOffset;

					}

				}


				nSrcBitOffset++;
				if(nSrcBitOffset == 32)
				{
					nSrcBitOffset = 0;
					pSrcData++;
					pResultData++;

				}


			}//for-each(col)
		}//for-each(row)

	}



	//
	//@功能:求取位图的轮廓
	void GetEdge(const CBitFrame& srcFrame, CBitFrame& edgeFrame)
	{
		int nWidth  = srcFrame.Width();
		int nHeight = srcFrame.Height();
		edgeFrame.SetSize(nWidth, nHeight);


		const DWORD* pSrcData     = srcFrame.GetData();
		const int dwordSizePerRow = nWidth>>5;
		DWORD* pEdgeData        = edgeFrame.GetData();
		int nSrcDWordBitOffset            = 0;


		for(int nRow = 0; nRow < nHeight; nRow++)
		{
			for(int nCol = 0; nCol < nWidth; nCol++)
			{

				int nNeighbourCount  = 0;
				int x,y;
				x = nCol;
				y = nRow;


				if(x-1 < 0  || x+1 >= nWidth || y+1>= nHeight || y-1 < 0)//剔除画面以外的点
				{

				}
				else
				{
					//8邻域检查
					for(int r = -1; r <= 1; r++)
					{
						const DWORD* pRowCenter = pSrcData + r*dwordSizePerRow;

						for(int c=-1; c <= 1; c++)
						{
							if(r == 0 && c == 0 ) continue;
							const DWORD* pdwData = pRowCenter;
							int   nEdgeDWordBitOffset = nSrcDWordBitOffset + c;//双字内偏移

							if(nEdgeDWordBitOffset < 0)
							{
								nEdgeDWordBitOffset += 32;
								pdwData -- ;
							}

							if(nEdgeDWordBitOffset >= 32)
							{
								nEdgeDWordBitOffset -= 32;
								pdwData++;
							}


							if(*pdwData & (1<< nEdgeDWordBitOffset))
							{
								nNeighbourCount++;
							}
						}//for-each(c)
					}//for-each(r)

				}

				if((*pSrcData & (1<<nSrcDWordBitOffset)) &&  nNeighbourCount < 8)//小于8邻域则为边界点
				{
					*pEdgeData |= (1<<nSrcDWordBitOffset);
				}

				nSrcDWordBitOffset++;

				if(nSrcDWordBitOffset == 32)
				{
					nSrcDWordBitOffset = 0;
					pSrcData++;
					pEdgeData++;

				}


			}//for_each(nCol)

		}//for_each(nRow)


	}



	//@功能:根据目标的重心距离进行合并
	//@参数:
	void Merge(std::vector<TBlobObject>& allObjs, int nMaxDist)
	{
		std::vector<TBlobObject>::iterator itCur = allObjs.begin();


		//计算重心
		for(;itCur != allObjs.end();itCur++)
		{
			itCur->GPos.x = itCur->my/itCur->mass;
			itCur->GPos.y = itCur->mx/itCur->mass;
			itCur->bIsValid = TRUE;
		}

		itCur = allObjs.begin();

		int nMaxDistSqure = nMaxDist*nMaxDist;
		for(;itCur != allObjs.end();itCur++)
		{		
			if(!itCur->bIsValid) continue;
			std::vector<TBlobObject>::iterator itCandidate = itCur ;
			while(1)
			{
				itCandidate++;
				if(itCandidate == allObjs.end())
				{
					break;
				}
				if(!itCandidate->bIsValid) continue;

				int nCur2CandiateDist = (itCandidate->GPos.x - itCur->GPos.x)*(itCandidate->GPos.x - itCur->GPos.x) + (itCandidate->GPos.y - itCur->GPos.y)*(itCandidate->GPos.y - itCur->GPos.y);

				if(nCur2CandiateDist < nMaxDistSqure)
				{


					BOOL bMerge = TRUE;
					//判断是否存在其它角点, 离候选合并角点更近
					std::vector<TBlobObject>::iterator itCmp = itCur ;

					while(1)
					{
						itCmp ++;
						if(itCmp == allObjs.end()) break;
						if(itCmp == itCur) continue;

						int nCmp2CandidateDist = (itCmp->GPos.x - itCandidate->GPos.x)*(itCmp->GPos.x - itCandidate->GPos.x) + (itCmp->GPos.y - itCandidate->GPos.y)*(itCmp->GPos.y - itCandidate->GPos.y);
						int nCmp2CurDist       = (itCmp->GPos.x - itCur->GPos.x)*(itCmp->GPos.x - itCur->GPos.x) + (itCmp->GPos.y - itCur->GPos.y)*(itCmp->GPos.y - itCur->GPos.y);

						if(nCmp2CurDist < nMaxDistSqure)
						{
							continue;
						}
						else
						{

							if(nCmp2CandidateDist < nCur2CandiateDist)
							{
								bMerge = FALSE;
								break;
							}
						}

					}



					if(bMerge)
					{

						itCur->mass += itCandidate->mass;
						itCur->mx   += itCandidate->mx;
						itCur->my   += itCandidate->my;

						if(itCur->rcArea.left > itCandidate->rcArea.left)
						{
							itCur->rcArea.left = itCandidate->rcArea.left;
						}

						if(itCur->rcArea.top > itCandidate->rcArea.top)
						{
							itCur->rcArea.top = itCandidate->rcArea.top;
						}


						if(itCur->rcArea.right < itCandidate->rcArea.right)
						{
							itCur->rcArea.right = itCandidate->rcArea.right;
						}


						if(itCur->rcArea.bottom < itCandidate->rcArea.bottom)
						{
							itCur->rcArea.bottom = itCandidate->rcArea.bottom;
						}

						//itCmp = allObjs.erase(itCmp);
						itCandidate->bIsValid = FALSE;
					}

				}//if			

			}//while
		}

		while(itCur != allObjs.end())
		{
			if(!itCur->bIsValid)
			{
				itCur = allObjs.erase(itCur);
			}
			else
			{
				itCur++;
			}
		}




		//重新计算重心
		itCur = allObjs.begin();
		for(;itCur != allObjs.end();itCur++)
		{
			itCur->GPos.x = itCur->my/itCur->mass;
			itCur->GPos.y = itCur->mx/itCur->mass;
		}

	}





	//@功能:根据目标边界之间的最小距离来合并目标
	void Merge(std::vector<TBlobObject>& allObjs, const CBitFrame& edgeFrame, int nMaxDist)
	{
		int nMaxDistSqure = nMaxDist*nMaxDist;
		const int MERGE_MASS_THRESHOLD = 50;

		UINT nObjCount = allObjs.size();
		
		std::vector<int> vecEdgeNeighours;//记录每个目标最近邻接点的数组

		vecEdgeNeighours.resize(nObjCount);

		for(std::vector<int>::size_type iObj = 0; iObj < nObjCount; iObj++)
		{
			vecEdgeNeighours[iObj] = -1;
			allObjs[iObj].bIsValid = TRUE;
		}//for


		//查找每个目标满足边界距离 < nMaxDist的最近邻接目标。
		for(std::vector<int>::size_type iObj = 0; iObj < nObjCount; iObj++)
		{
			int nMinEdgeDist2 = nMaxDistSqure;

			RECT& rcObj = allObjs[iObj].rcArea;
			for(std::vector<int>::size_type jCmpObj = 0; jCmpObj < vecEdgeNeighours.size(); jCmpObj++)
			{

				if(iObj == jCmpObj) continue;

				//进行区域边界距离比较
				int nDist2 = GetEdgeDist2(edgeFrame, allObjs[iObj].rcArea, allObjs[jCmpObj].rcArea);
				if(nDist2 < nMinEdgeDist2)
				{
					vecEdgeNeighours[iObj] = jCmpObj;
					nMinEdgeDist2 = nDist2;
				}
			}

		}//for-each(obj)


		//合并邻接目标
		for(std::vector<int>::size_type iObj = 0; iObj < nObjCount; iObj++)
		{
			if(vecEdgeNeighours[iObj] == -1) continue;

			int nMergeObjIndex = vecEdgeNeighours[iObj];//合并候选目标

			if(nMergeObjIndex == iObj)//以自己为邻接点, 说明一个"最近边界邻接"关系环结束,
			{
				continue;
			}
			TBlobObject& obj1  = allObjs[iObj];
			TBlobObject& obj2  = allObjs[nMergeObjIndex];

			assert(obj1.bIsValid);
			assert(obj2.bIsValid);


			if(obj1.mass >= MERGE_MASS_THRESHOLD && obj2.mass >= MERGE_MASS_THRESHOLD)
			{
				continue;
			}


			obj2.mass += obj1.mass;
			obj2.mx   += obj1.mx;
			obj2.my   += obj1.my;

			if(obj2.rcArea.left > obj1.rcArea.left)
			{
				obj2.rcArea.left = obj1.rcArea.left;
			}

			if(obj2.rcArea.top > obj1.rcArea.top)
			{
				obj2.rcArea.top = obj1.rcArea.top;
			}


			if(obj2.rcArea.right < obj1.rcArea.right)
			{
				obj2.rcArea.right = obj1.rcArea.right;
			}


			if(obj2.rcArea.bottom < obj1.rcArea.bottom)
			{
				obj2.rcArea.bottom = obj1.rcArea.bottom;
			}

			obj1.bIsValid = FALSE;
			vecEdgeNeighours[iObj] = -1;


			//调整以iObj为邻接点的目标的编号
			for(std::vector<int>::size_type jObj = iObj+1; jObj < nObjCount; jObj++)
			{
				if(vecEdgeNeighours[jObj] == iObj)
				{
					vecEdgeNeighours[jObj] = nMergeObjIndex;

					if(jObj == nMergeObjIndex)//形成"最近边界邻接"关系闭环，需要给关系闭环对应的点簇查找满足"最近边界邻接"关系的邻接点集
					{
						int nMinEdgeDist2 = nMaxDistSqure;
						vecEdgeNeighours[nMergeObjIndex] = -1;

						for(std::vector<int>::size_type kCmpObj = 0; kCmpObj < nObjCount; kCmpObj ++)
						{
							if(nMergeObjIndex == kCmpObj) continue;
							if(!allObjs[kCmpObj].bIsValid) continue;

							int nDist2 = GetEdgeDist2(edgeFrame, allObjs[nMergeObjIndex].rcArea, allObjs[kCmpObj].rcArea);
							if(nDist2 < nMinEdgeDist2)
							{

								vecEdgeNeighours[nMergeObjIndex] = kCmpObj;
								nMinEdgeDist2 = nDist2;
							}


						}

					}
				}

			}


		}

		std::vector<TBlobObject>::iterator itCur = allObjs.begin();

		while(itCur != allObjs.end())
		{
			if(!itCur->bIsValid)
			{
				itCur = allObjs.erase(itCur);
			}
			else
			{
				itCur++;
			}
		}




		//重新计算重心
		itCur = allObjs.begin();
		for(;itCur != allObjs.end();itCur++)
		{
			itCur->GPos.x = itCur->my/itCur->mass;
			itCur->GPos.y = itCur->mx/itCur->mass;
		}
	}

    //@功能:鉴于目标点之间的距离要大于角点侦测半径的特点,剔除虚目标
    void FilterFalseObject(std::vector<TBlobObject>& allObjs, int nMaxTargteDist)
    {
        
        const int DIST_MAX = nMaxTargteDist*nMaxTargteDist;

        //查找每个目标的距离平方小于DIST_MAX的邻接目标。
        std::vector<TBlobObject>::iterator itCur=allObjs.begin();

       
        while(itCur != allObjs.end())
        {

             BOOL bErase = FALSE;
            std::vector<TBlobObject>::iterator itComp = itCur + 1;
            for(;itComp != allObjs.end(); )
            {

                int nDist = (itCur->GPos.x - itComp->GPos.x)*(itCur->GPos.x - itComp->GPos.x) + (itCur->GPos.y - itComp->GPos.y)*(itCur->GPos.y- itComp->GPos.y);
                if(nDist < DIST_MAX)
                {
                    //两者之间删除质量小的目标
                    if(itCur->mass > itComp->mass)
                    {
                        itComp = allObjs.erase(itComp);
                    }
                    else
                    {
                        
                        bErase = TRUE;
                        break;//break from inner for loop;
                    }


                    //

                }
                else
                {
                   itComp++;
                }

            }
            if(bErase)
            {
                itCur = allObjs.erase(itCur);
            }
            else
            {
                itCur ++;
            }

        }






    }






	//
	//@功能:计算两个目标边界之间的最近距离的平方
	int GetEdgeDist2(const CBitFrame& edgeFrame, const RECT& rc1, const RECT& rc2) const
	{

		int nWidth = edgeFrame.Width();
		const int dwordSizePerRow = nWidth>>5;
		const DWORD* pEdgeData =  edgeFrame.GetData();
		int nMinDist2 = (std::numeric_limits<int>::max)();


		int nDWordBitOffset_1 = 0;
		int nDWordBitOffset_2 = 0;



		const DWORD* pRowData_1 = pEdgeData + rc1.top * dwordSizePerRow + (rc1.left >> 5);


		for(LONG r1 = rc1.top; r1 < rc1.bottom; r1++)
		{

			const DWORD* pColData_1  = pRowData_1;
			nDWordBitOffset_1 = rc1.left % 32;

			for(LONG c1 = rc1.left; c1 < rc1.right; c1++)
			{

				//目标1的区域为1的部分
				if(*pColData_1 & (1<< nDWordBitOffset_1))
				{

					const DWORD * pRowData_2 = pEdgeData + rc2.top * dwordSizePerRow + (rc2.left >> 5);

					for(LONG r2 = rc2.top; r2 < rc2.bottom; r2++)
					{

						const DWORD* pColData_2 = pRowData_2;
						nDWordBitOffset_2 = rc2.left % 32;
						for(LONG c2 = rc2.left; c2 < rc2.right; c2++)
						{
							if(*pColData_2 & (1<<nDWordBitOffset_2))
							{

								int nDist2 = (r1 - r2)*(r1 - r2) + (c1 - c2)*(c1 - c2);

								if(nMinDist2 > nDist2)
								{
									nMinDist2 = nDist2;
								}

							}


							nDWordBitOffset_2 ++;
							if(nDWordBitOffset_2 == 32)
							{
								nDWordBitOffset_2 -= 32;
								pColData_2 ++;
							}

						}//for_each(c2)

						pRowData_2 += dwordSizePerRow;

					}//for_each(r2)

				}

				nDWordBitOffset_1 ++;
				if(nDWordBitOffset_1 == 32)
				{
					nDWordBitOffset_1 -= 32;
					pColData_1++;
				}



			}//for_each(c1)

			pRowData_1 += dwordSizePerRow;

		}//for_each(r1)

		return nMinDist2;
	}




	const std::vector<POINT>& GetCentroids()const
	{
		return m_vecCentroids;
	}

    const std::vector<POINT>& GetCenters()const
	{
		return m_vecCenters;
	}

protected:
	//检测结果
    //各个校正点的重心结果
	std::vector<POINT> m_vecCenters;
    std::vector<POINT> m_vecCentroids;

};//