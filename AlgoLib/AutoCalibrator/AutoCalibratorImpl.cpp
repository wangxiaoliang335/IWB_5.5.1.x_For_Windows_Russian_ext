#include "stdafx.h"
#include "AutoCalibratorImpl2.h"
#include "../MorphologyAlgo/inc/MorphologyAlgo.h"
#include "GlobalVar.h"
#include "SIMD.h"
#include "FillPolygon.h"
#include "SortInDirection.h"
#include <limits>
//@功能:开始校正, 供外部模块调用
//@参数:hNotifyWnd, 消息通知窗体
//      E_AutoCalibratePattern, 校正方案枚举值
//      bSaveInermediatFile, 是否保存校正的中间结果图片
//      bRecordVideo，是否记录校正过程中的原始视频
BOOL CAutoCalibratorImpl2::StartCalibrating(HWND hNotifyWnd, E_AutoCalibratePattern ePattern, CHANGECAMERAPARAMSPROC ChangeCameraParamsProc, LPVOID lpCtx, BOOL bSaveInermediatFile  ,BOOL bRecordVideo)
{
    m_bSaveInermediatFile  = bSaveInermediatFile;
    m_bRecordVideo         = bRecordVideo;
    m_fpChangeCameraParams = ChangeCameraParamsProc;
    m_lpCtx                = lpCtx;

    if(m_bIsCalibrating)
    {
        return FALSE;
    }


    //生成
    if(HWND(m_AutoCalibrateWnd) == NULL)
    {
        if(!m_AutoCalibrateWnd.CreateFullScreen())
        {
            return FALSE;
        }
    }


    m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_UNDEF;
    m_hNotifyWnd = hNotifyWnd;
    m_AutoCalibrateWnd.ShowWindow(SW_SHOW);
    m_AutoCalibrateWnd.SetFocus();


    //<<2013/03/11,by toxuke@gmail.com 重置自动校正窗体超时检测状态
    m_AutoCalibrateWnd.StartTimeoutDetect();
    //>>


    // m_eMonochromizeAlog = E_MONOCHROMIZE_ALOG_Y;//黑白化算法
    m_eCalibrateStage = E_AUTO_CALIBRATE_START;
    m_nStageWaitCount = 0;
    m_bIsCalibrating = TRUE;
    m_bIsSimulatedCalibrating = FALSE;

    //InitAutoCalibrateSaveDirectory();
    SYSTEMTIME now;
    GetLocalTime(&now);
    m_strCliabrateDebugDir.Format(
        _T("%s\\AutoCalibrate\\%04d_%02d_%02d_%02d_%02d_%02d"),
        g_strIntermediatRootDirName,
        now.wYear,
        now.wMonth,
        now.wDay,
        now.wHour,
        now.wMinute,
        now.wSecond);



    CAtlString strFileName;
    SYSTEMTIME localTime;
    GetLocalTime(&localTime);
    strFileName.Format(_T("%s\\AutoCalibrate_%04d%02d%02d-%02d%02d%02d%03d.avi"),
        m_strCliabrateDebugDir,
        localTime.wYear,
        localTime.wMonth,
        localTime.wDay,
        localTime.wHour,
        localTime.wMinute,
        localTime.wSecond,
        localTime.wMilliseconds);

    m_uCx = GetSystemMetrics(SM_CXSCREEN);
    m_uCy = GetSystemMetrics(SM_CYSCREEN);

    m_ePattern = ePattern;

    //隐藏鼠标光标
    ShowCursor(FALSE);


    m_nTryTimes = 1;
    //设置图片输出目录
    CString strImageDubugDir;
    strImageDubugDir.Format(_T("%s\\%d"),m_strCliabrateDebugDir, m_nTryTimes);
    SetImageDebugDir(strImageDubugDir);

    if(m_bRecordVideo)//需要保存校正视频
    {
        m_oAVIRecorder.OpenFile(strFileName, 640, 480);
    }

    return TRUE;
}


//@功能:停止校正
void CAutoCalibratorImpl2::EndCalibrating()
{
    ShowCursor(TRUE);
    if(m_bIsCalibrating)
    {
        if(m_bIsSimulatedCalibrating)
        {
            PostThreadMessage(m_dwSimulatedCalibrateThreadId, WM_QUIT, 0, 0 );
            //WaitForSingleObject(m_hSimulatedCalibrateThread, INFINITE);
            m_oAVIInput.Close();
        }
        else
        {
            //隐藏校正窗体
            m_AutoCalibrateWnd.ShowWindow(SW_HIDE);

            //关闭超时检测功能
            m_AutoCalibrateWnd.CloseTimeoutDetect();

            m_bIsCalibrating = FALSE;


            if(m_bRecordVideo)//详细输出时, 保存视频
            {
                m_oAVIRecorder.Close();
            }

            if(E_AUTO_CALIBRATOR_OK == this->m_eCalibrateError && !this->m_bSaveInermediatFile)
            {//成功且设置不保存自动校正中间文件标志

                //删除成功校正后的校正图偏和AVI文件
                DeleteDirectory(m_strCliabrateDebugDir);
            }
        }



        PostMessage(m_hNotifyWnd,WM_AUTO_CALIBRATE_DONE, WPARAM(this->m_eCalibrateError), LPARAM(this->m_bIsSimulatedCalibrating));
    }
}

//@功能:初始化屏幕上部和下部边界白色小方块的位置
//@参数:hWnd, 校正窗体句柄
//      nHorzSideSquareNumber, 水平方向白色小方块的个数
//      VertSideSquareNumber, 垂直方向白色小方块的个数
//      nSquareWidth, 白色小方块边长
//@说明:注意:如果垂直方向的白色小方块的数目为奇数，则上下两部分有两个点是重合的。

void CAutoCalibratorImpl2::InitBorderWhiteSquarePositions(HWND hWnd,int nHorzSideSquareNumber, int nVertSideSquareNumber, int nSquareWidth)
{
    //从计算机屏幕的左上角开始，按顺时针方向依次对各个白色小方块编号
    //例如:
    //0     1     2     3
    //                   
    //9                 4
    //                   
    //8    7      6     5
    //

    RECT rcClient;
    GetClientRect(hWnd,&rcClient);

    int i= 0;
    int W  = rcClient.right  - rcClient.left;
    int H  = rcClient.bottom - rcClient.top ;


    int  nHorzIntervalNum = nHorzSideSquareNumber - 1;
    int nVertIntervalNum  = nVertSideSquareNumber - 1;
    int _HorzInterval_Integer  = (W - nSquareWidth)/nHorzIntervalNum;//每个方块的水平间距数值的整数部分
    int _HorzInterval_Fraction = (W - nSquareWidth)%nHorzIntervalNum;//每个方块的水平间距数值的余数部分

    int _VertInterval_Integer  = (H - nSquareWidth)/nVertIntervalNum;//每个方块的水平间距数值的整数部分
    int _VertInterval_Fraction = (H - nSquareWidth)%nVertIntervalNum;//每个方块的水平间距数值的余数部分

    RECT rcSquare;
    rcSquare.left   = rcClient.left;
    rcSquare.top    = rcClient.top ;
    rcSquare.right  = rcSquare.left + nSquareWidth;
    rcSquare.bottom = rcSquare.top  + nSquareWidth;

    int _FractionCount = 0;

    int iIndex = 0;

    int nSqureNumber = 2*(nHorzSideSquareNumber - 1) + 2*(nVertSideSquareNumber - 1);
    m_vecBorderSquarePositions.resize(nSqureNumber);


    //顶边
    for(i=0; i < nHorzIntervalNum; i++)
    {
        m_vecBorderSquarePositions[iIndex++] = rcSquare;

        rcSquare.left  += _HorzInterval_Integer;
        _FractionCount += _HorzInterval_Fraction;
        if(_FractionCount > nHorzIntervalNum)
        {
            rcSquare.left ++;
            _FractionCount -= nHorzIntervalNum;
        }

        rcSquare.right = rcSquare.left + nSquareWidth;
    }


    _FractionCount = 0;
    //右边
    for(i=0; i < nVertIntervalNum; i++)
    {
        m_vecBorderSquarePositions[iIndex++] = rcSquare;
        rcSquare.top   += _VertInterval_Integer;
        _FractionCount += _VertInterval_Fraction;
        if(_FractionCount > nVertIntervalNum)
        {
            rcSquare.top ++;
            _FractionCount -= nVertIntervalNum;
        }
        rcSquare.bottom = rcSquare.top + nSquareWidth;

    }




    _FractionCount = 0;
    //底边
    for(i=0; i < nHorzIntervalNum; i++)
    {
        m_vecBorderSquarePositions[iIndex++] = rcSquare;
        rcSquare.left  -= _HorzInterval_Integer;
        _FractionCount -= _HorzInterval_Fraction;
        if(_FractionCount < -nHorzIntervalNum)
        {
            rcSquare.left --;
            _FractionCount += nHorzIntervalNum;
        }
        rcSquare.right = rcSquare.left + nSquareWidth;


    }


    //左边
    _FractionCount = 0;
    for(i=0; i < nHorzIntervalNum; i++)
    {
        m_vecBorderSquarePositions[iIndex++] = rcSquare;
        rcSquare.top   -= _VertInterval_Integer;
        _FractionCount -= _VertInterval_Fraction;
        if(_FractionCount < -nHorzIntervalNum)
        {
            rcSquare.top --;
            _FractionCount += nHorzIntervalNum;
        }
        rcSquare.bottom = rcSquare.top + nSquareWidth;
    }

    //最后一个方块
    m_vecBorderSquarePositions[iIndex] = rcSquare;

    //将边界小白块分为上下两部分
    //上半部分
    //0     1     2     3
    //                   
    //9                 4
    int nUpperHalfSquareNumber = nHorzSideSquareNumber + ((nVertSideSquareNumber + 1)/2 - 1)*2;
    m_vecUpperHalfSquarePositions.resize(nUpperHalfSquareNumber);

    int nHalfVertSideSquareNumber = (nVertSideSquareNumber + 1)/2;
    memcpy(&m_vecUpperHalfSquarePositions[0], &m_vecBorderSquarePositions[0], sizeof(RECT)*(nHorzSideSquareNumber + nHalfVertSideSquareNumber - 1));

    memcpy(
        &m_vecUpperHalfSquarePositions[nHorzSideSquareNumber + nHalfVertSideSquareNumber - 1],
        &m_vecBorderSquarePositions[nSqureNumber - (nHalfVertSideSquareNumber - 1)],
        sizeof(RECT)*nHalfVertSideSquareNumber - 1
        );





    //下半部分
    //9                 4
    //                   
    //8    7      6     5
    int nLowerHalfSquareNumber =  nHorzSideSquareNumber + ((nVertSideSquareNumber + 1)/2 - 1)*2;
    m_vecLowerHalfSquarePositions.resize(nLowerHalfSquareNumber);

    memcpy(
        &m_vecLowerHalfSquarePositions[0],
        &m_vecBorderSquarePositions[nHorzSideSquareNumber + nVertSideSquareNumber/2 - 1],
        sizeof(RECT)*nLowerHalfSquareNumber
        );
}






//@功能:显示围绕边界的白色方块
//@参数:hWnd, 绘制于其上的窗体句柄
//      nSquareWidth,白色方块的边长
//      nNumberEachSide, 每条边的白色方块的数目
//@日期:2013/10/22
void CAutoCalibratorImpl2::DrawSmallSquares(HWND hWnd, const RECT* SquarePositions, int nSquareNumber)
{

    HDC hDC = GetDC(hWnd);

    HBRUSH hBrush = ::CreateSolidBrush(WHITE);

    for(int i=0; i<nSquareNumber; i++)
    {
        FillRect(hDC, &SquarePositions[i], hBrush);
    }


    DeleteObject(hBrush);
    ReleaseDC(hWnd, hDC);

}


//@功能:重置"屏幕区域搜索"子状态机的状态
//@参数:grayImage, 输入的校正图片
void CAutoCalibratorImpl2::ResetSubStateMachine_SearachScreenArea(const CImageFrame& grayImage)
{
    m_tSubStateMachine_SearchScreenArea.nRunTimes   = 0;
    m_tSubStateMachine_SearchScreenArea.nFlashTimes = 0;
    m_tSubStateMachine_SearchScreenArea.bShowSquare = FALSE;
    m_tSubStateMachine_SearchScreenArea.frameAccumulate.SetSize(grayImage.Width(), grayImage.Height(), 2);
    m_tSubStateMachine_SearchScreenArea.frameAccumulate.Clear();
    if(FALSE == this->m_bIsSimulatedCalibrating)
    {
        FillBoard(m_AutoCalibrateWnd, 0, BLACK);
    }

}


//@功能:搜索所有白色方块的重心点，对这些点根据与总重心的方位角的大小进行排序
//@参数:srcImage, 16位灰度位图
//      pPts, 输出参数, 保存找到的重心的数组
//      nExpectedNumber, 期望找到的点的个数
//     pGlobalCentroid ,输出参数, 全局重心
BOOL CAutoCalibratorImpl2::SearchSquareCentroids(const CWordFrame&  srcImage, POINT* pPts, int nExpectedNumber, POINT* pGlobalCentroid)
{
    //16位灰度图片转为8位灰度图片
    int nImageWidth  = srcImage.Width();
    int nImageHeight = srcImage.Height();

    int nPixelCount = nImageWidth*nImageHeight;
    int nLoopCount  = nPixelCount;

    WORD max = srcImage.Max();
    WORD min = srcImage.Min();
    WORD range = max -min;
    const WORD* pWordPixel = srcImage.GetData();

    static CImageFrame grayImage;
    grayImage.SetSize(nImageWidth, nImageHeight, 2);

    BYTE* pPixel = grayImage.GetData();


    //16位灰度位图转为8位灰度位图
    while(nLoopCount)
    {
        *pPixel = BYTE( (UINT)255 * (UINT)(*pWordPixel - min)/(UINT)range);
        pWordPixel++;
        pPixel ++;
        nLoopCount --;
    }

    if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    {
        Debug_SaveImageFrame(grayImage, _T("BorderSquaresSrc.bmp"));
    }


    //根据白色高亮块占屏幕区域的百分比估算白色高亮区域在视频图像中所占的像素个数
    int nHightPixelCountEstimated = SQUARE_SIZE * SQUARE_SIZE * nExpectedNumber * nImageWidth * nImageHeight /(m_uCx * m_uCy);

    //统计直方图
    int hist[256];
    memset(hist, 0, sizeof(hist));
    nLoopCount = nPixelCount;
    pPixel = grayImage.GetData();
    while(nLoopCount)
    {
        hist[*pPixel] ++;
        pPixel ++;
        nLoopCount --;
    }


    BYTE threshold = 255;
    int nSum = 0;
    for(threshold=255;threshold > 0; threshold--)
    {
        nSum += hist[threshold];

        if(nSum >= nHightPixelCountEstimated)
        {
            break;
        }

    }


    static CBitFrame bitFrame;
    bitFrame.SetSize(nImageWidth, nImageHeight);

    static CImageFrame debugImage;
    debugImage.SetSize(nImageWidth, nImageHeight, 1);



    //8位灰度位图转化为二值化的1位位图
    GrayToBitFrame_SSE2(
        grayImage.GetData(), 
        (BYTE*)bitFrame.GetData(),
        threshold,
        nPixelCount);



    //目标检测
    CConnectedComponentScanlineAlogrithm<> blobDetect;

    blobDetect.ProcessImage(
        (const BYTE *)bitFrame.GetData(),
        nImageWidth,
        nImageHeight);


    int nMassThreshold = 2;//质量门限，质量小于该门限的目标被过滤

    int nSquaresFound  = blobDetect.GetObjCount(nMassThreshold);

    //BOOL bInc = TRUE;//门限增加标志

    //通过侦测到的目标的数目来动态确定门限的高低
    if(nSquaresFound != nExpectedNumber)
    {
        //if(nSquaresFound < nExpectedNumber)
        //{    //目标小于期望值, 则降低门限
        //    bInc = FALSE;
        //}
        //else
        //{
        //    bInc = TRUE;
        //}

        do
        {
            //if(bInc)
            //{
            if(threshold == 0xFF)
            {
                break;
            }
            threshold ++;

            //}
            //else
            //{
            //    if(threshold == 0x00)
            //    {
            //        break;
            //    }
            //    threshold --;
            //}

            GrayToBitFrame_SSE2(
                grayImage.GetData(), 
                (BYTE*)bitFrame.GetData(),
                threshold,
                nPixelCount);

            blobDetect.ProcessImage(
                (const BYTE *)bitFrame.GetData(),
                nImageWidth,
                nImageHeight);


            nSquaresFound  = blobDetect.GetObjCount(nMassThreshold);


            if(nSquaresFound == nExpectedNumber)
            {
                break;
            }

            //if(bInc && nSquaresFound <= nExpectedNumber)
            //{
            //    break;
            //}
            //else if(!bInc && nSquaresFound >=  nExpectedNumber)
            //{
            //    break;
            //}


        }while(1);
    }



    if(nSquaresFound != nExpectedNumber)
    {
        return FALSE;
    }
    else//成功
    {

        //二值化的1位位图转为8位灰度位图
        BitToGrayFrame_MMX(
            (const BYTE*)bitFrame.GetData(),
            debugImage.GetData(),
            bitFrame.GetPixelCount());

        if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
        {
            Debug_SaveImageFrame(debugImage, _T("BorderSquaresBinarization.bmp"));
        }



        //过滤小质量的目标
        blobDetect.FilterObject(nMassThreshold);


        //提取找到的边界点

        const TBlobObject* pBlobObj =  blobDetect.GetObjs();
        for(UINT i=0; i < blobDetect.GetObjCount(0); i++)
        {

            pPts[i].x = pBlobObj[i].GPos.x;
            pPts[i].y = pBlobObj[i].GPos.y;

        }

        //计算所有边界点的重心
        POINT ptCentroid;

        int mx = 0;
        int my = 0;
        for(int i=0; i < nExpectedNumber; i++)
        {
            mx += pPts[i].x;
            my += pPts[i].y;
        }


        ptCentroid.x = mx/nExpectedNumber;
        ptCentroid.y = my/nExpectedNumber;

        //对找到的边界点按照方位角度排序
        SortInDirection(&pPts[0], nExpectedNumber, ptCentroid);


        if(pGlobalCentroid)
        {
            *pGlobalCentroid = ptCentroid;
        }
        return TRUE;
    }
}


//@功能:处理边界矩形方块
BOOL CAutoCalibratorImpl2::ProcessBorderSquareImages(const CWordFrame&  srcImage, CAutoCalibratorImpl2::ESearchScreenPart eScreenPart)
{
    CImageFrame* pMaskFrame = NULL;
    POINT* pPts = NULL;
    POINT* pGlobalCentroid = NULL;
    int nPtNumber = 0;
    switch(eScreenPart)
    {

    case E_SEARCH_SCREEN_UPPER:
        m_vecUpperBorderCalibratePts.resize(m_vecUpperHalfSquarePositions.size());
        pPts            = &m_vecUpperBorderCalibratePts[0];
        nPtNumber       = m_vecUpperBorderCalibratePts.size();
        pMaskFrame      = &m_oUpperHalfMaskFrame;
        pGlobalCentroid = &m_ptUpperCentroid;
        break;

    case E_SEARCH_SCREEN_LOWER:
        m_vecLowerBorderCalibratePts.resize(m_vecLowerHalfSquarePositions.size());
        pPts            = &m_vecLowerBorderCalibratePts[0];
        nPtNumber       = m_vecLowerBorderCalibratePts.size();
        pMaskFrame      = &m_oLowerHalfMaskFrame;
        pGlobalCentroid = &m_ptLowerCentroid;
        break;

    case E_SEARCH_SCREEN_WHOLE:
        m_vecBorderCalibratePts.resize(m_vecBorderSquarePositions.size());
        pPts            = &m_vecBorderCalibratePts[0];
        nPtNumber       = m_vecBorderCalibratePts.size();
        pMaskFrame      = &m_oScreenMaskFrame;
        pGlobalCentroid = &m_ptAllCentroid;
        break;

    }//switch

    BOOL bRet = SearchSquareCentroids(srcImage, pPts, nPtNumber, pGlobalCentroid);
    if(!bRet) return FALSE;

    if(pMaskFrame)
    {
        int video_w = srcImage.Width();
        int video_h = srcImage.Height();
        //屏蔽图复位
        pMaskFrame->SetSize(video_w, video_h, 1);
        pMaskFrame->Clear();

        //在屏蔽图中填充多边形
        FillPolygon(
            pMaskFrame->GetData(),
            video_w,
            video_h,
            &pPts[0],
            nPtNumber,
            255,
            TRUE);



        if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
        {
            Debug_SaveImageFrame(*pMaskFrame, _T("ScreenMask.bmp"));
        }

        ////膨胀屏蔽位图4次
        //int nDilateTimes = 4;
        //for(int i=0;i < nDilateTimes; i++)
        //{
        //    Morph_Dilate8(
        //        pMaskFrame->GetData(),
        //        pMaskFrame->GetData(),
        //        pMaskFrame->Width(),
        //        pMaskFrame->Height());
        //}
        //if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
        //{
        //    Debug_SaveImageFrame(*pMaskFrame, _T("ScreenMask_Dilated.bmp"));
        //}

    }

    return TRUE;
}



//@功能:grayImage, 灰度图片
CAutoCalibratorImpl2::ESubStateMachineStatus CAutoCalibratorImpl2::RunSubStateMachine_SearchScreenArea(const CImageFrame& grayImage, CAutoCalibratorImpl2::ESearchScreenPart eScreenPart)
{
    ESubStateMachineStatus status = E_SUB_STATE_MACHINE_CONINTUE;

    //运行计数器+1
    m_tSubStateMachine_SearchScreenArea.nRunTimes ++;

    if(m_tSubStateMachine_SearchScreenArea.nRunTimes % MAX_STAGE_WAIT_COUNT == 0)
    {

        if(m_tSubStateMachine_SearchScreenArea.bShowSquare)//显示方块
        {

            CImageFrame diffImage = grayImage;
            diffImage -=  m_tSubStateMachine_SearchScreenArea.frameBackground ;

            if(this->m_bIsSimulatedCalibrating)
            {
                //调试输出
                m_oDebugWindow.PrintFrame(diffImage);
            }

            int video_w = diffImage.Width();
            int video_h = diffImage.Height();

            int nPixelCount = video_w * video_h;

            BYTE* pDiffPixel = diffImage.GetData();
            WORD* pWordPixel = m_tSubStateMachine_SearchScreenArea.frameAccumulate.GetData();

            int nLoopCount = nPixelCount;
            while(nLoopCount)
            {

                if(*pDiffPixel>= 8)//差分值>8,才认为是有效数据
                {
                    *pWordPixel += *pDiffPixel;
                }

                pWordPixel++;
                pDiffPixel++;
                nLoopCount --;
            }

            m_tSubStateMachine_SearchScreenArea.nFlashTimes ++;



            if( m_tSubStateMachine_SearchScreenArea.nFlashTimes >= MAX_FLASH_TIMES)
            {
                //闪烁过程结束, 处理收集到的图像数据

                if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
                {
                    Debug_SaveImageFrame(m_tSubStateMachine_SearchScreenArea.frameAccumulate, _T("AcccumulateDiff.bmp"));
                }

               //<<debug
                m_oDebugBorderSquareFrame.Merge(grayImage);
                //debug>>





                BOOL bRet = ProcessBorderSquareImages(
                    m_tSubStateMachine_SearchScreenArea.frameAccumulate,
                    eScreenPart);

                if(!bRet)
                {
                    return E_SUB_STATE_MACHINE_FAILED;
                }

                return E_SUB_STATE_MACHINE_FINISHED;
            }


            //全屏黑显示
            if(FALSE == this->m_bIsSimulatedCalibrating)
            {
                FillBoard(m_AutoCalibrateWnd, BLACK);
            }
            m_tSubStateMachine_SearchScreenArea.bShowSquare = FALSE;
        }
        else//未显示方块
        {

            m_tSubStateMachine_SearchScreenArea.frameBackground = grayImage;



            if(FALSE == this->m_bIsSimulatedCalibrating)
            {
                switch(eScreenPart)
                {
                case E_SEARCH_SCREEN_UPPER://显示上部边界方块
                    DrawSmallSquares(this->m_AutoCalibrateWnd, &m_vecUpperHalfSquarePositions[0], m_vecUpperHalfSquarePositions.size());
                    break;

                case E_SEARCH_SCREEN_LOWER://显示下部边界方块
                    DrawSmallSquares(this->m_AutoCalibrateWnd, &m_vecLowerHalfSquarePositions[0], m_vecLowerHalfSquarePositions.size());
                    break;

                case E_SEARCH_SCREEN_WHOLE://显示全部边界方块
                    DrawSmallSquares(this->m_AutoCalibrateWnd, &m_vecBorderSquarePositions[0], m_vecBorderSquarePositions.size());
                    break;

                }
            }

            m_tSubStateMachine_SearchScreenArea.bShowSquare = TRUE;
        }

    }

    return status;
}

//@功能:屏幕区域搜索结束后的后处理函数。主要有计算屏幕的完整屏蔽图、计算旋转矩阵。
//@参数:nImageWidth, 视频图像宽度
//      nImageHeight,视频图像高度
BOOL CAutoCalibratorImpl2::OnPostSearchScreenArea(int nImageWidth, int nImageHeight)
{
    //计算旋转矩阵
    //将所有角点做旋转操作, 使之在 ↓Y →X坐标系
    //计算旋转中心
    m_ptRoationCenter.x = (m_ptUpperCentroid.x + m_ptLowerCentroid.x)/2;
    m_ptRoationCenter.y = (m_ptUpperCentroid.y + m_ptLowerCentroid.y)/2;


    //
    //旋转矢量
    POINT Vx,Vy;

    double VLength;

    //Vy:从旋转重心到下部重心的矢量
    Vy.x = m_ptLowerCentroid.x - m_ptRoationCenter.x;
    Vy.y = m_ptLowerCentroid.y - m_ptRoationCenter.y;

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
    m_dbRotationMatrix[0][0] = (double)Vx.x/VLength;
    m_dbRotationMatrix[0][1] = (double)Vx.y/VLength;

    m_dbRotationMatrix[1][0] = (double)Vy.x/VLength;
    m_dbRotationMatrix[1][1] = (double)Vy.y/VLength;



    //合并上部的边界点和下部的边界点
    if(VERT_SIDE_SQUARE_NUMBER % 2 == 1)//垂直边有奇数个边界点
    {

        //由于上下两部分共用两个点, 需要将这两个点找出

        int relatedIndex[2][2];//[0]:距离最小的两个关联点的数组索引号
        ;//[1]:距离次最小的两个关联点的数组索引号
        memset(relatedIndex, 0 ,sizeof(relatedIndex));
        int nMinimumDist[2]    ;//
        nMinimumDist[0] = (std::numeric_limits<int>::max)();
        nMinimumDist[1] = (std::numeric_limits<int>::max)();

        for(size_t i=0; i < m_vecUpperBorderCalibratePts.size(); i++)
        {
            const POINT& pt1 = m_vecUpperBorderCalibratePts[i];
            for(size_t j=0; j < m_vecLowerBorderCalibratePts.size(); j++)
            {
                const POINT& pt2 = m_vecLowerBorderCalibratePts[j];

                int dist2 = (pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y);
                if(dist2 < nMinimumDist[0])
                {
                    relatedIndex[1][0] = relatedIndex[0][0];
                    relatedIndex[1][1] = relatedIndex[0][1];
                    nMinimumDist[1] = nMinimumDist[0];

                    nMinimumDist[0] = dist2;
                    relatedIndex[0][0] = i;
                    relatedIndex[0][1] = j;
                    continue;
                }

                if(dist2 < nMinimumDist[1])
                {
                    relatedIndex[1][0] = i;
                    relatedIndex[1][1] = j;
                    nMinimumDist[1] = dist2;
                }


            }
        }


        //m_vecBorderCalibratePts.resize(m_vecUpperBorderCalibratePts.size() + m_vecLowerBorderCalibratePts.size() - 2);
        m_vecBorderCalibratePts.resize(BORDER_SQUARE_NUMBER);

        for(size_t i=0; i<m_vecUpperBorderCalibratePts.size(); i++)
        {

            BOOL bMerged = FALSE;
            for(size_t j=0; j < _countof(relatedIndex); j++)
            {
                if(i == relatedIndex[j][0])
                {
                    POINT ptUpper = m_vecUpperBorderCalibratePts[i] ;
                    POINT ptLower = m_vecLowerBorderCalibratePts[relatedIndex[j][1]] ;

                    POINT ptAvg;
                    ptAvg.x     = (ptUpper.x + ptLower.x)>>1;
                    ptAvg.y     = (ptUpper.y + ptLower.y)>>1;

                    m_vecBorderCalibratePts[i] = ptAvg;
                    bMerged = TRUE;
                    break;
                }

            }

            if(!bMerged)
            {
                m_vecBorderCalibratePts[i] = m_vecUpperBorderCalibratePts[i];
            }
        }


        int k = m_vecUpperBorderCalibratePts.size();
        for(size_t i=0; i<m_vecLowerBorderCalibratePts.size(); i++)
        {

            BOOL bSkipped = FALSE;
            for(size_t j=0; j < _countof(relatedIndex); j++)
            {
                if(i == relatedIndex[j][1])
                {
                    bSkipped = TRUE;
                    break;
                }

            }

            if(!bSkipped)
            {
                m_vecBorderCalibratePts[k++] = m_vecLowerBorderCalibratePts[i];
            }
        }


    }
    else//垂直边有偶数个边界点
    {

        m_vecBorderCalibratePts.resize(BORDER_SQUARE_NUMBER);

        memcpy(
            &m_vecBorderCalibratePts[0],
            &m_vecUpperBorderCalibratePts[0],
            sizeof(POINT)*m_vecUpperBorderCalibratePts.size());

        memcpy(
            &m_vecBorderCalibratePts[m_vecUpperBorderCalibratePts.size()],
            &m_vecLowerBorderCalibratePts[0],
            sizeof(POINT)*m_vecLowerBorderCalibratePts.size());
    }

    //计算完整的屏蔽位图
    //计算所有边界点的重心
    POINT ptCentroid;

    int mx = 0;
    int my = 0;
    for(size_t i=0; i < BORDER_SQUARE_NUMBER; i++)
    {
        mx += m_vecBorderCalibratePts[i].x;
        my += m_vecBorderCalibratePts[i].y;
    }


    ptCentroid.x = mx/m_vecBorderCalibratePts.size();
    ptCentroid.y = my/m_vecBorderCalibratePts.size();

    //对找到的边界点按照方位角度排序
    SortInDirection(&m_vecBorderCalibratePts[0], m_vecBorderCalibratePts.size(), ptCentroid);


    m_ptAllCentroid = ptCentroid;


    //屏蔽图复位
    m_oScreenMaskFrame.SetSize(nImageWidth, nImageHeight, 1);
    m_oScreenMaskFrame.Clear();

    //在屏蔽图中填充多边形
    FillPolygon(
        m_oScreenMaskFrame.GetData(),
        nImageWidth,
        nImageHeight,
        &m_vecBorderCalibratePts[0],
        m_vecBorderCalibratePts.size(),
        255,
        TRUE);

    if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    {
        Debug_SaveImageFrame(m_oScreenMaskFrame, _T("FullScrenMask.bmp"));
    }


    //搜索边界上一圈点中,"左上角"的点在数组的索引号, 目的是建立"图像点"到"屏幕点"的映射关系。
    //
    //
    TurnPoint turnPts[4];
    int nFound = SearchTurnPoints(&m_vecBorderCalibratePts[0], m_vecBorderCalibratePts.size(), turnPts, _countof(turnPts));
    if(nFound != _countof(turnPts))
    {
        return FALSE;
    }

    //旋转扳正四个角点
    for(int i=0; i < _countof(turnPts); i++)
    {
        long x = turnPts[i].ptCoord.x - m_ptRoationCenter.x;
        long y = turnPts[i].ptCoord.y - m_ptRoationCenter.y;

        turnPts[i].ptCoord.x = m_ptRoationCenter.x + long(m_dbRotationMatrix[0][0]*x + m_dbRotationMatrix[0][1]*y);
        turnPts[i].ptCoord.y = m_ptRoationCenter.y + long(m_dbRotationMatrix[1][0]*x + m_dbRotationMatrix[1][1]*y);
    }


    int nLeftTopIndex = MatchLeftTopCorner(turnPts, m_ptRoationCenter, _countof(turnPts));
    if(nLeftTopIndex == -1)
    {
        return FALSE;
    }


    //
    m_vecBorderCalibMap.resize(BORDER_SQUARE_NUMBER);

    int nImagePtIndex = turnPts[nLeftTopIndex].nLayerPtIndex;


    //
    for(size_t nScreenPtIndex=0; nScreenPtIndex < BORDER_SQUARE_NUMBER; nScreenPtIndex++)
    {
     
        m_vecBorderCalibMap[nScreenPtIndex].ptImageCoord = m_vecBorderCalibratePts[nImagePtIndex];
        
        const RECT rcSquare = m_vecBorderSquarePositions[nScreenPtIndex];
        
        

        m_vecBorderCalibMap[nScreenPtIndex].ptScreenCoord.x   = (rcSquare.left + rcSquare.right )/2;
        m_vecBorderCalibMap[nScreenPtIndex].ptScreenCoord.y   = (rcSquare.top  + rcSquare.bottom)/2;
    }



    //
    if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
    {
        Debug_SaveImageFrame(m_oScreenMaskFrame, _T("FullScrenMask.bmp"));
    }


   //初始化校正图案
    SIZE patternSize;
    patternSize.cx = this->m_uCx;
    patternSize.cy = this->m_uCy;


    double dbRatio = 0.5;
    //计算上下部分面积之比的倒数
    //
    dbRatio = (double)m_oUpperHalfMaskFrame.Statistic(0xFF)/(double)m_oLowerHalfMaskFrame.Statistic(0xFF);
    //

    //初始化校正图案
    m_oCalibratePattern.InitPattern(this->m_ePattern, patternSize, dbRatio);


    //设置图像中的校正点数组尺寸
    m_vecInteriorCalibPoint.resize(this->m_oCalibratePattern.GetCalibrateMarkCount());

    //设置校正点图像坐标到屏幕坐标的映射数组的尺寸
    m_vecInteriorCalibMap.resize(this->m_oCalibratePattern.GetCalibrateMarkCount());


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
// ↑y
// |
//―+―→x
// |0
//查找位于左上角的角点, 该角点的特征是处于2,3象限, 且与X轴正向夹角
//最小
int CAutoCalibratorImpl2::MatchLeftTopCorner(const TurnPoint* turnPoints, POINT& ptG, int nTurnPointNumber)
{
    int iMatchIndex = -1;

    //注意到Tangent函数在[1/2π, 3/2π]之间单调递增,我们需要选取在[[1/2π, 3/2π]之间
    //Tangent值最小的角点
    double dbMinTan = (std::numeric_limits<double>::max)();

    for(int i=0; i<nTurnPointNumber; i++)
    {
        int nX, nY;

        nX = turnPoints[i].ptCoord.x - ptG.x;
        nY = -(turnPoints[i].ptCoord.y - ptG.y);//取负,让垂直向上为正方向

        if(nX>=0) continue;


        if(iMatchIndex == -1)
        {
            if(nX == 0)
            {
                if(nY> 0)
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
                dbMinTan = (double)nY/(double)nX;
            }

            iMatchIndex = i;
        }
        else
        {
            if(nX == 0)
            {
                if(nY>0)
                {
                    dbMinTan = (std::numeric_limits<double>::min)();
                    iMatchIndex = i;
                }
            }
            else
            {
                double dbTan = (double)nY/(double)nX;
                if(dbMinTan > dbTan)
                {
                    dbMinTan = dbTan;
                    iMatchIndex = i;
                }
            }
        }
    }

    return iMatchIndex;
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
    for(int i=0; i < nPtNumber; i++)
    {
        int iPrevIndex, iNextIndex;

        if(i == 0)
        {
            iPrevIndex = nPtNumber - 1;
        }
        else
        {
            iPrevIndex = i-1;
        }

        if(i == nPtNumber - 1)
        {
            iNextIndex = 0 ;
        }
        else
        {
            iNextIndex = i + 1;
        }

        POINT ptCurrent = pPts[i];
        POINT ptPrev    = pPts[iPrevIndex];
        POINT ptNext    = pPts[iNextIndex];


        long v1_dx, v1_dy, v2_dx, v2_dy;
        v1_dx = ptCurrent.x - ptPrev.x ;
        v1_dy = ptCurrent.y - ptPrev.y;


        v2_dx = ptNext.x - ptCurrent.x;
        v2_dy = ptNext.y - ptCurrent.y;


        //计算矢量的叉积
        long cross_product = v1_dx * v2_dy - v1_dy * v2_dx;

        //计算矢量的点积
        long dot_product = v1_dx*v2_dx + v1_dy*v2_dy;

        //V1×V2 = |V1||V2|sinθ
        //=>sinθ^2 = (V1×V2)^2 /[(|V1||V2|)^2]
        //注意此处防止整型计算溢出错误,2013/04/11
        long v1_length_sqare = v1_dx * v1_dx + v1_dy * v1_dy;
        long v2_length_sqare = v2_dx * v2_dx + v2_dy * v2_dy;


        double sin_v1_v2_square = (double(cross_product)/double(v1_length_sqare))*(double(cross_product)/double(v2_length_sqare));
        const double sin_30_square = 0.5*0.5;

        //当矢量夹角在[330, 360]和[0,30]之间时, 则认为是在边上的校正点,不是角上的点。
        if( dot_product > 0 //保证在第一、四象限
            &&
            sin_v1_v2_square < sin_30_square)
        {
            //为在边上的校正点
        }
        else
        {
            if(nTurnPointCount < nExpectedNum)
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

//@功能:给自动校正器投喂图片，并处理之
//@返回值:异常返回FALSE
BOOL CAutoCalibratorImpl2::FeedImage(const CYUY2Frame* pYUVFrame)
{

    if(pYUVFrame == NULL) return FALSE;


    if(FALSE == this->m_bIsSimulatedCalibrating)
    {
        if(m_bRecordVideo)////////根据设置保存视频
        {
            m_oAVIRecorder.Write((const BYTE*)pYUVFrame->GetData(), pYUVFrame->Size());
        }
    }


    CImageFrame monoFrame;

    //Covert2MonochromeFrame(*pYUVFrame, this->m_eMonochromizeAlog, monoFrame);
    Covert2MonochromeFrame_Y(*pYUVFrame, monoFrame);

    if(this->m_bIsSimulatedCalibrating)
    {
        m_oDebugWindow.PrintFrame(monoFrame);
    }


    BOOL bFailed = FALSE;


    ////static short s_BoardBrightness = 0;//屏幕白板亮度
    ////static short s_BInc            = 0;//屏幕亮度增量 inc=int[a/b] + float[a%b/b]
    ////static short s_BIncFraction    = 0;//屏幕亮度增量小数部分的整数表示，等于增量小数部分乘以除数，即扩大除数倍。
    ////static short s_BFractionCount  = 0;//屏幕亮度增量小数部分的整数表示的每步累加和。

    //static int  s_UpperPartPixelCount = 0;//上半部分像素个数
    //static int  s_LowerPartPixelCount = 0;//下半部分像素个数
    //static int  s_OuterMostMarkerIndex = 0;//


    switch(m_eCalibrateStage)
    {
    case E_AUTO_CALIBRATE_START://开始校正
        if(FALSE == this->m_bIsSimulatedCalibrating)
        {
            if(m_fpChangeCameraParams)
            {
                m_fpChangeCameraParams(E_CAMERA_PARAMS_CHANGE_SEARCH_SCREEN_AREA, m_lpCtx);
            }
        }

        InitBorderWhiteSquarePositions(m_AutoCalibrateWnd, HORZ_SIDE_SQUARE_NUMBER, VERT_SIDE_SQUARE_NUMBER, SQUARE_SIZE);//初始化
        
        
        //重置屏幕搜索子状态机
        ResetSubStateMachine_SearachScreenArea(monoFrame);

        
       //<<debug
        m_oDebugBorderSquareFrame.SetSize(monoFrame.Width(), monoFrame.Height(), 1);
        m_oDebugBorderSquareFrame.Clear();
      //debug>>

        m_nStageWaitCount = 0;
        //m_nStageTryTimes = 0;

        m_eCalibrateStage = E_SEARCH_SCREEN_UPPER_MASK_AREA;
        m_bCalibrateSequenceComplete = FALSE;
        break;


    case E_SEARCH_SCREEN_UPPER_MASK_AREA://搜索上部屏幕区域
        {
            ESubStateMachineStatus result = RunSubStateMachine_SearchScreenArea(monoFrame, E_SEARCH_SCREEN_UPPER);
            if(result == E_SUB_STATE_MACHINE_FINISHED)
            { 

                //重置屏幕搜索子状态机
                ResetSubStateMachine_SearachScreenArea(monoFrame);



                //转入搜索下半部屏幕区域阶段
                m_eCalibrateStage =E_SEARCH_SCREEN_LOWER_MASK_AREA;


            }
            else if(result == E_SUB_STATE_MACHINE_FAILED)//失败转退出
            {
                //成功的校正结果
                m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_DETECT_DISPLAY_AREA_FAILED;

                //转入校正结束阶段
                m_eCalibrateStage = E_AUTO_CALIBRATE_END;

            }
        }
        break;



    case E_SEARCH_SCREEN_LOWER_MASK_AREA://搜索下部屏幕区域
        {
            ESubStateMachineStatus result = RunSubStateMachine_SearchScreenArea(monoFrame, E_SEARCH_SCREEN_LOWER);
            if(result == E_SUB_STATE_MACHINE_FINISHED)
            { 

                BOOL bRet = OnPostSearchScreenArea(monoFrame.Width(), monoFrame.Height());
                if(!bRet)//失败了
                {
                     //成功的校正结果
                      m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_DETECT_DISPLAY_AREA_FAILED;

                     //转入校正结束阶段
                      m_eCalibrateStage = E_AUTO_CALIBRATE_END;
                      break;
                }


                if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
                {

                    //打印标记

                    Debug_SaveImageFrame(m_oDebugBorderSquareFrame, _T("BorderSqaureMatchResult.bmp"));
                }


                if(FALSE == this->m_bIsSimulatedCalibrating)
                {
                    FillBoard(m_AutoCalibrateWnd, 0, BLACK);
                }


                m_nStageWaitCount = 0;
                //转入校正棋盘格上部阶段
                m_eCalibrateStage = E_SEARCH_BACKGROUND;

            }
            else if(result == E_SUB_STATE_MACHINE_FAILED)//失败转退出
            {
                //成功的校正结果
                m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_DETECT_DISPLAY_AREA_FAILED;

                //转入校正结束阶段
                m_eCalibrateStage = E_AUTO_CALIBRATE_END;

            }
        }
        break;

    case E_SEARCH_BACKGROUND://搜索背景
        m_nStageWaitCount++;
         if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)
         {

             m_oBackgroundFrame = monoFrame;
             if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
             {
                 Debug_SaveImageFrame(m_oBackgroundFrame, _T("Background.bmp"));
             }

             if(FALSE == this->m_bIsSimulatedCalibrating)
                {
                    if(m_fpChangeCameraParams)
                    {
                        m_fpChangeCameraParams(E_CAMERA_PARAMS_CHANGET_PRINT_PATTERN, m_lpCtx);
                    }

                    //绘制上半部的校正图案
                    m_oCalibratePattern.DrawUpperPartPattern(m_AutoCalibrateWnd);
                }


            m_nStageWaitCount = 0;

            m_eCalibrateStage = E_CALIBRATE_UPPER_CHESSBOARD;//转入棋盘格校正

         }
         break;

    case E_CALIBRATE_UPPER_CHESSBOARD://校正棋盘格上部
        m_nStageWaitCount++;

        if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)
        {

            //m_oUpperSrcFrame = monoFrame;
            PreProcessPatternImage(monoFrame, m_oScreenMaskFrame, m_oBackgroundFrame, m_oUpperSrcFrame);

            
            int nAvgGridArea  = m_oUpperHalfMaskFrame.Statistic(0xFF)/this->m_oCalibratePattern.GetUpperGridCount();
            int nDetectRadius = int(sqrt(double(nAvgGridArea))/2.0);

            //校正上半部
            BOOL bRet = SubAreaCornerDetect(
                this->m_oCalibratePattern.GetUpperPartMarkCount(),
                this->m_oScreenMaskFrame,
                this->m_oUpperSrcFrame,
                m_dbRotationMatrix,
                m_ptRoationCenter,
                nDetectRadius,
                0);


            if(!bRet)//子区域校正失败
            {
                m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_CORNER_MATCH_FAILED;

                //转入校正结束阶段
                m_eCalibrateStage = E_AUTO_CALIBRATE_END;
                break;
            }

            if(FALSE == this->m_bIsSimulatedCalibrating)
            {
                //绘制上半部的校正图案
                m_oCalibratePattern.DrawLowerPartPattern(m_AutoCalibrateWnd);
            }


            m_nStageWaitCount = 0;
            m_eCalibrateStage = E_CALIBRATE_LOWER_CHESSBOARD;//转入棋盘校正


        }//if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)

        break;



    case E_CALIBRATE_LOWER_CHESSBOARD://校正棋盘格下部
        m_nStageWaitCount++;

        if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)
        {

            m_oLowerSrcFrame = monoFrame;
           // PreProcessPatternImage(monoFrame, this->m_oScreenMaskFrame, m_oBackgroundFrame, m_oLowerPatternFrame);

            //调试输出
            //if(this->m_bIsSimulatedCalibrating)
            //{
            //    m_oDebugWindow.PrintFrame(m_oLowerPatternFrame);
            //}

            int nAvgGridArea  = m_oLowerHalfMaskFrame.Statistic(0xFF)/this->m_oCalibratePattern.GetUpperGridCount();
            int nDetectRadius = int(sqrt(double(nAvgGridArea))/3.0);

            //校正下半部
            BOOL bRet = SubAreaCornerDetect(
                this->m_oCalibratePattern.GetLowerPartMarkCount(),
                this->m_oScreenMaskFrame,
                this->m_oLowerSrcFrame,
                m_dbRotationMatrix,
                m_ptRoationCenter,
                nDetectRadius,
                0);


            if(!bRet)//子区域校正失败
            {
                m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_CORNER_MATCH_FAILED;

                //转入校正结束阶段
                m_eCalibrateStage = E_AUTO_CALIBRATE_END;
                break;
            }



            //绘制旋转矢量
            //<<Debug
            CImageFrame outputFrame;
            outputFrame = m_oUpperSrcFrame;
            outputFrame.Merge(m_oLowerSrcFrame);
            outputFrame.Line(m_ptUpperCentroid, m_ptLowerCentroid, 255);
            outputFrame.DrawArrow(m_ptUpperCentroid, m_ptLowerCentroid, 30.0, 10, 255);

            CDib dibResult;
            LoadToDib(outputFrame, dibResult);

            for(std::vector<POINT>::size_type i=0; i<m_vecInteriorCalibPoint.size(); i++)
            {
                POINT pt = m_vecInteriorCalibPoint[i].ptImageCoord;
                dibResult.SetPixel(pt.x, pt.y, RGB(255,0,0));
            }

            CAtlString strFileName;
            strFileName.Format(_T("corner_match_result.bmp"));
            Debug_SaveDib(dibResult, strFileName);
            //

            std::vector<int> vecImage2ScreenMap;//"检测到的校正点"到"显示的校正点编号"的映射关系。

            //对所有校正点进行匹配
            BOOL bMatchSuccess = DoCornerMatch(m_vecInteriorCalibPoint, vecImage2ScreenMap);

            if(!bMatchSuccess)
            {
                //成功的校正结果
                m_eCalibrateError = E_AUTO_CALIBRATOR_ERROR_CORNER_MATCH_FAILED;

                //转入校正结束阶段
                m_eCalibrateStage = E_AUTO_CALIBRATE_END;
            }
            else
            {
                //输出校正结果
                for(std::vector<int>::size_type i = 0; i < vecImage2ScreenMap.size(); i++)
                {
                    if(vecImage2ScreenMap[i] != -1)
                    {
                        int nCornerIndex = vecImage2ScreenMap[i];

                        m_vecInteriorCalibMap[nCornerIndex].ptScreenCoord = this->m_oCalibratePattern.GetMarkCoord(nCornerIndex);

                        m_vecInteriorCalibMap[nCornerIndex].bIsMatched = TRUE;
                        m_vecInteriorCalibMap[nCornerIndex].ptImageCoord = this->m_vecInteriorCalibPoint[i].ptImageCoord;
                    }
                }


                if(this->m_bIsSimulatedCalibrating)
                {
                    std::vector<POINT> vecDbgData;
                    vecDbgData.resize(this->m_vecInteriorCalibPoint.size());
                    for(std::vector<POINT>::size_type i = 0;
                        i < vecDbgData.size();
                        i++)
                    {
                        vecDbgData[i] = m_vecInteriorCalibPoint[i].ptImageCoord;
                    }
                    this->m_oDebugWindow.PrintPtNo(vecDbgData, vecImage2ScreenMap);
                }

                //成功的校正结果
                m_eCalibrateError = E_AUTO_CALIBRATOR_OK;

                //转入校正结束阶段
                m_eCalibrateStage = E_AUTO_CALIBRATE_END;
            }//else


        }//if(m_nStageWaitCount >= MAX_STAGE_WAIT_COUNT)

        break;


    case E_AUTO_CALIBRATE_END:

        if(m_eCalibrateError == E_AUTO_CALIBRATOR_OK)
        {
            //完成校正,成功
            EndCalibrating();
            m_eCalibrateStage = E_AUTO_CALIBRATE_STOPPED;

        }
        else
        {
            if(m_nTryTimes == MAX_TRY_TIMES)
            {
                EndCalibrating();
                m_eCalibrateStage = E_AUTO_CALIBRATE_STOPPED;
            }
            else
            {
                //重新显示校正过程
                m_nTryTimes ++;//校正次数

                //设置图片输出目录
                CString strImageDubugDir;
                strImageDubugDir.Format(_T("%s\\%d"),m_strCliabrateDebugDir, m_nTryTimes);
                SetImageDebugDir(strImageDubugDir);

                m_eCalibrateStage = E_AUTO_CALIBRATE_START;
            }

        }
        break;

    case E_AUTO_CALIBRATE_STOPPED:

        return FALSE;

    }//switch

    return TRUE;

}


//@功能:返回正确的校正点数目
int CAutoCalibratorImpl2::GetValidCalibratePtNumber()const
{

    int nValidCorner = 0;

    for(int i=0; i < this->m_oCalibratePattern.GetCalibrateMarkCount(); i++)
    {
        if(m_aryAllCalibMap[i].bIsMatched)
        {
            nValidCorner++;
        }

    }//foreach(i)

    return nValidCorner;
}


//@功能:获得校正标记的坐标
//@参数:nIndex, 校正标记的索引编号
const TCalibCoordMap* CAutoCalibratorImpl2::GetCalibrateMarkerCoord(int nIndex) const
{
    const TCalibCoordMap* pCellCoord = NULL;

    int nSearchCount = 0;

    for(int i=0; i < this->m_oCalibratePattern.GetCalibrateMarkCount(); i++)
    {
        if(m_aryAllCalibMap[i].bIsMatched)
        {
            nSearchCount++;

            if(nSearchCount == nIndex+1)
            {
                pCellCoord = &m_aryAllCalibMap[i];
                break;
            }
        }

    }//foreach(i)

    return pCellCoord;
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
BOOL CAutoCalibratorImpl2::SubAreaCornerDetect(
    int nExpectedMarkerNumber,
    const CImageFrame& maskFrame,
    const CImageFrame& srcFrame,
    const double m[2][2],
    const POINT& ptCenter,
    int nDetectRadius,
    int nAreaNo)
{
    BOOL bRet = TRUE;

    //强鲁棒性角点检测
    //CRobustCornerDetector cornerDetector;

    //bRet = cornerDetector(srcFrame, maskFrame, nExpectedMarkerNumber, nDetectRadius, this->m_eDebugLevel, m_nTryTimes);

    CImageFrame rawImage = srcFrame;
    rawImage &= maskFrame;


    CFastCornerDetector fastCornerDetector;
     bRet = fastCornerDetector(
         rawImage.GetData(),
         rawImage.Width(),
         rawImage.Height(),
         nExpectedMarkerNumber, /*nDetectRadius,*/
         this->m_eDebugLevel, 
          m_nTryTimes);

    if(bRet)
    {
        //注意:检测器检测到的校正点坐标已经被放大2^INT_SCALE_SHIFT_SIZE倍。
        //
        //const std::vector<POINT>& vecGs = cornerDetector.GetCentroids();
        const std::vector<POINT>& vecGs = fastCornerDetector.GetResults();

        std::vector<TImageCalibPoint> cornerData;
        cornerData.resize(vecGs.size());

        for(std::vector<TImageCalibPoint>::size_type i = 0;
            i < vecGs.size(); i++)
        {
            cornerData[i].ptImageCoord = vecGs[i];

        }

        //
        // x |c11 c12 |
        // = | |
        // y |c21 c22 |

        //旋转矩阵系数
        double c11, c12, c21, c22;

        c11 = m[0][0];
        c12 = m[0][1];

        c21 = m[1][0];
        c22 = m[1][1];




        //进行旋转操作
        for(std::vector<TImageCalibPoint>::size_type i = 0;
            i < cornerData.size();
            i++)
        {
            const POINT& ptImageCoord = cornerData[i].ptImageCoord;
            POINT& ptRotatedCoord = cornerData[i].ptRotatedCoord;
            double dx = ptImageCoord.x - ptCenter.x;
            double dy = ptImageCoord.y - ptCenter.y;
            ptRotatedCoord.x = LONG(c11 * dx + c12 * dy);
            ptRotatedCoord.y = LONG(c21 * dx + c22 * dy);

            ptRotatedCoord.x += ptCenter.x;
            ptRotatedCoord.y += ptCenter.y;
        }


        for(std::vector<TImageCalibPoint>::size_type i = 0;
            i < cornerData.size();
            i++)
        {
            m_vecInteriorCalibPoint[m_vecInteriorCalibPtNumber++] = cornerData[i];
        }

        //<<debug
        if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
        {

            CImageFrame rotateCornersFrame(srcFrame.Width(), srcFrame.Height());

            for(std::vector<TImageCalibPoint>::size_type i = 0;
                i < cornerData.size();
                i++)
            {
                //<<//<<modified by toxuke@gmail.com, 2013/04/11。
                POINT ptOutput;
                ptOutput.x = cornerData[i].ptRotatedCoord.x >> INT_SCALE_SHIFT_SIZE;
                ptOutput.y = cornerData[i].ptRotatedCoord.y >> INT_SCALE_SHIFT_SIZE;
                //>>

                rotateCornersFrame.SetPixel(ptOutput, 255);
            }

            CDib dib;
            LoadToDib(rotateCornersFrame, dib);
            dib.SetPixel(ptCenter.x>>INT_SCALE_SHIFT_SIZE, ptCenter.y>>INT_SCALE_SHIFT_SIZE, RGB(255, 0, 0));
            CAtlString strFileName;
            strFileName.Format(_T("corner_point_rotate_adjust_%d.bmp"),nAreaNo);
            Debug_SaveDib(dib, strFileName);
        }
        //debug>>

    }

    return bRet;
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

    for(std::vector<TVertex>::size_type i = 0;
        i < nCornerRemained;
        i++)
    {
        vecVertex[i].ptCoord = corners[i].ptRotatedCoord;
        vecVertex[i].nAryIndex = i;
        vecImage2ScreenMap[i] = - 1;
    }


    int nLayer = 0;//第一圈,从最外圈开始算起



    int nLayerCount = m_oCalibratePattern.GetLayerCount();
    //按圈、层处理
    for(nLayer = 0; nLayer < nLayerCount; nLayer++)
    {

        const TRectLayer& rectLayer = m_oCalibratePattern.GetLayer(nLayer);

        //层在水平方向, 校正标记的个数
        int LayerMarkerHorzNumber = rectLayer.m_nHorzMarkNumber;

        //层在垂直方向, 校正标记的个数
        int LayerMarkerVertNumber = rectLayer.m_nVertMarkNumber;

        //期望的最外圈校正点个数
        int nExpectdPtNumber = rectLayer.m_vecScreenMarkNo.size() ;


        //如果是一个圈层
        if(LayerMarkerHorzNumber >1 && LayerMarkerVertNumber > 1)
        {
            //Step 1. 对每个校正点, 找出距离其最近的至多16个邻接点。
            for(std::vector<TVertex>::size_type i = 0;
                i < nCornerRemained;
                i++)
            {
                TVertex& curVertex = vecVertex[i];

                POINT P0 = curVertex.ptCoord;

                //和除自身以外的所有校正点进行比较
                for(std::vector<TVertex>::size_type j = 0;
                    j < nCornerRemained;
                    j++)
                {

                    if(i == j) continue;//不与自己比较

                    const TVertex& cmpVertex = vecVertex[j];
                    //if(!cmpVertex.bValid) continue;

                    POINT P1 = cmpVertex.ptCoord;
                    double distance = (P1.x - P0.x)*(P1.x - P0.x) + (P1.y - P0.y)*(P1.y - P0.y);


                    //与当前找到的最近邻接点比较
                    for(int k = 0; k < _countof(curVertex.aryNeighbours); k ++)
                    {

                        //第一次找到第k个最近邻接点
                        if(!curVertex.aryNeighbours[k].bValid)
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
                            if(distance < distCmp)//比已找到的最近邻接点的距离还要小，则插入
                            {

                                //前面的单元往后移一个,保证是最小的邻接点
                                for(int l = _countof(curVertex.aryNeighbours) - 1; l > k; l--)
                                {
                                    curVertex.aryNeighbours[l] = curVertex.aryNeighbours[l-1];
                                }

                                curVertex.aryNeighbours[k].distance = distance;
                                curVertex.aryNeighbours[k].nIndex = j;

                                if(curVertex.nNeighbourNumber < _countof(curVertex.aryNeighbours))
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
            for(std::vector<TVertex>::size_type i = 0;
                i < nCornerRemained;
                i++)
            {
                TVertex& curVertex = vecVertex[i];

                curVertex.bIsOuterLayer = FALSE;//先清除外圈角点标志

                int candidateNumber = curVertex.nNeighbourNumber ;
                POINT p0 = curVertex.ptCoord;
                UINT uQuadrant = 0;

                //第i个校正点的MAX_NEIBHBOUR个"最近邻接矢量"
                TNeighbourVector nvs[MAX_NEIBHBOUR];
                memset(nvs, 0 ,sizeof(nvs));


                for(int j = 0; j < candidateNumber; j++)
                {
                    if( !curVertex.aryNeighbours[j].bValid) continue;

                    int index = curVertex.aryNeighbours[j].nIndex;//邻接点的编号
                    POINT p1 = vecVertex[index].ptCoord;

                    //第i个校正点的第j个邻接矢量
                    TNeighbourVector nv;
                    nv.lDx = p1.x - p0.x;
                    nv.lDy = p1.y - p0.y;
                    nv.nVertexIndex = index;


                    if(nv.lDx > 0 && nv.lDy > 0 )
                    {
                        nv.nQaudrant = 0;
                    }
                    else if(nv.lDx<0 && nv.lDy > 0)
                    {
                        nv.nQaudrant = 1;
                    }
                    else if(nv.lDx == 0 && nv.lDy > 0)
                    {
                        nv.nQaudrant = 1;
                    }
                    else if(nv.lDx <0 && nv.lDy < 0)
                    {
                        nv.nQaudrant = 2;
                    }
                    else if(nv.lDx == 0 && nv.lDy < 0)
                    {
                        nv.nQaudrant = 3;
                    }
                    else if(nv.lDy == 0 && nv.lDx > 0 )
                    {
                        nv.nQaudrant = 0;
                    }
                    else if(nv.lDy == 0 && nv.lDx < 0)
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
                    for(k = 0; k < j; k++)
                    {

                        //比较已有的矢量的角度,使得所有矢量按从小到大的方位角排列
                        TNeighbourVector nvtemp = nvs[k];

                        if(nv.nQaudrant < nvtemp.nQaudrant)
                        {
                            bInsertPosFound = TRUE;
                            break;
                        }
                        else if(nv.nQaudrant == nvtemp.nQaudrant)
                        {
                            LONG crossProduct = nvtemp.lDx * nv.lDy - nvtemp.lDy*nv.lDx ;
                            if(crossProduct < 0 )//与原矢量的夹角>0, 则矢量积>0
                            {
                                bInsertPosFound = TRUE;
                                break;
                            }

                        }//else

                    }//for(k)


                    //既有矢量往后挪动一个单元
                    if(bInsertPosFound)
                    {
                        for(int l=j;l >k; l--)
                        {
                            nvs[l] = nvs[l-1];
                        }
                        nvs[k] = nv;
                    }
                    else
                    {
                        nvs[j] = nv;//插入到最末
                    }
                }//for(j)

                //Step 3.对已按照方位角排列的"最近邻接矢量",判断像邻两个矢量之间的夹角是否大于90度,
                // 如果大于90度, 则认为是属于最外圈的校正点
                for(int j = 0;j < candidateNumber;j ++)
                {
                    TNeighbourVector v1;
                    TNeighbourVector v2;

                    if(j == candidateNumber - 1)
                    {
                        v1 = nvs[j];
                        v2 = nvs[0];
                    }
                    else
                    {
                        v1 = nvs[j];
                        v2 = nvs[j+1];

                    }

                    //矢量的×积
                    //v1×v2=|v1||v2|sin(v1^v2);
                    long crossProduct = v1.lDx * v2.lDy - v1.lDy * v2.lDx;

                    //矢量的.积
                    //V1.V2 = |v1||v2|cos(v1^v2);
                    long dotProduct = v1.lDx * v2.lDx + v1.lDy * v2.lDy;

                    long v1_length_sqare = v1.lDx * v1.lDx + v1.lDy * v1.lDy;
                    long v2_length_sqare = v2.lDx * v2.lDx + v2.lDy * v2.lDy;


                    //double cosV1_V2_square = double(dotProduct*dotProduct)/(double)(v1_length_sqare*v2_length_sqare);
                    double cosV1_V2_square = (double(dotProduct)/double(v1_length_sqare)) * (double(dotProduct)/double(v2_length_sqare));
                    double cos145_squre = 0.67101007166283436652204980734113;


                    if(crossProduct<0)//sin(v1^v2)<0, 说明夹角范围在180~360度之间,属于外圈校正点
                    {
                        curVertex.bIsOuterLayer = TRUE;
                        break;
                    }
                    else if(dotProduct < 0 && cosV1_V2_square > cos145_squre)//cos(v1^v2)<0, 说明范围在145~180度之间, 属于外圈校正点
                    {
                        curVertex.bIsOuterLayer = TRUE;
                        break;
                    }

                }//for(j)

            }//for(i)


            //统计找到的最外圈角点的个数
            int nOuterLayerPtNumber = 0;

            for(std::vector<TVertex>::size_type i = 0;
                i < nCornerRemained;
                i++)
            {
                TVertex& curVertex = vecVertex[i];

                if(curVertex.bIsOuterLayer)
                {
                    nOuterLayerPtNumber ++;
                }

            }//for_each(i)





            //调试输出找到的一圈校正点
            //<<debug
            if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
            {
                CAtlString strFileName;
                strFileName.Format(_T("Layer%02d_CornerPoints.bmp"), nLayer );


                //CImageFrame frameOutLayer(640, 480);
                CImageFrame frameOutLayer(m_oScreenMaskFrame.Width(), m_oScreenMaskFrame.Height());
                for(std::vector<TVertex>::size_type i = 0;
                    i < nCornerRemained;
                    i++)
                {
                    TVertex& curVertex = vecVertex[i];

                    if(!curVertex.bIsOuterLayer) continue;

                    POINT ptStart = curVertex.ptCoord;

                    ptStart.x >>= INT_SCALE_SHIFT_SIZE;
                    ptStart.y >>= INT_SCALE_SHIFT_SIZE;

                    frameOutLayer.SetPixel(ptStart, 255);
                }

                Debug_SaveImageFrame(frameOutLayer, strFileName);
            }
            //debug>>


            //圈检测失败
            if(nOuterLayerPtNumber != nExpectdPtNumber)
            {

                bMatchSuccess = FALSE;
                break;
            }


            //提取最外圈校正点到单独的一个数组中,减少后续处理的循环次数
            std::vector<TVertex> outlayer;
            outlayer.resize(nOuterLayerPtNumber);


            //校正点在"所有校正点的集合"中的索引号到"最外圈校正点集合"的索引映射
            std::vector<int> mapForLayerNeighbourIndex;
            mapForLayerNeighbourIndex.resize(nCornerRemained);

            for(unsigned int i=0; i < nCornerRemained; i++)
            {
                mapForLayerNeighbourIndex[i] = -1;//-1表示不存在映射对应关系
            }

            int nOutLayerPtIndex = 0;
            for(std::vector<TVertex>::size_type i = 0;
                i < nCornerRemained;
                i++)
            {
                TVertex& curVertex = vecVertex[i];

                if(curVertex.bIsOuterLayer)
                {
                    outlayer[nOutLayerPtIndex] = curVertex;

                    mapForLayerNeighbourIndex[i] = nOutLayerPtIndex;
                    nOutLayerPtIndex++;
                }

            }//for_each(i)


            //调整最外圈校正点"最近邻接点"的索引号为最外圈校正点数组中的索引号， 即只保留属于外圈的"最近邻接点", 以便能够方便地顺时针遍历外圈校正点
            for(unsigned int i=0; i< outlayer.size(); i++)
            {
                unsigned int nNeigbours = outlayer[i].nNeighbourNumber;
                TVertex& curVertex = outlayer[i];
                unsigned int nLayerNeigbourCount = 0;
                for(unsigned int j=0; j<nNeigbours; j++)
                {
                    int nOldNeighbourIndex = curVertex.aryNeighbours[j].nIndex;

                    if(mapForLayerNeighbourIndex[nOldNeighbourIndex] == -1)
                    {
                        curVertex.nNeighbourNumber -- ;
                        curVertex.aryNeighbours[j].bValid = FALSE;
                    }
                    else
                    {

                        //注意到始终有 j>= nLayerNeigbourCount
                        //将原来第j个元素往前挪到第nLayerNeigbourCount个元素处,
                        //这样确保最外圈上到校正点的所有最近相邻校正点的信息始终从数组aryNeighbours的开头存放。
                        if(j != nLayerNeigbourCount)
                        {
                            curVertex.aryNeighbours[nLayerNeigbourCount] = curVertex.aryNeighbours[j];
                            //作废元素j
                            curVertex.aryNeighbours[j].bValid = FALSE;
                        }

                        //新的邻接点的index指的是在外圈数组中的索引号
                        curVertex.aryNeighbours[nLayerNeigbourCount].nIndex = mapForLayerNeighbourIndex[nOldNeighbourIndex];
                        nLayerNeigbourCount++;
                    }

                }//fo(j)

                //assert(curVertex.nNeighbourNumber == 2);
            }//for(i)



            BOOL bLayerMatchSuccess = FALSE;

            //根据最外圈找到的校正点, 计算圈内重心
            POINT ptG;

            long lMx = 0;//
            long lMy = 0;//
            long lM = 0;//
            for(std::vector<TVertex>::size_type i = 0; i< outlayer.size(); i++)
            {
                lMx += outlayer[i].ptCoord.x;
                lMy += outlayer[i].ptCoord.y;
                lM ++;
            }//for(i)

            ptG.x = lMx/lM;//
            ptG.y = lMy/lM;//


            //圈内重心G为原点,
            //计算从重心G到圈上校正点的矢量的方位角,
            //将矢量按照方位角的大小排序
            //采用插入法排序
            std::vector<TNeighbourVector> layerVector;
            layerVector.resize(outlayer.size());

            for(std::vector<TVertex>::size_type i = 0; i< outlayer.size(); i++)
            {
                const POINT& ptVertex = outlayer[i].ptCoord;
                TNeighbourVector nv;
                nv.lDx = ptVertex.x - ptG.x;
                nv.lDy = ptVertex.y - ptG.y;
                nv.nVertexIndex = i;

                //计算当前矢量所属象限
                if(nv.lDx > 0 && nv.lDy > 0 )
                {
                    nv.nQaudrant = 0;
                }
                else if(nv.lDx<0 && nv.lDy > 0)
                {
                    nv.nQaudrant = 1;
                }
                else if(nv.lDx == 0 && nv.lDy > 0)
                {
                    nv.nQaudrant = 1;
                }
                else if(nv.lDx <0 && nv.lDy < 0)
                {
                    nv.nQaudrant = 2;
                }
                else if(nv.lDx == 0 && nv.lDy < 0)
                {
                    nv.nQaudrant = 3;
                }
                else if(nv.lDy == 0 && nv.lDx > 0 )
                {
                    nv.nQaudrant = 0;
                }
                else if(nv.lDy == 0 && nv.lDx < 0)
                {
                    nv.nQaudrant = 2;
                }
                else
                {
                    nv.nQaudrant = 3;
                }

                BOOL bInsertPosFound = FALSE;//插入位置找到标志

                unsigned int k = 0;

                //采用插入排序算法
                //查找插入位置
                for(k = 0; k < i; k++)
                {

                    //比较已有的矢量的角度,使得所有矢量按从小到大的方位角排列
                    TNeighbourVector nvCmp = layerVector[k];

                    if(nv.nQaudrant < nvCmp.nQaudrant)
                    {
                        bInsertPosFound = TRUE;
                        break;
                    }
                    else if(nv.nQaudrant == nvCmp.nQaudrant)//矢量方位在同一象限内
                    {
                        LONG crossProduct = nvCmp.lDx * nv.lDy - nvCmp.lDy*nv.lDx ;
                        if(crossProduct < 0 )//与原矢量的夹角>0, 则矢量积>0
                        {
                            bInsertPosFound = TRUE;
                            break;
                        }

                    }//else

                }//for(k)


                //需要插入,既有矢量往后挪动一个单元
                if(bInsertPosFound)
                {
                    for(unsigned int l=i;l >k; l--)
                    {
                        layerVector[l] = layerVector[l-1];
                    }

                    layerVector[k] = nv;
                }
                else
                {
                    layerVector[i] = nv;//插入到最末
                }


            }//for(i)


            //外圈校正点已经按方位有序排列了,现在需要从一圈校正点中找出四个角上的点。
            //在角上的校正点的特点是:前后相邻矢量的夹角>30度, 边上的校正点的相邻矢量的夹角小于30度。
            TurnPoint turns[4];
            int nTurnPointCount = 0;
            for(unsigned int i=0; i < layerVector.size(); i++)
            {
                int iPrevIndex, iNextIndex, iCurrentIndex;

                if(i == 0)
                {
                    iPrevIndex = layerVector[layerVector.size() - 1].nVertexIndex;
                }
                else
                {
                    iPrevIndex = layerVector[i - 1].nVertexIndex;
                }

                if(i == layerVector.size() - 1)
                {
                    iNextIndex = layerVector[0].nVertexIndex;
                }
                else
                {
                    iNextIndex = layerVector[i + 1].nVertexIndex;
                }

                iCurrentIndex = layerVector[i].nVertexIndex;


                POINT ptCurrent = outlayer[iCurrentIndex].ptCoord;
                POINT ptPrev = outlayer[iPrevIndex].ptCoord;
                POINT ptNext = outlayer[iNextIndex].ptCoord;


                long v1_dx, v1_dy, v2_dx, v2_dy;
                v1_dx = ptCurrent.x - ptPrev.x ;
                v1_dy = ptCurrent.y - ptPrev.y;

                ;

                v2_dx = ptNext.x - ptCurrent.x;
                v2_dy = ptNext.y - ptCurrent.y;

                //
                long cross_product = v1_dx * v2_dy - v1_dy * v2_dx;

                long dot_product = v1_dx*v2_dx + v1_dy*v2_dy;

                //V1×V2 = |V1||V2|sinθ
                //=>sinθ^2 = (V1×V2)^2 /[(|V1||V2|)^2]
                //注意此处防止整型计算溢出错误,2013/04/11
                long v1_length_sqare = v1_dx * v1_dx + v1_dy * v1_dy;
                long v2_length_sqare = v2_dx * v2_dx + v2_dy * v2_dy;

                //double sin_v1_v2_square = double(cross_product * cross_product)/double(v1_dx * v1_dx + v1_dy * v1_dy)/double(v2_dx * v2_dx + v2_dy * v2_dy);
                double sin_v1_v2_square = (double(cross_product)/double(v1_length_sqare))*(double(cross_product)/double(v2_length_sqare));
                const double sin_30_square = 0.5*0.5;

                //当矢量夹角在[330, 360]和[0,30]之间时, 则认为是在边上的校正点,不是角上的点。
                if( dot_product > 0 //保证在第一、四象限
                    &&
                    sin_v1_v2_square < sin_30_square)
                {
                    //为在边上的校正点
                }
                else
                {
                    if(nTurnPointCount < _countof(turns))
                    {
                        //防止在异常情况下内存访问越界
                        turns[nTurnPointCount].ptCoord = outlayer[iCurrentIndex].ptCoord;
                        turns[nTurnPointCount].nLayerPtIndex = iCurrentIndex;
                    }

                    nTurnPointCount++;
                }

            }//for(i)


            if(nTurnPointCount < 4 || nTurnPointCount > 4)
            {
                bMatchSuccess = FALSE;
                break;
            }


            int iTopLeftCornerPointIndex = MatchLeftTopCorner(turns, ptG, _countof(turns));


            if(iTopLeftCornerPointIndex == -1)
            {
                bMatchSuccess = FALSE;
                break;
            }

            int iStartMatchIndex = 0;


            for(unsigned int i = 0; i< layerVector.size(); i++)
            {

                if(turns[iTopLeftCornerPointIndex].nLayerPtIndex == layerVector[i].nVertexIndex)
                {
                    iStartMatchIndex = i;
                    bLayerMatchSuccess = TRUE;
                    break;
                }
            }//for

            if(!bLayerMatchSuccess)
            {
                bMatchSuccess = FALSE;
                break;
            }



            unsigned int nMatchCount = 0;
            int iMatchIndex = iStartMatchIndex;

            //校正点屏幕编号
            //int nScreenCheckerIndex = nLayserFirstCheckerScreenIndex;
            //nLayserFirstCheckerScreenIndex += (MarkerNumberEachRow + 1);


            //屏幕编号递增量
            //int nScreenCheckerIndexInc = 1;

            while(nMatchCount < layerVector.size())
            {
                const TVertex& v = outlayer[layerVector[iMatchIndex].nVertexIndex];


                vecImage2ScreenMap[v.nAryIndex] = rectLayer.m_vecScreenMarkNo[nMatchCount];//subarea.m_aryMarkScreenGlobalNo[nScreenCheckerIndex];


                iMatchIndex ++;

                if(iMatchIndex == layerVector.size())
                {
                    iMatchIndex = 0;
                }

                nMatchCount ++;
            }


            //剔除最外圈点为下一圈处理做准备
            std::vector<TVertex>::iterator it = vecVertex.begin();

            while(it != vecVertex.end())
            {
                if(it->bIsOuterLayer)
                {
                    it = vecVertex.erase(it);
                    //it->bValid = FALSE;
                    nCornerRemained --;
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


            //<<调试输出去除最外圈校正点后的校正点集合
            if(m_eDebugLevel == E_CALIB_DEBUG_LEVEL_VERBOSE)
            {
                //CImageFrame nextLayerFrame(640, 480);
                CImageFrame nextLayerFrame(m_oScreenMaskFrame.Width(), m_oScreenMaskFrame.Height());
                for(std::vector<TVertex>::size_type i = 0;
                    i < vecVertex.size();
                    i++)
                {
                    TVertex& curVertex = vecVertex[i];

                    //if(!curVertex.bIsOuterLayer) continue;

                    POINT ptImage = curVertex.ptCoord;
                    ptImage.x >>= INT_SCALE_SHIFT_SIZE;
                    ptImage.y >>= INT_SCALE_SHIFT_SIZE;

                    nextLayerFrame.SetPixel(ptImage, 255);


                    //for(unsigned int k=0; k < curVertex.nNeighbourNumber; k++)
                    //{
                    //    if(!curVertex.aryNeighbours[k].bValid) continue;
                    //    int nIndex = curVertex.aryNeighbours[k].nIndex;
                    //    const POINT& ptEnd = vecVertex[nIndex].ptCoord;
                    //    //frame.Line(ptStart, ptEnd, 255);
                    //    //m_oDebugWindow.PrintFrame(nextLayerFrame);
                    //}

                    //frame.Clear();


                }

                CAtlString strFileName;
                strFileName.Format(_T("Grid_Layer%02d_Excluded.bmp"), nLayer+1);
                Debug_SaveImageFrame(nextLayerFrame, strFileName);
            }
        }//if is circle layer
        else  if(LayerMarkerHorzNumber == 1)//整个层为一列
        {
            if(vecVertex.size() != nExpectdPtNumber)
            {

                bMatchSuccess = FALSE;
                break;
            }


            //按照从Y坐标小到大的顺序匹配
            //vecImage2ScreenMap
            std::vector<TVertex>::iterator it,itMatch;

            unsigned int nMatchCount = 0;

            do
            {
                itMatch = vecVertex.begin();
                for(it = itMatch + 1; it != vecVertex.end(); it++)
                {
                    if(itMatch->ptCoord.y > it->ptCoord.y)
                    {
                        itMatch = it;
                    }
                }
                vecImage2ScreenMap[itMatch->nAryIndex] = rectLayer.m_vecScreenMarkNo[nMatchCount];

                vecVertex.erase(itMatch);

            }while(vecVertex.size());

            bMatchSuccess = TRUE;

        }
        else if(LayerMarkerVertNumber == 1)//整个层为一行
        {
            if(vecVertex.size() != nExpectdPtNumber)
            {
                bMatchSuccess = FALSE;
                break;
            }


            ////按照从X坐标小到大的顺序匹配
            //vecImage2ScreenMap
            std::vector<TVertex>::iterator it,itMatch;

            unsigned int nMatchCount = 0;

            do
            {
                itMatch = vecVertex.begin();
                for(it = itMatch + 1; it != vecVertex.end(); it++)
                {
                    if(itMatch->ptCoord.x > it->ptCoord.x)
                    {
                        itMatch = it;
                    }
                }
                vecImage2ScreenMap[itMatch->nAryIndex] = rectLayer.m_vecScreenMarkNo[nMatchCount++];

                vecVertex.erase(itMatch);

            }while(vecVertex.size());

            bMatchSuccess = TRUE;
        }
    }//for_each(rectLayer)

    return bMatchSuccess;
}



//@功能:模拟校正
BOOL CAutoCalibratorImpl2::DoSimulateCalibrate(LPCTSTR lpszAVIFilePath, HWND hNotifyWnd,  UINT uCx, UINT uCy, E_AutoCalibratePattern eGridsNumber)
{
    if(this->m_bIsCalibrating)
    {
        this->EndCalibrating();
    }

    if(!m_oAVIInput.OpenFile(lpszAVIFilePath))
    {
        return FALSE;
    }


    this->m_hNotifyWnd = hNotifyWnd;


    m_uCx = uCx;
    m_uCy = uCy;

    SYSTEMTIME now;
    GetLocalTime(&now);
    m_strCliabrateDebugDir.Format(
        _T("%s\\AutoCalibrate\\%04d_%02d_%02d_%02d_%02d_%02d"),
        g_strIntermediatRootDirName,
        now.wYear,
        now.wMonth,
        now.wDay,
        now.wHour,
        now.wMinute,
        now.wSecond);


    m_nTryTimes = 0;

    //设置图片输出目录
    CString strImageDubugDir;
    strImageDubugDir.Format(_T("%s\\%d"),m_strCliabrateDebugDir, m_nTryTimes);
    SetImageDebugDir(strImageDubugDir);


    //m_eMonochromizeAlog = E_MONOCHROMIZE_ALOG_Y;//黑白化算法
    m_eCalibrateStage = E_AUTO_CALIBRATE_START;
    m_nStageWaitCount = 0;
    m_bIsSimulatedCalibrating = TRUE;
    m_bIsCalibrating = TRUE;

    this->m_ePattern = eGridsNumber;

    DWORD dwThreadId = 0;
    m_hSimulatedCalibrateThread = ::CreateThread(
        NULL,
        0,
        SimulatedCalibrateProc,
        (LPVOID)this,
        0,
        &dwThreadId);


    return TRUE;

}

