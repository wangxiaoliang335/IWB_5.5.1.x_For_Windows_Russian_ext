#include "stdafx.h"
#include "..\headers.h"

#include "EM.h"
#include <limits>
#include "..\inc\SortVertex.h"
#include "..\inc\ExpandPolygon.h"

void DebugHeap()
{
    DWORD dwFlags = 0x0;
    std::vector<int> vecTemp;
    vecTemp.resize(10000);
    BOOL bRet = HeapValidate(GetProcessHeap(), dwFlags, NULL);

    vecTemp[0] = 10;

    if (!bRet)
    {
        ::DebugBreak();
    }

}
//@功能:返回画面的梯度能量
//@参数:pImageData, 指向图像像素数据的指针
//      nWidth, 图像宽度
//      nHeight, 图像高度
//@说明:画面越清晰，则梯度能量越大。
__int64 GradientEnergy(const BYTE* pImageData, int nWidth, int nHeight)
{
    __int64 e = 0;
    for (int y = 0; y < nHeight; y++)
    {
        for (int x = 0; x < nWidth; x++)
        {
            int dx = int(pImageData[y*nWidth + x + 1]) - int(pImageData[y*nWidth + x]);
            int dy = int(pImageData[(y + 1)*nWidth + x]) - int(pImageData[y*nWidth + x]);
            e += dx*dx + dy*dy;
        }
    }
    return e;
}

//@功能:返回画面的梯度能量，位于屏蔽图内的像素不参与计算。
//@参数:pImageData, 指向图像像素的指针
//      pMask, 指向屏蔽图像素的指针
//      nWidth, 图像宽度
//      nHeight, 图像高度
//@说明:画面越清晰，则梯度能量越大。
__int64 GradientEnergy(const BYTE* pImageData, const BYTE* pMask, int nWidth, int nHeight)
{
    __int64 e = 0;
    for (int y = 0; y < nHeight; y++)
    {
        for (int x = 0; x < nWidth; x++)
        {
            if (0 == pMask[y*nWidth + x]) continue;//屏蔽区以外的点
            int dx = int(pImageData[y*nWidth + x + 1]) - int(pImageData[y*nWidth + x]);
            int dy = int(pImageData[(y + 1)*nWidth + x]) - int(pImageData[y*nWidth + x]);
            e += dx*dx + dy*dy;

        }

    }
    return e;
}


BYTE GetImageBinarizeThreshold_DoublePeak(const CImageFrame&  srcImage, const CImageFrame& maskImage)
{
    int hist[256];
    memset(hist, 0, sizeof(hist));
    const BYTE* pSrcData = srcImage.GetData();
    const BYTE* pMaskData = maskImage.GetData();
    int nPixelCount = srcImage.GetPixelCount();
    for (int i = 0; i < nPixelCount; i++)
    {
        BYTE cData = *pSrcData;

        if (*pMaskData == 0xFF)
        {
            hist[cData] ++;
        }
        pSrcData++;
        pMaskData++;
    }

    BYTE threshold = GetBinarizeThreshold(hist);


    return threshold;
}


BYTE GetImageBinarizeThreshold_Ostu(const CImageFrame&  srcImage, const CImageFrame& maskImage, bool bDiscardZeroPixel = false)
{
    int hist[256];
    memset(hist, 0, sizeof(hist));
    const BYTE* pSrcData = srcImage.GetData();
    const BYTE* pMaskData = maskImage.GetData();
    int nPixelCount = srcImage.GetPixelCount();
    int nPixelInMaskArea = 0;
    for (int i = 0; i < nPixelCount; i++)
    {
        BYTE cData = *pSrcData;

        if (*pMaskData == 0xFF)
        {
            hist[cData] ++;
            nPixelInMaskArea++;
        }
        pSrcData++;
        pMaskData++;
    }

    //if (bCanDiscardZeroPixel && hist[0] > (nPixelInMaskArea*1/5))
    //{//使能废弃像素值为0的像素,并且像素值为0的像素个数大于20%。
    //    hist[0] = 0;
    //}

    if (bDiscardZeroPixel)
    {
        hist[0] = 0;
    }

    BYTE threshold = FindThreshold_Ostu(hist);


    return threshold;
}



//@功能:复位定位器状态
//@参数:nImageWidth, 图像宽度
//      nImageHeight,图像高度
//      hDisplay, 画面显示窗体
//     rcMonitor, 当前屏幕区的矩形区

void CMonitorAreaLocator::Reset(int nImageWidth, int nImageHeight, HWND hDisplayWnd, const RECT& rcMonitor, ECalibDebugLevel eDebugLevel)
{
    m_hDisplayWnd = hDisplayWnd;
    m_rcMonitor = rcMonitor;
    m_nSubAreaId = 0;
    m_bSuccess = FALSE;
    m_eRunStage = E_RUN_STAGE_BEGIN;

    //m_nStageWaitCount = 0;    

    int nMonitorWidth = rcMonitor.right - rcMonitor.left;
    int nMonitorHeight = rcMonitor.bottom - rcMonitor.top;

    int nHalfWidth = nMonitorWidth >> 1;
    int nHalfHeight = nMonitorHeight >> 1;

    //初始化四个子区域矩形坐标
    m_SubAreaRect[0].left = rcMonitor.left;
    m_SubAreaRect[0].right = rcMonitor.left + nHalfWidth;
    m_SubAreaRect[0].top = rcMonitor.top;
    m_SubAreaRect[0].bottom = rcMonitor.top + nHalfHeight;


    m_SubAreaRect[1].left = rcMonitor.left + nHalfWidth;
    m_SubAreaRect[1].right = rcMonitor.right;
    m_SubAreaRect[1].top = rcMonitor.top;
    m_SubAreaRect[1].bottom = rcMonitor.top + nHalfHeight;


    m_SubAreaRect[2].left = rcMonitor.left;
    m_SubAreaRect[2].right = rcMonitor.left + nHalfWidth;
    m_SubAreaRect[2].top = rcMonitor.top + nHalfHeight;
    m_SubAreaRect[2].bottom = rcMonitor.bottom;

    m_SubAreaRect[3].left = rcMonitor.left + nHalfWidth;
    m_SubAreaRect[3].right = rcMonitor.right;
    m_SubAreaRect[3].top = rcMonitor.top + nHalfHeight;
    m_SubAreaRect[3].bottom = rcMonitor.bottom;

    //调试级别
    m_eDebugLevel = eDebugLevel;
}

//@功能:根据采样得到的背景图和白屏图的差分图,提取出白屏区域
//@参数:refBkgndFrame, 输入参数, 16bit背景图
//      refSampleFrame,输入参数, 16bit白屏图
//      refMaskFrame,  输出参数, 8Bit屏蔽图, 像素值 > 0 的区域对应白屏区域
static BOOL GenMaskFrame(const CWordFrame& refBkgndFrame, const CWordFrame& refSampleFrame, CImageFrame& refMaskFrame, BOOL bDebug = FALSE)
{
    int nWidth = refBkgndFrame.Width();
    int nHeight = refBkgndFrame.Height();
    int nLoopCount = refBkgndFrame.GetPixelCount();
    refMaskFrame.SetSize(nWidth, nHeight, 1);

    //计算差分图
    CWordFrame diffFrame;
    diffFrame.SetSize(nWidth, nHeight, 2);

    WORD*       pDiffValue = diffFrame.GetData();
    const WORD* pSampleValue = refSampleFrame.GetData();
    const WORD* pBkgndValue = refBkgndFrame.GetData();

    UINT32 dwSum = 0;
    while (nLoopCount)
    {
        WORD wSampleValue = *pSampleValue;

        //注意:
        //
        //由于摄像头的自动增益, 同一个背景处的像素在“背景图”中的亮度要大于其在“白屏图”中的亮度，
        //因此需要适当减弱背景图中像素的亮度
        //
        //这里衰减系数取2/3

        //#define ATTENUATION_COEFFICIENT  3/4
        //#define ATTENUATION_COEFFICIENT  1
       //WORD wBkgndValue   = (*pBkgndValue) * ATTENUATION_COEFFICIENT;
        WORD wBkgndValue = *pBkgndValue;

        WORD wDiffValue = 0;
        if (wSampleValue > wBkgndValue)
        {
            wDiffValue = wSampleValue - wBkgndValue;
        }

        *pDiffValue = wDiffValue;

        pDiffValue++;
        pSampleValue++;
        pBkgndValue++;

        nLoopCount--;
    }

    float fAvgPerPixel = (float)dwSum / (float)(nWidth*nHeight);
    bool debug = false;
    if (fAvgPerPixel > 255)
    {
        debug = true;
    }

    if (bDebug)
    {
        Debug_SaveImageFrame(refSampleFrame, _T("SampleFrame.jpg"));
        Debug_SaveImageFrame(diffFrame, _T("DifferenceImage.jpg"));
    }

    //将16Bit差分图转化为8位位图
    WordFrame2ByteFrame(diffFrame, refMaskFrame);

    //统计直方图
    int hist[256];
    memset(hist, 0, sizeof(hist));
    nLoopCount = refMaskFrame.GetPixelCount();;
    const BYTE* pPixel = refMaskFrame.GetData();
    while (nLoopCount)
    {
        hist[*pPixel] ++;
        pPixel++;
        nLoopCount--;
    }

    //ostu法查找最优门限
    BYTE threshold = FindThreshold_Ostu(hist);
    //threshold = (BYTE)((int)threshold*60/100);//门限降低到60%
    threshold = (BYTE)((int)threshold * 60 / 100);//门限降低到60%

    //二值化屏蔽图
    refMaskFrame.Binarize(threshold);

    //在屏蔽图中只保留最大的白块区域
    int nScreenAreaPixelCount = 0;

    //提取最大的目标,作为屏幕区域
    BOOL bRet = ExtractTheLargestObject(refMaskFrame, NULL, &nScreenAreaPixelCount);

    return bRet;
}

//@功能:处理每一帧灰度图片
//@参数:srcFrame, 图片帧对象
//      bSimulate, 模拟校正标志
//@返回值:TRUE, 正常处理。
//        FALSE,处理错误。
BOOL CMonitorAreaLocator::Process(const CImageFrame& srcFrame, BOOL bSimulate)
{
    m_oWaitTimer.Trigger();
    wchar_t szTmp[1024] = { 0 };
    switch (m_eRunStage)
    {

    case E_RUN_STAGE_BEGIN:
        //打出黑板图案
        if (!bSimulate)
        {
            FillBoard(this->m_hDisplayWnd, BACKGROUND_COLOR, &this->m_rcMonitor);
        }

        //m_nStageWaitCount = 0;
        m_oWaitTimer.Reset();
        m_oBlackBoardAccFrame.SetSize(srcFrame.Width(), srcFrame.Height(), 2);
        m_oBlackBoardAccFrame.Clear();

        m_eRunStage = E_RUN_STAGE_SAMPLE_BLACKBOARD;
        break;

    case E_RUN_STAGE_SAMPLE_BLACKBOARD:

        if (m_oWaitTimer.IsWaitTimeout(BLACK_BOARD_SAMPLE_START_TIME) && !m_oWaitTimer.IsWaitTimeout(BLACK_BOARD_SAMPLE_END_TIME))
        {
            //累加图片
            AccumulateImageFrame(srcFrame, m_oBlackBoardAccFrame);
        }
        //else if(BLACK_BOARD_SAMPLE_END_COUNT < m_nStageWaitCount)
        else if (m_oWaitTimer.IsWaitTimeout(WHITE_BOARD_SAMPLE_END_TIME))
        {

            if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
            {
                Debug_SaveImageFrame(m_oBlackBoardAccFrame, _T("BkgndFrame.jpg"));
            }

            //转入搜索子区域屏蔽图阶段
            m_nSubAreaId = 0;

            if (!bSimulate)
            {
                //子区域填充白色                
                FillBoard(this->m_hDisplayWnd, FOREGROUND_COLOR, &m_SubAreaRect[m_nSubAreaId]);
            }

            // m_nStageWaitCount = 0;
            m_oWaitTimer.Reset();
            m_oWhiteBoardAccFrame.SetSize(srcFrame.Width(), srcFrame.Height(), 2);
            m_oWhiteBoardAccFrame.Clear();

            //转入采样子区域图像状态
            m_eRunStage = E_RUN_STATE_SAMPLE_SUBAREA;
        }

        break;

    case E_RUN_STATE_SAMPLE_SUBAREA:

        //采样子区域白块图像
        if (m_oWaitTimer.IsWaitTimeout(WHITE_BOARD_SAMPLE_START_TIME) && !m_oWaitTimer.IsWaitTimeout(WHITE_BOARD_SAMPLE_END_TIME))
        {
            //累加图片
            AccumulateImageFrame(srcFrame, m_oWhiteBoardAccFrame);
        }
        //else if(WHITE_BOARD_SAMPLE_END_COUNT < m_nStageWaitCount)
        else if (m_oWaitTimer.IsWaitTimeout(WHITE_BOARD_SAMPLE_END_TIME))
        {
            if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
            {
                Debug_SaveImageFrame(m_oWhiteBoardAccFrame, _T("SubAreaSrc.jpg"));
            }

            //生成子区域的屏蔽图
            BOOL bRet = GenMaskFrame(m_oBlackBoardAccFrame, m_oWhiteBoardAccFrame, m_SubAreaMask[m_nSubAreaId], m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_VERBOSE);
            if (!bRet) return FALSE;
            //填补内部空洞.
            CImageFrame& subMaskFrame = m_SubAreaMask[m_nSubAreaId];

            Morph_FillHole(subMaskFrame.GetData(), subMaskFrame.GetData(), subMaskFrame.Width(), subMaskFrame.Height());

            if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
            {
                Debug_SaveImageFrame(m_SubAreaMask[m_nSubAreaId], _T("SubArea.jpg"));
            }

            m_nSubAreaId++;

            if (m_nSubAreaId < SUB_AREA_COUNT)
            {
                if (!bSimulate)
                {
                    //擦除
                    FillBoard(this->m_hDisplayWnd, BACKGROUND_COLOR, &this->m_rcMonitor);
                    //子区域填充白色
                    FillBoard(this->m_hDisplayWnd, FOREGROUND_COLOR, &m_SubAreaRect[m_nSubAreaId]);
                }
                //转入下一子区域的屏蔽图搜索。
                //m_nStageWaitCount = 0;

                m_oWaitTimer.Reset();
                m_oWhiteBoardAccFrame.SetSize(srcFrame.Width(), srcFrame.Height(), 2);
                m_oWhiteBoardAccFrame.Clear();


            }
            else
            {
                if (!bSimulate)
                {
                    //全屏白
                    FillBoard(this->m_hDisplayWnd, FOREGROUND_COLOR, &m_rcMonitor);
                }
                //m_nStageWaitCount = 0;
                m_oWaitTimer.Reset();
                m_oWhiteBoardAccFrame.SetSize(srcFrame.Width(), srcFrame.Height(), 2);
                m_oWhiteBoardAccFrame.Clear();

                m_eRunStage = E_RUN_STAGE_SAMPLE_WHITEBOARD;
            }

        }

        break;

    case E_RUN_STAGE_SAMPLE_WHITEBOARD:

        if (m_oWaitTimer.IsWaitTimeout(WHITE_BOARD_SAMPLE_START_TIME) && !m_oWaitTimer.IsWaitTimeout(WHITE_BOARD_SAMPLE_END_TIME))
        {
            //累加图片
            AccumulateImageFrame(srcFrame, m_oWhiteBoardAccFrame);
        }
        //else if(WHITE_BOARD_SAMPLE_END_COUNT < m_nStageWaitCount)
        else if (m_oWaitTimer.IsWaitTimeout(WHITE_BOARD_SAMPLE_END_TIME))
        {
            //生成子区域的屏蔽图
            BOOL bRet = GenMaskFrame(m_oBlackBoardAccFrame, m_oWhiteBoardAccFrame, m_oMaskFrame, m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG);
            if (!bRet) return FALSE;

            if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
            {
                Debug_SaveImageFrame(m_oMaskFrame, _T("FullMask.jpg"));
            }

            m_eRunStage = E_RUN_STAGE_END;
        }
        break;


    case E_RUN_STAGE_END:

        for (int i = 0; i < SUB_AREA_COUNT; i++)
        {
            m_oMaskFrame |= m_SubAreaMask[i];

        }

        //计算四个区域的重心,
        for (int i = 0; i < SUB_AREA_COUNT; i++)
        {
            int nPixelCount = m_SubAreaMask[i].GetPixelCount();
            const BYTE* pPixel = m_SubAreaMask[i].GetData();
            int nWidth = m_SubAreaMask[i].Width();
            int nHeight = m_SubAreaMask[i].Height();
            wsprintf(szTmp, L" xxxxxx i:%d, nWidth:%d, nHeight:%d\n", i, nWidth, nHeight);
            OutputDebugString(szTmp);

            long nMx = 0, nMy = 0, nMass = 0;
            for (int y = 0; y < nHeight; y++)
            {
                for (int x = 0; x < nWidth; x++)
                {
                    if (*pPixel == 0xFF)
                    {
                        nMx += x;
                        nMy += y;
                        nMass++;
                    }

                    pPixel++;
                }//for-each(x)

            }//for-each(y)


            if (nMass)
            {
                m_SubAreaCentroids[i].x = nMx / nMass;
                m_SubAreaCentroids[i].y = nMy / nMass;
            }
            else
            {
                m_SubAreaCentroids[i].x = -1;
                m_SubAreaCentroids[i].y = -1;
            }
            wsprintf(szTmp, L" xxxxxx i:%d, m_SubAreaCentroids x:%d, m_SubAreaCentroids y:%d\n", i, m_SubAreaCentroids[i].x, m_SubAreaCentroids[i].y);
            OutputDebugString(szTmp);
        }

        //补洞
        FillHole(m_oMaskFrame);

        if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
        {
            Debug_SaveImageFrame(m_oMaskFrame, _T("ResultMask.jpg"));
        }

        m_bSuccess = TRUE;

        break;

    }//switch

    return TRUE;
}


//@功能:显示围绕边界的白色实心圆
//@参数:hWnd, 绘制于其上的窗体句柄
//      CirclePositions,白色实心圆的的外接矩形数组
//      nCircleNumber, 白色实心圆的数目
//@日期:
void  DrawCircleMarkers(HWND hWnd, const RECT* pCirclePositions, int nCircleNumber, COLORREF clrFill = FOREGROUND_COLOR)
{

    HDC hDC = GetDC(hWnd);


    HBRUSH hBrush = ::CreateSolidBrush(clrFill);
    HBRUSH hBrushOld = (HBRUSH)SelectObject(hDC, hBrush);

    for (int i = 0; i < nCircleNumber; i++)
    {
        RECT rc = pCirclePositions[i];

        ScreenToClient(hWnd, (LPPOINT)&rc.left);
        ScreenToClient(hWnd, (LPPOINT)&rc.right);
        Ellipse(hDC, rc.left, rc.top, rc.right, rc.bottom);
    }

    SelectObject(hDC, hBrushOld);
    DeleteObject(hBrush);

    ReleaseDC(hWnd, hDC);

}


//@功能:显示围绕边界的白色实心圆
//@参数:hWnd, 绘制于其上的窗体句柄
//      CirclePositions,白色实心圆的的外接矩形数组
//      nCircleNumber, 白色实心圆的数目
//@日期:
void  DrawCircleMarkers(HWND hWnd, const RECT* pCirclePositions, int nCircleNumber, const double* pDisplayIntensity, COLORREF clrFill = FOREGROUND_COLOR)
{

    HDC hDC = GetDC(hWnd);


    int R = GetRValue(clrFill);
    int G = GetGValue(clrFill);
    int B = GetBValue(clrFill);

    for (int i = 0; i < nCircleNumber; i++)
    {
        //int newR = int(R*pDisplayIntensity[i]);
        //int newG = int(G*pDisplayIntensity[i]);
        //int newB = int(B*pDisplayIntensity[i]);
        int newR = R;
        int newG = G;
        int newB = B;


        COLORREF clrBrush = RGB(newR, newG, newB);

        HBRUSH hBrush = ::CreateSolidBrush(clrBrush);
        HBRUSH hBrushOld = (HBRUSH)SelectObject(hDC, hBrush);

        RECT rc = pCirclePositions[i];

        ScreenToClient(hWnd, (LPPOINT)&rc.left);
        ScreenToClient(hWnd, (LPPOINT)&rc.right);
        Ellipse(hDC, rc.left, rc.top, rc.right, rc.bottom);

        SelectObject(hDC, hBrushOld);
        DeleteObject(hBrush);
    }

    ReleaseDC(hWnd, hDC);

}

//=====CMonitorBoundaryFinder====
//@功能:以指定的参考点为原点建立直角坐标系,按照方位角从小到大的顺序排列各个点。
//@参数:pBorderMarker, 指向边界界桩点的数组
//      nPtNumber, 点的数目
//      ptOrigin, 原点坐标
void SortInDirection(TBorderMarker*  pBorderMarker, int nPtNumber, const POINT& ptOrigin)
{
    //方向矢量
    struct TDirectionVector
    {
        int nQuadrant;//方位所在象限编号(1,2,3,4)
        //方位角始边为x轴正向, 依照象限方位角逐渐增加
        LONG lDx;//相对于原点的水平矢量大小
        LONG lDy;//相对于原点的垂直矢量大小

        TBorderMarker marker;//原始数据.
        //int nPtIndex ;//在原数组中的索引
    };

    std::vector<TDirectionVector>  vecPtDirections;
    vecPtDirections.resize(nPtNumber);

    for (int i = 0; i < nPtNumber; i++)
    {

        TDirectionVector temp;
        temp.marker = pBorderMarker[i];

        temp.lDx = pBorderMarker[i].ptCentroid.x - ptOrigin.x;
        temp.lDy = pBorderMarker[i].ptCentroid.y - ptOrigin.y;
        //temp.nPtIndex = i;


        if (temp.lDx > 0 && temp.lDy >= 0)
        {
            temp.nQuadrant = 1;//QuadrantⅠ第一象限
        }
        else if (temp.lDx <= 0 && temp.lDy > 0)
        {
            temp.nQuadrant = 2;//Quadrant Ⅱ第二象限
        }
        else if (temp.lDx < 0 && temp.lDy <= 0)
        {
            temp.nQuadrant = 3;//Quadrant Ⅲ第三象限
        }
        else
        {
            temp.nQuadrant = 4;//QuadrantⅣ第四象限
        }


        //从前往后寻找插入位置
        int j = 0;
        for (j = 0; j < i; j++)
        {

            //第j个元素的象限 > "插入点"的象限
            //意味着要插在位置j处，原有位置j及其以后的数据后移。
            if (vecPtDirections[j].nQuadrant > temp.nQuadrant)
            {

                break;
            }
            else if (vecPtDirections[j].nQuadrant == temp.nQuadrant)
            {
                LONG crossProduct = temp.lDx * vecPtDirections[j].lDy - temp.lDy*vecPtDirections[j].lDx;

                //以当前矢量为始边, 被比较的矢量为终边, 
                //矢量叉积>0,意味着被比较的矢量的方位角大于当前矢量的方位角，找到了插入位置
                if (crossProduct > 0)
                {
                    break;
                }//if
            }
        }

        for (int k = i; k > j; k--)
        {
            vecPtDirections[k] = vecPtDirections[k - 1];
        }

        vecPtDirections[j] = temp;

    }//for

    for (int i = 0; i < nPtNumber; i++)
    {
        pBorderMarker[i] = vecPtDirections[i].marker;
    }

}


CMonitorBoundaryFinder::CMonitorBoundaryFinder()
    :
    //m_nRunTimes(0),
    m_nFlashTimes(0),
    m_bShowMarker(FALSE),
    m_eDebugLevel(E_CALIB_DEBUG_LEVEL_CONCISE),
    m_hDispWnd(NULL),
    m_MarkerDiameter(INITIAL_MARKER_DIAMETER),
    //m_eScreenPart(E_SEARCH_SCREEN_UPPER),
    //m_bIsSimulateInput(FALSE),
    m_bDone(FALSE),
    m_nHorzSideMarkerNumber(MINIMUM_SIDE_MARKER_NUMBER),
    m_nVertSideMarkerNumber(MINIMUM_SIDE_MARKER_NUMBER)
{

}


CMonitorBoundaryFinder::~CMonitorBoundaryFinder()
{

}

//@功能:复位状态机
//@参数:frameInitalScreenMask,初始的屏幕屏蔽图
//      hDispWnd, 显示窗体
//      ECalibDebugLevel, 调试级别
//      bSimualte, 模拟校正标志
void CMonitorBoundaryFinder::Reset(const CImageFrame& frameInitalScreenMask, HWND hDispWnd, const RECT& rcMonitor, ECalibDebugLevel eDebugLevel, BOOL bSimulate)
{
    int nImageWidth = frameInitalScreenMask.Width();
    int nImageHeight = frameInitalScreenMask.Height();

    m_hDispWnd = hDispWnd;
    m_frameBackground.SetSize(nImageWidth, nImageHeight, 2);
    m_frameForeground.SetSize(nImageWidth, nImageHeight, 2);

    m_rcMonitor = rcMonitor;
    m_eDebugLevel = eDebugLevel;

    int nMonitorWidth = rcMonitor.right - m_rcMonitor.left;
    int nMonitorHeight = rcMonitor.bottom - m_rcMonitor.top;
    int nMarkDiameter = INITIAL_MARKER_DIAMETER;

    //m_nHorzSideMarkerNumber  = HORZ_SIDE_MARKER_NUMBER;
    //m_nVertSideMarkkerNumber = VERT_SIDE_MARKER_NUMBER;


    if (nMonitorWidth >= nMonitorHeight)
    {//宽大于高
        m_nVertSideMarkerNumber = MINIMUM_SIDE_MARKER_NUMBER;

        nMarkDiameter = nMonitorHeight / (2 * m_nVertSideMarkerNumber - 1);

        //nMonitorWidth = (2n-1)*nMarkDiameter
        m_nHorzSideMarkerNumber = (nMonitorWidth / nMarkDiameter + 1) >> 1;

        if (m_nHorzSideMarkerNumber % 2 == 0)
        {//如果水平方向的边界点数目为偶数，则修正为奇数
            m_nHorzSideMarkerNumber--;
        }

    }
    else
    {//高大于宽
        m_nHorzSideMarkerNumber = MINIMUM_SIDE_MARKER_NUMBER;

        nMarkDiameter = nMonitorWidth / (2 * m_nHorzSideMarkerNumber - 1);

        //nMonitorHeight = (2n-1)*nMarkDiameter
        m_nVertSideMarkerNumber = (nMonitorHeight / nMarkDiameter + 1) >> 1;

        if (m_nVertSideMarkerNumber % 2 == 0)
        {//如果水平方向的边界点数目为偶数，则修正为奇数
            m_nVertSideMarkerNumber--;
        }
    }

    InitBoundaryMarkerPositions(m_nHorzSideMarkerNumber, m_nVertSideMarkerNumber, nMarkDiameter);

    m_oWaitTimer.Reset();

    m_nFlashTimes = 0;
    m_bShowMarker = FALSE;


    m_oInitialScreenMask = frameInitalScreenMask;
    m_frameForeground.Clear();
    m_frameBackground.Clear();

    if (!bSimulate)
    {
        FillBoard(m_hDispWnd, BACKGROUND_COLOR, &m_rcMonitor);
    }

    m_bDone = FALSE;
}


//@功能:初始化屏幕上部和下部边界实心圆的位置
//@参数:hWnd, 校正窗体句柄
//      nHorzSideSquareNumber, 水平方向白色小方块的个数。
//      VertSideSquareNumber, 垂直方向白色小方块的个数。
//      nSquareWidth, 实心圆的直径。
//@说明:注意:如果垂直方向的白色实心圆的数目为奇数，则上下两部分有两个点是重合的。
void CMonitorBoundaryFinder::InitBoundaryMarkerPositions(int nHorzSideSquareNumber, int nVertSideSquareNumber, int nDiameter)
{
    //从计算机屏幕的左上角开始，按顺时针方向依次对各个白色小方块编号
    //例如:
    //0     1     2     3
    //                   
    //9                 4
    //                   
    //8    7      6     5
    //

    //RECT rcClient;
    //GetClientRect(m_hDispWnd,&rcClient);

    int i = 0;


    int W = m_rcMonitor.right - m_rcMonitor.left;
    int H = m_rcMonitor.bottom - m_rcMonitor.top;


    int  nHorzIntervalNum = nHorzSideSquareNumber - 1;
    int nVertIntervalNum = nVertSideSquareNumber - 1;
    int _HorzInterval_Integer = (W - nDiameter) / nHorzIntervalNum;//每个方块的水平间距数值的整数部分
    int _HorzInterval_Fraction = (W - nDiameter) % nHorzIntervalNum;//每个方块的水平间距数值的余数部分

    int _VertInterval_Integer = (H - nDiameter) / nVertIntervalNum;//每个方块的水平间距数值的整数部分
    int _VertInterval_Fraction = (H - nDiameter) % nVertIntervalNum;//每个方块的水平间距数值的余数部分

    RECT rcSquare;
    rcSquare.left = m_rcMonitor.left;
    rcSquare.top = m_rcMonitor.top;
    rcSquare.right = rcSquare.left + nDiameter;
    rcSquare.bottom = rcSquare.top + nDiameter;

    int _FractionCount = 0;

    int iIndex = 0;

    int nMarkerNumber = 2 * (nHorzSideSquareNumber - 1) + 2 * (nVertSideSquareNumber - 1);
    m_vecBorderMarkerPositions.resize(nMarkerNumber);


    //顶边
    for (i = 0; i < nHorzIntervalNum; i++)
    {
        m_vecBorderMarkerPositions[iIndex++] = rcSquare;

        rcSquare.left += _HorzInterval_Integer;
        _FractionCount += _HorzInterval_Fraction;
        if (_FractionCount > nHorzIntervalNum)
        {
            rcSquare.left++;
            _FractionCount -= nHorzIntervalNum;
        }

        rcSquare.right = rcSquare.left + nDiameter;
    }

    _FractionCount = 0;


    //右边
    for (i = 0; i < nVertIntervalNum; i++)
    {
        m_vecBorderMarkerPositions[iIndex++] = rcSquare;
        rcSquare.top += _VertInterval_Integer;
        _FractionCount += _VertInterval_Fraction;
        if (_FractionCount > nVertIntervalNum)
        {
            rcSquare.top++;
            _FractionCount -= nVertIntervalNum;
        }

        rcSquare.bottom = rcSquare.top + nDiameter;
    }

    _FractionCount = 0;
    //底边
    for (i = 0; i < nHorzIntervalNum; i++)
    {
        m_vecBorderMarkerPositions[iIndex++] = rcSquare;
        rcSquare.left -= _HorzInterval_Integer;
        _FractionCount -= _HorzInterval_Fraction;
        if (_FractionCount < -nHorzIntervalNum)
        {
            rcSquare.left--;
            _FractionCount += nHorzIntervalNum;
        }

        rcSquare.right = rcSquare.left + nDiameter;
    }


    //左边
    _FractionCount = 0;
    for (i = 0; i < nVertIntervalNum; i++)
    {
        m_vecBorderMarkerPositions[iIndex++] = rcSquare;
        rcSquare.top -= _VertInterval_Integer;
        _FractionCount -= _VertInterval_Fraction;
        if (_FractionCount < -nHorzIntervalNum)
        {
            rcSquare.top--;
            _FractionCount += nHorzIntervalNum;
        }
        rcSquare.bottom = rcSquare.top + nDiameter;
    }


    //计算各个实心圆的显示强度, 四个顶角处的符号显示强度最大, 四条边中间位置处的符号显示最弱
    m_vecDisplayIntensity.resize(nMarkerNumber);


    for (int i = 0; i < nMarkerNumber; i++)
    {
        m_vecDisplayIntensity[i] = 1.0;
    }

    double max_intensity = 1.0;
    double min_intensity = 0.8;



    struct TMarkLineInfo
    {
        int start_index;
        int end_index;
    };


    TMarkLineInfo markLineInfos[] =
    {
        {                        0                        , nHorzSideSquareNumber - 1                          },
        {nHorzSideSquareNumber + nVertSideSquareNumber - 2, nHorzSideSquareNumber * 2 + nVertSideSquareNumber - 3},

    };


    for (int i = 0; i < _countof(markLineInfos); i++)
    {
        int start_index = markLineInfos[i].start_index;
        int end_index = markLineInfos[i].end_index;

        int number = end_index - start_index + 1;

        bool even_number = false;

        if (number % 2 == 0)
        {
            even_number = true;

        }

        int middle_index = (start_index + end_index - (even_number ? 1 : 0)) / 2;

        int nStepCount = middle_index - start_index;
        double decStep = 0.0;

        if (start_index >= middle_index)
        {
            continue;
        }

        decStep = (min_intensity - max_intensity) / nStepCount;

        for (int i = 1; i <= nStepCount; i++)
        {
            m_vecDisplayIntensity[start_index + i] = max_intensity + i*decStep;
        }

        middle_index = middle_index + 1;

        for (int i = 1; i <= (even_number ? nStepCount : nStepCount - 1); i++)
        {
            m_vecDisplayIntensity[end_index - i] = max_intensity + i*decStep;
        }

    }//for

}



//@功  能:处理灰度图片
//@参  数:grayImage, 输入的灰度图片
//        bSimulate, 模拟校正标志
//@返回值:TRUE,处理成功
//        FALSE, 处理失败
BOOL CMonitorBoundaryFinder::Process(const CImageFrame& grayImage, BOOL bSimulate)
{
    //运行计数器+1
    //m_nRunTimes ++;

    int nVideoWidth = grayImage.Width();
    int nVideoHeight = grayImage.Height();

    m_oWaitTimer.Trigger();

    //if(WAIT_STEADY_SAMPLE_COUNT < m_nRunTimes && m_nRunTimes  <= WAIT_SAMPLE_END_STAGE_COUNT)
    if (m_oWaitTimer.IsWaitTimeout(WAIT_STEDAY_SAMPLE_TIME) && !m_oWaitTimer.IsWaitTimeout(WAIT_SAMPLE_END_TIME))
    {//采样阶段

        OutputDebugString(L" xxxxxx CMonitorBoundaryFinder WAIT_STEDAY_SAMPLE_TIME\n");
        if (m_bShowMarker)
        {//前景累加
            AccumulateImageFrame(grayImage, m_oInitialScreenMask, m_frameForeground);
        }
        else
        {//背景累加
            AccumulateImageFrame(grayImage, m_oInitialScreenMask, m_frameBackground);
        }
    }
    //else if(WAIT_SAMPLE_END_STAGE_COUNT < m_nRunTimes)
    else if (m_oWaitTimer.IsWaitTimeout(WAIT_SAMPLE_END_TIME))
    { //采样完毕阶段

        OutputDebugString(L" xxxxxx CMonitorBoundaryFinder WAIT_SAMPLE_END_TIME\n");
        //m_nRunTimes  = 0;
        m_oWaitTimer.Reset();

        if (m_bShowMarker)
        {
            OutputDebugString(L" xxxxxx CMonitorBoundaryFinder m_bShowMarker true\n");
            m_nFlashTimes++;
            wsprintf(szTmp, L" xxxxxx CMonitorBoundaryFinder m_nFlashTimes:%d\n", m_nFlashTimes);
            OutputDebugString(szTmp);

            if (m_nFlashTimes >= MAX_FLASH_TIMES)
            {
                //闪烁达到最大次数, 处理收集到的图像数据
                m_nFlashTimes = 0;

                //计算差分图
                CWordFrame diffFrame(nVideoWidth, nVideoHeight);

                diffFrame = m_frameForeground - m_frameBackground;

                if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
                {
                    Debug_SaveImageFrame(diffFrame, _T("AcccumulateDiff.jpg"));
                }


                BOOL bRet = ProcessDiffImage(
                    diffFrame);

                if (!bRet)
                {
                    OutputDebugString(L" xxxxxx CMonitorBoundaryFinder ProcessDiffImage bRet false\n");
                    return FALSE;
                }

                OutputDebugString(L" xxxxxx CMonitorBoundaryFinder ProcessDiffImage m_bDone true\n");
                m_bDone = TRUE;
                return TRUE;
            }

            //全屏红显示
            if (!bSimulate)
            {
                //2014/04/21,在双液晶屏上拼接环境中测试，意外发现，如果屏幕背景为黑色, 则四角的白色圆圈在画面中显得很暗，
                //如果背景不为黑色， 则角的白色圆圈反而显得很亮。
                //
                //进一步实验发现红色背景效果最好
                //绿色背景时，屏幕中间上部高亮, 曝光过度。
               //FillBoard(m_hDispWnd, RED, &m_rcMonitor);
                FillBoard(m_hDispWnd, BACKGROUND_COLOR, &m_rcMonitor);
            }

            m_bShowMarker = FALSE;
        }
        else//未显示圆圈
        {
            if (!bSimulate)
            {
                OutputDebugString(L" xxxxxx CMonitorBoundaryFinder bSimulate false\n");
                DrawCircleMarkers(m_hDispWnd, &m_vecBorderMarkerPositions[0], m_vecBorderMarkerPositions.size(), &this->m_vecDisplayIntensity[0], FOREGROUND_COLOR);
            }
            OutputDebugString(L" xxxxxx CMonitorBoundaryFinder m_bShowMarker true\n");
            m_bShowMarker = TRUE;
        }
    }

    return TRUE;
}




//@功能:搜索所有白色实心圆的重心点，对这些点根据与总重心的方位角的大小进行排序
//@参数:srcImage, 16位灰度位图
//      pPtBorderMarker, 输出参数, 保存找到的标记实心圆重心的数组
//      nExpectedNumber, 期望找到的点的个数
//     pGlobalCentroid ,输出参数, 全局重心
BOOL CMonitorBoundaryFinder::SearchCircleCentroids(const CWordFrame&  srcImage, TBorderMarker* pPtBorderMarker, int nExpectedNumber, POINT* pGlobalCentroid)
{
    //16位灰度图片转为8位灰度图片
    int nImageWidth = srcImage.Width();
    int nImageHeight = srcImage.Height();

    int nPixelCount = nImageWidth*nImageHeight;
    int nLoopCount = nPixelCount;

    WORD max = srcImage.Max();
    WORD min = srcImage.Min();
    WORD range = max - min;
    const WORD* pWordPixel = srcImage.GetData();

    static CImageFrame grayImage;
    grayImage.SetSize(nImageWidth, nImageHeight, 2);

    BYTE* pPixel = grayImage.GetData();

    //16位灰度位图转为8位灰度位图
    while (nLoopCount)
    {
        if (range == 0)
        {
            *pPixel = (BYTE)0xFF;
        }
        else
        {
            if (*pPixel < 32)//16位灰度图片中,亮度值<32的认为是噪声
            {
                *pPixel = 0x00;
            }

            BYTE cValue = BYTE((UINT)255 * (UINT)(*pWordPixel - min) / (UINT)range);

            *pPixel = cValue;
        }
        pWordPixel++;
        pPixel++;
        nLoopCount--;
    }

    if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
    {
        Debug_SaveImageFrame(grayImage, _T("BorderCircleSrc.jpg"));
    }


    CImageFrame binarizedGrayFrame;
    BYTE initValue = 0;
    binarizedGrayFrame.SetSize(nImageWidth, nImageHeight, 1, &initValue);

    CBitFrame bitFrame;
    bitFrame.SetSize(nImageWidth, nImageHeight);

    CImageFrame debugImage;
    debugImage.SetSize(nImageWidth, nImageHeight, 1);


    for (size_t i = 0; i < m_vecSubAreaPolygons.size(); i++)
    {
        const PolygonVertices& polygon = m_vecSubAreaPolygons[i];

        CImageFrame subAreaMask;
        BYTE initValue = 0x00;
        subAreaMask.SetSize(nImageWidth, nImageHeight, 1, &initValue);


        FillPolygon(
            subAreaMask.GetData(),
            nImageWidth,
            nImageHeight,
            &polygon[0],
            polygon.size(),
            255,
            TRUE);


        //debug
        if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_VERBOSE)
        {
            CString strFileName;
            strFileName.Format(_T("subarea(%d)-part.jpg"), i + 1);
            Debug_SaveImageFrame(subAreaMask, strFileName);
        }

        //BYTE threshold = GetImageBinarizeThreshold_DoublePeak(grayImage, subAreaMask);
        //GetImageBinarizeThreshold_Ostu的最后一个参数废弃像素值为0的像素
        //
        BYTE threshold = GetImageBinarizeThreshold_Ostu(grayImage, subAreaMask, TRUE);

        if (threshold > 10)
        {
            threshold -= 5;
        }
        CImageFrame subAreaSrcImage = grayImage;
        subAreaSrcImage &= subAreaMask;


        subAreaSrcImage.Binarize(threshold);


        //调试时输出二值化图片
        if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_VERBOSE)
        {
            CString strFileName;
            strFileName.Format(_T("subarea(%d)-binarized.jpg"), i + 1);

            Debug_SaveImageFrame(subAreaSrcImage, strFileName);
        }

        //bitFrame |= subareaBitImage;

        binarizedGrayFrame |= subAreaSrcImage;

    }


    if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
    {
        Debug_SaveImageFrame(binarizedGrayFrame, _T("GrayFrameBorderSquaresBinarization.jpg"));
    }


    //先腐蚀后膨胀(开操作)
    //删除离散的细小干扰点。
    //Morph_Open(
    //    binarizedGrayFrame.GetData(),
    //    binarizedGrayFrame.GetData(),
    //    binarizedGrayFrame.Width(),
    //    binarizedGrayFrame.Height());

    //if(m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
    //{
    //    Debug_SaveImageFrame(binarizedGrayFrame, _T("GrayFrameBorderSquaresOpenOP.jpg"));
    //}


    GrayToBitFrame_SSE2((const BYTE*)binarizedGrayFrame.GetData(), (BYTE*)bitFrame.GetData(), 0x80, nImageWidth*nImageHeight);
    if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
    {
        //二值化的1位位图转为8位灰度位图
        BitToGrayFrame_MMX(
            (const BYTE*)bitFrame.GetData(),
            debugImage.GetData(),
            bitFrame.GetPixelCount());
        Debug_SaveImageFrame(debugImage, _T("BitFrameBorderCircles.jpg"));
    }


    //目标检测
    CConnectedComponentScanlineAlogrithm<1024> blobDetect;

    //找到的目标的数组
    std::vector<TBlobObject> vecObjsFound;
    vecObjsFound.resize(nExpectedNumber);

    blobDetect.ProcessImage(
        (const BYTE *)bitFrame.GetData(),
        nImageWidth,
        nImageHeight);

    int nMassThreshold = 4;//质量门限，质量小于该门限的目标被过滤

    int nObjFound = blobDetect.GetObjCount(nMassThreshold);

    //if (nObjFound >= nExpectedNumber)
    {
        //根据距离门限合并目标，消除干扰点
        //合并后的目标个数
        int nObjCountAfterMerge = nObjFound;
        TBlobObject* pBlobObj = blobDetect.GetObjs();

        if (nObjFound > nExpectedNumber)
        {
            for (int i = 0; i < nObjFound; i++)
            {
                TBlobObject& obj1 = pBlobObj[i];
                if (!obj1.bIsValid) continue;

                long d1 = max(obj1.rcArea.right - obj1.rcArea.left, obj1.rcArea.bottom - obj1.rcArea.top) >> 2;

                for (int j = i + 1; j < nObjFound; j++)
                {
                    TBlobObject& obj2 = pBlobObj[j];
                    if (!obj2.bIsValid) continue;

                    long dx = (obj1.GPos.x - obj2.GPos.x);
                    long dy = (obj1.GPos.y - obj2.GPos.y);
                    long R2 = dx*dx + dy*dy;

                    long d2 = max(obj2.rcArea.right - obj2.rcArea.left, obj2.rcArea.bottom - obj2.rcArea.top) >> 2;

                    if (R2 < (d1 + d2)*(d1 + d2))//目标1与目标2之间的距离小于目标2的外接矩形的最大宽度的2倍
                    {
                        if (obj1.mass > obj2.mass)
                        {
                            obj2.bIsValid = FALSE;//obj2被合并
                        }
                        else
                        {
                            obj1.bIsValid = FALSE;//obj1被合并
                            break;
                        }
                    }
                }
                if (!obj1.bIsValid) continue;
                nObjCountAfterMerge++;
            }
        }

        wsprintf(szTmp, L" xxxxxx SearchCircleCentroids nObjCountAfterMerge:%d, nExpectedNumber:%d\n", nObjCountAfterMerge, nExpectedNumber);
        OutputDebugString(szTmp);
        if (nObjCountAfterMerge >= nExpectedNumber)
        {
            const TBlobObject* pBlobObj = blobDetect.GetObjs();

            size_t j = 0;
            for (int i = 0; i < nObjFound; i++)
            {
                if (pBlobObj[i].bIsValid)
                {
                    vecObjsFound[j++] = pBlobObj[i];

                    if (j >= vecObjsFound.size())
                        break;
                }
            }
            nObjFound = nExpectedNumber;
        }
    }

    wsprintf(szTmp, L" xxxxxx SearchCircleCentroids nObjFound:%d, nExpectedNumber:%d\n", nObjFound, nExpectedNumber);
    OutputDebugString(szTmp);
    if (nObjFound < nExpectedNumber)
    {
        return FALSE;
    }
    else//成功
    {

        for (int i = 0; i < nExpectedNumber; i++)
        {

            pPtBorderMarker[i].ptCentroid = vecObjsFound[i].GPos;
            pPtBorderMarker[i].rcBound = vecObjsFound[i].rcArea;
        }

        //计算所有边界点的重心
        POINT ptCentroid;

        int mx = 0;
        int my = 0;
        for (int i = 0; i < nExpectedNumber; i++)
        {
            mx += pPtBorderMarker[i].ptCentroid.x;
            my += pPtBorderMarker[i].ptCentroid.y;
        }


        ptCentroid.x = mx / nExpectedNumber;
        ptCentroid.y = my / nExpectedNumber;

        //对找到的边界点按照方位角度排序
        SortInDirection(pPtBorderMarker, nExpectedNumber, ptCentroid);


        if (pGlobalCentroid)
        {
            *pGlobalCentroid = ptCentroid;
        }
        return TRUE;
    }
}


//@功能:处理差分图片
//@参数:diffImage, 差分图
//      eScreenPart, 屏幕区域枚举值
//BOOL CMonitorBoundaryFinder::ProcessDiffImage(const CWordFrame& diffImage, CMonitorBoundaryFinder::ESearchScreenPart eScreenPart)
BOOL CMonitorBoundaryFinder::ProcessDiffImage(const CWordFrame& diffImage)
{
    CImageFrame* pMaskFrame = NULL;
    TBorderMarker* pBorderMarker = NULL;
    POINT* pGlobalCentroid = NULL;
    int nPtNumber = 0;


    m_vecBorderCalibratePts.resize(m_vecBorderMarkerPositions.size());
    pGlobalCentroid = &m_ptCentroid;
    pBorderMarker = &m_vecBorderCalibratePts[0];
    nPtNumber = m_vecBorderCalibratePts.size();
    BOOL bRet = SearchCircleCentroids(diffImage, pBorderMarker, nPtNumber, pGlobalCentroid);

    if (!bRet) return FALSE;


    return TRUE;
}

int CMonitorBoundaryFinder::GetMarkerCount()const
{
    int BORDER_MARKER_NUMBER = (m_nHorzSideMarkerNumber - 1) * 2 + (m_nVertSideMarkerNumber - 1) * 2;

    return BORDER_MARKER_NUMBER;
}



//静态函数
//功能:亮度控制回调函数
//@参数:bInc, TRUE:增加亮度，FALSE:减少亮度
//      absDiff, 亮度差值绝对值
//      lpCtxData,上下文数据
bool CAutoCalibratorImpl2::BrightnessCtrlCallback(bool bInc, BYTE absDiff, BYTE nIndex, LPVOID lpCtxData)
{
    CAutoCalibratorImpl2* lpThis = reinterpret_cast<CAutoCalibratorImpl2*>(lpCtxData);
    if (lpThis == NULL) return false;
    BOOL bRet = FALSE;
    if (lpThis->m_fpChangeCameraParams)
    {
        //AtlTrace(_T("[AuotBrightnessRegulator]abs_diff=%d, %s.\n"), (UINT)absDiff, bInc?_T("Increase"):_T("Decrease"));
        bRet = lpThis->m_fpChangeCameraParams(bInc ? E_CAMERA_BRIGHTNESS_INCREASE : E_CAMERA_BRIGHTNESS_DECREASE, lpThis->m_lpCtx, absDiff, nIndex);
    }
    return bRet ? true : false;
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
//BOOL CAutoCalibratorImpl2::StartCalibrating(HWND hNotifyWnd, E_AutoCalibratePattern ePattern, BYTE cBrightness, CHANGECAMERAPARAMSPROC ChangeCameraParamsProc, LPVOID lpCtx, COLORREF clrGridHighlight, BOOL bSaveInermediatFile  ,BOOL bRecordVideo)


//@功能:开始校正, 供外部模块调用
//@参数:autoCalibrateParams, 自动校正参数
//      staticMaskingParams, 静态屏蔽参数
BOOL CAutoCalibratorImpl2::StartCalibrating(const TAutoCalibrateParams& autoCalibrateParams, const TStaticMaskingParams& staticMaskingParams)
{
    //调试级别
    m_eDebugLevel = autoCalibrateParams.eDebugLevel;

    m_vecScreenInfos = autoCalibrateParams.vecScreenInfos;

    int nMonitorCount = m_vecScreenInfos.size();
    if (nMonitorCount == 0)
    {
        return FALSE;
    }

    m_oScreenMap.Clear();

    m_vecMonitorCalibResults.resize(nMonitorCount);

    m_nDispMonitorId = 0;

    m_CurrentMonitorInfo = m_vecScreenInfos[m_nDispMonitorId];

    m_bSaveInermediatFile = autoCalibrateParams.bSaveInermediatFile;
    m_bRecordVideo = autoCalibrateParams.bRecordVideo;
    m_fpChangeCameraParams = autoCalibrateParams.ChangeCameraParamsProc;
    m_lpCtx = autoCalibrateParams.lpCtx;

    m_bEnableOnlineScreenArea = autoCalibrateParams.bEnableOnlineScreenArea;

    m_oautocalibrateparamslist = autoCalibrateParams.autocalibrateparamslist;

    //设置自动校正时间放大倍数
    const AutoCalibrateImageParams& autocalibrateParams = m_oautocalibrateparamslist[0].autoCalibrateImageParams;
    InitWaitTimer(autocalibrateParams.autoCalibrateSpeed, autocalibrateParams.videoDislayDelay);

    //SQUARE_SIZE  = INITIAL_SQUARE_SIZE;

    m_AutoBrightnessRegulator.Reset();//自动亮度控制复位

    //设置自动亮度调节时期望的画面平均亮度
    //m_AutoBrightnessRegulator.SetExpectedBrightness(autoCalibrateParams.cAvgBrightness);
    m_AutoBrightnessRegulator.SetAutoCalibrateParamsIndex(0);


    m_AutoBrightnessRegulator.SetFeedbackCtrlFunction(BrightnessCtrlCallback, (LPVOID)this);
    if (m_bEnableOnlineScreenArea)
    {
        m_AutoBrightnessRegulator.SetExpectedBrightness((m_oautocalibrateparamslist[0].autoCalibrateImageParams.autoCalibrateExpectedBrightness) >> 1);
    }
    else
    {
        m_AutoBrightnessRegulator.SetExpectedBrightness(m_oautocalibrateparamslist[0].autoCalibrateImageParams.autoCalibrateExpectedBrightness);
    }

    m_bTestAutoBrightnessCtrlMode = FALSE; //置非自动亮度调节测试模式

    if (m_bIsWorking)
    {
        return FALSE;
    }

    //自动校正时的校正图案中白色块的亮度值(0~255)
    //m_clrGridHighlight = autoCalibrateParams.clrGridHighlight;
    BYTE  gray = m_oautocalibrateparamslist[0].autoCalibrateImageParams.autoCalibrateHilightGray;
    m_clrGridHighlight = RGB(gray, gray, gray);

    RECT rcVirtualScreen;

    rcVirtualScreen.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    rcVirtualScreen.top = GetSystemMetrics(SM_YVIRTUALSCREEN);

    rcVirtualScreen.right = rcVirtualScreen.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    rcVirtualScreen.bottom = rcVirtualScreen.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);

    //rcVirtualScreen.right = rcVirtualScreen.left + GetSystemMetrics(SM_CXSCREEN);
    //rcVirtualScreen.bottom = rcVirtualScreen.top + GetSystemMetrics(SM_CYSCREEN);

    //生成校正窗体，尺寸为包含所有屏幕的虚拟屏幕的尺寸
    if (HWND(m_AutoCalibrateWnd) == NULL)
    {
        if (!(m_AutoCalibrateWnd.CreateWindow)(rcVirtualScreen))
        {
            return FALSE;
        }
    }
    else
    {   //在更改分屏幕辨率时，需要调整校正窗体的尺寸
        m_AutoCalibrateWnd.MoveWindow(rcVirtualScreen);
    }


    m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_UNDEF;
    m_hNotifyWnd = autoCalibrateParams.hNotifyWnd;
    m_AutoCalibrateWnd.ShowWindow(SW_SHOW);
    m_AutoCalibrateWnd.SetFocus();


    // m_eMonochromizeAlog = E_MONOCHROMIZE_ALOG_Y;//黑白化算法
    m_eCalibrateStage = E_AUTO_CALIBRATE_START;
    //m_nStageWaitCount = 0;
    m_oWaitTimer.Reset();

    m_bIsWorking = TRUE;
    m_bIsSimulatedCalibrating = FALSE;



    //InitAutoCalibrateSaveDirectory();
    SYSTEMTIME now;
    GetLocalTime(&now);

    m_ePattern = autoCalibrateParams.ePattern;

    //自动校正的调试路径
    m_strImageOutputBaseDir.Format(
        _T("%s\\AutoCalibrate\\%04d_%02d_%02d_%02d_%02d_%02d"),
        g_strIntermediatRootDirName,
        now.wYear,
        now.wMonth,
        now.wDay,
        now.wHour,
        now.wMinute,
        now.wSecond
    );


    m_strCurrentImageOutputDir.Format(
        _T("%s\\Monitor_%d"),
        m_strImageOutputBaseDir,
        m_nDispMonitorId + 1);


    //m_uCx, m_uCy指向当前监视器的屏幕尺寸
    m_uMonitorWidth = m_CurrentMonitorInfo.rcArea.right - m_CurrentMonitorInfo.rcArea.left;
    m_uMonitorHeight = m_CurrentMonitorInfo.rcArea.bottom - m_CurrentMonitorInfo.rcArea.top;



    SYSTEMTIME localTime;
    GetLocalTime(&localTime);

    //自动校正的录像文件路径
    m_strDebugVideoFullPath.Format(_T("%s\\AutoCalibrate_Sceen_%04d%02d%02d-%02d%02d%02d%03d_[pattern %d,screen %dX%d,brightness %u].avi"),
        m_strImageOutputBaseDir,
        localTime.wYear,
        localTime.wMonth,
        localTime.wDay,
        localTime.wHour,
        localTime.wMinute,
        localTime.wSecond,
        localTime.wMilliseconds,
        GetPatternCols(m_ePattern),
        m_uMonitorWidth,
        m_uMonitorHeight,
        m_AutoBrightnessRegulator.GetExpectedBrightness());


    //隐藏鼠标光标
    ShowCursor(FALSE);


    //每个屏幕的校正尝试次数
    m_nTryTimes = 0;

    //最大尝试次数，要能够覆盖所有的画面参数
    if (this->m_nMaxTryTimes < (int)this->m_oautocalibrateparamslist.size())
    {
        this->m_nMaxTryTimes = (int)this->m_oautocalibrateparamslist.size();
    }

    //设置图片输出目录
    CString strImageDubugDir;
    strImageDubugDir.Format(_T("%s\\%d"), m_strCurrentImageOutputDir, m_nTryTimes + 1);
    SetImageDebugDir(strImageDubugDir);

    if (m_bRecordVideo)//需要保存校正视频
    {
        //不再在"校正点自动采集模块"中录像,改为在外部的视频源处录制
        //m_oAVIRecorder.OpenFile(m_strDebugVideoFullPath, 640, 480);
    }


    //清除校正点数组
    m_vecBorderCalibMap.clear();
    m_vecInteriorCalibMap.clear();
    m_aryAllCalibMap.clear();


    m_bUseOldMethod = FALSE;


    //<<2013/03/11,by toxuke@gmail.com 重置自动校正窗体超时检测状态
    m_AutoCalibrateWnd.StartTimeoutDetect();
    //>>


    //<<2014/12/09, 静态屏蔽参数
    this->m_tStaticMaskingParams = staticMaskingParams;
    //>>

    if (IsLanguageBarVisible())
    {
        ShowLanguageBar(FALSE);
        m_bRestoreLanguageBar = TRUE;
    }
    else
    {
        m_bRestoreLanguageBar = FALSE;
    }

    if (IsTaskBarVisible())
    {
        ShowTaskBar(FALSE);
        m_bRestoreTaskbar = TRUE;
    }
    else
    {
        m_bRestoreTaskbar = FALSE;
    }

    m_eWorkMode = E_WORK_MODE_AUTO_CALIBRATE;


    if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
    {
        //模拟校正用数据
        DbgDumpSimulateParams(autoCalibrateParams);
    }

    m_nDbgFrameCount = 0;
    return TRUE;
}

//@功能:结束工作
void CAutoCalibratorImpl2::Finish()
{
    switch (m_eWorkMode)
    {
    case E_WORK_MODE_AUTO_CALIBRATE:
        EndCalibrating();
        break;

    case E_WORK_MODEL_AUTO_MASKING:
        EndAutoMasking();
        break;
    }

}

void CAutoCalibratorImpl2::OnDeviceMissing()
{
    switch (m_eWorkMode)
    {
    case E_WORK_MODE_AUTO_CALIBRATE:
        ShowCursor(TRUE);
        if (m_bIsWorking)
        {
            if (m_bIsSimulatedCalibrating)
            {
                PostThreadMessage(m_dwSimulatedCalibrateThreadId, WM_QUIT, 0, 0);
                //m_oAVIInput.Close();
                m_oAVIInput.close();
            }
            else
            {
                //隐藏校正窗体
                m_AutoCalibrateWnd.ShowWindow(SW_HIDE);
                //关闭超时检测功能
                m_AutoCalibrateWnd.CloseTimeoutDetect();
                m_bIsWorking = FALSE;
            }
            PostMessage(m_hNotifyWnd, WM_AUTO_CALIBRATE_DONE, WPARAM(E_AUTO_CALIBRATOR_ERROR_NOT_FOUND_DEVICE), LPARAM(this->m_bIsSimulatedCalibrating));
            //恢复语言栏
            if (m_bRestoreLanguageBar)
            {
                ShowLanguageBar(TRUE);
            }
            //恢复任务栏
            if (m_bRestoreTaskbar)
            {
                ShowTaskBar(TRUE);
            }
        }
        break;

    case E_WORK_MODEL_AUTO_MASKING:
        ShowCursor(TRUE);
        if (m_bIsWorking)
        {
            if (m_bIsSimulatedCalibrating)
            {
                PostThreadMessage(m_dwSimulatedCalibrateThreadId, WM_QUIT, 0, 0);
                //m_oAVIInput.Close();
                m_oAVIInput.close();
            }
            else
            {
                //隐藏校正窗体
                m_AutoCalibrateWnd.ShowWindow(SW_HIDE);
                //关闭超时检测功能
                m_AutoCalibrateWnd.CloseTimeoutDetect();
                m_bIsWorking = FALSE;
            }
            PostMessage(m_hNotifyWnd, WM_SEARCH_SCREEN_AREA_DONE, 0, LPARAM(E_AUTO_MASKING_NOT_FOUND_DEVICE));
            //恢复语言栏
            if (m_bRestoreLanguageBar)
            {
                ShowLanguageBar(TRUE);
            }
        }
        break;
    }

}

//@功能:结束自动校正
void CAutoCalibratorImpl2::EndCalibrating()
{
    ShowCursor(TRUE);
    if (m_bIsWorking)
    {
        if (m_bIsSimulatedCalibrating)
        {
            PostThreadMessage(m_dwSimulatedCalibrateThreadId, WM_QUIT, 0, 0);
            //WaitForSingleObject(m_hSimulatedCalibrateThread, INFINITE);
            //m_oAVIInput.Close();
            m_oAVIInput.close();
            
        }
        else
        {
            //隐藏校正窗体
            m_AutoCalibrateWnd.ShowWindow(SW_HIDE);

            //关闭超时检测功能
            m_AutoCalibrateWnd.CloseTimeoutDetect();

            m_bIsWorking = FALSE;


            if (m_bRecordVideo)//详细输出时, 保存视频
            {
                //m_oAVIRecorder.Close();
            }

            if (E_AUTO_CALIBRATOR_OK == this->m_eCalibrateError && !this->m_bSaveInermediatFile)
            {//成功且设置不保存自动校正中间文件标志
#ifdef NDEBUG
                //删除成功校正后的校正图偏和AVI文件
                DeleteDirectory(m_strCurrentImageOutputDir);

#endif
            }
        }

        PostMessage(m_hNotifyWnd, WM_AUTO_CALIBRATE_DONE, WPARAM(this->m_eCalibrateError), LPARAM(this->m_bIsSimulatedCalibrating));

        //恢复语言栏
        if (m_bRestoreLanguageBar)
        {
            ShowLanguageBar(TRUE);
        }

        //恢复任务栏
        if (m_bRestoreTaskbar)
        {
            ShowTaskBar(TRUE);
        }
    }
}


//@功能:在一圈点中查找拐点
//@参数:pPts, 圈上点的数组,已按照方向有序排列
//      nPtNumber, 圈上点的数目
//      pTurnpoint,保存拐点的数组
//      nExpectedNum, 拐点数组的元素个数
//@返回:拐点数目
int CAutoCalibratorImpl2::SearchTurnPoints(const POINT* pPts, int nPtNumber, TurnPoint* pTurnpoint, int nExpectedNum)
{

    int nTurnPointCount = 0;
    //依据是:"以当前点为终点"的向量和"以当前点为始点"的向量之间的夹角来判断是否为拐点
    //若当前点为拐点，则矢量夹角较大。
    for (int i = 0; i < nPtNumber; i++)
    {
        int iPrevIndex, iNextIndex;

        if (i == 0)
        {
            iPrevIndex = nPtNumber - 1;
        }
        else
        {
            iPrevIndex = i - 1;
        }

        if (i == nPtNumber - 1)
        {
            iNextIndex = 0;
        }
        else
        {
            iNextIndex = i + 1;
        }

        POINT ptCurrent = pPts[i];
        POINT ptPrev = pPts[iPrevIndex];
        POINT ptNext = pPts[iNextIndex];


        long v1_dx, v1_dy, v2_dx, v2_dy;
        v1_dx = ptCurrent.x - ptPrev.x;
        v1_dy = ptCurrent.y - ptPrev.y;


        v2_dx = ptNext.x - ptCurrent.x;
        v2_dy = ptNext.y - ptCurrent.y;


        //计算矢量的叉积
        double cross_product = v1_dx * v2_dy - v1_dy * v2_dx;

        //计算矢量的点积
        double dot_product = v1_dx*v2_dx + v1_dy*v2_dy;

        //V1×V2 = |V1||V2|sinθ
        //=>sinθ^2 = (V1×V2)^2 /[(|V1||V2|)^2]
        //注意此处防止整型计算溢出错误,2013/04/11
        double v1_length_sqare = v1_dx * v1_dx + v1_dy * v1_dy;
        double v2_length_sqare = v2_dx * v2_dx + v2_dy * v2_dy;


        double sin_v1_v2_square = (double(cross_product) / double(v1_length_sqare))*(double(cross_product) / double(v2_length_sqare));
        const double sin_20_square = sin(20.0*M_PI / 180.0)*sin(20.0*M_PI / 180.0);

        //当矢量夹角在[340, 360]和[0,20]之间时, 则认为是在边上的校正点,不是角上的点。
        if (dot_product > 0 //保证在第一、四象限
            &&
            sin_v1_v2_square < sin_20_square)
        {
            //为在边上的校正点
        }
        else
        {
            if (nTurnPointCount < nExpectedNum)
            {
                //防止在异常情况下内存访问越界
                pTurnpoint[nTurnPointCount].ptCoord = pPts[i];
                pTurnpoint[nTurnPointCount].nLayerPtIndex = i;
            }

            nTurnPointCount++;
        }

    }//for(i)

    return nTurnPointCount;
}

//@功能:二值化图片
//@参数:srcImage, 原始图片
//      bitImage, 输出的二值化位图
void CAutoCalibratorImpl2::BinarizeImage(const CImageFrame& srcImage, const CImageFrame& maskImage, CBitFrame& bitImage)
{

    int nImageWidth = srcImage.Width();
    int nImageHeight = srcImage.Height();

    bitImage.SetSize(nImageWidth, nImageHeight);

    /*
    //图片二值化,利用Expectation-Maximization算法确定二值化门限。
    //BYTE threshold = EM_Threshold(m_oPatternFrame,m_oScreenMaskFrame);
    //BYTE threshold = GetImageBinarizeThreshold_Ostu(m_oPatternFrame, m_oScreenMaskFrame);
    //BYTE threshold = GetImageBinarizeThreshold_DoublePeak(srcImage, maskImage);
    BYTE threshold = GetImageBinarizeThreshold_Ostu(srcImage, maskImage);
    GrayToBitFrame_SSE2((const BYTE*)srcImage.GetData(), (BYTE*)bitImage.GetData(), threshold, nImageWidth*nImageHeight);
    */

    for (size_t i = 0; i < m_vecPolygons.size(); i++)
    {
        const PolygonVertices& polygon = m_vecPolygons[i];

        CImageFrame subAreaMask;
        BYTE initValue = 0x00;
        subAreaMask.SetSize(nImageWidth, nImageHeight, 1, &initValue);


        FillPolygon(
            subAreaMask.GetData(),
            nImageWidth,
            nImageHeight,
            &polygon[0],
            polygon.size(),
            255,
            TRUE);


        subAreaMask &= maskImage;//用膨胀后的全局屏蔽图裁剪局部屏蔽图

        //debug
        if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_VERBOSE)
        {
            CString strFileName;
            strFileName.Format(_T("subarea(%d)-mask.jpg"), i + 1);
            Debug_SaveImageFrame(subAreaMask, strFileName);
        }

        //BYTE threshold = GetImageBinarizeThreshold_DoublePeak(srcImage, subAreaMask);

        BYTE threshold = GetImageBinarizeThreshold_Ostu(srcImage, subAreaMask);

        //
        if (threshold > 10)
        {
            threshold -= 5;
        }

        CImageFrame subAreaSrcImage = srcImage;
        subAreaSrcImage &= subAreaMask;

        CBitFrame subareaBitImage;
        subareaBitImage.SetSize(nImageWidth, nImageHeight);

        GrayToBitFrame_SSE2((const BYTE*)subAreaSrcImage.GetData(), (BYTE*)subareaBitImage.GetData(), threshold, nImageWidth*nImageHeight);


        //调试时输出二值化图片
        if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_VERBOSE)
        {
            CImageFrame debugFrame;
            debugFrame.SetSize(subareaBitImage.Width(), subareaBitImage.Height(), 1);

            //二值化的1位位图转为8位灰度位图
            BitToGrayFrame_MMX(
                (const BYTE*)subareaBitImage.GetData(),
                debugFrame.GetData(),
                subareaBitImage.GetPixelCount());

            CString strFileName;
            strFileName.Format(_T("subarea(%d)-binarized.jpg"), i + 1);

            Debug_SaveImageFrame(debugFrame, strFileName);
        }

        bitImage |= subareaBitImage;

    }

}

void CAutoCalibratorImpl2::PartitionScreenArea()
{
    POINT(&subareaCentroids)[4] = m_oMonitorAreaLocator.GetSubAreaCentroids();

    TPoint2D  pt2d[4];

    for (int i = 0; i < 4; i++)
    {
        pt2d[i].d[0] = subareaCentroids[i].x;
        pt2d[i].d[1] = subareaCentroids[i].y;
    }

    RECT rcImage;
    rcImage.left = 0;
    rcImage.top = 0;
    rcImage.right = m_oInitialScreenMask.Width() - 1;//必须-1，否则得到的多边形顶点越界 image_width和image_heigth
    rcImage.bottom = m_oInitialScreenMask.Height() - 1;//

    BOOL bRet = m_oAreaSplitter.SplitBorderArea(
        rcImage,
        pt2d,
        2,
        2);

    if (!bRet) return;

    CImageFrame debugFrame;
    debugFrame.SetSize(
        m_oInitialScreenMask.Width(),
        m_oInitialScreenMask.Height());

    int nAreaCount = m_oAreaSplitter.GetAreaCount();

    m_vecPolygons.resize(nAreaCount + 1);//9个区域, 中间还有一个区域

    //沿边的8个区域
    for (int nAreaIndex = 0; nAreaIndex < nAreaCount; nAreaIndex++)
    {
        const TPoint2D* polygon = NULL;
        int ptCount = 0;

        if (m_oAreaSplitter.GetAreaPolygonPoints(nAreaIndex, &polygon, &ptCount))
        {
            m_vecPolygons[nAreaIndex].resize(ptCount);

            for (int i = 0; i < ptCount; i++)
            {
                m_vecPolygons[nAreaIndex][i].x = long(polygon[i].d[0]);
                m_vecPolygons[nAreaIndex][i].y = long(polygon[i].d[1]);
            }

            FillPolygon(
                debugFrame.GetData(),
                debugFrame.Width(),
                debugFrame.Height(),
                &m_vecPolygons[nAreaIndex][0],
                ptCount,
                30 * (nAreaIndex % 8),
                FALSE);

        }//if
    }//for

    //中间区域
    PolygonVertices&  centerPolygonArea = m_vecPolygons[nAreaCount];
    centerPolygonArea.resize(4);
    centerPolygonArea[0] = subareaCentroids[0];
    centerPolygonArea[1] = subareaCentroids[1];
    centerPolygonArea[2] = subareaCentroids[3];
    centerPolygonArea[3] = subareaCentroids[2];

    FillPolygon(
        debugFrame.GetData(),
        debugFrame.Width(),
        debugFrame.Height(),
        &centerPolygonArea[0],
        centerPolygonArea.size(),
        30 * 8/*index*/,
        FALSE);

    if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
    {
        Debug_SaveImageFrame(
            debugFrame,
            _T("splitArea.jpg"));
    }

    /*
    //计算重心
    //计算所有边界点的重心
    POINT ptCentroid;

    int mx = 0;
    int my = 0;
    for(size_t i=0; i < boundaryPoints.size(); i++)
    {
        mx += boundaryPoints[i].x;
        my += boundaryPoints[i].y;
    }


    ptCentroid.x = mx / boundaryPoints.size();
    ptCentroid.y = my / boundaryPoints.size();

    //划分为四个多变形区域
    int nAreaCount = 4;
    m_vecPolygons.resize(nAreaCount);

    int subAreaHorzBoundaryPointCount = (CMonitorBoundaryFinder::HORZ_SIDE_MARKER_NUMBER  + 1)/ 2;
    if(CMonitorBoundaryFinder::HORZ_SIDE_MARKER_NUMBER % 2 == 0)
    {
        subAreaHorzBoundaryPointCount ++;
    }

    int subAreaVertBoundaryPointCount =  (CMonitorBoundaryFinder::VERT_SIDE_MARKER_NUMBER + 1)/ 2;
    if(CMonitorBoundaryFinder::VERT_SIDE_MARKER_NUMBER % 2 == 0)
    {
        subAreaVertBoundaryPointCount ++;
    }


    int nSubAreaBoundaryPointCount = subAreaHorzBoundaryPointCount + subAreaVertBoundaryPointCount ;

    for( int areaIndex = 0; areaIndex < nAreaCount; areaIndex ++)
    {
        m_vecPolygons[areaIndex].resize(nSubAreaBoundaryPointCount);
    }

    //subarea 1
    int subareaBoundaryPointIndex  = 0;

    int horzBoundaryPointFirstIndex = 0;
    int vertBoundaryPointFirstIndex = CMonitorBoundaryFinder::BORDER_MARKER_NUMBER - subAreaVertBoundaryPointCount + 1;

    for(int i = 0; i < subAreaHorzBoundaryPointCount; i ++)
    {
        m_vecPolygons[0][subareaBoundaryPointIndex ++] = boundaryPoints[horzBoundaryPointFirstIndex + i];
    }

    m_vecPolygons[0][subareaBoundaryPointIndex++] = ptCentroid;

    for(int i = 0; i < subAreaVertBoundaryPointCount - 1; i++)
    {
        m_vecPolygons[0][subareaBoundaryPointIndex ++] = boundaryPoints[vertBoundaryPointFirstIndex + i];
    }

    //subarea 2
     subareaBoundaryPointIndex  = 0;

     horzBoundaryPointFirstIndex = CMonitorBoundaryFinder::HORZ_SIDE_MARKER_NUMBER  - subAreaHorzBoundaryPointCount;
    vertBoundaryPointFirstIndex  = CMonitorBoundaryFinder::HORZ_SIDE_MARKER_NUMBER;

    for(int i = 0; i < subAreaHorzBoundaryPointCount; i++)
    {
        m_vecPolygons[1][subareaBoundaryPointIndex ++] = boundaryPoints[horzBoundaryPointFirstIndex + i];
    }

    for(int i = 0; i < subAreaVertBoundaryPointCount - 1; i++)
    {
         m_vecPolygons[1][subareaBoundaryPointIndex ++] = boundaryPoints[vertBoundaryPointFirstIndex + i];
    }

     m_vecPolygons[1][subareaBoundaryPointIndex] = ptCentroid;

    //subarea 3
     subareaBoundaryPointIndex  = 0;
     horzBoundaryPointFirstIndex = CMonitorBoundaryFinder::HORZ_SIDE_MARKER_NUMBER + CMonitorBoundaryFinder::VERT_SIDE_MARKER_NUMBER;
     vertBoundaryPointFirstIndex = CMonitorBoundaryFinder::HORZ_SIDE_MARKER_NUMBER - 1 + CMonitorBoundaryFinder::VERT_SIDE_MARKER_NUMBER  -  subAreaVertBoundaryPointCount;


    for(int i=0; i <  subAreaVertBoundaryPointCount; i++)
    {
         m_vecPolygons[2][subareaBoundaryPointIndex ++] = boundaryPoints[vertBoundaryPointFirstIndex + i];
    }


    for(int i = 0; i < subAreaHorzBoundaryPointCount - 1; i++)
    {

         m_vecPolygons[2][subareaBoundaryPointIndex ++] = boundaryPoints[horzBoundaryPointFirstIndex + i];
    }

    m_vecPolygons[2][subareaBoundaryPointIndex] = ptCentroid;



    //subarea 4
    subareaBoundaryPointIndex  = 0;
    horzBoundaryPointFirstIndex = (CMonitorBoundaryFinder::HORZ_SIDE_MARKER_NUMBER - 1) * 2  + CMonitorBoundaryFinder::VERT_SIDE_MARKER_NUMBER  - subAreaHorzBoundaryPointCount;
    vertBoundaryPointFirstIndex = (CMonitorBoundaryFinder::HORZ_SIDE_MARKER_NUMBER - 1) * 2  + CMonitorBoundaryFinder::VERT_SIDE_MARKER_NUMBER;

    for(int i = 0; i < subAreaHorzBoundaryPointCount; i++)
    {
         m_vecPolygons[3][subareaBoundaryPointIndex ++] = boundaryPoints[horzBoundaryPointFirstIndex + i];
    }

    for(int i = 0; i < subAreaVertBoundaryPointCount - 1; i++)
    {
        m_vecPolygons[3][subareaBoundaryPointIndex ++] = boundaryPoints[vertBoundaryPointFirstIndex + i];
    }

    m_vecPolygons[3][subareaBoundaryPointIndex] = ptCentroid;

    */

}


//@功能:屏幕区域搜索结束后的后处理函数。主要有计算屏幕的完整屏蔽图、计算旋转矩阵。
//@参数:nImageWidth, 视频图像宽度
//      nImageHeight,视频图像高度
//@返回值;成功, TRUE
//        失败, FALSE
BOOL CAutoCalibratorImpl2::OnPostSearchScreenBoundary(int nImageWidth, int nImageHeight)
{

    m_ptRoationCenter = m_oMonitorBoundaryFinder.GetScreenCentroid();

    OutputDebugString(L" xxxxxx Entry OnPostSearchScreenBoundary\n");

    //旋转矢量
    POINT Vx, Vy;

    double VLength;

    POINT(&subareaCentroids)[4] = m_oMonitorAreaLocator.GetSubAreaCentroids();

    POINT ptLowerCentroid;
    ptLowerCentroid.x = (subareaCentroids[2].x + subareaCentroids[3].x) >> 1;
    ptLowerCentroid.y = (subareaCentroids[2].y + subareaCentroids[3].y) >> 1;

    //Vy:从旋转重心到下部重心的矢量
    Vy.x = ptLowerCentroid.x - m_ptRoationCenter.x;
    Vy.y = ptLowerCentroid.y - m_ptRoationCenter.y;

    VLength = sqrt(double(Vy.x*Vy.x + Vy.y * Vy.y));

    //Vx⊥Vy
    Vx.x = Vy.y;
    Vx.y = -Vy.x;

    //计算旋转矩阵
    //
    //|x|   |c11  c12| |u|
    //| | = |        |*| |
    //|y|   |c21  c22| |v|
    //验证:
    //|u|  |Vx.x|    |x|  |Vx.x/VLength Vx.y/VLength| |Vx.x/VLength| |1|
    //| | =|    |    | | =|                         |*|            |=| |
    //|v|  |Vx.y| 时,|y|  |Vy.x/VLength Vy.y/VLength| |Vx.y/Vlenth | |0|
    //和
    //|u|  |Vy.x|    |x|  |Vx.x/VLength Vx.y/VLength| |Vy.x/VLength| |0|
    //| | =|    |    | | =|                         |*|            |=| |
    //|v|  |Vy.y| 时,|y|  |Vy.x/VLength Vy.y/VLength| |Vy.y/Vlenth | |1|

    //
    m_dbRotationMatrix[0][0] = (double)Vx.x / VLength;
    m_dbRotationMatrix[0][1] = (double)Vx.y / VLength;

    m_dbRotationMatrix[1][0] = (double)Vy.x / VLength;
    m_dbRotationMatrix[1][1] = (double)Vy.y / VLength;

    m_vecBorderCalibratePts = m_oMonitorBoundaryFinder.GetBorderCalibratePts();

    //计算完整的屏蔽位图
    //计算所有边界点的重心
    POINT ptCentroid;

    int mx = 0;
    int my = 0;
    //for(size_t i=0; i < CMonitorBoundaryFinder::BORDER_MARKER_NUMBER; i++)
    for (int i = 0; i < m_oMonitorBoundaryFinder.GetMarkerCount(); i++)
    {
        mx += m_vecBorderCalibratePts[i].ptCentroid.x;
        my += m_vecBorderCalibratePts[i].ptCentroid.y;
    }


    ptCentroid.x = mx / m_vecBorderCalibratePts.size();
    ptCentroid.y = my / m_vecBorderCalibratePts.size();


    //对找到的边界点按照方位角度排序
    SortInDirection(&m_vecBorderCalibratePts[0], m_vecBorderCalibratePts.size(), ptCentroid);

    //m_ptScreenCentroid = ptCentroid;


    //屏蔽图复位
    m_oScreenMaskFrame.SetSize(nImageWidth, nImageHeight, 1);
    m_oScreenMaskFrame.Clear();


    std::vector<POINT> vecBorderPts;
    vecBorderPts.resize(m_vecBorderCalibratePts.size());
    for (size_t i = 0; i < m_vecBorderCalibratePts.size(); i++)
    {
        vecBorderPts[i] = m_vecBorderCalibratePts[i].ptCentroid;
    }


    //搜索边界上一圈点中,"左上角"的点在数组的索引号, 目的是建立"图像点"到"屏幕点"的映射关系。
    TurnPoint turnPts[4];
    int nFound = SearchTurnPoints(&vecBorderPts[0], vecBorderPts.size(), turnPts, _countof(turnPts));
    if (nFound != _countof(turnPts))
    {
        return FALSE;
    }

    //旋转扳正四个角点,以便确定位于计算机屏幕左上角的点的索引号。
    for (int i = 0; i < _countof(turnPts); i++)
    {
        long x = turnPts[i].ptCoord.x - m_ptRoationCenter.x;
        long y = turnPts[i].ptCoord.y - m_ptRoationCenter.y;

        turnPts[i].ptCoord.x = m_ptRoationCenter.x + long(m_dbRotationMatrix[0][0] * x + m_dbRotationMatrix[0][1] * y);
        turnPts[i].ptCoord.y = m_ptRoationCenter.y + long(m_dbRotationMatrix[1][0] * x + m_dbRotationMatrix[1][1] * y);
    }


    int nLeftTopIndex = MatchLeftTopCorner(turnPts, _countof(turnPts));
    if (nLeftTopIndex == -1)
    {
        return FALSE;
    }

    m_vecBorderCalibMap.resize(m_oMonitorBoundaryFinder.GetMarkerCount());


    int nImagePtIndex = turnPts[nLeftTopIndex].nLayerPtIndex;

    const std::vector<RECT>& borderMarkers = this->m_oMonitorBoundaryFinder.GetBorderMarkerPositions();

    for (int nScreenPtIndex = 0; nScreenPtIndex < m_oMonitorBoundaryFinder.GetMarkerCount(); nScreenPtIndex++)
    {

        //边界点的图像坐标
        m_vecBorderCalibMap[nScreenPtIndex].pt2DImageCoord.d[0] = double(m_vecBorderCalibratePts[nImagePtIndex].ptCentroid.x);
        m_vecBorderCalibMap[nScreenPtIndex].pt2DImageCoord.d[1] = double(m_vecBorderCalibratePts[nImagePtIndex].ptCentroid.y);


        //边界点的屏幕坐标
        //const RECT& rcSquare = m_vecBorderCirclePositions[nScreenPtIndex];
        const RECT& rcSquare = borderMarkers[nScreenPtIndex];

        m_vecBorderCalibMap[nScreenPtIndex].ptScreenCoord.x = (rcSquare.left + rcSquare.right) >> 1;
        m_vecBorderCalibMap[nScreenPtIndex].ptScreenCoord.y = (rcSquare.top + rcSquare.bottom) >> 1;


        POINT ptBorders[8];//外界矩形的8个边界点
        //
        //0    1     2
        // ●─●─●
        // │  │  │
        //7●─┼─●3
        // │  │  │  
        // ●─●─●
        //6   5      4

        const RECT& bound = m_vecBorderCalibratePts[nImagePtIndex].rcBound;

        ptBorders[0].x = bound.left;
        ptBorders[0].y = bound.top;

        ptBorders[1].x = (bound.left + bound.right) >> 1;
        ptBorders[1].y = bound.top;

        ptBorders[2].x = bound.right;
        ptBorders[2].y = bound.top;

        ptBorders[3].x = bound.right;
        ptBorders[3].y = (bound.top + bound.bottom) >> 1;

        ptBorders[4].x = bound.right;
        ptBorders[4].y = bound.bottom;

        ptBorders[5].x = (bound.left + bound.right) >> 1;
        ptBorders[5].y = bound.bottom;

        ptBorders[6].x = bound.left;
        ptBorders[6].y = bound.bottom;

        ptBorders[7].x = bound.left;
        ptBorders[7].y = (bound.top + bound.bottom) >> 1;

        long dx = (ptBorders[0].x - ptCentroid.x);
        long dy = (ptBorders[0].y - ptCentroid.y);
        long R2 = dx*dx + dy*dy;
        long MaxR2 = R2;

        vecBorderPts[nScreenPtIndex] = ptBorders[0];

        //在屏幕边界校正标记的外接矩形的8个边界点中，选取距离屏幕中心最远的点
        for (int i = 1; i < _countof(ptBorders); i++)
        {
            dx = (ptBorders[i].x - ptCentroid.x);
            dy = (ptBorders[i].y - ptCentroid.y);
            R2 = dx*dx + dy*dy;
            if (R2 > MaxR2)
            {
                MaxR2 = R2;
                vecBorderPts[nScreenPtIndex] = ptBorders[i];
            }
        }

        nImagePtIndex++;
        //if(nImagePtIndex == CMonitorBoundaryFinder::BORDER_MARKER_NUMBER)

        if (nImagePtIndex == m_oMonitorBoundaryFinder.GetMarkerCount())
        {
            nImagePtIndex = 0;
        }
    }

    if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
    {
        CImageFrame debugFrame = m_oScreenMaskFrame;

        for (size_t i = 0; i < vecBorderPts.size(); i++)
        {
            long x = vecBorderPts[i].x + 5;
            long y = vecBorderPts[i].y + 5;

            debugFrame.SetPixel(vecBorderPts[i], 0xFF);

            char szNo[32];
            sprintf_s(szNo, _countof(szNo), "%d", i + 1);
            debugFrame.PutStr(x, y, szNo, 0x80);

        }

        Debug_SaveImageFrame(debugFrame, _T("BorderVertices.jpg"));

    }

    std::vector<POINT> vecBorderPtsReduced = vecBorderPts;

    //过滤掉冗余的共线点
    int nBoderPointCount = FilterOutColinearPoints(
        &vecBorderPts[0],
        vecBorderPts.size(),
        &vecBorderPtsReduced[0]);


    vecBorderPts.resize(nBoderPointCount);
    memcpy(&vecBorderPts[0], &vecBorderPtsReduced[0], sizeof(POINT)*nBoderPointCount);


    std::vector<POINT> vecBorderExpandedPts = vecBorderPts;

    OutputDebugString(L" xxxxxx Entry ExpandPolygon\n");

    //多边形各边沿其重心与各边的垂线向外膨胀。
    BOOL bExpandSuccess = ExpandPolygon(
        &vecBorderPts[0],
        vecBorderPts.size(),
        m_tStaticMaskingParams.nMaskEroseSize,
        &vecBorderExpandedPts[0]);

    if (bExpandSuccess)
    {

        FillPolygon(
            m_oScreenMaskFrame.GetData(),
            nImageWidth,
            nImageHeight,
            &vecBorderExpandedPts[0],
            vecBorderExpandedPts.size(),
            255,
            TRUE);

        if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
        {
            CImageFrame debugFrame = m_oScreenMaskFrame;

            for (size_t i = 0; i < vecBorderPts.size(); i++)
            {
                POINT ptEnd;
                if (i == vecBorderPts.size() - 1)
                {
                    ptEnd = vecBorderPts[0];
                }
                else
                {
                    ptEnd = vecBorderPts[i + 1];
                }
                debugFrame.Line(vecBorderPts[i], ptEnd, 0x80);

            }

            for (size_t i = 0; i < vecBorderPts.size(); i++)
            {
                debugFrame.SetPixel(vecBorderPts[i], 0x40);
            }

            if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
            {
                Debug_SaveImageFrame(debugFrame, _T("FullScrenMask.jpg"));
            }
        }
    }
    else
    {
        FillPolygon(
            m_oScreenMaskFrame.GetData(),
            nImageWidth,
            nImageHeight,
            &vecBorderPts[0],
            vecBorderPts.size(),
            255,
            TRUE);

        if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
        {
            Debug_SaveImageFrame(m_oScreenMaskFrame, _T("FullScrenMask.jpg"));
        }

        //如果顶点法扩充失败的话，在用原有的办法扩充就好
        //屏幕区块膨胀8个像素
        for (int i = 0; i < m_tStaticMaskingParams.nMaskEroseSize; i++)
        {
            Morph_Dilate8(m_oScreenMaskFrame.GetData(), m_oScreenMaskFrame.GetData(), m_oScreenMaskFrame.Width(), m_oScreenMaskFrame.Height());
        }
    }

    //<<debug
    if (this->m_bIsSimulatedCalibrating)
    {
        std::vector<POINT> vecDbgData;
        std::vector<int> vecNo;

        vecDbgData.resize(this->m_vecBorderCalibMap.size());
        vecNo.resize(this->m_vecBorderCalibMap.size());

        for (std::vector<POINT>::size_type i = 0;
            i < vecDbgData.size();
            i++)
        {
            vecDbgData[i].x = long(m_vecBorderCalibMap[i].pt2DImageCoord.d[0]);
            vecDbgData[i].y = long(m_vecBorderCalibMap[i].pt2DImageCoord.d[1]);
            vecNo[i] = i;
        }
        this->m_oDebugWindow.PrintPtNo(vecDbgData, vecNo);
    }
    //debug>>


    OutputDebugString(L" xxxxxx m_oLowerHalfMaskFrame m_oUpperHalfMaskFrame\n");
    //初始化校正图案
    double dbRatio = 0.5;
    //计算上下部分面积之比的倒数，对校正图案第一行高度和最后一行的高度进行比例补偿。
    dbRatio = (double)m_oLowerHalfMaskFrame.Statistic(0xFF) / (double)m_oUpperHalfMaskFrame.Statistic(0xFF);
    //



    //设置图像中的校正点数组尺寸
    //m_vecInteriorCalibPoint.resize(this->m_oCalibratePattern.GetCalibrateMarkCount());

    //设置校正点图像坐标到屏幕坐标的映射数组的尺寸
    //m_vecInteriorCalibMap.resize(this->m_oCalibratePattern.GetCalibrateMarkCount());

    //内部校正点计数器
    m_nInteriorCalibPtNumber = 0;


    return TRUE;

}

//@功 能:在角点数组中, 查找左上角的匹配候选点。
//@参 数:turnPoints, 角点数组
// ptG, 重心坐标
// nTurnPoints, 角点个数
//@返回值:返回左上角的角点的数组索引号
// -1, 则未找到左上角的角点。
//@说 明: 该函数被函数DoSubAreaCornerMatch调用。
//
//在以圈内重心G为原点, 垂直向上为Y的坐标系内,
//  ↑y
//  |
//―+―→x
//  |0
//查找位于左上角的角点, 该角点的特征是处于2,3象限, 且与X轴正向夹角
//最小
//int CAutoCalibratorImpl2::MatchLeftTopCorner(const TurnPoint* turnPoints, POINT& ptG, int nTurnPointNumber)
int CAutoCalibratorImpl2::MatchLeftTopCorner(const TurnPoint* turnPoints, int nTurnPointNumber)
{
    int iMatchIndex = -1;


    //计算重心
    POINT ptG;

    int  sum_x = 0, sum_y = 0;
    for (int i = 0; i < nTurnPointNumber; i++)
    {
        sum_x += turnPoints[i].ptCoord.x;
        sum_y += turnPoints[i].ptCoord.y;
    }

    ptG.x = sum_x / nTurnPointNumber;
    ptG.y = sum_y / nTurnPointNumber;


    //注意到Tangent函数在[1/2π, 3/2π]之间单调递增,我们需要选取在[[1/2π, 3/2π]之间
    //Tangent值最小的角点
    double dbMinTan = (std::numeric_limits<double>::max)();

    for (int i = 0; i < nTurnPointNumber; i++)
    {
        int nX, nY;

        nX = turnPoints[i].ptCoord.x - ptG.x;
        nY = -(turnPoints[i].ptCoord.y - ptG.y);//取负,让垂直向上为正方向

        if (nX >= 0) continue;


        if (iMatchIndex == -1)
        {
            if (nX == 0)
            {
                if (nY > 0)
                {
                    dbMinTan = (std::numeric_limits<double>::min)();
                }
                else
                {
                    dbMinTan = (std::numeric_limits<double>::max)();
                }
            }
            else
            {
                dbMinTan = (double)nY / (double)nX;
            }

            iMatchIndex = i;
        }
        else
        {
            if (nX == 0)
            {
                if (nY > 0)
                {
                    dbMinTan = (std::numeric_limits<double>::min)();
                    iMatchIndex = i;
                }
            }
            else
            {
                double dbTan = (double)nY / (double)nX;
                if (dbMinTan > dbTan)
                {
                    dbMinTan = dbTan;
                    iMatchIndex = i;
                }
            }
        }
    }

    return iMatchIndex;
}




//@功能:合并边界校正点和屏幕内部的校正点, 
void CAutoCalibratorImpl2::OnPostCalibrate()
{
    //生成总屏蔽图
    ProduceAllMonitorMaskFrame();

    //生成屏幕映射图
    ProduceScreenMap();

    //输出结果
    m_tCalibData.szImage = GetImageSize();
    m_tCalibData.allMonitorCalibData.resize(m_vecScreenInfos.size());

    for (UINT i = 0; i < m_vecMonitorCalibResults.size(); i++)
    {
        m_tCalibData.allMonitorCalibData[i].calibData = m_vecMonitorCalibResults[i].calibMap;
        m_tCalibData.allMonitorCalibData[i].rcMonitor = m_vecScreenInfos[i].rcArea;
        m_tCalibData.allMonitorCalibData[i].radius = m_vecMonitorCalibResults[i].circleRadius;
    }
}


//@功能:收集当前显示屏幕的校正数据
//      在多屏模式下，一个屏幕有一组校正数据和屏蔽图
void CAutoCalibratorImpl2::CollectMonitorCalibrateData()
{

    int nInteriorCalibPtNumber = m_vecInteriorCalibMap.size();

    int nBorderCalibPtNumber = 0;

    if (this->m_bBorderCalibratrPtsIsValid)
    {
        nBorderCalibPtNumber = m_vecBorderCalibMap.size();
    }


    //边界点的数据 
     //m_vecMonitorCalibResults[m_nDispMonitorId].calibMap.resize(nBorderCalibPtNumber + nInteriorCalibPtNumber);
    m_vecMonitorCalibResults[m_nDispMonitorId].calibMap.resize(nInteriorCalibPtNumber);

    int nPtIndex = 0;

    /*
    for(int i=0; i<nBorderCalibPtNumber; i++)
    {
        m_vecMonitorCalibResults[m_nDispMonitorId].calibMap[nPtIndex].ptScreenCoord  = m_vecBorderCalibMap[i].ptScreenCoord;
        m_vecMonitorCalibResults[m_nDispMonitorId].calibMap[nPtIndex].ptImageCoord = m_vecBorderCalibMap[i].ptImageCoord;
        nPtIndex ++;
    }*/

    for (int i = 0; i < nInteriorCalibPtNumber; i++)
    {
        m_vecMonitorCalibResults[m_nDispMonitorId].calibMap[nPtIndex].ptScreenCoord = m_vecInteriorCalibMap[i].ptScreenCoord;
        m_vecMonitorCalibResults[m_nDispMonitorId].calibMap[nPtIndex].pt2DImageCoord = m_vecInteriorCalibMap[i].pt2DImageCoord;
        nPtIndex++;
    }

    //膨胀屏幕检测区域,即腐蚀屏蔽区
    /*
    for(int i=0; i < this->m_tStaticMaskingParams.nMaskEroseSize; i++)
    {
        Morph_Dilate8(
            m_oScreenMaskFrame.GetData(),
            m_oScreenMaskFrame.GetData(),
            m_oScreenMaskFrame.Width(),
            m_oScreenMaskFrame.Height());

    }
    */
    m_vecMonitorCalibResults[m_nDispMonitorId].maskFrame = m_oScreenMaskFrame;
    m_vecMonitorCalibResults[m_nDispMonitorId].circleRadius = this->m_oCalibratePattern.GetCircleRaidus();

}

//@功能:生成所有屏幕区域组成的总屏蔽图
void CAutoCalibratorImpl2::ProduceAllMonitorMaskFrame()
{
    m_oAllMonitorMaskFrame = m_vecMonitorCalibResults[0].maskFrame;

    //for(int nDispId=1; nDispId < m_oMonitorFinder.GetMonitorCount(); nDispId++)
    for (UINT nDispId = 1; nDispId < m_vecScreenInfos.size(); nDispId++)
    {
        m_oAllMonitorMaskFrame.Merge(m_vecMonitorCalibResults[nDispId].maskFrame);
    }//for

    if (!m_oStaticMaskFinder.GetMaskFrame().IsEmpty())
    {
        //这个是后面静态屏蔽得到的结果
        m_oAllMonitorMaskFrame &= this->m_oStaticMaskFinder.GetMaskFrame();
    }

    if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
    {
        //静态屏蔽图
        Debug_SaveImageFrame(m_oAllMonitorMaskFrame, _T("FullMask.jpg"));
    }
}

//@功能:生成光点到屏幕区号的映射数据
void CAutoCalibratorImpl2::ProduceScreenMap()
{
    int W = m_vecMonitorCalibResults[0].maskFrame.Width();
    int H = m_vecMonitorCalibResults[0].maskFrame.Height();


    m_oScreenMap.SetSize(W, H, 1);


    //for(int nDispId=0; nDispId < m_oMonitorFinder.GetMonitorCount(); nDispId++)
    for (UINT nDispId = 0; nDispId < m_vecScreenInfos.size(); nDispId++)
    {
        BYTE val = 1 << nDispId;
        BYTE* pMap = m_oScreenMap.GetData();
        BYTE* pSrc = m_vecMonitorCalibResults[nDispId].maskFrame.GetData();
        for (int r = 0; r < H; r++)
        {
            for (int c = 0; c < W; c++)
            {
                if (*pSrc)
                {
                    *pMap = *pMap | val;
                }
                pSrc++;
                pMap++;
            }
        }
    }

}

//@功能: 当前屏幕收集校正数据成功
//@说明: 继续下一个屏幕的校正，如果已经是最后一个屏幕了，则结束校正，通知主窗体。
void CAutoCalibratorImpl2::OnMonitorCalibrateDataReady()
{
    CollectMonitorCalibrateData();//收集校正数据
    m_nDispMonitorId++;//屏幕索引号+1

    if ((UINT)m_nDispMonitorId < m_vecScreenInfos.size())
    {
        //不是最后一个屏幕，则继续下一个屏幕的数据收集
        m_CurrentMonitorInfo = m_vecScreenInfos[m_nDispMonitorId];

        //计算图片输出目录
        m_strCurrentImageOutputDir.Format(
            _T("%s\\Monitor_%d"),
            (LPCTSTR)m_strImageOutputBaseDir,
            m_nDispMonitorId + 1);


        //设置图片输出目录
        CString strImageDubugDir;
        strImageDubugDir.Format(_T("%s\\%d"), m_strCurrentImageOutputDir, m_nTryTimes);
        SetImageDebugDir(strImageDubugDir);


        CAtlString strFileName;
        SYSTEMTIME localTime;
        GetLocalTime(&localTime);

        //自动校正的录像文件路径
        /*
        strFileName.Format(_T("%s\\AutoCalibrate_Sceen(%d)_%04d%02d%02d-%02d%02d%02d%03d_(%dX%d_%u).avi"),
            m_strCurrentImageOutputDir,
            m_nDispMonitorId + 1,
            localTime.wYear,
            localTime.wMonth,
            localTime.wDay,
            localTime.wHour,
            localTime.wMinute,
            localTime.wSecond,
            localTime.wMilliseconds,
            m_uMonitorWidth,
            m_uMonitorHeight,
            m_AutoBrightnessRegulator.GetExpectedBrightness());



        if(m_bRecordVideo)//需要保存校正视频,重新打开录像文件
        {
            m_oAVIRecorder.OpenFile(strFileName, 640, 480);
        }
        */


        //屏幕的校正尝试次数复位
        this->m_nTryTimes = 0;

        //复位用旧法搜索屏幕上下半区的标志
        m_bUseOldMethod = FALSE;

        //this->m_nStageWaitCount = 0;
        m_oWaitTimer.Reset();

        //开始新的屏幕校正
        m_eCalibrateStage = E_AUTO_CALIBRATE_START;
    }
    else
    {

        if (!this->m_bIsSimulatedCalibrating)
        {//非模拟校正模式
            //静态屏蔽前的预处理
            if (this->m_tStaticMaskingParams.fpPreStaticMaskingProc)
            {
                (*this->m_tStaticMaskingParams.fpPreStaticMaskingProc)(this->m_tStaticMaskingParams.lpPreStaticMaskingCtx);
            }
        }

        //阶段计数器
        //m_nStageWaitCount = 0;

        //阶段帧计数
        //m_nStageFrameCount = 0;
        m_oWaitTimer.Reset();

        //转入静态屏蔽阶段
        m_eCalibrateStage = E_START_MASKING;
    }

}

//@功能: 当前屏幕采集校正数据失败
//@说明: 如果尝试次数小于设定的最大尝试次数,则继续尝试；若已经尝试了最大次数，则结束校正，通知主窗体校正失败。
void CAutoCalibratorImpl2::OnMonitorCollectDataFail()
{
    m_nTryTimes++;
    if (m_nTryTimes == MAX_TRY_TIMES)
    {  //进行了最大次数的尝试,则结束数据收集工作。
        EndCalibrating();

        m_eCalibrateStage = E_AUTO_CALIBRATE_STOPPED;
    }
    else
    {
        //重新显示校正过程
        //设置图片输出目录
        CString strImageDubugDir;
        strImageDubugDir.Format(_T("%s\\%d"), m_strCurrentImageOutputDir, m_nTryTimes + 1);
        SetImageDebugDir(strImageDubugDir);

        if (m_eCalibrateError == E_AUTO_CALIBRATOR_ERROR_DETECT_DISPLAY_AREA_FAILED)
        {  //如果是检查屏幕边界错误, 则将边界标志直径加倍
            //SQUARE_SIZE += (SQUARE_SIZE>>1);
        }

        //if (FALSE == this->m_bIsSimulatedCalibrating)
        {
            //选取下一组画面数据
            int nIndex = m_nTryTimes % this->m_oautocalibrateparamslist.size();

            if (this->m_ePattern > E_CALIBRATE_PATTERN_9_COLS)
            {
                this->m_ePattern = E_AutoCalibratePattern((int)this->m_ePattern - 1);
            }

            ///////更改摄像头的
            if (m_fpChangeCameraParams)
            {
                m_fpChangeCameraParams(E_CAMERA_AUTO_CALIBRATE, m_lpCtx, 0, nIndex);
            }

            ///摄像头参数需要修改
            m_AutoBrightnessRegulator.SetAutoCalibrateParamsIndex(nIndex);
            //修改校正时期望的画面平均亮度
            m_AutoBrightnessRegulator.SetExpectedBrightness(m_oautocalibrateparamslist[nIndex].autoCalibrateImageParams.autoCalibrateExpectedBrightness);

            //自动校正时的校正图案中白色块的亮度值(0~255)        
            BYTE gray = m_oautocalibrateparamslist[nIndex].autoCalibrateImageParams.autoCalibrateHilightGray;
            m_clrGridHighlight = RGB(gray, gray, gray);

            //设置自动校正时间放大倍数
            //SetTimeMagnification(m_oautocalibrateparamslist[nIndex].autoCalibrateImageParams.autoCalibrateSpeed);
            const AutoCalibrateImageParams& autocalibrateParams = m_oautocalibrateparamslist[0].autoCalibrateImageParams;
            InitWaitTimer(autocalibrateParams.autoCalibrateSpeed, autocalibrateParams.videoDislayDelay);
        }

        //复位用旧法搜索屏幕上下半区的标志
        m_bUseOldMethod = FALSE;

        //this->m_nStageWaitCount = 0;
        m_oWaitTimer.Reset();

        m_eCalibrateStage = E_AUTO_CALIBRATE_START;
    }
}

BOOL CAutoCalibratorImpl2::FeedImage(const CImageFrame* pGrayFrame)
{
    switch (m_eWorkMode)
    {
    case E_WORK_MODE_AUTO_CALIBRATE:
        return FeedImage_AutoCalibrate(pGrayFrame);
        break;

    case E_WORK_MODEL_AUTO_MASKING:
        return FeedImage_AutoMask(pGrayFrame);
        break;
    }

    return FALSE;
}

//@功能:给自动校正器投喂图片驱动自动校正进程。
//@返回值:异常返回FALSE
BOOL CAutoCalibratorImpl2::FeedImage_AutoCalibrate(const CImageFrame* pGrayFrame)
{

    if (pGrayFrame == NULL) return FALSE;

    if (FALSE == this->m_bIsSimulatedCalibrating)
    {
        if (m_bRecordVideo)//根据设置保存视频
        {
            //m_oAVIRecorder.Write((const BYTE*)pYUVFrame->GetData(), pYUVFrame->Size());
        }
    }
    else
    {
        m_oDebugWindow.PrintFrame(*pGrayFrame);
    }

    m_nDbgFrameCount++;

    CImageFrame monoFrame = *pGrayFrame;

    BOOL bFailed = FALSE;

    static int s_LowerPartPixelCount = 0;
    static int s_UpperPartPixelCount = 0;


    //"测试亮度自动调节"模式
    if (m_bTestAutoBrightnessCtrlMode)
    {
        m_AutoBrightnessRegulator.ProcessImage(monoFrame.GetData(), monoFrame.Width(), monoFrame.Height());
        return TRUE;
    }


    BOOL bRet = FALSE;

    m_oWaitTimer.Trigger();

    switch (m_eCalibrateStage)
    {
    case E_AUTO_CALIBRATE_START://开始校正

        if (FALSE == this->m_bIsSimulatedCalibrating)//非仿真模式
        {
            if (m_fpChangeCameraParams)
            {
                //对比度调节到最大, 抑制噪声
                // m_fpChangeCameraParams(E_CAMERA_CONTRAST_MAX, m_lpCtx, 0);

                //对比度调节到最大值的2/3, 避免取最大对比度时, 对屏幕四个角处的校正图案的亮度抑制。
                //<<2014/04/21
                m_fpChangeCameraParams(E_CAMERA_CONTRAST, m_lpCtx, 170, 0);  //170
                //2014/04/21>>
            }
        }

        //显示全屏白色
        if (FALSE == this->m_bIsSimulatedCalibrating)//非仿真模式
        {
            FillBoard(m_AutoCalibrateWnd, FOREGROUND_COLOR, &m_CurrentMonitorInfo.rcArea);  //白色
        }

        //m_nStageWaitCount = 0;
        m_oWaitTimer.Reset();
        m_eCalibrateStage = E_AUTO_CHANGE_BRIGHTNESS_1;

        break;


    case E_AUTO_CHANGE_BRIGHTNESS_1://自动调节画面亮度
    {
        //m_nStageWaitCount ++;

        //自动亮度调节
        BYTE brightnessDiff = m_AutoBrightnessRegulator.ProcessImage(monoFrame.GetData(), monoFrame.Width(), monoFrame.Height());
        // if(m_nStageWaitCount < 1*60)//至少调节1秒钟。
        if (!m_oWaitTimer.IsWaitTimeout(1000))//至少调节1秒钟。
        {
            break;
        }

        // if(brightnessDiff > 10 && m_nStageWaitCount < 2*60)//最大调节时间是2秒钟
        if (brightnessDiff > 10 && !m_oWaitTimer.IsWaitTimeout(2000))//最大调节时间是2秒钟
        {
            break;
        }

        //复位边界点坐标有效标志
        m_bBorderCalibratrPtsIsValid = FALSE;

        m_oMonitorAreaLocator.Reset(monoFrame.Width(), monoFrame.Height(), m_AutoCalibrateWnd, m_CurrentMonitorInfo.rcArea, this->m_eDebugLevel);

        //m_nStageWaitCount = 0;
        m_oWaitTimer.Reset();

        //需要打出四块屏蔽图，因为需要计算重心的值。
        m_eCalibrateStage = E_SEARCH_SCREEN_AREA;

    }
    break;


    case E_SEARCH_SCREEN_AREA:

        bRet = m_oMonitorAreaLocator.Process(monoFrame, m_bIsSimulatedCalibrating);
        if (!bRet)
        {
            this->OnMonitorCollectDataFail();
            break;
        }
        else
        {
            if (m_oMonitorAreaLocator.IsSuccess())
            {
                m_oInitialScreenMask = m_oMonitorAreaLocator.GetMaskFrame();

                //屏幕区块膨胀10个像素
                for (int i = 0; i < 10; i++)
                {
                    Morph_Dilate8(m_oInitialScreenMask.GetData(), m_oInitialScreenMask.GetData(), m_oInitialScreenMask.Width(), m_oInitialScreenMask.Height());
                }

                if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
                {
                    Debug_SaveImageFrame(m_oInitialScreenMask, _T("InitialScreenMaskDilated.jpg"));
                }

                //根据边界点划分屏幕区域
                PartitionScreenArea();
                if (m_vecPolygons.size() == 0)
                {
                    this->OnMonitorCollectDataFail();
                    break;
                }

                m_oMonitorBoundaryFinder.Reset(m_oInitialScreenMask, m_AutoCalibrateWnd, m_CurrentMonitorInfo.rcArea, this->m_eDebugLevel, this->m_bIsSimulatedCalibrating);
                //设置子区域多边形
                m_oMonitorBoundaryFinder.SetSubAreaPolygons(&this->m_vecPolygons[0], 8);

                //m_nStageWaitCount = 0;
                m_oWaitTimer.Reset();

                m_eCalibrateStage = E_SEARCH_SCREEN_IMAGE_BOUNDARY;

            }
        }

        break;


    case E_AUTO_CHANGE_BRIGHTNESS_2://第二次自动调节画面亮度
    {

        //if(m_nStageWaitCount < 60)//延时1秒钟
        if (!m_oWaitTimer.IsWaitTimeout(1000))//延时1秒钟
        {
            break;
        }
        else
        {
            m_oWaitTimer.Reset();
            m_eCalibrateStage = E_SEARCH_SCREEN_IMAGE_BOUNDARY;
        }

    }
    break;

    case E_SEARCH_SCREEN_IMAGE_BOUNDARY:

        //屏幕影像边界查找
        bRet = this->m_oMonitorBoundaryFinder.Process(monoFrame, m_bIsSimulatedCalibrating);

        if (!bRet)
        {
            OutputDebugString(L" xxxxxx m_oMonitorBoundaryFinder Process bRet false\n");
            //精确查找屏幕影像轮廓失败
            m_oScreenMaskFrame = this->m_oInitialScreenMask;

            //显示黑屏
            if (FALSE == this->m_bIsSimulatedCalibrating)
            {
                FillBoard(m_AutoCalibrateWnd, BACKGROUND_COLOR, &m_CurrentMonitorInfo.rcArea);
            }


            if (FALSE == this->m_bIsSimulatedCalibrating)//非仿真模式
            {
                if (m_fpChangeCameraParams)
                {
                    //对比度调节到缺省值,为的是看清校正图案
                    m_fpChangeCameraParams(E_CAMERA_CONTRAST_DEFAULT, m_lpCtx, 0, 0);
                    //m_fpChangeCameraParams(E_CAMERA_AUTO_CALIBRATE, m_lpCtx, 0);
                }
            }

            //m_nStageWaitCount = 0;
            m_oWaitTimer.Reset();

            //转入校正棋盘格上部阶段
            m_eCalibrateStage = E_SEARCH_BACKGROUND;

            //使用旧方法判断旋转方向
            m_bUseOldMethod = TRUE;

        }
        else
        {
            OutputDebugString(L" xxxxxx m_oMonitorBoundaryFinder Process bRet true\n");
            if (!this->m_oMonitorBoundaryFinder.IsDone()) break;

            OutputDebugString(L" xxxxxx OnPostSearchScreenBoundary\n");
            //屏幕边界搜索后续处理
            BOOL bRet = OnPostSearchScreenBoundary(monoFrame.Width(), monoFrame.Height());

            if (bRet)
            {//精确搜索屏幕区域成功

                //if(m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
                //{
                //    Debug_SaveImageFrame(m_oDebugBorderSquareFrame, _T("BorderSqaureMatchResult.jpg"));
                //}

                if (FALSE == this->m_bIsSimulatedCalibrating)
                {
                    FillBoard(m_AutoCalibrateWnd, 0, BACKGROUND_COLOR);
                }


                if (FALSE == this->m_bIsSimulatedCalibrating)//非仿真模式
                {
                    if (m_fpChangeCameraParams)
                    {
                        //对比度调节到缺省值,为的是看清校正图案
                        m_fpChangeCameraParams(E_CAMERA_CONTRAST_DEFAULT, m_lpCtx, 0, 0);
                    }
                }

                //m_nStageWaitCount = 0;
                m_oWaitTimer.Reset();

                m_bBorderCalibratrPtsIsValid = TRUE;//设置边界点坐标有效标志

               //不使用旧方法判断旋转方向
                m_bUseOldMethod = FALSE;

                //转入校正棋盘格上部阶段
                m_eCalibrateStage = E_SEARCH_BACKGROUND;
            }
            else
            {
                //精确搜索屏幕区域失败
                m_oScreenMaskFrame = this->m_oInitialScreenMask;

                //显示黑屏
                if (FALSE == this->m_bIsSimulatedCalibrating)
                {
                    FillBoard(m_AutoCalibrateWnd, BACKGROUND_COLOR, &m_CurrentMonitorInfo.rcArea);
                }

                if (FALSE == this->m_bIsSimulatedCalibrating)//非仿真模式
                {
                    if (m_fpChangeCameraParams)
                    {
                        //对比度调节到缺省值,为的是看清校正图案
                        m_fpChangeCameraParams(E_CAMERA_CONTRAST_DEFAULT, m_lpCtx, 0, 0);
                        //m_fpChangeCameraParams(E_CAMERA_AUTO_CALIBRATE, m_lpCtx, 0);
                    }
                }
                //m_nStageWaitCount = 0;
                m_oWaitTimer.Reset();

                //转入校正棋盘格上部阶段
                m_eCalibrateStage = E_SEARCH_BACKGROUND;

                //使用旧方法判断旋转方向
                m_bUseOldMethod = TRUE;
            }
        }

        break;


    case E_SEARCH_BACKGROUND://搜索背景
        //自动亮度调节
        //if(!this->m_bIsSimulatedCalibrating)
        //m_AutoBrightnessRegulator.ProcessImage(monoFrame.GetData(), monoFrame.Width(), monoFrame.Height());

        //m_nStageWaitCount++;
        //if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)
        if (m_oWaitTimer.IsWaitTimeout(MAX_NEED_WAIT_TIME))
        {
            m_oBackgroundFrame = monoFrame;

            if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
            {
                Debug_SaveImageFrame(m_oBackgroundFrame, _T("Background.jpg"));
            }

            if (m_bUseOldMethod)//使用旧法，判断镜头旋转方向
            {

                RECT rcUpperArea = this->m_CurrentMonitorInfo.rcArea;
                rcUpperArea.bottom = rcUpperArea.top + (rcUpperArea.bottom - rcUpperArea.top) / 2;

                if (FALSE == this->m_bIsSimulatedCalibrating)
                {
                    //显示上半部白屏
                    FillBoard(m_AutoCalibrateWnd, FOREGROUND_COLOR, &rcUpperArea);
                }

                //m_nStageWaitCount = 0;
                m_oWaitTimer.Reset();

                m_eCalibrateStage = E_DETECT_ROTATION_1;
            }
            else
            {
                if (FALSE == this->m_bIsSimulatedCalibrating)//非模拟
                {
                    m_liImageGrdientEnergy = 0;

                    //绘制上半部的校正图案
                    //m_oCalibratePattern.DrawUpperPartPattern(this->m_AutoCalibrateWnd, m_clrGridHighlight);
                     //m_oCalibratePattern.DrawPattern(this->m_AutoCalibrateWnd);
                }

                //m_nStageWaitCount = 0;
                m_oWaitTimer.Reset();

                m_eCalibrateStage = E_PRINT_PATTERN;//转入校正图案打印输出阶段
            }

        }
        break;


    case E_DETECT_ROTATION_1://侦测旋转方向第一步, 侦测上半部高亮区域重心
        //m_nStageWaitCount++;

        //if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)
        if (m_oWaitTimer.IsWaitTimeout(MAX_NEED_WAIT_TIME))
        {
            monoFrame -= m_oBackgroundFrame;

            //调试输出
            if (this->m_bIsSimulatedCalibrating)
            {
                m_oDebugWindow.PrintFrame(monoFrame);
            }


            //二值化灰度图图片
            //统计直方图
            int hist[256];
            memset(hist, 0, sizeof(hist));
            int nLoopCount = monoFrame.GetPixelCount();;
            const BYTE* pPixel = monoFrame.GetData();
            while (nLoopCount)
            {
                hist[*pPixel] ++;
                pPixel++;
                nLoopCount--;
            }

            //ostu法查找最优门限
            BYTE threshold = FindThreshold_Ostu(hist);
            threshold = (BYTE)((int)threshold * 85 / 100);//门限降低至85%

            monoFrame.Binarize(threshold);

            //在二值图中提取面积最大的高亮区域。
            RECT rcBoundary;
            int nPixelCount = 0;
            POINT ptCenter;

            //计算白块的重心
            BOOL bRet = ExtractTheLargestObject(monoFrame, &rcBoundary, &nPixelCount, &ptCenter);

            if (!bRet)
            {
                bFailed = TRUE;
            }
            else
            {
                m_vRotation.ptFrom.x = ptCenter.x;
                m_vRotation.ptFrom.y = ptCenter.y;
                //m_ptUpperCentroid       = ptCenter;
                s_UpperPartPixelCount = nPixelCount;
            }

            if (bFailed)
            {
                m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_DETECT_DISPLAY_AREA_FAILED;
                //失败
                this->OnMonitorCollectDataFail();
                break;
            }
            else
            {
                //上半部屏蔽图
                m_oUpperHalfMaskFrame = monoFrame;

                if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
                {
                    Debug_SaveImageFrame(monoFrame, _T("Rotation_01.jpg"));
                }
                //调试输出
                if (this->m_bIsSimulatedCalibrating)
                {
                    m_oDebugWindow.PrintFrame(monoFrame);
                }

                if (FALSE == this->m_bIsSimulatedCalibrating)
                {
                    FillBoard(m_AutoCalibrateWnd, BACKGROUND_COLOR);//全黑

                    RECT rcLowerArea = this->m_CurrentMonitorInfo.rcArea;

                    rcLowerArea.top = rcLowerArea.top + (rcLowerArea.bottom - rcLowerArea.top) / 2;

                    //显示下半部白屏
                    FillBoard(m_AutoCalibrateWnd, FOREGROUND_COLOR, &rcLowerArea);

                }
                //m_nStageWaitCount = 0;
                m_oWaitTimer.Reset();
                m_eCalibrateStage = E_DETECT_ROTATION_2;
            }
        }

        break;

    case E_DETECT_ROTATION_2://侦测旋转方向第二步, 侦测下半部高亮区域重心

        //m_nStageWaitCount++;

        //if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)
        if (m_oWaitTimer.IsWaitTimeout(MAX_NEED_WAIT_TIME))
        {
            monoFrame -= m_oBackgroundFrame;

            //调试输出
            if (this->m_bIsSimulatedCalibrating)
            {
                m_oDebugWindow.PrintFrame(monoFrame);
            }


            //二值化灰度图图片
            int hist[256];
            memset(hist, 0, sizeof(hist));
            int nLoopCount = monoFrame.GetPixelCount();;
            const BYTE* pPixel = monoFrame.GetData();
            while (nLoopCount)
            {
                hist[*pPixel] ++;
                pPixel++;
                nLoopCount--;
            }

            //ostu法查找最优门限
            BYTE threshold = FindThreshold_Ostu(hist);
            threshold = (BYTE)((int)threshold * 85 / 100);//门限降低至85%
            monoFrame.Binarize(threshold);

            //调试输出
            if (this->m_bIsSimulatedCalibrating)

            {
                m_oDebugWindow.PrintFrame(monoFrame);
            }
            //在二值图中提取面积最大的高亮区域。
            RECT rcBoundary;
            int nPixelCount = 0;
            POINT ptCenter;

            //计算白块的重心
            BOOL bRet = ExtractTheLargestObject(monoFrame, &rcBoundary, &nPixelCount, &ptCenter);

            if (!bRet)
            {
                bFailed = TRUE;
            }
            else
            {
                m_vRotation.ptTo.x = ptCenter.x;
                m_vRotation.ptTo.y = ptCenter.y;
                s_LowerPartPixelCount = nPixelCount;
            }

            if (bFailed)
            {

                m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_DETECT_DISPLAY_AREA_FAILED;
                this->OnMonitorCollectDataFail();
                break;

                //m_eCalibrateStage = E_AUTO_CALIBRATE_END;
            }
            else
            {
                //下半部屏蔽图
                m_oLowerHalfMaskFrame = monoFrame;

                if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
                {
                    Debug_SaveImageFrame(monoFrame, _T("Rotation_02.jpg"));
                }

                //调试输出
                if (this->m_bIsSimulatedCalibrating)
                {
                    m_oDebugWindow.PrintFrame(monoFrame);
                }

                //合并屏蔽图
                this->m_oScreenMaskFrame |= m_oUpperHalfMaskFrame;
                this->m_oScreenMaskFrame |= m_oLowerHalfMaskFrame;
                //补洞操作
                FillHole(m_oScreenMaskFrame);

                //膨胀屏蔽图
                for (int i = 0; i < 10; i++)
                {
                    Morph_Dilate8(
                        m_oScreenMaskFrame.GetData(),
                        m_oScreenMaskFrame.GetData(),
                        m_oScreenMaskFrame.Width(),
                        m_oScreenMaskFrame.Height());

                }

                if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
                {
                    Debug_SaveImageFrame(monoFrame, _T("FullScreenMask.jpg"));
                }

                //将所有角点做旋转操作, 使之在 ↓Y →X坐标系
                //计算旋转中心
                m_ptRoationCenter.x = (m_vRotation.ptFrom.x + m_vRotation.ptTo.x) / 2;
                m_ptRoationCenter.y = (m_vRotation.ptFrom.y + m_vRotation.ptTo.y) / 2;


                //
                //旋转矢量
                POINT Vx, Vy;
                double VLength;

                Vy.x = m_vRotation.ptTo.x - m_ptRoationCenter.x;
                Vy.y = m_vRotation.ptTo.y - m_ptRoationCenter.y;

                VLength = sqrt(double(Vy.x*Vy.x + Vy.y * Vy.y));

                Vx.x = Vy.y;
                Vx.y = -Vy.x;

                //计算旋转矩阵
                //
                // x | c11 c12 |
                // = |         |
                // y |c21 c22  |
                //double c11, c12, c21, c22;

                m_dbRotationMatrix[0][0] = (double)Vx.x / VLength;
                m_dbRotationMatrix[0][1] = (double)Vx.y / VLength;

                m_dbRotationMatrix[1][0] = (double)Vy.x / VLength;
                m_dbRotationMatrix[1][1] = (double)Vy.y / VLength;


                if (FALSE == this->m_bIsSimulatedCalibrating)
                {
                    FillBoard(m_AutoCalibrateWnd, BACKGROUND_COLOR);//全黑
                }

                //==================================================================
                //初始化校正图案
                double dbRatio = 0.5;
                //计算上下部分面积之比的倒数
                //
                dbRatio = (double)s_LowerPartPixelCount / (double)s_UpperPartPixelCount;
                //

                //初始化校正图案
                //m_oCalibratePattern.InitPattern(this->m_ePattern, this->m_CurrentMonitorInfo.rcMonitor, dbRatio);

                if (FALSE == this->m_bIsSimulatedCalibrating)
                {
                    // m_liImageGrdientEnergy = 0;

                     //绘制上半部的校正图案
                     //m_oCalibratePattern.DrawUpperPartPattern(this->m_AutoCalibrateWnd, m_clrGridHighlight);
                     //m_oCalibratePattern.DrawPattern(this->m_AutoCalibrateWnd);
                }

                //m_nStageWaitCount = 0;
                m_oWaitTimer.Reset();
                //m_nStageFrameCount = 0;
                m_eCalibrateStage = E_PRINT_PATTERN; // 转入校正图案打印输出阶段
            }
        }

        break;

    case  E_PRINT_PATTERN://打印校正图案
        if (FALSE == this->m_bIsSimulatedCalibrating)//非模拟校正状态。
        {
            m_liImageGrdientEnergy = 0;

            //初始化校正图案
            int nScreenWidth = this->m_CurrentMonitorInfo.rcArea.right - this->m_CurrentMonitorInfo.rcArea.left;
            int nScreenHeight = this->m_CurrentMonitorInfo.rcArea.bottom - this->m_CurrentMonitorInfo.rcArea.top;


            int Radius_A = m_oCalibratePattern.CalculateCalibPatternRadius(this->m_ePattern, nScreenWidth);
            int Radius_B = m_oCalibratePattern.CalculateCalibPatternRadius(this->m_ePattern, nScreenHeight);

            //取两半径数值小者
            int Radius = Radius_A > Radius_B ? Radius_B : Radius_A;

            m_oCalibratePattern.InitPattern(Radius, this->m_CurrentMonitorInfo.rcArea);


            //显示校正图案
            m_oCalibratePattern.DrawPattern(this->m_AutoCalibrateWnd, m_clrGridHighlight, BACKGROUND_COLOR);

            if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
            {
                DbgDumpPatternInfo(Radius, this->m_CurrentMonitorInfo.rcArea);
            }

        }
        //设置校正点图像坐标到屏幕坐标的映射数组的尺寸
        m_vecInteriorCalibMap.resize(this->m_oCalibratePattern.GetCalibrateMarkCount());


        //内部校正点计数器
        m_nInteriorCalibPtNumber = 0;

        m_oWaitTimer.Reset();
        m_eCalibrateStage = E_WAIT_PATTERN;
        break;

    case E_WAIT_PATTERN:
        if (m_oWaitTimer.IsWaitTimeout(MAX_NEED_WAIT_TIME))
        {
            m_oPatternFrame = monoFrame;
            m_liImageGrdientEnergy = GradientEnergy(monoFrame.GetData(), m_oScreenMaskFrame.GetData(), monoFrame.Width(), monoFrame.Height());

            m_oWaitTimer.Reset();
            m_eCalibrateStage = E_PROCESS_PATTERN;
        }

        break;


    case E_PROCESS_PATTERN://处理校正图案
        if (m_oWaitTimer.IsWaitTimeout(MAX_NEED_WAIT_TIME))
        {
            //选取最清晰的上部图片(即梯度能量最大)的图片
            __int64 liCurrentEnergy = GradientEnergy(monoFrame.GetData(), m_oScreenMaskFrame.GetData(), monoFrame.Width(), monoFrame.Height());

            if (liCurrentEnergy > m_liImageGrdientEnergy)
            {
                m_liImageGrdientEnergy = liCurrentEnergy;
                m_oPatternFrame = monoFrame;
            }

            //if(m_nStageWaitCount < MAX_STAGE_WAIT_COUNT*2)  break;//等待时间不够,继续等待。
            if (!m_oWaitTimer.IsWaitTimeout(MAX_NEED_WAIT_TIME * 2))  break;//等待时间不够,继续等待。


            if (this->m_bIsSimulatedCalibrating)
            {
                m_oDebugWindow.PrintFrame(m_oPatternFrame);
                m_oDebugWindow.PrintFrame(m_oScreenMaskFrame);
                m_oDebugWindow.PrintFrame(m_oBackgroundFrame);
            }

            if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
            {
                Debug_SaveGrayToPNG(m_oPatternFrame.GetData(), m_oPatternFrame.Width(), m_oPatternFrame.Height(), _T("Pattern.png"));
            }

            m_oRawPatternFrame = m_oPatternFrame;//保存原始的校正图案

            //预处理,减除背景
            PreProcessPatternImage(m_oPatternFrame, m_oScreenMaskFrame, m_oBackgroundFrame, m_oPatternFrame);

            //<<debug
            if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
            {

                Debug_SaveGrayToPNG(m_oPatternFrame.GetData(), m_oPatternFrame.Width(), m_oPatternFrame.Height(), _T("PatternSubstractBackground.png"));
            }

            //debug>>
            if (this->m_bIsSimulatedCalibrating)
            {
                m_oDebugWindow.PrintFrame(m_oPatternFrame);
            }

            //二值化校正图案.
            CBitFrame bitFrame;
            BinarizeImage(m_oPatternFrame, m_oScreenMaskFrame, bitFrame);

            CImageFrame binarizedGrayFrame;
            binarizedGrayFrame.SetSize(bitFrame.Width(), bitFrame.Height(), 1);

            //二值化的1位位图转为8位灰度位图
            BitToGrayFrame_MMX(
                (const BYTE*)bitFrame.GetData(),
                binarizedGrayFrame.GetData(),
                bitFrame.GetPixelCount());

            //<<debug
            //调试时输出二值化图片
            if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
            {
                Debug_SaveGrayToPNG(binarizedGrayFrame.GetData(), binarizedGrayFrame.Width(), binarizedGrayFrame.Height(), _T("BinarizedPattern.png"));
            }
            //>>

         //补洞
            Morph_FillHole(binarizedGrayFrame.GetData(), binarizedGrayFrame.GetData(), binarizedGrayFrame.Width(), binarizedGrayFrame.Height());

            //
             //<<debug
             //调试时输出二值化图片
            if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
            {
                Debug_SaveGrayToPNG(binarizedGrayFrame.GetData(), binarizedGrayFrame.Width(), binarizedGrayFrame.Height(), _T("BinarizedPatternWithHoleFilled.png"));
            }
            //>>
            BinarizeGrayToBitFrame_SSE2(binarizedGrayFrame.GetData(), (unsigned char*)bitFrame.GetData(), 0x80, binarizedGrayFrame.GetPixelCount());

            //提取二值图片中blobs的重心。
            CConnectedComponentScanlineAlogrithm<> blobDetector;

            blobDetector.ProcessImage((const BYTE*)bitFrame.GetData(), bitFrame.Width(), bitFrame.Height());

            const TBlobObject* pObjs = blobDetector.GetObjs();
            int nObjCount = blobDetector.GetObjCount();


            if (nObjCount < m_oCalibratePattern.GetCalibrateMarkCount())
            {//找到的目标小于
                bRet = FALSE;//失败
            }
            else
            {
                //blobDetector已经将找到的Blob按照质量从大到小排列了, 即pObj[0]质量最大。
                CollectCalibrateMarkers(
                    pObjs,
                    m_oCalibratePattern.GetCalibrateMarkCount(),
                    m_dbRotationMatrix,
                    m_ptRoationCenter);

                //<<debug
                //DebugTool_LoadCalibrateData("CalibrateData.txt", m_dbRotationMatrix, m_ptRoationCenter);
                //debug>>

                std::vector<int> vecImage2ScreenMap;//"检测到的校正点"到"显示的校正点编号"的映射关系。

                 //对所有校正点进行匹配
                bRet = DoCornerMatch(m_vecInteriorCalibPoint, vecImage2ScreenMap);

                if (bRet)
                {
                    for (std::vector<int>::size_type i = 0; i < vecImage2ScreenMap.size(); i++)
                    {
                        if (vecImage2ScreenMap[i] != -1)
                        {
                            int nCornerIndex = vecImage2ScreenMap[i];

                            m_vecInteriorCalibMap[nCornerIndex].ptScreenCoord = this->m_oCalibratePattern.GetMarkCoord(nCornerIndex);
                            m_vecInteriorCalibMap[nCornerIndex].pt2DImageCoord = this->m_vecInteriorCalibPoint[i].pt2DImageCoord;
                        }
                    }

                    //输出校正点采集结果
                    CRGBAFrame argbaFrame;
                    //argbFrame
                    argbaFrame.SetSize(m_oRawPatternFrame.Width(), m_oRawPatternFrame.Height());
                    GrayToARGB_SSE2(m_oRawPatternFrame.GetData(), argbaFrame.GetData(), m_oRawPatternFrame.GetPixelCount());
                    for (size_t i = 0; i < m_vecInteriorCalibMap.size(); i++)
                    {
                        POINT pt;
                        pt.x = long(m_vecInteriorCalibMap[i].pt2DImageCoord.d[0] + .5);
                        pt.y = long(m_vecInteriorCalibMap[i].pt2DImageCoord.d[1] + .5);

                        argbaFrame.SetPixel(
                            pt.x,
                            pt.y,
                            0xFF0000FF);
                    }

                    Debug_SaveRGBAToPNG(argbaFrame.GetData(), argbaFrame.Width(), argbaFrame.Height(), _T("CalibratePoints.png"));

                }
            }
            if (!bRet)
            {
                m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_CORNER_MATCH_FAILED;

                OnMonitorCollectDataFail();
                //转入校正结束阶段
                //m_eCalibrateStage = E_AUTO_CALIBRATE_END;
                break;
            }
            else
            {
                //显示其为全屏黑色，为其他屏幕校正作准备
                if (!this->m_bIsSimulatedCalibrating)
                {
                    FillBoard(m_AutoCalibrateWnd, BACKGROUND_COLOR, &m_CurrentMonitorInfo.rcArea);
                }

                //成功的校正结果
                m_eCalibrateError = E_AUTO_CALIBRATOR_OK;
                OnMonitorCalibrateDataReady();
            }
        }//if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)

        break;

    case E_START_MASKING:
        //设置静态屏蔽镜头参数,合上滤光片
        if (FALSE == this->m_bIsSimulatedCalibrating)//非仿真模式
        {
            if (m_fpChangeCameraParams)
            {
                m_fpChangeCameraParams(E_CAMERA_AUTO_MASK, m_lpCtx, 0, 0);
            }
        }

        //初始化静态屏蔽器
        this->m_oStaticMaskFinder.Reset(
            monoFrame.Width(),
            monoFrame.Height(),
            m_tStaticMaskingParams.cStaticMaskThreshold,
            m_AutoCalibrateWnd,
            m_CurrentMonitorInfo.rcArea, true);  //为fasle的话说明不显示数字

        m_oWaitTimer.Reset();

        //转入静态屏蔽阶段
        m_eCalibrateStage = E_WAIT_MASKING;

        break;

    case E_WAIT_MASKING:

        if (m_oWaitTimer.IsWaitTimeout(MAX_NEED_WAIT_TIME))
        {

            m_oWaitTimer.Reset();

            //转入静态屏蔽阶段
            m_eCalibrateStage = E_END_MASKING;

        }
        break;

        //case E_STATIC_MASKING:
    case E_END_MASKING:
        ////m_nStageWaitCount ++;
        //m_nStageFrameCount ++;
        ////if(m_nStageWaitCount == 1)
        //if (m_nStageFrameCount == 1)
        //{
        //    //设置静态屏蔽镜头参数,合上滤光片
        //    if(FALSE == this->m_bIsSimulatedCalibrating)//非仿真模式
        //    {
        //        if(m_fpChangeCameraParams)
        //        { 
        //            m_fpChangeCameraParams(E_CAMERA_AUTO_MASK, m_lpCtx, 0, 0);
        //        }
        //    }

        //}
        //
        ////else if(m_nStageWaitCount < MAX_STAGE_WAIT_COUNT)
        //else if (m_nStageFrameCount < MAX_STAGE_FRAME_COUNT)
        //{   
        //    //just skip
        //}
        ////else if(m_nStageWaitCount == MAX_STAGE_WAIT_COUNT)
        //else if (MAX_STAGE_FRAME_COUNT == m_nStageFrameCount )
        //{
        //    //初始化静态屏蔽器
        //    this->m_oStaticMaskFinder.Reset(
        //        monoFrame.Width(),
        //        monoFrame.Height(),
        //        m_tStaticMaskingParams.cStaticMaskThreshold,
        //        m_AutoCalibrateWnd,
        //        m_CurrentMonitorInfo.rcArea,false);  //为fasle的话说明不显示数字
        //}
        ////else 
        ////else if(MAX_STAGE_FRAME_COUNT < m_nStageFrameCount && m_oWaitTimer.IsWaitTimeout(MAX_NEED_WAIT_TIME))

        if (m_oWaitTimer.IsWaitTimeout(MAX_NEED_WAIT_TIME))
        {//静态屏蔽处理
            if (this->m_oStaticMaskFinder.Process(monoFrame, true))
            {
                //自动屏蔽结束
                m_eCalibrateStage = E_AUTO_CALIBRATE_END;

                //成功的校正结果
                m_eCalibrateError = E_AUTO_CALIBRATOR_OK;
            }
        }

        break;

    case E_AUTO_CALIBRATE_END:
        m_nTryTimes++;

        if (E_AUTO_CALIBRATOR_OK == this->m_eCalibrateError)
        {//校正成功

            //准备校正结果
            OnPostCalibrate();

            m_eCalibrateStage = E_AUTO_CALIBRATE_STOPPED;
        }

        //完成校正
        EndCalibrating();
        break;

    case E_AUTO_CALIBRATE_STOPPED:
        return FALSE;

    }//switch

    return TRUE;
}


//@功能:返回屏幕个数
int CAutoCalibratorImpl2::GetMonitorCount()const
{
    //return m_oMonitorFinder.GetMonitorCount();
    return m_vecScreenInfos.size();
}


//@功能:获得校正标记的坐标
//@参数:nIndex, 校正标记的索引编号
//@功能:获取指定屏幕的校正数据
//@参数:nMonitorID， 显示器Id
//@说明:
//const TMonitorCalibData* CAutoCalibratorImpl2::GetMonitorCalibData(int nMonitorID) const
//{
//    //if(nMonitorID > m_oMonitorFinder.GetMonitorCount()) 
//    if(nMonitorID > m_vecMonitorCalibResults.size()) 
//    {
//        throw std::runtime_error("GetMonitorCalibData invalid parameters");
//    }
//    TMonitorCalibData data;
//    data.calibData = m_vecMonitorCalibResults[nMonitorID].calibMap;
//    data.rcMonitor = m_vecMonitors[nMonitorID].rcMonitor;
//    return &m_vecMonitorCalibResults[nMonitorID];
//}


//@功能:获取所有屏幕的校正数据
const TCalibData* CAutoCalibratorImpl2::GetCalibrateData() const
{
    return &m_tCalibData;
}

//@功能:返回校正窗体句柄
HWND CAutoCalibratorImpl2::GetCalibrateHWnd()const
{
    return (HWND)m_AutoCalibrateWnd;
}


CAtlString  CAutoCalibratorImpl2::GetCalibrateIntermediataDirName()
{
    return g_strImageDebugRoot;
}


////@功能:子区域校正
////@参数:
////     nExpectedMarkerNumber,期望找到的校正点的数目
////     maskFrame, 屏蔽图片
////     srcFrame, 校正图片
////     roateMatrix, 将校正图样旋转到水平垂直时使用的旋转矩阵系数
////     ptCenter, 旋转中心的图像坐标
////     nGridSize, 棋盘格尺寸
////    nAreaNo,区域编号
////@说明:找到的角点存放到数组m_vecInteriorCalibPoint中
//BOOL CAutoCalibratorImpl2::SubAreaCornerDetect(
//    int nExpectedMarkerNumber,
//    const CImageFrame& maskFrame,
//    const CImageFrame& srcFrame,
//    const double m[2][2],
//    const POINT& ptCenter,
//    int nGridSize,
//    int nAreaNo)
//{
//    BOOL bRet = TRUE;
//
//    //强鲁棒性角点检测
//    //CRobustCornerDetector cornerDetector;
//
//    //bRet = cornerDetector(srcFrame, maskFrame, nExpectedMarkerNumber, nDetectRadius, this->m_eDebugLevel, m_nTryTimes);
//
//    //CImageFrame rawImage = srcFrame;
//
//
//    CCrossDetector fastCornerDetector;
//    bRet = fastCornerDetector(
//        srcFrame.GetData(),
//        srcFrame.Width(),
//        srcFrame.Height(),
//        nExpectedMarkerNumber, 
//        nGridSize,
//        maskFrame.GetData(),
//        this->m_eDebugLevel, 
//        m_nTryTimes);
//
//    if(bRet)
//    {
//        //注意:检测器检测到的校正点坐标已经被放大2^INT_SCALE_SHIFT_SIZE倍。
//        //
//        //const std::vector<POINT>& vecGs = cornerDetector.GetCentroids();
//        const POINT* pPtFound = fastCornerDetector.GetResults();
//
//        //定精度图像坐标
//        std::vector<TImageCalibPoint> cornerData;
//        cornerData.resize(nExpectedMarkerNumber);
//
//        for(int i = 0;
//            i < nExpectedMarkerNumber; i++)
//        {
//            cornerData[i].ptImageCoord = pPtFound[i];
//
//        }
//
//
//        //高精度值图像坐标
//        const POINT* pPtHDFound = fastCornerDetector.GetHDResults();
//        for(int i = 0;
//            i < nExpectedMarkerNumber; i++)
//        {
//            cornerData[i].ptHDImgCoord = pPtHDFound[i];
//
//        }
//
//
//        //
//        // x |c11 c12 |
//        // = | |
//        // y |c21 c22 |
//
//        //旋转矩阵系数
//        double c11, c12, c21, c22;
//
//        c11 = m[0][0];
//        c12 = m[0][1];
//
//        c21 = m[1][0];
//        c22 = m[1][1];
//
//
//
//
//        //进行旋转操作
//        for(std::vector<TImageCalibPoint>::size_type i = 0;
//            i < cornerData.size();
//            i++)
//        {
//            const POINT& ptImageCoord = cornerData[i].ptImageCoord;
//            POINT& ptRotatedCoord = cornerData[i].ptRotatedCoord;
//            double dx = ptImageCoord.x - ptCenter.x;
//            double dy = ptImageCoord.y - ptCenter.y;
//            ptRotatedCoord.x = LONG(c11 * dx + c12 * dy);
//            ptRotatedCoord.y = LONG(c21 * dx + c22 * dy);
//
//            ptRotatedCoord.x += ptCenter.x;
//            ptRotatedCoord.y += ptCenter.y;
//        }
//
//
//        for(std::vector<TImageCalibPoint>::size_type i = 0;
//            i < cornerData.size();
//            i++)
//        {
//            m_vecInteriorCalibPoint[m_nInteriorCalibPtNumber++] = cornerData[i];
//        }
//
//        //<<debug
//        if(m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
//        {
//
//            CImageFrame rotateCornersFrame(srcFrame.Width(), srcFrame.Height());
//
//            for(std::vector<TImageCalibPoint>::size_type i = 0;
//                i < cornerData.size();
//                i++)
//            {
//                //<<//<<modified by toxuke@gmail.com, 2013/04/11。
//                POINT ptOutput;
//                ptOutput.x = cornerData[i].ptRotatedCoord.x /*>> INT_SCALE_SHIFT_SIZE*/;
//                ptOutput.y = cornerData[i].ptRotatedCoord.y /*>> INT_SCALE_SHIFT_SIZE*/;
//                //>>
//
//                rotateCornersFrame.SetPixel(ptOutput, 255);
//            }
//
//            CDib dib;
//            LoadToDib(rotateCornersFrame, dib);
//            dib.SetPixel(ptCenter.x/*>>INT_SCALE_SHIFT_SIZE*/, ptCenter.y/*>>INT_SCALE_SHIFT_SIZE*/, RGB(255, 0, 0));
//            CAtlString strFileName;
//            strFileName.Format(_T("corner_point_rotate_adjust_%d.jpg"),nAreaNo);
//            Debug_SaveDibToJPEG(dib, strFileName);
//        }
//        //debug>>
//    }
//
//    return bRet;
//}

    //@功能:收集校正点坐标的函数
void CAutoCalibratorImpl2::CollectCalibrateMarkers(
    const TBlobObject* pObjs,
    int nBlobCount,
    const double rotationMatrix[2][2],//坐标旋转矩阵
    const POINT& ptCenter)
{
    // x |c11 c12 |
    // = | |
    // y |c21 c22 |

    //旋转矩阵系数
    double c11, c12, c21, c22;

    c11 = rotationMatrix[0][0];
    c12 = rotationMatrix[0][1];

    c21 = rotationMatrix[1][0];
    c22 = rotationMatrix[1][1];


    m_vecInteriorCalibPoint.resize(nBlobCount);

    for (int i = 0; i < nBlobCount; i++)
    {
        POINT ptRotatedCoord;//用于匹配的旋转坐标

        double dx = pObjs[i].GPos.x - ptCenter.x;
        double dy = pObjs[i].GPos.y - ptCenter.y;
        ptRotatedCoord.x = LONG(c11 * dx + c12 * dy);
        ptRotatedCoord.y = LONG(c21 * dx + c22 * dy);

        ptRotatedCoord.x += ptCenter.x;
        ptRotatedCoord.y += ptCenter.y;

        m_vecInteriorCalibPoint[i].pt2DImageCoord.d[0] = (double)pObjs[i].my / (double)pObjs[i].mass;
        m_vecInteriorCalibPoint[i].pt2DImageCoord.d[1] = (double)pObjs[i].mx / (double)pObjs[i].mass;
        m_vecInteriorCalibPoint[i].ptRotatedCoord = ptRotatedCoord;

    }


    //<<debug
    if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
    {

        CImageFrame rotateCornersFrame(m_oPatternFrame.Width(), m_oPatternFrame.Height());

        for (std::vector<TImageCalibPoint>::size_type i = 0;
            i < m_vecInteriorCalibPoint.size();
            i++)
        {

            rotateCornersFrame.SetPixel(m_vecInteriorCalibPoint[i].ptRotatedCoord, 255);
        }

        CDib dib;
        LoadToDib(rotateCornersFrame, dib);
        dib.SetPixel(ptCenter.x, ptCenter.y, RGB(255, 0, 0));
        CAtlString strFileName;
        strFileName.Format(_T("calibrate_marker_roated.png"));
        Debug_SaveDibToPNG(dib, strFileName);//保存为PNG,避免颜色失真
    }
    //debug>>
}


//@功能:过滤不合格的多余边界点
//@返回:合格的边界点个数
int CAutoCalibratorImpl2::FilteroutUnqualifiedOutlayerVertex(TVertex* pVertex, int nVertexCount)
{
    int nRealOutlayerVertexCount = 0;

    std::vector<TNeighbourVector> vecNV;//以最外圈层点为原点，其他顶点的方位图
    vecNV.resize(nVertexCount - 1);//不包含最外圈层点自身

    for (int i = 0; i < nVertexCount; i++)
    {//对每个最外圈层点循环
        TVertex& curVertex = pVertex[i];
        if (!curVertex.bIsOuterLayer)
        {
            continue;
        }
        POINT p0 = curVertex.ptCoord;
        int nvFound = 0;

        for (int j = 0; j < nVertexCount; j++)
        {//查看其他点是如何在该点附近分布的
            if (i == j) continue;//不包含最外圈层点自身

            POINT p1 = pVertex[j].ptCoord;

            //第i个校正点的第j个邻接矢量
            TNeighbourVector nv;
            nv.lDx = p1.x - p0.x;
            nv.lDy = p1.y - p0.y;
            nv.nVertexIndex = j;

            if (nv.lDx > 0 && nv.lDy > 0)
            {
                nv.nQaudrant = 0;
            }
            else if (nv.lDx < 0 && nv.lDy > 0)
            {
                nv.nQaudrant = 1;
            }
            else if (nv.lDx == 0 && nv.lDy > 0)
            {
                nv.nQaudrant = 1;
            }
            else if (nv.lDx < 0 && nv.lDy < 0)
            {
                nv.nQaudrant = 2;
            }
            else if (nv.lDx == 0 && nv.lDy < 0)
            {
                nv.nQaudrant = 3;
            }
            else if (nv.lDy == 0 && nv.lDx > 0)
            {
                nv.nQaudrant = 0;
            }
            else if (nv.lDy == 0 && nv.lDx < 0)
            {
                nv.nQaudrant = 2;
            }
            else
            {
                nv.nQaudrant = 3;
            }


            BOOL bInsertPosFound = FALSE;//插入位置找到标志

            int k = 0;

            //采用插入排序算法
            //查找插入位置
            for (k = 0; k < nvFound; k++)
            {

                //比较已有的矢量的角度,使得所有矢量按从小到大的方位角排列
                TNeighbourVector nvtemp = vecNV[k];

                if (nv.nQaudrant < nvtemp.nQaudrant)
                {
                    bInsertPosFound = TRUE;
                    break;
                }
                else if (nv.nQaudrant == nvtemp.nQaudrant)
                {
                    LONG crossProduct = nvtemp.lDx * nv.lDy - nvtemp.lDy*nv.lDx;
                    if (crossProduct < 0)//与原矢量的夹角>0, 则矢量积>0
                    {
                        bInsertPosFound = TRUE;
                        break;
                    }

                }//else

            }//for(k)


             //既有矢量往后挪动一个单元
            if (bInsertPosFound)
            {
                for (int l = nvFound; l > k; l--)
                {
                    vecNV[l] = vecNV[l - 1];
                }
                vecNV[k] = nv;
            }
            else
            {
                vecNV[nvFound] = nv;//插入到最末
            }

            nvFound++;

        }//for(j)


        BOOL bIsRealOutlayerVertex = FALSE;
        //判断像邻两个矢量之间的夹角是否大于90度,夹角范围在180~360度
        // 如果大于90度, 则认为是属于最外圈的校正点
        for (int j = 0; j < nvFound; j++)
        {
            TNeighbourVector v1;
            TNeighbourVector v2;

            if (j == nvFound - 1)
            {
                v1 = vecNV[j];
                v2 = vecNV[0];
            }
            else
            {
                v1 = vecNV[j];
                v2 = vecNV[j + 1];

            }

            //矢量的×积
            //v1×v2=|v1||v2|sin(v1^v2);
            long crossProduct = v1.lDx * v2.lDy - v1.lDy * v2.lDx;

            //矢量的.积
            //V1.V2 = |v1||v2|cos(v1^v2);
            //2017/10/20, 注意类型如果设为long, dotProduct_Sign_Square可能会溢出。
            __int64 dotProduct = v1.lDx * v2.lDx + v1.lDy * v2.lDy;
            __int64 dotProduct_Sign_Square = dotProduct * dotProduct * ((dotProduct > 0) ? 1 : -1);

            long v1_length_square = v1.lDx * v1.lDx + v1.lDy * v1.lDy;
            long v2_length_square = v2.lDx * v2.lDx + v2.lDy * v2.lDy;


            static double cos145 = cos(145.0*M_PI / 180.0);
            static long dotProduct_threshold = long(1000 * cos145);
            static long dotProduct_Sign_Square_Threshold = dotProduct_threshold*dotProduct_threshold*(dotProduct_threshold > 0 ? 1 : -1);
            static long norm_square_product_threshold = 1000 * 1000;

            if (crossProduct < 0)//sin(v1^v2)<0, 说明夹角范围在180~360度之间,属于外圈校正点
            {
                bIsRealOutlayerVertex = TRUE;
                break;
            }
            else if (
                __int64(dotProduct_Sign_Square) * __int64(norm_square_product_threshold)
                <
                __int64(dotProduct_Sign_Square_Threshold) * __int64(v1_length_square)* __int64(v2_length_square))//cos(v1^v2)<0, 说明范围在145~180度之间, 属于外圈校正点
            {
                bIsRealOutlayerVertex = TRUE;
                break;
            }

        }//for(j)w


        if (!bIsRealOutlayerVertex)
        {
            curVertex.bIsOuterLayer = FALSE;
        }
        else
        {
            nRealOutlayerVertexCount++;
        }

    }//for(i)

    return nRealOutlayerVertexCount;
}

//@功能:图像中角点与屏幕中校正点的匹配
//@参数:
// corners, 已知视频图像中的的角点坐标数组
// vecImage2ScreenMap, 每个视频图像中的角点到屏幕校正点编号的映射数组。
BOOL CAutoCalibratorImpl2::DoCornerMatch(const std::vector<TImageCalibPoint>& corners, std::vector<int>& vecImage2ScreenMap)
{
    BOOL bMatchSuccess = TRUE;

    std::vector<TVertex> vecVertex;

    //剩余的校正点的个数
    unsigned int nCornerRemained = corners.size();

    vecVertex.resize(nCornerRemained);
    vecImage2ScreenMap.resize(nCornerRemained);

    for (std::vector<TVertex>::size_type i = 0;
        i < nCornerRemained;
        i++)
    {
        vecVertex[i].ptCoord = corners[i].ptRotatedCoord;
        vecVertex[i].nAryIndex = i;
        vecImage2ScreenMap[i] = -1;
    }


    int nLayer = 0;//第一圈,从最外圈开始算起


    DWORD rgba_colors[6] = {
        0xFF0000FF,
        0xFFFF0000,
        0xFF00FFFF,
        0xFFFF0000,
        0xFFFF00FF,
        0xFFFFFF00
    };

    CRGBAFrame rgbaFrame;
    rgbaFrame.SetSize(this->m_oScreenMaskFrame.Width(), this->m_oScreenMaskFrame.Height());
    rgbaFrame.Clear(0xFF000000);

    int nLayerCount = m_oCalibratePattern.GetLayerCount();
    //按圈、层处理
    for (nLayer = 0; nLayer < nLayerCount; nLayer++)
    {

        const TRectLayer& rectLayer = m_oCalibratePattern.GetLayer(nLayer);

        //层在水平方向, 校正标记的个数
        int LayerMarkerHorzNumber = rectLayer.m_nHorzMarkNumber;

        //层在垂直方向, 校正标记的个数
        int LayerMarkerVertNumber = rectLayer.m_nVertMarkNumber;

        //期望的最外圈校正点个数
        int nExpectedPtNumber = rectLayer.m_vecScreenMarkNo.size();


        //如果是个圈层
        if (LayerMarkerHorzNumber > 1 && LayerMarkerVertNumber > 1)
        {
            //Step 1. 对每个校正点, 找出距离其最近的至多MAX_NEIBHBOUR个邻接点。
            for (std::vector<TVertex>::size_type i = 0;
                i < nCornerRemained;
                i++)
            {
                TVertex& curVertex = vecVertex[i];

                POINT P0 = curVertex.ptCoord;

                //和除自身以外的所有校正点进行比较
                for (std::vector<TVertex>::size_type j = 0;
                    j < nCornerRemained;
                    j++)
                {

                    if (i == j) continue;//不与自己比较

                    const TVertex& cmpVertex = vecVertex[j];
                    //if(!cmpVertex.bValid) continue;

                    POINT P1 = cmpVertex.ptCoord;
                    double distance = (P1.x - P0.x)*(P1.x - P0.x) + (P1.y - P0.y)*(P1.y - P0.y);


                    //与当前找到的最近邻接点比较
                    for (int k = 0; k < _countof(curVertex.aryNeighbours); k++)
                    {

                        //第一次找到第k个最近邻接点
                        if (!curVertex.aryNeighbours[k].bValid)
                        {
                            //更新最小邻接表
                            curVertex.aryNeighbours[k].bValid = TRUE;
                            curVertex.aryNeighbours[k].distance = distance;
                            curVertex.aryNeighbours[k].nIndex = j;
                            curVertex.nNeighbourNumber++;
                            break;//break k loop
                        }
                        else
                        {
                            double distCmp = curVertex.aryNeighbours[k].distance;
                            if (distance < distCmp)//比已找到的最近邻接点的距离还要小，则插入在位置k处。
                            {

                                //前面的单元往后移一个,保证是最小的邻接点
                                for (int l = _countof(curVertex.aryNeighbours) - 1; l > k; l--)
                                {
                                    curVertex.aryNeighbours[l] = curVertex.aryNeighbours[l - 1];
                                }

                                curVertex.aryNeighbours[k].distance = distance;
                                curVertex.aryNeighbours[k].nIndex = j;

                                if (curVertex.nNeighbourNumber < _countof(curVertex.aryNeighbours))
                                {
                                    curVertex.nNeighbourNumber++;
                                }

                                break;//break k loop
                            }//if
                            else
                            {
                                continue;//继续比较
                            }

                        }//else

                    }//for(k)

                }//for(j)

            }//for(i)


            //Step 2. 对每个校正点的几个"最近邻接矢量"数组按矢量的方位角排序
            for (std::vector<TVertex>::size_type i = 0;
                i < nCornerRemained;
                i++)
            {
                TVertex& curVertex = vecVertex[i];

                curVertex.bIsOuterLayer = FALSE;//先清除外圈角点标志

                int candidateNumber = curVertex.nNeighbourNumber;
                POINT p0 = curVertex.ptCoord;
                UINT uQuadrant = 0;

                //第i个校正点的MAX_NEIBHBOUR个"最近邻接矢量"
                TNeighbourVector nvs[MAX_NEIBHBOUR];
                memset(nvs, 0, sizeof(nvs));


                for (int j = 0; j < candidateNumber; j++)
                {
                    if (!curVertex.aryNeighbours[j].bValid) continue;

                    int index = curVertex.aryNeighbours[j].nIndex;//邻接点的编号
                    POINT p1 = vecVertex[index].ptCoord;

                    //第i个校正点的第j个邻接矢量
                    TNeighbourVector nv;
                    nv.lDx = p1.x - p0.x;
                    nv.lDy = p1.y - p0.y;
                    nv.nVertexIndex = index;

                    if (nv.lDx > 0 && nv.lDy > 0)
                    {
                        nv.nQaudrant = 0;
                    }
                    else if (nv.lDx < 0 && nv.lDy > 0)
                    {
                        nv.nQaudrant = 1;
                    }
                    else if (nv.lDx == 0 && nv.lDy > 0)
                    {
                        nv.nQaudrant = 1;
                    }
                    else if (nv.lDx < 0 && nv.lDy < 0)
                    {
                        nv.nQaudrant = 2;
                    }
                    else if (nv.lDx == 0 && nv.lDy < 0)
                    {
                        nv.nQaudrant = 3;
                    }
                    else if (nv.lDy == 0 && nv.lDx > 0)
                    {
                        nv.nQaudrant = 0;
                    }
                    else if (nv.lDy == 0 && nv.lDx < 0)
                    {
                        nv.nQaudrant = 2;
                    }
                    else
                    {
                        nv.nQaudrant = 3;
                    }

                    BOOL bInsertPosFound = FALSE;//插入位置找到标志

                    int k = 0;

                    //采用插入排序算法
                    //查找插入位置
                    for (k = 0; k < j; k++)
                    {

                        //比较已有的矢量的角度,使得所有矢量按从小到大的方位角排列
                        TNeighbourVector nvtemp = nvs[k];

                        if (nv.nQaudrant < nvtemp.nQaudrant)
                        {
                            bInsertPosFound = TRUE;
                            break;
                        }
                        else if (nv.nQaudrant == nvtemp.nQaudrant)
                        {
                            LONG crossProduct = nvtemp.lDx * nv.lDy - nvtemp.lDy*nv.lDx;
                            if (crossProduct < 0)//与原矢量的夹角>0, 则矢量积>0
                            {
                                bInsertPosFound = TRUE;
                                break;
                            }

                        }//else

                    }//for(k)


                    //既有矢量往后挪动一个单元
                    if (bInsertPosFound)
                    {
                        for (int l = j; l > k; l--)
                        {
                            nvs[l] = nvs[l - 1];
                        }
                        nvs[k] = nv;
                    }
                    else
                    {
                        nvs[j] = nv;//插入到最末
                    }
                }//for(j)

                //Step 3.对已按照方位角排列的"最近邻接矢量",判断像邻两个矢量之间的夹角是否大于90度,夹角范围在180~360度
                // 如果大于90度, 则认为是属于最外圈的校正点
                for (int j = 0; j < candidateNumber; j++)
                {
                    TNeighbourVector v1;
                    TNeighbourVector v2;

                    if (j == candidateNumber - 1)
                    {
                        v1 = nvs[j];
                        v2 = nvs[0];
                    }
                    else
                    {
                        v1 = nvs[j];
                        v2 = nvs[j + 1];

                    }

                    //矢量的×积
                    //v1×v2=|v1||v2|sin(v1^v2);
                    long crossProduct = v1.lDx * v2.lDy - v1.lDy * v2.lDx;

                    //矢量的.积
                    //V1.V2 = |v1||v2|cos(v1^v2);
                    //2017/10/20, 注意类型如果设为long, dotProduct_Sign_Square可能会溢出。
                    __int64 dotProduct = v1.lDx * v2.lDx + v1.lDy * v2.lDy;
                    __int64 dotProduct_Sign_Square = dotProduct * dotProduct * ((dotProduct > 0) ? 1 : -1);

                    long v1_length_square = v1.lDx * v1.lDx + v1.lDy * v1.lDy;
                    long v2_length_square = v2.lDx * v2.lDx + v2.lDy * v2.lDy;


                    static double cos145 = cos(145.0*M_PI / 180.0);
                    static long dotProduct_threshold = long(1000 * cos145);
                    static long dotProduct_Sign_Square_Threshold = dotProduct_threshold*dotProduct_threshold*(dotProduct_threshold > 0 ? 1 : -1);
                    static long norm_square_product_threshold = 1000 * 1000;

                    if (crossProduct < 0)//sin(v1^v2)<0, 说明夹角范围在180~360度之间,属于外圈校正点
                    {
                        curVertex.bIsOuterLayer = TRUE;
                        break;
                    }
                    else if (
                        __int64(dotProduct_Sign_Square) * __int64(norm_square_product_threshold)
                        <
                        __int64(dotProduct_Sign_Square_Threshold) * __int64(v1_length_square)* __int64(v2_length_square))//cos(v1^v2)<0, 说明范围在145~180度之间, 属于外圈校正点
                    {
                        curVertex.bIsOuterLayer = TRUE;
                        break;
                    }

                }//for(j)w

            }//for(i)


            //统计找到的最外圈角点的个数
            int nOuterLayerPtNumber = 0;

            for (std::vector<TVertex>::size_type i = 0;
                i < nCornerRemained;
                i++)
            {
                TVertex& curVertex = vecVertex[i];

                if (curVertex.bIsOuterLayer)
                {
                    nOuterLayerPtNumber++;
                }

            }//for_each(i)


            //调试输出找到的一圈校正点
            //<<debug
            if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
            {
                DWORD clr = rgba_colors[nLayer%_countof(rgba_colors)];
                for (std::vector<TVertex>::size_type i = 0;
                    i < nCornerRemained;
                    i++)
                {
                    TVertex& curVertex = vecVertex[i];

                    if (!curVertex.bIsOuterLayer) continue;
                    POINT pt = curVertex.ptCoord;
                    rgbaFrame.SetPixel(pt.x, pt.y, clr);
                }

            }
            //debug>>

            //圈检测失败
            if (nOuterLayerPtNumber != nExpectedPtNumber)
            {
                bool bBreakAsError = TRUE;
                if (nOuterLayerPtNumber > nExpectedPtNumber)
                {
                    //进一步过滤到多余的最外圈层点
                    nOuterLayerPtNumber = FilteroutUnqualifiedOutlayerVertex(&vecVertex[0], vecVertex.size());
                    if (nOuterLayerPtNumber == nExpectedPtNumber)
                    {
                        bBreakAsError = FALSE;
                    }
                }

                if (bBreakAsError)
                {
                    Debug_SaveRGBAToPNG(rgbaFrame.GetData(), rgbaFrame.Width(), rgbaFrame.Height(), _T("LayerVertexNumErr.png"));
                    bMatchSuccess = FALSE;
                    break;
                }

            }

            //提取最外圈校正点到单独的一个数组中,减少后续处理的循环次数
            std::vector<TVertex> outlayer;
            outlayer.resize(nOuterLayerPtNumber);


            //校正点在"所有校正点的集合"中的索引号到"最外圈校正点集合"的索引映射
            //std::vector<int> mapForLayerNeighbourIndex;
            //mapForLayerNeighbourIndex.resize(nCornerRemained);

            //for(unsigned int i=0; i < nCornerRemained; i++)
            //{
            //    mapForLayerNeighbourIndex[i] = -1;//-1表示不存在映射对应关系
            //}

            int nOutLayerPtIndex = 0;
            for (std::vector<TVertex>::size_type i = 0;
                i < nCornerRemained;
                i++)
            {
                TVertex& curVertex = vecVertex[i];

                if (curVertex.bIsOuterLayer)
                {
                    outlayer[nOutLayerPtIndex] = curVertex;

                    //mapForLayerNeighbourIndex[i] = nOutLayerPtIndex;
                    nOutLayerPtIndex++;
                }

            }//for_each(i)


            BOOL bLayerMatchSuccess = FALSE;

            CVertexSort<TVertex>  vertexSorter;


            if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
            {
                CImageFrame dbgFrame;
                BYTE value = 0x10;
                dbgFrame.SetSize(m_oPatternFrame.Width(), m_oPatternFrame.Height(), 1, &value);

                for (size_t i = 0; i < outlayer.size(); i++)
                {
                    int nNo = (int)i;
                    POINT pt = outlayer[i].ptCoord;

                    dbgFrame.SetPixel(pt, 0xFF);

                    pt.x += 2;
                    pt.y += 2;

                    char szText[32];
                    sprintf_s(szText, sizeof(szText), "%d", nNo);


                    dbgFrame.PutStr(pt.x, pt.y, szText, 0x7E, 7/*font size*/);
                }

                Debug_SaveImageFrame(dbgFrame, _T("LayerVertexBeforeSort.jpg"));
            }



            //排列圈层上的点，使之成为一个四边形
            if (!vertexSorter(&outlayer[0], outlayer.size(), true))
            {
                bMatchSuccess = FALSE;
                break;
            }



            if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
            {
                CImageFrame dbgFrame;
                BYTE value = 0x10;
                dbgFrame.SetSize(m_oPatternFrame.Width(), m_oPatternFrame.Height(), 1, &value);

                for (size_t i = 0; i < outlayer.size(); i++)
                {
                    int nNo = (int)i;
                    POINT pt = outlayer[i].ptCoord;

                    dbgFrame.SetPixel(pt, 0xFF);

                    pt.x += 2;
                    pt.y += 2;

                    char szText[32];
                    sprintf_s(szText, sizeof(szText), "%d", nNo);


                    dbgFrame.PutStr(pt.x, pt.y, szText, 0x7E, 7/*font size*/);
                }

                Debug_SaveImageFrame(dbgFrame, _T("LayerVertexAfterSort.jpg"));
            }



            //外圈校正点已经按方位有序排列了,现在需要从一圈校正点中找出四个角上的点。
            TurnPoint  turns[4];

            int nTurnPointCount = 0;

            nTurnPointCount = SearchTurnPoint(&outlayer[0], outlayer.size(), &turns[0], _countof(turns));

            if (nTurnPointCount != 4)
            {
                bMatchSuccess = FALSE;
                break;
            }


            //int iTopLeftCornerPointIndex = MatchLeftTopCorner(turns, ptG, _countof(turns));
            int iTopLeftCornerPointIndex = MatchLeftTopCorner(turns, _countof(turns));


            if (iTopLeftCornerPointIndex == -1)
            {
                bMatchSuccess = FALSE;
                break;
            }

            int iStartMatchIndex = 0;


            iStartMatchIndex = turns[iTopLeftCornerPointIndex].nLayerPtIndex;


            unsigned int nMatchCount = 0;
            int iMatchIndex = iStartMatchIndex;



            while (nMatchCount < outlayer.size())
            {
                const TVertex& v = outlayer[iMatchIndex];

                vecImage2ScreenMap[v.nAryIndex] = rectLayer.m_vecScreenMarkNo[nMatchCount];//subarea.m_aryMarkScreenGlobalNo[nScreenCheckerIndex];

                iMatchIndex++;

                if (iMatchIndex == outlayer.size())
                {
                    iMatchIndex = 0;
                }

                nMatchCount++;
            }


            //剔除最外圈点为下一圈处理做准备
            std::vector<TVertex>::iterator it = vecVertex.begin();

            while (it != vecVertex.end())
            {
                if (it->bIsOuterLayer)
                {
                    it = vecVertex.erase(it);
                    //it->bValid = FALSE;
                    nCornerRemained--;
                }
                else
                {
                    it->nNeighbourNumber = 0;
                    memset(it->aryNeighbours, 0, sizeof(it->aryNeighbours));
                    it->bIsOuterLayer = FALSE;
                    it->bIsTurnPoint = FALSE;
                    it++;
                }
            }
        }//if is circle layer


        else  if(LayerMarkerHorzNumber == 1)//整个层为一列
        {
            if (vecVertex.size() != nExpectedPtNumber)
            {

                bMatchSuccess = FALSE;
                break;
            }


            //按照从Y坐标小到大的顺序匹配
            //vecImage2ScreenMap
            std::vector<TVertex>::iterator it, itMatch;

            unsigned int nMatchCount = 0;

            do
            {
                itMatch = vecVertex.begin();
                for (it = itMatch + 1; it != vecVertex.end(); it++)
                {
                    if (itMatch->ptCoord.y > it->ptCoord.y)
                    {
                        itMatch = it;
                    }
                }
                vecImage2ScreenMap[itMatch->nAryIndex] = rectLayer.m_vecScreenMarkNo[nMatchCount++];

                vecVertex.erase(itMatch);

            } while (vecVertex.size());

            bMatchSuccess = TRUE;

        }
        else if (LayerMarkerVertNumber == 1)//整个层为一行
        {
            if (vecVertex.size() != nExpectedPtNumber)
            {
                bMatchSuccess = FALSE;
                break;
            }


            ////按照从X坐标小到大的顺序匹配
            //vecImage2ScreenMap
            std::vector<TVertex>::iterator it, itMatch;

            unsigned int nMatchCount = 0;

            do
            {
                itMatch = vecVertex.begin();
                for (it = itMatch + 1; it != vecVertex.end(); it++)
                {
                    if (itMatch->ptCoord.x > it->ptCoord.x)
                    {
                        itMatch = it;
                    }
                }
                vecImage2ScreenMap[itMatch->nAryIndex] = rectLayer.m_vecScreenMarkNo[nMatchCount++];

                vecVertex.erase(itMatch);

            } while (vecVertex.size());

            bMatchSuccess = TRUE;
        }
    }//for_each(rectLayer)


    if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
    {

        for (size_t I = 0; I < vecImage2ScreenMap.size(); I++)
        {
            int nNo = vecImage2ScreenMap[I];
            POINT pt = corners[I].ptRotatedCoord;
            pt.x += 2;
            pt.y += 2;

            char szText[32];
            sprintf_s(szText, sizeof(szText), "%d", nNo);

            rgbaFrame.PutStr(pt.x, pt.y, szText, 0xFFFFFFFF, 7/*font size*/);
        }

        Debug_SaveRGBAToPNG(rgbaFrame.GetData(), rgbaFrame.Width(), rgbaFrame.Height(), _T("LayerMatch.png"));
    }


    return bMatchSuccess;
}



//@功能:模拟校正
//@参数:lpszAVIFilePath， 录像文件路径
//      hNotifyWnd, 消息通知窗体句柄
//      uCx, 屏幕像素宽度
//      uCy, 屏幕像素高度
//     cBrightness, 设定的画面平均亮度
//     eGridsNumber, 校正图案
BOOL CAutoCalibratorImpl2::DoSimulateCalibrate(LPCTSTR lpszAVIFilePath, HWND hNotifyWnd, UINT uCx, UINT uCy, BYTE cBrightness, E_AutoCalibratePattern eGridsNumber)
{

    if (this->m_bIsWorking)
    {
        this->EndCalibrating();
    }

    //if(!m_oAVIInput.OpenFile(lpszAVIFilePath))
    if (!m_oAVIInput.open(CT2A(lpszAVIFilePath)))
    {
        return FALSE;
    }


    //设置自动亮度调节时期望的画面平均亮度
    m_AutoBrightnessRegulator.Reset();
    m_AutoBrightnessRegulator.SetExpectedBrightness(cBrightness);
    m_AutoBrightnessRegulator.SetFeedbackCtrlFunction(NULL);

    m_bTestAutoBrightnessCtrlMode = FALSE;

    this->m_hNotifyWnd = hNotifyWnd;

    //SQUARE_SIZE  = INITIAL_SQUARE_SIZE;

    //m_uCx = uCx;
    //m_uCy = uCy;

    this->m_CurrentMonitorInfo.rcArea.left = 0;
    this->m_CurrentMonitorInfo.rcArea.top = 0;
    this->m_CurrentMonitorInfo.rcArea.right = uCx;
    this->m_CurrentMonitorInfo.rcArea.bottom = uCy;


    m_vecScreenInfos.resize(1);
    m_vecScreenInfos[0] = m_CurrentMonitorInfo;

    m_vecMonitorCalibResults.resize(1);


    m_nDispMonitorId = 0;


    this->m_uMonitorWidth = uCx;
    this->m_uMonitorHeight = uCy;

    SYSTEMTIME now;
    GetLocalTime(&now);
    m_strCurrentImageOutputDir.Format(
        _T("%s\\AutoCalibrate\\%04d_%02d_%02d_%02d_%02d_%02d"),
        g_strIntermediatRootDirName,
        now.wYear,
        now.wMonth,
        now.wDay,
        now.wHour,
        now.wMinute,
        now.wSecond);


    m_nTryTimes = 1;

    //设置图片输出目录
    CString strImageDubugDir;
    strImageDubugDir.Format(_T("%s\\%d"), m_strCurrentImageOutputDir, m_nTryTimes);
    SetImageDebugDir(strImageDubugDir);


    //m_eMonochromizeAlog = E_MONOCHROMIZE_ALOG_Y;//黑白化算法
    m_eCalibrateStage = E_AUTO_CALIBRATE_START;

    //m_nStageWaitCount = 0;
    m_oWaitTimer.Reset();

    m_bIsSimulatedCalibrating = TRUE;
    m_bIsWorking = TRUE;

    this->m_ePattern = eGridsNumber;

    //int Radius = m_oCalibratePattern.CalculateCalibPatternRadius(this->m_ePattern, uCx);
    //m_oCalibratePattern.InitPattern(Radius, this->m_CurrentMonitorInfo.rcArea);

    TCHAR szSmulateParamsFileName[MAX_PATH];
    _tcscpy_s(szSmulateParamsFileName, _countof(szSmulateParamsFileName), lpszAVIFilePath);


    //PathCchRemoveFileSpec(szSmulateParamsFileName, _countof(szSmulateParamsFileName));
    PathRemoveFileSpec(szSmulateParamsFileName);

    //PathCchAppend(szSmulateParamsFileName, _countof(szSmulateParamsFileName), _T("simulate-params.txt"));
    PathAppend(szSmulateParamsFileName, _T("simulate-params.txt"));

    DbgLoadSimulateParams(szSmulateParamsFileName);


    //每个屏幕的校正尝试次数
    m_nTryTimes = 0;

    if (m_oautocalibrateparamslist.size() > 0)
    {
        //设置自动校正时间放大倍数
        const AutoCalibrateImageParams& autocalibrateParams = m_oautocalibrateparamslist[0].autoCalibrateImageParams;
        InitWaitTimer(autocalibrateParams.autoCalibrateSpeed, autocalibrateParams.videoDislayDelay);


        //最大尝试次数，要能够覆盖所有的画面参数
        if (this->m_nMaxTryTimes < (int)this->m_oautocalibrateparamslist.size())
        {
            this->m_nMaxTryTimes = (int)this->m_oautocalibrateparamslist.size();
        }

        if (m_bEnableOnlineScreenArea)
        {
            m_AutoBrightnessRegulator.SetExpectedBrightness((m_oautocalibrateparamslist[0].autoCalibrateImageParams.autoCalibrateExpectedBrightness) >> 1);
        }
        else
        {
            m_AutoBrightnessRegulator.SetExpectedBrightness(m_oautocalibrateparamslist[0].autoCalibrateImageParams.autoCalibrateExpectedBrightness);
        }
    }
    else
    {
        //设置自动校正时间放大倍数
        InitWaitTimer(1, 0);
    }

    m_nDbgFrameCount = 0;

    DWORD dwThreadId = 0;
    m_hSimulatedCalibrateThread = ::CreateThread(
        NULL,
        0,
        SimulatedCalibrateProc,
        (LPVOID)this,
        0,
        &dwThreadId);


    //<<added by toxuke@gmail.com, 2015/01/13
    this->m_eWorkMode = E_WORK_MODE_AUTO_CALIBRATE;//自动校正而不是自动屏蔽
    //>>

    return TRUE;

}



//调试函数
//@功能:测试亮度自动调节功能
void CAutoCalibratorImpl2::TestAutoBrightnessControl(CHANGECAMERAPARAMSPROC ChangeCameraParamsProc, LPVOID lpCtx)
{

    //生成校正窗口,以便接收画面信息
    if (HWND(m_AutoCalibrateWnd) == NULL)
    {
        RECT rcPosition;
        rcPosition.left = 0;
        rcPosition.top = 0;
        rcPosition.right = GetSystemMetrics(SM_CXSCREEN) - 1;
        rcPosition.bottom = GetSystemMetrics(SM_CYSCREEN) - 1;

        if (!(m_AutoCalibrateWnd.CreateWindow)(rcPosition, FALSE))
        {
            return;
        }
    }

    this->m_fpChangeCameraParams = ChangeCameraParamsProc;
    this->m_lpCtx = lpCtx;

    m_eCalibrateStage = E_AUTO_CALIBRATE_START;
    m_bTestAutoBrightnessCtrlMode = TRUE;
}


//@功能:查看校正图案    
void CAutoCalibratorImpl2::ViewCalibratePattern()
{
    //在主显示器上显示校正图案
    if (HWND(m_AutoCalibrateWnd) == NULL)
    {
        RECT rcPosition;
        rcPosition.left = 0;
        rcPosition.top = 0;
        rcPosition.right = GetSystemMetrics(SM_CXSCREEN) - 1;
        rcPosition.bottom = GetSystemMetrics(SM_CYSCREEN) - 1;

        if (!(m_AutoCalibrateWnd.CreateWindow)(rcPosition))
        {
            return;
        }
    }

    m_AutoCalibrateWnd.ShowWindow(SW_SHOW);
    this->m_oCalibratePattern.DrawPattern(m_AutoCalibrateWnd);
}


//============================
//自动屏蔽相关函数




//@功能:开始自动屏蔽,供外部模块调用
BOOL CAutoCalibratorImpl2::StartMasking(const TAutoMaskingParams& autoMaskingParams, const TStaticMaskingParams& staticMaskingParams, int nSrcImageWidth, int nSrcImageHeight)
{
    //调试级别
    m_eDebugLevel = autoMaskingParams.eDebugLevel;

    m_vecScreenInfos = autoMaskingParams.vecScreenInfos;

    //搜索机器上的显示器
    // m_oMonitorFinder.SearchMonitor();

    //int nMonitorCount = m_oMonitorFinder.GetMonitorCount();
    int nMonitorCount = m_vecScreenInfos.size();
    if (nMonitorCount == 0)
    {
        return FALSE;
    }

    m_oScreenMap.Clear();

    m_vecMonitorCalibResults.resize(nMonitorCount);

    m_nDispMonitorId = 0;

    //m_CurrentMonitorInfo = *m_oMonitorFinder.GetMointorInfo(m_nDispMonitorId);
    m_CurrentMonitorInfo = m_vecScreenInfos[m_nDispMonitorId];




    m_bSaveInermediatFile = autoMaskingParams.bSaveInermediatFile;
    m_bRecordVideo = autoMaskingParams.bRecordVideo;
    m_fpChangeCameraParams = autoMaskingParams.ChangeCameraParamsProc;
    m_lpCtx = autoMaskingParams.lpCtx;


    //    m_oImageParamsList     = autoMaskingParams.imageParamsList;

    m_oautocalibrateparamslist = autoMaskingParams.autocalibrateparamslist;

    m_AutoBrightnessRegulator.Reset();//自动亮度控制复位

    //设置自动亮度调节时期望的画面平均亮度
    //m_AutoBrightnessRegulator.SetExpectedBrightness(autoCalibrateParams.cAvgBrightness);
    m_AutoBrightnessRegulator.SetExpectedBrightness(m_oautocalibrateparamslist[0].autoCalibrateImageParams.autoCalibrateExpectedBrightness);

    m_bTestAutoBrightnessCtrlMode = FALSE; //置非自动亮度调节测试模式

    if (m_bIsWorking)
    {
        return FALSE;
    }

    RECT rcVirtualScreen;

    rcVirtualScreen.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    rcVirtualScreen.top = GetSystemMetrics(SM_YVIRTUALSCREEN);

    rcVirtualScreen.right = rcVirtualScreen.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    rcVirtualScreen.bottom = rcVirtualScreen.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);


    //生成校正窗体，尺寸为包含所有屏幕的虚拟屏幕的尺寸
    if (HWND(m_AutoCalibrateWnd) == NULL)
    {
        if (!(m_AutoCalibrateWnd.CreateWindow)(rcVirtualScreen))
        {
            return FALSE;
        }
    }
    else
    {   //在更改分屏幕辨率时，需要调整校正窗体的尺寸
        m_AutoCalibrateWnd.MoveWindow(rcVirtualScreen);
    }

    m_hNotifyWnd = autoMaskingParams.hNotifyWnd;
    m_AutoCalibrateWnd.ShowWindow(SW_SHOW);
    m_AutoCalibrateWnd.SetFocus();


    // m_eMonochromizeAlog = E_MONOCHROMIZE_ALOG_Y;//黑白化算法
    m_eCalibrateStage = E_AUTO_CALIBRATE_START;
    //m_nStageWaitCount = 0;
    m_oWaitTimer.Reset();

    m_bIsWorking = TRUE;
    m_bIsSimulatedCalibrating = FALSE;



    //InitAutoCalibrateSaveDirectory();
    SYSTEMTIME now;
    GetLocalTime(&now);


    //准备调试输出路径
    m_strImageOutputBaseDir.Format(
        _T("%s\\AutoMask\\%04d_%02d_%02d_%02d_%02d_%02d"),
        g_strIntermediatRootDirName,
        now.wYear,
        now.wMonth,
        now.wDay,
        now.wHour,
        now.wMinute,
        now.wSecond);

    m_strCurrentImageOutputDir.Format(
        _T("%s\\Monitor_%d"),
        m_strImageOutputBaseDir,
        m_nDispMonitorId + 1);


    //m_uCx, m_uCy指向当前监视器的屏幕尺寸
    m_uMonitorWidth = m_CurrentMonitorInfo.rcArea.right - m_CurrentMonitorInfo.rcArea.left;
    m_uMonitorHeight = m_CurrentMonitorInfo.rcArea.bottom - m_CurrentMonitorInfo.rcArea.top;


    CAtlString strFileName;
    SYSTEMTIME localTime;
    GetLocalTime(&localTime);


    m_strDebugVideoFullPath.Format(_T("%s\\AutoMask_Sceen(%d)_%04d%02d%02d-%02d%02d%02d%03d_(%dX%d_%u).avi"),
        m_strCurrentImageOutputDir,
        m_nDispMonitorId + 1,
        localTime.wYear,
        localTime.wMonth,
        localTime.wDay,
        localTime.wHour,
        localTime.wMinute,
        localTime.wSecond,
        localTime.wMilliseconds,
        m_uMonitorWidth,
        m_uMonitorHeight,
        //autoCalibrateParams.cAvgBrightness);
        m_AutoBrightnessRegulator.GetExpectedBrightness());


    //隐藏鼠标光标
    ShowCursor(FALSE);


    ////每个屏幕的校正尝试次数
    //m_nTryTimes = 0;

    ////最大尝试次数，要能够覆盖所有的画面参数
    //if(this->m_nMaxTryTimes < (int)this->m_oImageParamsList.size())
    //{
    //    this->m_nMaxTryTimes = (int)this->m_oImageParamsList.size();
    //}

    //设置图片输出目录
    CString strImageDubugDir;
    strImageDubugDir.Format(_T("%s\\%d"), m_strCurrentImageOutputDir, m_nTryTimes + 1);
    SetImageDebugDir(strImageDubugDir);

    if (m_bRecordVideo)//需要保存校正视频
    {
        //不再在"校正点自动采集模块"中录像,改为在外部的视频源处录制.
        //m_oAVIRecorder.OpenFile(strFileName, 640, 480);
    }


    m_bUseOldMethod = FALSE;

    //<<2013/03/11,by toxuke@gmail.com 重置自动校正窗体超时检测状态
    m_AutoCalibrateWnd.StartTimeoutDetect();
    //>>


    //<<2014/12/09, 静态屏蔽参数
    this->m_tStaticMaskingParams = staticMaskingParams;
    //>>



    m_eAutoMaskingError = E_AUTO_MASKING_ERROR_UNDEF;


    //m_bDoStaticMaskingOnly = autoMaskingParams.bDoStaticMaskingOnly;

    if (autoMaskingParams.bDoStaticMaskingOnly)//只作静态屏蔽
    {

        BYTE value = 0xFF;
        m_oScreenMaskFrame.SetSize(nSrcImageWidth, nSrcImageHeight, 1, &value);

        //初始各屏幕屏蔽图为全屏有效
        for (int i = 0; i < nMonitorCount; i++)
        {
            m_vecMonitorCalibResults[i].maskFrame.SetSize(nSrcImageWidth, nSrcImageHeight, 1, &value);
        }


        m_eAutoMaskStage = E_AUTO_MASK_STATIC_START;//直接进入静态屏蔽
    }
    else
    {
        m_eAutoMaskStage = E_AUTO_MASK_START;//进入自动屏蔽阶段
    }

    if (IsLanguageBarVisible())
    {
        ShowLanguageBar(FALSE);
        m_bRestoreLanguageBar = TRUE;
    }
    m_eWorkMode = E_WORK_MODEL_AUTO_MASKING;
    return TRUE;
}




//@功能:给自动屏蔽状态机投喂图片。
//@返回值:异常返回FALSE
BOOL CAutoCalibratorImpl2::FeedImage_AutoMask(const CImageFrame* pGrayFrame)
{
    if (pGrayFrame == NULL) return FALSE;

    if (FALSE == this->m_bIsSimulatedCalibrating)
    {
        //if(m_bRecordVideo)//根据设置保存视频
        //{
        //    m_oAVIRecorder.Write((const BYTE*)pGrayFrame->GetData(), pGrayFrame->Size());
        //}
    }

    CImageFrame monoFrame = *pGrayFrame;
    //Covert2MonochromeFrame_Y(*pYUVFrame, monoFrame);


    //if(this->m_bIsSimulatedCalibrating)
    //{
    //    m_oDebugWindow.PrintFrame(monoFrame);
    //}


    //BOOL bFailed = FALSE;

    //RECT rc;
    BOOL bRet = FALSE;

    m_oWaitTimer.Trigger();
    switch (m_eAutoMaskStage)
    {
    case E_AUTO_MASK_START://开始校正

        if (FALSE == this->m_bIsSimulatedCalibrating)//非仿真模式
        {
            if (m_fpChangeCameraParams)
            {
                //对比度调节到最大, 抑制噪声
                // m_fpChangeCameraParams(E_CAMERA_CONTRAST_MAX, m_lpCtx, 0);

                //对比度调节到最大值的2/3, 避免取最大对比度时, 对屏幕四个角处的校正图案的亮度抑制。
                //<<2014/04/21
                m_fpChangeCameraParams(E_CAMERA_CONTRAST, m_lpCtx, 170, 0);
                //2014/04/21>>
            }
        }

        //显示全屏白色
        if (FALSE == this->m_bIsSimulatedCalibrating)//非仿真模式
        {
            FillBoard(m_AutoCalibrateWnd, FOREGROUND_COLOR, &m_CurrentMonitorInfo.rcArea);
        }

        //m_nStageWaitCount = 0;
        m_oWaitTimer.Reset();

        m_eAutoMaskStage = E_AUTO_MASK_ADJUST_BRIGHTNESS;

        break;


    case E_AUTO_MASK_ADJUST_BRIGHTNESS://自动调节画面亮度
    {
        //m_nStageWaitCount ++;

        //自动亮度调节
        BYTE brightnessDiff = m_AutoBrightnessRegulator.ProcessImage(monoFrame.GetData(), monoFrame.Width(), monoFrame.Height());
        //if(m_nStageWaitCount < 1*60)//至少调节1秒钟。
        if (m_oWaitTimer.IsWaitTimeout(1000))//至少调节1秒钟
        {
            break;
        }

        //if(brightnessDiff > 10 && m_nStageWaitCount < 5*60)//最大调节时间是5分钟
        if (brightnessDiff > 10 && !m_oWaitTimer.IsWaitTimeout(2000))//最大调节时间是1分钟
        {
            break;
        }

        m_oMonitorAreaLocator.Reset(monoFrame.Width(), monoFrame.Height(), m_AutoCalibrateWnd, m_CurrentMonitorInfo.rcArea, this->m_eDebugLevel);

        //m_nStageWaitCount = 0;
        m_oWaitTimer.Reset();
        m_eAutoMaskStage = E_AUTO_MASK_SEARCH_SCREEN_AREA;
    }
    break;


    case E_AUTO_MASK_SEARCH_SCREEN_AREA://
        bRet = m_oMonitorAreaLocator.Process(monoFrame);
        if (!bRet)
        {
            //自动屏蔽失败
            m_eAutoMaskingError = E_AUTO_MASKING_ERROR_DETECT_DISPLAY_AREA_FAILED;
            m_eAutoMaskStage = E_AUTO_MASK_END;
            break;
        }
        else
        {
            if (m_oMonitorAreaLocator.IsSuccess())
            {

                m_oInitialScreenMask = m_oMonitorAreaLocator.GetMaskFrame();

                //屏幕区块膨胀10个像素
                for (int i = 0; i < 10; i++)
                {
                    Morph_Dilate8(m_oInitialScreenMask.GetData(), m_oInitialScreenMask.GetData(), m_oInitialScreenMask.Width(), m_oInitialScreenMask.Height());
                }

                if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
                {
                    Debug_SaveImageFrame(m_oInitialScreenMask, _T("InitialScreenMaskDilated.jpg"));
                }


                //根据边界点划分屏幕区域
                PartitionScreenArea();


                //重置屏幕搜索子状态机,当前校正屏幕背景变红色。
                //ResetSubStateMachine_SearachScreenArea(monoFrame);
                m_oMonitorBoundaryFinder.Reset(m_oInitialScreenMask, m_AutoCalibrateWnd, m_CurrentMonitorInfo.rcArea, this->m_eDebugLevel);

                //设置子区域多边形
                m_oMonitorBoundaryFinder.SetSubAreaPolygons(&this->m_vecPolygons[0], 8);

                //m_nStageWaitCount = 0;
                m_oWaitTimer.Reset();
                m_eAutoMaskStage = E_AUTO_MASK_SEARCH_SCREEN_IMAGE_BOUNDARY;

            }
        }

        break;


    case E_AUTO_MASK_SEARCH_SCREEN_IMAGE_BOUNDARY:

        //屏幕影像边界查找
        bRet = this->m_oMonitorBoundaryFinder.Process(monoFrame);

        if (!bRet)
        {//精确查找屏幕影像轮廓失败

            m_oScreenMaskFrame = this->m_oInitialScreenMask;

            //显示黑屏
            if (FALSE == this->m_bIsSimulatedCalibrating)
            {
                FillBoard(m_AutoCalibrateWnd, BACKGROUND_COLOR, &m_CurrentMonitorInfo.rcArea);
            }


            if (FALSE == this->m_bIsSimulatedCalibrating)//非仿真模式
            {
                if (m_fpChangeCameraParams)
                {
                    //对比度调节到缺省值,为的是看清校正图案
                    //m_fpChangeCameraParams(E_CAMERA_CONTRAST_DEFAULT, m_lpCtx, 0);
                    int nIndex = m_nTryTimes % this->m_oautocalibrateparamslist.size();
                    m_fpChangeCameraParams(E_CAMERA_AUTO_CALIBRATE, m_lpCtx, 0, nIndex);
                }
            }

            //m_nStageWaitCount = 0;
            m_oWaitTimer.Reset();

            //转入校正棋盘格上部阶段
            m_eAutoMaskStage = E_AUTO_MASK_SEARCH_BACKGROUND;

            //使用旧方法判断旋转方向
            m_bUseOldMethod = TRUE;


        }
        else
        {
            if (!this->m_oMonitorBoundaryFinder.IsDone()) break;

            //m_ptUpperCentroid = this->m_oMonitorBoundaryFinder.GetUpperPartCentroid();
            //m_ptLowerCentroid = this->m_oMonitorBoundaryFinder.GetLowerPartCentroid();
            //m_vecUpperBorderCalibratePts = this->m_oMonitorBoundaryFinder.GetUpperBorderCalibratePts();
            //m_vecLowerBorderCalibratePts = this->m_oMonitorBoundaryFinder.GetLowerBorderCalibratePts();


            //m_oUpperHalfMaskFrame        = this->m_oMonitorBoundaryFinder.GetUpperHalfMaskFrame();
            //m_oLowerHalfMaskFrame        = this->m_oMonitorBoundaryFinder.GetLowerHalfMaskFrame();


            //屏幕边界搜索后续处理
            BOOL bRet = OnPostSearchScreenBoundary(monoFrame.Width(), monoFrame.Height());

            if (bRet)
            {//精确搜索屏幕区域成功

                if (FALSE == this->m_bIsSimulatedCalibrating)
                {
                    FillBoard(m_AutoCalibrateWnd, 0, BACKGROUND_COLOR);
                }

                //if(FALSE == this->m_bIsSimulatedCalibrating)//非仿真模式
                //{
                //    if(m_fpChangeCameraParams)
                //    { 
                //        //对比度调节到缺省值,为的是看清校正图案
                //        m_fpChangeCameraParams(E_CAMERA_CONTRAST_DEFAULT, m_lpCtx, 0);
                //    }
                //}

                //静态屏蔽前的预处理, 通过回调函数打开激光器
                if (this->m_tStaticMaskingParams.fpPreStaticMaskingProc)
                {
                    (*this->m_tStaticMaskingParams.fpPreStaticMaskingProc)(this->m_tStaticMaskingParams.lpPreStaticMaskingCtx);
                }

                //m_nStageWaitCount = 0;
                m_oWaitTimer.Reset();

                //转入静态屏蔽阶段
                m_eAutoMaskStage = E_AUTO_MASK_STATIC_MASKING;
            }
            else
            {//精确搜索屏幕区域失败
                m_oScreenMaskFrame = this->m_oInitialScreenMask;

                //显示黑屏
                if (FALSE == this->m_bIsSimulatedCalibrating)
                {
                    FillBoard(m_AutoCalibrateWnd, BACKGROUND_COLOR, &m_CurrentMonitorInfo.rcArea);
                }


                if (FALSE == this->m_bIsSimulatedCalibrating)//非仿真模式
                {
                    if (m_fpChangeCameraParams)
                    {
                        int nIndex = m_nTryTimes % this->m_oautocalibrateparamslist.size();
                        m_fpChangeCameraParams(E_CAMERA_AUTO_CALIBRATE, m_lpCtx, 0, nIndex);
                    }
                }

                //m_nStageWaitCount = 0;
                m_oWaitTimer.Reset();

                //转入校正棋盘格上部阶段
                m_eAutoMaskStage = E_AUTO_MASK_SEARCH_BACKGROUND;

                //使用旧方法判断旋转方向
                m_bUseOldMethod = TRUE;
            }
        }

        break;

    case E_AUTO_MASK_SEARCH_BACKGROUND://搜索背景
        //自动亮度调节
        //if(!this->m_bIsSimulatedCalibrating)
        //m_AutoBrightnessRegulator.ProcessImage(monoFrame.GetData(), monoFrame.Width(), monoFrame.Height());

        //m_nStageWaitCount++;
        //if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)
        if (m_oWaitTimer.IsWaitTimeout(MAX_NEED_WAIT_TIME))
        {
            m_oBackgroundFrame = monoFrame;

            if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
            {
                Debug_SaveImageFrame(m_oBackgroundFrame, _T("Background.jpg"));
            }

            {
                RECT rcUpperArea = this->m_CurrentMonitorInfo.rcArea;
                rcUpperArea.bottom = rcUpperArea.top + (rcUpperArea.bottom - rcUpperArea.top) / 2;

                if (FALSE == this->m_bIsSimulatedCalibrating)
                {
                    //显示上半部白屏
                    FillBoard(m_AutoCalibrateWnd, FOREGROUND_COLOR, &rcUpperArea);
                }
            }

            //m_nStageWaitCount = 0;
            m_oWaitTimer.Reset();

            m_eAutoMaskStage = E_AUTO_MASK_SERACH_UPPER_HALF_MASK;
        }

        break;


    case E_AUTO_MASK_SERACH_UPPER_HALF_MASK://侦测旋转方向第一步, 侦测上半部高亮区域重心
        //m_nStageWaitCount++;

        //if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)
        if (m_oWaitTimer.IsWaitTimeout(MAX_NEED_WAIT_TIME))
        {
            monoFrame -= m_oBackgroundFrame;

            //调试输出
            if (this->m_bIsSimulatedCalibrating)
            {
                m_oDebugWindow.PrintFrame(monoFrame);
            }


            //二值化灰度图图片
            //统计直方图
            int hist[256];
            memset(hist, 0, sizeof(hist));
            int nLoopCount = monoFrame.GetPixelCount();;
            const BYTE* pPixel = monoFrame.GetData();
            while (nLoopCount)
            {
                hist[*pPixel] ++;
                pPixel++;
                nLoopCount--;
            }

            //ostu法查找最优门限
            BYTE threshold = FindThreshold_Ostu(hist);
            threshold = (BYTE)((int)threshold * 85 / 100);//门限降低至85%

            monoFrame.Binarize(threshold);



            //在二值图中提取面积最大的高亮区域。
            RECT rcBoundary;
            int nPixelCount = 0;
            POINT ptCenter;

            //计算白块的重心
            BOOL bRet = ExtractTheLargestObject(monoFrame, &rcBoundary, &nPixelCount, &ptCenter);


            if (!bRet)
            {
                //自动屏蔽失败
                m_eAutoMaskingError = E_AUTO_MASKING_ERROR_DETECT_DISPLAY_AREA_FAILED;
                m_eAutoMaskStage = E_AUTO_MASK_END;
                break;

            }
            else
            {

                //上半部屏蔽图
                m_oUpperHalfMaskFrame = monoFrame;

                if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
                {
                    Debug_SaveImageFrame(monoFrame, _T("Rotation_01.jpg"));
                }
                //调试输出
                if (this->m_bIsSimulatedCalibrating)
                {
                    m_oDebugWindow.PrintFrame(monoFrame);
                }

                if (FALSE == this->m_bIsSimulatedCalibrating)
                {
                    FillBoard(m_AutoCalibrateWnd, BACKGROUND_COLOR);//全黑

                    RECT rcLowerArea = this->m_CurrentMonitorInfo.rcArea;

                    rcLowerArea.top = rcLowerArea.top + (rcLowerArea.bottom - rcLowerArea.top) / 2;

                    //显示下半部白屏
                    FillBoard(m_AutoCalibrateWnd, FOREGROUND_COLOR, &rcLowerArea);

                }

                //m_nStageWaitCount = 0;
                m_oWaitTimer.Reset();
                m_eAutoMaskStage = E_AUTO_MASK_SERACH_LOWER_HALF_MASK;
            }
        }

        break;


    case E_AUTO_MASK_SERACH_LOWER_HALF_MASK://侦测旋转方向第二步, 侦测下半部高亮区域重心

        //m_nStageWaitCount++;

        //if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)
        if (m_oWaitTimer.IsWaitTimeout(MAX_NEED_WAIT_TIME))
        {
            monoFrame -= m_oBackgroundFrame;

            //调试输出
            if (this->m_bIsSimulatedCalibrating)
            {
                m_oDebugWindow.PrintFrame(monoFrame);
            }


            //二值化灰度图图片
            int hist[256];
            memset(hist, 0, sizeof(hist));
            int nLoopCount = monoFrame.GetPixelCount();;
            const BYTE* pPixel = monoFrame.GetData();
            while (nLoopCount)
            {
                hist[*pPixel] ++;
                pPixel++;
                nLoopCount--;
            }

            //ostu法查找最优门限
            BYTE threshold = FindThreshold_Ostu(hist);
            threshold = (BYTE)((int)threshold * 85 / 100);//门限降低至85%
            monoFrame.Binarize(threshold);


            //调试输出
            if (this->m_bIsSimulatedCalibrating)
            {
                m_oDebugWindow.PrintFrame(monoFrame);
            }


            //在二值图中提取面积最大的高亮区域。
            RECT rcBoundary;
            int nPixelCount = 0;
            POINT ptCenter;

            //计算白块的重心
            BOOL bRet = ExtractTheLargestObject(monoFrame, &rcBoundary, &nPixelCount, &ptCenter);

            if (!bRet)
            {
                //自动屏蔽失败
                m_eAutoMaskingError = E_AUTO_MASKING_ERROR_DETECT_DISPLAY_AREA_FAILED;
                m_eAutoMaskStage = E_AUTO_MASK_END;
                break;
            }
            else
            {
                //下半部屏蔽图
                m_oLowerHalfMaskFrame = monoFrame;

                if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
                {
                    Debug_SaveImageFrame(monoFrame, _T("Rotation_02.jpg"));
                }

                //调试输出
                if (this->m_bIsSimulatedCalibrating)
                {
                    m_oDebugWindow.PrintFrame(monoFrame);
                }

                this->m_oScreenMaskFrame |= m_oLowerHalfMaskFrame;
                //补洞操作
                FillHole(m_oScreenMaskFrame);

                if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
                {
                    Debug_SaveImageFrame(monoFrame, _T("FullScreenMask.jpg"));
                }


                //m_nStageWaitCount = 0;
                m_oWaitTimer.Reset();

                //静态屏蔽前的预处理, 通过回调函数打开激光器
                if (this->m_tStaticMaskingParams.fpPreStaticMaskingProc)
                {
                    (*this->m_tStaticMaskingParams.fpPreStaticMaskingProc)(this->m_tStaticMaskingParams.lpPreStaticMaskingCtx);
                }
                m_eAutoMaskStage = E_AUTO_MASK_STATIC_START;
            }
        }

        break;

    case E_AUTO_MASK_STATIC_START:

        //静态屏蔽前的预处理, 通过回调函数打开激光器
        if (this->m_tStaticMaskingParams.fpPreStaticMaskingProc)
        {
            (*this->m_tStaticMaskingParams.fpPreStaticMaskingProc)(this->m_tStaticMaskingParams.lpPreStaticMaskingCtx);
        }

        //腐蚀屏蔽图，扩大屏幕区域
        //膨胀屏幕检测区域,即腐蚀屏蔽区
        for (int i = 0; i < this->m_tStaticMaskingParams.nMaskEroseSize; i++)
        {

            Morph_Dilate8(
                m_oScreenMaskFrame.GetData(),
                m_oScreenMaskFrame.GetData(),
                m_oScreenMaskFrame.Width(),
                m_oScreenMaskFrame.Height());

        }

        m_vecMonitorCalibResults[m_nDispMonitorId].maskFrame = m_oScreenMaskFrame;

        //设置静态屏蔽镜头参数,合上滤光片
        if (FALSE == this->m_bIsSimulatedCalibrating)//非仿真模式
        {
            if (m_fpChangeCameraParams)
            {
                m_fpChangeCameraParams(E_CAMERA_AUTO_MASK, m_lpCtx, 0, 0);
            }
        }

        //初始化静态屏蔽器
        this->m_oStaticMaskFinder.Reset(
            monoFrame.Width(),
            monoFrame.Height(),
            m_tStaticMaskingParams.cStaticMaskThreshold,
            m_AutoCalibrateWnd,
            m_CurrentMonitorInfo.rcArea, true);

        //m_nStageWaitCount = 0;
        m_oWaitTimer.Reset();
        //m_nStageFrameCount = 0;
        m_eAutoMaskStage = E_AUTO_MASK_STATIC_MASKING;//E_AUTO_MASK_STATIC_MASKING;
        break;


    case E_AUTO_MASK_STATIC_MASKING://静态屏蔽

        if (m_oWaitTimer.IsWaitTimeout(IR_LED_FLASH_TIME))
        {//静态屏蔽阶段
            if (this->m_oStaticMaskFinder.Process(monoFrame, true))
            {
                m_eAutoMaskingError = E_AUTO_MASKING_OK;
                //自动屏蔽结束
                m_eAutoMaskStage = E_AUTO_MASK_END;
            }
        }

        break;

    case E_AUTO_MASK_END:
        //结束自动自动屏蔽();
        EndAutoMasking();
        m_eAutoMaskStage = E_AUTO_MASK_STOPPED;
        break;

    case E_AUTO_MASK_STOPPED:

        return FALSE;

    }//switch

    return TRUE;
}


//@功能:自动屏蔽结束事件
void CAutoCalibratorImpl2::EndAutoMasking()
{
    if (E_AUTO_MASKING_OK == m_eAutoMaskingError)
    {
        //生成总屏蔽图
        ProduceAllMonitorMaskFrame();

        //生成屏幕映射图
        ProduceScreenMap();
    }

    ShowCursor(TRUE);
    if (m_bIsWorking)
    {
        if (m_bIsSimulatedCalibrating)
        {
            PostThreadMessage(m_dwSimulatedCalibrateThreadId, WM_QUIT, 0, 0);
            //WaitForSingleObject(m_hSimulatedCalibrateThread, INFINITE);
            //m_oAVIInput.Close();
            m_oAVIInput.close();
        }
        else
        {
            //隐藏校正窗体
            m_AutoCalibrateWnd.ShowWindow(SW_HIDE);

            //关闭超时检测功能
            m_AutoCalibrateWnd.CloseTimeoutDetect();

            m_bIsWorking = FALSE;


            //if(m_bRecordVideo)//详细输出时, 保存视频
            //{
            //    m_oAVIRecorder.Close();
            //}

            if (E_AUTO_CALIBRATOR_OK == this->m_eCalibrateError && !this->m_bSaveInermediatFile)
            {//成功且设置不保存自动校正中间文件标志

                //删除成功校正后的校正图偏和AVI文件
                DeleteDirectory(m_strCurrentImageOutputDir);
            }
        }

        PostMessage(m_hNotifyWnd, WM_SEARCH_SCREEN_AREA_DONE, 0, LPARAM(this->m_eAutoMaskingError == E_AUTO_MASKING_OK));

        //恢复语言栏
        if (m_bRestoreLanguageBar)
        {
            ShowLanguageBar(TRUE);
        }
    }
}



ULONG _stdcall CAutoCalibratorImpl2::SimulatedCalibrateProc(LPVOID lpCtx)
{

    CAutoCalibratorImpl2* pCalibrator = reinterpret_cast<CAutoCalibratorImpl2*>(lpCtx);

    const BITMAPINFOHEADER* bmpinfoHeader = pCalibrator->m_oAVIInput.GetAVIFormat();
    if (bmpinfoHeader == NULL)
    {
        return -1;
    }

    int nImageWidth  = bmpinfoHeader->biWidth;
    int nImageHeight = bmpinfoHeader->biHeight;

    //生成调试窗体
    int nCaptionHeight = GetSystemMetrics(SM_CYCAPTION);
    int nFrameCx = GetSystemMetrics(SM_CXFRAME);
    int nFrameCy = GetSystemMetrics(SM_CYFRAME);


    RECT rcPosition;
    rcPosition.left  = 0;
    rcPosition.top   = 0;
    rcPosition.right = nImageWidth;
    rcPosition.bottom = nImageHeight;

    rcPosition.right += nFrameCx * 2;
    rcPosition.bottom += nCaptionHeight + nFrameCy * 2;


    pCalibrator->m_oDebugWindow.Create(NULL, rcPosition, _T("Dbg Window"), WS_POPUP | WS_OVERLAPPEDWINDOW | WS_VISIBLE);


    pCalibrator->m_oDebugWindow.SetWindowPos(
        HWND_TOPMOST,
        rcPosition.left,
        rcPosition.top,
        rcPosition.right - rcPosition.left,
        rcPosition.bottom - rcPosition.top,
        SWP_SHOWWINDOW);


    


    CImageFrame mjpgFrame;
    CImageFrame grayFrame;

    mjpgFrame.SetSize(nImageWidth, nImageHeight, bmpinfoHeader->biBitCount >> 3);
    grayFrame.SetSize(nImageWidth, nImageHeight, 1);

    CImageFrame onlineScreenArea;

    if (pCalibrator->m_bEnableOnlineScreenArea && pCalibrator->m_vecDbgOnlineScreenVertices.size())
    {
        BYTE initValue = 0x00;
        onlineScreenArea.SetSize(bmpinfoHeader->biWidth, bmpinfoHeader->biHeight, 1, &initValue);
        FillPolygon(
            onlineScreenArea.GetData(),
            bmpinfoHeader->biWidth,
            bmpinfoHeader->biHeight,
            &pCalibrator->m_vecDbgOnlineScreenVertices[0],
            pCalibrator->m_vecDbgOnlineScreenVertices.size(),
            255,
            TRUE);

        Debug_SaveImageFrame(onlineScreenArea, _T("OnlineScreenArea.jpg"));
    }
    else
    {
        BYTE initValue = 0xFF;
        onlineScreenArea.SetSize(bmpinfoHeader->biWidth, bmpinfoHeader->biHeight, 1, &initValue);
    }


    pCalibrator->m_oCalibratePattern.InitPattern(pCalibrator->m_nDbgPatternRadius, pCalibrator->m_rcDbgMonitor);


    CMJPG_Decoder m_oMJPGDecoder;//MJPG解码器

    ::SetTimer(NULL, 1, 10, NULL);

    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0))
    {

        switch (msg.message)
        {
        case WM_TIMER:
        {
            LONG lBytes = 0;
            LONG lSamples = 0;
            BOOL bRet = FALSE;


            mjpgFrame.SetSize(nImageWidth, nImageHeight, 2);
            //bRet = pCalibrator->m_oAVIInput.Read((BYTE*)mjpgFrame.GetData(), mjpgFrame.Size(), &lBytes, &lSamples);
            bRet = pCalibrator->m_oAVIInput.readFrame((BYTE*)mjpgFrame.GetData(), mjpgFrame.Size(), &lBytes, &lSamples);
            if (!bRet)
            {

                PostThreadMessage(::GetCurrentThreadId(), WM_QUIT, 0, 0);
                break;
            }

            if (bmpinfoHeader->biCompression == MJPG)
            {
                int nMJPGDataLength = 0;
                m_oMJPGDecoder.ProcessData(mjpgFrame.GetData(), lBytes, (BYTE *)grayFrame.GetData(), &nMJPGDataLength);
            }
            else if (bmpinfoHeader->biCompression == YUY2)
            {
                Covert2MonochromeFrame_Y(*(CYUY2Frame*)&mjpgFrame, grayFrame);
            }

            //用在线的屏蔽图做与运算
            grayFrame &= onlineScreenArea;

            bRet = pCalibrator->FeedImage(&grayFrame);
            if (!bRet)
            {
                //pCalibrator->m_oDebugWindow.DestroyWindow();
                PostThreadMessage(::GetCurrentThreadId(), WM_QUIT, 0, 0);
            }

        }

        break;


        default:
            TranslateMessage(&msg);
            DispatchMessage(&msg);

        }//switch

    }//while


    pCalibrator->m_oDebugWindow.DestroyWindow();
    pCalibrator->m_bIsSimulatedCalibrating = FALSE;
    pCalibrator->m_bIsWorking = FALSE;
    //pCalibrator->m_oAVIInput.Close();
    pCalibrator->m_oAVIInput.close();

    return 0;

}

//调试工具
void CAutoCalibratorImpl2::DebugTool_LoadCalibrateData(
    const char* szCalirateDataFileName,
    const double rotationMatrix[2][2],//坐标旋转矩阵
    const POINT& ptCenter)
{

    FILE* file = NULL;
    fopen_s(&file, szCalirateDataFileName, "r");
    int nCount = 0;
    int nGroupId = 0;
    if (file == NULL) return;


    // x |c11 c12 |
    // = | |
    // y |c21 c22 |

    //旋转矩阵系数
    double c11, c12, c21, c22;

    c11 = rotationMatrix[0][0];
    c12 = rotationMatrix[0][1];

    c21 = rotationMatrix[1][0];
    c22 = rotationMatrix[1][1];

    m_vecInteriorCalibPoint.clear();

    char szText[1024];

    BOOL bIsCalibrateDataBegin = FALSE;
    RECT rcMonitor;
    enum EStage {
        E_STAGE_BEGIN = 0,
        E_READ_SCREEN_SIZE = 1,
        E_SEARCH_CALIBRATE_DATA = 2,
        E_READ_POINT_DATA = 3
    };

    EStage stage = E_STAGE_BEGIN;
    while (fgets(szText, _countof(szText), file))
    {

        switch (stage)
        {
        case E_STAGE_BEGIN:
            if (_stricmp(szText, "#screen dimension in pixel\n") == 0)
            {
                stage = E_READ_SCREEN_SIZE;
            }
            break;

        case E_READ_SCREEN_SIZE:
        {
            int nRet =
                sscanf_s(szText,
                    "%d,%d,%d,%d",
                    &rcMonitor.left,
                    &rcMonitor.top,
                    &rcMonitor.right,
                    &rcMonitor.bottom);

            if (nRet != 4) continue;

            stage = E_SEARCH_CALIBRATE_DATA;
        }

        break;

        case E_SEARCH_CALIBRATE_DATA:

            if (_stricmp(szText, "#calibrate points(u,v,x,y)\n") == 0)
            {
                stage = E_READ_POINT_DATA;
            }
            break;


        case E_READ_POINT_DATA:
        {

            double u, v;
            int nGet = sscanf_s(szText,
                "%lf,%lf",
                &u,
                &v);

            if (nGet != 2)
            {
                continue;
            }

            TImageCalibPoint imageCalibPoint;
            imageCalibPoint.pt2DImageCoord.d[0] = -1;
            imageCalibPoint.pt2DImageCoord.d[1] = -1;

            //double du = u - ptCenter.x;
            //double dv = v - ptCenter.y;
            //imageCalibPoint.ptRotatedCoord.x = LONG(c11 * du + c12 * dv) + ptCenter.x;
            //imageCalibPoint.ptRotatedCoord.y = LONG(c21 * du + c22 * dv) + ptCenter.y;
            imageCalibPoint.ptRotatedCoord.x = long(u);
            imageCalibPoint.ptRotatedCoord.y = long(v);
            m_vecInteriorCalibPoint.push_back(imageCalibPoint);
        }
        break;

        }
    }//whie

    if (file)
    {
        fclose(file);
    }

    int nScreenWidth = rcMonitor.right - rcMonitor.left;
    this->m_ePattern = E_CALIBRATE_PATTERN_11_COLS;
    int Radius = m_oCalibratePattern.CalculateCalibPatternRadius(this->m_ePattern, nScreenWidth);
    m_oCalibratePattern.InitPattern(Radius, rcMonitor);

    //<<debug
    if (m_eDebugLevel >= E_CALIB_DEBUG_LEVEL_DEBUG)
    {

        CImageFrame rotateCornersFrame(m_oPatternFrame.Width(), m_oPatternFrame.Height());

        for (std::vector<TImageCalibPoint>::size_type i = 0;
            i < m_vecInteriorCalibPoint.size();
            i++)
        {

            rotateCornersFrame.SetPixel(m_vecInteriorCalibPoint[i].ptRotatedCoord, 255);
        }

        CDib dib;
        LoadToDib(rotateCornersFrame, dib);
        dib.SetPixel(ptCenter.x, ptCenter.y, RGB(255, 0, 0));
        CAtlString strFileName;
        strFileName.Format(_T("debugtool_calibrate_marker_roated.png"));
        Debug_SaveDibToPNG(dib, strFileName);//保存为PNG,避免颜色失真
    }
    //debug>>
}


void CAutoCalibratorImpl2::InitWaitTimer(DWORD dwTimeMagnification, DWORD dwVideoDisplayDelay)
{
    this->m_oWaitTimer.Init(dwTimeMagnification, dwVideoDisplayDelay);
    this->m_oMonitorAreaLocator.GetWaiterTimer().Init(dwTimeMagnification, dwVideoDisplayDelay);
    this->m_oMonitorBoundaryFinder.GetWaiterTimer().Init(dwTimeMagnification, dwVideoDisplayDelay);
}


//

//调试工具,记录模拟参数
void CAutoCalibratorImpl2::DbgDumpSimulateParams(const TAutoCalibrateParams& autoCalibrateParams)
{
    FILE* file;

    CAtlString strFileName;
    strFileName.Format(
        _T("%s\\simulate-params.txt"),
        m_strImageOutputBaseDir);

    CT2CA szFileName(strFileName);

    fopen_s(&file, szFileName, "a");

    if (file == NULL) return;

    char szData[1024];


    const AutoCalibrateParamsList& autoCalibrateParamList = autoCalibrateParams.autocalibrateparamslist;
    //校正参数
    for (size_t i = 0; i < autoCalibrateParamList.size(); i++)
    {
        sprintf_s(
            szData,
            _countof(szData),
            "%s\n",
            "#calibrate params");
        fwrite(szData, 1, strlen(szData), file);

        sprintf_s(
            szData,
            _countof(szData),
            "expected-brightness=%d, spd=%d, video-delay=%d, brightness=%d, gamma=%d\n",

            autoCalibrateParamList[i].autoCalibrateImageParams.autoCalibrateExpectedBrightness,
            autoCalibrateParamList[i].autoCalibrateImageParams.autoCalibrateSpeed,
            autoCalibrateParamList[i].autoCalibrateImageParams.videoDislayDelay,
            autoCalibrateParamList[i].videoprocampproperty.Prop_VideoProcAmp_Brightness,
            autoCalibrateParamList[i].videoprocampproperty.Prop_VideoProcAmp_Gamma);

        fwrite(szData, 1, strlen(szData), file);
    }

    //使能在线屏蔽图
    sprintf_s(
        szData,
        _countof(szData),
        "%s\n",
        "#EnableOnlineScreenArea");
    fwrite(szData, 1, strlen(szData), file);

    sprintf_s(
        szData,
        _countof(szData),
        "%s\n",
        autoCalibrateParams.bEnableOnlineScreenArea ? "TRUE" : "FALSE");
    fwrite(szData, 1, strlen(szData), file);

    //手动在线屏蔽图顶点位置
    sprintf_s(
        szData,
        _countof(szData),
        "%s\n",
        "#Online Screen Area Vertices");
    fwrite(szData, 1, strlen(szData), file);


    const std::vector<POINT>&  onlineVertices = autoCalibrateParams.vecOnlineAreaVertices;

    int nVerticesCount = onlineVertices.size();
    memset(szData, 0, sizeof(szData));
    for (int i = 0; i < nVerticesCount; i++)
    {
        char text[128];
        sprintf_s(text, _countof(text), "(%d,%d)%s",
            onlineVertices[i].x,
            onlineVertices[i].y,
            i == (nVerticesCount - 1) ? "" : ";");

        strcat_s(szData, _countof(szData), text);
    }

    strcat_s(szData, _countof(szData), "\n");
    fwrite(szData, 1, strlen(szData), file);


    //屏幕宽度
    sprintf_s(
        szData,
        _countof(szData),
        "%s\n",
        "#monitor  width");
    fwrite(szData, 1, strlen(szData), file);

    sprintf_s(
        szData,
        _countof(szData),
        "%d\n",
        m_uMonitorWidth);
    fwrite(szData, 1, strlen(szData), file);

    //屏幕高度
    sprintf_s(
        szData,
        _countof(szData),
        "%s\n",
        "#monitor  height");

    fwrite(szData, 1, strlen(szData), file);

    sprintf_s(
        szData,
        _countof(szData),
        "%d\n",
        m_uMonitorHeight);
    fwrite(szData, 1, strlen(szData), file);

    fclose(file);

}

//@功能:记录校正图案中圆的半径和屏幕像素尺寸
void CAutoCalibratorImpl2::DbgDumpPatternInfo(int radius, const RECT& rcArea)
{
    FILE* file;

    CAtlString strFileName;
    strFileName.Format(
        _T("%s\\simulate-params.txt"),
        m_strImageOutputBaseDir);

    CT2CA szFileName(strFileName);
    fopen_s(&file, szFileName, "a");

    if (file == NULL) return;
    char szData[1024];

    //校正图案中圆的半径
    sprintf_s(
        szData,
        _countof(szData),
        "%s\n",
        "#Pattern Radius");

    fwrite(szData, 1, strlen(szData), file);

    sprintf_s(
        szData,
        _countof(szData),
        "%d\n",
        radius);
    fwrite(szData, 1, strlen(szData), file);


    //屏幕区域
    sprintf_s(
        szData,
        _countof(szData),
        "%s\n",
        "#Monitor Area");

    fwrite(szData, 1, strlen(szData), file);

    sprintf_s(
        szData,
        _countof(szData),
        "%d,%d,%d,%d\n",
        rcArea.left,
        rcArea.top,
        rcArea.right,
        rcArea.bottom);
    fwrite(szData, 1, strlen(szData), file);


    fclose(file);


}

void CAutoCalibratorImpl2::DbgLoadSimulateParams(LPCTSTR lpszFileName)
{
    FILE* file = NULL;
    CT2CA szFileName(lpszFileName);

    fopen_s(&file, szFileName, "r");

    if (file == NULL) return;

    char szText[1024];

    m_oautocalibrateparamslist.clear();

    m_vecDbgOnlineScreenVertices.clear();

    m_nDbgPatternRadius = 58;
    m_rcDbgMonitor = RECT{ 0, 0, 1920,1080 };

    enum EParsetage
    {
        E_PARSE_STAGE_READ_COMMENTS,
        E_PARSE_ONLINE_AREA_FLAG,
        E_PARSE_ONLINE_AREA_VERTICES,
        E_PARSE_PATTERN_RADIUS,
        E_PARSE_MONITOR_AREA,
        E_PARSE_CALIBRATE_PARAM,
    };

    EParsetage eParseStage = E_PARSE_STAGE_READ_COMMENTS;
    int nRet = 0;
    while (fgets(szText, _countof(szText), file))
    {
        //删除行尾空格和回车换行符
        for (int pos = strlen(szText) - 1; pos >= 0; pos--)
        {
            if (szText[pos] == '\r' || szText[pos] == '\n' || szText[pos] == ' ')
            {
                szText[pos] = '\0';
            }
            else
            {
                break;
            }
        }

        switch (eParseStage)
        {
        case E_PARSE_STAGE_READ_COMMENTS:
            if (_stricmp(szText, "#calibrate params") == 0)
            {
                eParseStage = E_PARSE_CALIBRATE_PARAM;
            }
            else if (_stricmp(szText, "#EnableOnlineScreenArea") == 0)
            {
                eParseStage = E_PARSE_ONLINE_AREA_FLAG;
            }
            else if (_stricmp(szText, "#Online Screen Area Vertices") == 0)
            {
                eParseStage = E_PARSE_ONLINE_AREA_VERTICES;
            }
            else if (_stricmp(szText, "#Pattern Radius") == 0)
            {
                eParseStage = E_PARSE_PATTERN_RADIUS;
            }
            else if (_stricmp(szText, "#Monitor Area") == 0)
            {
                eParseStage = E_PARSE_MONITOR_AREA;
            }
            break;


        case E_PARSE_CALIBRATE_PARAM:
        {
            AutoCalibrateParams params;
            memset(&params, 0, sizeof(params));

            nRet = sscanf_s(szText,
                "expected-brightness=%hhd, spd=%hhd, video-delay=%hhd, brightness=%d, gamma=%d\n",
                &params.autoCalibrateImageParams.autoCalibrateExpectedBrightness,
                &params.autoCalibrateImageParams.autoCalibrateSpeed,
                &params.autoCalibrateImageParams.videoDislayDelay,
                &params.videoprocampproperty.Prop_VideoProcAmp_Brightness,
                &params.videoprocampproperty.Prop_VideoProcAmp_Gamma
            );

            if (nRet == 5)
            {
                m_oautocalibrateparamslist.push_back(params);
            }

            eParseStage = E_PARSE_STAGE_READ_COMMENTS;
        }

        break;

        case E_PARSE_ONLINE_AREA_FLAG:
            if (_stricmp(szText, "TRUE") == 0)
            {
                m_bEnableOnlineScreenArea = TRUE;
            }
            else if (_stricmp(szText, "FALSE") == 0)
            {
                m_bEnableOnlineScreenArea = FALSE;
            }

            eParseStage = E_PARSE_STAGE_READ_COMMENTS;

            break;

        case E_PARSE_ONLINE_AREA_VERTICES:
        {
            const char* pStart = szText;
            const char* pEnd = pStart;

            while (*pEnd)
            {
                if (*pEnd == ';')
                {//
                    POINT pt;
                    int nRet = sscanf_s(pStart, "(%d, %d)", &pt.x, &pt.y);
                    if (nRet == 2)
                    {
                        m_vecDbgOnlineScreenVertices.push_back(pt);
                    }
                    pEnd++;
                    pStart = pEnd;
                }
                else
                {
                    pEnd++;
                }
            }//while

            if (pStart != pEnd)
            {
                POINT pt;
                nRet = sscanf_s(pStart, "(%d, %d)", &pt.x, &pt.y);
                if (nRet == 2)
                {
                    m_vecDbgOnlineScreenVertices.push_back(pt);
                }
            }

            eParseStage = E_PARSE_STAGE_READ_COMMENTS;
        }
        break;

        case E_PARSE_PATTERN_RADIUS:
            nRet = sscanf_s(szText, "%d", &m_nDbgPatternRadius);
            eParseStage = E_PARSE_STAGE_READ_COMMENTS;
            break;


        case E_PARSE_MONITOR_AREA:
            nRet = sscanf_s(
                szText, "%d,%d,%d,%d",
                &m_rcDbgMonitor.left,
                &m_rcDbgMonitor.top,
                &m_rcDbgMonitor.right,
                &m_rcDbgMonitor.bottom);

            eParseStage = E_PARSE_STAGE_READ_COMMENTS;
            break;
        }//switch
    }//while

    fclose(file);
}

const CImageFrame& CAutoCalibratorImpl2::GetMaskFinderFrame(CImageFrame& maskframe)
{
    if (!m_oStaticMaskFinder.GetMaskFrame().IsEmpty())
    {
        return maskframe &= this->m_oStaticMaskFinder.GetMaskFrame();
    }
    return maskframe;
}

