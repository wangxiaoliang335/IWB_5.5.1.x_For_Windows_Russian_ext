#pragma once
//
#include <limits>
#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>
//图片帧
template<class T = BYTE>
class CImageFrameT
{
public:
	typedef T ElementType;
	CImageFrameT()
		:
	  m_lRef(1),
	  m_pData(NULL),
	  m_nDataCount(0),
	  m_nWidth(0),
	  m_nHeight(0)
	  {

	  }

	  CImageFrameT(int nSize)
	  {
		  m_pData = new T[nSize];
		  m_nDataCount = nSize;
		  memset(m_pData, 0, sizeof(T)*m_nDataCount);
		  AddRef();

	  }

	  CImageFrameT(UINT nWidth, UINT nHeight)
	  {
		m_nWidth    = nWidth;
		m_nHeight   = nHeight;
		m_nDataCount = nWidth * nHeight;
		m_pData = new T[m_nDataCount];

		memset(m_pData, 0, sizeof(T)*m_nDataCount);
	  }

	  CImageFrameT(const CImageFrameT& right)
		  :
	  m_lRef(1),
	  m_pData(NULL),
	  m_nDataCount(0),
	  m_nWidth(0),
	  m_nHeight(0)
	  {
		//*this = right;
	    m_nWidth  = right.Width();
		m_nHeight = right.Height();
		m_nDataCount = right.Size();
		m_pData = new T[m_nDataCount];

		memcpy(m_pData, right.GetData(), sizeof(T)*m_nDataCount);
	  }


	  ~CImageFrameT()
	  {

		  if(m_pData)
		  {
			  delete[] m_pData;
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

	  UINT Width()const
	  {
		  return m_nWidth;
	  }

	  UINT Height() const
	  {
		  return m_nHeight;
	  }


	  const T* GetData()const
	  {
		  return m_pData;
	  }

	  T* GetData()
	  {
		  return m_pData;
	  }

	  int Size()const
	  {
		  return m_nDataCount*sizeof(T);

	  }

	  int BytesPerPixel()const
	  {
		  if(m_nHeight == 0 || m_nWidth == 0) return 0;
		  return m_nDataCount/(m_nHeight* m_nWidth);
	  }

	  void SetSize(int nWidth, int nHeight, int nGranularityPerPixel)
	  {
		 
		  int nNewDataCount = (nHeight* nWidth*nGranularityPerPixel  + sizeof(T)/2)/sizeof(T);

		  if(m_pData)
		  {

			  if(nNewDataCount != m_nDataCount)
			  {
					delete[] m_pData;
					
					m_pData = new T[nNewDataCount];						
					m_nDataCount = nNewDataCount;
				    memset(m_pData, 0, sizeof(T)*m_nDataCount);
					
				}


		  }
		  else
		  {
			  m_pData =  new T[nNewDataCount];	
			  m_nDataCount = nNewDataCount;
			  memset(m_pData, 0, sizeof(T)*m_nDataCount);
			  

		  }

		  
		  m_nWidth    = nWidth;
		  m_nHeight   = nHeight;
	  }


	  int Write(const T* pData, int nWidth, int nHeight, int nGranularityPerPixel)
	  {	

		  SetSize(nWidth, nHeight, nGranularityPerPixel);


		  return Write(pData, m_nDataCount*sizeof(T));

	  }



	  int Write(const T* pData, int nByteSize)
	  {

		  if(pData == NULL) return 0;

		  if(nByteSize > (int)(m_nDataCount*sizeof(T)))
		  {
			  nByteSize = m_nDataCount*sizeof(T);
		  }
		  memcpy(m_pData, pData, nByteSize);


		  return  nByteSize;
	  }

	  BOOL IsFree() const
	  {

		  return m_lRef == 1;
	  }

	  void Clear()
	  {
			memset(m_pData, 0 ,sizeof(T)*m_nDataCount);
	  }


	  void Copy(const CImageFrameT<T>& right)
	  {

		  int nWidth  = m_nWidth  > right.Width()? right.Width():m_nWidth;
		  int nHeight = m_nHeight > right.Height()?right.Height():m_nHeight;

		  const T* pSrcData = right.GetData(); 
		  for(int row=0; row<nHeight; row++)
		  {
			  int nRightRowOffset = row * right.Width();
			  int nLeftRowOffset  = row * m_nWidth;
			  for(int col=0; col<nWidth; col++)
			  {
				  m_pData[nLeftRowOffset + col] = pSrcData[nRightRowOffset + col];

			  }
		  }
	  }

	  
	   CImageFrameT<T>& operator=(const CImageFrameT<T>& right)
	  {

		  if(m_nDataCount != right.m_nDataCount)
		  {
				delete[] m_pData;
				m_nDataCount = right.m_nDataCount;
				m_pData = new T[m_nDataCount];
		  }

		  m_nWidth  = right.Width();
          m_nHeight = right.Height();

		  memcpy(m_pData, right.m_pData, sizeof(T) * m_nDataCount);

		  return *this;
	  }

	   CImageFrameT<T>& operator^(const CImageFrameT<T>& right)
	   {

			assert(m_nWidth == right.Width() && m_nHeight == right.Height());

			for(int i=0; i < m_nDataCount; i++)
		   {
			   m_pData[i] ^= (right.m_pData[i]);
		   }


		   return *this;
	   }


	   CImageFrameT<T>& operator&(const CImageFrameT<T>& right)
	   {

			assert(m_nWidth == right.Width() && m_nHeight == right.Height());

			for(int i=0; i < m_nDataCount; i++)
		   {
			   m_pData[i] &= (right.m_pData[i]);
		   }


		   return *this;
	   }



	   	CImageFrameT<T>& operator&=(const CImageFrameT<T>& right)
	   {

			operator&(right);


		   return *this;
	   }

	   //@功能:每个象素对应相减,
	   void operator -=(const CImageFrameT<T>& right)
	   {
			assert(m_nWidth == right.Width() && m_nHeight == right.Height());

			T* pLeft = m_pData;
			const T* pRight = right.m_pData;
			for(UINT row=0; row < this->m_nHeight; row ++)
			{
				for(UINT col=0; col < this->m_nWidth; col++)
				{
					if(*pLeft > *pRight)
					{
						*pLeft -= *pRight;
					}
					else
					{
						*pLeft = 0;
					}


					pRight ++;
					pLeft  ++;
				}//for-each(col)



			}//for-each(row)
		  
	   }


	   void Invert()
	   {
		   for(int i=0; i < m_nDataCount; i++)
		   {
				m_pData[i] = ~(m_pData[i]);
		   }

	   }


	   int Statistic(T val)const
	   {

		   int nCount = 0;
		   for(int i=0; i < m_nDataCount; i++)
		   {
				if(m_pData[i] == val)
				{
					nCount ++;
				}
		   }

		   return nCount;
	   }

	   T Max()const
	   {
		   T max = 0;
		   
		   for(int i=0; i < m_nDataCount; i++)
		   {
				if(m_pData[i] > max)
				{
					max = m_pData[i];
				}
		   }

		   return max;
	   }

	   T Min() const
	   {
		   //std::numeric_limits<T>::max与WinDef.h中的max宏定义冲突
		   //用添加()的方法解决。
		   T min = (std::numeric_limits<T>::max)();
		   for(int i=0; i < m_nDataCount; i++)
		   {
				if(m_pData[i] < min)
				{
					min = m_pData[i];
				}
		   }

		   return min;

	   }

	   T Avg() const
	   {

		   long double sum = 0;
			for(int i=0; i < m_nDataCount; i++)
		    {
				sum += m_pData[i];				
			}

			T avg = T( sum / m_nDataCount);
			return avg;
	   }


   //利用Breshenham算法绘制直线
   //参数:
   //
   //算法:直线方程由始点<x1,y1>和终点<x2,y2>确定
   //设直线方程为 y = kx +b, k,b可由始点<x1,y1>和终点<x2,y2>确定,<x1,y1>和<x2,y2>严格
   //符合该方程。但计算机屏幕线上的其他点未必严格符合此方程。即y[i]≠k*x[i] + b,因为k,b
   //为浮点数。
   //
   //
   //
  void Line(POINT start, POINT end, T value)
  {
	LONG dx =  (end.x - start.x > 0)?end.x - start.x : start.x - end.x ;
	LONG dy  = (end.y - start.y > 0)?end.y - start.y :start.y  - end.y;
	LONG sx, sy, err, e2;
	
	sx = (start.x < end.x)?1:-1;			
	sy = (start.y < end.y)?1:-1;
	err = dx -dy;

	do
	{
	
		SetPixel(start, value);
		//m_pData[start.y * m_nWidth + start.x] = value;
		
		if(start.x == end.x && start.y == end.y) break;
		e2 = err*2;
		if(e2 > -dy)
		{	
			err -= dy;
			start.x += sx;
		}
		if(e2 < dx)
		{
			err += dx;
			start.y  += sy;
		}
	}while(1);

  }

  inline void SetPixel(const POINT& pt, T value)
  {
		if(pt.x  < 0 || pt.x >= (LONG)m_nWidth) return;
		if(pt.y  < 0 || pt.y >= (LONG)m_nHeight) return;
		m_pData[pt.y * m_nWidth + pt.x] = value;
  }

  //
  //@功能:给从ptStart开始到ptEnd结束的线段的末端始端添加箭头
  //@参数:ptStart, 线段起始端
  //      ptEnd,线段结束端
  //      angle,箭头与直线的夹角(0~180)
  //      nArrowSize,箭头斜边长度
  void DrawArrow(POINT& ptStart, POINT& ptEnd, double angle, int nArrowSize, T value)
  {

	  if(angle<0.0) angle = 0.0;
	  if(angle > 180.0) angle  = 180.0;

	  long Vx = ptStart.x - ptEnd.x;
	  long Vy = ptStart.y - ptEnd.y;
	  long Vl = LONG(sqrt(float(Vx * Vx + Vy * Vy))); 
	  
	  if(Vl > 0)
	  {
		  Vx = Vx * nArrowSize /Vl;
		  Vy = Vy * nArrowSize /Vl;
	  }
	 

	  
	  double dbX, dbY;
	  double phi = angle*M_PI/180.0;
	  
	  //1.矢量右手方向
	  dbX = Vx * cos(phi) - Vy * sin(phi);
	  dbY = Vx * sin(phi) + Vy * cos(phi);

	  POINT ptArrowEnd;
	  ptArrowEnd.x = ptEnd.x + (int)dbX;
	  ptArrowEnd.y = ptEnd.y + (int)dbY;

	  Line(ptEnd, ptArrowEnd,value);

	  
	  //2.矢量左手方向
	  phi = -phi;
	  dbX = Vx * cos(phi) - Vy * sin(phi);
	  dbY = Vx * sin(phi) + Vy * cos(phi);

	  ptArrowEnd.x = ptEnd.x + (int)dbX;
	  ptArrowEnd.y = ptEnd.y + (int)dbY;

	  Line(ptEnd, ptArrowEnd,value);
	  

  }


  
  //@功能:绘制矩形
  //@参数:area, 矩形区域
  //      value, 亮度值
  void DrawRectange(const RECT& area, T value)
  {

	 RECT rcArea = area;
	if(rcArea.left < 0 ) rcArea.left = 0;
	if(rcArea.right >= (LONG)m_nWidth) rcArea.right = m_nWidth - 1;

	if(rcArea.top < 0 ) rcArea.top = 0;
	if(rcArea.bottom >= (LONG)m_nHeight) rcArea.bottom = m_nHeight - 1;

	POINT p1, p2, p3, p4;
	p1.x = rcArea.left;
	p1.y = rcArea.top;

	p2.x = rcArea.right;
	p2.y = rcArea.top;

	p3.x = rcArea.right;
	p3.y = rcArea.bottom;


	p4.x = rcArea.left;
	p4.y = rcArea.bottom;


	Line(p1, p2, value);
	Line(p2, p3, value);
	Line(p3, p4, value);
	Line(p4, p1, value);
	

  }

protected:
	volatile long  m_lRef;//引用计数器
	T*     m_pData       ;
	int   m_nDataCount    ;//类型尺寸

	UINT   m_nWidth      ;
	UINT   m_nHeight     ;
};

typedef CImageFrameT<BYTE> CImageFrame;
typedef CImageFrameT<WORD> CYUY2Frame;

