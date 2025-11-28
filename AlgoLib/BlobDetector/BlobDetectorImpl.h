#pragma once

template<int MAX_OBJECT_COUNT=256, int MAX_IMAGE_WIDTH = 1920, int MAX_IMAGE_HEIGHT = 1080>
class BlobDetectorBase
{
public:
	virtual BOOL ProcessImage(const BYTE* pData, int nWidth, int nHeight) = 0;

	const TBlobObject* GetObjs() const 
	{
		return &m_aryObjInfo[0];
	}

	TBlobObject* GetObjs() 
	{
		return &m_aryObjInfo[0];
	}

    //
    //@功能:返回大于制定质量门限的目标的数目
    //@参数:nMassThreshold, 指定的质量门限, 缺省门限为0
    //@说明:如果质量门限设为0,则返回所有目标。
	UINT GetObjCount(unsigned int nMassThreshold =0) const
	{
        int nObjCount = m_nObjectCount;
        if(nMassThreshold != 0)
        {
            nObjCount = 0;
            UINT i=0;
            while(i < m_nObjectCount)
            {
                const TBlobObject& obj = m_aryObjInfo[i];
                if(obj.mass > nMassThreshold)
                {
                    nObjCount ++;
                }
                i ++;
            }
        }

		return nObjCount;
	}


    //@功能:依照设定的质量门限过滤目标
    void FilterObject(int nMassThreshold)
    {
        UINT i = 0;
        UINT nOldObjectCount = m_nObjectCount; 
        UINT nLoopCount = 0;
        while(nLoopCount < nOldObjectCount)
        {
            if(m_aryObjInfo[i].mass < nMassThreshold)
            {
               
                //总目标数-1
                m_nObjectCount --;

                 //从后往前挪动一个位置
                for(UINT j=i; j < m_nObjectCount; j++)
                {
                    m_aryObjInfo[j] = m_aryObjInfo[j+1];
                }

                //i 保持不变。
            }
            else
            {
                i++;
            }

            nLoopCount ++;
        };

    }

    //@功能:按照目标的质量大小来排序
    //@参数:bAscend, 升序排列
    virtual void Sort(BOOL bAscend) = 0;

protected:
	TBlobObject m_aryObjInfo[MAX_OBJECT_COUNT] ;//目标信息数组
	UINT         m_nObjectCount ;//目标个数
};


 #include "../inc/Algorithm/HeapSort.h"

//基于行扫描的目标捕获算法
//处理数据是1位位图
//template<int MAX_OBJECT_COUNT=256,int MAX_IMAGE_WIDTH = 640, int MAX_IMAGE_HEIGHT = 480>
template<int MAX_OBJECT_COUNT,int MAX_IMAGE_WIDTH, int MAX_IMAGE_HEIGHT>
class CConnectedComponentScanlineAlogrithm:public BlobDetectorBase<MAX_OBJECT_COUNT,MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT>
{

	struct TInternalObject;

	struct TSegment
	{
		UINT              StartX ;
		UINT              EndX   ;
		UINT              Length ;
		TInternalObject*  pObj   ;//关联的目标编号

	};

	template<class T>
	struct LinkNode
	{
		T val;
		LinkNode<T>* next;

	};

	//一行数据的扫描结果
	struct TScanLineResults
	{
		LinkNode<TSegment>    m_arySegments [(MAX_IMAGE_WIDTH +1)/2];
		UINT                  m_nSegmentCount;
	};



	//临时目标
	struct TInternalObject
	{
		//关联的新的线段链表
		LinkNode<TSegment>*  pNewSegmentListHead;
		LinkNode<TSegment>*  pNewSegmentListTail;

		//
		LinkNode<TSegment>*  pOldSegmentListHead;
		LinkNode<TSegment>*  pOldSegmentListTail;
		unsigned int m0;
		unsigned int mx;//对x轴的矩
		unsigned int my;//对y轴的矩
		RECT      rcBound;
		BOOL      bIsValid  ;//有效标志
		BOOL      bIsSeperated;//断裂成一个独立目标的标志


	};

protected:
	TInternalObject  m_aryInternalObjs[(MAX_IMAGE_WIDTH+1)/2];
	int              m_nInternalObjectCount;

	TInternalObject* m_aryInternalObjMemPool[(MAX_IMAGE_WIDTH+1)/2];
	int              m_nObjMemCount;


	void InitInternalObjMemPool()
	{
		TInternalObject* pObj = &m_aryInternalObjs[ _countof(m_aryInternalObjMemPool) - 1];
		int i=0;
		while(i < _countof(m_aryInternalObjMemPool))
		{
			m_aryInternalObjMemPool[i] = pObj --;
			i++;
		}

		m_nObjMemCount = _countof(m_aryInternalObjMemPool);
	}

	TInternalObject* AllocInternaObjMem()
	{
		TInternalObject* pObj = NULL;

		if(m_nObjMemCount)
		{
			m_nObjMemCount --;

			
			pObj = m_aryInternalObjMemPool[m_nObjMemCount];
			pObj->bIsValid = TRUE;//目标有效标志
			//pObj->newSegmentListHead.prev = NULL;
			//pObj->newSegmentListHead.next = &pObj->newSegmentListTail;
			//pObj->newSegmentListTail.prev = &pObj->newSegmentListHead;
			//pObj->newSegmentListTail.next = NULL;
			pObj->pNewSegmentListHead = pObj->pNewSegmentListTail = NULL;


			//pObj->oldSegmentListHead.prev = NULL;
			//pObj->oldSegmentListHead.next = &pObj->oldSegmentListTail;
			//pObj->oldSegmentListTail.prev = &pObj->oldSegmentListHead;
			//pObj->oldSegmentListTail.next = NULL;
			pObj->pOldSegmentListHead = pObj->pOldSegmentListTail = NULL;


			m_nInternalObjectCount ++;
		}

		return pObj;
	}

	void RecycleInternalObjMem(TInternalObject* pObjFree)
	{
		pObjFree->bIsValid = FALSE;//目标无效

        if(m_nObjMemCount >= _countof(m_aryInternalObjMemPool))
        {
            assert(false);
        }
        else
        {
    		m_aryInternalObjMemPool[m_nObjMemCount] = pObjFree;
	    	m_nObjMemCount ++;
    		m_nInternalObjectCount--;
        }
	}
public:
	virtual BOOL ProcessImage(const BYTE* pBits, int nWidth, int nHeight)
	{
	//	if (nWidth != 1280 || nHeight != 720)
	//	{
	//		DebugBreak();

	//	}
		//字节位串长度查找表, b3 b2 b1 b0 = 位串长度, b7=位串类型,1:'1'串, 0:'0'串
		static unsigned int LUT[256] = 
		{
			0x08,//00000000B,0 
			0x81,//00000001B,1 
			0x01,//00000010B,2 
			0x82,//00000011B,3 
			0x02,//00000100B,4 
			0x81,//00000101B,5 
			0x01,//00000110B,6 
			0x83,//00000111B,7 
			0x03,//00001000B,8 
			0x81,//00001001B,9 
			0x01,//00001010B,10 
			0x82,//00001011B,11 
			0x02,//00001100B,12 
			0x81,//00001101B,13 
			0x01,//00001110B,14 
			0x84,//00001111B,15 
			0x04,//00010000B,16 
			0x81,//00010001B,17 
			0x01,//00010010B,18 
			0x82,//00010011B,19 
			0x02,//00010100B,20 
			0x81,//00010101B,21 
			0x01,//00010110B,22 
			0x83,//00010111B,23 
			0x03,//00011000B,24 
			0x81,//00011001B,25 
			0x01,//00011010B,26 
			0x82,//00011011B,27 
			0x02,//00011100B,28 
			0x81,//00011101B,29 
			0x01,//00011110B,30 
			0x85,//00011111B,31 
			0x05,//00100000B,32 
			0x81,//00100001B,33 
			0x01,//00100010B,34 
			0x82,//00100011B,35 
			0x02,//00100100B,36 
			0x81,//00100101B,37 
			0x01,//00100110B,38 
			0x83,//00100111B,39 
			0x03,//00101000B,40 
			0x81,//00101001B,41 
			0x01,//00101010B,42 
			0x82,//00101011B,43 
			0x02,//00101100B,44 
			0x81,//00101101B,45 
			0x01,//00101110B,46 
			0x84,//00101111B,47 
			0x04,//00110000B,48 
			0x81,//00110001B,49 
			0x01,//00110010B,50 
			0x82,//00110011B,51 
			0x02,//00110100B,52 
			0x81,//00110101B,53 
			0x01,//00110110B,54 
			0x83,//00110111B,55 
			0x03,//00111000B,56 
			0x81,//00111001B,57 
			0x01,//00111010B,58 
			0x82,//00111011B,59 
			0x02,//00111100B,60 
			0x81,//00111101B,61 
			0x01,//00111110B,62 
			0x86,//00111111B,63 
			0x06,//01000000B,64 
			0x81,//01000001B,65 
			0x01,//01000010B,66 
			0x82,//01000011B,67 
			0x02,//01000100B,68 
			0x81,//01000101B,69 
			0x01,//01000110B,70 
			0x83,//01000111B,71 
			0x03,//01001000B,72 
			0x81,//01001001B,73 
			0x01,//01001010B,74 
			0x82,//01001011B,75 
			0x02,//01001100B,76 
			0x81,//01001101B,77 
			0x01,//01001110B,78 
			0x84,//01001111B,79 
			0x04,//01010000B,80 
			0x81,//01010001B,81 
			0x01,//01010010B,82 
			0x82,//01010011B,83 
			0x02,//01010100B,84 
			0x81,//01010101B,85 
			0x01,//01010110B,86 
			0x83,//01010111B,87 
			0x03,//01011000B,88 
			0x81,//01011001B,89 
			0x01,//01011010B,90 
			0x82,//01011011B,91 
			0x02,//01011100B,92 
			0x81,//01011101B,93 
			0x01,//01011110B,94 
			0x85,//01011111B,95 
			0x05,//01100000B,96 
			0x81,//01100001B,97 
			0x01,//01100010B,98 
			0x82,//01100011B,99 
			0x02,//01100100B,100 
			0x81,//01100101B,101 
			0x01,//01100110B,102 
			0x83,//01100111B,103 
			0x03,//01101000B,104 
			0x81,//01101001B,105 
			0x01,//01101010B,106 
			0x82,//01101011B,107 
			0x02,//01101100B,108 
			0x81,//01101101B,109 
			0x01,//01101110B,110 
			0x84,//01101111B,111 
			0x04,//01110000B,112 
			0x81,//01110001B,113 
			0x01,//01110010B,114 
			0x82,//01110011B,115 
			0x02,//01110100B,116 
			0x81,//01110101B,117 
			0x01,//01110110B,118 
			0x83,//01110111B,119 
			0x03,//01111000B,120 
			0x81,//01111001B,121 
			0x01,//01111010B,122 
			0x82,//01111011B,123 
			0x02,//01111100B,124 
			0x81,//01111101B,125 
			0x01,//01111110B,126 
			0x87,//01111111B,127 
			0x07,//10000000B,128 
			0x81,//10000001B,129 
			0x01,//10000010B,130 
			0x82,//10000011B,131 
			0x02,//10000100B,132 
			0x81,//10000101B,133 
			0x01,//10000110B,134 
			0x83,//10000111B,135 
			0x03,//10001000B,136 
			0x81,//10001001B,137 
			0x01,//10001010B,138 
			0x82,//10001011B,139 
			0x02,//10001100B,140 
			0x81,//10001101B,141 
			0x01,//10001110B,142 
			0x84,//10001111B,143 
			0x04,//10010000B,144 
			0x81,//10010001B,145 
			0x01,//10010010B,146 
			0x82,//10010011B,147 
			0x02,//10010100B,148 
			0x81,//10010101B,149 
			0x01,//10010110B,150 
			0x83,//10010111B,151 
			0x03,//10011000B,152 
			0x81,//10011001B,153 
			0x01,//10011010B,154 
			0x82,//10011011B,155 
			0x02,//10011100B,156 
			0x81,//10011101B,157 
			0x01,//10011110B,158 
			0x85,//10011111B,159 
			0x05,//10100000B,160 
			0x81,//10100001B,161 
			0x01,//10100010B,162 
			0x82,//10100011B,163 
			0x02,//10100100B,164 
			0x81,//10100101B,165 
			0x01,//10100110B,166 
			0x83,//10100111B,167 
			0x03,//10101000B,168 
			0x81,//10101001B,169 
			0x01,//10101010B,170 
			0x82,//10101011B,171 
			0x02,//10101100B,172 
			0x81,//10101101B,173 
			0x01,//10101110B,174 
			0x84,//10101111B,175 
			0x04,//10110000B,176 
			0x81,//10110001B,177 
			0x01,//10110010B,178 
			0x82,//10110011B,179 
			0x02,//10110100B,180 
			0x81,//10110101B,181 
			0x01,//10110110B,182 
			0x83,//10110111B,183 
			0x03,//10111000B,184 
			0x81,//10111001B,185 
			0x01,//10111010B,186 
			0x82,//10111011B,187 
			0x02,//10111100B,188 
			0x81,//10111101B,189 
			0x01,//10111110B,190 
			0x86,//10111111B,191 
			0x06,//11000000B,192 
			0x81,//11000001B,193 
			0x01,//11000010B,194 
			0x82,//11000011B,195 
			0x02,//11000100B,196 
			0x81,//11000101B,197 
			0x01,//11000110B,198 
			0x83,//11000111B,199 
			0x03,//11001000B,200 
			0x81,//11001001B,201 
			0x01,//11001010B,202 
			0x82,//11001011B,203 
			0x02,//11001100B,204 
			0x81,//11001101B,205 
			0x01,//11001110B,206 
			0x84,//11001111B,207 
			0x04,//11010000B,208 
			0x81,//11010001B,209 
			0x01,//11010010B,210 
			0x82,//11010011B,211 
			0x02,//11010100B,212 
			0x81,//11010101B,213 
			0x01,//11010110B,214 
			0x83,//11010111B,215 
			0x03,//11011000B,216 
			0x81,//11011001B,217 
			0x01,//11011010B,218 
			0x82,//11011011B,219 
			0x02,//11011100B,220 
			0x81,//11011101B,221 
			0x01,//11011110B,222 
			0x85,//11011111B,223 
			0x05,//11100000B,224 
			0x81,//11100001B,225 
			0x01,//11100010B,226 
			0x82,//11100011B,227 
			0x02,//11100100B,228 
			0x81,//11100101B,229 
			0x01,//11100110B,230 
			0x83,//11100111B,231 
			0x03,//11101000B,232 
			0x81,//11101001B,233 
			0x01,//11101010B,234 
			0x82,//11101011B,235 
			0x02,//11101100B,236 
			0x81,//11101101B,237 
			0x01,//11101110B,238 
			0x84,//11101111B,239 
			0x04,//11110000B,240 
			0x81,//11110001B,241 
			0x01,//11110010B,242 
			0x82,//11110011B,243 
			0x02,//11110100B,244 
			0x81,//11110101B,245 
			0x01,//11110110B,246 
			0x83,//11110111B,247 
			0x03,//11111000B,248 
			0x81,//11111001B,249 
			0x01,//11111010B,250 
			0x82,//11111011B,251 
			0x02,//11111100B,252 
			0x81,//11111101B,253 
			0x01,//11111110B,254 
			0x88,//11111111B,255 
		};

		//TDashLine aryDashLines[2][IMAGE_MAX_WIDTH/2 + 1];//最多只可能有IMAGE_MAX_WIDTH/2 + 1个dash line对象

		m_nObjectCount = 0;
		InitInternalObjMemPool();
		m_nInternalObjectCount = 0;

		const DWORD* pdwBits = reinterpret_cast<const DWORD*>(pBits);


		//int nImageWidth      = frame.Width ();
		//UINT nLastDashLineAryNo    = 0;//记录上一条扫描线线段信息的数组编号(0或者1),每处理一次在0,1中切换
		//UINT nNewDashLineAryNo     = 1;//记录当前扫描线线段信息的数组编号(0或者1),每处理一次在0,1中切换
		//UINT nLastDashLineCount    = 0;

		//int nUpdateDashLineAryNo  = 1;
		//m_nObjectCount            = 0;//目标个数复位

		//UINT aryRecycleObj[MAX_OBJECT_COUNT];//回收的目标编号
		//UINT nRecycledObjCount = 0        ;//回收的目标个数

		//每行的双字尺寸
		unsigned int nRowSizeInDWord = (nWidth + 31) >> 5;

		////当前扫描线的DashLine的计数器复位
		//int nNewDashLineCount = 0;
		//int nX = 0;

		TScanLineResults scanline[2];
		scanline[0].m_nSegmentCount = 0;
		scanline[1].m_nSegmentCount = 0;
		TScanLineResults* pLastScanLine =  &scanline[1];
		TScanLineResults* pCurScanLine  =  &scanline[0];



		for(int row = 0; row < nHeight; row ++)
		{

			//Step1: 检索每行扫描线中的'1'串。
			LinkNode<TSegment>* pNewSegmentNode = &pCurScanLine->m_arySegments[-1];//初始指向第[-1]个元素
			TSegment* pSegment = NULL;

			int nSegmentCount  = 0;
			BOOL bBreak = TRUE;//断裂标记
			int nX = 0;
			//检索一条扫描线的'1'串
			for(unsigned int i=0; i < nRowSizeInDWord; i++)
			{
				//一次读入四个字节，以便减少内存访问时间
				DWORD dwData = *pdwBits++;

				//因为图像中绝大多数的区域为0，一次处理32个像素，提高处理速度。
				if(0x00000000 == dwData)
				{
					bBreak = TRUE;
					nX += 32;
					continue;
				}
				else if(0xFFFFFFFF == dwData)
				{
					if(bBreak)
					{
						pNewSegmentNode ++;
						nSegmentCount ++;
						pSegment = &pNewSegmentNode->val;

						pSegment->Length = 32;
						pSegment->StartX = nX;
						pSegment->pObj =  NULL;
						bBreak = FALSE;

					}
					else
					{
						pSegment->Length += 32;
					}
					nX +=32;
					continue;
				}
				else
				{
					for(unsigned int j=0; j< 4;j++)
					{
						BYTE cData = dwData & 0xFF;
						if(0x00 == cData)
						{
							bBreak = TRUE;
							nX += 8;
						}
						else if(0xFF == cData)
						{
							if(bBreak)
							{
								pNewSegmentNode ++;
								nSegmentCount ++;
								pSegment = &pNewSegmentNode->val;

								pSegment->Length = 8;
								pSegment->StartX = nX;
								pSegment->pObj =  NULL;
								bBreak = FALSE;

							}
							else
							{
								pSegment->Length += 8;
							}
							nX +=8;

						}
						else
						{
							int  shiftCount = 0;

							while(cData)//注意:cData=0时，查表得出的结果是8，导致nX计数错误。
							{
								//通过查表来计算字节中连续'1'串或者'0'串的长度
								int L     = LUT[cData] & 0x0F;
								int type  = LUT[cData] & 0x80;

								if(type)//'1'串
								{
									if(bBreak)
									{
										pNewSegmentNode ++;
										nSegmentCount ++;
										pSegment = &pNewSegmentNode->val;
										pSegment->Length  = L;
										pSegment->StartX  = nX;
										pSegment->pObj =  NULL;
										bBreak = FALSE;

									}
									else
									{
										pSegment->Length += L;
									}

								}
								else//遇到'0'串
								{
									bBreak = TRUE;
								}

								nX += L;
								cData >>= L;
								shiftCount += L;
							}//while

							if(shiftCount < 8)
							{
								bBreak = TRUE;
								nX += 8 - shiftCount;
							}

						}

						dwData >>=8;
					}//for(j)
				}//else

			}//for(i)

			pCurScanLine->m_nSegmentCount = nSegmentCount;

			//bool bDebug = true;
			//if (pCurScanLine->m_nSegmentCount > 320)
			//{
			//	bDebug = false;
			//}

			//Step2.开始生成、归并临时目标
			if(!MergeScanLine(row, pCurScanLine,pLastScanLine))
			{
				//return FALSE;
				break;

			}

			//Step3.交换上一行线和当前行扫描线的指针
			TScanLineResults*  pTemp = pLastScanLine;
			pLastScanLine = pCurScanLine;
			pCurScanLine = pTemp;



		}//for-each-row

		//最后剩下的内部目标全部是目标
		if(this->m_nInternalObjectCount)
		{
			int nObjCount = m_nInternalObjectCount;
			TInternalObject* pInternalObj = &m_aryInternalObjs[0];

			while(nObjCount)
			{
				if(pInternalObj->bIsValid)
				{
						//if(m_nObjectCount < _countof(m_aryObjInfo))
						//{
						//	TBlobObject* pObjInfo = &m_aryObjInfo[m_nObjectCount];

						//	pObjInfo->mass       = pInternalObj->m0;
						//	pObjInfo->mx         = pInternalObj->mx;
						//	pObjInfo->my         = pInternalObj->my;
						//	pObjInfo->rcArea     = pInternalObj->rcBound;

						//	RecycleInternalObjMem(pInternalObj);

						//	m_nObjectCount ++;
						//}
                    StoreAsObject(*pInternalObj);
                    RecycleInternalObjMem(pInternalObj);

					nObjCount--;
				}


				pInternalObj ++;
		}//while
	 }

        //计算各个目标的重心
        for(UINT i=0 ;i < m_nObjectCount; i++)
        {
           TBlobObject* pObjInfo  =  &m_aryObjInfo[i];
           pObjInfo->GPos.x = (pObjInfo->my + pObjInfo->mass/2) /pObjInfo->mass;
           pObjInfo->GPos.y = (pObjInfo->mx + pObjInfo->mass/2) /pObjInfo->mass;

        }

        Sort(FALSE);//降序排列

		return TRUE;
	}//function ProcessImage

    //@功能:按照目标的质量大小来排序
    //@参数:bAscend, 升序排列
    virtual void Sort(BOOL bAscend)
    {
      CHeapSortAlgorithm<TBlobObject> heapSort;

      //堆排序
      heapSort(&this->m_aryObjInfo[0], this->m_nObjectCount, bAscend?true:false);
    }

protected:


	BOOL MergeScanLine(int row, TScanLineResults* pCurScanLine, TScanLineResults* pLastScanLine)
	{
		int nObjCount = this->m_nInternalObjectCount;
		TInternalObject* pInternalObj = &m_aryInternalObjs[0];
		while(nObjCount)
		{
			if(pInternalObj->bIsValid)
			{
				pInternalObj->bIsSeperated = TRUE;
				nObjCount --;

			}
			pInternalObj ++;
		}


		if(0 == pLastScanLine->m_nSegmentCount)
		{
			//所有扫描线中的'1'串自动生成为临时目标
			for(UINT i=0; i < pCurScanLine->m_nSegmentCount; i++)
			{

				TInternalObject* pObj = AllocInternaObjMem();
				if(pObj == NULL) return FALSE;
				LinkNode<TSegment>& node = pCurScanLine->m_arySegments[i];
				TSegment& s = node.val;
				s.EndX = s.StartX + s.Length;
				pObj->m0 = s.Length;
				pObj->mx = s.Length * row ;//对X轴的一阶矩
				pObj->my = (s.Length * (s.StartX + s.EndX - 1))>>1;//对Y轴的一阶矩

				RECT& rcBound = pObj->rcBound;
				rcBound.left   = s.StartX;
				rcBound.right  = s.EndX;
				rcBound.top    = row;
				rcBound.bottom = row + 1;

				//将s加入关联的短线链表
				node.next = NULL;
				pObj->pNewSegmentListHead = &node;
				pObj->pNewSegmentListTail = &node;
				pObj->bIsSeperated = FALSE;

				s.pObj = pObj;

					//<<debug
					LinkNode<TSegment>* pDebugNode = pObj->pOldSegmentListHead;
					while(pDebugNode)
					{
						pDebugNode = pDebugNode->next;
						assert(pDebugNode != pObj->pOldSegmentListHead);
					}
					//debug>>
			}

		}
		else//合并和生成操作
		{

			unsigned int  CmpIndex =0;//上条扫描线的'1'串的索引号

			for(UINT i=0;i < pCurScanLine->m_nSegmentCount; i++)
			{

				LinkNode<TSegment>* pCurSegmentNode = &pCurScanLine->m_arySegments[i];
				TSegment* pCurSegment = &pCurSegmentNode->val;

				BOOL bAdjacent = FALSE;

				int nCurStartX = pCurSegment->StartX;
				int nCurEndX   = pCurSegment->EndX = nCurStartX + pCurSegment->Length;

				int  nFirstAdjacentSegmentIndex = CmpIndex;//第一条与当前短线邻接的上一条扫描线中的短线的索引下标
				int  nAdjacentSegmentCount      = 0;//与当前短线邻接的上一条扫描线中的短线的数目

				while(CmpIndex < pLastScanLine->m_nSegmentCount)
				{
					LinkNode<TSegment>* pCmpSegmentNode = &pLastScanLine->m_arySegments[CmpIndex];
					TSegment* pCmpSegment = &pCmpSegmentNode->val;

					int nCmpStartX = pCmpSegment->StartX;
					int nCmpEndX  = pCmpSegment->EndX;

					if(nCurEndX < nCmpStartX)
					{
						//不邻接情形1
						//Last              □□□□□
						//Now  ■■■■■■
						break;
					}
					else if(nCurEndX < nCmpEndX)
					{
						nAdjacentSegmentCount++;
						bAdjacent = true;

						//邻接情形1
						//Last              □□□□□
						//Now  ■■■■■■■■■■


						//邻接情形2
						//Last  □□□□□□□□□□
						//Now     ■■■■■
						break;

					}
					else
					{
						if(nCurStartX <= nCmpEndX)
						{
							//邻接情形3
							//Last     □□
							//Now     ■■■■■
							CmpIndex++;
							nAdjacentSegmentCount++;
							bAdjacent = true;
							continue;
						}
						else
						{
							//不邻接情形2
							//Last     □□
							//Now          ■■■■■
							
							CmpIndex++;
							nFirstAdjacentSegmentIndex = CmpIndex;
							
							continue;
						}

					}

				}//while()


				//满足邻接关系
				if(bAdjacent)
				{

					LinkNode<TSegment>* pAdjacentSegmentNode = &pLastScanLine->m_arySegments[nFirstAdjacentSegmentIndex];
					TSegment* pAdjacentSegment = &pAdjacentSegmentNode->val;
					TInternalObject* pObjMerged  = pAdjacentSegment->pObj;

					while(-- nAdjacentSegmentCount)
					{
						pAdjacentSegmentNode ++;
						pAdjacentSegment = &pAdjacentSegmentNode->val;
						TInternalObject* pObjDeleted = pAdjacentSegment->pObj;
						if(pObjDeleted == pObjMerged)
						{
							continue;//两条线段关联的目标相同，则不作合并操作。
						}

						if(pObjDeleted < pObjMerged)
						{
							TInternalObject* temp = pObjMerged;
							pObjMerged  = pObjDeleted;
							pObjDeleted = temp;

						}

						//更新新的关联短线关联的目标指针。
						LinkNode<TSegment>* pNode = pObjDeleted->pNewSegmentListHead;
						while(pNode)
						{

							pNode->val.pObj = pObjMerged;
							pNode = pNode->next;
						}

						//更新旧的关联短线关联的目标指针。
						pNode = pObjDeleted->pOldSegmentListHead;
						while(pNode)
						{

							pNode->val.pObj = pObjMerged;
							pNode = pNode->next;
						}

						//旧的关联短线链表合并
						pObjMerged->pOldSegmentListTail->next = pObjDeleted->pOldSegmentListHead;
						pObjMerged->pOldSegmentListTail = pObjDeleted->pOldSegmentListTail;



						//新的关联短线链表合并
						if(pObjMerged->pNewSegmentListTail)
						{
							//查找此处Bug，花费8小时, 2013/03/09
							//注意到:删除目标的新的关联短线链表可能为空, 此种情况下不能够更新合并目标中的关联短线链表
							if(pObjDeleted->pNewSegmentListTail)
							{
								pObjMerged->pNewSegmentListTail->next = pObjDeleted->pNewSegmentListHead;
								pObjMerged->pNewSegmentListTail       = pObjDeleted->pNewSegmentListTail;
							}
						}
						else
						{
							pObjMerged->pNewSegmentListHead = pObjDeleted->pNewSegmentListHead;
							pObjMerged->pNewSegmentListTail = pObjDeleted->pNewSegmentListTail;
						}
						
						//
						pObjMerged->m0 += pObjDeleted->m0;
						pObjMerged->mx += pObjDeleted->mx;
						pObjMerged->my += pObjDeleted->my;

						MergeRect(&pObjMerged->rcBound, &pObjDeleted->rcBound);

						//回收删除的目标的内存空间
						RecycleInternalObjMem(pObjDeleted);
					}


					//并将质量和矩加入目标
					pObjMerged->m0 += pCurSegment->Length;
					pObjMerged->mx += pCurSegment->Length * row ;//对X轴的一阶矩
					pObjMerged->my += (pCurSegment->Length * (nCurStartX + nCurEndX - 1))>>1;//对Y轴的一阶矩
					pCurSegment->pObj = pObjMerged;


					if(pObjMerged->rcBound.left    > nCurStartX) pObjMerged->rcBound.left   = nCurStartX;
					if(pObjMerged->rcBound.right   < nCurEndX  ) pObjMerged->rcBound.right  = nCurEndX;
					if(pObjMerged->rcBound.bottom <= row       ) pObjMerged->rcBound.bottom = row+1;


					//将短线加入到目标的关联短线链表中去
					if(pObjMerged->pNewSegmentListTail)
					{
						pCurSegmentNode->next = pObjMerged->pNewSegmentListHead;
						pObjMerged->pNewSegmentListHead = pCurSegmentNode;
					}
					else
					{
						pCurSegmentNode->next = NULL;
						pObjMerged->pNewSegmentListHead = pObjMerged->pNewSegmentListTail = pCurSegmentNode;
					}
					

					pObjMerged->bIsSeperated = FALSE;

				}
				else//不邻接
				{

					//当前’1’串关联的目标为空,则生成新的目标
					//assert(NULL == pCurSegment->pObj);

					TInternalObject* pObj = AllocInternaObjMem();
					if(pObj == NULL) return FALSE;

					pObj->m0 = pCurSegment->Length;
					pObj->mx = pCurSegment->Length * row ;//对X轴的一阶矩
					pObj->my = (pCurSegment->Length * (pCurSegment->StartX + pCurSegment->EndX - 1))>>1;//对Y轴的一阶矩

					RECT& rcBound = pObj->rcBound;
					rcBound.left   = pCurSegment->StartX;
					rcBound.right  = pCurSegment->EndX;
					rcBound.top    = row;
					rcBound.bottom = row + 1;
					

					pCurSegment->pObj = pObj;
					//将短线加入到目标的关联短线链表中去
					pCurSegmentNode->next = NULL;
					pObj->pNewSegmentListHead = pCurSegmentNode;
					pObj->pNewSegmentListTail = pCurSegmentNode;
					pObj->bIsSeperated = FALSE;


				}


			}//foreach segment found
		}//else



		//交换目标关联的扫描线信息
		nObjCount = m_nInternalObjectCount;
		pInternalObj = &m_aryInternalObjs[0];
		while(nObjCount)
		{
			if(pInternalObj->bIsValid)
			{
				if(pInternalObj->bIsSeperated)//分裂成独立的目标了，
				{
                    /*
					if(m_nObjectCount < _countof(m_aryObjInfo))
					{
						TBlobObject* pObjInfo = &m_aryObjInfo[m_nObjectCount];

						pObjInfo->mass       = pInternalObj->m0;
						pObjInfo->mx         = pInternalObj->mx;
						pObjInfo->my         = pInternalObj->my;
						pObjInfo->rcArea     = pInternalObj->rcBound;
                        pObjInfo->bIsValid   = TRUE;

						RecycleInternalObjMem(pInternalObj);

						m_nObjectCount ++;

					}
					else
					{
						return FALSE;
					}
                    */
                    StoreAsObject(*pInternalObj);
                    RecycleInternalObjMem(pInternalObj);
				}
				else
				{
					pInternalObj->pOldSegmentListHead = pInternalObj->pNewSegmentListHead;
					pInternalObj->pOldSegmentListTail = pInternalObj->pNewSegmentListTail;

					pInternalObj->pNewSegmentListHead = NULL;
					pInternalObj->pNewSegmentListTail = NULL;

				}

				nObjCount -- ;

			}
			pInternalObj ++;
		}//while();



		return TRUE;

	}


	void MergeRect(RECT* pRectLeft, const RECT* pRectRight)
	{
		if(pRectLeft->left > pRectRight->left)
		{
			pRectLeft->left  = pRectRight->left;
		}

		if(pRectLeft->right < pRectRight->right)
		{
			pRectLeft->right = pRectRight->right;
		}

		if(pRectLeft->top > pRectRight->top)
		{
			pRectLeft->top  = pRectRight->top;
		}

		if(pRectLeft->bottom < pRectRight->bottom)
		{
			pRectLeft->bottom = pRectRight->bottom;
		}

	}


    //@功能:将新找到的目标的质量插入排序到数组m_aryObjInfo中去
    //      数组的元素按照目标质量大小降序排列。
    //@说明:如果图片中有很多个小目标, 使得目标总数大于MAX_OBJECT_COUNT, 
    //      插入排序的最终效果是只保留质量最大的MAX_OBJECT_COUNT个目标
    void StoreAsObject(const TInternalObject& internalObject)
    {
        UINT nInsertPos = 0;
        
        //从前往后搜索插入位置, 直到新目标的质量大于元素的质量
        for (nInsertPos = 0; nInsertPos < m_nObjectCount; nInsertPos++)
        {
            const TBlobObject &obj =  m_aryObjInfo[nInsertPos];
            if (internalObject.m0 > obj.mass)
            {
                break;
            }
            else
            {
                continue;
            }

        }//for

        if (nInsertPos == m_nObjectCount)
        {//最为最末一个元素插入

            if (m_nObjectCount < MAX_OBJECT_COUNT)
            {
                TBlobObject& objInfo = m_aryObjInfo[m_nObjectCount];
                objInfo.mass     = internalObject.m0;
                objInfo.mx       = internalObject.mx;
                objInfo.my       = internalObject.my;
                objInfo.rcArea   = internalObject.rcBound;
                objInfo.bIsValid = TRUE;
                m_nObjectCount ++;
            }

        }//if
        else
        {//
            int nLastPos = m_nObjectCount;

            if (nLastPos >= MAX_OBJECT_COUNT)
            {
                nLastPos = MAX_OBJECT_COUNT - 1;
            }
            else
            {
                m_nObjectCount++;
            }

            //m_aryObjInfo[InsertPos .. m_nObjectCount-1]之间的元素往后挪动一个位置
            for (unsigned int pos = nLastPos; pos > nInsertPos; pos--)
            {
                m_aryObjInfo[pos] = m_aryObjInfo[pos - 1];
            }

            TBlobObject& objInfo = m_aryObjInfo[nInsertPos];
            objInfo.mass     = internalObject.m0;
            objInfo.mx       = internalObject.mx;
            objInfo.my       = internalObject.my;
            objInfo.rcArea   = internalObject.rcBound;
            objInfo.bIsValid = TRUE;
        }//else

		if (m_nObjectCount == 0)
		{
			int ddddd = 0;
		}
    }



};