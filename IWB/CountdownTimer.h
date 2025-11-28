

class CCountdownTimer
{
protected:
    CCountdownTimer(int nCountStartNum, int nStepTimeout, const RECT* pRect, /*int nFontHeight,*/ COLORREF clrText, HWND hNotifyWnd, UINT uNotifyMsg)
    {

        m_nCurrentNum = m_nCountStartNum = nCountStartNum;
        m_nStepTimeout   = nStepTimeout;
        if(pRect)
        {
            m_rcWndArea      = *pRect;
        }
        else
        {
            //int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
            //int nCyScreen = GetSystemMetrics(SM_CYSCREEN);
#if 0
            int nCxVScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
            int nCyVScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
            int nXVScreen  = GetSystemMetrics(SM_XVIRTUALSCREEN);
            int nYVScreen  = GetSystemMetrics(SM_YVIRTUALSCREEN);
#else
            int nCxVScreen = GetSystemMetrics(SM_CXSCREEN);
            int nCyVScreen = GetSystemMetrics(SM_CYSCREEN);
            int nXVScreen = 0;
            int nYVScreen = 0;

#endif

            //m_rcWndArea.left  = (nCxScreen - DEFAULT_WIDTH)/2;
            //m_rcWndArea.right = m_rcWndArea.left + DEFAULT_WIDTH;
            /*
            m_rcWndArea.left = nCxScreen/4;
            m_rcWndArea.right = nCxScreen*3/4;


            //m_rcWndArea.top   = (nCyScreen - DEFAULT_HEIGHT)/2;
            //m_rcWndArea.bottom= m_rcWndArea.top + DEFAULT_HEIGHT;
            m_rcWndArea.top    = nCyScreen/4;
            m_rcWndArea.bottom = nCyScreen*3/4;
            */

            m_rcWndArea.left    = nXVScreen;
            m_rcWndArea.right   = nXVScreen + nCxVScreen;

            m_rcWndArea.top     = nYVScreen;
            m_rcWndArea.bottom  = nYVScreen + nCyVScreen;
        }

        LOGFONT lf;
        memset(&lf, 0, sizeof(LOGFONT));
        lf.lfHeight        = (m_rcWndArea.bottom - m_rcWndArea.top)/2;
        lf.lfWidth         = 0;
        lf.lfWeight        = FW_BOLD;
        lf.lfCharSet       = ANSI_CHARSET;
        lf.lfOutPrecision  = OUT_DEFAULT_PRECIS;
        lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
        memcpy(lf.lfFaceName, _T("Times New Roman"), _countof(lf.lfFaceName));

        m_hFont = CreateFontIndirect(&lf);

        m_clrText = clrText;
        m_hNotifyWnd = hNotifyWnd;
        m_uNotifyMsg = uNotifyMsg;

        static int s_InstanceCount = 1;


        CAtlString strClassName;

        strClassName.Format(_T("Countdown Timer Window %d"), s_InstanceCount++);

        //m_hbrBackground = ::CreateSolidBrush(RGB(0,0,0));
        m_hbrBackground = ::CreateSolidBrush(RGB(255,0,0));
        WNDCLASSEX wndclass;
        wndclass.cbSize = sizeof wndclass;
        wndclass.style = CS_SAVEBITS;
        wndclass.lpfnWndProc = StaticWndProc;  
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        m_hInst = wndclass.hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);;
        wndclass.hIcon = 0;
        wndclass.hCursor = 0;
        wndclass.hbrBackground = m_hbrBackground;
        wndclass.lpszMenuName = 0;
        wndclass.lpszClassName = strClassName;
        wndclass.hIconSm = 0;
        ATOM  atom = RegisterClassEx(&wndclass);

        HDC hDCScreen = ::GetDC(GetDesktopWindow());
        m_hMemDC = ::CreateCompatibleDC(hDCScreen);
        m_hBitmap =::CreateCompatibleBitmap(hDCScreen, m_rcWndArea.right - m_rcWndArea.left, m_rcWndArea.bottom - m_rcWndArea.top);

        ::SetTextColor(m_hMemDC, m_clrText);

        ::ReleaseDC(GetDesktopWindow(), hDCScreen);

        m_hBitmapOld = (HBITMAP)::SelectObject(m_hMemDC, m_hBitmap);
        m_hFontOld   = (HFONT)::SelectObject(m_hMemDC, m_hFont);

        m_hWnd = ::CreateWindowEx(
            WS_EX_TOOLWINDOW|WS_EX_TOPMOST,//dwExStyle,
            strClassName,
            strClassName,//windows name
            WS_POPUP|WS_VISIBLE,
            m_rcWndArea.left,
            m_rcWndArea.top,
            m_rcWndArea.right - m_rcWndArea.left,
            m_rcWndArea.bottom - m_rcWndArea.top,
            NULL,//owner window
            0,//Menu ID
            m_hInst,
            (LPVOID)this);

        //ShowWindow(m_hWnd, SW_SHOW);

        //搜索收集屏幕信息
        //m_oDispMonitorFinder.SearchMonitor();
        theApp.GetMonitorFinder().SearchMonitor();

        SetTimer(m_hWnd,
            1, //nIDEvent
            nStepTimeout, 
            NULL);




    }

    ~CCountdownTimer()
    {
        ::SelectObject(m_hMemDC, m_hBitmapOld);
        ::SelectObject(m_hMemDC, m_hFontOld);
        ::DeleteObject(m_hFont);
        ::DeleteDC(m_hMemDC);		
        ::DeleteObject(m_hbrBackground);

    }




    LRESULT  WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
    {
        //AtlTrace(_T("msg=0x%x\n"), uMsg);

        switch(uMsg)
        {

        case WM_NCDESTROY:
            {
                KillTimer(m_hWnd, 1);

                //自删除
                delete this;

                FreeThunk(&CCountdownTimer::WndProc, this);


                break;
            }

        case WM_CREATE:
            {

                break;
            }

        case WM_SHOWWINDOW:

            break;

        case WM_ERASEBKGND:

            return 1;//erases the background
            break;

        case WM_NCPAINT:

            return 0;
            break;

        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps); 

                //RECT rcClient;
                //GetClientRect(hWnd, &rcClient);

                //FillRect(m_hMemDC, &rcClient, m_hbrBackground);	
                //SetBkMode(m_hMemDC, TRANSPARENT);

                //TCHAR szText[32];
                //_stprintf_s(szText, _countof(szText), _T("%d"), m_nCurrentNum);
                //DrawText(m_hMemDC, szText, _tcsclen(szText), &rcClient, DT_CENTER | DT_VCENTER);
                //

                //BitBlt(
                //	hdc, 
                //	0, 
                //	0, 
                //	rcClient.right - rcClient.left,
                //	rcClient.bottom - rcClient.top,
                //	m_hMemDC,
                //	0,
                //	0,
                //	SRCCOPY);


                EndPaint(hWnd, &ps); 
                return 0;

                break;
            }

        case WM_TIMER:
            {
                m_nCurrentNum --;

                //InvalidateRect(m_hWnd, NULL, TRUE);
                //UpdateWindow(m_hWnd); 


                //SetWindowPos(
                //    this->m_hWnd,
                //    HWND_TOPMOST,
                //    0,0,
                //    0,0,
                //     SWP_NOMOVE | SWP_NOSIZE);


                //int nMointorCount = m_oDispMonitorFinder.GetMonitorCount();

                int nMointorCount = theApp.GetMonitorFinder().GetMonitorCount();

                for(int iMonitorId = 0; iMonitorId < nMointorCount; iMonitorId++)
                {

                    //RECT rcClient;
                    //GetClientRect(hWnd, &rcClient);
                    //int iWidth  = rcClient.right - rcClient.left;
                    //int iHeight = rcClient.bottom - rcClient.top;
                    TCHAR szText[32];
                    _stprintf_s(szText, _countof(szText), _T("%d"), m_nCurrentNum);


                    //const MonitorInfo* pMonitorInfo = m_oDispMonitorFinder.GetMointorInfo(iMonitorId);
                    const MonitorInfo* pMonitorInfo = theApp.GetMonitorFinder().GetMointorInfo(iMonitorId);
                    if(NULL == pMonitorInfo) continue;

                    //屏幕区域
                    RECT rcMonitor = pMonitorInfo->rcMonitor;
                    ScreenToClient(hWnd, (LPPOINT)&rcMonitor.left);
                    ScreenToClient(hWnd, (LPPOINT)&rcMonitor.right);

                    int nMonitorWidth  = rcMonitor.right  - rcMonitor.left;
                    int nMonitorHeight = rcMonitor.bottom - rcMonitor.top;


                    FillRect(m_hMemDC, &rcMonitor, m_hbrBackground);
                    SetBkMode(m_hMemDC, TRANSPARENT);

                    //显示图片
                    DrawText(m_hMemDC, szText, _tcsclen(szText), &rcMonitor, DT_CENTER|DT_VCENTER| DT_SINGLELINE );

                    //int nTextWidth  = rcText.right  - rcText.left;
                    //int nTextHeight = rcText.bottom - rcText.top;

                    //rcText.left  = iWidth/2 - nTextWidth/2;
                    //rcText.right = rcText.left + nTextWidth;

                    //rcText.top   =  iWidth/2 - nText


                    HDC hdcScreen = GetDC(this->m_hWnd);

                    // Call UpdateLayeredWindow
                    //BLENDFUNCTION blend = {0};
                    //blend.BlendOp = AC_SRC_OVER;
                    //blend.SourceConstantAlpha = 127;
                    //blend.AlphaFormat = 0;//AC_SRC_ALPHA;
                    //POINT ptPos = {0, 0};
                    //SIZE sizeWnd = {iWidth, iHeight};
                    //POINT ptSrc = {0, 0};
                    //BOOL bRet = UpdateLayeredWindow(hWnd, hdcScreen, NULL, &sizeWnd, m_hMemDC, &ptSrc, RGB(0,0,0), NULL, ULW_COLORKEY);

                    BOOL bRet = BitBlt(
                        hdcScreen,
                        rcMonitor.left,
                        rcMonitor.top ,
                        nMonitorWidth,
                        nMonitorHeight,
                        m_hMemDC,
                        0,
                        0,
                        SRCCOPY);





                    //BOOL bRet = UpdateLayeredWindow(hWnd, hdcScreen, NULL, &sizeWnd, m_hMemDC, &ptSrc, RGB(0,0,0), &blend, ULW_ALPHA);

                    DWORD dwErr = 0;

                    if(!bRet)
                    {

                        dwErr = GetLastError();
                    }

                    ReleaseDC(this->m_hWnd, hdcScreen);
                }
                if(m_nCurrentNum == 0)
                {
                    PostMessage(m_hNotifyWnd, m_uNotifyMsg, 0, 0);

                    //SetWindowPos(
                    //    this->m_hWnd,
                    //    HWND_BOTTOM,
                    //    0,0,
                    //    0,0,
                    //     SWP_NOMOVE | SWP_NOSIZE);

                    ShowWindow(hWnd, SW_HIDE);
                    DestroyWindow(m_hWnd);

                    //刷新桌面
                    //HWND hDesktop = GetDesktopWindow();
                    //InvalidateRect(hDesktop, NULL, TRUE);
                    //UpdateWindow(hDesktop);
                }

            }
        }
        return DefWindowProc(hWnd,uMsg,wParam,lParam);
    }

    static LRESULT CALLBACK StaticWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
    {

        if(uMsg == WM_CREATE)
        {
            CREATESTRUCT* lpcs = reinterpret_cast< CREATESTRUCT*>(lParam);

            SetWindowLong(
                hWnd, 
                GWL_WNDPROC,
                (LONG)MF2GF<WNDPROC>(&CCountdownTimer::WndProc, (CCountdownTimer*)lpcs->lpCreateParams));



            /*commented out by xuke@gmail.com, 2012/09/03
            SetWindowLong(hWnd, 
            GWL_EXSTYLE, 
            GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
            */

            // Make this window 70% alpha
            //SetLayeredWindowAttributes(hWnd, RGB(0,0,0), (255 * 70) / 100, LWA_ALPHA|LWA_COLORKEY);



            // Ask the window and its children to repaint
            //RedrawWindow(hWnd, 
            //         NULL, 
            //         NULL, 
            //         RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);

        }

        return DefWindowProc(hWnd,uMsg,wParam,lParam);
    }

public:	
    //@功能:开启
    static BOOL StartCountdownTimer(int nCountStartNum, int nStepTimeout, const RECT* pRect, /*int nFontHeight, */COLORREF clrText, HWND hNotifyWnd, UINT uNotifyMsg)
    {
        CCountdownTimer* pNew = new CCountdownTimer(nCountStartNum, nStepTimeout, pRect, /*nFontHeight,*/ clrText, hNotifyWnd, uNotifyMsg);



        return TRUE;
    }

protected:
    HBRUSH m_hbrBackground;
    HWND  m_hWnd          ;
    int   m_nCountStartNum;
    int   m_nCurrentNum   ;
    int   m_nStepTimeout  ;//倒计数定时间隔
    RECT  m_rcWndArea     ;//
    HFONT m_hFont         ;//计数显示的字体
    HFONT m_hFontOld      ;//
    COLORREF m_clrText    ;//字体颜色
    HWND     m_hNotifyWnd ;//计数到0时通知消息的接收窗体
    UINT     m_uNotifyMsg ;//通知消息

    //static 	const TCHAR* m_szClassName;

    //static const int DEFAULT_WIDTH  = 100;
    //static const int DEFAULT_HEIGHT = 100;
    HINSTANCE m_hInst;
    HDC       m_hMemDC;
    HBITMAP   m_hBitmap;
    HBITMAP   m_hBitmapOld;
    //CDispMonitorFinder m_oDispMonitorFinder;


};
//_declspec(selectany) const TCHAR* CCountdownTimer::m_szClassName = _T("Countdown Timer Window");