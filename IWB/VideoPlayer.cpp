#include "stdafx.h"
//#include "headers.h"
//CLSID_NullRenderer is define din qedit.h
//#include <qedit.h>
EXTERN_C const CLSID CLSID_NullRenderer;

CVideoPlayer::CVideoPlayer(int nID)
:
//m_pVW(NULL),
m_pMC(NULL),
m_pME(NULL),
m_pFilterGraph(NULL),
//m_pCaptureGraphBuilder(NULL),
m_pCaptureFilter(NULL),
m_hPlayWnd(NULL),
m_hNotifyWnd(NULL),
m_pInterceptFilter(NULL),
m_bIsDetecting(FALSE),
m_pWC(NULL),
m_dwGraphRegister(0),
m_bIsClosing(FALSE),
m_nID(nID),
//m_bTextIsValidate(FALSE),
m_nDisplayTimesCounter(0),
m_sCurrentCameraResolution("")
{
    LOG_FUNC_CALL;

    m_eVideoState = E_VIDEO_STOP;

    DbgSetModuleLevel(LOG_TRACE, 5);
    InitializeVideo();

	memset(m_szStatusText, sizeof(m_szStatusText), 0);
    //HDC hdcDesktop = GetDC(GetDesktopWindow());
    //LONG PointSize = 40;

    //LOGFONT lf;
    //memset(&lf, 0, sizeof(LOGFONT));
    //lf.lfHeight        =  -MulDiv(PointSize, GetDeviceCaps(hdcDesktop, LOGPIXELSY), 72);;
    //lf.lfWidth         = 0;
    //lf.lfWeight        = FW_BOLD;
    //lf.lfCharSet       = ANSI_CHARSET;
    //lf.lfOutPrecision  = OUT_DEFAULT_PRECIS;
    //lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    //memcpy(lf.lfFaceName, _T("Times New Roman"), _countof(lf.lfFaceName));
    //m_hFont    = CreateFontIndirect(&lf);

    //ReleaseDC(GetDesktopWindow(), hdcDesktop);

}

CVideoPlayer::~CVideoPlayer()
{
    LOG_FUNC_CALL;

    UnInitializeVideo();

    //DeleteObject(m_hFont);


}

void CVideoPlayer::SetInterceptFilter(CInterceptFilter* pFilter)
{
    m_pInterceptFilter = pFilter;
}


HRESULT CVideoPlayer::InitializeVideo()
{
    HRESULT hr = S_OK;

    //COM Library Intialization
    hr = CoInitialize(NULL);

    memset(&m_tDesiredImageFormat, 0 ,sizeof(m_tDesiredImageFormat));
    return hr ;
}


CInterceptFilter* CVideoPlayer::GetInterceptFilter()
{
    return this->m_pInterceptFilter;
}


void CVideoPlayer::UnInitializeVideo()
{

    StopDetect();
    CloseInterfaces(); 
    CoUninitialize();
}


HRESULT CVideoPlayer::GetInterfaces()
{
    HRESULT hr;

    if(m_pFilterGraph == NULL){
        // Create the filter graph
        hr = CoCreateInstance (CLSID_FilterGraph, NULL, CLSCTX_INPROC,
            IID_IGraphBuilder, (void **) &m_pFilterGraph);
        if (FAILED(hr))
            return hr;
    }
    else
        return E_FAIL ;

#ifdef REGISTER_FILTERGRAPH
    hr = AddToRot(m_pFilterGraph, &m_dwRegister);
#endif

    //if(m_pCaptureGraphBuilder == NULL) {
    //	// Create the capture graph builder
    //	hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
    //		IID_ICaptureGraphBuilder2, (void **) &m_pCaptureGraphBuilder);
    //	if (FAILED(hr))
    //		return hr;
    //}
    //else
    //	return E_FAIL ;

    // Obtain interfaces for media control and Video Window
    hr = m_pFilterGraph->QueryInterface(IID_IMediaControl,(LPVOID *) &m_pMC);
    if (FAILED(hr))
        return hr;

    //hr = m_pFilterGraph->QueryInterface(IID_IVideoWindow, (LPVOID *) &m_pVW);
    //if (FAILED(hr))
    //	return hr;

    hr = m_pFilterGraph->QueryInterface(IID_IMediaEvent, (LPVOID *) &m_pME);
    if (FAILED(hr))
        return hr;


    // Set the window handle used to process graph events
    hr = m_pME->SetNotifyWindow((OAHWND)this->m_hNotifyWnd, WM_GRAPHNOTIFY, 0);


    return hr;
}

HRESULT CVideoPlayer::CloseInterfaces()
{
    //LOG("enter %s", __FUNCTION__);
	m_bIsClosing =TRUE;
    if(m_pFilterGraph)
    {

#ifdef _DEBUG
        PrintFilterGraph(m_pFilterGraph);
#endif

    }

    HRESULT hr = S_OK;
    // Stop previewing data
    if (m_pMC)
        m_pMC->Stop();

    // Stop receiving events
    if (m_pME)
        m_pME->SetNotifyWindow(NULL, WM_GRAPHNOTIFY, 0);

    // Relinquish ownership (IMPORTANT!) of the video window.
    // Failing to call put_Owner can lead to assert failures within
    // the video renderer, as it still assumes that it has a valid
    // parent window.
    //if(m_pVW)
    //{
    //	m_pVW->put_Visible(OAFALSE);
    //	m_pVW->put_Owner(NULL);
    //}
#ifdef REGISTER_FILTERGRAPH
    if(m_dwRegister)
    {
        RemoveFromRot(m_dwRegister);
        m_dwRegister = 0;
    }

#endif

    // Release DirectShow interfaces
    SAFE_RELEASE(m_pMC);
    SAFE_RELEASE(m_pME);
    //SAFE_RELEASE(m_pVW);
    SAFE_RELEASE(m_pWC);



    if(m_pFilterGraph)
    {

#ifdef _DEBUG
        PrintFilterGraph(m_pFilterGraph);
#endif

        CComPtr<IEnumFilters> pEnum = NULL;

        hr = m_pFilterGraph->EnumFilters(&pEnum);

        IBaseFilter *pFilter = NULL;

        while(S_OK == pEnum->Next(1, &pFilter, NULL))
        {
            //Remove the filter
            m_pFilterGraph->RemoveFilter(pFilter);

            //Reset the enumerator
            pEnum->Reset();

            ULONG uRef = pFilter->Release();
            AtlTrace(_T("pFilter 0x%x ref count %d.\r\n"), pFilter, uRef);
        }
    }


    SAFE_RELEASE(m_pCaptureFilter);

//    SAFE_RELEASE(m_pInterceptFilter);

    //SAFE_RELEASE(m_pFilterGraph);

    if(m_pFilterGraph)
    {

        ULONG uRef = m_pFilterGraph->Release();
        AtlTrace(_T("Filter Graph 0x%x Ref Count=%d\r\n"),m_pFilterGraph, uRef);
        m_pFilterGraph = NULL;
    }

    //SAFE_RELEASE(m_pCaptureGraphBuilder);

    //LOG("leave %s", __FUNCTION__);
	m_bIsClosing =FALSE;
    return hr;
}

HRESULT CVideoPlayer::FindCaptureDevice(IBaseFilter **ppSrcFilter)
{

    HRESULT hr;
    IBaseFilter * pSrc = NULL;
    CComPtr <IMoniker> pMoniker =NULL;
    ULONG cFetched;

    if (!ppSrcFilter)
        return E_POINTER;

    // Create the system device enumerator  
    CComPtr <ICreateDevEnum> pDevEnum =NULL;

    hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
        IID_ICreateDevEnum, (void **) &pDevEnum);
    if (FAILED(hr))
    {
        return hr;
    }

    // Create an enumerator for the video capture devices
    CComPtr <IEnumMoniker> pClassEnum = NULL;

    hr = pDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
    if (FAILED(hr))
    {
        return hr;
    }

    // If there are no enumerators for the requested type, then 
    // CreateClassEnumerator will succeed, but pClassEnum will be NULL.
    if (pClassEnum == NULL)
    {
        return E_FAIL;
    }

    // Use the first video capture device on the device list.
    // Note that if the Next() call succeeds but there are no monikers,
    // it will return S_FALSE (which is not a failure).  Therefore, we
    // check that the return code is S_OK instead of using SUCCEEDED() macro.

    //To Find the Analog Capture Devices....

    BOOL         Found ;
    IPin        *pP = 0;
    IEnumPins   *pins=0;
    //ULONG        n;
    //PIN_INFO     pinInfo;
    Found	= FALSE;
    IKsPropertySet *pKs=0;
    //GUID guid;
    //DWORD dw;
    BOOL fMatch = FALSE;
    CComPtr<IPropertyBag> ptrPropBag;

    int nFoundDeviceInstNo = 0;
    while(S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched)))
        //if(S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched)))
    {	

        CComPtr<IPropertyBag> ptrPropBag;

        hr = pMoniker->BindToStorage(
            0,//bind context
            0,//
            IID_IPropertyBag,
            (VOID**)&ptrPropBag);

        if (hr != S_OK)
        {
            pMoniker = NULL;
            continue;
        }

        VARIANT varDevPath;
        VariantInit(&varDevPath);
        hr = ptrPropBag->Read(L"DevicePath", &varDevPath, 0);

        if (hr != S_OK)
        {
            pMoniker = NULL;
            continue;
        }


        if(!this->m_strCaptureDevicePath.IsEmpty())
        {
            wchar_t szTmp[1024] = { 0 };
            wsprintf(szTmp, L" xxxx m_strCaptureDevicePath:%s\n", this->m_strCaptureDevicePath.GetBuffer());
            OutputDebugString(szTmp);

            if(m_strCaptureDevicePath != OLE2CT(varDevPath.bstrVal))
            {
                pMoniker = NULL;
                continue;
            }

        }

        // Bind Moniker to a filter object
        hr = pMoniker->BindToObject(0,0,IID_IBaseFilter, (void**)&pSrc);
        if (FAILED(hr))
        {
            return hr;
        }
#ifdef _LOG
            LOG_INF("Open Caputre Device %s", (const char*) CT2CA(m_strCaptureDevicePath));
#endif
        break;


    }//while

    // Copy the found filter pointer to the output parameter.
    // Do NOT Release() the reference, since it will still be used
    // by the calling function.
    *ppSrcFilter = pSrc;

    return hr;
}


HRESULT CVideoPlayer::PlayVideo(HWND hWnd,  HWND hNotifyWnd)
{
    LOG_FUNC_CALL;

    HRESULT hr = CloseInterfaces();
    if(hWnd != NULL)
    {
        m_hPlayWnd = hWnd;
    }

    if(hNotifyWnd != NULL)
    {
        m_hNotifyWnd = hNotifyWnd;
    }

    hr = GetInterfaces();

    if(FAILED(hr))
    {
        LOG_ERR("GetInterfaced Failed! Leave PlayVideo with Error 0x0%x", hr);
        return hr;
    }


    hr = BuildGraph();
    //zzq
    if(FAILED(hr))
    {
        LOG_ERR("BuildGraph Failed! Leave PlayVideo with Error 0x%x", hr);
        return hr;
    }


    //// Set the video window to be a child of the main window
    //hr = m_pVW->put_Owner((OAHWND)m_hPlayWnd);
    //if (FAILED(hr))
    //	return hr;

    //// Set video window style
    //hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
    //if (FAILED(hr))
    //	return hr;


    //RECT rc;
    //// Make the preview video fill our window
    //::GetClientRect(m_hPlayWnd,&rc);
    //m_pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);


    //// Make the video window visible, now that it is properly positioned
    //hr = m_pVW->put_Visible(OATRUE);
    //if (FAILED(hr))
    //	return hr;

    //hr = m_pVW->put_MessageDrain((OAHWND)m_hPlayWnd);


    //    hr = this->m_pWC->SetVideoClippingWindow(m_hPlayWnd);

    //RECT rcPlayArea;
    //GetClientRect(m_hPlayWnd, &rcPlayArea);

    //this->m_pWC->SetVideoPosition(NULL, &rcPlayArea);

#ifdef _LOG
    LOG_INF("(%s) Ready to call m_pMC->Run.", __FUNCTION__);
#endif

    hr = m_pMC->Run();//hr  0x8007048f ERROR_DEVICE_NOT_CONNECTED
    //hr	0x8007001f 连到系统上的设备没有发挥作用。 Device is not functioning properly
    //E_NOINTERFACE 不支持此接口 0x80004002
    //
#ifdef _LOG
    LOG_INF("(%s) m_pMC->Run finished.", __FUNCTION__);
#endif

    if(FAILED(hr))
    {
        LPVOID lpMsgBuf = NULL;

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            hr,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, 
            NULL );

        // Display the error message and exit the process
        //AtlTrace(_T("IMediaControl::Run return Error:%s\r\n"), lpMsgBuf);

        //通知停止状态
//      CString strStatusText;
//      strStatusText.Format(_T("%s,%s"), g_oResStr[IDS_STRING443],lpMsgBuf);
        PostMessage(m_hNotifyWnd, WM_CAMERA_STATUS_NOTIFY, (WPARAM)(LPCTSTR)g_oResStr[IDS_STRING443], (LPARAM)m_nID);

        LOG_ERR("(%s) m_pMC->Run() failed with error code: 0x%x, %s", __FUNCTION__, hr, (const char*)CT2CA((LPTSTR)lpMsgBuf));
        LocalFree(lpMsgBuf);

       
    }
    else if(hr == S_FALSE)
    {
        //If the return value is S_FALSE, you can wait for the transition to complete by calling the IMediaControl::GetState method. 
        //If the method fails, some filters might be in a running state
        FILTER_STATE fs;
        LONG msTimeOut = 1000;//Duration of the time-out, in milliseconds, or INFINITE to specify an infinite time-out.
        hr = m_pMC->GetState(msTimeOut, (OAFilterState*)&fs);

#ifdef _LOG
    LOG_INF("(%s) m_pMC->Run() return S_FALSE", __FUNCTION__);
#endif
    }




    if(m_pFilterGraph)
    {

#ifdef _DEBUG
        PrintFilterGraph(m_pFilterGraph);
#endif

    }


    return hr;
}



HRESULT CVideoPlayer::Stop()
{
    HRESULT hr = S_OK;
    CloseInterfaces();
    return hr;

}


//@功能:建立Filter Graph
//
// |capture source|=>|smart tee filter|=>|Intercept filter|=>|Video Mixing Render|
//
HRESULT CVideoPlayer::BuildGraph()
{
    LOG_FUNC_CALL;

    HRESULT hr = S_OK;

    hr = FindCaptureDevice(&m_pCaptureFilter);

    if(FAILED(hr))
    {
      //通知设备丢失
    //  CString strStatusText = g_oResStr[IDS_STRING445];
      PostMessage(m_hNotifyWnd, WM_CAMERA_STATUS_NOTIFY, (WPARAM)(LPCTSTR)g_oResStr[IDS_STRING445], (LPARAM)m_nID);
      return E_FAIL;
       LOG_ERR("FindCaptureDevice return 0x%x", hr);
       return hr;
    }

#ifdef _DEBUG
    PrintObjRef(m_pCaptureFilter);
#endif


    if(m_pCaptureFilter == NULL)
    {
        LOG_ERR("Capture Filter is NULL");

      //通知设备丢失
     // CString strStatusText = g_oResStr[IDS_STRING445];
	  PostMessage(m_hNotifyWnd, WM_CAMERA_STATUS_NOTIFY, (WPARAM)(LPCTSTR)g_oResStr[IDS_STRING445], (LPARAM)m_nID);
      return E_FAIL;
    }


    FILTER_INFO filterInfo;
    hr = m_pCaptureFilter->QueryFilterInfo(&filterInfo);
    if(FAILED(hr))
    {
        LOG_ERR("QueryFilterInfo failed with error 0x%x.", hr);
        return hr;
    }

#ifdef _DEBUG
    PrintObjRef(m_pCaptureFilter);
#endif

    //1.Add Filter Graph To Filter Graph
    if(!m_pCaptureFilter) return E_FAIL;
    if(!m_pFilterGraph  ) return E_FAIL;
    hr = m_pFilterGraph->AddFilter(m_pCaptureFilter, L"Capture Filter");
    if(FAILED(hr))
    {
        LOG_ERR("Add Capture Filter failed with error 0x%x.", hr);
        return hr;
    }

    //added by toxuke@gmail.com, 2013/03/19
    //Create Smart Teee filter
    CComPtr<IBaseFilter> ptrSmartTee;
    hr = CoCreateInstance(
        CLSID_SmartTee, NULL,
        CLSCTX_INPROC, IID_IBaseFilter, (void**)&ptrSmartTee); 
    if (FAILED(hr))
    {
        LOG_ERR("Create Smart Tee filter failed with error 0x%x", hr);
        return hr;
    }

    hr = m_pFilterGraph->AddFilter(ptrSmartTee, L"Smart Tee"); 
    if (FAILED(hr)) 
    {
        LOG_ERR("Add Smart Tee filer to filter graph failed with error 0x%x.", hr);
        return hr;
    }


#ifdef _DEBUG
    PrintObjRef(m_pCaptureFilter);
#endif

    //LOG("%s, Step3", __FUNCTION__);
    //ASSERT(ChangeCaptureFreq(60));

    if(m_tDesiredImageFormat.bmiHeader.biHeight != 0 && m_tDesiredImageFormat.bmiHeader.biWidth != 0)
    {
        SetImageFormat(m_tDesiredImageFormat);
    }


    //Create VMR
    CComPtr<IBaseFilter> ptrVMR;


    hr = CoCreateInstance(CLSID_NullRenderer, NULL,
        CLSCTX_INPROC, IID_IBaseFilter, (void**)&ptrVMR); 
    if (FAILED(hr))
    {
        LOG_ERR("Create Null  Renderer failed with error 0x%x.", hr);
        return hr;
    }

    hr = m_pFilterGraph->AddFilter(ptrVMR, L"NULL Renderer"); 
    if (FAILED(hr)) 
    {
       LOG_ERR("Add NULL Renderer to filter graph failed with error 0x%x.", hr);
        return hr;
    }

#ifdef _DEBUG
    PrintObjRef(m_pCaptureFilter);
#endif

    CComPtr<IPin> ptrCaptureOutputPin;
    hr = FindPin(m_pCaptureFilter, PINDIR_OUTPUT, NULL, &ptrCaptureOutputPin);

    if(FAILED(hr))
    {
        LOG_ERR("FindPin failed with error 0x%x.", hr);
        return hr;
    }

#ifdef _DEBUG
    PrintObjRef(m_pCaptureFilter);
#endif

    CComPtr<IAMBufferNegotiation> ptrBufferNegotiation;
    hr = ptrCaptureOutputPin->QueryInterface(IID_IAMBufferNegotiation , (VOID**)&ptrBufferNegotiation);

    ALLOCATOR_PROPERTIES  allocaProperty;
    allocaProperty.cBuffers  = 4;//Number of buffers created by the allocator.
    allocaProperty.cbBuffer  = m_tDesiredImageFormat.bmiHeader.biHeight * m_tDesiredImageFormat.bmiHeader.biWidth * 2;//Size of each buffer in bytes, excluding any prefix.
    allocaProperty.cbAlign   = 2;
    allocaProperty.cbPrefix  = 0;



    if(hr == S_OK)
    {

        hr = ptrBufferNegotiation->SuggestAllocatorProperties(&allocaProperty);

        if(FAILED(hr))
        {
            LOG_ERR("IAMBufferNegotiation::SuggestAllocatorProperties failed with erro 0x%x.\r\n", hr);
        }
        else
        {
            LOG_INF("IAMBufferNegotiation::SuggestAllocatorProperties succeeded with cBuffers=%d, cBuffer=%d, cbAlign=%d, cbPrefix=%d\r\n",
                allocaProperty.cBuffers,
                allocaProperty.cbBuffer,
                allocaProperty.cbAlign,
                allocaProperty.cbPrefix);
        }

    }   
    else
    {
        LOG_ERR("CaptureOutputPin QueryInterface IAMBufferNegotiation failed with error 0x%x. \r\n", hr);

    }

#ifdef _DEBUG
    PrintObjRef(m_pCaptureFilter);
#endif


    CComPtr<IEnumMediaTypes> ptrEnumMediaTypes;
    hr = ptrCaptureOutputPin->EnumMediaTypes(&ptrEnumMediaTypes);

    AM_MEDIA_TYPE* pmt;
    ULONG cFetched;
    hr = ptrEnumMediaTypes->Next(1,&pmt,&cFetched);

#ifdef _DEBUG
    PrintObjRef(m_pCaptureFilter);
#endif


    //2.Add MotionDetectFilter to Filter Graph
    if(!m_pInterceptFilter)
    {
        return E_FAIL;
    }


    hr = m_pFilterGraph->AddFilter(m_pInterceptFilter, L"Motion Detect Filter");
    if(FAILED(hr))
    {
        LOG_ERR("Add Montion Detect Filter failed with error 0x%x", hr);
        return hr;
    }

#ifdef _DEBUG
    PrintObjRef(m_pCaptureFilter);
#endif


    //连接Source Capture Output Pin ==>Smart Tee Input Pin
    CComPtr<IPin> ptrSmartInputPin;
    hr = FindPin(ptrSmartTee, PINDIR_INPUT, NULL, &ptrSmartInputPin);
    ;
    hr = m_pFilterGraph->ConnectDirect(ptrCaptureOutputPin/*out*/,  ptrSmartInputPin/*in*/, pmt);
    if(FAILED(hr))
    {
        //hr =0x8004025f, VFW_E_NOT_IN_GRAPH
        LOG_ERR("Connect capture filter with Smart Tee input pin failed with error 0x%x", hr);
        return hr;

    }

    //连接>Smart Tee Privew Pin ==> Motion Detect Filter Input Pin
    CComPtr<IPin> ptrSmartOutputPin;
    hr = FindPin(ptrSmartTee, PINDIR_OUTPUT, L"Preview", &ptrSmartOutputPin);
    if(FAILED(hr))
    {
        LOG_ERR("Retrieve smart tee output pin failed with error 0x%x", hr);
        return hr;
    }



    CComPtr<IPin> ptrMotionDetectFilterInputPin;
    ptrMotionDetectFilterInputPin = m_pInterceptFilter->GetPin(0);

    hr = m_pFilterGraph->ConnectDirect(ptrSmartOutputPin/*out*/,  ptrMotionDetectFilterInputPin/*in*/, pmt);

    if(FAILED(hr))
    {
		LOG_ERR("Connect ptrSmartOutputPin to ptrMotionDetectFilterInputPin failed with error 0x%x", hr);
        return hr;
    }

#ifdef _DEBUG
    PrintObjRef(m_pCaptureFilter);
#endif


    if(ptrBufferNegotiation)
    {
        hr = ptrBufferNegotiation->GetAllocatorProperties(&allocaProperty);

        if(hr == S_OK)
        {
            LOG_INF("IAMBufferNegotiation::GetAllocatorProperties cBuffers=%d cbBuffer=%d, cbAlign=%d, cbPrefix=%d\r\n", allocaProperty.cBuffers, allocaProperty.cbBuffer, allocaProperty.cbAlign, allocaProperty.cbPrefix);
        }
    }
    else
    {
        LOG_ERR("%s, ptrBufferNegotiation is NULL", __FUNCTION__);
    }

    CComPtr<IPin> ptrMotionDetectFilterOutputPin;
    ptrMotionDetectFilterOutputPin = m_pInterceptFilter->GetPin(1);
    if(!ptrMotionDetectFilterOutputPin)
    {
		LOG_ERR("ptrMotionDetectFilterOutputPin == NULL");
        return E_FAIL;
    }


#ifdef _DEBUG
    PrintObjRef(m_pCaptureFilter);
#endif

    hr = m_pFilterGraph->Render(ptrMotionDetectFilterOutputPin);
    if(FAILED(hr))
    {
        LOG_ERR("m_pFilterGraph->Render failed with error: 0x%x, Leave %s", hr,__FUNCTION__);
        return hr;
    }

#ifdef _DEBUG
    PrintObjRef(m_pCaptureFilter);
#endif


#ifdef _DEBUG
    PrintFilterGraph(m_pFilterGraph);
#endif

    return hr;

}

////@功能:开始侦测
////@参数:hPlayWnd, 播放窗体句柄
////      lpszDeviceName, 设备名称
////      nDeviceInstNo, 设备实列编号(从1开始编号)
////      pVideoInfoHeader, 指向指定的视频格式的指针
////@返回值:
////   成功,返回TRUE
////   失败,返回FALSE
//BOOL CVideoPlayer::StartDetect( HWND hPlayWnd, LPCTSTR lpszDeviceName, int nDeviceInstNo, VIDEOINFOHEADER* pVideoInfoHeader)
//{
//	LOG("enter %s", __FUNCTION__);
//
//	m_strCaptureDeviceName   = lpszDeviceName;
//	m_nCaptureDeviceInstNo = nDeviceInstNo;
//
//	if(pVideoInfoHeader)
//	{
//		m_tDesiredImageFormat = *pVideoInfoHeader;
//	}
//
//	HRESULT hr  = PlayVideo(hPlayWnd);
//	if(FAILED(hr))
//	{
//		return FALSE;
//	}
//
//	BOOL bRet = m_pInterceptFilter->StartDetect(m_tDesiredImageFormat.bmiHeader.biWidth, m_tDesiredImageFormat.bmiHeader.biHeight);
//
//	m_bIsDetecting = bRet;
//
//	//m_pInterceptFilter->ViewMonoImage (hDebugWnd  );
//	//m_pInterceptFilter->ViewBkgndImage(hDebugWnd2 );
//	//m_pInterceptFilter->ViewDiffImage (hDebugWnd3 );
//
//	LOG("Leave %s", __FUNCTION__);
//	return bRet;
//
//}

//@功能:开始侦测
//@参数:lpszDevicePath, 设备路径
//      pVideoInfoHeader, 指向指定的视频格式的指针
//@返回值:
//   成功,返回TRUE
//   失败,返回FALSE
BOOL CVideoPlayer::StartDetect(LPCTSTR lpszDevicePath,  const VIDEOINFOHEADER* pVideoInfoHeader)
{

	return StartDetect(m_hPlayWnd, m_rcDispArea, m_hNotifyWnd, lpszDevicePath, pVideoInfoHeader);
}


//@功能:开始侦测
//@参数:hPlayWnd,       播放窗体句柄
//      rcDispArea,     视频显示区域
//      hNotifyWnd,     通知消息接收窗体的句柄
//      lpszDevicePath, 设备路径
//      pVideoInfoHeader, 指向指定的视频格式的指针
//@返回值:
//   成功,返回TRUE
//   失败,返回FALSE
BOOL CVideoPlayer::StartDetect( HWND hPlayWnd, const RECT& rcDispArea, HWND hNotifyWnd, LPCTSTR lpszDevicePath,  const VIDEOINFOHEADER* pVideoInfoHeader)
{
    LOG_FUNC_CALL;

	if (m_bIsClosing)
	{
		return FALSE;
	}

    m_strCaptureDevicePath   = lpszDevicePath;
    m_rcDispArea  = rcDispArea;

    if(pVideoInfoHeader)
    {
        m_tDesiredImageFormat = *pVideoInfoHeader;
    }

    HRESULT hr  = PlayVideo(hPlayWnd, hNotifyWnd);
    if(hr != S_OK)
    {
        return FALSE;
    }


    /*
    if(m_oRealFrameDib.GetWidth()  != m_tDesiredImageFormat.bmiHeader.biWidth|| 
       m_oRealFrameDib.GetHeight() != m_tDesiredImageFormat.bmiHeader.biHeight)
	{
		m_oRealFrameDib.Create(m_tDesiredImageFormat.bmiHeader.biWidth,  m_tDesiredImageFormat.bmiHeader.biHeight);
        m_oRGB.SetSize(m_tDesiredImageFormat.bmiHeader.biWidth,  m_tDesiredImageFormat.bmiHeader.biHeight,3);
	}
    */
    if(
        m_oRealFrameARGB.GetWidth()  !=  m_tDesiredImageFormat.bmiHeader.biWidth|| 
        m_oRealFrameARGB.GetHeight() != m_tDesiredImageFormat.bmiHeader.biHeight
        )
    {
        m_oRealFrameARGB.SetSize( m_tDesiredImageFormat.bmiHeader.biWidth,  m_tDesiredImageFormat.bmiHeader.biHeight);

    }

    BOOL bRet = m_pInterceptFilter->StartDetect(hNotifyWnd, m_tDesiredImageFormat.bmiHeader.biWidth, m_tDesiredImageFormat.bmiHeader.biHeight);

    m_bIsDetecting = bRet;

    m_eVideoState = E_VIDEO_RUNNING;

	PostMessage(this->m_hNotifyWnd, WM_FPSNOTIFY, (WPARAM)0, (LPARAM)m_nID);
	PostMessage(m_hNotifyWnd, WM_CAMERA_STATUS_NOTIFY, (WPARAM)(LPCTSTR)g_oResStr[IDS_STRING442], (LPARAM)m_nID);
    
    return bRet;

}


BOOL CVideoPlayer::StopDetect( )
{
    LOG_FUNC_CALL;

    BOOL bRet = FALSE;
   if(m_eVideoState == E_VIDEO_RUNNING)
   {

    if(!m_pInterceptFilter) return E_FAIL;

    bRet =  m_pInterceptFilter->StopDetect();

    m_bIsDetecting = FALSE;

    CloseInterfaces();

      m_eVideoState = E_VIDEO_STOP;

      //通知停止状态
     // CString strStatusText = g_oResStr[IDS_STRING444];
      PostMessage(m_hNotifyWnd, WM_CAMERA_STATUS_NOTIFY, (WPARAM)(LPCTSTR)g_oResStr[IDS_STRING444], (LPARAM)m_nID);
   }

   //作废显示下你
   //m_bTextIsValidate = FALSE;

    return bRet;

}

void CVideoPlayer::MissStatusInfo()
{
	//CString strStatusText = g_oResStr[IDS_STRING445];
	PostMessage(m_hNotifyWnd, WM_CAMERA_STATUS_NOTIFY, (WPARAM)(LPCTSTR)g_oResStr[IDS_STRING445], (LPARAM)m_nID);
}



//设置接收通知消息的窗体
void CVideoPlayer::SetNotifyMessage(HWND hWnd, UINT uMsg)
{
    if(!m_pInterceptFilter) return;

    m_pInterceptFilter->SetNotifyWindow(hWnd, uMsg);

}


BOOL CVideoPlayer::IsDetecting()const
{
    return m_bIsDetecting;
}

BOOL CVideoPlayer::ResizeVideo(const RECT& rcNewSize)
{
    BOOL bRet = FALSE;
    //if(m_pVW)
    //{
    //	int nWidth  = rcNewSize.right  - rcNewSize.left;
    //	int nHeight = rcNewSize.bottom - rcNewSize.top; 
    //	HRESULT hr = m_pVW->SetWindowPosition(rcNewSize.left, rcNewSize.top, nWidth, nHeight);

    //	bRet = hr == S_OK?TRUE:FALSE;
    //}

    if(m_pWC)
    {
        HRESULT hr = this->m_pWC->SetVideoClippingWindow(m_hPlayWnd);

        RECT rcPlayArea;
        GetClientRect(m_hPlayWnd, &rcPlayArea);

        this->m_pWC->SetVideoPosition(NULL, &rcPlayArea);

        bRet = hr == S_OK?TRUE:FALSE;
    }

    return bRet;
}

//@功能:设置亮度检测门限
//BOOL CVideoPlayer::SetYThreshold(BYTE threshold)
//{
//    if(m_pInterceptFilter == NULL) return FALSE;
//
//    m_pInterceptFilter->SetYThreshold(threshold);
//
//    return TRUE;
//
//}

//@返回:亮度检测门限
//BYTE CVideoPlayer::GetThreshold()const
//{
//    BYTE threshold = 0;
//
//    if(m_pInterceptFilter)
//    {
//        threshold = m_pInterceptFilter->GetYThreshold();
//    }
//
//    return threshold;
//}


//@功能:重置视频剪切区域
//BOOL CVideoPlayer::ResetVideoClipArea()
//{
//    if(m_pInterceptFilter == NULL) return FALSE;
//
//    m_pInterceptFilter->ResetVideoClipArea();
//    return TRUE;
//}
//
////@功能:设置视频剪切区域
//BOOL CVideoPlayer::SetVideoClipArea(const RECT& rcClipArea)
//{
//    if(m_pInterceptFilter == NULL) return FALSE;
//
//    m_pInterceptFilter->SetVideoClipArea(rcClipArea);
//    return TRUE;
//}

//@功能:更改捕获频率
//@说明:该函数调用后, 实际的采样频率并未发生显著变化。
BOOL CVideoPlayer::ChangeCaptureFreq(int  nFrequency)
{
    HRESULT hr = S_OK;
    BOOL    bRet = FALSE;
    //if(!m_pCaptureGraphBuilder) return FALSE;
    if(m_pCaptureFilter == NULL) return FALSE;


    CComPtr<IPin> ptrCaptureOutputPin;
    //CComPtr<IPin> ptrCaptureInputPin;
    hr  = FindPin(m_pCaptureFilter, PINDIR_OUTPUT, NULL, &ptrCaptureOutputPin);
    // hr  = FindPin(m_pCaptureFilter, PINDIR_INPUT, NULL, &ptrCaptureInputPin);

    if(FAILED(hr))
    {
        return FALSE;
    }

    CComPtr<IAMStreamConfig>  ptrConfig=NULL; 

    //hr = m_pCaptureGraphBuilder-> FindInterface(
    //	&PIN_CATEGORY_CAPTURE,
    //	0,
    //	m_pCaptureFilter,
    //	IID_IAMStreamConfig,
    //	(void**)&ptrConfig); 

    hr = ptrCaptureOutputPin->QueryInterface(IID_IAMStreamConfig, (void**)&ptrConfig);
    //hr = ptrCaptureInputPin->QueryInterface(IID_IAMStreamConfig, (void**)&ptrConfig);

    if(hr == S_OK)
    {
        AM_MEDIA_TYPE*   pmt   =   NULL; 

        hr = ptrConfig->GetFormat(&pmt);

        if(hr == S_OK)
        {
            if(pmt-> formattype   ==   FORMAT_VideoInfo)
            {
                VIDEOINFOHEADER   *pvi   = (VIDEOINFOHEADER   *)pmt-> pbFormat;
                pvi-> AvgTimePerFrame = (LONGLONG)(10000000/nFrequency)   ;//100-nanosecond units
                //VFW_E_WRONG_STATE ((HRESULT)0x80040227L)
                hr = ptrConfig-> SetFormat(pmt); 
                if(hr == S_OK)
                {
                    bRet = TRUE;
                }
                else
                {
                    switch(hr)
                    {
                    case VFW_E_WRONG_STATE:
                        AtlTrace(_T("%S Ln %d VFW_E_WRONG_STATE\r\n"), __FUNCTION__, __LINE__);
                        break;
                    }
                }
            }
            DeleteMediaType(pmt);
        }

    }

    return bRet;
}

BOOL CVideoPlayer::SetImageFormat(const VIDEOINFOHEADER& vih)
{
    HRESULT hr = S_OK;
    BOOL    bRet = FALSE;
    //if(!m_pCaptureGraphBuilder) return FALSE;
    if(m_pCaptureFilter == NULL) return FALSE;

    CComPtr<IAMStreamConfig>  ptrConfig=NULL; 

#ifdef _DEBUG
    PrintObjRef(m_pCaptureFilter);
#endif

    //hr = m_pCaptureFilter->FindPin((IID_IAMStreamConfig, (void**)&ptrConfig);
    CComPtr<IPin> ptrCaptureOutputPin;
    hr  = FindPin(m_pCaptureFilter, PINDIR_OUTPUT, NULL, &ptrCaptureOutputPin);

    if(FAILED(hr))
    {
        return FALSE;
    }

#ifdef _DEBUG
    PrintObjRef(m_pCaptureFilter);
#endif

    hr = ptrCaptureOutputPin->QueryInterface(IID_IAMStreamConfig, (void**)&ptrConfig);

#ifdef _DEBUG
    PrintObjRef(m_pCaptureFilter);
#endif

    /*hr = m_pCaptureGraphBuilder-> FindInterface(
    &PIN_CATEGORY_CAPTURE,
    0,
    m_pCaptureFilter,
    IID_IAMStreamConfig,
    (void**)&ptrConfig); */

    if(hr == S_OK)
    {
        AM_MEDIA_TYPE*   pmt   =   NULL; 

        hr = ptrConfig->GetFormat(&pmt);


        //
        int iCount, iSize;
        hr = ptrConfig->GetNumberOfCapabilities(&iCount, &iSize);

        if(S_OK == hr)
        {
            VIDEO_STREAM_CONFIG_CAPS scc;
            AM_MEDIA_TYPE *pmt;
            for(int iIndex = 0;iIndex < iCount; iIndex++)
            {
                hr = ptrConfig->GetStreamCaps(iIndex, &pmt, (BYTE*)&scc);
                if(S_OK == hr)
                {
                    AtlTrace(_T("[caps %d] iIndexMinFrameInterval=%lli, MaxFrameInterval=%lli\n"), iIndex, scc.MinFrameInterval, scc.MaxFrameInterval);
                }
            }
        }

#ifdef _DEBUG
        PrintObjRef(m_pCaptureFilter);
#endif
        if(hr == S_OK)
        {
            if(pmt-> formattype   ==   FORMAT_VideoInfo)
            {
                VIDEOINFOHEADER   *pvi   = (VIDEOINFOHEADER *)pmt-> pbFormat;
                if(pmt->cbFormat == sizeof(VIDEOINFOHEADER))
                {
                    *pvi = vih;
                    //int nFrequency = 60;
                  //  int nFrequency = 30;     ////50
                  //  pvi->AvgTimePerFrame = (LONGLONG)(10000000/nFrequency);
                   // pvi->dwBitRate       = 1280*720/10;
                   // pmt->lSampleSize     = 1280*720/4;
                   // pmt->subtype = MEDIASUBTYPE_MJPG;   
                   // pmt->subtype = MEDIASUBTYPE_YUY2;   
                    hr = ptrConfig->SetFormat(pmt);

                    if(hr == S_OK)
                    {
                         LOG_INF("Set Image Format MJPG 640*480 succeeded.\r\n");
                    }
                    else
                    {
                        LOG_ERR("IAMStreamConfig Set Image Format Failed With Error 0x%x \r\n", hr);
                    }
                }

#ifdef _DEBUG
                PrintObjRef(m_pCaptureFilter);
#endif

            }

            DeleteMediaType(pmt);

        }

    }
#ifdef _DEBUG
    PrintObjRef(m_pCaptureFilter);
#endif

    return bRet;
}


BOOL CVideoPlayer::GetVideoSize(SIZE&  size)
{
    if (m_pCaptureFilter == NULL) return FALSE;

    HRESULT hr = S_OK;
    BOOL    bRet = FALSE;

    CComPtr<IAMStreamConfig>  ptrConfig = NULL;
    CComPtr<IPin> ptrCaptureOutputPin;
    hr = FindPin(m_pCaptureFilter, PINDIR_OUTPUT, NULL, &ptrCaptureOutputPin);

    if (FAILED(hr))
    {
        return FALSE;
    }

    hr = ptrCaptureOutputPin->QueryInterface(IID_IAMStreamConfig, (void**)&ptrConfig);
    if (FAILED(hr))
    {
        return FALSE;
    }

     AM_MEDIA_TYPE*   pmt = NULL;
    
     hr = ptrConfig->GetFormat(&pmt);

     if (FAILED(hr)) return FALSE;

     if (pmt->formattype == FORMAT_VideoInfo)
     {

         VIDEOINFOHEADER   *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;

         if (pmt->cbFormat == sizeof(VIDEOINFOHEADER))
         {
             size.cx = pvi->bmiHeader.biWidth;
             size.cy = pvi->bmiHeader.biHeight;
         }
         else
         {
             return FALSE;
         }
     }
     else if (pmt->formattype == FORMAT_VideoInfo2)
     {
         VIDEOINFOHEADER2   *pvi = (VIDEOINFOHEADER2 *)pmt->pbFormat;

         if (pmt->cbFormat == sizeof(VIDEOINFOHEADER2))
         {
             size.cx = pvi->bmiHeader.biWidth;
             size.cy = pvi->bmiHeader.biHeight;
         }
         else
         {
             return FALSE;
         }
     }
     else
     {
         return FALSE;
     }

    return TRUE;
}


HRESULT CVideoPlayer::ShowPropertyPage(HWND hwndParent)
{	
    if(!m_pCaptureFilter) return E_POINTER;
    HRESULT hr = S_OK;

    CComPtr<ISpecifyPropertyPages> ptrSpecify = NULL;

    hr = m_pCaptureFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&ptrSpecify);

    if (SUCCEEDED(hr))
    {
        do
        {
            FILTER_INFO FilterInfo;
            hr = m_pCaptureFilter->QueryFilterInfo(&FilterInfo);
            if (FAILED(hr))
                break;

            CAUUID caGUID;

            hr = ptrSpecify->GetPages(&caGUID);

            if (FAILED(hr))
                break;
            CComPtr<IUnknown> ptrFilterUnk ;

            m_pCaptureFilter->QueryInterface(IID_IUnknown, (void **)&ptrFilterUnk);

            // Display the filter's property page
            OleCreatePropertyFrame(
                hwndParent, // Parent window
                0, // x (Reserved)
                0, // y (Reserved)
                FilterInfo.achName, // Caption for the dialog box
                1, // Number of filters
                &(ptrFilterUnk.p), // Pointer to the filter
                caGUID.cElems, // Number of property pages
                caGUID.pElems, // Pointer to property page CLSIDs
                0, // Locale identifier
                0, // Reserved
                NULL // Reserved
                );
            //clean up

            FilterInfo.pGraph->Release();
            CoTaskMemFree(caGUID.pElems);
        } while(0);
    }//if

    return hr;
}


//CPenPosDetector* CVideoPlayer::GetPenPosDetector()
//{
//    CPenPosDetector* pPosDetector = NULL;
//
//    if(m_pInterceptFilter)
//    {
//        pPosDetector = &m_pInterceptFilter->GetPenPosDetector(); 
//    }
//    return pPosDetector;
//}
//
//const CPenPosDetector* CVideoPlayer::GetPenPosDetector() const
//{
//    const CPenPosDetector* pPosDetector = NULL;
//
//    if(m_pInterceptFilter)
//    {
//        pPosDetector = &m_pInterceptFilter->GetPenPosDetector(); 
//    }
//    return pPosDetector;
//}

IBaseFilter* CVideoPlayer::GetCaptureFilter()
{
    return m_pCaptureFilter;
}

//@功能:设置Capture Filter的Gamma值
//@参数:lGammaValue, Specifies the gamma, as gamma * 100. Values range from 1 to 500. 
//BOOL CVideoPlayer::ChangeGammaValue(LONG lGammaValue)
//{
//	if(m_pCaptureFilter == NULL) return FALSE;
//	CComPtr<IAMVideoProcAmp> ptrAMVideoProcAmp;
//
//	HRESULT hr = m_pCaptureFilter->QueryInterface( IID_IAMVideoProcAmp, (void**)&ptrAMVideoProcAmp);
//	if(FAILED(hr))
//	{
//		return FALSE;
//	}
//
//	long lMin, lMax, lSteppingDelta, lDefault, lCapsFlags;
//	hr = 
//	ptrAMVideoProcAmp->GetRange(
//		VideoProcAmp_Gamma, 
//		&lMin,
//		&lMax,
//		&lSteppingDelta,
//		&lDefault,
//		&lCapsFlags);
//
//	if(FAILED(hr))
//	{
//		return FALSE;
//	}
//
//	if(lGammaValue < lMin)
//	{
//		lGammaValue = lMin;
//	}
//	else if(lGammaValue > lMax)
//	{
//		lGammaValue = lMax;
//	}
//
//	ptrAMVideoProcAmp->Set(VideoProcAmp_Gamma, lGammaValue, lCapsFlags);
//	return TRUE;
//}



//Added by toxuke@gmail.com, 2013/05/10
//@功能:更改摄像头参数
BOOL CVideoPlayer::SetCameraParams(const TVideoProcAmpProperty& newParams)
{
#ifdef _DEBUG
     CPerfDetector perf(_T("CVideoPlayer::SetCameraParams"));
#endif

    if(m_pCaptureFilter == NULL) return FALSE;
    CComPtr<IAMVideoProcAmp> ptrAMVideoProcAmp;

    HRESULT hr = m_pCaptureFilter->QueryInterface( IID_IAMVideoProcAmp, (void**)&ptrAMVideoProcAmp);
    if(FAILED(hr))
    {
        return FALSE;
    }

    long lMin, lMax, lSteppingDelta, lDefault, lCapsFlags;

    //1.亮度
    do
    {
        hr = 
            ptrAMVideoProcAmp->GetRange(
            VideoProcAmp_Brightness, 
            &lMin,
            &lMax,
            &lSteppingDelta,
            &lDefault,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }

        long lBrightness = newParams.Prop_VideoProcAmp_Brightness;

		if(lBrightness < lMin)
        {
            lBrightness = lMin;
        }
        else if(lBrightness > lMax)
        {
            lBrightness = lMax;
        }

        hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Brightness, lBrightness, lCapsFlags);
    }while(0);


    //2.对比度
    do
    {
        hr = 
            ptrAMVideoProcAmp->GetRange(
            VideoProcAmp_Contrast, 
            &lMin,
            &lMax,
            &lSteppingDelta,
            &lDefault,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }

        long lContrast = newParams.Prop_VideoProcAmp_Contrast;

        if(lContrast < lMin)
        {
            lContrast = lMin;
        }
        else if(lContrast > lMax)
        {
            lContrast = lMax;
        }
        hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Contrast, lContrast, lCapsFlags);

    }while(0);

    //3.色调
    do
    {
        hr = 
            ptrAMVideoProcAmp->GetRange(
            VideoProcAmp_Hue, 
            &lMin,
            &lMax,
            &lSteppingDelta,
            &lDefault,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }

        long lHue= newParams.Prop_VideoProcAmp_Hue;
        if(lHue < lMin)
        {
            lHue = lMin;
        }
        else if(lHue > lMax)
        {
            lHue = lMax;
        }
        hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Hue, lHue, lCapsFlags);

    }while(0);



    //4.饱和度
    do
    {
        hr = 
            ptrAMVideoProcAmp->GetRange(
            VideoProcAmp_Saturation, 
            &lMin,
            &lMax,
            &lSteppingDelta,
            &lDefault,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }

        long lSaturation = newParams.Prop_VideoProcAmp_Satuation;
        if(lSaturation < lMin)
        {
            lSaturation = lMin;
        }
        else if(lSaturation > lMax)
        {
            lSaturation = lMax;
        }
        hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Saturation, lSaturation, lCapsFlags);

    }while(0);


    //5.锐利度
    do
    {
        hr = 
            ptrAMVideoProcAmp->GetRange(
            VideoProcAmp_Sharpness, 
            &lMin,
            &lMax,
            &lSteppingDelta,
            &lDefault,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }

        long lSharpness = newParams.Prop_VideoProcAmp_Sharpness;
        if(lSharpness < lMin)
        {
            lSharpness = lMin;
        }
        else if(lSharpness > lMax)
        {
            lSharpness = lMax;
        }
        hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Sharpness, lSharpness, lCapsFlags);

    }while(0);


    //6.伽马值
    do
    {
        hr = 
            ptrAMVideoProcAmp->GetRange(
            VideoProcAmp_Gamma, 
            &lMin,
            &lMax,
            &lSteppingDelta,
            &lDefault,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }

        long lGamma = newParams.Prop_VideoProcAmp_Gamma;
        if(lGamma < lMin)
        {
            lGamma = lMin;
        }
        else if(lGamma > lMax)
        {
            lGamma = lMax;
        }

        hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Gamma, lGamma, lCapsFlags);
    }while(0);


    //7.颜色启用
    do
    {
        hr = 
            ptrAMVideoProcAmp->GetRange(
            VideoProcAmp_ColorEnable, 
            &lMin,
            &lMax,
            &lSteppingDelta,
            &lDefault,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }

        long  lColorEnable = newParams.Prop_VideoProcAmp_ColorEnable;
        if(lColorEnable < lMin)
        {
            lColorEnable = lMin;
        }
        else if(lColorEnable > lMax)
        {
            lColorEnable = lMax;
        }
        hr = ptrAMVideoProcAmp->Set(VideoProcAmp_ColorEnable, lColorEnable, lCapsFlags);
    }while(0);


    //8.白平衡
    do
    {
        hr = 
            ptrAMVideoProcAmp->GetRange(
            VideoProcAmp_WhiteBalance, 
            &lMin,
            &lMax,
            &lSteppingDelta,
            &lDefault,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break; 
        }

        long lWhiteBalance = newParams.Prop_VideoProcAmp_WhiteBalance;
        if(lWhiteBalance < lMin)
        {
            lWhiteBalance = lMin;
        }
        else if(lWhiteBalance > lMax)
        {
            lWhiteBalance = lMax;
        }
        hr = ptrAMVideoProcAmp->Set(VideoProcAmp_WhiteBalance, lWhiteBalance, lCapsFlags);

    }while(0);


    //9.背光补偿
    do
    {
        hr = 
            ptrAMVideoProcAmp->GetRange(
            VideoProcAmp_BacklightCompensation, 
            &lMin,
            &lMax,
            &lSteppingDelta,
            &lDefault,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }

        long lBacklightCompensation = newParams.Prop_VideoProcMap_BacklightCompensation;
        if(lBacklightCompensation < lMin)
        {
            lBacklightCompensation = lMin;
        }
        else if(lBacklightCompensation > lMax)
        {
            lBacklightCompensation  = lMax;
        }
        hr = ptrAMVideoProcAmp->Set(VideoProcAmp_BacklightCompensation, lBacklightCompensation, lCapsFlags);
    }while(0);

    //10.增益
    do
    {
        hr = 
            ptrAMVideoProcAmp->GetRange(
            VideoProcAmp_Gain, 
            &lMin,
            &lMax,
            &lSteppingDelta,
            &lDefault,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }

		long  lGain =  newParams.Prop_VideoProcMap_Gain;
        if(lGain < lMin)
        {
            lGain = lMin;
        }
        else if(lGain > lMax)
        {
            lGain = lMax;
        }
        hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Gain, lGain, lCapsFlags);

    }while(0);


	/////////////////////////////////////曝光时间的设置
	do
	{
	    CComPtr<IAMCameraControl> ptrAMCameraControl;	
	    hr = m_pCaptureFilter->QueryInterface(IID_IAMCameraControl, (void**)&ptrAMCameraControl);
	    if (FAILED(hr))
	    {
			return TRUE;
//			break;
	    }

	    hr =
		   ptrAMCameraControl->GetRange(
			   CameraControl_Exposure,
			   &lMin,
			   &lMax,
			   &lSteppingDelta,
			   &lDefault,
			   &lCapsFlags);
		if (FAILED(hr))
		{
			return TRUE;
//			break;
		}

	    long lExposure = newParams.Prop_CameraControl_Exposure;
	    if (lExposure <lMin)
	    {
		    lExposure = lMin;
	    }
	    else if (lExposure>lMax)
	    {
		    lExposure = lMax;
	    }

	    hr = ptrAMCameraControl->Set(CameraControl_Exposure, lExposure, CameraControl_Flags_Manual);

	} while (0);

    return (hr==S_OK)?TRUE:FALSE;

}

//@功能:设置摄像的单个参数
 BOOL CVideoPlayer::SetCameraParams(long lValue, VideoProcAmpProperty eProperty)
{
    if(m_pCaptureFilter == NULL) return FALSE;
    CComPtr<IAMVideoProcAmp> ptrAMVideoProcAmp;
    HRESULT hr = m_pCaptureFilter->QueryInterface( IID_IAMVideoProcAmp, (void**)&ptrAMVideoProcAmp);
    if(FAILED(hr))
    {
        return FALSE;
    }

    long lMin, lMax, lSteppingDelta, lDefault, lCapsFlags;

    hr = 
        ptrAMVideoProcAmp->GetRange(
        eProperty, 
        &lMin,
        &lMax,
        &lSteppingDelta,
        &lDefault,
        &lCapsFlags);

    if(FAILED(hr))
    {
        return FALSE;
    }


    if(lValue < lMin)
    {
        lValue = lMin;
    }
    else if(lValue > lMax)
    {
        lValue = lMax;
    }
    hr = ptrAMVideoProcAmp->Set(eProperty, lValue, lCapsFlags);

    if(FAILED(hr))
    {
        return FALSE;
    }

    return TRUE;
}

//@功能:获取摄像头参数
BOOL CVideoPlayer::GetCameraParams(TVideoProcAmpProperty& saveParams)
 {

    if(m_pCaptureFilter == NULL) return FALSE;
    CComPtr<IAMVideoProcAmp> ptrAMVideoProcAmp;

    HRESULT hr = m_pCaptureFilter->QueryInterface( IID_IAMVideoProcAmp, (void**)&ptrAMVideoProcAmp);
    if(FAILED(hr))
    {
        return FALSE;
    }

    long lCapsFlags;


    //1.亮度
    do
    {
        hr = 
            ptrAMVideoProcAmp->Get(
            VideoProcAmp_Brightness,
            &saveParams.Prop_VideoProcAmp_Brightness,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }
     
    }while(0);


    //2.对比度
    do
    {
        hr = 
            ptrAMVideoProcAmp->Get(
			VideoProcAmp_Contrast,
            &saveParams.Prop_VideoProcAmp_Contrast,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }

    }while(0);




    //3.色调
    do
    {
        hr = 
            ptrAMVideoProcAmp->Get(
            VideoProcAmp_Hue,
            &saveParams.Prop_VideoProcAmp_Hue,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }

    }while(0);



    //4.饱和度
    do
    {
        hr = 
            ptrAMVideoProcAmp->Get(
            VideoProcAmp_Saturation,
            &saveParams.Prop_VideoProcAmp_Satuation,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }

    }while(0);


    //5.锐利度
    do
    {
        hr = 
            ptrAMVideoProcAmp->Get(
            VideoProcAmp_Sharpness,
            &saveParams.Prop_VideoProcAmp_Sharpness,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }

    }while(0);


    //6.伽马值
    do
    {
        hr = 
            ptrAMVideoProcAmp->Get(
            VideoProcAmp_Gamma,
            &saveParams.Prop_VideoProcAmp_Gamma,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }
    }while(0);


    //7.颜色启用
    do
    {
        hr = 
            ptrAMVideoProcAmp->Get(
            VideoProcAmp_ColorEnable,
            &saveParams.Prop_VideoProcAmp_ColorEnable,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }
    }while(0);


    //8.白平衡
    do
    {
        hr = 
            ptrAMVideoProcAmp->Get(
            VideoProcAmp_WhiteBalance,
            &saveParams.Prop_VideoProcAmp_WhiteBalance,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }

    }while(0);


    //9.背光补偿
    do
    {
        hr = 
            ptrAMVideoProcAmp->Get(
            VideoProcAmp_BacklightCompensation,
            &saveParams.Prop_VideoProcMap_BacklightCompensation,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }
    }while(0);


    //10.增益
    do
    {
        hr = 
            ptrAMVideoProcAmp->Get(
            VideoProcAmp_Gain,
            &saveParams.Prop_VideoProcMap_Gain,
            &lCapsFlags);

        if(FAILED(hr))
        {
            //return FALSE;
            break;
        }

    }while(0);

	////////////////曝光时间
	///////////////曝光在7725的摄像头中都是失败的因此不能返回FALSE,要不后面的就不执行了。
	CComPtr<IAMCameraControl> ptrAMCameraControl;
	hr = m_pCaptureFilter->QueryInterface(IID_IAMCameraControl, (void**)&ptrAMCameraControl);
	if (FAILED(hr))
	{
		return TRUE;
	}

	hr =
		ptrAMCameraControl->Get(
			CameraControl_Exposure,
			&saveParams.Prop_CameraControl_Exposure,
			&lCapsFlags);

	if (FAILED(hr))
	{
		return TRUE;
	}

    return TRUE;
 }

//@功能:获取摄像头亮度
BOOL CVideoPlayer::GetCameraBrightness(long & lValue, long& lMax, long& lMin, long& lSteppingDelta, long& lDefault)
{
    if(m_pCaptureFilter == NULL) return FALSE;
    CComPtr<IAMVideoProcAmp> ptrAMVideoProcAmp;
    HRESULT hr = m_pCaptureFilter->QueryInterface( IID_IAMVideoProcAmp, (void**)&ptrAMVideoProcAmp);
    if(FAILED(hr))
    {
        return FALSE;
    }

    long lCapsFlags;

    hr = ptrAMVideoProcAmp->GetRange(
        VideoProcAmp_Brightness, 
        &lMin,
        &lMax,
        &lSteppingDelta,
        &lDefault,
        &lCapsFlags);

    if(FAILED(hr))
    {
        return FALSE;
    }

    hr = ptrAMVideoProcAmp->Get(
        VideoProcAmp_Brightness, 
        &lValue,
        &lCapsFlags);

    if(FAILED(hr))
    {
        return FALSE;
    }

    return TRUE;

}

//@功能:获取摄像头对比度
BOOL CVideoPlayer::GetCameraContrast(long & lValue, long& lMax, long& lMin, long& lSteppingDelta, long& lDefault)
{
    if(m_pCaptureFilter == NULL) return FALSE;
    CComPtr<IAMVideoProcAmp> ptrAMVideoProcAmp;
    HRESULT hr = m_pCaptureFilter->QueryInterface( IID_IAMVideoProcAmp, (void**)&ptrAMVideoProcAmp);
    if(FAILED(hr))
    {
        return FALSE;
    }

    long lCapsFlags;
    
    hr = ptrAMVideoProcAmp->GetRange(
        VideoProcAmp_Contrast, 
        &lMin,
        &lMax,
        &lSteppingDelta,
        &lDefault,
        &lCapsFlags);

    if(FAILED(hr))
    {
        return FALSE;
    }


    hr = ptrAMVideoProcAmp->Get(
        VideoProcAmp_Contrast, 
        &lValue,
        &lCapsFlags);

    if(FAILED(hr))
    {
        return FALSE;
    }
    return  TRUE;
}



//@功能:获取摄像头Gamma值
BOOL CVideoPlayer::GetCameraGamma(long & lValue, long& lMax, long& lMin, long& lSteppingDelta, long& lDefault)
{
    if(m_pCaptureFilter == NULL) return FALSE;
    CComPtr<IAMVideoProcAmp> ptrAMVideoProcAmp;
    HRESULT hr = m_pCaptureFilter->QueryInterface( IID_IAMVideoProcAmp, (void**)&ptrAMVideoProcAmp);
    if(FAILED(hr))
    {
        return FALSE;
    }

    long lCapsFlags;

    hr = ptrAMVideoProcAmp->GetRange(
        VideoProcAmp_Gamma, 
        &lMin,
        &lMax,
        &lSteppingDelta,
        &lDefault,
        &lCapsFlags);

    if(FAILED(hr))
    {
        return FALSE;
    }

    hr = ptrAMVideoProcAmp->Get(
        VideoProcAmp_Gamma, 
        &lValue,
        &lCapsFlags);

    if(FAILED(hr))
    {
        return FALSE;
    }
    return TRUE;

}

/*
//@功能:设置摄像头的放大增益系数
BOOL CVideoPlayer::SetCameraGain(const long& lGain)
{
    if(m_pCaptureFilter == NULL) return FALSE;
    CComPtr<IAMVideoProcAmp> ptrAMVideoProcAmp;
    HRESULT hr = m_pCaptureFilter->QueryInterface( IID_IAMVideoProcAmp, (void**)&ptrAMVideoProcAmp);
    if(FAILED(hr))
    {
        return FALSE;
    }

    long lMin, lMax, lSteppingDelta, lDefault, lCapsFlags;

    hr = 
        ptrAMVideoProcAmp->GetRange(
        VideoProcAmp_Gain, 
        &lMin,
        &lMax,
        &lSteppingDelta,
        &lDefault,
        &lCapsFlags);

    if(FAILED(hr))
    {
        return FALSE;
    }

    long lValue = lGain;
    if(lValue < lMin)
    {
        lValue = lMin;
    }
    else if(lValue > lMax)
    {
        lValue = lMax;
    }
    hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Gain, lValue, lCapsFlags);

    if(FAILED(hr))
    {
        return FALSE;
    }

    return TRUE;
}
*/

const RECT& CVideoPlayer::GetDisplayArea() const
{
    return m_rcDispArea;
}

void CVideoPlayer::SetDisplayArea(const RECT& rcDispArea)
{
    m_rcDispArea = rcDispArea;

    int nWidth  = rcDispArea.right  - rcDispArea.left;
    int nHeight = rcDispArea.bottom - rcDispArea.top;

    m_oMemFrame.SetSize(nWidth, nHeight);
}

void CVideoPlayer::SetDisplayWnd(HWND hWnd)
{
	m_hPlayWnd = hWnd;
}

void CVideoPlayer::SetNotifyWnd(HWND hWnd)
{
	m_hNotifyWnd = hWnd;
}


//@功能:在窗体中显示视频帧
//
//@参数:pYUY2Frame, YUY2图片帧
//      nImageWidth,  图片宽度
//      nImageHeight, 图片高度
//BOOL CVideoPlayer::DisplayFrame(const BYTE* pYUY2Frame, int nImageWidth, int nImageHeight)


//@功能:在窗体中显示视频帧
//
//@参数:pARGBFrame  , ARGB图片帧
//      nImageWidth , 图片宽度
//      nImageHeight, 图片高度
BOOL CVideoPlayer::DisplayFrame(const BYTE* pARGBFrame, int nImageWidth, int nImageHeight)
{
	if(!IsWindow(m_hPlayWnd)) return FALSE;
	if(!IsWindowVisible(m_hPlayWnd)) return FALSE;
	if(!pARGBFrame) return FALSE;


    /*
	if(m_oRealFrameDib.GetWidth() != nImageWidth|| m_oRealFrameDib.GetHeight() != nImageHeight)
	{
		m_oRealFrameDib.Create(nImageWidth, nImageHeight);
	}


    //注意这里m_oRGB的内存地址是16字节对齐的
    unsigned char*  pFirstRGBLineData = (BYTE*)m_oRGB.GetData() + (nImageHeight-1)*nImageWidth*3;
	//YUY2ToDIB(pYUY2Frame, nImageWidth, nImageHeight, m_oRealFrameDib.GetBits());
    ARGBTo24BitDIB_SSSE3(pARGBFrame, nImageWidth, nImageHeight, pFirstRGBLineData);

    memcpy(m_oRealFrameDib.GetBits(),(void*)m_oRGB.GetData(), nImageHeight*nImageWidth*3);

    */
	if(m_oRealFrameARGB.GetWidth() != nImageWidth|| m_oRealFrameARGB.GetHeight() != nImageHeight)
	{
		m_oRealFrameARGB.SetSize(nImageWidth, nImageHeight);
	}
     memcpy(m_oRealFrameARGB.GetBuffer(), pARGBFrame, nImageHeight*nImageWidth*4);



    int nWidth  = m_rcDispArea.right  - m_rcDispArea.left;
    int nHeight = m_rcDispArea.bottom - m_rcDispArea.top ;

	SetStretchBltMode(m_oMemFrame.GetDC(), HALFTONE);
    
	//在窗体上显示图片
    ::StretchBlt(
        m_oMemFrame.GetDC(),
        0,
        0,
        nWidth,
        nHeight,
        m_oRealFrameARGB.GetDC(),
        0,
        0,
       m_oRealFrameARGB.GetWidth(),
        m_oRealFrameARGB.GetHeight(),
        SRCCOPY);

    DisplayText(m_oMemFrame.GetDC(), nImageWidth, nImageHeight);

	HDC hDC = GetDC(m_hPlayWnd);
    BitBlt(
        hDC,
        m_rcDispArea.left,
        m_rcDispArea.top,
        nWidth,
        nHeight,
        m_oMemFrame.GetDC(),
        0,
        0,
        SRCCOPY);

    ReleaseDC(m_hPlayWnd, hDC);

	return TRUE;
}


//@功能:在视频画面中显示文字
void CVideoPlayer::SetDisplayInfo(LPCTSTR lpszInfo, int nDisplayTimes)
{
    //m_strDisplayText       = lpszInfo;
    //m_nDisplayTimesCounter = 0;
    //m_bTextIsValidate      = TRUE;
	//CRect  rcDisplayArea(0, 0, m_oRealFrameARGB.GetWidth(), m_oRealFrameARGB.GetHeight());

    //TOSDText::RectF rcDisplayArea = { 0.0, 0.0, 1.0, 1.0 };
    RectF rcDisplayArea = { 0.0, 0.0, 1.0, 1.0 };
	AddOSDText(
		E_OSDTEXT_TYPE_ERROR_INFO,
		lpszInfo,
		rcDisplayArea,
		DT_VCENTER | DT_CENTER | DT_SINGLELINE,
		40L,
		_T("Times New Roman"),
        nDisplayTimes);

}


BOOL CVideoPlayer::AddOSDText(
	EOSDTextType eOSDTextType,
	const TCHAR* szText,
	//const TOSDText::RectF&  rcDisplayArea,
    const RectF&  rcDisplayArea,
	UINT dwDrawTextFormat,
	LONG lFontSize,
	const TCHAR* lpszFontFaceName,
	int nDisplayTimes)
{
	if (eOSDTextType < 0 || eOSDTextType >= E_OSDTEXT_TYPE_COUNT)
	{
		return FALSE;
	}

	TOSDText& osd = m_osdtext[eOSDTextType];

	osd.SetText(szText);
	osd.SetFont(lFontSize, lpszFontFaceName);
	osd.SetDisplayArea(rcDisplayArea, dwDrawTextFormat);
	osd.SetDisplayTimes(nDisplayTimes);

	return TRUE;

}


void CVideoPlayer::ClearOSDText(EOSDTextType eOSDTextType)
{
	if (eOSDTextType < 0 || eOSDTextType >= E_OSDTEXT_TYPE_COUNT)
	{
		return;
	}
	TOSDText& osd = m_osdtext[eOSDTextType];
	osd.SetText(_T(""));
}

//在画面中显示文字信息
void CVideoPlayer::DisplayText(HDC hDC, int nImageWidth, int nImageHeight)
{
	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, RGB(255, 255, 0));

	for (int i = 0; i < E_OSDTEXT_TYPE_COUNT; i++)
	{
		TOSDText& osd = m_osdtext[i];
		if (0 == osd.GetDisplayTimesCounter()) continue;

		if (0 == _tcsclen(osd.GetText())) continue;
		HFONT hFontOld = (HFONT)SelectObject(hDC, osd.GetFont());
        //TOSDText::RectF normalized_text_area  = osd.GetDisplayArea();
        RectF normalized_text_area = osd.GetDisplayArea();

        int nDispAreaWidth  = m_rcDispArea.right  - m_rcDispArea.left;
        int nDispAreaHeight = m_rcDispArea.bottom - m_rcDispArea.top ;

        //将归一化矩形区域转化为实际的显示区域
        RECT rcTextArea;
        rcTextArea.left   = /*m_rcDispArea.left +*/ normalized_text_area.left     * nDispAreaWidth ;
        rcTextArea.right  = /*m_rcDispArea.left +*/ normalized_text_area.right    * nDispAreaWidth ;
        rcTextArea.top    = /*m_rcDispArea.top  +*/ normalized_text_area.top      * nDispAreaHeight;
        rcTextArea.bottom = /*m_rcDispArea.top  +*/ normalized_text_area.bottom   * nDispAreaHeight;


		UINT dwFormat = osd.GetDrawTextFormat();
		
		RECT rcNeedArea;
		DrawText(hDC, (LPCTSTR)osd.GetText(), _tcsclen(osd.GetText()), &rcNeedArea, dwFormat|DT_CALCRECT);
		int nNeedArea_Width = rcNeedArea.right - rcNeedArea.left;
		int nTextArea_Width = rcTextArea.right - rcTextArea.left;
		if (nNeedArea_Width > nTextArea_Width && nNeedArea_Width !=0 )
		{//显示不小，按比例缩小字体尺寸,使的文字内容在TextArea中能够完整显示。
			long lNewFontSize = osd.GetFontSize() * nTextArea_Width / nNeedArea_Width;
			SelectObject(hDC, hFontOld);
			osd.SetFont(lNewFontSize, osd.GetFontName());
			hFontOld = (HFONT)SelectObject(hDC, osd.GetFont());
		}

		DrawText(hDC, (LPCTSTR)osd.GetText(), _tcsclen(osd.GetText()), &rcTextArea, dwFormat);
		
		SelectObject(hDC, hFontOld);

		if (osd.GetDisplayTimesCounter() != -1)
		{
			osd.DecDisplayTimesCounter();
        }

	}//for
    
}

void CVideoPlayer::UpdateVideoStreamForamtInfo(int nImageWidth, int nImageHeight, DWORD ImageType, float fps, int nId)
{

     //视频压缩格式,
	 CString  Compress;
	 Compress.Format(_T("%c%c%c%c"), ImageType & 0xFF, (ImageType >> 8) & 0xFF, (ImageType >> 16) & 0xFF, (ImageType >> 24) & 0xFF);
	 ////这个是需要在高级设置的对话框中进行显示的，每个摄像头的所有格式都需要各自显示的，这个只是当前选中的格式而已
	 m_sCurrentCameraResolution.Format(_T("%d X %d %s"), nImageWidth, nImageHeight, Compress);
	 /////这个是在帧率下显示的只显示第一个摄像头的信息	 
	 if (theApp.ScreenMode() == EScreenModeSingle)
	 {
		 _stprintf_s(
			       m_szStatusText,
			       _countof(m_szStatusText),
			       _T("%s(%d*%d-%c%c%c%c)"),
			        g_oResStr[IDS_STRING442],
		            nImageWidth,
		            nImageHeight,
		            ImageType & 0xFF, 
		            (ImageType >> 8) & 0xFF, 
		            (ImageType >> 16) & 0xFF, 
		            (ImageType >> 24) & 0xFF
		             );

	      PostMessageW(m_hNotifyWnd, WM_CAMERA_STATUS_NOTIFY, (WPARAM)(LPCTSTR)m_szStatusText, (LPARAM)nId);
		  PostMessage(this->m_hNotifyWnd, WM_FPSNOTIFY, (WPARAM)(fps*2), (LPARAM)nId);
	     ////SendMessage(m_hNotifyWnd, WM_CAMERA_STATUS_NOTIFY, (WPARAM)(LPCTSTR)strStatusText, (LPARAM)m_nID);
	}
	else
	{
         CString strVideoInfo;
         strVideoInfo.Format(
                           _T("#%d %c%c%c%c %d*%d@%.0f"),
                           m_nID+1,
                           ImageType & 0xFF, (ImageType >> 8) & 0xFF, (ImageType >> 16) & 0xFF, (ImageType >> 24) & 0xFF,
                           nImageWidth,
                           nImageHeight,
                           fps*2);

        //TOSDText::RectF textArea = { 0.0, 0.0, 1.0, 1.0 };
        RectF textArea = { 0.0, 0.0, 1.0, 1.0 };
        AddOSDText(
                   E_OSDTEXT_TYPE_FORMAT_INFO,
                   (LPCTSTR)strVideoInfo,
                   textArea,
                   DT_TOP | DT_LEFT | DT_SINGLELINE,
                   10L,
                   _T("Times New Roman"),
                  -1 );

		PostMessage(m_hNotifyWnd, WM_CAMERA_STATUS_NOTIFY, (WPARAM)(LPCTSTR)g_oResStr[IDS_STRING442], (LPARAM)nId);
	}
}

CString CVideoPlayer::CurrentCameraResolution()
{
	return m_sCurrentCameraResolution;
}


