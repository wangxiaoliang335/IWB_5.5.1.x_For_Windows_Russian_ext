#include "stdafx.h"
//#include "headers.h"
#include "../inc/FillPolygon.h"
#include "..\MorphologyAlgo\inc\MorphologyAlgo.h"
#include <math.h>
#define YELLOW RGB(255,255,0)
#define RED RGB(255,0,0)
#define GREEN  RGB(0  ,255,0)  

#define CROSS_SIZE 20
#define BKGND_COLOR RGB(0,0,255)
#include <map>

CManualCalibrateWnd::CManualCalibrateWnd()
:
m_ePtCalibState(E_ALL_CALIB_DONE),
m_nLightSpotDetectionCount(0),
m_uPeriodicCheckTimerId(NULL),
m_dwCtxData(0),
m_bRestoreTaskbar(FALSE),
m_nImageWidth(640),
m_nImageHeight(480)
{
    m_nCurrentCalibratePos = 0;
}


CManualCalibrateWnd::~CManualCalibrateWnd()
{

    //DestroyWindow(m_hWnd);
    //UnregisterClass(m_szClassName,m_hInstacne);
}

//LRESULT  CManualCalibrateWnd::InternalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
LRESULT   CManualCalibrateWnd::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    if(uMsg == WM_ACTIVATEAPP)
    {
        AtlTrace("WM_ACTIVATEAPP,wParam = 0x%x\n",wParam);
        //int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
        //int nCyScreen = GetSystemMetrics(SM_CYSCREEN);
        //RECT rcWindow;
        //GetWindowRect(hWnd,&rcWindow);
        //
        //int nWidth = rcWindow.right - rcWindow.left;
        //int nHeight = rcWindow.bottom - rcWindow.top;

        //BOOL bFullScreen = (nWidth == nCxScreen) && (nHeight == nCyScreen); 
        //if(bFullScreen)
        //{
        //	if(wParam == 1)
        //	{
        //		SetWindowPos(
        //		 hWnd,
        //		HWND_TOPMOST, 
        //		0,
        //		0,
        //		0,
        //		0,
        //		SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
        //	}
        //	else 
        //	{
        //		

        //		//通过Atl+tab键切换到其他程序了,隐藏播放窗口
        //		SetWindowPos(
        //		 hWnd,
        //		HWND_TOP, 
        //		0,
        //		0,
        //		0,
        //		0,
        //		SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW);

        //	}
        //}

    }
    //出现了如下的奇怪情形
    //WM_ACTIVATEAPP,wParam = 0x0
    //WM_ACTIVATE,wParam = 0x1
    else if( uMsg == WM_ACTIVATE)
    {
        AtlTrace("WM_ACTIVATE,wParam = 0x%x\n",wParam);

    }
    else if(uMsg == WM_LBUTTONDBLCLK)
    {
        AtlTrace("WM_LBUTTONDBLCLK\n");
    }
    else if(uMsg == WM_ERASEBKGND)
    {	

        HDC hDC = (HDC)wParam;
        RECT rcClient;
        GetClientRect(hWnd, &rcClient);

        HBRUSH hBkBrush = ::CreateSolidBrush(BKGND_COLOR);
        FillRect(hDC, &rcClient,hBkBrush);

        DeleteObject(hBkBrush);

        return 1;
    }
    else if(uMsg == WM_PAINT)
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);

        for(int i = 0; i <= m_nCurrentCalibratePos; i++)
        {
            POINT ptCenter = m_vecCrossSymbol[i].ptCenter;
            ptCenter.x -= m_rcBoundary.left;
            ptCenter.y -= m_rcBoundary.top;


            DrawCarlibrationMark(
                ps.hdc, 
                ptCenter,
                m_vecCrossSymbol[i].bAdjusted?m_vecCrossSymbol[i].clrAdjustAfter : m_vecCrossSymbol[i].clrAdjustBefore,
                m_vecCrossSymbol[i].size);

        }

        RECT rcClient;
        GetClientRect(hWnd, &rcClient);

        EndPaint(hWnd, &ps);

    }
    else if(uMsg == WM_LBUTTONDOWN)
    {
		int ddd = 0;
    }	
    else  if(uMsg == WM_LBUTTONUP )
    {
		int  ii = 0;
    }
    else if(uMsg == WM_RBUTTONDOWN)
    {
        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);

        HMENU hMenu    = ::LoadMenu(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDR_MENU_CTXMENU));
        HMENU hCtxMenu = ::GetSubMenu(hMenu, 1);

        if(hCtxMenu)
        {

            TrackPopupMenu(
                hCtxMenu, 
                TPM_RIGHTALIGN|TPM_BOTTOMALIGN,
                pt.x + m_rcBoundary.left,
                pt.y + m_rcBoundary.top,
                0,
                m_hWnd,
                NULL);


        }
        DestroyMenu(hMenu);

    }
    else if(uMsg == WM_COMMAND)
    {
        if(wParam == ID_ADJUSTWNDMENU_EXIT_ADJUST)
        {
            if(AbortCalibrate())
            {
				
                SendMessage(m_hNotifyWnd, WM_FINISH_MAUNUAL_CALIBRATING, FALSE, m_dwCtxData);
            }
        }
    }
    else if(uMsg == WM_CLOSE)
    {
        if(AbortCalibrate())
        {
			
            SendMessage(m_hNotifyWnd, WM_FINISH_MAUNUAL_CALIBRATING, FALSE, m_dwCtxData);
        }
    }
    else if(uMsg == WM_MANUAL_CALIBRATE_DATA)
    {
        //校正数据输入
        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);

        TStateMachineInputStimulus stimulus;
        stimulus.eStimulusType = E_STIMULUS_TYPE_SPOT_DATA;
        stimulus.ptLightSpot   = pt;
        RunCalibStateMachineProc(stimulus);


    }
    else if(uMsg == WM_TIMER)//定时器
    {
        UINT uTimerId = (UINT)wParam;
        if(uTimerId == PERIODIC_CHECK_EVENT_ID)
        {
            TStateMachineInputStimulus stimulus;
            stimulus.eStimulusType = E_STIMULUS_TYPE_TIMER;
            stimulus.uTimerId      = uTimerId;

            RunCalibStateMachineProc(stimulus);
        }
    }
    else if(uMsg == WM_KEYDOWN)
    {
        if(wParam == VK_ESCAPE)
        {//ESC键退出
            if(AbortCalibrate())
            {
                SendMessage(m_hNotifyWnd, WM_FINISH_MAUNUAL_CALIBRATING, FALSE, m_dwCtxData);
            }
        }
    }

    return DefWindowProc(hWnd,uMsg,wParam,lParam);
}


//@功能:开始手动校正
//@参数:parameters, 手动校正相关参数
//@说明:全屏显示校正窗口,并计算校正点的坐标。
void CManualCalibrateWnd::StartCalibrate(const TManualCalibrateParameters& parameters,int ImageWidth ,int ImageHaight)
{
    m_calibrateParameters = parameters;
	m_nImageWidth = ImageWidth;
	m_nImageHeight = ImageHaight;

    this->SetNotifyWnd(m_calibrateParameters.hNotifyWnd);
    m_nCurMonitorId = 0;

    m_oAllCalibMap.resize(m_calibrateParameters.vecScreenInfos.size());

    for(size_t i = 0; i < m_calibrateParameters.vecScreenInfos.size(); i++)
    {
        m_oAllCalibMap[i].rcMonitor   = parameters.vecScreenInfos[i].rcArea;
    }

	//RECT rcBoundary;
	//rcBoundary.left   = 0;
	//rcBoundary.top    = 0;
	//rcBoundary.right  = 0;
	//rcBoundary.bottom = 0;
	for (size_t i = 0; i < m_calibrateParameters.vecScreenInfos.size(); i++)
	{
		RECT rcArea = parameters.vecScreenInfos[i].rcArea;
        if (0 == i)
        {
            m_rcBoundary = rcArea;
        }
        else
        {
            if (rcArea.left < m_rcBoundary.left) m_rcBoundary.left = rcArea.left;
            if (rcArea.right > m_rcBoundary.right) m_rcBoundary.right = rcArea.right;
            if (rcArea.top < m_rcBoundary.top) m_rcBoundary.top = rcArea.top;
            if (rcArea.bottom > m_rcBoundary.top) m_rcBoundary.bottom = rcArea.bottom;
        }
	}
	

    InitMonitorCalibrateSymCoord();

    //FullScreen(TRUE);
	Create();
	SetWindowPos(
		m_hWnd,
		HWND_TOPMOST,
		//HWND_TOP, 
        m_rcBoundary.left,
        m_rcBoundary.top,
        m_rcBoundary.right,
        m_rcBoundary.bottom,
		SWP_SHOWWINDOW);
	SetFocus(m_hWnd);


    InvalidateRect(m_hWnd, NULL, TRUE);

    if(m_hNotifyWnd)
    {
        EnableWindow(m_hNotifyWnd, FALSE);
    }

    m_nCurrentCalibratePos = E_PT_LEFT_TOP;
    m_ePtCalibState = E_CURRENT_POINT_BEGIN_CALIBRATING;
    m_nLightSpotDetectionCount = 0;

    //开启检查定时器
    m_uPeriodicCheckTimerId = SetTimer(m_hWnd, PERIODIC_CHECK_EVENT_ID, PERIODIC_CHECK_INTERVAL, NULL);

    //初始化屏幕屏蔽图，整个屏幕图全部遮蔽，无屏幕区域
    BYTE cInitValue = 0x00;
	if (ImageWidth != 0 && ImageHaight != 0) {
	     m_oScreenAreaMask.SetSize(ImageWidth, ImageHaight, 1, &cInitValue);
	}
	else {	
       m_oScreenAreaMask.SetSize(DEFAULT_VIDEO_WIDTH, DEFAULT_VIDEO_HEIGHT, 1, &cInitValue);
	}

	if(IsTaskBarVisible())
	{
		ShowTaskBar(FALSE);
		m_bRestoreTaskbar = TRUE;
	}
	else
	{
		m_bRestoreTaskbar = FALSE;
	}

	m_dwCtxData = (this->m_calibrateParameters.nCalibratePointsInRow & 0xFF) | (this->m_calibrateParameters.nCalibratePointsInCol << 8);
}


//@功能:中断校正
BOOL CManualCalibrateWnd::AbortCalibrate()
{
    if(MessageBox(m_hWnd, g_oResStr[IDS_STRING104], g_oResStr[IDS_STRING105], MB_ICONINFORMATION|MB_OKCANCEL) == IDOK)
    {
        //FullScreen(FALSE);
		ShowWindow(m_hWnd, SW_HIDE);

        if(m_hNotifyWnd) EnableWindow(m_hNotifyWnd, TRUE);

        KillTimer(this->m_hWnd,PERIODIC_CHECK_EVENT_ID);
        m_uPeriodicCheckTimerId = NULL;

		//恢复任务栏
		if(m_bRestoreTaskbar)
		{
			ShowTaskBar(TRUE);
		}
        return TRUE;
    }
    else
    {
        m_nCurrentCalibratePos = E_PT_LEFT_TOP;
        InitMonitorCalibrateSymCoord();
        InvalidateRect(m_hWnd, NULL, TRUE);
		SetFocus(this->m_hWnd);
    }
	
    return FALSE;

}

//@功能:绘制校正十字符号
//@参数:hDC,
//      ptCenter, 十字架中心的坐标
//      color, 十字的颜色
//      size, 十字的长宽和高
void CManualCalibrateWnd::DrawCarlibrationMark(HDC hDC, const POINT& ptCenter, COLORREF color,  const SIZE& size)
{
    const int nLineWidth = 2;

    HPEN hPen = ::CreatePen(PS_SOLID, nLineWidth, color);
    HPEN hPenOld = (HPEN)::SelectObject(hDC, hPen);


    Ellipse(hDC,
        ptCenter.x - (size.cx >> 1),
        ptCenter.y - (size.cy >> 1),
        ptCenter.x + (size.cx >> 1),
        ptCenter.y + (size.cy >> 1));

    POINT ptStart,ptEnd;

    //水平线
    ptStart.x   = ptCenter.x - (size.cx >> 1);
    ptEnd  .x   = ptCenter.x + (size.cx >> 1);
    ptStart.y   = ptEnd.y = ptCenter.y;
    MoveToEx(hDC, ptStart.x, ptStart.y, NULL);
    LineTo  (hDC, ptEnd  .x, ptEnd  .y);

    //垂直线
    ptStart.x   = ptEnd.x = ptCenter.x;
    ptStart.y   = ptCenter.y - (size.cy >> 1);
    ptEnd.y     = ptCenter.y + (size.cy >> 1);

    MoveToEx(hDC, ptStart.x, ptStart.y, NULL);
    LineTo  (hDC, ptEnd  .x, ptEnd  .y);


    ::SelectObject(hDC, hPenOld);
    ::DeleteObject(hPen);

}


//@功能:初始化校正点坐标
//@参数:parameters, 手动校正参数
void  CManualCalibrateWnd::InitMonitorCalibrateSymCoord()
{
    switch (m_calibrateParameters.eManualCalibrateType)
    {
    case E_MUNUAL_CALIB_FULL_SCREEN:
        InitRectCalibrateSymCoord();
        break;

    case E_MUNUAL_CALIB_CIRCLE:
        InitCircleCalibrateSymCoord();
        break;
    }
}


void  CManualCalibrateWnd::InitRectCalibrateSymCoord()
{
    int nCalibratePointsInRow = this->m_calibrateParameters.nCalibratePointsInRow;
    int nCalibratePointsInCol = this->m_calibrateParameters.nCalibratePointsInCol;
    int nCalibratePointsNum   = nCalibratePointsInRow * nCalibratePointsInCol;

    m_oAllCalibMap[m_nCurMonitorId].calibData.resize(nCalibratePointsNum);

    m_vecCrossSymbol.resize(nCalibratePointsNum);

    const RECT& rcMonitor = this->m_calibrateParameters.vecScreenInfos[m_nCurMonitorId].rcArea;

    int nMonitorWidth  = rcMonitor.right  - rcMonitor.left;
    int nMonitorHeight = rcMonitor.bottom - rcMonitor.top ;

    int crossSymbolIndex = 0;
    int nX = 0;
    int nY = 0;

    BOOL bEvenNumber = (nCalibratePointsInRow % 2) == 0;

    for(int col=0; col < nCalibratePointsInCol; col++)
    {
        nX = rcMonitor.left + CROSS_SIZE/2 + (nMonitorWidth - CROSS_SIZE)*(col)/(nCalibratePointsInCol -1);

        for(int row=0; row < nCalibratePointsInRow; row++)
        {
            TCrossSymbol& cross = m_vecCrossSymbol[crossSymbolIndex];
            cross.clrAdjustBefore = RED;
            cross.clrAdjustAfter  = GREEN ;
            cross.bAdjusted       = FALSE ;
            cross.size.cx         = CROSS_SIZE;
            cross.size.cy         = CROSS_SIZE;

            nY = rcMonitor.top +  CROSS_SIZE/2 + (nMonitorHeight - CROSS_SIZE)*(row)/(nCalibratePointsInRow-1);


            if(row == nCalibratePointsInRow - 1)
            {//最后一行
                if(bEvenNumber)//一行中有偶数个校正点,最中间的2个校正点上移 1/3校正点垂直间隔
                {
                    if(col == (nCalibratePointsInRow>>1) - 1 || col == (nCalibratePointsInRow>>1))
                    {
                        nY -= (nMonitorHeight - CROSS_SIZE)/(nCalibratePointsInCol - 1)/3;
                    }
                }
                else//一行中有奇数个校正点, 最中间的校正点上移 1/3校正点垂直间隔
                {
                    if(col == (nCalibratePointsInRow >> 1))
                    {
                        nY -= (nMonitorHeight - CROSS_SIZE)/(nCalibratePointsInCol - 1)/3; 
                    }//col
                }
            }

            cross.ptCenter.x = nX;
            cross.ptCenter.y = nY;

            TCalibCoordPair& calibCoordPair = m_oAllCalibMap[m_nCurMonitorId].calibData[crossSymbolIndex];
            calibCoordPair.ptScreenCoord.x = nX;
            calibCoordPair.ptScreenCoord.y = nY;
            crossSymbolIndex++;

        }//for(col)

    }//for(row)

}

void  CManualCalibrateWnd::InitCircleCalibrateSymCoord()
{
    int nCalibratePointsInRow = this->m_calibrateParameters.nCalibratePointsInRow;
    int nCalibratePointsInCol = this->m_calibrateParameters.nCalibratePointsInCol;
    int nCalibratePointsNum = nCalibratePointsInRow * nCalibratePointsInCol;

    m_oAllCalibMap[m_nCurMonitorId].calibData.resize(nCalibratePointsNum);

    m_vecCrossSymbol.resize(nCalibratePointsNum);

    LOG_INF("nCalibratePointsNum=%d\r\n", nCalibratePointsNum);

    const RECT& rcMonitor = this->m_calibrateParameters.vecScreenInfos[m_nCurMonitorId].rcArea;

    int nMonitorWidth = rcMonitor.right - rcMonitor.left;
    int nMonitorHeight = rcMonitor.bottom - rcMonitor.top;

    int crossSymbolIndex = 0;
    int nX = 0;
    int nY = 0;

    const int LAYER_COUNT = 4;
    int layersNumbers[LAYER_COUNT] = { 1, 4, 8, 12 };

    int ptCenterX = nMonitorWidth >> 1;
    int ptCenterY = nMonitorHeight >> 1;
    int radius = min(nMonitorWidth, nMonitorHeight) >> 1;

    int radius_interval = (radius - CROSS_SIZE) / (LAYER_COUNT - 1);
    int r = 0;

    int xScaleSize = g_tSysCfgData.globalSettings.circleCalibrateXScale;
    int yScaleSize = g_tSysCfgData.globalSettings.circleCalibrateYScale;

    LOG_INF("xScaleSize=%d, yScaleSize=%d", xScaleSize, yScaleSize);
    for (int layer = 0; layer < _countof(layersNumbers); layer++)
    {
        double theta_step = (M_PI*2)/layersNumbers[layer];
        double theta = 0.0;
        for (int i = 0; i < layersNumbers[layer]; i++)
        {

            int nX = ptCenterX + r*cos(theta) * xScaleSize /100.0;
            int nY = ptCenterY + r*sin(theta) * yScaleSize /100.0;

            TCrossSymbol& cross = m_vecCrossSymbol[crossSymbolIndex];
            cross.clrAdjustBefore = RED;
            cross.clrAdjustAfter = GREEN;
            cross.bAdjusted = FALSE;
            cross.size.cx = CROSS_SIZE;
            cross.size.cy = CROSS_SIZE;

            cross.ptCenter.x = nX;
            cross.ptCenter.y = nY;

            LOG_INF("layer=%d, i=%d, crossSymbolIndex=%d, x=%d, y=%d", layer, i, crossSymbolIndex,nX, nY);
            TCalibCoordPair& calibCoordPair = m_oAllCalibMap[m_nCurMonitorId].calibData[crossSymbolIndex];
            calibCoordPair.ptScreenCoord.x = nX;
            calibCoordPair.ptScreenCoord.y = nY;

            crossSymbolIndex++;

            theta += theta_step;
        }

        r += radius_interval;

    }//for
    
}


//@功能:状态机过程函数
//@参数:stimulus:输入的激励
void CManualCalibrateWnd::RunCalibStateMachineProc(const TStateMachineInputStimulus& stimulus)
{
    switch(m_ePtCalibState)
    {
       case E_CURRENT_POINT_BEGIN_CALIBRATING://校正点坐标开始采集
           if(E_STIMULUS_TYPE_SPOT_DATA ==  stimulus.eStimulusType)
           {
               m_ptFirstInputImageCoord   = stimulus.ptLightSpot;
               m_nLightSpotDetectionCount = 0;
               m_ePtCalibState = E_CURRENT_POINT_CALIBRATING;
           }
           break;

      case E_CURRENT_POINT_CALIBRATING://校正点坐标采集中

          if(E_STIMULUS_TYPE_SPOT_DATA ==  stimulus.eStimulusType)
          {
              const POINT& ptInput = stimulus.ptLightSpot;

              int nDeltaU = ptInput.x - m_ptFirstInputImageCoord.x ;
              int nDeltaV = ptInput.y - m_ptFirstInputImageCoord.y ;

              //AtlTrace(_T("x=%d, y=%d, first_x=%d, first_y=%d,ptnDeltaU=%d, nDeltaV=%d, u_deviation=%d, v_deivation=%d \n"), 
              //    ptInput.x,
              //    ptInput.y,
              //    m_ptFirstInputImageCoord.x,
              //    m_ptFirstInputImageCoord.y,
              //    nDeltaU,
              //    nDeltaV,
              //    MAX_PERMIT_U_DITHER_DEVIATION,
              //    MAX_PERMIT_V_DITHER_DEVIATION);

              if(abs(nDeltaU) < MAX_PERMIT_U_DITHER_DEVIATION && abs(nDeltaV) < MAX_PERMIT_V_DITHER_DEVIATION)
              {
                  m_nLightSpotDetectionCount ++;

                  if(m_nLightSpotDetectionCount >= MAX_DETECT_COUNT)
                  {
                      //记录采集到的校正点图像坐标
                      TPoint2D pt2dInput;
                      pt2dInput.d[0] = (double)ptInput.x/(double(1 << INT_SCALE_SHIFT_SIZE));
                      pt2dInput.d[1] = (double)ptInput.y/(double(1 << INT_SCALE_SHIFT_SIZE));

                      m_oAllCalibMap[m_nCurMonitorId].calibData[m_nCurrentCalibratePos].pt2DImageCoord = pt2dInput;


                      m_vecCrossSymbol[m_nCurrentCalibratePos].bAdjusted = TRUE;

                      //重绘校正标记(采集完坐标的校正点颜色会发生变化)
                      InvalidateRect(m_hWnd, NULL, TRUE);


                      //移动鼠标指针到完成坐标采集的校正符号处
                      POINT ptCrossSymbol;
                      ptCrossSymbol = m_vecCrossSymbol[m_nCurrentCalibratePos].ptCenter;
                      SetCursorPos(ptCrossSymbol.x, ptCrossSymbol.y);


                      //记录光斑最后显示的时刻
                      m_dwLastSpotAppearTime = GetTickCount();

                      //进入"等待校正坐标采集结束状态"
                      m_ePtCalibState = E_WAIT_CURRENT_POINT_CALIBRATING_END;
                 }
             }
             else
             {    //超过了允许的抖动引起的偏移量,重新开始采样。
                  m_ePtCalibState = E_CURRENT_POINT_BEGIN_CALIBRATING;
             }
          }
          break;

    case E_WAIT_CURRENT_POINT_CALIBRATING_END://校正点坐标采集结束
        {
            BOOL bEnd = FALSE;

            if(E_STIMULUS_TYPE_SPOT_DATA ==  stimulus.eStimulusType)
            {//输入光斑数据
                const POINT& ptInput = stimulus.ptLightSpot;
                int nDeltaU = ptInput.x - m_ptFirstInputImageCoord.x ;
                int nDeltaV = ptInput.y - m_ptFirstInputImageCoord.y ;

                //if(abs(nDeltaU) > MIN_CALIB_PT_DISTANCE || abs(nDeltaV) > MIN_CALIB_PT_DISTANCE)
                //{//光斑位置发生大移动, 可以认为坐标采样结束
                //    bEnd = TRUE;
                //}
                //更新光斑最后显示的时刻
                m_dwLastSpotAppearTime = GetTickCount();

            }
            else if(E_STIMULUS_TYPE_TIMER == stimulus.eStimulusType)
            {//定时检查时间到
                DWORD dwNow = GetTickCount();
                DWORD dwElapse = dwNow - m_dwLastSpotAppearTime;
                if(dwElapse > CALIB_END_TIME_THRESHOLD)
                {//长时间没有光斑输入数据, 可以认为坐标采样结束
                    bEnd = TRUE;
                }

            }

            if(bEnd)
            {//当前校正点完成了

                m_nCurrentCalibratePos ++;//校正点编号增加1

                int nCalibratePointsInRow = this->m_calibrateParameters.nCalibratePointsInRow;
                int nCalibratePointsInCol = this->m_calibrateParameters.nCalibratePointsInCol;
                int nCalibratePointsTotalNum = nCalibratePointsInRow * nCalibratePointsInCol;

                if(m_nCurrentCalibratePos  == nCalibratePointsTotalNum )
                {
                    //计算屏幕图像在视频中的轮廓图像
                    UpdateScreenAreaMask();//更新屏幕屏蔽区域


                    this->m_nCurMonitorId ++;

                    if(this->m_nCurMonitorId  ==  this->m_calibrateParameters.vecScreenInfos.size())
                    {   //所有屏幕已经校正完毕了，则结束退出
                        if(m_hNotifyWnd)
                        {
							PostMessage(m_hNotifyWnd, WM_FINISH_MAUNUAL_CALIBRATING, TRUE, m_dwCtxData);

							//恢复任务栏
							if(m_bRestoreTaskbar)
							{	
								ShowTaskBar(TRUE);
							}

                            //FullScreen(FALSE);
							ShowWindow(m_hWnd, SW_HIDE);

                            if(m_hNotifyWnd) EnableWindow(m_hNotifyWnd, TRUE);

                            m_ePtCalibState = E_ALL_CALIB_DONE;

                            KillTimer(this->m_hWnd, PERIODIC_CHECK_EVENT_ID);
                            m_uPeriodicCheckTimerId = NULL;

                        }
                    }
                    else
                    {
                        //继续手动校正下一个屏幕
                        InitMonitorCalibrateSymCoord();
                        m_nCurrentCalibratePos = E_PT_LEFT_TOP;
                        m_nLightSpotDetectionCount = 0;
                        m_ePtCalibState = E_CURRENT_POINT_BEGIN_CALIBRATING;
                    }
                }
                else
                {
                    m_ePtCalibState = E_CURRENT_POINT_BEGIN_CALIBRATING;
                }

                InvalidateRect(m_hWnd, NULL, TRUE);

            }//if(bEnd)


        }//case
        break;

    default:
        //do nothing
        break;
        //
    }//switch
}


//@功能:设备丢失事件响应函数
//
void CManualCalibrateWnd::OnDeviceMissing()
{
 //   MessageBox(this->m_hWnd, g_oResStr[IDS_STRING454], g_oResStr[IDS_STRING109], MB_ICONERROR|MB_OK);

    //FullScreen(FALSE);
	ShowWindow(m_hWnd, SW_HIDE);

    if(m_hNotifyWnd) EnableWindow(m_hNotifyWnd, TRUE);

    KillTimer(this->m_hWnd,PERIODIC_CHECK_EVENT_ID);
    m_uPeriodicCheckTimerId = NULL;

	//恢复系统任务栏
	if (m_bRestoreTaskbar)
	{
		ShowTaskBar(TRUE);
	}
    SendMessage(m_hNotifyWnd, WM_FINISH_MAUNUAL_CALIBRATING, FALSE, m_dwCtxData);

}

//@功能:根据手动校正点勾勒的轮廓更新屏幕影像屏蔽图
//@参数:无
void CManualCalibrateWnd::UpdateScreenAreaMask()
{
    switch (m_calibrateParameters.eManualCalibrateType)
    {
    case E_MUNUAL_CALIB_FULL_SCREEN:
        UpdateRectScreenAreaMask();
        break;

    case E_MUNUAL_CALIB_CIRCLE:
        UpdateCircleScreenAreaMask();
        break;
    }
}

void CManualCalibrateWnd::UpdateRectScreenAreaMask()
{

    //已知校正按照列优先,从上往下手动采集光斑数据
    //按顺序提取边界处的采样点
    int nPtIndex     = 0;
    int nVertexIndex = 0;
    int nRowCount    = m_calibrateParameters.nCalibratePointsInCol;
    int nColCount    = m_calibrateParameters.nCalibratePointsInRow;

    int nLastRowNo   = nRowCount - 1;
    int nLastColNo   = nColCount - 1;
    const  std::vector<TCalibCoordPair>& calibData = m_oAllCalibMap[m_nCurMonitorId].calibData;



    std::vector<POINT> vertex;//屏幕遮蔽区多边形顶点坐标数组
    int nVertexCount = (nRowCount + nColCount - 2) << 1;//×2

    //最底边中间的校正点上移了,中间的校正点不能够用作屏蔽图的轮廓点，需要跳过
    BOOL bEvenNumber = ((nColCount % 2) == 0)?TRUE:FALSE;

    nVertexCount -= bEvenNumber?2 : 1;

    vertex.resize(nVertexCount);

   //按照顺时针方向提取作为边界点的校正点的索引
    std::vector<int> aryPtIndex;
    aryPtIndex.resize(nVertexCount);

    nPtIndex = 0;
   
    //提取最上边一行校正点索引号
    for(int col=0; col < nLastColNo; col++)
    {
        aryPtIndex[nVertexIndex] = nPtIndex;
        nPtIndex += nRowCount;
        nVertexIndex ++;
    }//for-each(col)

    //提取最左边一列校正点索引号
    for(int row = 0; row <  nLastRowNo; row++)
    {
        aryPtIndex[nVertexIndex] = nPtIndex;
        nPtIndex ++;
        nVertexIndex ++;
    }
    

    //提取最底边一行校正点索引号
    for(int col=nLastColNo; col > 0; col--)
    {
         int nColIndex = nPtIndex / nRowCount;

         BOOL bSkip = FALSE;
        if(bEvenNumber)
        {   
            if(nColIndex == (nColCount >> 1) || nColIndex == (nColCount >> 1)  -1)
            {//是最中间的两个点之一则跳过
                bSkip = TRUE;
            }

        }
        else
        {
            if(nColIndex == (nColCount >> 1))
            {//是最中间的点则跳过
                bSkip = TRUE;
            }

        }

        if(!bSkip)
        {
            aryPtIndex[nVertexIndex] = nPtIndex;
            nVertexIndex ++;
        }

        nPtIndex -= nRowCount;
        
    }

    //提取最右边一列校正点索引号
    for(int row = nLastRowNo; row > 0; row --)
    {
        aryPtIndex[nVertexIndex] = nPtIndex;
        nPtIndex --;
        nVertexIndex ++;
    }


    for(int i=0; i < nVertexCount; i++)
    {
        int nPtIndex = aryPtIndex[i];
        POINT ptImage;
        ptImage.x  = long(calibData[nPtIndex].pt2DImageCoord.d[0]);;
        ptImage.y  = long(calibData[nPtIndex].pt2DImageCoord.d[1]);
        
        vertex[i] = ptImage;
    }

    //在屏蔽图中填充多边形
    FillPolygon(
        m_oScreenAreaMask.GetData(),
        m_oScreenAreaMask.Width(),
        m_oScreenAreaMask.Height(),
        &vertex[0],
        nVertexCount,
        0xFF,
        TRUE);

	///modify zhaown 2019.7.22
   // SaveToBitmap(m_oScreenAreaMask, _T("c:\\ManualScreenMask.bmp"));

    //屏蔽图中的屏幕区域膨胀10个像素
    for(int i=0;i<8; i++)
    {
        Morph_Dilate8(m_oScreenAreaMask.GetData(),m_oScreenAreaMask.GetData(),m_oScreenAreaMask.Width(), m_oScreenAreaMask.Height());
    }

}


void CManualCalibrateWnd::UpdateCircleScreenAreaMask()
{
    const  std::vector<TCalibCoordPair>& calibData = m_oAllCalibMap[m_nCurMonitorId].calibData;

    std::vector<POINT> vertex;//屏幕遮蔽区多边形顶点坐标数组
    int nVertexCount = 12;
    vertex.resize(nVertexCount);

    int nPtIndex = 13;
    for (int i = 0; i < nVertexCount; i++)
    {
        POINT ptImage;
        ptImage.x = long(calibData[nPtIndex].pt2DImageCoord.d[0]);;
        ptImage.y = long(calibData[nPtIndex].pt2DImageCoord.d[1]);

        vertex[i] = ptImage;
        nPtIndex++;
    }

    //在屏蔽图中填充多边形
    FillPolygon(
        m_oScreenAreaMask.GetData(),
        m_oScreenAreaMask.Width(),
        m_oScreenAreaMask.Height(),
        &vertex[0],
        nVertexCount,
        0xFF,
        TRUE);

    //SaveToBitmap(m_oScreenAreaMask, _T("c:\\CircleManualScreenMask.bmp"));

    //屏蔽图中的屏幕区域膨胀10个像素
    for (int i = 0; i<8; i++)
    {
        Morph_Dilate8(m_oScreenAreaMask.GetData(), m_oScreenAreaMask.GetData(), m_oScreenAreaMask.Width(), m_oScreenAreaMask.Height());
    }

}