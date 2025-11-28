#pragma once
//
#include <limits>
#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>


extern unsigned char ascii_font_8X16[][16];
extern unsigned char ascii_font_5X7[][5];
static const int alignment = 32;
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
        m_nPixelCount(0),
        m_nWidth(0),
        m_nHeight(0)
    {

    }

    CImageFrameT(int nSize)
    {
		m_pData=  (T*)_aligned_malloc(nSize, alignment);//32字节对齐分配内存空间,以便使用SIMD指令
        m_nPixelCount = nSize;
        memset(m_pData, 0, sizeof(T)*m_nPixelCount);
        AddRef();

    }

    CImageFrameT(UINT nWidth, UINT nHeight)
    {
        m_nWidth    = nWidth;
        m_nHeight   = nHeight;
        m_nPixelCount = nWidth * nHeight;

        m_pData=  (T*)_aligned_malloc(m_nPixelCount*sizeof(T), alignment);//32字节对齐分配内存空间,以便使用SIMD指令

        memset(m_pData, 0, sizeof(T)*m_nPixelCount);
    }

    CImageFrameT(const CImageFrameT& right)
        :
    m_lRef(1),
        m_pData(NULL),
        m_nPixelCount(0),
        m_nWidth(0),
        m_nHeight(0)
    {
        //*this = right;
        m_nWidth  = right.Width();
        m_nHeight = right.Height();
        m_nPixelCount = m_nWidth*m_nHeight;

        m_pData=  (T*)_aligned_malloc(m_nPixelCount*sizeof(T), alignment);//32字节对齐分配内存空间,以便使用SIMD指令
        //m_pData = new T[m_nPixelCount];

        memcpy(m_pData, right.GetData(), sizeof(T)*m_nPixelCount);
    }


    ~CImageFrameT()
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
        return m_nPixelCount*sizeof(T);

    }

    int BytesPerPixel()const
    {
        if(m_nHeight == 0 || m_nWidth == 0) return 0;
        return m_nPixelCount/(m_nHeight* m_nWidth);
    }

    int GetPixelCount() const
    {
        return m_nHeight*m_nWidth;
    }
    //@功能:设置图片帧尺寸
    //@参数:nWidth, 宽度
    //      nHeight, 高度
    //      nGranularityPerPixel, 每个象素的字节数
    void SetSize(int nWidth, int nHeight, int nGranularityPerPixel = sizeof(T), const T* pInitValue = NULL)
    {
        int nNewPixelCount = (nHeight* nWidth*nGranularityPerPixel  + sizeof(T)/2)/sizeof(T);

        if(m_pData)
        {//已分配了内存，检查是否小于需要的
            if(nNewPixelCount != m_nPixelCount)
            {
                _aligned_free(m_pData);

                m_nPixelCount = nNewPixelCount;

                m_pData=  (T*)_aligned_malloc(m_nPixelCount*sizeof(T), alignment);//32字节对齐分配内存空间,以便使用SIMD指令
            }
            //进行初始化操作
            if(pInitValue)
            {
                int nLoopCount = m_nPixelCount;
                for(int i = 0; i < nLoopCount; i++)
                {
                    m_pData[i] = *pInitValue;
                }
            }

        }
        else if(nNewPixelCount)//尺寸大于0
        {
            m_nPixelCount = nNewPixelCount;
            //注意:_aligned_malloc在输入的内存尺寸为0时，返回的内存指针不为0
            m_pData=  (T*)_aligned_malloc(m_nPixelCount*sizeof(T), alignment);//32位字节对齐分配内存空间 

            if(pInitValue)
            {
                int nLoopCount = m_nPixelCount;
                for(int i = 0; i < nLoopCount; i++)
                {
                    m_pData[i] = *pInitValue;
                }
            }
        }

        m_nWidth    = nWidth;
        m_nHeight   = nHeight;

    }

    int Write(const T* pData, int nWidth, int nHeight, int nGranularityPerPixel)
    {	

        SetSize(nWidth, nHeight, nGranularityPerPixel);
        return Write(pData, m_nPixelCount*sizeof(T));

    }



    int Write(const T* pData, int nByteSize)
    {

        if(pData == NULL) return 0;

        if(nByteSize > (int)(m_nPixelCount*sizeof(T)))
        {
            nByteSize = m_nPixelCount*sizeof(T);
        }
        memcpy(m_pData, pData, nByteSize);


        return  nByteSize;
    }

    BOOL IsFree() const
    {

        return m_lRef == 1;
    }

    void Clear(T value = T(0x00))
    {
        long nLoopCount = m_nPixelCount;
        int i = 0;
        while(nLoopCount--)
        {
            m_pData[i++] = value;
        }
        
        
        //memset(m_pData, cVal ,sizeof(T)*m_nPixelCount);
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
        if(m_nPixelCount != right.m_nPixelCount)
        {
            //delete[] m_pData;
            _aligned_free(m_pData);
            m_nPixelCount = right.m_nPixelCount;

            //m_pData = new T[m_nPixelCount];
            m_pData=  (T*)_aligned_malloc(m_nPixelCount*sizeof(T), alignment);//32字节对齐分配内存空间,以便使用SIMD指令

        }

        m_nWidth  = right.Width();
        m_nHeight = right.Height();

        memcpy(m_pData, right.m_pData, sizeof(T) * m_nPixelCount);

        return *this;
    }

    CImageFrameT<T>& operator^(const CImageFrameT<T>& right)
    {

        assert(m_nWidth == right.Width() && m_nHeight == right.Height());

        for(int i=0; i < m_nPixelCount; i++)
        {
            m_pData[i] ^= (right.m_pData[i]);
        }

        return *this;
    }

    CImageFrameT<T>& operator&(const CImageFrameT<T>& right)
    {

        assert(m_nWidth == right.Width() && m_nHeight == right.Height());

        for(int i=0; i < m_nPixelCount; i++)
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

    CImageFrameT<T>& operator|(const CImageFrameT<T>& right)
    {
        assert(m_nWidth == right.Width() && m_nHeight == right.Height());
        for(int i=0; i < m_nPixelCount; i++)
        {
            m_pData[i] |= (right.m_pData[i]);
        }
        return *this;
    }

    CImageFrameT<T>& operator|=(const CImageFrameT<T>& right)
    {
        assert(m_nWidth == right.Width() && m_nHeight == right.Height());
        operator|(right);
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

    CImageFrameT<T> operator -(const CImageFrameT<T>& right)
    {
        assert(m_nWidth == right.Width() && m_nHeight == right.Height());

        CImageFrameT<T> imageDest(*this);


        T* pLeft = imageDest.GetData();
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

        return imageDest;

    }


    void Invert()
    {
        for(int i=0; i < m_nPixelCount; i++)
        {
            m_pData[i] = ~(m_pData[i]);
        }

    }


    int Statistic(T val)const
    {

        int nCount = 0;
        for(int i=0; i < m_nPixelCount; i++)
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

        for(int i=0; i < m_nPixelCount; i++)
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
        for(int i=0; i < m_nPixelCount; i++)
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
        for(int i=0; i < m_nPixelCount; i++)
        {
            sum += m_pData[i];				
        }

        T avg = T( sum / m_nPixelCount);
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
        SetPixel(pt.x, pt.y, value);
    }

    inline void SetPixel(int x, int y , T value)
    {
        if(x  < 0 || x >= (LONG)m_nWidth) return;
        if(y  < 0 || y >= (LONG)m_nHeight) return;
        m_pData[y * m_nWidth + x] = value;
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
		if (rcArea.left == rcArea.right || rcArea.top == rcArea.bottom) return;

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

    //@功能:绘制矩形
    //@参数:area, 矩形区域
    //      value, 亮度值
    void DrawCross(const POINT& center, int size, T value)
    {
        POINT p1, p2;

        p1 = p2 = center;
        p1.x = center.x - size / 2;
        p2.x = center.x + size / 2;
        Line(p1, p2, value);

        p1 = p2 = center;
        p1.y = center.y - size / 2;
        p2.y = center.y + size / 2;
        Line(p1, p2, value);


    }


     //@功能:以取最大值方式进行融合
    //@参数:right, 被融合的图片
    void Or(const CImageFrameT<T>& right)
    {
        assert(m_nWidth == right.Width() && m_nHeight == right.Height());


        T* pLeft = m_pData;
        const T* pRight = right.m_pData;
        for(UINT row=0; row < this->m_nHeight; row ++)
        {
            for(UINT col=0; col < this->m_nWidth; col++)
            {
                
                T val = *pLeft;
            
                *pLeft = val | *pRight;

                pRight ++;
                pLeft  ++;
            }//for-each(col)

        }//for-each(row)

    }




    //@功能:以指定的百分比融合图片
    //@参数:right, 被融合的图片
    //      nPercent, 融合百分比
    void Merge(const CImageFrameT<T>& right, int nRightPercent)
    {
        assert(m_nWidth == right.Width() && m_nHeight == right.Height());

        
        if(nRightPercent > 100) nRightPercent = 100;
        if(nRightPercent < 0 )  nRightPercent = 0;

        int nLeftPercent = 100 - nRightPercent;

        T* pLeft = m_pData;
        const T* pRight = right.m_pData;
        for(UINT row=0; row < this->m_nHeight; row ++)
        {
            for(UINT col=0; col < this->m_nWidth; col++)
            {
                
                T val = T((int(*pLeft) * nLeftPercent + int(*pRight)*nRightPercent + 50)/100);

                *pLeft = val;
                pRight ++;
                pLeft  ++;
            }//for-each(col)

        }//for-each(row)

    }


     //@功能:以取最大值方式进行融合
    //@参数:right, 被融合的图片
    void Merge(const CImageFrameT<T>& right)
    {
        assert(m_nWidth == right.Width() && m_nHeight == right.Height());


        T* pLeft = m_pData;
        const T* pRight = right.m_pData;
        for(UINT row=0; row < this->m_nHeight; row ++)
        {
            for(UINT col=0; col < this->m_nWidth; col++)
            {
                
                T val = *pLeft;
                if(val < *pRight)
                {
                    val = *pRight;
                }

                *pLeft = val;
                pRight ++;
                pLeft  ++;
            }//for-each(col)

        }//for-each(row)

    }

    //@功能:设置图片的所有像素为指定值
    //
    void Set(T val )
    {
        T* pData = m_pData;
        for(UINT row=0; row < this->m_nHeight; row ++)
        {
            for(UINT col=0; col < this->m_nWidth; col++)
            {   
                *pData = val;
                pData ++;
            }
        }
    }

    //@功能:二值化灰度图片
    //@参数:threshold:门限值, 大于等于该值则置为(std::numeric_limits<T>:max)()
    //                小于门限置为0
    void Binarize(T threshold)
    {
        T* pData = m_pData;
        for(UINT row=0; row < this->m_nHeight; row ++)
        {
            for(UINT col=0; col < this->m_nWidth; col++)
            {   
                if(*pData >= threshold)
                {
                    *pData = (std::numeric_limits<T>::max)();
                }
                else
                {
                    *pData = 0;
                }
                pData ++;
            }
        }

    }



    //@功能:在图片上显示ASCII字符
    //@参数:x,字符显示的水平起始坐标
    //      y,字符显示的垂直起始坐标
    //      g,灰度值
    //      fs,字体高度
    //@日期:2014/04/24
    void PutChar(int x, int y, char ch, T g, int fs = 16)
    {
        if(ch < 0 || ch > 128) return;

        switch(fs)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            {   //每个字符的字模数据中1个字节表示一列数据
                //一个字模有5列
                unsigned char (&font_data)[5] = ascii_font_5X7[ch - 32];
                
                for(int c = 0; c < 5 ; c ++)
                {
                    unsigned char d = font_data[c];

                    int yy = y;

                    for(int r = 0; r < 8; r++)
                    {
                        if(d & (1 << r) )
                        {
                            SetPixel(x, yy, g);
                        }
                        yy++;
                    }

                    x ++;
                                          
                }//for-each(c)

            }

            break;


        case 16:
        default:
            {  //每个字符的字模数据中1个字节表示一行数据
                unsigned char (&font_data)[16] = ascii_font_8X16[ch - 32];
                for(int r = 0; r < _countof(font_data); r++)
                {
                    unsigned char d = font_data[r];
                    int xx = x;
                    for(int c = 0; c < 8 ; c++)
                    {
                        if(d & (1<<c))
                        {
                            SetPixel(xx, y, g);
                        }
                        xx ++;
                    }

                    y ++;

                }
            }
            

        }//switch
    }


    //@功能:在图片上显示ASCII字符串
    //@参数:x,字符显示的水平起始坐标
    //      y,字符显示的垂直起始坐标
    //      g,灰度值
    //     fs,字体高度
    //@日期:2014/04/24
    void PutStr(int x, int y, const char* szText, T g, int fs = 16)
    {
        
        int space = 8;
        switch(fs)
        {
        case 1:
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
        case 8:
            space = 4;

        default:
            space = 8;
        };

        for(size_t i = 0; i < strlen(szText); i++)
        {
            PutChar(x, y, szText[i], g, fs);

            x += space;
        }
    }


    BOOL IsEmpty() const
    {
        return m_nPixelCount == 0?TRUE:FALSE;
    }

protected:
    volatile long  m_lRef;//引用计数器
    T*     m_pData       ;
    int   m_nPixelCount    ;//像素总数

    UINT   m_nWidth      ;
    UINT   m_nHeight     ;
};


typedef CImageFrameT<BYTE> CImageFrame;
typedef CImageFrameT<BYTE> CByteFrame;
typedef CImageFrameT<WORD> CYUY2Frame;
typedef CImageFrameT<WORD> CWordFrame;
//typedef CImageFrameT<DWORD> CARGBFrame;
typedef CImageFrameT<DWORD> CBRGAFrame;//BB RR GG AA(低到高)
typedef CImageFrameT<DWORD> CRGBAFrame;//RR RR BB AA(低到高)
