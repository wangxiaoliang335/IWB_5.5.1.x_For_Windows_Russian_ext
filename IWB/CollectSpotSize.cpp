#include "StdAfx.h"
//#include "headers.h"
#define YELLOW RGB(255,255,0)
#define RED    RGB(255,0,0)
#define GREEN  RGB(0  ,255,0)
#define BKGND_COLOR RGB(0,0,255)
#define ROYAL_BLUE  RGB(65,105,225)

#define MARGIN_WIDTH 30  
#define SYMBOL_SIZE  20


//采集光斑尺寸时, 允许光斑水平方向抖动的距离占水平宽度的百分比
#define LIGHT_SPOT_DITHER_OFFSET_PERCENT_X 12

//采集光斑尺寸时, 允许光斑垂直方向抖动的距离占垂直高度的百分比
#define LIGHT_SPOT_DITHER_OFFSET_PERCENT_Y 12

//采样位置最视频中的最小间距
//#define SAMPLE_U_MINIMUM_INTERVAL 20
//#define SAMPLE_V_MINIMUM_INTERVAL 20

//#define SAMPLE_MINIMUM_RANGE2   (SAMPLE_U_MINIMUM_INTERVAL*SAMPLE_U_MINIMUM_INTERVAL +  SAMPLE_V_MINIMUM_INTERVAL * SAMPLE_V_MINIMUM_INTERVAL)

CCollectSpotSize * g_pCollectSpotSize = NULL;
//int CCollectSpotSize::s_nSpotSettingCount = 0;    //手动采集的参数
SpotManualCollectInfoManager CCollectSpotSize::m_oSpotManualCollectInfoManager;

CCollectSpotSize::CCollectSpotSize(E_COLLECTSPOT_MODE eCollectSpotMode, HWND hOwner)    //构造函数
:
m_szClassName(_T("PlayWnd Blob Setting ")),
m_bFullScreen(FALSE),
m_hWnd(NULL),
m_hOwner(NULL),
m_eSpotSamplingState(E_ALL_SPOT_SAMPLEING_END),
m_nSampleTimes(0),
m_nCurrentSampleNo(0),/*,
m_eSpotSamplingMode(eCollectSpotMode)*/
//m_eSpotSamplePattern(E_SAMPLE_COLLECT_PATTERN_9_Points),
m_nSymbolHorzInterval(50),
m_nSymbolVertInterval(50),
m_nSampleNumEachRow(0),
m_nSampleNumEachCol(0),
m_nSensorID(-1),
m_nSelectDragIndex(-1),
m_nCollectSensorCount(0)
{
    WNDCLASSEX wnd;
    wnd.cbSize = sizeof wnd;
    wnd.style = 0;
    wnd.lpfnWndProc = WinProc;
    wnd.cbClsExtra = 0;
    wnd.cbWndExtra = 0;
    m_hInstacne = wnd.hInstance = _AtlBaseModule.m_hInst;
    wnd.hIcon = 0;
    wnd.hCursor = 0;
    wnd.hbrBackground = 0;
    wnd.lpszMenuName = NULL;
    wnd.lpszClassName = m_szClassName;
    wnd.hIconSm = 0;
    ATOM atom = RegisterClassEx(&wnd);

    m_nCxVScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    m_nCyVScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	m_ShowFlag = TRUE;

    //if (theApp.GetDoubleScreenMerge())
    //{
    //	DoubleScreenMergeCount =  MAX_COLLECT_NUMBER_DOUBLE;
    //}
    //else
    //{
    //	DoubleScreenMergeCount = MAX_SAMPLE_NUMBER;
    //}


    //m_vecSampleSymbols.resize(DoubleScreenMergeCount);
    //m_vecStandardCross.resize(DoubleScreenMergeCount);
    //m_vecMaxSpot.resize(DoubleScreenMergeCount);

    //m_vecSampleSymbols.resize(MAX_SAMPLE_NUMBER);
    m_vecSampleData.resize   (MAX_SPOT_SAMPLING_TIMES);

    //InitSamplePosition();
    ASSERT(g_pCollectSpotSize == NULL);
    g_pCollectSpotSize =this;

    //m_ptLastSamplePointInScreen.x = 0;
    //m_ptLastSamplePointInScreen.y = 0;

    SIZE sizeScreen = ::GetActualScreenControlSize();
    OnDisplayChange(sizeScreen.cx, sizeScreen.cy);

}

CCollectSpotSize::~CCollectSpotSize(void)      //析构函数
{
    DestroyWindow(m_hWnd); 
    UnregisterClass(m_szClassName,m_hInstacne);
}

///窗体的创建
BOOL CCollectSpotSize::Create()
{
    m_hWnd = ::CreateWindowEx(
        WS_EX_TOOLWINDOW,
        m_szClassName,
        m_szClassName,
        WS_POPUP,
        0,0,0,0,
        NULL,
        0,
        _AtlBaseModule.m_hInst,
        0);
    
    GetWindowPlacement(m_hWnd,&m_OldWndPlacement);
    return (m_hWnd != NULL);
}

//窗体全屏化
void CCollectSpotSize::FullScreen(BOOL bFull)
{
    if (bFull)
    {
        GetWindowPlacement(m_hWnd,&m_OldWndPlacement);
        RECT rcPosition;
        rcPosition.left = 0;
        rcPosition.top  = 0;
        rcPosition.right = m_nCxVScreen;
        rcPosition.bottom= m_nCyVScreen;

        SetWindowPos(
            m_hWnd,
            HWND_TOPMOST,
            rcPosition.left,
            rcPosition.top,
            rcPosition.right,
            rcPosition.bottom,
            SWP_SHOWWINDOW);

        m_bFullScreen = TRUE;
        SetFocus(m_hWnd);
    }
    else
    {
        SetWindowPlacement(m_hWnd,&m_OldWndPlacement);
        ShowWindow(m_hWnd,SW_HIDE);

        m_bFullScreen = FALSE;
    }

}

HWND CCollectSpotSize::GetOwnerWnd() const
{
    return m_hOwner;
}

void CCollectSpotSize::SetOwnerWnd(HWND hWnd)
{
    m_hOwner = hWnd;
}

//参数:rcMonitor:显示器矩形区域坐标
void CCollectSpotSize::InitSamplePosition(const RECT& rcMonitor)
{

    int nSymbolIndex = 0;
    int nX = 0;
    int nY = 0;

    m_nCxVScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    m_nCyVScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    m_rcCurrentMonitor = rcMonitor;

    int nSampleCount = m_nSampleNumEachCol * m_nSampleNumEachRow;
    m_vecSampleSymbols.resize(nSampleCount);
	m_vecConfigCross.resize(nSampleCount);


    for(size_t i=0; i< m_ScreenLightspotSample.size(); i++)
    {
        m_ScreenLightspotSample[i].vecSampleSize.resize(nSampleCount);
    }
    
    int nMonitorWidth  = rcMonitor.right  - rcMonitor.left;
    int nMonitorHeight = rcMonitor.bottom - rcMonitor.top ;
	int nMonitorLeft   = rcMonitor.left;
	int nMonitorTop    = rcMonitor.top;
    int nTopMargin     = MARGIN_WIDTH    ;//第一行采样符号与上边界的距离
    int nBottomMargin  = MARGIN_WIDTH    ;//最后一行采样符号与下边界的距离
    int nLeftMargin    = MARGIN_WIDTH    ;//第一列采样符号与左边界的距离
    int nRightmMargin  = MARGIN_WIDTH    ;//最后一列采样符号与右边界的距离


    //采样符号水平间隔
    m_nSymbolHorzInterval = (nMonitorWidth - nLeftMargin - nRightmMargin)/(m_nSampleNumEachRow - 1);

    //采样符号的垂直间隔
    m_nSymbolVertInterval = ( nMonitorHeight - nTopMargin - nBottomMargin)/(m_nSampleNumEachCol - 1);


    //以列优先，从上到下排列采样点
    for(int col = 0; col < m_nSampleNumEachRow;col++)
    {
        nX = nMonitorLeft + nLeftMargin + (nMonitorWidth - nLeftMargin - nRightmMargin)*(col)/(m_nSampleNumEachRow - 1);

        for (int row = 0; row < m_nSampleNumEachCol;row++)
        {
            nY = nMonitorTop + nTopMargin + ( nMonitorHeight - nTopMargin - nBottomMargin)*(row)/(m_nSampleNumEachCol - 1);

			RECT rect = {(col*nMonitorWidth)/ m_nSampleNumEachRow,(row*nMonitorHeight)/ m_nSampleNumEachCol,((col+1)*nMonitorWidth)/ m_nSampleNumEachRow,((row+1)*nMonitorHeight)/ m_nSampleNumEachCol };
            
			TSampleSymbol& symbol   = m_vecSampleSymbols[nSymbolIndex];
			symbol.clrSampleAdjustment = YELLOW;
            symbol.clrSampleBefore = RED;
            symbol.clrSampleAfter  = GREEN;
            symbol.bSampled        = FALSE;
            symbol.size.cx         = SYMBOL_SIZE;
            symbol.size.cy         = SYMBOL_SIZE;
            symbol.ptDisplay.x     = nX;
            symbol.ptDisplay.y     = nY;
			symbol.rcRect          = rect;

			m_vecConfigCross[nSymbolIndex].x = nX ;
			m_vecConfigCross[nSymbolIndex].y = nY ;

            nSymbolIndex++;
        }
    }

    nSymbolIndex = 0;

    //按列优先排列,计算每列中各个采样点的位置
    for(int col = 0 ; col < m_nSampleNumEachRow;col++)
    {
         nX = nMonitorLeft + (nMonitorWidth * col) / (m_nSampleNumEachRow - 1);

        for(int row = 0; row < m_nSampleNumEachCol ;row++ )
        {
            nY = nMonitorTop + (nMonitorHeight * row)/(m_nSampleNumEachCol - 1);

            TSampleSymbol& symbol   = m_vecSampleSymbols[nSymbolIndex];

            symbol.ptCenter.x = nX;
            symbol.ptCenter.y = nY;

            nSymbolIndex++;
        }
    }

	//激光器底下的采样点向下偏移1/3间隔距离
 	int nOffsetY = nMonitorHeight / (m_nSampleNumEachCol - 1);
	nOffsetY = nOffsetY * 1 / 4;
	m_vecSampleSymbols[3].ptDisplay.y += nOffsetY;

	m_vecConfigCross[3].y = m_vecSampleSymbols[3].ptDisplay.y;

	////前面是初始化状态。现在需要根据配置文件中的坐标值把显示的坐标值进行修改，如果没有配置文件就按照原来的执行
	if (LoadCollectSpotPoint())
	{
		for(unsigned int i = 0 ; i <m_vecConfigCross.size() ;i++ )
		{
			TSampleSymbol& symbol = m_vecSampleSymbols[i];

			symbol.ptDisplay.x = m_vecConfigCross[i].x ;
			symbol.ptDisplay.y = m_vecConfigCross[i].y ;
		}
	}
}

void CCollectSpotSize::DrawCross(HDC hDC, const POINT& ptSymbol, COLORREF color,  const SIZE& size)
{
    const int nLineWindth = 1;
    HPEN hPen        = ::CreatePen(PS_SOLID,nLineWindth,color);
    HPEN hPenold     = (HPEN)::SelectObject(hDC,hPen);

    HBRUSH hBrush    = (HBRUSH)::GetStockObject(NULL_BRUSH);   //创建空画刷
    HBRUSH hBrushOld = (HBRUSH)::SelectObject(hDC,hBrush);

    Rectangle(
        hDC,
        ptSymbol.x -size.cx/2,
        ptSymbol.y - size.cy/2,
        ptSymbol.x + size.cx/2,
        ptSymbol.y + size.cy/2);

    FillRect(
        hDC,
        CRect(ptSymbol.x -size.cx/2 ,ptSymbol.y - size.cy/2,ptSymbol.x + size.cx/2,ptSymbol.y + size.cy/2),
        hBrush);

    ::SelectObject(hDC, hBrushOld);
    ::SelectObject(hDC, hPenold);
    DeleteObject(hPen);

}

void CCollectSpotSize::DrawLine(HDC hDC, const POINT& ptStart, const POINT& ptEnd, COLORREF color)
{
	const int nLineWidth = 2;
	HPEN hPen = ::CreatePen(PS_SOLID, nLineWidth, color);
	HPEN hPenOld = (HPEN)::SelectObject(hDC, hPen);

	MoveToEx(hDC, ptStart.x, ptStart.y, NULL);
	LineTo(hDC, ptEnd.x, ptEnd.y);
}


LRESULT CALLBACK CCollectSpotSize::WinProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    if (g_pCollectSpotSize)
    {
        return g_pCollectSpotSize->InternalWndProc(hWnd,uMsg,wParam,lParam);
    }

    return FALSE;
}


LRESULT CCollectSpotSize::InternalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(uMsg == WM_ACTIVATEAPP)
    {
        AtlTrace("WM_ACTIVATEAPP,wParam = 0x%x\n",wParam);
    }

    else if (uMsg == WM_ACTIVATE)
    {
        AtlTrace("WM_ACTIVATE,wParam = 0x%x\n",wParam);
    }

    else if (uMsg == WM_PAINT)
    {
        ///////////////////首先画出所有需要设置的位置框
        PAINTSTRUCT ps;
        BeginPaint(hWnd,&ps);
        CAtlString str;

        RECT rcClient;
        GetClientRect(hWnd,&rcClient);
        SetBkMode(ps.hdc,TRANSPARENT);
        SetTextColor(ps.hdc,RGB(255,255,255));

        //if (g_oMouseEventGen.GetCollectSpotMode() == COLLECTSPOT_MODE_COLLECT)
        //if(m_eSpotSamplingMode == COLLECTSPOT_MODE_COLLECT)
		for(unsigned int k = 0 ; k < m_vecSampleSymbols.size() ;k++)
		{
			POINT ptDisplay = m_vecSampleSymbols[k].ptDisplay;
			ScreenToClient(hWnd, &ptDisplay);
			DrawCross(
				ps.hdc,
				ptDisplay,
				m_vecSampleSymbols[k].clrSampleAdjustment,
				m_vecSampleSymbols[k].size);
		}

		POINT ptStart,ptEnd;
		//水平方向
		for(int j = 1 ; j <m_nSampleNumEachCol;j++)
		{
		   ptStart.x = m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.left;
		   ptStart.y = m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.top + (m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.bottom - m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.top)*j /m_nSampleNumEachCol;
		   ptEnd.x =   m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.right;
		   ptEnd.y =   m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.top + (m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.bottom - m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.top)*j / m_nSampleNumEachCol;
		   
		   ScreenToClient(hWnd, &ptStart);
		   ScreenToClient(hWnd, &ptEnd);
		   DrawLine(ps.hdc, ptStart, ptEnd, ROYAL_BLUE);
		}
        //垂直方向
		for(int m = 0; m < m_nSampleNumEachRow ; m++)
		{
		   ptStart.x = m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.left + (m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.right - m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.left)*m / m_nSampleNumEachRow;
		   ptStart.y = m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.top;
		   ptEnd.x = m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.left + (m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.right - m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.left)*m / m_nSampleNumEachRow;
		   ptEnd.y = m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.bottom;

		   ScreenToClient(hWnd, &ptStart);
		   ScreenToClient(hWnd, &ptEnd);

		   DrawLine(ps.hdc, ptStart, ptEnd, ROYAL_BLUE);
		}


		if (m_ShowFlag)
		{
            for (int i =0; i <= m_nCurrentSampleNo; i++)
            {
			    POINT ptDisplay = m_vecSampleSymbols[i].ptDisplay;
			    ScreenToClient(hWnd, &ptDisplay);
                DrawCross(		
			       ps.hdc,
				   ptDisplay,
                   m_vecSampleSymbols[i].bSampled ? m_vecSampleSymbols[i].clrSampleAfter : m_vecSampleSymbols[i].clrSampleBefore,
                   m_vecSampleSymbols[i].size);			
            }
			m_ShowFlag = FALSE;
		}
		else
		{
			for (int i = 0; i <= m_nCurrentSampleNo; i++)
			{
				POINT ptDisplay = m_vecSampleSymbols[i].ptDisplay;
				ScreenToClient(hWnd, &ptDisplay);
				if (m_vecSampleSymbols[i].bSampled)
				{
			    	DrawCross(
					        ps.hdc,
					        ptDisplay,
					        m_vecSampleSymbols[i].clrSampleAfter,
					        m_vecSampleSymbols[i].size);
				}
			}
			m_ShowFlag = TRUE;
		}


        DrawText(ps.hdc,g_oResStr[IDS_STRING432],_tcslen(g_oResStr[IDS_STRING432]),&this->m_rcCurrentMonitor, DT_CENTER|DT_TOP);
        EndPaint(hWnd,&ps);

        ::SetTimer(m_hWnd, 1, 1000, NULL);  
    }

    else if (uMsg == WM_COLLECT_SPOT_DATA)//光斑采集消息响应
    {
        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);
        LONG nArea = (LONG)(wParam);

		SpotManualCollectInfo  Info;
		CCollectSpotSize::m_oSpotManualCollectInfoManager.Fetch(&Info);

        switch(m_eSpotSamplingState)
        {
        case E_ALL_SPOT_SAMPLING_START://开始所有光斑的采集
            m_nCurrentSampleNo   = 0;
            m_eSpotSamplingState = E_CURRENT_SYMBOL_SAMPLING_START;

            break;

        case E_CURRENT_SYMBOL_SAMPLING_START://当前符号处的光斑采样开始
            {
                const POINT& ptSymbolPos =  m_vecSampleSymbols[m_nCurrentSampleNo].ptDisplay;
				//delete by vera_zhao 2018.12.10
//              int nOffsetX = pt.x  - ptSymbolPos.x;
//              int nOffsetY = pt.y  - ptSymbolPos.y;
				int nOffsetX = (int)Info.ptPos.d[0] - ptSymbolPos.x;
				int nOffsetY = (int)Info.ptPos.d[1] - ptSymbolPos.y;
                if(abs(nOffsetX) > m_nLightSpotDitherOffsetX || abs(nOffsetY) > m_nLightSpotDitherOffsetY)
                {
                    break;
                }
                else
                {
                    m_nSampleTimes = 0;
                    m_eSpotSamplingState  = E_CURRENT_SYMBOL_CONTINUE_SAMPLING;
                }
            }

            break;


        case E_CURRENT_SYMBOL_CONTINUE_SAMPLING://当前符号处的光斑采样进行中
            {
                const POINT& ptSymbolPos =  m_vecSampleSymbols[m_nCurrentSampleNo].ptDisplay;
				//delete by vera_zhao 2018.12.10
//                int nOffsetX = pt.x  - ptSymbolPos.x;
//                int nOffsetY = pt.y  - ptSymbolPos.y;

				int nOffsetX = (int)Info.ptPos.d[0] - ptSymbolPos.x;
				int nOffsetY = (int)Info.ptPos.d[1] - ptSymbolPos.y;

                if(abs(nOffsetX) > m_nLightSpotDitherOffsetX || abs(nOffsetY) > m_nLightSpotDitherOffsetY)
                {//与采样符号的屏幕位置相差过大,重新开始开始采样
                    m_eSpotSamplingState = E_CURRENT_SYMBOL_SAMPLING_START;
                    break;
                }
                else
                {
                    m_vecSampleData[m_nSampleTimes].Area= Info.rcArea;
					m_vecSampleData[m_nSampleTimes].mass = Info.mass;
                    m_nSampleTimes++;


                    if (m_nSampleTimes == MAX_SPOT_SAMPLING_TIMES)  
                    {//采集了30次都是在一个点上，说明这个点是要取得的点

                        m_vecSampleSymbols[m_nCurrentSampleNo].bSampled = TRUE;
                        m_ScreenLightspotSample[this->m_nCurMonitorAreaId].vecSampleSize[m_nCurrentSampleNo].ptCenter = m_vecSampleSymbols[m_nCurrentSampleNo].ptCenter;
                        
                        int nCalcuteSum = 0;
						int nActiveSum = 0;
                        //多次采样,取平均数值
                        //前一半数据，认为处于暂态，舍弃不用
                        for(int i = MAX_SPOT_SAMPLING_TIMES/2 ; i < MAX_SPOT_SAMPLING_TIMES ; i++)
                        {
							nCalcuteSum += (m_vecSampleData[i].Area.right- m_vecSampleData[i].Area.left)*(m_vecSampleData[i].Area.bottom - m_vecSampleData[i].Area.top);
							nActiveSum += m_vecSampleData[i].mass;
                        }
                        
                        float nCalArea = (float)nCalcuteSum * 2/ MAX_SPOT_SAMPLING_TIMES;
						float nActiveArea = (float)nActiveSum * 2 / MAX_SPOT_SAMPLING_TIMES;
						float  percentageArea = 0.0;
						if (nCalArea !=0) {
							percentageArea = nActiveArea / nCalArea;
						}
	
                        m_ScreenLightspotSample[this->m_nCurMonitorAreaId].vecSampleSize[m_nCurrentSampleNo].lSize = (LONG)nActiveArea;
						m_ScreenLightspotSample[this->m_nCurMonitorAreaId].vecSampleSize[m_nCurrentSampleNo].fPercentage = percentageArea;
                        
                        m_eSpotSamplingState = E_CURRENT_SYMBOL_SAMPLING_END;

                    }//if

                }//else

            }//case E_CURRENT_SYMBOL_CONTINUE_SAMPLING
            break;


        case E_CURRENT_SYMBOL_SAMPLING_END://当前符号处的光斑采集结束
            //这是最后一个点的信息加进在数组中。
            if (m_nCurrentSampleNo == m_vecSampleSymbols.size() -1)
            {
                if(m_nCurMonitorAreaId < (int)m_vecMonitorAreas.size() -1 )
                {//还未采样到最后一个屏幕, 继续采样下一个屏幕

                    m_nCurMonitorAreaId++;
                   InitSamplePosition(m_vecMonitorAreas[m_nCurMonitorAreaId]);
                   InvalidateRect(m_hWnd,NULL,TRUE);
                   m_nCurrentSampleNo = 0;
                   m_nSampleTimes = 0;

                   m_eSpotSamplingState = E_CURRENT_SYMBOL_SAMPLING_START;
                }
                else
                {//全部采样完毕则退出

                    InvalidateRect(m_hWnd,NULL,TRUE);

                    //通知消息给消息响应窗体，结束采样
                    if (m_hOwner)
                    {
                        //结束光斑采集
                        PostMessage(m_hOwner,WM_FINISH_COLLECTSPOT, m_ScreenLightspotSample.size(), m_nSensorID);

                        //关闭全屏显示
                        //FullScreen(FALSE);
						ShowWindow(m_hWnd, SW_HIDE);

                        if(m_hOwner) EnableWindow(m_hOwner, TRUE);
						SaveCollectSpotPoint();
                    }

                    m_eSpotSamplingState = E_ALL_SPOT_SAMPLEING_END;
                }
            }
            else//继续进行下一个点的采集
            {
                m_nCurrentSampleNo ++ ;

                InvalidateRect(m_hWnd,NULL,TRUE);

                m_eSpotSamplingState = E_CURRENT_SYMBOL_SAMPLING_START;

                m_nSampleTimes = 0;

            }
            break;


        case E_ALL_SPOT_SAMPLEING_END://所有光斑的采集结束

            return 0;//

        default:
            break;
        }//switch

 

    }//else if
    else if (uMsg == WM_LBUTTONDOWN)//单击鼠标左键
    {

        //BOOL f = KillTimer(m_hWnd,1);//删除定时器（鼠标的实时坐标）

        //if (g_oMouseEventGen.GetCollectSpotMode() == COLLECTSPOT_MODE_MANUAL)
        //{
        //	POINT PtPosition;
        //	PtPosition.x = LOWORD(lParam);
        //	PtPosition.y = HIWORD(lParam);

        //	BOOL bFlag = FALSE;

        //	CSpotSettingDlg dlg(CWnd::FromHandle(m_hWnd));

        //	for(unsigned int i =0; i< m_vecSampleSymbols.size();i++)
        //	{
        //		if ((PtPosition.x < m_vecSampleSymbols[i].ptCenter.x + SYMBOL_SIZE && PtPosition.x > m_vecSampleSymbols[i].ptCenter.x - SYMBOL_SIZE)
        //			&& PtPosition.y < m_vecSampleSymbols[i].ptCenter.y + SYMBOL_SIZE && PtPosition.y > m_vecSampleSymbols[i].ptCenter.y - SYMBOL_SIZE)
        //		{
        //			dlg.m_x_Spot = m_vecSampleSymbols[i].ptCenter.x;
        //			dlg.m_y_Spot = m_vecSampleSymbols[i].ptCenter.y;

        //			bFlag = TRUE;
        //			break;
        //		}
        //	}

        //	if (!bFlag)
        //	{
        //		MessageBox(m_hWnd,g_oResStr[IDS_STRING431],g_oResStr[IDS_STRING103],MB_ICONINFORMATION|MB_OKCANCEL);
        //		return 1;
        //	}

        //	///////////////////////////////判断如果是重复点击一个点时需要进行判断

        //	for(unsigned int i =0; i<m_vecMaxSpot.size();i++)
        //	{
        //		if ((PtPosition.x< m_vecMaxSpot[i].ptCenter.x + SYMBOL_SIZE && PtPosition.x > m_vecMaxSpot[i].ptCenter.x - SYMBOL_SIZE)
        //			&&(PtPosition.y< m_vecMaxSpot[i].ptCenter.y + SYMBOL_SIZE && PtPosition.y> m_vecMaxSpot[i].ptCenter.y - SYMBOL_SIZE))
        //		{
        //			if (MessageBox(m_hWnd,g_oResStr[IDS_STRING430],g_oResStr[IDS_STRING105],MB_ICONINFORMATION|MB_OKCANCEL) == IDOK)
        //			{
        //				s_nSpotSettingCount--;
        //				break;
        //			}
        //			else
        //			{
        //				return 1;
        //			}
        //		}
        //	}

        //	//////////////然后弹出对话框，对话框中的X、Y的坐标已经给出了，只要输入面积就可以了，或者修改X、Y的坐标值
        //	if (dlg.DoModal() == IDOK)
        //	{
        //		if (dlg.m_x_Spot == 0 || dlg.m_y_Spot == 0 || dlg.m_s_Spot == 0)
        //		{
        //			MessageBox(m_hWnd,g_oResStr[IDS_STRING429],g_oResStr[IDS_STRING103],MB_OK |MB_ICONERROR);
        //			return 1;
        //		}

        //		int Spot_X_Position = dlg.m_x_Spot;
        //		int Spot_Y_Position = dlg.m_y_Spot;
        //		long Spot_S_Area    = dlg.m_s_Spot;

        //		for (unsigned int i =0; i< m_vecSampleSymbols.size();i++)
        //		{
        //			if ( (Spot_X_Position<m_vecSampleSymbols[i].ptCenter.x + SYMBOL_SIZE && Spot_X_Position > m_vecSampleSymbols[i].ptCenter.x - SYMBOL_SIZE)
        //				&& (Spot_Y_Position < m_vecSampleSymbols[i].ptCenter.y +SYMBOL_SIZE && Spot_Y_Position > m_vecSampleSymbols[i].ptCenter.y - SYMBOL_SIZE))
        //			{
        //				m_vecMaxSpot[i].ptCenter.x = Spot_X_Position;
        //				m_vecMaxSpot[i].ptCenter.y = Spot_Y_Position;
        //				m_vecMaxSpot[i].lSize      = Spot_S_Area;

        //				m_vecSampleSymbols[i].bAdjusted = TRUE;
        //				s_nSpotSettingCount++;
        //				break;

        //			}
        //		}
        //	}
        //}
        //::SetTimer(m_hWnd,1,100,NULL);

		POINT PtPosition;
		PtPosition.x = LOWORD(lParam);
		PtPosition.y = HIWORD(lParam);
		ClientToScreen(hWnd,&PtPosition);
		m_nSelectDragIndex = -1;
		int nScreenOffsetX = m_vecSampleSymbols[0].ptCenter.x ;
		int nScreenOffsetY = m_vecSampleSymbols[0].ptCenter.y ;

		for(unsigned int i = 0; i < m_vecSampleSymbols.size(); i++)
		{
			if ( (PtPosition.x > nScreenOffsetX + m_vecSampleSymbols[i].rcRect.left && PtPosition.x < nScreenOffsetX + m_vecSampleSymbols[i].rcRect.right)
				&& (PtPosition.y >nScreenOffsetY + m_vecSampleSymbols[i].rcRect.top && PtPosition.y < nScreenOffsetY + m_vecSampleSymbols[i].rcRect.bottom) )
			{
				m_nSelectDragIndex = i;
				break;
			}
		}
    }
	else if (uMsg == WM_LBUTTONUP)
	{
		//左键弹起
		POINT PtPosition;
		PtPosition.x = LOWORD(lParam);
		PtPosition.y = HIWORD(lParam);
		ClientToScreen(hWnd, &PtPosition);
		int nScreenOffsetX = m_vecSampleSymbols[0].ptCenter.x;
		int nScreenOffsetY = m_vecSampleSymbols[0].ptCenter.y;

		if (m_nSelectDragIndex != -1)
		{
			if ( (PtPosition.x > nScreenOffsetX + m_vecSampleSymbols[m_nSelectDragIndex].rcRect.left && PtPosition.x < nScreenOffsetX + m_vecSampleSymbols[m_nSelectDragIndex].rcRect.right)
				&& (PtPosition.y >nScreenOffsetY + m_vecSampleSymbols[m_nSelectDragIndex].rcRect.top && PtPosition.y < nScreenOffsetY + m_vecSampleSymbols[m_nSelectDragIndex].rcRect.bottom) )
			{
		       if (!m_vecSampleSymbols[m_nSelectDragIndex].bSampled)
		       {
		          m_vecSampleSymbols[m_nSelectDragIndex].ptDisplay.x = PtPosition.x ;
		          m_vecSampleSymbols[m_nSelectDragIndex].ptDisplay.y = PtPosition.y ;

				  m_vecConfigCross[m_nSelectDragIndex].x = PtPosition.x ;
				  m_vecConfigCross[m_nSelectDragIndex].y = PtPosition.y ;

		          InvalidateRect(m_hWnd, NULL, TRUE);
		      }
			}
			m_nSelectDragIndex = -1;
		}
	}
    else if(uMsg == WM_RBUTTONDOWN)        //单击鼠标右键
    {
        //::KillTimer(m_hWnd,1);

        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);

        HMENU hMenu = ::LoadMenu(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDR_MENU_CTXMENU));
        HMENU hCtxMenu = ::GetSubMenu(hMenu,2);

		if (m_nSensorID > -1)
		{
		    pt.x = pt.x+m_ScreenLightspotSample[this->m_nCurMonitorAreaId].rcMonitor.left;
		}

        if (hCtxMenu)
        {
            TrackPopupMenu(hCtxMenu,
                TPM_RIGHTALIGN|TPM_BOTTOMALIGN,
                pt.x,
                pt.y,
                0,
                m_hWnd,
                NULL);
        }

        DestroyMenu(hMenu);
    }

    else if(uMsg == WM_COMMAND)
    {
        if (wParam == ID_EXIT_BLOBSETING)
        {
/*            if (m_nCurMonitorId == m_oMonitorFinder.GetMonitorCount() )
            {
                if (m_hOwner)
                {
                    PostMessage(m_hOwner,WM_FINISH_COLLECTSPOT,0,0);
                    FullScreen(FALSE);
                    if (m_hOwner)
                    {
                        EnableWindow(m_hOwner,TRUE);
                    }
                }
            }
            else*/ if (AbortCollectSpotSize())
            {
                SendMessage(m_hOwner,WM_BREAK_COLLECTSPOT, m_nSensorID,0);
            }
        }
    }

    else if(uMsg == WM_ERASEBKGND)
    {
        HDC hDC = (HDC)wParam;
        RECT rcClient;
        GetClientRect(hWnd,&rcClient);
        HBRUSH hBBrush = ::CreateSolidBrush(BKGND_COLOR);

        FillRect(hDC,&rcClient,hBBrush);
        DeleteObject(hBBrush);

        return 1;
    }

    else if(uMsg == WM_CLOSE)                   //结束
    {
        if (AbortCollectSpotSize())
        {
            SendMessage(m_hOwner,WM_BREAK_COLLECTSPOT, m_nSensorID,0);
        }

    }
    else if(uMsg == WM_TIMER)                                  //定时器
    {
		InvalidateRect(m_hWnd, NULL, TRUE); 

        //if (g_oMouseEventGen.GetCollectSpotMode() == COLLECTSPOT_MODE_MANUAL)
        //{
        //	CAtlString  m_Point;
        //	POINT CursorPoint;
        //	GetCursorPos(&CursorPoint);
        //	int print_x = CursorPoint.x;
        //	int print_y = CursorPoint.y;
        //	m_Point.Format( _T("%d,%d"),print_x,print_y);

        //	HDC hDC = ::GetDC(m_hWnd);
        //	RECT rcRect;
        //	rcRect.left   = CursorPoint.x ;
        //	rcRect.top    = CursorPoint.y;
        //	rcRect.right  = CursorPoint.x+ 70;
        //	rcRect.bottom = CursorPoint.y + 30;

        //	SetBkMode(hDC,TRANSPARENT);
        //	SetTextColor(hDC,RGB(255,255,255));
        //	DrawText(hDC,m_Point,m_Point.GetLength(),&rcRect,NULL);

        //	ReleaseDC(m_hWnd,hDC);
        //}

    }
    else if(uMsg == WM_MOUSEMOVE)                              //鼠标移动
    {
        //if(g_oMouseEventGen.GetCollectSpotMode() == COLLECTSPOT_MODE_MANUAL)
        //{
        //          InvalidateRect(m_hWnd,NULL,TRUE);
        //}
    }
    else if(uMsg == WM_KEYDOWN)
    {
        if(wParam == VK_ESCAPE)
        {   //ESC按下键中途退出
            if (AbortCollectSpotSize())
            {
                SendMessage(m_hOwner,WM_BREAK_COLLECTSPOT, m_nSensorID,0);
            }
        }
    }

    return DefWindowProc(hWnd,uMsg,wParam,lParam);

}

//@功能:开始光斑采样
//@参数:pMonitorInfo, 屏幕信息数组
//      nMonitorCount, 屏幕个数
//BOOL  CCollectSpotSize::StartCollectSpotSize(const MonitorInfo* pMonitorInfo, int nMonitorCount, HWND hNotifyWnd, ESampleCollectPattern ePattern)
BOOL  CCollectSpotSize::StartCollectSpotSize(const RECT* pMonitorAreas, int nAreaCount, HWND hNotifyWnd, int nSampleNumEachRow, int nSampleNumEachCol,int nSensorId, TCHAR *lpszbuf,int nSensorCount)
{
	//一个显示设备都未找到，则立即返回
	if (nAreaCount == 0) return FALSE;
	//>>

    if(this->m_hWnd == NULL)
    {
        if(!Create())
        {
            return FALSE;
        }

        m_hOwner = hNotifyWnd;//通知窗体
        
    }
	m_nSensorID = nSensorId;
	if (nSensorId > -1)
	{
		//单屏采集
		m_nCollectSensorCount = 1;
	}
	else
	{  
		m_nCollectSensorCount = nSensorCount;
	}

	wcscpy_s(CollectSpotDragPath, lpszbuf);

    //采样样式
    //m_eSpotSamplePattern = ePattern;
    
    //复制屏幕信息
	m_vecMonitorAreas.resize(nAreaCount);
    m_ScreenLightspotSample.resize(nAreaCount);


	RECT rcBoundary;
	rcBoundary.left   = 0;
	rcBoundary.top    = 0;
	rcBoundary.right  = 0;
	rcBoundary.bottom = 0;

    for(int i=0; i< nAreaCount; i++)
    {
		m_vecMonitorAreas[i] = pMonitorAreas[i];
        m_ScreenLightspotSample[i].rcMonitor = pMonitorAreas[i];

		RECT rcArea = pMonitorAreas[i];
		if(m_nSensorID > -1)
		{
			rcBoundary.left = rcArea.left;
			rcBoundary.right = rcArea.right;
			rcBoundary.top = rcArea.top;
			rcBoundary.bottom = rcArea.bottom;
		}
		else
		{
		    if (rcArea.left  < rcBoundary.left    ) rcBoundary.left   = rcArea.left;
		    if (rcArea.right > rcBoundary.right   ) rcBoundary.right  = rcArea.right;
		    if (rcArea.top   < rcBoundary.top     ) rcBoundary.top    = rcArea.top;
		    if (rcArea.bottom > rcBoundary.bottom ) rcBoundary.bottom = rcArea.bottom;
		}
    }


    m_nSampleNumEachCol = nSampleNumEachCol;
    m_nSampleNumEachRow = nSampleNumEachRow;
  
    //初始化
    m_nCurrentSampleNo =0;
    m_eSpotSamplingState = E_ALL_SPOT_SAMPLING_START;

    //初始化第一个校正屏幕
	m_nCurMonitorAreaId = 0;
    InitSamplePosition(m_vecMonitorAreas[m_nCurMonitorAreaId]);

	SetWindowPos(
		m_hWnd,
		HWND_TOPMOST,
		//HWND_TOP, 
		rcBoundary.left,
		rcBoundary.top,
		rcBoundary.right  - rcBoundary.left,
		rcBoundary.bottom - rcBoundary.top,
		SWP_SHOWWINDOW);
	SetFocus(m_hWnd);
	

    //FullScreen(TRUE);

    InvalidateRect(m_hWnd,NULL,FALSE);
    if (m_hOwner)
    {
        EnableWindow(m_hOwner,FALSE);
    }


    return TRUE;
}

BOOL CCollectSpotSize::AbortCollectSpotSize()
{
    if (MessageBox(m_hWnd,g_oResStr[IDS_STRING428],g_oResStr[IDS_STRING105],MB_ICONINFORMATION|MB_OKCANCEL) == IDOK)
    {
        //FullScreen(FALSE);
		ShowWindow(m_hWnd, SW_HIDE);
        if (m_hOwner)
        {
            EnableWindow(m_hOwner,TRUE);
        }
        return TRUE;
    }
    else
    {
        m_nCurrentSampleNo = 0;
		m_nCurMonitorAreaId = 0;
        InitSamplePosition(m_vecMonitorAreas[m_nCurMonitorAreaId]);
        InvalidateRect(m_hWnd,NULL,TRUE);
    }
    return FALSE;
}



//@功能:屏幕分辨率变化事件响应函数
//@参数:nScreenWidth, 新的屏幕宽度
//      nScreenHeight,新的屏幕高度
void CCollectSpotSize::OnDisplayChange(int nScreenWidth, int nScreenHeight)
{
    m_nLightSpotDitherOffsetX = (int)((double)nScreenWidth  * LIGHT_SPOT_DITHER_OFFSET_PERCENT_X/100.0);
    m_nLightSpotDitherOffsetY = (int)((double)nScreenHeight * LIGHT_SPOT_DITHER_OFFSET_PERCENT_Y/100.0);
}


//@功能:设备丢失时的事件响应函数
void CCollectSpotSize::OnDeviceMissing()
{
    //
    MessageBox(this->m_hWnd, g_oResStr[IDS_STRING454], g_oResStr[IDS_STRING109], MB_ICONERROR|MB_OK);

    //FullScreen(FALSE);
	ShowWindow(m_hWnd, SW_HIDE);

    if (m_hOwner)
    {
        SendMessage(m_hOwner,WM_BREAK_COLLECTSPOT, m_nSensorID,0);
        EnableWindow(m_hOwner,TRUE);
    }

}
//@功能：加载配置中
BOOL  CCollectSpotSize::LoadCollectSpotPoint()
{

	TiXmlDocument oXMLDoc;
	if (!oXMLDoc.LoadFile(CT2A(CollectSpotDragPath), TIXML_ENCODING_UTF8))		return FALSE;
	TiXmlElement *pRootElement = oXMLDoc.RootElement();
	if (pRootElement == NULL)		return FALSE;

	TiXmlNode* pChild = NULL;
	do
	{
		pChild = pRootElement->IterateChildren(pChild);
		if(NULL == pChild)
		{
			break;
		}
		const char* lpszElementName = pChild->Value();
		if (_stricmp(lpszElementName, "Sensor") == 0)
		{
			const char* SensorCount = ((TiXmlElement*)pChild)->Attribute("number");
			int nSensorCount = atoi(SensorCount);
			if (nSensorCount != m_nCollectSensorCount && nSensorCount != 0)
			{
				return FALSE;
			}
		}
		else if (_stricmp(lpszElementName, "Points") == 0)
		{
			const char* PointCount = ((TiXmlElement*)pChild)->Attribute("count");
			int nPointCount = atoi(PointCount);

			if (PointCount == 0) return false;
			m_vecConfigCross.resize(nPointCount);

			TiXmlNode * pChildren = NULL;
			int nIndex = 0;
			do
			{
				pChildren = pChild->IterateChildren(pChildren);
				if (NULL == pChildren)    	break;

				const char* lpszElementName = pChildren->Value();
				if (_stricmp(lpszElementName, "Point") == 0)
				{
					const char* szX = ((TiXmlElement*)pChildren)->Attribute("X");
					const char* szY = ((TiXmlElement*)pChildren)->Attribute("Y");

					m_vecConfigCross[nIndex].x = atof(szX);
					m_vecConfigCross[nIndex].y = atof(szY);
					nIndex++;
				}

			} while (pChildren);
		}
	} while (pRootElement);

	return TRUE;
}

BOOL  CCollectSpotSize::SaveCollectSpotPoint()
{
	TiXmlDocument oXMLDoc;
	TiXmlDeclaration Declaration("1.0", "UTF-8", "no");
	oXMLDoc.InsertEndChild(Declaration);

	TiXmlElement * pConfig = new TiXmlElement("Config");
	oXMLDoc.LinkEndChild(pConfig);

	TiXmlElement * pSersorcount= new TiXmlElement("Sensor");
	pSersorcount->SetAttribute("number", m_nCollectSensorCount);
	pConfig->LinkEndChild(pSersorcount);


	TiXmlElement * pLensConfig = new TiXmlElement("Points");
	pLensConfig->SetAttribute("count", m_vecConfigCross.size());
	pConfig->LinkEndChild(pLensConfig);

	for (unsigned int i = 0; i < m_vecConfigCross.size(); i++)
	{
		TiXmlElement * pElement = new TiXmlElement("Point");
		pElement->SetDoubleAttribute("X", m_vecConfigCross[i].x );
		pElement->SetDoubleAttribute("Y", m_vecConfigCross[i].y );
		pLensConfig->LinkEndChild(pElement);
	}

	//以UTF-8编码格式保存
	TiXmlPrinter  printer;
	oXMLDoc.Accept(&printer);

	char UTF8BOM[3] = { '\xEF','\xBB','\xBF' };

	std::ofstream theFile;
	//注意:
	//代码若为:theFile.open(CT2A(lpszConfigFilePath) , ios_base::out) ;
	//则路径中若有中文字符，
	//mbstowc_s(NULL,wc_Name, FILENAME_MAX,fileName,FILENAMEMAX-1)
	//返回的wc_name中为乱码
	//

	theFile.open(CT2W(CollectSpotDragPath), ios_base::out);

	if (theFile.is_open())
	{
		theFile.write(UTF8BOM, 3);

		int length = strlen(printer.CStr());

		char* utf_8_buf = (char*)malloc(length * 4);
		memset(utf_8_buf, 0, length * 4);

		//Unicode转为UTF8编码
		WideCharToMultiByte(
			CP_UTF8,
			0,
			CA2W(
				printer.CStr(),
				936 //gb2312
			),
			length,
			utf_8_buf,
			length * 4,
			NULL,
			NULL);


		theFile.write(utf_8_buf, strlen(utf_8_buf));

		theFile.close();

		free(utf_8_buf);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}