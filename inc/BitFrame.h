//#pragma once
#ifndef BITFRAME_H_INCLUDE
#define BITFRAME_H_INCLUDE

class CBitFrame
{
public:
    CBitFrame():
      m_lRef(1),
          m_pData(NULL),
          m_nDataSizeInDword(0),
          m_nWidth(0),
          m_nHeight(0)
      {

      }


      ~CBitFrame()
      {

          if(m_pData)
          {
              //delete[] m_pData;
              _aligned_free(m_pData);
          }

      }


      LONG AddRef()
      {

          InterlockedIncrement(&m_lRef);
          return m_lRef;
      }

      LONG Release()
      {
          LONG lRef = 0;	
          InterlockedDecrement(&m_lRef);

          lRef = m_lRef;
          if(m_lRef == 0)
          {
              delete this;
          }

          return lRef;
      }

      LONG RefCount()const
      {
          return m_lRef;
      }

      int Width()const
      {
          return m_nWidth;
      }

      int Height() const
      {
          return m_nHeight;
      }


      const DWORD* GetData()const
      {
          return m_pData;
      }

      DWORD* GetData()
      {
          return m_pData;
      }

      int Size()const
      {
          return m_nDataSizeInDword;

      }

      //@功能:返回像素个数
      int GetPixelCount() const
      {
        return m_nWidth * m_nHeight;
      }

      void SetSize(int nWidth, int nHeight)
      {
          //int nRowDWordSize = nWidth + (32 - 1)/32;
          //int nNewDataSize =  nWidth * nRowDWordSize;
          int nNewDataSize = (nWidth*nHeight) >> 5;

          if(m_pData)
          {

              if(nNewDataSize > m_nDataSizeInDword)
              {
                  //delete[] m_pData;
                  _aligned_free(m_pData);

                  m_nDataSizeInDword = nNewDataSize;
                  m_pData=  (DWORD*)_aligned_malloc(m_nDataSizeInDword*sizeof(DWORD), 32);//32字节对齐分配内存空间,以便使用SIMD指令
                  memset(m_pData, 0, sizeof(DWORD)*nNewDataSize);
              }

          }
          else
          {

              m_nDataSizeInDword = nNewDataSize;
              m_pData=  (DWORD*)_aligned_malloc(m_nDataSizeInDword*sizeof(DWORD), 32);//32字节对齐分配内存空间,以便使用SIMD指令
              memset(m_pData, 0, sizeof(DWORD)*nNewDataSize);

          }


          m_nWidth    = nWidth;
          m_nHeight   = nHeight;


      }


      //int Write(const BYTE* pData, int nWidth, int nHeight)
      //{	

      // SetSize(nWidth, nHeight);
      //return Write(pData, m_nDataSize);
      //}



      //int Write(const WORD* pData, int nSize)
      //{

      // if(pData == NULL) return 0;

      // if(nSize > m_nDataSize)
      // {
      //  nSize = m_nDataSize;
      // }

      // memcpy(m_pData, pData, nSize);
      // return  nSize;
      //}

      BOOL IsFree() const
      {

          return m_lRef == 1;
      }


      void Clear()
      {

          memset(m_pData, 0, m_nDataSizeInDword * sizeof(DWORD));
      }

      CBitFrame& operator= (const CBitFrame& right)
      {

          int nWidth  = right.Width();  
          int nHeight = right.Height();

          SetSize(nWidth, nHeight);


          const DWORD* pRightDataCursor = right.GetData();
          DWORD* pDestDataCursor        = m_pData;

          memcpy(pDestDataCursor, pRightDataCursor, m_nDataSizeInDword * sizeof(DWORD));

          return *this;
      }

      CBitFrame& operator &= (const CBitFrame& right)
      {
        assert(m_nDataSizeInDword == right.m_nDataSizeInDword);

        for(int i=0; i < m_nDataSizeInDword; i++)
        {
            m_pData[i] &= (right.m_pData[i]);
        }

        return *this;
      }

      CBitFrame& operator |= (const CBitFrame& right)
      {
        assert(m_nDataSizeInDword == right.m_nDataSizeInDword);

        for(int i=0; i < m_nDataSizeInDword; i++)
        {
            m_pData[i] |= (right.m_pData[i]);
        }

        return *this;
      }

protected:
    DWORD* m_pData;
    volatile long  m_lRef;
    int   m_nDataSizeInDword;
    int   m_nWidth ;
    int   m_nHeight;
};
#endif //