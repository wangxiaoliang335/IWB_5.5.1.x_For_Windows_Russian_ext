#pragma once

#ifndef SAFE_RELEASE

#define SAFE_RELEASE( x )  \
    if ( NULL != x )       \
    {                      \
        x->Release( );     \
        x = NULL;          \
    }

#endif



class CInterceptFilter;

//
//#define DT_TOP                      0x00000000
//#define DT_LEFT                     0x00000000
//#define DT_CENTER                   0x00000001
//#define DT_RIGHT                    0x00000002
//#define DT_VCENTER                  0x00000004
//#define DT_BOTTOM                   0x00000008
//#define DT_WORDBREAK                0x00000010
//#define DT_SINGLELINE               0x00000020
//#define DT_EXPANDTABS               0x00000040
//#define DT_TABSTOP                  0x00000080
//#define DT_NOCLIP                   0x00000100
//#define DT_EXTERNALLEADING          0x00000200
//#define DT_CALCRECT                 0x00000400
//#define DT_NOPREFIX                 0x00000800
//#define DT_INTERNAL                 0x00001000
//OSD(Over Screen Display Text)
class TOSDText
{
public:
    //struct RectF
    //{
    //    float left;
    //    float top;
    //    float right;
    //    float bottom;
    //};

	TOSDText()
		:
		m_strText(_T("")),
		m_dwDrawTextFormat(0U),
		m_lFontSize(10),
		m_strFontFaceName(_T("Times New Roman")),
		m_hFont(NULL),
		m_nDisplayTimesCounter(-1)
	{
		m_rcDisplayArea.left   = 0.0;
        m_rcDisplayArea.top    = 0.0;
		m_rcDisplayArea.right  = 1.0;
		m_rcDisplayArea.bottom = 1.0;

	}
	TOSDText(
		const TCHAR* szText,
		const RectF&  rcDisplayArea,
		UINT dwDrawTextFormat,
		LONG lFontSize,
		const TCHAR* lpszFontFaceName	
		)
		:
		m_strText(szText),
		m_dwDrawTextFormat(dwDrawTextFormat),
		m_lFontSize(lFontSize),
		m_strFontFaceName(lpszFontFaceName),
		m_hFont(NULL)

	{

		m_rcDisplayArea = rcDisplayArea;

	}

	~TOSDText()
	{
		if (m_hFont)
		{
			::DeleteObject(m_hFont);
		}
	}

	void SetFont(LONG lFontSize, const TCHAR* lpszFontFaceName)
	{
		if (lFontSize != m_lFontSize || _tcsicmp((LPCTSTR)m_strFontFaceName, lpszFontFaceName) != 0)
		{
			if (m_hFont)
			{
				::DeleteObject(m_hFont);
				m_hFont = NULL;
			}
			m_lFontSize = lFontSize;
			m_strFontFaceName = lpszFontFaceName;

			HDC hdcDesktop = GetDC(GetDesktopWindow());
			LOGFONT lf;
			memset(&lf, 0, sizeof(LOGFONT));
			lf.lfHeight = -MulDiv(m_lFontSize, GetDeviceCaps(hdcDesktop, LOGPIXELSY), 72);;
			lf.lfWidth = 0;
			lf.lfWeight = FW_BOLD;
			lf.lfCharSet = ANSI_CHARSET;
			lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
			lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
			memcpy(lf.lfFaceName, (LPCTSTR)m_strFontFaceName, _countof(lf.lfFaceName));
			m_hFont = CreateFontIndirect(&lf);
			ReleaseDC(GetDesktopWindow(), hdcDesktop);

		}
	}

	HFONT GetFont()
	{
		if (NULL == m_hFont)
		{
			HDC hdcDesktop = GetDC(GetDesktopWindow());
			LOGFONT lf;
			memset(&lf, 0, sizeof(LOGFONT));
			lf.lfHeight = -MulDiv(m_lFontSize, GetDeviceCaps(hdcDesktop, LOGPIXELSY), 72);;
			lf.lfWidth = 0;
			lf.lfWeight = FW_BOLD;
			lf.lfCharSet = ANSI_CHARSET;
			lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
			lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
			memcpy(lf.lfFaceName, (LPCTSTR)m_strFontFaceName, _countof(lf.lfFaceName));
			m_hFont = CreateFontIndirect(&lf);
			ReleaseDC(GetDesktopWindow(), hdcDesktop);
		}

		return m_hFont;
	}

	void SetText(const TCHAR* lpszText)
	{
		m_strText = lpszText;
	}

	const TCHAR* GetText()const
	{
		return (LPCTSTR)m_strText;
	}


	void SetDisplayArea(const RectF& rcDisplayArea, UINT dwDrawTextFormat)
	{
		m_rcDisplayArea    = rcDisplayArea;
		m_dwDrawTextFormat = dwDrawTextFormat;
	}

	const RectF& GetDisplayArea()const
	{
		return m_rcDisplayArea;
	}

	UINT GetDrawTextFormat()const
	{
		return m_dwDrawTextFormat;
	}


	void DecDisplayTimesCounter()
	{
		if (m_nDisplayTimesCounter >= 0)
		{
			m_nDisplayTimesCounter--;
		}

	}

	void SetDisplayTimes(int nTimes)
	{
		m_nDisplayTimesCounter = nTimes;
	}

	int GetDisplayTimesCounter()const 
	{
		return m_nDisplayTimesCounter;
	}
	LONG GetFontSize()const
	{
		return m_lFontSize;
	}

	LPCTSTR GetFontName()const
	{
		return (LPCTSTR)m_strFontFaceName;
	}
    
	
protected:
	CString      m_strText;
	HFONT        m_hFont;
    RectF        m_rcDisplayArea;//坐标归一化到[0,1]内的矩形
	UINT         m_dwDrawTextFormat;//API DrawText的Format参数
	LONG         m_lFontSize;
	CString      m_strFontFaceName;

	int          m_nDisplayTimesCounter;//显示次数计数器。-1，无穷次；0：停止显示
};



enum EOSDTextType
{
    E_OSDTEXT_TYPE_ERROR_INFO  = 0,//校正信息
    E_OSDTEXT_TYPE_GUIDE_BOX   = 1,//引导框
    E_OSDTEXT_TYPE_FORMAT_INFO = 2,//视频格式信息
	E_OSDTEXT_TYPE_SHOW_INFO   = 3,
	E_OSDTEXT_TYPE_LENS_INFO   = 4,//固件中的镜头
    E_OSDTEXT_TYPE_COUNT       = 5
};

class CVideoPlayer
{
public:
	CVideoPlayer(int nID=0);

	~CVideoPlayer();

    void SetInterceptFilter(CInterceptFilter* pFilter);
    CInterceptFilter* GetInterceptFilter();


	HRESULT Stop();

	//@功能:开始侦测
	//@参数:hPlayWnd,       播放窗体句柄
	//      rcDispArea,     视频显示区域
	//      hNotifyWnd,     通知消息接收窗体的句柄
	//      lpszDevicePath, 设备路径
	//      pVideoInfoHeader, 指向指定的视频格式的指针
	//@返回值:
	//   成功,返回TRUE
	//   失败,返回FALSE
	BOOL StartDetect(HWND hPlayWnd, const RECT& rcDispArea, HWND hNotifyWnd, LPCTSTR lpszDevicePath,  const VIDEOINFOHEADER* pVideoInfoHeader);


	//@功能:开始侦测
	//@参数:lpszDevicePath, 设备路径
	//      pVideoInfoHeader, 指向指定的视频格式的指针
	//@返回值:
	//   成功,返回TRUE
	//   失败,返回FALSE
	BOOL StartDetect(LPCTSTR lpszDevicePath,  const VIDEOINFOHEADER* pVideoInfoHeader);

	BOOL StopDetect();

	void MissStatusInfo();

    //@功能:在窗体中显示视频帧
    //
    //@参数:pYUY2Frame, YUY2图片帧
    //      nImageWidth,  图片宽度
    //      nImageHeight, 图片高度
    //BOOL CVideoPlayer::DisplayFrame(const BYTE* pYUY2Frame, int nImageWidht, int nImageHeight);

    //@功能:在窗体中显示视频帧
    //@参数:pARGBFrame  , ARGB图片帧
    //      nImageWidth , 图片宽度
    //      nImageHeight, 图片高度
    BOOL DisplayFrame(const BYTE* pARGBFrame, int nImageWidth, int nImageHeight);

    const RECT& GetDisplayArea() const;
    void SetDisplayArea(const RECT& rcDispArea);
	void SetDisplayWnd(HWND hWnd);
	void SetNotifyWnd(HWND hWnd);


	////@功能:设置亮度检测门限
	//BOOL SetYThreshold(BYTE threshold);

	////@返回:亮度检测门限
	//BYTE GetThreshold()const;


	//设置接收通知消息的窗体
	void SetNotifyMessage(HWND hWnd, UINT uMsg);

	BOOL IsDetecting()const;
	BOOL ResizeVideo(const RECT& rcNewSize);

	////@功能:重置视频剪切区域
	//BOOL ResetVideoClipArea();

	////@功能:设置视频剪切区域
	//BOOL SetVideoClipArea(const RECT& rcClipArea);


	BOOL ChangeCaptureFreq(int  nFrequency);

	BOOL SetImageFormat(const VIDEOINFOHEADER& vih);

	HRESULT ShowPropertyPage(HWND hWndParent);

	//CPenPosDetector* GetPenPosDetector();
	//const CPenPosDetector* GetPenPosDetector() const;

	IBaseFilter* GetCaptureFilter();


	
	//@功能:设置Capture Filter的Gamma值
	//@参数:lGammaValue, Specifies the gamma, as gamma * 100. Values range from 1 to 500. 
	//BOOL ChangeGammaValue(LONG nGammaValue);


    //@功能:更改摄像头参数    
    BOOL SetCameraParams(const TVideoProcAmpProperty& newParams);


    //@功能:获取摄像头参数
    BOOL GetCameraParams(TVideoProcAmpProperty& saveParams);

    //@功能:设置摄像的单个参数
    BOOL SetCameraParams(long  lValue, VideoProcAmpProperty eProperty);

    //@功能:获取摄像头亮度
    BOOL GetCameraBrightness(long & lValue, long& lMax, long& lMin, long& lSteppingDelta, long& lDefault);

    //@功能:获取摄像头对比度
    BOOL GetCameraContrast(long & lValue, long& lMax, long& lMin, long& lSteppingDelta, long& lDefault);

    //@功能:获取摄像头Gamma值
    BOOL GetCameraGamma(long & lValue, long& lMax, long& lMin, long& lSteppingDelta, long& lDefault);


    //@功能:设置摄像头的放大增益系数
    //@说明:0x33AA,打开IRCUT的滤光片
    //@说明:0x44AA,关闭IRCUT的滤光片
    //BOOL SetCameraGain(const long & lGain);

    HWND GetPlayWnd()const
    {
        return m_hPlayWnd;
    }


    //@功能:在视频画面中显示文字
    void SetDisplayInfo(LPCTSTR lpszInfo, int nDisplayTimes=300);



	//@功能:增加OSD显示文字
	//
	BOOL AddOSDText(
		EOSDTextType eOSDTextType,
		const TCHAR* szContent,
		//const TOSDText::RectF&  rcDisplayArea,
        const RectF&  rcDisplayArea,
		UINT deDrawTextFormat,
		LONG lFontSize,
		const TCHAR* lpszFontFaceName,
		int nDisplayTimes);


	void ClearOSDText(EOSDTextType eOSDTextType);

	void UpdateVideoStreamForamtInfo(int nImageWidth, int nImageHeight, DWORD ImageType, float fps,int nId);
	CString   CurrentCameraResolution();


    BOOL GetVideoSize(SIZE&  size);

protected:
    //
	HRESULT PlayVideo(HWND hWnd = NULL, HWND hNotifyWnd=NULL);

	HRESULT InitializeVideo();

	void UnInitializeVideo();

	HRESULT GetInterfaces();
	HRESULT CloseInterfaces();

	HRESULT BuildGraph();
	HRESULT FindCaptureDevice(IBaseFilter **ppSrcFilter);

    //在画面中显示OSD文字信息
    void DisplayText(HDC hDC, int nImageWidth, int nImageHeight);


	//IVideoWindow* m_pVW ; 
	IMediaControl* m_pMC ;
	IMediaEventEx* m_pME ;	
	//ICaptureGraphBuilder2* m_pCaptureGraphBuilder;
	IGraphBuilder* m_pFilterGraph;

	IBaseFilter* m_pCaptureFilter;

	//IVMRWindowlessControl9* m_pWC;
    IVMRWindowlessControl* m_pWC;



	HWND m_hPlayWnd  ;//播放窗体句柄
    RECT m_rcDispArea;//视频显示区域
   // CDib m_oRealFrameDib;
   //CImageFrameT<DWORD> m_oRGB;
    CDibCanvas m_oRealFrameARGB;

    CDibCanvas m_oMemFrame;//内存DC, 避免画面闪烁

    HWND m_hNotifyWnd;

    //图像拦截滤器
	CInterceptFilter* m_pInterceptFilter;


	DWORD m_dwRegister;
	BOOL m_bIsDetecting;

	//
	//CAtlString m_strCaptureDeviceName;
	//int        m_nCaptureDeviceInstNo;
	CAtlString   m_strCaptureDevicePath;
	VIDEOINFOHEADER m_tDesiredImageFormat;

	//

	DWORD m_dwGraphRegister;
	BOOL m_bIsClosing;

    enum EVideoState
    {
        E_VIDEO_RUNNING,//视频正在播放
        E_VIDEO_STOP   ,//视频停止
        E_VIDEO_ERR    ,//视频错误
    }m_eVideoState;


    int m_nID;//关联的编号

    //CString  m_strDisplayText                ;//显示文字
    int      m_nDisplayTimesCounter            ;//显示次数计数
    //BOOL     m_bTextIsValidate               ;//文字有效标志
    static const int  MAX_DISPLAY_TIMES = 300;
    //HFONT m_hFont;



	TOSDText m_osdtext[E_OSDTEXT_TYPE_COUNT];

	CString    m_sCurrentCameraResolution;

	TCHAR m_szStatusText[256];
};
