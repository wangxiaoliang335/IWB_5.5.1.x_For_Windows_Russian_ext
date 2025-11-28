#pragma once
//@功能:显示校正符号的手动再采集
typedef void  (_stdcall* OnManualSampleDoneCallBackProc)(LPVOID lpCtx, BOOL bSuccess);

class CCalibrateSymbolManualSampler
{
public:
    //@参数:clrText, 缺省的文字颜色。
    CCalibrateSymbolManualSampler()
        :
    m_hWnd (NULL),
    m_clrSymbol(RGB(255,0,0)),
    m_nSymbolSize(30),
    m_fpSampleDoneProc(NULL)
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

    ~CCalibrateSymbolManualSampler()
    {
        if(m_hWnd)
        {
            ::DestroyWindow(m_hWnd);//销毁窗体句柄
        }
         
        FreeThunk(&CCalibrateSymbolManualSampler::WndProc, this);


        ::SelectObject(m_hMemDC, m_hBitmapOld);
        ::SelectObject(m_hMemDC, m_hFontOld);
        ::DeleteObject(m_hFont);
        ::DeleteDC(m_hMemDC);
        ::DeleteObject(m_hbrBackground);

    }


    virtual BOOL CreateWnd()
    {
        CAtlString strClassName;
        strClassName.Format(_T("Calibrate Symbol Manual Sampler Window"));
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
            //WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_LAYERED,//分层窗体。
            WS_EX_TOOLWINDOW|WS_EX_TOPMOST,
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

		/*
         SetLayeredWindowAttributes(
             this->m_hWnd , //A handle to the layered window.
             RGB(0,0,0)   , //specifies the transparency color key
             255          ,//the opacity of the layered window. 
            LWA_ALPHA |   //Use bAlpha to determine the opacity of the layered window.
            LWA_COLORKEY  //se crKey as the transparency color
            );
        */


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
            {
                UINT uTimerId = (UINT)wParam;
                if(uTimerId == PERIODIC_CHECK_EVENT_ID)
                {
                    TStateMachineInputStimulus stimulus;
                    stimulus.eStimulusType = E_STIMULUS_TYPE_TIMER;
                    stimulus.uTimerId      = uTimerId;

                    StateMachineProc(stimulus);
                }
            }
            
            break;

        case WM_KEYDOWN:
            if(wParam == VK_ESCAPE)
            {  
                if(m_fpSampleDoneProc)
               {
                   m_fpSampleDoneProc(m_lpCtx, TRUE);
               }

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
            
        case WM_CALIBRATE_MANUAL_RESAMPLE://光斑检测器发送来的光斑位置和屏幕坐标信息。
            {
                    //校正数据输入
                    POINT ptVideo;
                    ptVideo.x = LOWORD(wParam);
                    ptVideo.y = HIWORD(wParam);

                    POINT ptScreen;
                    ptScreen.x = LOWORD(lParam);
                    ptScreen.y = HIWORD(lParam);

                    TStateMachineInputStimulus stimulus;
                    stimulus.eStimulusType = E_STIMULUS_TYPE_SPOT_DATA;
                    stimulus.coords.ptLightSpot   = ptVideo;
                    stimulus.coords.ptScreen      = ptScreen;
                    StateMachineProc(stimulus);
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
                (LONG)MF2GF<WNDPROC>(&CCalibrateSymbolManualSampler::WndProc, (CCalibrateSymbolManualSampler*)lpcs->lpCreateParams));
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


    HWND GetHwnd() const { return this->m_hWnd;}

    //@功能:获取校正图案在视频中的坐标。
    const std::vector<TPoint2D> & GetSymbolsCoordInVideo()
    {
        return this->m_vecSymbolsInVideo;
    }

    //@功能:获取光斑在屏幕中的坐标。
    const std::vector<TPoint2D> & GetLightSpotsCoordInVideo()
    {
        return this->m_vecLightSpotsInVideo;
    }

    const std::vector<POINT> & GetActualTouchPosInScreen()
    {
        return this->m_vecActualTouchPosInScreen;
    }


    const std::vector<POINT> & GetSymbolPosInScreen()
    {
        return this->m_vecSymoblsInScreen;
    }



public:
    //@功能:显示校正图案
    //@参数:rcDisplayMonitor, 显示器桌面尺寸
    //      pSymbolPosInScreen, 校正符号在计算机屏幕上的位置坐标数组
    //      pSymbolPosInVideo, 校正符号在摄像头视频中的位置坐标数组
    //      nSymbolCount, 校正符号个数
    //      clrSymbol,校正符号颜色
    //      fpCallback, 回调函数
    //      lpCtx，回调函数参数
    void DoManualResample(const RECT& rcDisplayMonitor,  const POINT* pSymbolPosInScreen, const TPoint2D* pSymbolPosInVideo, int nSymbolCount, COLORREF clrSymbol, OnManualSampleDoneCallBackProc fpCallback, LPVOID lpCtx)
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

         ShowWindow(this->m_hWnd, SW_SHOW);

        //填充背景
        FillRect(m_hMemDC, &rcDisplayMonitor, m_hbrBackground);


        //保存校正点屏幕坐标数据
        m_vecSymoblsInScreen.resize(nSymbolCount);
        memcpy(&m_vecSymoblsInScreen[0], pSymbolPosInScreen, sizeof(POINT)*nSymbolCount);


        //保存校正点摄像头图像坐标数据
        m_vecSymbolsInVideo.resize(nSymbolCount);
        memcpy(&m_vecSymbolsInVideo[0], pSymbolPosInVideo, sizeof(TPoint2D)*nSymbolCount);


        //分配采样点视频坐标数组
        m_vecLightSpotsInVideo.resize(nSymbolCount);


        //实际触控点数组
        m_vecActualTouchPosInScreen.resize(nSymbolCount);


        //开启检查定时器
        m_uPeriodicCheckTimerId = SetTimer(m_hWnd, PERIODIC_CHECK_EVENT_ID, PERIODIC_CHECK_INTERVAL, NULL);

        m_fpSampleDoneProc = fpCallback;
        m_lpCtx            = lpCtx;

        m_clrSymbol = clrSymbol;

        
        //状态机初始化
        m_nSymbolIndex = 0;
        m_eStage = E_FIRST_STAGE;


        DrawSymbol(m_vecSymoblsInScreen[0], m_clrSymbol);

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


    //激励类型
    enum E_StimulusType
    {
        E_STIMULUS_TYPE_SPOT_DATA,//光斑数据
        E_STIMULUS_TYPE_TIMER    ,//定时器
    };

    //状态机输入激励
    struct TStateMachineInputStimulus
    {
        E_StimulusType eStimulusType;//激励类型枚举值
        union
        {
            struct 
            {
                POINT ptLightSpot;//光斑视频坐标
                POINT ptScreen   ;//光斑屏幕坐标
            }coords;

            UINT  uTimerId   ;//定时器Id
        };
    };

    void StateMachineProc(const TStateMachineInputStimulus& stimulus)
    {

        switch(m_eStage)
        {
        case E_FIRST_STAGE:
            if(E_STIMULUS_TYPE_SPOT_DATA == stimulus.eStimulusType )
            {
                m_nLightSpotDetectionCount = 0;
                m_ptVideoInput = stimulus.coords.ptLightSpot;
                //m_ptScreenInput = stimulus.coords.ptScreen;
                m_eStage = E_WAIT_STAGE;
            }
            break;


        case E_WAIT_STAGE:
            if(E_STIMULUS_TYPE_SPOT_DATA == stimulus.eStimulusType )
            {
                const POINT& ptVideoNew  = stimulus.coords.ptLightSpot;
                const POINT& ptScreenNew = stimulus.coords.ptScreen;
                
                int deltaU = abs(ptVideoNew.x - m_ptVideoInput.x);
                int deltaV = abs(ptVideoNew.y - m_ptVideoInput.y);
                if(deltaU < MAX_PERMIT_U_DITHER_DEVIATION && deltaV < MAX_PERMIT_V_DITHER_DEVIATION)
                {
                    m_nLightSpotDetectionCount ++;

                    if (1 == m_nLightSpotDetectionCount)
                    {
                        m_dwSingleSampleStartTime = GetTickCount();
                    }

                    //数据平滑处理
                    //视频坐标平滑
                    //m_ptVideoInput.x = m_ptVideoInput.x * 7/10 +  ptVideoNew.x * 3/10;
                    //m_ptVideoInput.y = m_ptVideoInput.y * 7/10 +  ptVideoNew.y * 3/10;

                    m_ptVideoInput.x = ptVideoNew.x;
                    m_ptVideoInput.y = ptVideoNew.y;

                    //屏幕坐标平滑处理
                   // m_ptScreenInput.x = m_ptScreenInput.x * 7/10 + ptScreenNew.x * 3/10;
                   // m_ptScreenInput.y = m_ptScreenInput.y * 7/10 + ptScreenNew.y * 3/10;


                    if(m_nLightSpotDetectionCount > MAX_DETECT_COUNT)
                    {//采集结束

                        DWORD dwNow = GetTickCount();
                        DWORD dwElapse = dwNow - m_dwSingleSampleStartTime;
                        if(dwElapse > 500)
                        {
                            //记录光斑位置信息
                            m_vecLightSpotsInVideo[m_nSymbolIndex].d[0] = double(m_ptVideoInput.x) / double(1 << INT_SCALE_SHIFT_SIZE);
                            m_vecLightSpotsInVideo[m_nSymbolIndex].d[1] = double(m_ptVideoInput.y) / double(1 << INT_SCALE_SHIFT_SIZE);

                            m_vecActualTouchPosInScreen[m_nSymbolIndex] = ptScreenNew;

                            AtlTrace(_T("du=%f,dv=%f\n"), ptVideoNew.x - m_ptVideoInput.x, ptVideoNew.y - m_ptVideoInput.y);
                            //符号变颜色
                            DrawSymbol(m_vecSymoblsInScreen[m_nSymbolIndex], RGB(0, 255, 0));


                        m_dwLastSpotAppearTime = GetTickCount();

                            //跳到数据采集结束阶段
                            m_eStage = E_END_STAGE;
                        }
                    }

                }
                else
                {
                    //回到第一阶段
                    m_eStage = E_FIRST_STAGE;
                    m_nLightSpotDetectionCount = 0;
                }
            }
            break;
        
        case E_END_STAGE:
            {
                BOOL bEnd = FALSE;

                if(E_STIMULUS_TYPE_SPOT_DATA == stimulus.eStimulusType )
                {
                     m_dwLastSpotAppearTime = GetTickCount();
                 }
                 else if(E_STIMULUS_TYPE_TIMER == stimulus.eStimulusType )
                 {
                    //定时检查时间到
                    DWORD dwNow = GetTickCount();
                    DWORD dwElapse = dwNow - m_dwLastSpotAppearTime;

                    if(dwElapse > SAMPLE_END_TIME_THRESHOLD)
                    {
                        //长时间没有光斑输入数据, 可以认为坐标采样结束
                        bEnd = TRUE;
                    }
                 }
                
                 if(bEnd)
                 {

                    if(m_nSymbolIndex == m_vecSymoblsInScreen.size() -1)
                    {
                        if(m_fpSampleDoneProc)
                        {
                            m_fpSampleDoneProc(m_lpCtx, TRUE);
                        }

                        Hide();

                        m_eStage = E_ALL_DONE;
                    }
                    else
                    {
                        m_nSymbolIndex ++;
                        DrawSymbol(m_vecSymoblsInScreen[m_nSymbolIndex], m_clrSymbol);
                        m_eStage = E_FIRST_STAGE;

                    }

                 }

            }
            
            break;

        case E_ALL_DONE:

            break;
        }//switch


    }



    void DrawSymbol(POINT & pt, COLORREF clrSymbol)
    {

        HPEN hPen = ::CreatePen(PS_SOLID, 2, clrSymbol);

        HPEN hPenOld = (HPEN)::SelectObject(m_hMemDC, hPen);

        HBRUSH hBrushOld = (HBRUSH)::SelectObject(m_hMemDC, GetStockObject(HOLLOW_BRUSH));

        //在校正位置处绘制校正十字
        //for(int i=0; i <= m_nSymbolIndex; i++)
        //{

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
            
        //}//for
        

        SelectObject(m_hMemDC, hBrushOld);
        SelectObject(m_hMemDC, hPenOld);
        DeleteObject(hPen);


        InvalidateRect(this->m_hWnd, NULL, FALSE);
        UpdateWindow(this->m_hWnd);

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


    std::vector<POINT>   m_vecSymoblsInScreen    ;//校正点屏幕坐标数组,输入时为校正符号的校正位置，输出为实际的光标屏幕坐标。
    std::vector<TPoint2D> m_vecSymbolsInVideo    ;//校正符号在视频中的位置
    std::vector<TPoint2D> m_vecLightSpotsInVideo ;//光斑在视频中的位置
    std::vector<POINT>    m_vecActualTouchPosInScreen;//在屏幕上的实际触控坐标
    

    int                m_nSymbolIndex;//校正点所引号

    enum EStage
    {
        E_FIRST_STAGE = 1,//当前点开始采集阶段
        E_WAIT_STAGE  = 2, //等待阶段
        E_END_STAGE   = 3, //当前点采集结束阶段
        E_ALL_DONE    = 4, //结束了
    };

    EStage m_eStage;//采样阶段。

    POINT  m_ptVideoInput  ;//光斑的视频坐标,
    //POINT  m_ptScreenInput ;//光斑对应的屏幕坐标
    static const int PERIODIC_CHECK_EVENT_ID   = 1  ;//周期检测定时器Id
    static const int PERIODIC_CHECK_INTERVAL   = 100;//100ms定时器
    static const int SAMPLE_END_TIME_THRESHOLD = 300;//判断"校正点校正结束"的光斑消失时间门限,单位:ms

    UINT m_uPeriodicCheckTimerId;//校正时的定时器


    
    int m_nLightSpotDetectionCount;//光斑检测次数
    static const int MAX_DETECT_COUNT  = 10;//每个校正点从开始到坐标采集完毕时需要采集的次数
    DWORD m_dwSingleSampleStartTime;//单次采样开始时间

    static const int MAX_PERMIT_U_DITHER_DEVIATION = 3 << INT_SCALE_SHIFT_SIZE ;//最大允许的由于光斑抖动引起的水平偏移量, 单位:视频坐标
    static const int MAX_PERMIT_V_DITHER_DEVIATION = 3 << INT_SCALE_SHIFT_SIZE;//最大允许的由于光斑抖动引起的垂直偏移量, 单位:视频坐标


     DWORD       m_dwLastSpotAppearTime;//上一次光斑显示时刻

     OnManualSampleDoneCallBackProc  m_fpSampleDoneProc;
     LPVOID            m_lpCtx;

   
};