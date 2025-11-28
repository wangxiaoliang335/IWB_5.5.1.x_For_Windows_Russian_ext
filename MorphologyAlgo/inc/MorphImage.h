#pragma once

    //形态学处理的是二值化位图，使用1位位图来表示
    class CMorphImage
    {
    public:
        //@功能:从二值化灰度位图转化为单位位图
        //@参数:pSrcData,　源数据指针
        //      nWidth, 图片宽度
        //      nHeight,　图片高度
        //@说明:如果指向源数据的指针为空，则单位单位初始化为0
        CMorphImage(const BYTE* pSrcData, int nWidth, int nHeight)
        {
            Init(pSrcData, nWidth, nHeight);
        }


        CMorphImage(const CMorphImage& right)
        {

            m_nWidth  = right.m_nWidth;
            m_nHeight = right.m_nHeight;


            m_nSizeInDWord = (m_nWidth * m_nHeight + 16)>>5;//四舍五入计算双字数据尺寸。
            m_pdwBitData = new DWORD[m_nSizeInDWord]; 

            memcpy(m_pdwBitData, right.m_pdwBitData, sizeof(DWORD)*m_nSizeInDWord);
        }

        virtual ~CMorphImage()
        {

            delete [] m_pdwBitData;
        }



        void Init(const BYTE* pSrcData, int nWidth, int nHeight)
        {
            m_nWidth  = nWidth;
            m_nHeight = nHeight;


            m_nSizeInDWord = (nWidth * nHeight + 16)>>5;//四舍五入计算双字数据尺寸。
            m_pdwBitData = new DWORD[m_nSizeInDWord]; 

            if(pSrcData)
            {
                int nLoopCount = nWidth * nHeight;
                int nBitPos  = 0;
                DWORD dwData = 0x00000000;
                DWORD* pDWord = (DWORD*)m_pdwBitData;

                while(nLoopCount)
                {
                    if(*pSrcData == 0xFF)
                    {
                        dwData  |= 1<<nBitPos;
                    }

                    nBitPos ++;

                    //每32位写入一个双字的数据
                    if(nBitPos == 32)
                    {
                        nBitPos = 0;
                        *pDWord = dwData;
                        dwData = 0x00000000;
                        pDWord ++;
                    }
                    
                    pSrcData ++  ;
                    nLoopCount --;
                    
                }//while

                if(nBitPos != 0)
                {
                    *pDWord = dwData;
                }
            }
            else
            {
                memset(m_pdwBitData, 0, sizeof(DWORD)*m_nSizeInDWord);
            }


        }


        const BYTE*  GetBits()const
        {
            return (const BYTE*)m_pdwBitData;
        }

        BYTE* GetBits()
        {
           return(BYTE*) m_pdwBitData;
        }

        operator const BYTE*()const
        {
            return (const BYTE*)m_pdwBitData;
        }

        operator  BYTE*()
        {
            return (BYTE*)m_pdwBitData;
        }

        BOOL operator == (const CMorphImage& right)
        {
            if(this->m_nHeight != right.m_nHeight || this->m_nWidth != right.m_nWidth)
            {
                return 0;
            }

             return memcmp(this->m_pdwBitData, right.m_pdwBitData, this->m_nSizeInDWord * sizeof(DWORD)) == 0;
        }


        CMorphImage& operator =(const CMorphImage& right)
        {
            m_nWidth  = right.m_nWidth;
            m_nHeight = right.m_nHeight;


            m_nSizeInDWord = (m_nWidth * m_nHeight + 16)>>5;//四舍五入计算双字数据尺寸。
            m_pdwBitData = new DWORD[m_nSizeInDWord]; 

            memcpy(m_pdwBitData, right.m_pdwBitData, sizeof(DWORD)*m_nSizeInDWord);
            return *this;
        }


        CMorphImage operator& (const CMorphImage& right)
        {

            CMorphImage result(right);
            for(int i=0; i<m_nSizeInDWord; i++)
            {
                result.m_pdwBitData[i] =m_pdwBitData[i] &  right.m_pdwBitData[i];
            }

            return result;
        }


        int GetWidth()const
        {
            return m_nWidth;
        }

        int GetHeight()const
        {
            return m_nHeight;
        }


        void Complement()
        {
            DWORD* pDWord = m_pdwBitData;
            for(int i=0; i<m_nSizeInDWord; i++)
            {
                *pDWord = ~(*pDWord);
                pDWord ++;
            }
        }


        //
        BYTE GetPixel(int x, int y)const
        {
            int nDWordNo = 0;
            int nBitOffsetInDword = 0;

            if(y == 0)
            {
                nDWordNo = x >> 5;
                nBitOffsetInDword =  x & 0x0000001F;//offset = x % 32;
            }
            else 
            {
                int nBitCount = (y - 1) * m_nWidth + x;
                nDWordNo = nBitCount >> 5;
                nBitOffsetInDword = nBitCount & 0x00000001F;
            }

            DWORD* pDWord  = m_pdwBitData + nDWordNo;

           if(*pDWord & (1 << nBitOffsetInDword))
           {
                return 1;
           }

           return 0;
        }


        //
        void SetPixel(int x, int y, BYTE val)
        {
            int nDWordNo = 0;
            int nBitOffsetInDword = 0;

            if(y == 0)
            {
                nDWordNo = x >> 5;
                nBitOffsetInDword =  x & 0x000001F;
            }
            else 
            {
                int nBitCount = (y - 1) * m_nWidth + x;
                nDWordNo = nBitCount >> 5;
                nBitOffsetInDword = nBitCount & 0x0000001F;
            }

            DWORD* pDWord = (DWORD*)(m_pdwBitData) + nDWordNo;

            if(val)//置1
            {
                *pDWord |= 1 << nBitOffsetInDword;
            }
            else//复零
            {
                *pDWord &= ~(1 << nBitOffsetInDword);
            }
            
        }


        //@功能:转化为灰度图片
        //
        void ToGray(BYTE* pDstData)const
        {

            if(pDstData == NULL) return;
            
            int nLoopCount = m_nWidth * m_nHeight;
            int nBitPos  = 0;

             DWORD* pDWord = (DWORD*)m_pdwBitData;
             DWORD dwData = *pDWord;
             while(nLoopCount --)
             {
                 if(dwData & (1 << nBitPos ))
                 {
                      *pDstData = 0xFF;
                 }
                 else
                 {
                      *pDstData = 0x00;
                 }

                 nBitPos ++;
                 if(nBitPos == 32)
                 {
                    nBitPos = 0;
                    pDWord ++;
                    dwData = *pDWord;
                 }

                 pDstData ++;
            }//while

        }


        int GetDWordSize()const
        {
            return m_nSizeInDWord;
        }



        //@功能:膨胀操作
        //@参数:pKernel, 指向结构元素的指针
        //      nKernelWidth, 结构元素的宽
        //      nKernelHeight, 结构元素的高
        void Dilate(
            const BYTE* pKernel,
            int nKernelWidth,
            int nKernelHeight)
        {

            int nBitPos = 0;
            int nWordNo  = 0;

            int nBitOffset = 0;

            //第一行, 第一列
            if(m_pdwBitData[nWordNo] & (1 << nBitOffset))
            {

            }


            //第一行, 第二列至倒数第二列
            for(int c = 1; c < m_nWidth - 1; c++)
            {

            }

            //第一行, 最后一列


            //第一行和倒数第二行之间
            for(int r=1; r < m_nHeight-1; r++)
            {


                //第一列


                //
                for(int c=1; c < m_nWidth-1; c++)
                {


                }//for-each(col)

                //最后一列


            }//for-each(row)


            //最后一行, 第一列

           //最后一行, 第二列至倒数第二列


            //最后一行, 最后一列


        }


    protected:
        int   m_nWidth      ;
        int   m_nHeight     ;
        DWORD* m_pdwBitData    ;//1位位图数据
        INT    m_nSizeInDWord;//数据尺寸，以双字为单位

    };