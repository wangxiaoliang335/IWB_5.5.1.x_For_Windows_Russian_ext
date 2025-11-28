#pragma once
#include <atlwin.h>
#include <Mf2StdGf.h>
#include <MsgDefine.h>
#include <ImageFrame.h>
#include <BitFrame.h>
#include <ImageDib.h>
#include <DIB/Dib.h>
#include <AVI/AVIWriter.h>
#include <AVI/AVIReader.h>
#include <limits>
#include <vector>

#include <ImageDebug.h>

#include <assert.h>
#include <AlgoLib.h>
#include <BlobDetectorImpl.h>
//#include <RobustCornerDetect.h>

#include "CalibratePattern.h"
#include "Histogram.h"
#include <ImageFrameAPI.h>
#include  <MyAPI.h>
//#include "../MorphologyAlgo/inc/MorphologyAlgo.h"
#include "AutoCalibratorInterface.h"
#include "AutoCalibrateWnd.h"
#include "GrayImageBinarize.h"
#include "CrossDetector.h"
#include "AutoBrightnessRegulator.h"
#include "DispMonitorFinder.h"
#include "../inc/SplitArea.h"
#include "../../inc/MJPG/MJPG_Decoder.h"
#include "../../inc/FillPolygon.h"
#include "../../libAVI/libAVI.h"

typedef std::vector<POINT> PolygonVertices;


#define BLACK RGB(0,    0,   0)
#define WHITE RGB(255,255, 255)
#define RED   RGB(255,  0,   0)
#define BLUE  RGB(0,    0, 255)
#define GREEN RGB(0,  255,   0)

//前景颜色
#define FOREGROUND_COLOR WHITE

#define BACKGROUND_COLOR BLACK


//在超短焦定位实践中， 发现采用一幅棋盘格图进行校正存在以下缺陷：
//A: 在摄像头中看到的图像呈上宽下窄的梯形， 上边的宽度是下边宽度的2倍，
// 上边的高度是下边高度的4倍。 若使用统一的标准查找上部分和下部分的角点时，
// 会发现下半部分会有很多噪点目标， 这些噪点目标的存在使得寻找正确的角点变得
// 困难。
//
//为了克服上述的缺陷, 改进后的自动校正算法将屏幕分成若干个矩形区域， 每个区域进行校正
//时只在对应屏幕区域区域内显示国际象棋棋盘格样式的校正图案，对角点的搜索也改为分区域搜索。
//整个自动校正算法计算步骤修改如下
//Step1:图形预处理,包括
// 1.1 将彩色图片处理成灰度图片
// 1.2 进入角点检测器前从图片中减去环境背景图，消除环境干扰。
//
//
//Step2:在相机画面中找出计算屏幕影像对应的区域, 这块区域也在后续图像处理用作屏蔽图
// 1.1 将计算机屏幕背景刷成白色，对图像二值化, 得到只保留最大高亮区域的二值位图A。
// 1.2 将计算机屏幕背景刷成黑色，对图像二值化, 得到黑白二值图B.
// 1.3 A中为1的区域并且B中对应为0的区域即为真正的屏幕区域
//
//Step3:计算机屏幕在摄像头画面中的偏转角度
// 2.1 先在计算机屏幕上显示上白下黑的黑白条块， 计算摄像头画面中白色区域的重心坐标G1.
// 2.2 再在计算机屏幕上显示上黑下白的黑白条块， 计算摄像头画面中白色区域的重心坐标G2.
// 2.3 计算矢量V<G1, G2>, V为屏幕垂直向矢量在摄像头画面中的方位。
//
//
//Step3.进入子区域校正阶段
// 在计算机屏幕自区域输出校正棋盘图案， 分析并找出摄像头面中出各个校正角点的位置，并与已知的
// 屏幕坐标建立一对一映射关系。
//
// 如果所有子区域校正完毕， 转步骤4， 否则继续执行步骤3
//
//
//Step4.将所有找到的屏幕校正点与摄像头画面角点的二元组集合带入校正方程，计算校正方程系数。
//


//针对60帧灰度MJPG编码摄像头的改进措施(2013/08/30)
//1.由于帧率加倍, 校正时使用的白屏和黑屏不再亮度逐渐增亮显示, 改为立即显示。
//
//2.校正方法修改
//2.1:对外圈的校正点, 屏幕上每次只打出一个点的校正团
//2.2:对内部校正点, 屏幕上将校正图案分成上下两部分分别显示。
//    由于校正图案分为上下两部分显示, 因此无需查找上下两部分的屏蔽图了。
//@说明:pRect，以窗体左上角为坐标原点(删除)
//      pRect, 以主屏幕左上角为坐标原点
inline void FillBoard(HWND hWnd, COLORREF clr, const RECT* pRect=NULL)
{
    HDC hDC = ::GetDC(hWnd);

    HBRUSH hBrush = ::CreateSolidBrush(clr);//


    RECT rcClient;

    if(pRect)
    {
        rcClient = *pRect;
        LPPOINT pPoint = (LPPOINT)&rcClient; 
        ScreenToClient(hWnd, pPoint);
        ScreenToClient(hWnd, pPoint + 1);

    }
    else
    {
        ::GetClientRect(hWnd,&rcClient);
    }

    
    BOOL bRet = FillRect(hDC, &rcClient, hBrush);
    ::DeleteObject(hBrush);
    ReleaseDC(hWnd, hDC);


}


//=========静态屏蔽查找者=======
class CStaticMaskFinder
{
public:
        CStaticMaskFinder()
            :
        m_cMaskThreshold(0x080),
        m_nCountDown(0),
        m_hFont(NULL),
        m_hDisplayWnd(NULL),
        m_hFontOld(NULL),
        m_hDC(NULL),
        m_hbrWhite(NULL)
        {

        }
        
        ~CStaticMaskFinder()
        {
        
        }


        //@功能:复位
        //@参数:nImageWidth,  图像宽度
        //      nImageHeight, 图像高度
        //      cMaskThreshold, 屏蔽门限, 当亮度值大于此门限时屏蔽
        //      hDisplayWnd, 进度显示窗体
        //      rcMonitor, 以主屏幕左上角为坐标原点坐标系中的矩形区域
        void Reset(int nImageWidth, int nImageHeight, BYTE cMaskThreshold, HWND hDisplayWnd, const RECT& rcMonitor,bool bDispNumber)
        {  
            m_oStaticMaskFrame.SetSize(nImageWidth, nImageHeight, cMaskThreshold);
            
            //全部初始化为0xFF，即无任何屏蔽状态。
            m_oStaticMaskFrame.Clear(0xFF);

            m_cMaskThreshold = cMaskThreshold;

            m_hDisplayWnd = hDisplayWnd;

            m_rcMonitor = rcMonitor;
            LPPOINT pPoint = (LPPOINT)&m_rcMonitor;
            ScreenToClient(hDisplayWnd, pPoint);
            ScreenToClient(hDisplayWnd, pPoint + 1);

            InitGDIObject();

            /////add by vera_zhao 2018.12.29
            //是否显示数字
            if (bDispNumber)
            {
                m_nCountDown = MAX_COUNT_DOWN;
                DispNumber(m_nCountDown / 15);
            }
            else
            {
                m_nCountDown = 25;
            }      
        }


        //@功能:接收来自摄像头的每一帧数据，多次处理的屏蔽图叠加。
        //@参数:scrFrame, 8位的灰度图片
        //@返回值:TRUE, 静态屏蔽结束
        //        FALSE, 静态屏蔽中
        BOOL Process(const CImageFrame& srcFrame,bool bDispNumber)
        {
            if(m_nCountDown == 0) return  TRUE;//自动屏蔽已经结束。

            const BYTE* pSrc  = srcFrame.GetData();
                  BYTE* pMask = m_oStaticMaskFrame.GetData();
               int nLoopCount = srcFrame.GetPixelCount();
               while(nLoopCount)
               {
                   if(*pSrc > m_cMaskThreshold)
                   {
                        *pMask = 0x00;
                   }
                   
                   pSrc  ++;
                   pMask ++;
                   nLoopCount --;
               }//while

               m_nCountDown --;

               if (bDispNumber)
               {
                  if(m_nCountDown % 15 == 0)
                  {
                     int number = m_nCountDown / 15;
                     DispNumber(number);
                  }
               }
               else
               {
                   //不做操作
               }

               if(0 == m_nCountDown)
               {
                    ReleaseGDIObject();
                    return TRUE;
               }

               return FALSE;
        }//

        //功能:返回静态屏蔽图
        const CImageFrame& GetMaskFrame() { return m_oStaticMaskFrame;}

protected:
        void InitGDIObject()
        {
            RECT rcWnd;
            GetClientRect(m_hDisplayWnd, &rcWnd);
            int nWndHeight = rcWnd.bottom - rcWnd.top;

            LOGFONT lf;
            memset(&lf, 0, sizeof(LOGFONT));
            lf.lfHeight        = nWndHeight/2;
            lf.lfWidth         = 0;
            lf.lfWeight        = FW_BOLD;
            lf.lfCharSet       = ANSI_CHARSET;
            lf.lfOutPrecision  = OUT_DEFAULT_PRECIS;
            lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
            memcpy(lf.lfFaceName, _T("Times New Roman"), _countof(lf.lfFaceName));

            m_hFont    = CreateFontIndirect(&lf);
            m_hDC      = GetDC(m_hDisplayWnd);
            m_hFontOld = (HFONT)SelectObject(m_hDC, m_hFont);
            m_hbrWhite = ::CreateSolidBrush(RGB(255,255,255));

            m_clrOldText = SetTextColor(m_hDC, RGB(255,0,0));

        }

        void ReleaseGDIObject()
        {
            if(m_hFontOld)
            {
                SelectObject(m_hDC, m_hFontOld);
            }

            SetTextColor(m_hDC, m_clrOldText);

            DeleteObject(m_hbrWhite);
            ReleaseDC(m_hDisplayWnd, m_hDC);
            m_hDC = NULL;
        }

        void DispNumber(int num)
        {
           TCHAR szText[32];
           _stprintf_s(szText, _countof(szText), _T("%d"), num);

            //RECT rcWnd;
            //GetClientRect(m_hDisplayWnd, &rcWnd);
            
            //FillRect(m_hDC, &rcWnd, m_hbrWhite);
           FillRect(m_hDC, &m_rcMonitor, m_hbrWhite);
            SetBkMode(m_hDC, TRANSPARENT);

            //显示文字信息
            //DrawText(m_hDC, szText, _tcsclen(szText), &rcWnd, DT_CENTER|DT_VCENTER| DT_SINGLELINE );
            DrawText(m_hDC, szText, _tcsclen(szText), &m_rcMonitor, DT_CENTER|DT_VCENTER| DT_SINGLELINE );
        }

protected:
    CImageFrame  m_oStaticMaskFrame;//静态屏蔽图, 屏蔽区像素值为0
    BYTE         m_cMaskThreshold  ;//屏蔽门限, 大于该值即屏蔽
    HWND         m_hDisplayWnd     ;//显示图案的窗体
    RECT         m_rcMonitor       ;//屏幕矩形区，以校正窗体左上角为坐标原点
    int          m_nCountDown      ;//倒数计数器

   

    //显示相关GDI对象
    HFONT m_hFont                  ;//计数显示的字体句柄
    HFONT m_hFontOld               ;//
    HDC   m_hDC                    ;//
    HBRUSH m_hbrWhite              ;//白色画刷
    COLORREF m_clrOldText          ;
    static const int MAX_COUNT_DOWN = 45;  //45
};

inline void AccumulateImageFrame(const CImageFrame& srcFrame, CWordFrame& destFrame)
{
    const BYTE* pBytePixel = srcFrame.GetData();
    WORD* pWordPixel = destFrame.GetData();

    int nLoopCount = destFrame.GetPixelCount();

     while(nLoopCount)
    {
        *pWordPixel += *pBytePixel;
        pWordPixel++;
        pBytePixel++;
        nLoopCount --;
    }                
}

//存在不同帧率的摄像头，以及部分品牌投影机显示延迟问题的存在。
//将延迟等待的帧数改为绝对时间
class CWaitTimer
{
public:
    CWaitTimer()
        :
    m_dwVideoDisplayDelay(0)
    {
        Init(1, 0, FALSE);
    }

    //@参数:dwLatencyTimes， 时间延迟倍数
    void Init(DWORD dwTimeMagnification, DWORD dwVideoDisplayDelay, BOOL bUseAbsTime=FALSE)
    {
         m_dwTimeBegin = GetTickCount();

        if (dwTimeMagnification <= 0 )
        {
            dwTimeMagnification = 1;
        }
        if (dwTimeMagnification > 10)
        {
            dwTimeMagnification = 10;
        }
        m_dwTimeMagnification = dwTimeMagnification;

        m_dwVideoDisplayDelay = dwVideoDisplayDelay;
        m_dwFrameCount = 0;

        m_bUseAsbTime = bUseAbsTime;
    }

   
    void Reset()
    {
        m_dwTimeBegin = GetTickCount();
        m_dwFrameCount = 0;
    }

    BOOL IsWaitTimeout(DWORD dwWaitTimeInMs)
    {
        if (m_bUseAsbTime)
        {
            DWORD dwTimeElapsed = GetTimeElapsed();

            return (dwTimeElapsed >= dwWaitTimeInMs*m_dwTimeMagnification + m_dwVideoDisplayDelay) ? TRUE : FALSE;
        }
        else
        {
            const int MILLI_SECOND_PER_FRAME = 16;
            DWORD dwNeeedWaitFrames = (dwWaitTimeInMs + (MILLI_SECOND_PER_FRAME  >> 1))/ MILLI_SECOND_PER_FRAME;
            dwNeeedWaitFrames *= m_dwTimeMagnification;
            dwNeeedWaitFrames += m_dwVideoDisplayDelay / MILLI_SECOND_PER_FRAME;
            return (m_dwFrameCount >= dwNeeedWaitFrames) ? TRUE : FALSE;
        }
    }


    void Trigger()
    {
        if (!m_bUseAsbTime)
        {
            m_dwFrameCount++;
        }

    }

    DWORD GetTimeMagnification()
    {
        return m_dwTimeMagnification;
    }
protected:
    DWORD GetTimeElapsed()
    {
        DWORD dwNow = GetTickCount();
        DWORD dwElpase = 0;

        if (dwNow >= m_dwTimeBegin)
        {
            dwElpase = dwNow - m_dwTimeBegin;
        }
        else
        {//系统计数器复零了
            dwElpase = (0xFFFFFFFF - m_dwTimeBegin) + dwNow;
        }
        return dwElpase;
    }


protected:
    DWORD m_dwTimeBegin        ;//计数起始时间
    DWORD m_dwTimeMagnification;//时间放大倍数
    DWORD m_dwFrameCount;
    DWORD m_bUseAsbTime;//使用绝对时间
    DWORD m_dwVideoDisplayDelay;//视频显示延迟，单位:ms

};


//屏幕区域定位器
class CMonitorAreaLocator
{
public:
    CMonitorAreaLocator()
        :
    m_nSubAreaId(0),
    m_bSuccess(FALSE),
    m_hDisplayWnd(FALSE),
    m_eDebugLevel()
    {

    }

    ~CMonitorAreaLocator()
    {

    }

    //@功能:复位定位器状态
    //@参数:nImageWidth, 图像宽度
    //      nImageHeight,图像高度
    //      hDisplay, 画面显示窗体
    //     rcMonitor, 当前屏幕区的矩形区
    void Reset(int nImageWidth, int nImageHeight, HWND hDisplayWnd, const RECT& rcMonitor, ECalibDebugLevel eDebugLevel = E_CALIB_DEBUG_LEVEL_CONCISE);


    //@功能:处理每一帧灰度图片
    //@参数:srcFrame, 图片帧对象
    //      bSimulate, 模拟校正标志
    //@返回值:TRUE, 正常处理。
    //        FALSE,处理错误。
     BOOL Process(const CImageFrame& srcFrame, BOOL bSimulate = FALSE);

     //@功能:获取屏蔽图
     const CImageFrame& GetMaskFrame(){return m_oMaskFrame;}

     //@功能:是否定位成功标志
     BOOL IsSuccess()const{return m_bSuccess;}

     static const int SUB_AREA_COUNT = 4;
     POINT (&GetSubAreaCentroids())[SUB_AREA_COUNT]
     {
        return  m_SubAreaCentroids;
     }

    CWaitTimer& GetWaiterTimer() 
    {
        return m_oWaitTimer;
    }
protected:
    
    CImageFrame m_oMaskFrame;//屏蔽帧

    HWND        m_hDisplayWnd;//显示窗体

    RECT        m_rcMonitor  ;//当前屏幕的矩形坐标

    //子区域搜索, 目的是为了减少在光照不均匀的情形下, 确保屏幕屏蔽图不缺角。
    
    CImageFrame m_SubAreaMask[SUB_AREA_COUNT];//
    RECT        m_SubAreaRect[SUB_AREA_COUNT];//
    int         m_nSubAreaId;//子区域Id

    POINT       m_SubAreaCentroids[SUB_AREA_COUNT];//子区域的重心，
    //区域搜索结果状态:
    BOOL        m_bSuccess;//
    
    enum E_RUN_STAGE
    {
        E_RUN_STAGE_BEGIN            ,//开始阶段
        E_RUN_STAGE_SAMPLE_BLACKBOARD,//采样黑板
        E_RUN_STATE_SAMPLE_SUBAREA   ,//采样子区域
        E_RUN_STAGE_SAMPLE_WHITEBOARD,//采样白板
        E_RUN_STAGE_END               //结束阶段
    }m_eRunStage;

    CWordFrame  m_oBlackBoardAccFrame ;//黑屏图案帧累加图
    CWordFrame  m_oWhiteBoardAccFrame ;//白屏图案帧累加图
    //int         m_nStageWaitCount     ;//阶段计数

    //static const int STAGE_MINIMUM_WAIT_COUNT = 5;//最小等待次数
    CWaitTimer m_oWaitTimer;
    static const DWORD STAGE_MINIMUM_WAIT_TIME = 100;//等待时间100ms


    //static const int WHITE_BOARD_SAMPLE_START_COUNT = STAGE_MINIMUM_WAIT_COUNT*2;//开始采样白板时的帧计数值
    //static const int WHITE_BOARD_SAMPLE_END_COUNT   = STAGE_MINIMUM_WAIT_COUNT*3;//结束白板采样时的帧计数值

    //static const int BLACK_BOARD_SAMPLE_START_COUNT = STAGE_MINIMUM_WAIT_COUNT*2;//结束采样时的帧计数值
    //static const int BLACK_BOARD_SAMPLE_END_COUNT   = STAGE_MINIMUM_WAIT_COUNT*3;//结束采样时的帧计数值


    static const DWORD  WHITE_BOARD_SAMPLE_START_TIME = STAGE_MINIMUM_WAIT_TIME * 2;//开始采样白板时需等待的时间
    static const DWORD  WHITE_BOARD_SAMPLE_END_TIME   = STAGE_MINIMUM_WAIT_TIME * 3;//结束采样白板时需等待的时间

    static const DWORD  BLACK_BOARD_SAMPLE_START_TIME = STAGE_MINIMUM_WAIT_TIME * 2;//开始采样黑板时需等待的时间
    static const DWORD  BLACK_BOARD_SAMPLE_END_TIME   = STAGE_MINIMUM_WAIT_TIME * 3;//结束采样黑板时需等待的时间

    ECalibDebugLevel m_eDebugLevel;//调试级别
    
};


inline void AccumulateImageFrame(const CImageFrame& srcFrame, const CImageFrame& maskFrame, CWordFrame& destFrame)
{
    const BYTE* pSrcPixel  = srcFrame.GetData();
    const BYTE* pMaskPixel = maskFrame.GetData();
    WORD* pDestPixel       = destFrame.GetData();

    int nLoopCount = destFrame.GetPixelCount();

     while(nLoopCount)
    {
        if(*pMaskPixel)
        {
            *pDestPixel += *pSrcPixel;
        }

        pMaskPixel ++;
        pSrcPixel  ++;
        pDestPixel ++;
        nLoopCount --;
    }//while
}
//==========================
    struct TBorderMarker//视频图像中的边界点的位置信息
    {
        POINT ptCentroid;//质心
        RECT  rcBound   ;//包围边界目标的矩形区域
    };

//==========================
//功能:屏幕边界查找器
class CMonitorBoundaryFinder
{
public:
     CMonitorBoundaryFinder();
    ~CMonitorBoundaryFinder();

    //@功能:复位状态机
    //@参数:frameInitalScreenMask,初始的屏幕屏蔽图
    //      hDispWnd, 显示窗体
    //      ECalibDebugLevel, 调试级别
    //      bSimulate, 模拟校正标志
    void Reset(const CImageFrame& frameInitalScreenMask, HWND hDispWnd, const RECT& rcMonitor, ECalibDebugLevel eDebugLevel= E_CALIB_DEBUG_LEVEL_CONCISE, BOOL bSimulate = FALSE);

    //@功  能:处理处理灰度图片
    //@参  数:grayImage, 输入的灰度图片
    //        bSimulate, 模拟校正标志
    //@返回值:TRUE,处理成功
    //        FALSE, 处理失败
    BOOL Process(const CImageFrame& grayImage, BOOL bSimulate = FALSE);

    BOOL IsDone()const{return m_bDone;}

    //const POINT& GetUpperPartCentroid()const{return m_ptUpperCentroid;}
    //const POINT& GetLowerPartCentroid()const{return m_ptLowerCentroid;}

    const POINT& GetScreenCentroid()const{return m_ptCentroid;}


    //const std::vector<TBorderMarker>& GetUpperBorderCalibratePts() const {return m_vecUpperBorderCalibratePts;}
    //const std::vector<TBorderMarker>& GetLowerBorderCalibratePts() const {return m_vecLowerBorderCalibratePts;}

    const std::vector<TBorderMarker>& GetBorderCalibratePts() const {return m_vecBorderCalibratePts;}

    //const CImageFrame& GetUpperHalfMaskFrame()const { return m_oUpperHalfMaskFrame;}
    //const CImageFrame& GetLowerHalfMaskFrame()const { return m_oLowerHalfMaskFrame;}

    const std::vector<RECT>& GetBorderMarkerPositions()const {return m_vecBorderMarkerPositions;}

    void SetSubAreaPolygons(const PolygonVertices* pPolygons, int nCount)
    {
        m_vecSubAreaPolygons.resize(nCount);
        for(int i = 0; i < nCount; i++)
        {
            m_vecSubAreaPolygons[i] = pPolygons[i];
        }
    }

    int GetMarkerCount()const;

    CWaitTimer& GetWaiterTimer()
    {
        return m_oWaitTimer;
    }
protected:
    //@功能:初始化边界标记位置坐标
    void InitBoundaryMarkerPositions(int nHorzSideSquareNumber, int nVertSideSquareNumber, int nSquareWidth);


    BOOL SearchCircleCentroids(const CWordFrame&  srcImage, TBorderMarker* pPtBorderMarker, int nExpectedNumber, POINT* pGlobalCentroid);

    //@功能:处理差分图片
    //@参数:diffImage, 差分图
    //      eScreenPart, 屏幕区域枚举值
    //BOOL ProcessDiffImage(const CWordFrame& diffImage, ESearchScreenPart eScreenPart);
    BOOL ProcessDiffImage(const CWordFrame& diffImage);


     //int m_nRunTimes                ;//运行次数计数
     static const int MAX_FLASH_TIMES = 3;

     int m_nFlashTimes                ;//闪烁次数
     int m_bShowMarker                ;//是否显示边界标记(实心圆)标志
     CWordFrame m_frameBackground     ;//暗帧
     CWordFrame m_frameForeground     ;//亮帧
     CImageFrame m_oInitialScreenMask ;//屏幕区域屏蔽图

     CImageFrame m_oScreenMaskFrame   ;//完整的屏蔽图
    
    POINT  m_ptCentroid               ;//重心

    //在屏幕四周显示的高亮白色实心圆的直径 , 单位像素。
    static const int INITIAL_MARKER_DIAMETER = 45;
    int m_MarkerDiameter;

    wchar_t szTmp[1024] = { 0 };

    //采用实心小圆的坐标来勾勒屏幕影像在画面中的边界
    std::vector<RECT> m_vecBorderMarkerPositions       ;//全部边界圆的位置(计算机屏幕坐标)
    std::vector<double> m_vecDisplayIntensity          ;//每个校正实心圆的显示强度,范围0~1.0, 屏幕四个顶角处的实心圆显示强度最大。

    std::vector<TBorderMarker> m_vecBorderCalibratePts    ;//在摄像头视频中的边界点的坐标,最后的结果应该按照从左上角开始,顺时针方向排列(视频坐标)
    std::vector<PolygonVertices> m_vecSubAreaPolygons;//图像划分的多边形子区域, 用来分区设定二值化门限

    RECT                   m_rcMonitor       ;//屏幕矩形区域
    ECalibDebugLevel       m_eDebugLevel     ;//调试级别

    HWND                   m_hDispWnd        ;//显示窗体
    BOOL                   m_bDone           ;//搜索结束标志


    CWaitTimer m_oWaitTimer;
    const int WAIT_STEDAY_SAMPLE_TIME = 160;//等待稳定采样时的时间,单位:ms
    const int WAIT_SAMPLE_END_TIME    = 330;//阶段结束的时间,单位:ms

    //水平方向的边界点个数
    int m_nHorzSideMarkerNumber;

    //垂直方向的边界点个数
    int m_nVertSideMarkerNumber;

    static const int MINIMUM_SIDE_MARKER_NUMBER = 5;
    
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//@类名，自动校正器
class CAutoCalibratorImpl2:public IAutoCalibratorInterface
{
public:
    //角点数据
    struct TImageCalibPoint
    {
        TPoint2D  pt2DImageCoord ;//角点在图像中的坐标
        POINT     ptRotatedCoord ;//经过旋转处理过的图像坐标
       
        //POINT ptHDImgCoord ;//高精度图像坐标值,即扩大了2^INT_SCALE_SIZE倍, 避免了四舍五入带来的精度误差
    };



public:
    CAutoCalibratorImpl2(/*int nCalibratePtNumber = 36*/)
        :
        m_bIsWorking(FALSE),
        m_AutoCalibrateWnd(this),
        m_eCalibrateStage(E_AUTO_CALIBRATE_UNDEF),
        m_bIsSimulatedCalibrating(FALSE),
        m_eCalibrateError(E_AUTO_CALIBRATOR_OK),
        //m_nMaskAreaPixelCount(0),
        //m_eMonochromizeAlog(E_MONOCHROMIZE_ALOG_Y),
        //m_eSubareaCalibrateStage(E_SUB_AREA_SEARCH_CALIBRATE_UNDEF),
        m_eDebugLevel(E_CALIB_DEBUG_LEVEL_CONCISE),
        //m_bDisplayPatternAgainWhenFailed(TRUE),
        //m_bCalibrateSequenceComplete(FALSE),
        m_nTryTimes(0),
        m_fpChangeCameraParams(NULL),
        m_lpCtx(NULL),
        m_bTestAutoBrightnessCtrlMode(FALSE),
        m_AutoBrightnessRegulator(100,BrightnessCtrlCallback, (LPVOID)this),
        //SQUARE_SIZE(25),
        m_nCalibratePtNumber(0),
        m_hNotifyWnd(NULL),
        m_nMaxTryTimes(MAX_TRY_TIMES),
        m_bRestoreLanguageBar(FALSE),
        m_bUseOldMethod(FALSE),
        m_bBorderCalibratrPtsIsValid(FALSE),
        m_bEnableOnlineScreenArea(FALSE),
        m_nDbgFrameCount(0),
        m_nDbgPatternRadius(58)
    {
        
        m_uMonitorWidth  = GetSystemMetrics(SM_CXSCREEN);
        m_uMonitorHeight = GetSystemMetrics(SM_CYSCREEN);

        //m_vecCalibrateSequence.resize(E_SEQUENCE_FRAME_NUM);

    }


    ~CAutoCalibratorImpl2()
    {

        m_AutoCalibrateWnd.DestroyWindow();
    }



    //@功能:开始校正, 供外部模块调用
    //@参数:
    //      hNotifyWnd, 消息通知窗体句柄
    //      E_AutoCalibratePattern, 校正方案枚举值
    //      cBrightness, 校正时画面亮度平均值
    //      ChangeCameraParamsProc, 调节摄像头参数的回调函数
    //      lpCtx, ChangeCameraParamsProc的上下文参数
    //      clrGridHighlight, 校正图案中高亮块的显示颜色
    //      bSaveInermediatFile, 是否保存校正的中间结果图片
    //      bRecordVideo，是否记录校正过程中的原始视频
    //   
    //virtual BOOL StartCalibrating(HWND hNotifyWnd, E_AutoCalibratePattern eGridPattern=E_CALIBRATE_PATTERN_11_COLS, BYTE cBrightness = 50, CHANGECAMERAPARAMSPROC ChangeCameraParamsProc = NULL, LPVOID lpCtx = NULL, COLORREF clrGridHighlight=RGB(255,255,255),BOOL bSaveInermediatFile = FALSE ,BOOL bRecordVideo = FALSE);


    //@功能:开始校正, 供外部模块调用
    //@参数:autoCalibrateParams, 自动校正参数
    //      staticMaskingParams, 静态屏蔽参数
     virtual BOOL StartCalibrating(const TAutoCalibrateParams& autoCalibrateParams, const TStaticMaskingParams& staticMaskingParams);


    //@功能:结束工作
    virtual void Finish();

    //@功能:停止校正
    void EndCalibrating();

    //@功能:结束自动屏幕区域搜索操作
    //void EndSearchingCurrentMonitorArea();

    //@功能:结束自动屏幕区域搜索操作
    //void EndSearchingAllMonitorArea();

   

    //@功能:给自动校正器投喂图片，并处理之
    //@返回值:异常返回FALSE
    virtual BOOL FeedImage(const CImageFrame* pGrayFrame);


    //@功能:给自动校正器投喂图片，并处理之
    //@返回值:异常返回FALSE
    BOOL FeedImage_AutoCalibrate(const CImageFrame* pGrayFrame);


    //@功能:返回屏幕个数
    virtual int GetMonitorCount()const;


    //@功能:获取指定屏幕的校正数据
    //@参数:nMonitorID， 显示器Id
    //@说明:
    //virtual  TMonitorCalibData GetMonitorCalibData(int nMonitorID) const;


     //@功能:获取所有屏幕的校正数据
     virtual  const TCalibData*  GetCalibrateData()const;

     void OnDeviceMissing();

    //@功能:子区域校正
    //@参数:
    //     nExpectedMarkerNumber,期望找到的校正点的数目
    //     maskFrame, 屏蔽图片
    //     srcFrame, 校正图片
    //     roateMatrix, 将校正图样旋转到水平垂直时使用的旋转矩阵系数
    //     ptCenter, 旋转中心的图像坐标
    //     nDetectRadius, 校正标记搜索的半径
    //    nAreaNo,区域编号
    //@说明:找到的角点存放到数组m_vecInteriorCalibPoint中
    //BOOL SubAreaCornerDetect(
    //    int nExpectedMarkerNumber,
    //    const CImageFrame& maskFrame,
    //    const CImageFrame& srcFrame,
    //    const double m[2][2],
    //    const POINT& ptCenter,
    //    int nDetectRadius,
    //    int nAreaNo);



    //@功能:收集校正点坐标的函数
    void CollectCalibrateMarkers(
        const TBlobObject* pObjs,
        int nBlobCount,
        const double rotationMatrix[2][2],//坐标旋转矩阵
        const POINT& ptCenter
        );


    //@功能:返回校正窗体句柄
    virtual HWND GetCalibrateHWnd()const;

    //@功能：返回自动校正后失败的中间结果图片路径
    virtual CAtlString  GetCalibrateIntermediataDirName();

    
    //@功能:模拟校正
    BOOL DoSimulateCalibrate(LPCTSTR lpszAVIFilePath, HWND hNotifyWnd,  UINT uCx, UINT uCy, BYTE cBrightness, E_AutoCalibratePattern eGridsNumber=E_CALIBRATE_PATTERN_11_COLS);
    
	CalibrateAlgo m_oCalibrateAlgo;

	struct TNeighbourVector
    {
        int nQaudrant;//方位所在象限编号
        LONG lDx ;
        LONG lDy ;
        //POINT pt ;
        int nVertexIndex;//矢量关联的校正点下标
    };

    //static const int MAX_NEIBHBOUR = 16;
    static const int MAX_NEIBHBOUR = 16;

    struct TVertex
    {
        POINT ptCoord ;//图像坐标
        unsigned nAryIndex ;//在数组中的索引号
        //BOOL bValid ;//数据有效标志
        unsigned int nNeighbourNumber ;//邻接点数目
        BOOL bIsOuterLayer ;//在外圈标志
        BOOL bIsTurnPoint ;//是拐角处角点的标志

        struct
        {
            bool bValid;
            int nIndex;
            double distance;

        }aryNeighbours[MAX_NEIBHBOUR];//邻接点索引号

        TVertex()
        {
            ptCoord.x = 0;
            ptCoord.y = 0;
            nNeighbourNumber = 0;
            bIsOuterLayer = FALSE;
            bIsTurnPoint = FALSE;
            nAryIndex = 0;
            memset(aryNeighbours, 0, sizeof(aryNeighbours));
        }

    };

     struct TurnPoint
    {
        POINT ptCoord      ;//图像坐标
        UINT nLayerPtIndex ;//在圈中的索引号
    };


    //@功 能:在角点数组中, 查找左上角的匹配候选点。
    //@参 数:turnPoints, 角点数组
    // ptG, 重心坐标
    // nTurnPoints, 角点个数
    //@返回值:返回左上角的角点的数组索引号
    // -1, 则未找到左上角的角点。
    //@说 明: 该函数被函数DoSubAreaCornerMatch调用。
    //
    //在以圈内重心G为原点, 垂直向上为Y的坐标系内,
    // ↑y
    // |
    //―+―→x
    // |0
    //查找位于左上角的角点, 该角点的特征是处于2,3象限, 且与X轴正向夹角
    //最小
    //int MatchLeftTopCorner(const TurnPoint* turnPoints, POINT& ptG, int nTurnPointNumber);
     int MatchLeftTopCorner(const TurnPoint* turnPoints, int nTurnPointNumber);


   //@功能:在一圈点中查找拐点
   //@参数:pPts, 圈上点的数组,已按照方向有序排列
   //      nPtNumber, 圈上点的数目
   //     pTurnpoint,保存拐点的数组
   //     nExpectedNum, 拐点数组的元素个数
   //@返回:拐点数目
   int SearchTurnPoints(const POINT* pPts, int nPtNumber, TurnPoint* pTurnpoint, int nExpectedNum);


   //@功能:二值化图片
   //@参数:srcImage, 原始图片
   //      bitImage, 输出的二值化位图
   void BinarizeImage(const CImageFrame& srcImage, const CImageFrame& maskImage, CBitFrame& bitImage);

   //@功能:图像中角点与屏幕中校正点的匹配
   //@参数:
   // corners, 已知视频图像中的的角点坐标数组
   // vecImage2ScreenMap, 每个视频图像中的角点到屏幕校正点编号的映射数组。
   BOOL DoCornerMatch(const std::vector<TImageCalibPoint>& corners, std::vector<int>& vecImage2ScreenMap);

   //@功能:过滤不合格的边界点
   //@返回:合格的边界点个数
   int FilteroutUnqualifiedOutlayerVertex(TVertex* pVertex, int nVertexCount);

    //@功能:YUV分量累加黑白图像
    void Covert2MonochromeFrame_YUV(const CImageFrame& YUVFrame, CImageFrame& monoFrame)
    {
        UINT nImageWidth = YUVFrame.Width();
        UINT nImageHeight = YUVFrame.Height();
        monoFrame.SetSize(nImageWidth, nImageHeight, YUVFrame.BytesPerPixel());

        const BYTE* pYUVData = YUVFrame.GetData();
        BYTE* pMonoData = monoFrame.GetData();
        for(UINT row = 0; row < nImageHeight; row++)
        {
            for(UINT col = 0; col < nImageWidth; col++)
            {

                if(col % 2 ==1)
                {
                    //
                    int nSum = (int)pYUVData[0] + (int)pYUVData[1] + (int)pYUVData[3];

                    *pMonoData = (BYTE)(((int)pYUVData[0] + (int)pYUVData[1] + (int)pYUVData[3]) /3);
                }
                else
                {
                    *pMonoData = (BYTE)(((int)pYUVData[1] + (int)pYUVData[2] + (int)pYUVData[3]) /3);
                }

                pMonoData++;

                if(col % 2 == 0)
                {
                    pYUVData += 4;
                }

            }//foreach col
        }//foreach row
    }

    //@功能:预处理正图案画面,使得画面中的黑白对比更加强烈
    //@参数:patternFrame, 源图
    // maskFrame, 屏蔽图
    // bkgrndFrame,背景图
    // outFrame, 输出的图片对象
    void PreProcessPatternImage(const CImageFrame& patternFrame, const CImageFrame& maskFrame, const CImageFrame& bkgndFrame, CImageFrame& outFrame)
    {

        CImageFrame srcImage = patternFrame;
        int nWidth =  srcImage.Width();
        int nHeight = srcImage.Height();
        outFrame.SetSize(nWidth, nHeight, 1);
        outFrame.Clear();

        BYTE* pDestData = outFrame.GetData();
        const BYTE* pSrcData   = srcImage.GetData();
        const BYTE* pBkgndData = bkgndFrame.GetData();
        const BYTE* pMaskData  = maskFrame.GetData();

        int nLoop = outFrame.Size();

        //差分处理
        while(nLoop--)
        {
            if(*pMaskData == 0xFF)
            {

                BYTE srcData = *pSrcData;
                BYTE bkData  = *pBkgndData;
                BYTE destData;

                BYTE subtractBkData = (bkData >> 2) * 3;

                if(srcData < subtractBkData)
                {
                    destData = 0x00;
                }
                else
                {
                    destData = srcData - subtractBkData;
                    //if(destData < 5)//差分值小于10可以认为是噪声
                    //{
                    //    destData = 0;
                    //}
                }

                *pDestData = destData;
            }

            pDestData++;
            pSrcData ++;
            pBkgndData ++;
            pMaskData++;

        }//while()

        //加大差分图的对比度
        MaximizeContrastness(outFrame);

    }

    //调试函数
    //@功能:测试亮度自动调节功能
    virtual void TestAutoBrightnessControl(CHANGECAMERAPARAMSPROC ChangeCameraParamsProc = NULL, LPVOID lpCtx = NULL);

    //@功能:查看校正图案    
    virtual void ViewCalibratePattern();


    class CDebugWindow:public CWindowImpl<CDebugWindow>
    {
    public:

        BEGIN_MSG_MAP(CDebugWindow)
            MESSAGE_HANDLER(WM_PAINT, OnPaint)
            /*MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
            MESSAGE_HANDLER(WM_CREATE, OnCreate)
            MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
            MESSAGE_HANDLER(WM_FILE_END,OnPlayFileEnd)
            MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
            COMMAND_ID_HANDLER(ID_FULL_SCREEN,OnFullScreen)
            COMMAND_ID_HANDLER(ID_OPEN_CFG_FILE,OnOpenCfgFile)
            MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
            MESSAGE_HANDLER(WM_NCDESTROY, OnNcDestroy)*/
        END_MSG_MAP()

        LRESULT PrintFrame(const CImageFrame& frame, BOOL bExpand = FALSE/*灰度增强显示*/)
        {
            if(m_hWnd == NULL) return E_FAIL;
            UINT nImageWidth = frame.Width();
            UINT nImageHeight = frame.Height();

            if(m_oDib.GetWidth() != nImageWidth
                ||
                m_oDib.GetHeight() != nImageHeight)
            {
                if(!m_oDib.Create(nImageWidth, nImageHeight))
                {

                    return -1L;
                }

            }

            BYTE BytePerPixel = m_oDib.GetBiBitCount()>>3;
            BYTE* pDestData = (BYTE*)m_oDib.GetBits() + (nImageHeight - 1)*nImageWidth*BytePerPixel ;
            const BYTE* pData = frame.GetData();
            int nLineBytes = nImageWidth*BytePerPixel;


            BYTE min=0, max=0;
            BYTE range =0;
            if(bExpand)
            {
                min = frame.Min();
                max = frame.Max();

                if(min == max) bExpand = FALSE;
                range = max - min;
            }

            for(UINT i=0; i<nImageHeight; i++)
            {
                for(UINT j=0; j<nImageWidth; j++)
                {
                    int val = pData[0];
                    if(bExpand)
                    {
                        val = (val-min)*255/range;
                    }

                    pDestData[0] = (BYTE)val;//B
                    pDestData[1] = (BYTE)val;//G
                    pDestData[2] = (BYTE)val;//R

                    pDestData += BytePerPixel;
                    pData ++;
                }

                pDestData -= nLineBytes * 2;
            }


            HDC hDC = GetDC();
            RECT rcClient;
            GetClientRect(&rcClient);

            m_oDib.Draw(hDC,
                rcClient.right - rcClient.left,
                rcClient.bottom - rcClient.top
                );

            ReleaseDC(hDC);

            return 0;

        }

        //@说明:编号为vecNo[i]的校正点对应的视频坐标为vecPts[i]。
        LRESULT PrintPtNo(const std::vector<POINT>& vecPts, const std::vector<int>& vecNo)
        {
            HDC hDC = GetDC();
            LOGFONT lf;
            memset(&lf, 0, sizeof(LOGFONT)); // Zero out the structure.
            lf.lfHeight = 12; // Request a 12-pixel-height font.
            lstrcpy(lf.lfFaceName, _T( "Arial ")); // Request a face name "Arial ".
            HFONT font = ::CreateFontIndirect(&lf) ; // Create the font.
            HFONT fontOld = (HFONT)::SelectObject(hDC, font);
            ::SetBkMode (hDC, TRANSPARENT);
            ::SetTextColor(hDC, RGB(255,0,0));

            for(unsigned int i=0; i < vecPts.size(); i++)
            {
                CAtlString strText;
                strText.Format(_T("%d"), vecNo[i]);

                ::TextOut(hDC,vecPts[i].x, vecPts[i].y, strText, strText.GetLength());
                ::SetPixel(hDC, vecPts[i].x, vecPts[i].y,RGB(0,255,0));
            }

            ::SelectObject(hDC, fontOld);
            ::DeleteObject(font);
            ReleaseDC(hDC);
            return 0;
        }



    protected:

        LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
        {
            // TODO: Add your message handler code here and/or call default
            PAINTSTRUCT ps;

            HDC hDC = ::BeginPaint(m_hWnd,&ps);
            RECT rcClient;
            GetClientRect(&rcClient);

            EndPaint(&ps);
            bHandled = TRUE;
            return 0;
        }


        //DIB对象
        CDib m_oDib;
    };

    static ULONG _stdcall SimulatedCalibrateProc(LPVOID lpCtx);
    //{

    //    CAutoCalibratorImpl2* pCalibrator = reinterpret_cast<CAutoCalibratorImpl2*>(lpCtx);

    //    //生成调试窗体
    //    int nCaptionHeight = GetSystemMetrics(SM_CYCAPTION);
    //    int nFrameCx = GetSystemMetrics(SM_CXFRAME);
    //    int nFrameCy = GetSystemMetrics(SM_CYFRAME);

    //    RECT rcPosition;
    //    rcPosition.left = 0;
    //    rcPosition.top = 0;

    //    rcPosition.right = 640;
    //    rcPosition.bottom = 480;

    //    rcPosition.right += nFrameCx*2;
    //    rcPosition.bottom += nCaptionHeight + nFrameCy*2;


    //    pCalibrator->m_oDebugWindow.Create(NULL, rcPosition, _T("Dbg Window"), WS_POPUP | WS_OVERLAPPEDWINDOW | WS_VISIBLE);


    //    pCalibrator->m_oDebugWindow.SetWindowPos(
    //        HWND_TOPMOST,
    //        rcPosition.left,
    //        rcPosition.top,
    //        rcPosition.right - rcPosition.left,
    //        rcPosition.bottom - rcPosition.top,
    //        SWP_SHOWWINDOW);

    //    const BITMAPINFOHEADER* bmpinfoHeader = pCalibrator->m_oAVIInput.GetAVIFormat();
    //    if(bmpinfoHeader == NULL)
    //    {
    //        return -1;
    //    }

    //    CImageFrame mjpgFrame;
    //    CImageFrame grayFrame;

    //    mjpgFrame.SetSize(bmpinfoHeader->biWidth, bmpinfoHeader->biHeight, bmpinfoHeader->biBitCount >> 3);
    //    grayFrame.SetSize(bmpinfoHeader->biWidth, bmpinfoHeader->biHeight, 1);

    //    CImageFrame onlineScreenArea;
    //    BYTE initValue = 0x00;
    //    onlineScreenArea.SetSize(bmpinfoHeader->biWidth, bmpinfoHeader->biHeight, 1, &initValue);
    //    FillPolygon(
    //        onlineScreenArea.GetData(),
    //        bmpinfoHeader->biWidth,
    //        bmpinfoHeader->biHeight,
    //        &pCalibrator->m_vecDbgOnlineScreenVertices[0],
    //        pCalibrator->m_vecDbgOnlineScreenVertices.size(),
    //        255,
    //        TRUE);

    //    Debug_SaveImageFrame(onlineScreenArea, _T("OnlineScreenArea.jpg"));


    //     CMJPG_Decoder m_oMJPGDecoder;//MJPG解码器

    //    ::SetTimer(NULL, 1, 10, NULL);

    //    MSG msg;
    //    while(::GetMessage(&msg, NULL, 0, 0 ))
    //    {

    //        switch(msg.message)
    //        {
    //        case WM_TIMER:
    //            {
    //                LONG lBytes = 0;
    //                LONG lSamples = 0;
    //                BOOL bRet = FALSE;

    //                bRet = pCalibrator->m_oAVIInput.Read((BYTE*)mjpgFrame.GetData(), mjpgFrame.Size(), &lBytes, & lSamples);
    //                if(!bRet)
    //                {

    //                    PostThreadMessage(::GetCurrentThreadId(), WM_QUIT, 0 ,0);
    //                    break;
    //                }

    //                int nMJPGDataLength = 0;
    //                m_oMJPGDecoder.ProcessData(mjpgFrame.GetData(), lBytes,(BYTE *)grayFrame.GetData(), &nMJPGDataLength);
    //                
    //                //用在线的屏蔽图做与运算
    //                grayFrame &= onlineScreenArea;

    //                bRet = pCalibrator->FeedImage(&grayFrame);
    //                if(!bRet)
    //                {
    //                    //pCalibrator->m_oDebugWindow.DestroyWindow();
    //                    PostThreadMessage(::GetCurrentThreadId(), WM_QUIT, 0 ,0);
    //                }

    //            }

    //            break;


    //        default:
    //            TranslateMessage(&msg);
    //            DispatchMessage(&msg);

    //        }//switch

    //    }//while


    //    pCalibrator->m_oDebugWindow.DestroyWindow();
    //    pCalibrator->m_bIsSimulatedCalibrating = FALSE;
    //    pCalibrator->m_bIsWorking = FALSE;
    //    pCalibrator->m_oAVIInput.Close();

    //    return 0;

    //}

    //@功能:返回8位格式的屏蔽位图
    virtual const CImageFrame& GetMaskFrame() const
    {
        //return this->m_oScreenMaskFrame;
        //多屏模式下，返回所有屏幕的屏蔽图合成的总屏蔽图
        return this->m_oAllMonitorMaskFrame;
    }

	//@功能:返回8位格式的静态屏蔽图
	virtual  const CImageFrame& GetMaskFinderFrame(CImageFrame& maskframe) ;

    //@功能:返回光点位置到屏幕编号的映射数组
    virtual const CImageFrame& GetScreenMap() const
    {
        return this->m_oScreenMap;
    }


    virtual void SetDebugLevel(ECalibDebugLevel eDebugLevel)
    {
        m_eDebugLevel = eDebugLevel;
    }

    virtual SIZE GetScreenSize()const
    {

        SIZE szScreen;
        szScreen.cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        szScreen.cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        return szScreen;
    }


    virtual SIZE GetImageSize()const
    {
        SIZE szImage;
        szImage.cx = m_oAllMonitorMaskFrame.Width() ;
        szImage.cy = m_oAllMonitorMaskFrame.Height();
        return szImage;
    }

    //@功能:返回调试用录像文件完整路径
    virtual const TCHAR* GetDebugVideoFilePath()const
    {
        return m_strDebugVideoFullPath;
    }

protected:
    BOOL              m_bIsWorking     ;//正在校正进行中标志
    CAutoCalibrateWnd m_AutoCalibrateWnd   ;//自动校正窗体
    BOOL              m_nCalibratePtNumber ;//校正点的数目。

    CHANGECAMERAPARAMSPROC m_fpChangeCameraParams;//改变摄像头参数的回调函数指针
    LPVOID                 m_lpCtx               ;///改变摄像头参数的回调函数上下文数据


    //@功能:将屏幕区域划分为多个多边形区域
    void PartitionScreenArea();


    //@功能:屏幕区域搜索结束后的后处理函数。主要有计算屏幕的完整屏蔽图、计算旋转矩阵。
    //@参数:nImageWidth, 视频图像宽度
    //      nImageHeight,视频图像高度
    BOOL OnPostSearchScreenBoundary(int nImageWidth, int nImageHeight);


    //@功能:激励"屏幕区域搜索"子状态机, 使之运行。
    //@功能:grayImage, 灰度图片
    //ESubStateMachineStatus RunSubStateMachine_SearchScreenArea(const CImageFrame& grayImage, ESearchScreenPart eScreenPart);



    //@功能: 当前屏幕收集校正数据成功
    //@说明: 继续下一个屏幕的校正，如果已经是最后一个屏幕了，则结束校正，通知主窗体。
    void OnMonitorCalibrateDataReady();

    //@功能: 当前屏幕采集校正数据失败
    //@说明: 如果尝试次数小于设定的最大尝试次数,则继续尝试；若已经尝试了最大次数，则结束校正，通知主窗体校正失败。
    void OnMonitorCollectDataFail();

    //@功能:合并边界校正点和屏幕内部的校正点, 
       void OnPostCalibrate();


    //@功能:准备当前显示屏幕的校正数据
    void CollectMonitorCalibrateData();


    std::vector<TBorderMarker> m_vecBorderCalibratePts      ;//在摄像头视频中的边界点的坐标,最后的结果应该按照从左上角开始,顺时针方向排列(视频坐标)
    //std::vector<TBorderMarker> m_vecUpperBorderCalibratePts ;//在摄像头视频中上半部边界点的坐标,最后的结果应该按照从上部左上角开始,顺时针方向排列(视频坐标)
    //std::vector<TBorderMarker> m_vecLowerBorderCalibratePts ;//在摄像头视频中下半部边界点的坐标,最后的结果应该按照从下部左上角开始,顺时针方向排列(视频坐标)
    BOOL m_bBorderCalibratrPtsIsValid                       ;//边界校正点坐标正确标志


    //POINT  m_ptUpperCentroid                           ;//上部全局重心
    //POINT  m_ptLowerCentroid                           ;//下部全局重心
    //POINT  m_ptScreenCentroid                          ;//全部的重心

    POINT  m_ptRoationCenter                           ;//图像旋转中心
    double m_dbRotationMatrix[2][2]                    ;//将校正图案调整为水平和垂直方向的旋转矩阵。


    CWordFrame  m_oBlackBoardAccFrame                  ;//全屏黑屏帧累加图
    CWordFrame  m_oWhiteBoardAccFrame                  ;//全屏白屏帧加图
    CImageFrame m_oInitialScreenMask                   ;//初始屏幕区域屏蔽图

    CWordFrame  m_oUpperBorderFrame                    ;//上部边界轮廓图
    CWordFrame  m_oLowerBorderFrame                    ;//下部边界轮廓图


    CImageFrame m_oUpperHalfMaskFrame                  ;//上部屏蔽图
    CImageFrame m_oLowerHalfMaskFrame                  ;//下部屏蔽图
    CImageFrame m_oScreenMaskFrame                     ;//屏幕屏蔽图


    //    CImageFrame m_oLowerSrcFrame                       ;//下部校正图案
    CImageFrame   m_oPatternFrame                         ;//校正图案黑白图片
    CImageFrame   m_oRawPatternFrame                      ;//原始校正图案黑白图片备份
     
    __int64     m_liImageGrdientEnergy                 ;//图片的能量梯度


    std::vector<TImageCalibPoint> m_vecInteriorCalibPoint ;//在视频中看到的内部校正点数组
    int m_nInteriorCalibPtNumber                          ;//内部校正点数目



    //自动校正输出结果
    std::vector<TCalibCoordPair> m_vecBorderCalibMap      ;//屏幕边界上的校正点,从屏幕坐标到视频坐标的映射。
    std::vector<TCalibCoordPair> m_vecInteriorCalibMap    ;//屏幕内部的校正点,从屏幕坐标到视频坐标的映射。

    std::vector<TCalibCoordPair> m_aryAllCalibMap         ;//所有校正点按照屏幕坐标从上到下, 从左到右的顺序片列

    CImageFrame m_oBackgroundFrame                       ;//背景图片帧, 2012/03/08, 屏幕黑屏时, 摄像头中看到的图像。

    //<<调试变量
   // CImageFrame m_oDebugBorderSquareFrame;
    //>>调试变量



    static const int MAX_STAGE_TRY_TIMES = 2;
    static const int DISPLAY_AREA_MININUM_LUMA = 20;//显示区域最小亮度

    //<<2019/11/11, 存在不同帧率的摄像头，以及部分品牌投影机显示延迟问题的存在。
    //将延迟等待的帧数改为绝对时间
    CWaitTimer m_oWaitTimer;
    static const DWORD MAX_NEED_WAIT_TIME = 250;//校正阶段之间需要等待的时间间隔, 单位:ms
    static const DWORD IR_LED_FLASH_TIME   = 3000;//红外通信灯闪烁时间

    void InitWaitTimer(DWORD dwTimeMagnification, DWORD dwVideoDisplayDelay);



    //校正阶段枚举值
    enum E_CalibrateStage
    {
        E_AUTO_CALIBRATE_START = 0,//自动校正开始
        E_AUTO_CHANGE_BRIGHTNESS_1 = 1,//自动调整亮度
        //E_SEARCH_FULL_SCREEN_MASK_WHITEBOARD = 2,//搜索屏幕区域, 打印黑屏
        //E_SEARCH_FULL_SCREEN_MASK_BLACKBOARD = 3,//搜索屏幕区域, 打印白屏

        E_SEARCH_SCREEN_AREA = 2, //定位屏幕区域

        E_AUTO_CHANGE_BRIGHTNESS_2 = 4,//自动调整亮度

        //E_SEARCH_SCREEN_UPPER_MASK_AREA      = 5,//采用打印出上半部边界白点的方式，搜索上半部屏蔽区
        //E_SEARCH_SCREEN_LOWER_MASK_AREA      = 6,//采用打印出下半部边界白点的方式，搜索下半部屏蔽区
        E_SEARCH_SCREEN_IMAGE_BOUNDARY = 5,//搜索屏幕图像的边界

        E_SEARCH_BACKGROUND = 7,//搜索背景

        E_DETECT_ROTATION_1 = 8,//采用采用打出上部白块的方式, 搜寻上半部屏蔽区
        E_DETECT_ROTATION_2 = 9,//采用采用打出下部白块的方式, 搜寻下半部屏蔽区

        E_PRINT_PATTERN   = 10,//打印显示图案
        E_WAIT_PATTERN    = 11,//等待图案
        E_PROCESS_PATTERN                  = 12,//处理校图案

        E_START_MASKING                    = 13,//开始静态屏蔽
        E_WAIT_MASKING                     = 14,//等待静态屏蔽
        E_END_MASKING                      = 15,//结束静态屏蔽

        E_AUTO_CALIBRATE_END               = 16,//自动校正结束
        E_AUTO_CALIBRATE_STOPPED           = 17,//自动校正完成
        E_AUTO_CALIBRATE_UNDEF             = -1,
    };


    E_CalibrateStage m_eCalibrateStage ;//自动校正阶段步骤
    //E_SubAreaCalibrateStage m_eSubareaCalibrateStage;//子区域校正阶段步骤

    //CAviWriter m_oAVIRecorder        ;//AVI录制对象
    //CAviReader m_oAVIInput              ;//AVI输入源
    CAVIFile m_oAVIInput;  //AVI输入

    BOOL m_bIsSimulatedCalibrating      ;//正在模拟校正。
    HANDLE m_hSimulatedCalibrateThread  ;//模拟校正线程
    DWORD m_dwSimulatedCalibrateThreadId;//模拟校正线程id
    CDebugWindow m_oDebugWindow         ;//调试输出窗体


    //int m_nMaskAreaPixelCount ;//屏蔽区域像素尺寸
    //BOOL m_bCalibrateSuccess ;
    HWND m_hNotifyWnd                  ;//通知窗体句柄


    EAutoCalibrateError m_eCalibrateError;

    struct Vec
    {
        POINT ptFrom;
        POINT ptTo;
    };


    UINT m_nCurrentSubAreaIndex;

    ECalibDebugLevel m_eDebugLevel;

    UINT m_uMonitorWidth;//屏幕宽度
    UINT m_uMonitorHeight;//屏幕高度



    BOOL m_bDisplayPatternAgainWhenFailed;//当此校正失败后, 下次是否再次显示校正图案,


    //<<2013/03/07, 校正图案对象
    //校正方案枚举值
    E_AutoCalibratePattern m_ePattern;

    //校正图案对象
    //CArithSeriesRowPattern m_oCalibratePattern;
    CCirclePattern m_oCalibratePattern;

    BOOL m_bSaveInermediatFile;//是否保存自动校正中间文件标志
    BOOL m_bRecordVideo;

    BOOL m_bEnableOnlineScreenArea;
    //>>

    //<<2013/08/19
    static const BYTE MINIMUM_GRAY_VALUE = 64;
    //>>
 
    CAtlString m_strImageOutputBaseDir   ;//调试图片输出根目录
    CAtlString m_strCurrentImageOutputDir;//调试图片输出目录名称
    CAtlString m_strDebugVideoFullPath   ;//调试用视频存储位置


    //功能:亮度控制回调函数
    //@参数:bInc, TRUE:增加亮度，FALSE:减少亮度
    //      absDiff, 亮度差值绝对值
    //      lpCtxData,上下文数据
    static bool BrightnessCtrlCallback(bool bInc, BYTE absDiff, BYTE nIndex, LPVOID lpCtxData);

    CAutoBrightnessRegulator<BYTE> m_AutoBrightnessRegulator;//自动亮度调节器

    BOOL m_bTestAutoBrightnessCtrlMode  ;//测试自动亮度调节模式

    //@功能:生成所有屏幕区域组成的总屏蔽图
    void ProduceAllMonitorMaskFrame();

    //@功能:生成光点到屏幕区号的映射数据
    void ProduceScreenMap();

    COLORREF m_clrGridHighlight;//黑白方格中，白块的显示颜色

    //<<2014/04/09
    BOOL  m_bUseOldMethod;//使用通过显示上下白块的方式来计算旋转方向的标志。
    Vec m_vRotation      ;//屏幕图像在图像中的偏转位置矢量

    //CDispMonitorFinder m_oMonitorFinder;

    //每个显示器对应一个校正结果
    //校正结果
    struct TCalibResults
    {
        std::vector<TCalibCoordPair> calibMap ;
        CImageFrame                 maskFrame ;
        int                        circleRadius;
    };
    
    //映射光点位于哪个屏幕区域的映射数组，用灰度图片数据结构来存储。
    //在位置(x,y)处的像素值v的每位1表示所处的屏幕区号
    //例如,
    //00000001,表示处于屏幕0
    //00000011,表示同时处于屏幕0和1
    //最大对应8个屏
    CImageFrame m_oScreenMap;
    
    std::vector<TCalibResults> m_vecMonitorCalibResults;//校正结果

    int m_nDispMonitorId; //显示器Id号
    std::vector<TScreenInfo> m_vecScreenInfos;
    TScreenInfo m_CurrentMonitorInfo;//当前显示器信息
   
    CImageFrame m_oAllMonitorMaskFrame;//所有监视器的屏蔽图，由各个监视器的屏蔽图组合而成

    TCalibData m_tCalibData;//输出的校正数据

    //<<added by toxuke@gmail.com, 2014/12/09
    TStaticMaskingParams m_tStaticMaskingParams;//静态屏蔽参数
    CStaticMaskFinder    m_oStaticMaskFinder   ;//静态查找器
    //>>
    //<<added by toxuke@gmail.com, 2014/12/16


//    AutoCalibrateImageParamsList m_oImageParamsList        ;//自动定位参数列表

    AutoCalibrateParamsList      m_oautocalibrateparamslist; //自动校正参数列表

    CMonitorAreaLocator          m_oMonitorAreaLocator     ;//屏幕区域定位器
    CMonitorBoundaryFinder       m_oMonitorBoundaryFinder  ;//屏幕边界查找器
    CAreaSplitter                m_oAreaSplitter           ;//画面区域划分器。
    int m_nTryTimes   ;//校正次数,从1开始计数
    int m_nMaxTryTimes;//最大尝试次数
    static const int MAX_TRY_TIMES = 4;

     std::vector<PolygonVertices> m_vecPolygons;

public:
    //工作模式
    enum EWorkMode
    {
        E_WORK_MODE_AUTO_CALIBRATE,//自动校正模式
        E_WORK_MODEL_AUTO_MASKING ,//自动屏蔽模式
    };
    EWorkMode    m_eWorkMode;


    //自动屏蔽阶段
    enum EAutoMaskStage
    {
        E_AUTO_MASK_START                        = 1,//自动屏蔽开始
        E_AUTO_MASK_ADJUST_BRIGHTNESS            = 2,//自动屏蔽"调整亮度"
        E_AUTO_MASK_SEARCH_SCREEN_AREA           = 3,//自动屏蔽"搜索屏幕区域"
        E_AUTO_MASK_SEARCH_SCREEN_IMAGE_BOUNDARY = 4,//自动屏蔽"搜索屏幕边界"
        E_AUTO_MASK_SEARCH_BACKGROUND            = 5,//自动屏蔽"搜索屏幕背景"
        E_AUTO_MASK_SERACH_UPPER_HALF_MASK       = 6,//自动搜索"上半部屏蔽图"
        E_AUTO_MASK_SERACH_LOWER_HALF_MASK       = 7,//自动搜索"下半部屏蔽图"
        E_AUTO_MASK_STATIC_START                 = 8,//"静态屏蔽"开始
        E_AUTO_MASK_STATIC_MASKING               = 9,//自动屏蔽"静态屏蔽"
        E_AUTO_MASK_END                          = 10, //自动屏蔽"结束"阶段
        E_AUTO_MASK_STOPPED                      = 11 //自动屏蔽"停止"阶段
    }m_eAutoMaskStage;


    //@功能:开始自动屏蔽,供外部模块调用
    virtual BOOL StartMasking(const TAutoMaskingParams& autoMaskingParams, const TStaticMaskingParams& staticMaskingParams, int nSrcImageWidth, int nSrcImageHeight);

    //@功能:给自动屏蔽状态机投喂图片。
    //@返回值:异常返回FALSE
    BOOL FeedImage_AutoMask(const CImageFrame* pYUVFrame);

    //@功能:自动屏蔽结束事件
    void EndAutoMasking();

   EAutoMaskingError m_eAutoMaskingError;//自动屏蔽结果状态 

   BOOL m_bRestoreLanguageBar;//是否恢复语言栏标志

   BOOL m_bRestoreTaskbar;//是否恢复状态栏标志



   //调试工具，2017/04/11
   void  DebugTool_LoadCalibrateData(
        const char* szCalirateDataFileName,//校正数据文件
        const double rotationMatrix[2][2],//坐标旋转矩阵
        const POINT& ptCenter);


   //调试工具,记录模拟参数
   //<<dbg
   void DbgDumpSimulateParams(const TAutoCalibrateParams& autoCalibrateParams);
   void DbgDumpPatternInfo(int radius, const RECT& rcArea);
   void DbgLoadSimulateParams(LPCTSTR lpszFileName);
   int  m_nDbgFrameCount;

   std::vector<POINT> m_vecDbgOnlineScreenVertices;
   int m_nDbgPatternRadius;
   RECT m_rcDbgMonitor;
   //dbg>
};