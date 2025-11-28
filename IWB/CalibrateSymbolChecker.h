#pragma once
//@功能:显示校正符号的半透明窗体
class CCalibrateSymbolChecker
{
public:
    //@参数:clrText, 缺省的文字颜色。
    CCalibrateSymbolChecker()
        :
    m_hWnd (NULL),
    m_clrSymbol(RGB(255,0,0)),
    m_nSymbolSize(30)
    {
        //全屏显示
#if 0
        int nCxVScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int nCyVScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        int nXVScreen  = GetSystemMetrics(SM_XVIRTUALSCREEN );
        int nYVScreen  = GetSystemMetrics(SM_YVIRTUALSCREEN );
#else
        int nCxVScreen = GetSystemMetrics(SM_CXSCREEN);
        int nCyVScreen = GetSystemMetrics(SM_CYSCREEN);
        int nXVScreen = 0;
        int nYVScreen = 0;
#endif

        m_rcWndArea.left    = nXVScreen;
        m_rcWndArea.right   = nXVScreen + nCxVScreen;

        m_rcWndArea.top     = nYVScreen;
        m_rcWndArea.bottom  = nYVScreen + nCyVScreen;
    

        //准备字体
        LOGFONT lf;
        memset(&lf, 0, sizeof(LOGFONT));
        lf.lfHeight        = (m_rcWndArea.bottom - m_rcWndArea.top);
        lf.lfWidth         = 0;
        lf.lfWeight        = FW_BOLD;
        lf.lfCharSet       = ANSI_CHARSET;
        lf.lfOutPrecision  = OUT_DEFAULT_PRECIS;
        lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
        memcpy(lf.lfFaceName, _T("Times New Roman"), _countof(lf.lfFaceName));

        m_hFont = CreateFontIndirect(&lf);


        //m_hbrBackground = ::CreateSolidBrush(RGB(0,0,0));
        m_hbrBackground = ::CreateSolidBrush(RGB(0,0,0));

        HDC hDCScreen = ::GetDC(GetDesktopWindow());
        m_hMemDC = ::CreateCompatibleDC(hDCScreen);
        m_hBitmap =::CreateCompatibleBitmap(hDCScreen, m_rcWndArea.right - m_rcWndArea.left, m_rcWndArea.bottom - m_rcWndArea.top);

        //::SetTextColor(m_hMemDC, m_clrText);
          SetBkMode(m_hMemDC, TRANSPARENT);

        ::ReleaseDC(GetDesktopWindow(), hDCScreen);

        m_hBitmapOld = (HBITMAP)::SelectObject(m_hMemDC, m_hBitmap);
        m_hFontOld   = (HFONT  )::SelectObject(m_hMemDC, m_hFont);


    }

    ~CCalibrateSymbolChecker()
    {
        if(m_hWnd)
        {
            ::DestroyWindow(m_hWnd);//销毁窗体句柄
        }
         FreeThunk(&CCalibrateSymbolChecker::WndProc, this);


        ::SelectObject(m_hMemDC, m_hBitmapOld);
        ::SelectObject(m_hMemDC, m_hFontOld);
        ::DeleteObject(m_hFont);
        ::DeleteDC(m_hMemDC);
        ::DeleteObject(m_hbrBackground);

    }


    virtual BOOL CreateWnd()
    {
        CAtlString strClassName;
        strClassName.Format(_T("Calibrate Checker Timer Window"));
        WNDCLASSEX wndclass;
        wndclass.cbSize = sizeof(wndclass);
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


        m_hWnd = ::CreateWindowEx(
            WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_LAYERED,//分层窗体。
            strClassName,
            strClassName,//windows name
            WS_POPUP,
            m_rcWndArea.left,
            m_rcWndArea.top,
            m_rcWndArea.right - m_rcWndArea.left,
            m_rcWndArea.bottom - m_rcWndArea.top,
            NULL,//owner window
            0,//Menu ID
            m_hInst,
            (LPVOID)this);

        if(m_hWnd == NULL) return FALSE;

         SetLayeredWindowAttributes(
             this->m_hWnd , //A handle to the layered window.
             RGB(0,0,0)   , //specifies the transparency color key
             255          ,//the opacity of the layered window. 
            LWA_ALPHA |   //Use bAlpha to determine the opacity of the layered window.
            LWA_COLORKEY  //se crKey as the transparency color
            );


         return TRUE;

    }


    LRESULT  WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
    {
        //AtlTrace(_T("msg=0x%x\n"), uMsg);

        switch(uMsg)
        {

        case WM_NCDESTROY:
            {
                this->m_hWnd = NULL;

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

                RECT rcClient;
                GetClientRect(hWnd, &rcClient);

                BitBlt(
                	hdc,
                	0, 
                	0, 
                	rcClient.right - rcClient.left,
                	rcClient.bottom - rcClient.top,
                	m_hMemDC,
                	0,
                	0,
                	SRCCOPY);


                EndPaint(hWnd, &ps); 
                return 0;

                break;
            }

        case WM_TIMER:
            
            break;

        case WM_KEYDOWN:
            if(wParam == VK_ESCAPE)
            {  
                Hide();
            }
            break;

        case WM_LBUTTONDOWN:
            //ShowWindow(this->m_hWnd, SW_HIDE);
            break;

        case WM_SYSCOMMAND:
            if(wParam == SC_MOVE)
            {
                return 0;
            }

            break;

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
                (LONG)MF2GF<WNDPROC>(&CCalibrateSymbolChecker::WndProc, (CCalibrateSymbolChecker*)lpcs->lpCreateParams));
        }

        return DefWindowProc(hWnd,uMsg,wParam,lParam);
    }


    BOOL IsVisible() const
    {
        if(m_hWnd)
        {
            return ::IsWindowVisible(m_hWnd);
        }

        return FALSE;
    }
public:
    //@功能:显示校正图案
    //@参数:rcDisplayMonitor, 显示器桌面尺寸
    //      pSymbolPos, 校正符号位置信息
    //      nSymbolCount, 校正符号个数
    void ShowCalibSymbols(const RECT& rcDisplayMonitor,  const POINT* pSymbolPos, int nSymbolCount, COLORREF clrSymbol, const POINT* pBackwardProjectPos = NULL)
    {
        if(nSymbolCount == 0) return;

        m_rcWndArea =  rcDisplayMonitor;

        if(NULL == m_hWnd) 
        {
            
            if(!CreateWnd())
            {

                return;
            }
        }
        else
        {
            int nWidth  = m_rcWndArea.right  - m_rcWndArea.left;
            int nHeight = m_rcWndArea.bottom - m_rcWndArea.top;

            MoveWindow(
                m_hWnd,
                m_rcWndArea.left,
                m_rcWndArea.top,
                nWidth,
                nHeight,
                FALSE   //do Not Repaint
                );

        }
        FillRect(m_hMemDC, &m_rcWndArea, m_hbrBackground);

        HPEN hPen = ::CreatePen(PS_SOLID, 2, clrSymbol);
        HPEN hPenOld = (HPEN)::SelectObject(m_hMemDC, hPen);
        HBRUSH hBrushOld = (HBRUSH)::SelectObject(m_hMemDC, GetStockObject(HOLLOW_BRUSH));

        //在校正位置处绘制校正十字
        for(int i=0; i < nSymbolCount; i++)
        {
            POINT pt = pSymbolPos[i];
            
            int x1,x2;
            int y1,y2;

            //draw horizontal line
            x1 = pt.x - m_nSymbolSize/2;
            x2 = pt.x + m_nSymbolSize/2;
            
            ::MoveToEx(m_hMemDC, x1, pt.y, NULL);
            ::LineTo(m_hMemDC, x2, pt.y);

            //draw vertical line
            y1 = pt.y - m_nSymbolSize/2;
            y2 = pt.y + m_nSymbolSize/2;

            ::MoveToEx(m_hMemDC, pt.x, y1, NULL);
            ::LineTo(m_hMemDC, pt.x, y2);

            //Draw A circle
            RECT rcCircle;
            rcCircle.left    = pt.x - m_nSymbolSize*1/3;
            rcCircle.right   = pt.x + m_nSymbolSize*1/3;
            rcCircle.top     = pt.y - m_nSymbolSize*1/3;
            rcCircle.bottom  = pt.y + m_nSymbolSize*1/3;
            
            ::Ellipse(
                m_hMemDC,
                rcCircle.left,
                rcCircle.top,
                rcCircle.right,
                rcCircle.bottom);
            
        }//for

        SelectObject(m_hMemDC, hPenOld);
        DeleteObject(hPen);


         hPen = ::CreatePen(PS_SOLID, 1, RGB(0,255,0));
         hPenOld = (HPEN)::SelectObject(m_hMemDC, hPen);
        
        if(pBackwardProjectPos)
        {//显示自动校正点采集得到的光斑位置, 反映射得到的屏幕坐标

            //在校正位置处绘制校正十字
            for(int i=0; i < nSymbolCount; i++)
            {
                POINT pt = pBackwardProjectPos[i];
                
                int x1,x2;
                int y1,y2;

                //draw horizontal line
                x1 = pt.x - m_nSymbolSize/4;
                x2 = pt.x + m_nSymbolSize/4;
                
                ::MoveToEx(m_hMemDC, x1, pt.y, NULL);
                ::LineTo(m_hMemDC, x2, pt.y);

                //draw vertical line
                y1 = pt.y - m_nSymbolSize/4;
                y2 = pt.y + m_nSymbolSize/4;

                ::MoveToEx(m_hMemDC, pt.x, y1, NULL);
                ::LineTo(m_hMemDC, pt.x, y2);

                //Draw A circle
                RECT rcCircle;
                rcCircle.left    = pt.x - m_nSymbolSize*1/3;
                rcCircle.right   = pt.x + m_nSymbolSize*1/3;
                rcCircle.top     = pt.y - m_nSymbolSize*1/3;
                rcCircle.bottom  = pt.y + m_nSymbolSize*1/3;
                
                //::Ellipse(
                //    m_hMemDC,
                //    rcCircle.left,
                //    rcCircle.top,
                //    rcCircle.right,
                //    rcCircle.bottom);
                
            }//for
        }
        SelectObject(m_hMemDC, hPenOld);
        DeleteObject(hPen);

       SelectObject(m_hMemDC, hBrushOld);

       ShowWindow(this->m_hWnd, SW_SHOW);

       
       UpdateWindow(this->m_hWnd);

    }


    //@功能:
    void Hide()
    {
        if(this->m_hWnd)
        {
            ShowWindow(this->m_hWnd, SW_HIDE);

            //销毁窗体
            DestroyWindow(this->m_hWnd);
        }
    }



protected:
    HBRUSH m_hbrBackground;
    HWND  m_hWnd          ;
    RECT  m_rcWndArea     ;//
    HFONT m_hFont         ;//计数显示的字体
    HFONT m_hFontOld      ;//
    
    COLORREF m_clrSymbol  ;//校正符号颜色
    int      m_nSymbolSize ;//校正符号尺寸

    HINSTANCE m_hInst;
    HDC       m_hMemDC;
    HBITMAP   m_hBitmap;
    HBITMAP   m_hBitmapOld;

   
};