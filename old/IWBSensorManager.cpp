#include "stdafx.h"
//#include "headers.h"
CIWBSensorManager::CIWBSensorManager()
:
m_hVideoDispWnd(NULL),
m_nAutoCalibrateTryTimes(0),
m_nCurrentSensorID(-1),
m_uCurrentSelectSensordId(0)
{
    m_pSpotListProcessor = new CSpotListProcessor();

    if(theApp.GetScreenMode()>= EScreenModeDouble)
    {//双屏模式
        //<<xuke
        Init(int(theApp.GetScreenMode()) + 1);
        //xuke>>
    }
    else
    {
        Init(1);
    }
 }

CIWBSensorManager::~CIWBSensorManager()
{
    Uninit();
    delete m_pSpotListProcessor;
}

//@功能:初始化
//@参数: nSensorCount, CMOS传感器数目
//       hVideoDispWnd,视频显示窗口
void CIWBSensorManager::Init(int nSensorCount)
{
    if(m_vecSensors.size() != nSensorCount)
    {
        m_vecSensors.resize(nSensorCount);
       for(int i=0; i<nSensorCount; i++)
       {
            m_vecSensors[i] = new CIWBSensor(i);
            //m_vecSensors[i]->GetPenPosDetector()->SetSpotListProcessor(&this->m_oSpotListProcessor);
            m_vecSensors[i]->GetPenPosDetector()->SetSpotListProcessor(this->m_pSpotListProcessor);
        }//for
    }


    m_vecVideoLayout.resize(nSensorCount);
    m_vecSplitter.resize(nSensorCount -1);

    //m_oSpotListProcessor.Init(nSensorCount);
    m_pSpotListProcessor->Init(nSensorCount);


    //int nCxScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    //int nCyScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
    int nCyScreen = GetSystemMetrics(SM_CYSCREEN);

    m_oScreenLayoutDesigner.Init(nSensorCount, nCxScreen, nCyScreen);

    ApplyScreenLayout();

}


//@功能:反初始化
void CIWBSensorManager::Uninit()
{
    for (size_t i = 0; i<m_vecSensors.size(); i++)
    {
        delete m_vecSensors[i];
        m_vecSensors[i] = NULL;
    }

    m_vecSensors.clear();
    m_oScreenLayoutDesigner.Uninit();
}


//@功能:给IWBSensor对象分派摄像头
//@参数:Camera设备列表
void CIWBSensorManager::AssignCamera(const CUsbCameraDeviceList& cameraList)
{
    //传感器个数
    int n = m_vecSensors.size();

    //传感器已被赋值标志
    std::vector<bool> sensor_assigned;
    sensor_assigned.resize(n);
    for(int i=0; i < n; i++)
    {
        sensor_assigned[i] = false;
    }

    //摄像头实例已被分配标志
    int m = cameraList.GetDeviceInstanceCount();
    std::vector<bool> camear_instance_assigned;
    camear_instance_assigned.resize(m);
    for(int i=0; i < m; i++)
    {
        camear_instance_assigned[i] = false;
    }

    //优先分配设备路径完全匹配的摄像头
    for(int i=0; i<n; i++)
    {
        CIWBSensor& sensor = *m_vecSensors[i];
        const TCaptureDeviceInstance& devInfo = sensor.GetDeviceInfo();
        for(int j=0; j < m; j++)
        {
            if(camear_instance_assigned[j]) continue;

            const TCaptureDeviceInstance*  pDevInst = cameraList.GetCaptureDeviceInstance(j);
            if(devInfo.m_strDevPath == pDevInst->m_strDevPath)
            {
                sensor.SetDeviceInfo(*pDevInst);
                camear_instance_assigned[j] = true;
                sensor_assigned[i] = true;
                break;
            }
        }
    }  

    //剩下的按顺序分配
    for(int i=0; i < n; i++)
    {   
        if(sensor_assigned[i]) continue;//已分配
        CIWBSensor& sensor = *m_vecSensors[i];
        for(int j=0; j<m; j++)
        {
            if(camear_instance_assigned[j]) continue;
            const TCaptureDeviceInstance*  pDevInst = cameraList.GetCaptureDeviceInstance(j);

            sensor.SetDeviceInfo(*pDevInst);
            camear_instance_assigned[j] = true;
            sensor_assigned[i] = true;
            break;
        }
    }

    //检查"未分配到设备的sensor"的设备路径是否与"已分配到设备的sensor“的设备路径相同。
    //若相同则"未分配到设备的sensor"的设备路径清空。
    //为什么需要要怎样做呢,
    //存在这种场景, 两屏模式，只插入一个摄像头，
    //问题再现步骤
    //step1: 将相机分配给第二个屏幕播放，弹出属性对话框按下确定,目的是更新g_tSysCfgData, 这样g_tSysCfgData中记录了
    //       第二个sensor的设备路径为相机的硬件路径。
    //step2: 将分屏模式切换到单屏模式，弹出属性对话框按下确定,目的是更新g_tSysCfgData,g_tSysCfgData中记录了
    //       第一个sensor的设备路径为相机的硬件路径，至此g_tSysCfgData中的两个sensor拥有相同设备路径了
    //step3: 将分屏模式切换到双屏模式，出现如下问题:
    //       两个sensor的设备路径完全相同，则它们无法交换播放窗体
    //       播放时一个窗体正常播放，另外一个窗体视频播放失败
    //
    //g_tSysCfgData.vecSensorConfig是一个元素个数为SENSOR_NUMBER的数组。
    for (int i = 0; i < n; i++)
    {
        if (sensor_assigned[i]) continue;//后续只处理"未分配到设备的sensor",已经分配了设备的则跳过
        
        CIWBSensor& sensorUnassigned = *m_vecSensors[i];
        const TCaptureDeviceInstance& devInfoUnassigned = sensorUnassigned.GetDeviceInfo();

        for (int j = 0; j < n; j++)
        {
            if (!sensor_assigned[j]) continue;//跳过"未分配到设备的sensor", 后续只与"已经分配到设备的sensor"比较。
            if (i == j) continue;//自己不跟自己比较

            const CIWBSensor& sensorAssigned = *m_vecSensors[j];
            const TCaptureDeviceInstance& devInfoAssigned = sensorAssigned.GetDeviceInfo();
            
            if (0 == _tcsicmp(devInfoUnassigned.m_strDevPath, devInfoAssigned.m_strDevPath))
            {
                //发现了"未分配到设备的sensor"和"已经分配到设备的sensor"的设备路径相同
                //则清空"未分配到设备的sensor"的设备路径。
                TCaptureDeviceInstance devPathClearedInstance = devInfoUnassigned;
                devPathClearedInstance.m_strDevPath = _T("");
                sensorUnassigned.SetDeviceInfo(devPathClearedInstance);
            }

        }
    }


}

//@功能:设置视频显示参数
//@参数:hDispWnd,  视频显示窗体句柄
//      hNotifyWnd, 播放消息接收窗体句柄
//      rcVideoDispArea, 视频显示区域
void CIWBSensorManager::SetVideoDispParams(HWND hDispWnd, HWND hNotifyWnd, RECT& rcVideoDispArea)
{
    int nSensorCount = (int)m_vecSensors.size();
    if(nSensorCount == 0) return;
    SetVideoDisplayArea(rcVideoDispArea);

    for(int i=0; i<nSensorCount; i++)
    {
        CIWBSensor* pSensor = m_vecSensors[i];
        pSensor->SetVideoDispParams(hDispWnd, m_vecVideoLayout[i], hNotifyWnd);

    }

    m_hVideoDispWnd = hDispWnd;
}


//@功能:开始运行
//@参数:
//		nSensorID, 传感器编号(0~n), -1时指全体传感器
BOOL CIWBSensorManager::StartRunning(int nSensorID)
{
    LOG_FUNC_CALL;

    int nSensorCount = (int)m_vecSensors.size();
    if(nSensorCount == 0) return FALSE;

    if(IsAllSensorStopped())
    {//处于所有传感器停止运行状态,则要启动光斑处理器

        //光斑处理器开始运行
        //<<added by toxuke@gmail.com,2015/02/15
        //this->m_oSpotListProcessor.StartProcess();
        this->m_pSpotListProcessor->StartProcess();
        //>>


        //开启系统下手势识别
        g_oWinGR.Start();
        /*
        关联windows下手势识别和功能执行上下文
        */
        g_oWinGR.AttachExceContext((IWGRUtilContext *)&m_execContext);
    }

    if(nSensorID != -1 && nSensorID >= 0 && nSensorID < nSensorCount)
    {
        if(nSensorID < 0 || nSensorID >= nSensorCount) return FALSE;
        CIWBSensor* pSensor = m_vecSensors[nSensorID];
        if(!pSensor->IsDetecting())pSensor->Run();
    }
    else
    {
        for(int i=0; i<nSensorCount; i++)
        {
            CIWBSensor* pSensor = m_vecSensors[i];

            if(!pSensor->IsDetecting())pSensor->Run();
        }
    }
    return TRUE;
}


//@功能:停止运行
//@参数:nSensorID, 传感器编号(0~n), -1时指全体传感器 
BOOL CIWBSensorManager::StopRunning(int nSensorID)
{
	GetCfgData(g_tSysCfgData);

    int nSensorCount = (int)m_vecSensors.size();
    if(nSensorCount == 0) return FALSE;

    if(nSensorID != -1 && nSensorID >= 0 && nSensorID < nSensorCount)
    {
        if(nSensorID < 0 || nSensorID >= nSensorCount) return FALSE;
        CIWBSensor* pSensor = m_vecSensors[nSensorID];
        pSensor->Stop();
    }
    else
    {
        for(int i=0; i<nSensorCount; i++)
        {
            CIWBSensor* pSensor = m_vecSensors[i];
            pSensor->Stop();
        }
    }
    

    if(IsAllSensorStopped())
    {//所有传感器已经关闭则, 关闭所有传感器共用的对象。
        //关闭windows下手势识别
        g_oWinGR.Stop();

        //光斑处理器停止运行
        //<<added by toxuke@gmail.com,2015/02/15
        //this->m_oSpotListProcessor.StopProcess();
        this->m_pSpotListProcessor->StopProcess();
        //>>
    }


    //更新界面显示
    InvalidateRect(m_hVideoDispWnd, NULL, TRUE);
    UpdateWindow(m_hVideoDispWnd);
    return TRUE;
}

//@功能:改变视频显示区域的尺寸
void CIWBSensorManager::SetVideoDisplayArea(const RECT& rcNewDisplayArea)
{
    int nSensorCount = (int)m_vecSensors.size();

    if(nSensorCount == 0) return;

    UpdateVideoLayout(rcNewDisplayArea);

    for(int i=0; i<nSensorCount; i++)
    {
        CIWBSensor* pSensor = m_vecSensors[i];
        const RECT& rcNewArea = this->m_vecVideoLayout[i];
        pSensor->GetVideoPlayer()->SetDisplayArea(rcNewArea);
    }

}


//@功能:切换设想头到指定的模式
void CIWBSensorManager::SwitchToMode(ESensorLensMode eMode, int nSensorID)
{
    int nSensorCount = (int)m_vecSensors.size();

    if(nSensorID == -1)
    {
        for(int i=0; i<nSensorCount; i++)
        {
            CIWBSensor* pSensor = m_vecSensors[i];
            pSensor->SwitchLensMode(eMode);
        }
    }
    else  if(0 <= nSensorID && nSensorID < nSensorCount)
    {
        m_vecSensors[nSensorID]->SwitchLensMode(eMode);
    }

}

//@功能:获取工作模式
//@说明:全局的工作模式与第一个相机保持一致
ESensorLensMode CIWBSensorManager::GetLensMode()const
{
    ESensorLensMode eMode = E_NORMAL_USAGE_MODE;

     int nSensorCount = (int)m_vecSensors.size();
     
     if(nSensorCount > 0)
     {
         eMode = m_vecSensors[0]->GetLensMode();
     }

     return eMode;
}


//@功能:判断是否有处于正常使用模式下的相机
BOOL CIWBSensorManager::HasNormalUsageCamera()const
{
    int nSensorCount = (int)m_vecSensors.size();
    BOOL bYes = FALSE;

    for (int i = 0; i < nSensorCount; i++)
    {
        if (m_vecSensors[i]->GetLensMode() == E_NORMAL_USAGE_MODE)
        {
            bYes = TRUE;
            break;
        }
    }

    return bYes;
}


void CIWBSensorManager::DrawSelectBound(HWND hWnd)
{

	HDC hDC = GetDC(hWnd);
	CBrush brushBlack;
	 brushBlack.CreateSolidBrush(RGB(0, 0, 0));

	CBrush* pBrushHalftone = CDC::GetHalftoneBrush();

	HBRUSH hBrushOld = NULL;

	UINT nSensorCount = m_vecSensors.size();
	for (UINT uSensorIdx = 0; uSensorIdx< nSensorCount; uSensorIdx++)
	{
		const RECT& rcArea = m_vecVideoLayout[uSensorIdx];
		RECT  rcInternal  = rcArea;
		RECT  rcExternal  = rcArea;
		InflateRect(&rcExternal, SELECT_BOUND_WIDTH, SELECT_BOUND_WIDTH);


		HRGN rgn = ::CreateRectRgnIndirect(&rcExternal);
		HRGN rgnInternal = ::CreateRectRgnIndirect(&rcInternal);
		::CombineRgn(rgn, rgnInternal, rgn, RGN_XOR);


		SelectClipRgn(hDC, rgn);

		RECT rcClipBox;
		GetClipBox(hDC, &rcClipBox);


		if (uSensorIdx == m_uCurrentSelectSensordId)
		{
			hBrushOld = (HBRUSH)::SelectObject(hDC, pBrushHalftone->GetSafeHandle());
			//https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-createpatternbrush
			//A brush created by using a monochrome (1 bit per pixel) bitmap has the text and background colors of the device context to which it is drawn. 
			//Pixels represented by a 0 bit are drawn with the current text color; pixels represented by a 1 bit are drawn with the current background color.
			SetTextColor(hDC, RGB(0, 0, 0));
			SetBkColor(hDC, RGB(238,149,114)); //RGB(255, 255, 255)
		}
		else
		{
			hBrushOld = (HBRUSH)::SelectObject(hDC, brushBlack.GetSafeHandle());
		}

		PatBlt(
			hDC,
			rcClipBox.left,
			rcClipBox.top,
			rcClipBox.right - rcClipBox.left,
			rcClipBox.bottom - rcClipBox.top,
			PATCOPY);

		 SelectClipRgn(hDC, NULL);
		::SelectObject(hDC, hBrushOld);

		::DeleteRgn(rgn);
		::DeleteRgn(rgnInternal);


	}

	ReleaseDC(hWnd, hDC);
}

//@参数:绘制布局框架
//@窗体:hWnd, 窗体句柄
void CIWBSensorManager::DrawLayoutFrame(HWND hWnd)
{   
    if(hWnd == NULL)
    {
        hWnd = m_hVideoDispWnd;
    }

    if(NULL == hWnd) return;

    int nSensorCount = m_vecSensors.size();

    HDC hDC = GetDC(hWnd);

    COLORREF clrBkgnd   = RGB(16,0,16);//key color for overlay surface
    COLORREF clrFace    = GetSysColor(COLOR_3DFACE);
    COLORREF clrShadow  = GetSysColor(COLOR_3DDKSHADOW);
    COLORREF clrHilight = GetSysColor(COLOR_3DHILIGHT);
    for(int i=0; i<nSensorCount; i++)
    {
        RECT& rcArea = m_vecVideoLayout[i];
        CIWBSensor* pSensor = m_vecSensors[i];

        //正在更新不刷背景，否则会闪烁
        if(!pSensor->GetVideoPlayer()->IsDetecting())
        {         
            //仿照CDC:FillSolidRect
            ::SetBkColor(hDC, clrBkgnd);
            ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rcArea, NULL, 0, NULL);
        }

        //绘制Splitter
        if(i != nSensorCount - 1)
        {
            RECT& rcSplitter = m_vecSplitter[i];
            //Draw3dRect
            ::SetBkColor(hDC, clrFace);
            ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rcSplitter, NULL, 0, NULL);

            ::SetBkColor(hDC, clrHilight);

            RECT rc;
            //top highlight line
            rc.left   = rcSplitter.left;
            rc.top    = rcSplitter.top ;
            rc.right  = rcSplitter.right - 1;
            rc.bottom = rcSplitter.top  + 1;
            ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

            //right vertical highlight line
            rc.left   = rcSplitter.left;
            rc.top    = rcSplitter.top ;
            rc.right  = rcSplitter.left   + 1;
            rc.bottom = rcSplitter.bottom - 1;
            ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

            ::SetBkColor(hDC, clrShadow);
            rc.left   = rcSplitter.right - 1;
            rc.top    = rcSplitter.top      ;
            rc.right  = rcSplitter.right    ;
            rc.bottom = rcSplitter.bottom   ;
            ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

            rc.left   = rcSplitter.left   ;
            rc.top    = rcSplitter.bottom-1;
            rc.right  = rcSplitter.right   ;
            rc.bottom = rcSplitter.bottom  ;
            ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
        }

    }

    ReleaseDC(hWnd, hDC);

	DrawSelectBound(hWnd);
}


//@功能:更新视频布局
void CIWBSensorManager::UpdateVideoLayout(const RECT& rcDisplayArea)
{
    int nSensorCount = m_vecSensors.size();

    if(nSensorCount == 0) return;

    int nDispWndWidth  = rcDisplayArea.right   - rcDisplayArea.left;
    int nDispWndHeight = rcDisplayArea.bottom  - rcDisplayArea.top ;

    int nEachVideoHeight   = nDispWndHeight;
    int nVideoDisplayWidth = nDispWndWidth - (nSensorCount - 1)*SPLITTER_WIDTH;

    int nEachVideoWidth          = nVideoDisplayWidth/nSensorCount;
    int nVideoWidthResidualInc   = nVideoDisplayWidth%nSensorCount;  
    int nVideoWidthResidualCount = 0;

    int left = rcDisplayArea.left;
    int top  = rcDisplayArea.top ;

    for(int i=0; i<nSensorCount; i++)
    {
        RECT& rcArea = m_vecVideoLayout[i];

        rcArea.left   = left;
        rcArea.top    = top;
        rcArea.right  = left + nEachVideoWidth;
        rcArea.bottom = top  + nEachVideoHeight;

        //右边界值四舍五入
        nVideoWidthResidualCount += nVideoWidthResidualInc;
        if(nVideoWidthResidualCount >= nSensorCount)
        {
            rcArea.right  ++;
            nVideoWidthResidualCount -= nSensorCount;

        }


        left += nEachVideoWidth;

        if(i != nSensorCount - 1)
        {
            RECT& rcSplitter = m_vecSplitter[i];
            rcSplitter.left   = left;
            rcSplitter.top    = top;
            rcSplitter.right  = left + SPLITTER_WIDTH;
            rcSplitter.bottom = top  + nEachVideoHeight;

            left += SPLITTER_WIDTH;
        }


		//
		::InflateRect(&rcArea, -SELECT_BOUND_WIDTH, -SELECT_BOUND_WIDTH);

    }//for

}

//@功能:载入配置数据
//@参数:allCfgData, 所有图像传感器的配置信息
void CIWBSensorManager::SetCfgData( TSysConfigData& sysCfgData)
{
    for(size_t i=0; i < sysCfgData.vecSensorConfig.size(); i++)
    {
        if(i >= m_vecSensors.size()) break;

        m_vecSensors[i]->SetCfgData(sysCfgData.vecSensorConfig[i], &sysCfgData.globalSettings);
        //设置工作模式
        //m_vecSensors[i]->SetLensMode(sysCfgData.globalSettings.eLensMode);
        m_vecSensors[i]->SetLensMode(sysCfgData.vecSensorConfig[i].eLensMode);
    }
    //
	if (theApp.GetUSBKeyTouchType() == E_DEVICE_FINGER_TOUCH_WHITEBOARD || theApp.GetUSBKeyTouchType() == E_DEVICE_PEN_TOUCH_WHITEBOARD)
	{
		sysCfgData.globalSettings.bTouchHIDMode = true;
		sysCfgData.globalSettings.bTouchTUIOMode = false;
	}
    this->m_pSpotListProcessor->GetVirtualHID().SetHIDMode(sysCfgData.globalSettings.eHIDDeviceMode);

	this->m_pSpotListProcessor->GetVirtualHID().SetSinglePointMode(sysCfgData.globalSettings.bSinglePointMode?true:false);

	this->m_pSpotListProcessor->GetVirtualHID().SetAirOperateMode(sysCfgData.globalSettings.bAirOperatePermission?true:false, sysCfgData.globalSettings.eClickMode);
	//////add by zhaown 2019.9.25
	////如果两种模式都为false的话，那么强行HID模式响应
	if ((!sysCfgData.globalSettings.bTouchHIDMode) && (!sysCfgData.globalSettings.bTouchTUIOMode))
	{
		this->m_pSpotListProcessor->GetVirtualHID().SetTouchHIDMode(true);
		sysCfgData.globalSettings.bTouchHIDMode = true;
	}
	else
	{
    	this->m_pSpotListProcessor->GetVirtualHID().SetTouchHIDMode(sysCfgData.globalSettings.bTouchHIDMode?true:false);
	}
	this->m_pSpotListProcessor->GetVirtualHID().SetTouchTUIOMode(sysCfgData.globalSettings.bTouchTUIOMode?true:false);


    //<Added by Jiqw 201412041914
    //<Added Reason: 解决触屏模式下，windows两触点手势与windows下手势的冲突问题/>
    g_oWGRConfig.SetHIDMode(E_DEV_MODE_TOUCHSCREEN == sysCfgData.globalSettings.eHIDDeviceMode);
    g_oGLBoardGR.SetIsTouchPadMode(E_DEV_MODE_TOUCHSCREEN == sysCfgData.globalSettings.eHIDDeviceMode);

    //Added by Jiqw 201412041914>    

    if (theApp.GetScreenMode() >= EScreenModeDouble)
    {
        //如果保存的屏幕划分信息和多屏屏接的数目一致，则载入屏幕划分信息
        //选择配置文件中屏幕数等于当前实际屏幕数的布局,

		const ESplitScreeMode& eSelectedSplitScreenMode = sysCfgData.screenLayoutManger.GetSelectedSplitScreenMode();
		
		const TScreenLayout* pScreenLayout = sysCfgData.screenLayoutManger.GetScreenLayout(eSelectedSplitScreenMode, m_vecSensors.size());

		if (pScreenLayout)
        {
			//this->m_oScreenLayoutDesigner.SetScreenRelativeLayouts(&pScreenLayout->vecScreens[0], pScreenLayout->vecScreens.size());
			//this->m_oScreenLayoutDesigner.SetRelativeMergeAreas(&pScreenLayout->vecMergeAreas[0], pScreenLayout->vecMergeAreas.size());
			this->m_oScreenLayoutDesigner.SetScreenLayout(eSelectedSplitScreenMode, pScreenLayout);
			this->ApplyScreenLayout();
		}

    }

    //2023/06/07, by xuke
    //<<
    this->UpdateCalibrateToleranceDistribute();
    //>>
}

void CIWBSensorManager::SetGlobalCfgData(TSysConfigData& sysCfgData)
{
	for (size_t i = 0; i < sysCfgData.vecSensorConfig.size(); i++)
	{
		if (i >= m_vecSensors.size()) break;
		m_vecSensors[i]->SetGlobalCfgData(&sysCfgData.globalSettings);
	}
}


//@功能:获取配置数据
//@参数:allCfgData, 所有图像传感器的配置信息
BOOL CIWBSensorManager::GetCfgData(TSysConfigData& sysCfgData)
{
    sysCfgData.globalSettings.eHIDDeviceMode = this->m_pSpotListProcessor->GetVirtualHID().GetHIDMode();

    size_t nSensorCount = m_vecSensors.size();
    if(nSensorCount == 0)return FALSE;
//  sysCfgData.vecSensorConfig.resize(nSensorCount);

    for(size_t i=0; i < nSensorCount; i++)
    {
        sysCfgData.vecSensorConfig[i] = m_vecSensors[i]->GetCfgData();
    }

    //全局工作模式和第一个相机的相机的工作模式保持一致。
    sysCfgData.globalSettings.eLensMode = m_vecSensors[0]->GetLensMode();
    
    if (theApp.GetScreenMode() >= EScreenModeDouble)
    {

		const TScreenLayout& screenLayout    = m_oScreenLayoutDesigner.GetScreenLayout();
		ESplitScreeMode     eSplitScreenMode = m_oScreenLayoutDesigner.GetSplitScreenMode();

		sysCfgData.screenLayoutManger.SetScreenLayout(eSplitScreenMode, screenLayout);
    }

    return TRUE;
}


//@功能:USB摄像头插入事件响应
//@参数:lpszDevicePath, 插入的USB设想头的设备路径
//void CIWBSensorManager::OnCameraPlugIn(LPCTSTR lpszDevicePath)
void CIWBSensorManager::OnCameraPlugIn(const TCaptureDeviceInstance& devInst)
{
    LOG_INF("CIWBSensorManager::OnCameraPlugIn(%s)\r\n", (const char*)CT2A(devInst.m_strDevPath));
    //优先分配设备路径完全匹配的摄像头
    BOOL bMatched = FALSE;
    int  nCandidateIndex  = -1;
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {
        CIWBSensor& sensor = *m_vecSensors[i];

        if(sensor.IsDetecting()) continue;
		//如果是从新插入摄像头，并且加密狗有变化
		sensor.UpdateTouchTypeFromUSBKey();

        const TCaptureDeviceInstance& devInfo = sensor.GetDeviceInfo();
        ////如果是路径相等的话还要比较是不是PID和VID相等//modify by zhaown 
        if( (_tcsicmp(devInfo.m_strDevPath, devInst.m_strDevPath) == 0) &&(devInfo.m_nPID == devInst.m_nPID)&&(devInfo.m_nVID == devInst.m_nVID))
        {
			sensor.SetDeviceInfo(devInst);
            //sensor.Run();
            StartRunning(sensor.GetID());

            bMatched = TRUE;
            break;
        }
        else if(nCandidateIndex == -1)
        {
            nCandidateIndex = i;
        }

    }//for

    if(!bMatched && nCandidateIndex != -1)
    {
        m_vecSensors[nCandidateIndex]->SetDeviceInfo(devInst);
        //m_vecSensors[nCandidateIndex]->Run();
        StartRunning(m_vecSensors[nCandidateIndex]->GetID());

    }
    //
}


//@功能:USB摄像头拔出事件响应,  插入的USB设想头的设备路径
void CIWBSensorManager::OnCameraPlugOut(LPCTSTR lpszDevicePath)
{
    LOG_INF("CIWBSensorManager::OnCameraPlugOut(%s)\r\n", (const char*)CT2A(lpszDevicePath));
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {
        CIWBSensor& sensor = *m_vecSensors[i];
        if(!sensor.IsDetecting()) continue;

        const TCaptureDeviceInstance& devInfo = sensor.GetDeviceInfo();

        if(_tcsicmp(devInfo.m_strDevPath, lpszDevicePath) == 0)
        {
            //如果正在进行光斑采集中，则提示是否退出光斑采集
            if(sensor.GetPenPosDetector()->IsCollectingSpot())
            {
                this->m_wndLightSpotSampling.OnDeviceMissing();
            }

            //调用设备丢失响应函数
            sensor.OnDeviceIsMissing();

            //停止运行
            this->StopRunning(sensor.GetID());

            sensor.ShowMissStatusInfo();

            break;
        }
    }


}


//@功能:根据位置确定图像是属于哪个传感器的
//@返回:非空，返回指向传感器对象的指针
//      NULL,未找到。
CIWBSensor* CIWBSensorManager::SensorFromPt(const POINT& ptPos)
{
    CIWBSensor* pSensor = NULL;
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {
        if(::PtInRect(&m_vecVideoLayout[i], ptPos))
        {
            pSensor = m_vecSensors[i];
            break;
        }
    }

    return pSensor;
}


void CIWBSensorManager::SelectAsCurrentSensor(CIWBSensor* pSensor)
{
	m_uCurrentSelectSensordId = pSensor->GetID();

	DrawSelectBound(m_hVideoDispWnd);
}

int  CIWBSensorManager::CurrentSelectSensor()
{
	return m_uCurrentSelectSensordId;
}

//CIWBSensor* CIWBSensorManager::GetSensor0()
//{
//	if (m_vecSensors.size() == 0) return NULL;
//	return m_vecSensors[0];
//}

CIWBSensor* CIWBSensorManager::GetSensor()
{
    
	if (m_vecSensors.size() == 0) return NULL;

	if (0 < m_uCurrentSelectSensordId && m_uCurrentSelectSensordId < m_vecSensors.size())
	{
		return m_vecSensors[m_uCurrentSelectSensordId];
	}
	else
	{
		return m_vecSensors[0];
	}
}

const CIWBSensor* CIWBSensorManager::GetSensor(int nID) const
{
    if ((size_t)nID >= m_vecSensors.size()) return NULL;
    return m_vecSensors[nID];	
}

CIWBSensor* CIWBSensorManager::GetSensor(int nID)
{
	if ((size_t)nID >= m_vecSensors.size()) return NULL;
	return m_vecSensors[nID];
}
//@功能:使能光笔
//@参数:bEnable,使能标志
void CIWBSensorManager::EnableOpticalPen(BOOL bEnable)
{
    for(size_t i=0; i<m_vecSensors.size(); i++)
    { 
         if(!bEnable)//如果是禁用光笔, 可以无条件禁用
         {
             m_vecSensors[i]->EnableOpticalPen(bEnable);
         }
         else
         {
             //如果是使能光笔，只有在"正常使用模式下"才能使能光笔, 这样可以避免其他模式下,干扰光点造成的光斑乱跳。
             if(m_vecSensors[i]->GetLensMode() == E_NORMAL_USAGE_MODE)
             {
                  m_vecSensors[i]->EnableOpticalPen(bEnable);
             }
         }
    }
}

//@功能:判断光笔是否在控制中
//@说明:只要有一支光笔在控制中,则返回TRUE
BOOL CIWBSensorManager::IsOpticalPenControlling()
{
   // BOOL bIsControlling = FALSE;
	BOOL bIsControlling = TRUE;
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {   
//      bIsControlling |= m_vecSensors[i]->IsOpticalPenControlling();
		bIsControlling &= m_vecSensors[i]->IsOpticalPenControlling();
    }

    return bIsControlling;
}

//@功能:开始图像传感器的自动校正流程
//@参数:ePattern, 校正图案模式
//      hNotifWindow, 校正结束后的通知消息"WM_AUTO_CALIBRATE_DONE"的接收窗体
//      nSensorID, -1,全部传感器校正
void  CIWBSensorManager::StartAutoCalibrate(E_AutoCalibratePattern ePattern, HWND hNotifyWindow, int nSensorID)
{
    if(nSensorID == -1)
    {
        //初始化校正结果数组
        m_vecCalibrateResults.resize(m_vecSensors.size());

        for(size_t i=0; i<m_vecCalibrateResults.size(); i++)
        {
            m_vecCalibrateResults[i] = FALSE;
        }

        //所有传感器禁用光笔
        for(size_t i=0; i<m_vecSensors.size(); i++)
        {   
            m_vecSensors[i]->EnableOpticalPen(FALSE);
        }


        if(m_vecSensors.size() >= 1)
        {
            for(size_t i=0; i<m_vecSensors.size(); i++)
            {
                if(m_vecSensors[i]->IsDetecting())
                {
                    m_nCurrentSensorID = i;
                    m_vecSensors[i]->StartAutoCalibrate(ePattern, hNotifyWindow);
                    
                    break;
                }
                else
                {
                    m_vecCalibrateResults[i] = FALSE;
                }
            }
        }

        m_eOperationMode = E_MODE_ALL_SENSOR;
    }
    else if( 0<= nSensorID && nSensorID < (int)m_vecSensors.size())
    {
        if(m_vecSensors[nSensorID]->IsDetecting())
        {
            m_nCurrentSensorID = nSensorID;
            m_vecSensors[nSensorID]->EnableOpticalPen(FALSE);
            m_vecSensors[nSensorID]->StartAutoCalibrate(ePattern, hNotifyWindow);

            m_eOperationMode = E_MODE_SINGLE_SENSOR;
            
            m_vecCalibrateResults.resize(1);
            m_vecCalibrateResults[0] = FALSE;
        }

    }

    m_nAutoCalibrateTryTimes = 0;
    m_hNotifyWindow = hNotifyWindow;
    m_eAutoCalibratePattern = ePattern;
}


//@功能:一个图像传感器自动校正完成后的事件响应函数
//@参数:bSuccess, 成功/失败标志
//      bSimulateMode,模拟校正模式
void CIWBSensorManager::OnIWBSensorAutoCalibrateDone(BOOL bSuccess, BOOL bSimulateMode)
{
    if(!bSuccess)//失败了
    {
        if (bSuccess != E_AUTO_CALIBRATOR_ERROR_NOT_FOUND_DEVICE)
        {
            m_nAutoCalibrateTryTimes ++;
            if(!bSimulateMode && m_nAutoCalibrateTryTimes < MAX_AUTOCALIBRATE_TRY_TIMES)
            {
                 //再进行一次校正
                 m_vecSensors[m_nCurrentSensorID]->StartAutoCalibrate(
                      m_eAutoCalibratePattern,
                      m_hNotifyWindow);
                 return;
           }
        }
    }
    if(m_eOperationMode == E_MODE_ALL_SENSOR)
    {//
        m_vecCalibrateResults[m_nCurrentSensorID] = bSuccess;

        while(m_nCurrentSensorID < (int)m_vecSensors.size() - 1)
        {
            m_nCurrentSensorID ++;
            
            if(m_vecSensors[m_nCurrentSensorID]->IsDetecting())
            {
                m_vecSensors[m_nCurrentSensorID]->StartAutoCalibrate(
                    m_eAutoCalibratePattern,
                    m_hNotifyWindow);

                return;//继续下一个传感器的自动校正。
            }
            
        }//while
         
        for(unsigned int i=0; i<m_vecCalibrateResults.size(); i++)
        {
            m_vecSensors[i]->OnAutoCalibrateDone(m_vecCalibrateResults[i]);
        }

    }
    else if(m_eOperationMode == E_MODE_SINGLE_SENSOR)
    {
        m_vecCalibrateResults[0] = bSuccess;
        m_vecSensors[m_nCurrentSensorID]->OnAutoCalibrateDone(m_vecCalibrateResults[0]);

    }

    //更新校正公差分布
    UpdateCalibrateToleranceDistribute();
}


//@功能:开始图像传感器的手动校正流程
//@参数:
//      hNotifWindow, 校正结束后的通知消息"WM_FINISH_MAUNUAL_CALIBRATING"的接收窗体
//      nPtsInRow, 每行校正点个数， -1: 表示从配置文件中读取
//      nPtsInCol, 每列校正点个数， -1: 表示从配置文件中读取
//      nSensorID, -1,全部传感器校正
void  CIWBSensorManager::StartManualCalibrate(HWND hNotifyWindow, int nPtsInRow, int nPtsInCol, int nSensorID, EManualCalibrateType eManualCalibType)
{
    if(nSensorID == -1)
    {
        //初始化校正结果数组
        m_vecCalibrateResults.resize(m_vecSensors.size());

        for(size_t i=0; i<m_vecCalibrateResults.size(); i++)
        {
            m_vecCalibrateResults[i] = FALSE;
        }

        //所有传感器禁用光笔
        for(size_t i=0; i<m_vecSensors.size(); i++)
        {   
            m_vecSensors[i]->EnableOpticalPen(FALSE);
        }


        if(m_vecSensors.size() >= 1)
        {
            m_nCurrentSensorID = 0;
            m_vecSensors[0]->StartManualCalibrate(hNotifyWindow, nPtsInRow, nPtsInCol, eManualCalibType);
            
        }

        m_eOperationMode = E_MODE_ALL_SENSOR;
    }
    else if( 0<= nSensorID && nSensorID < (int)m_vecSensors.size())
    {
        m_vecSensors[nSensorID]->EnableOpticalPen(FALSE);
        m_vecSensors[nSensorID]->StartManualCalibrate(hNotifyWindow, nPtsInRow, nPtsInCol, eManualCalibType);

        m_eOperationMode = E_MODE_SINGLE_SENSOR;
        m_nCurrentSensorID   = nSensorID;
        m_vecCalibrateResults.resize(1);
        m_vecCalibrateResults[0] = FALSE;

    }
    m_hNotifyWindow = hNotifyWindow;
}

//@功能:一个图像传感器手动校正正完成后的事件响应函数
//@参数:bSuccess, 成功/失败标志    
void CIWBSensorManager::OnIWBSensorManualCalibrateDone(BOOL bSuccess, DWORD dwCtxData)
{
    if(m_eOperationMode == E_MODE_ALL_SENSOR)
    {//
        m_vecCalibrateResults[m_nCurrentSensorID] = bSuccess;

        if(m_nCurrentSensorID < (int)m_vecSensors.size() - 1)
        {
            m_nCurrentSensorID ++;

            m_vecSensors[m_nCurrentSensorID]->StartManualCalibrate(
                m_hNotifyWindow,
                dwCtxData & 0xFF,//rows
                (dwCtxData >> 8) & 0x0FF //cols
                );
            return;
        }
    }

    if(m_eOperationMode == E_MODE_SINGLE_SENSOR)
    {
        m_vecCalibrateResults[0] = bSuccess;
        m_vecSensors[m_nCurrentSensorID]->OnManualCalibrateDone(m_vecCalibrateResults[0]);
    }
    else
    {
        for(unsigned int i=0; i<m_vecCalibrateResults.size(); i++)
        {
            m_vecSensors[i]->OnManualCalibrateDone(m_vecCalibrateResults[i]);
        }
    }

    //更新校正公差分布
    UpdateCalibrateToleranceDistribute();
}


//@功能:开启自动屏蔽流程
//@参数:hNotifWindow, 校正结束后的通知消息"WM_SEARCH_SCREEN_AREA_DONE"的接收窗体
//      nSensorID, 传感器ID; -1时,为全部传感器校正。
void CIWBSensorManager::StartSearchMaskArea(HWND hNotifyWindow, int nSensorID)
{
    size_t nSensorCount = m_vecSensors.size();
    if(nSensorID == -1)
    {
        //所有传感器禁用光笔
        for(size_t i=0; i < nSensorCount; i++)
        {   
            m_vecSensors[i]->EnableOpticalPen(FALSE);

            m_vecSensors[i]->SwitchLensMode(E_LASER_TUNING_MODE);
        }

        if(m_vecSensors.size() >= 1)
        {//???如果传感器未工作怎么办?
             for(size_t i = 0; i < nSensorCount; i++)
            {  
                if(m_vecSensors[i]->IsDetecting())
                {//第一个处于正常工作的图像传感器进入自动屏蔽流程
                    m_nCurrentSensorID = i;
                    m_vecSensors[i]->StartAutoMasking(hNotifyWindow);
                    break;
                }
             }
        }

        m_eOperationMode = E_MODE_ALL_SENSOR;
    }
    else if( 0<= nSensorID && nSensorID < (int)m_vecSensors.size())
    {
        if(m_vecSensors[nSensorID]->IsDetecting())
        {
            //自动光斑屏蔽时, 在多屏模式下所有激光器都有打开，否则生成的屏蔽图
            //只屏蔽部分干扰。
            m_vecLastSensorLensMode.resize(nSensorCount);

            for (size_t i = 0; i < nSensorCount; i++)
            {
                m_vecLastSensorLensMode[i] = m_vecSensors[i]->GetCfgData().eLensMode;
                m_vecSensors[i]->SwitchLensMode(E_LASER_TUNING_MODE);
            }

            //
            m_vecSensors[nSensorID]->EnableOpticalPen(FALSE);
            m_vecSensors[nSensorID]->StartAutoMasking(hNotifyWindow);

            m_eOperationMode    = E_MODE_SINGLE_SENSOR;
            m_nCurrentSensorID = nSensorID;
        }
    }
    
    m_hNotifyWindow = hNotifyWindow;
}

//@功能:对"屏蔽区搜索完毕"事件的响应函数
//@参数:bSuccess, 成功/失败标志
void CIWBSensorManager::OnIWBSensorSearchMaskAreaDone(BOOL bSuccess)
{
    m_vecSensors[m_nCurrentSensorID]->OnAutoSearchMaskAreaDone(bSuccess);

    if(m_eOperationMode == E_MODE_ALL_SENSOR)
    {//所有传感器自动屏蔽模式

        if(m_nCurrentSensorID < (int)m_vecSensors.size() - 1)
        {
            m_nCurrentSensorID ++;
            m_vecSensors[m_nCurrentSensorID]->StartAutoMasking(
                m_hNotifyWindow);
            return;
        }
    }
    else
    {
        int nSensorCount = m_vecSensors.size();
        //恢复其它Sensor的状态
        for (size_t nSensorID = 0; nSensorID < nSensorCount; nSensorID++)
        {
            if (nSensorID == m_nCurrentSensorID) continue;
            
            m_vecSensors[nSensorID]->SwitchLensMode(m_vecLastSensorLensMode[nSensorID]);
        }

    }
}



//@功能:判断校正是否成功
BOOL CIWBSensorManager::IsCalibarateOk()
{
    if(E_MODE_ALL_SENSOR == m_eOperationMode)
    {
        for(UINT i=0; i < m_vecCalibrateResults.size(); i++)
        {
            if(m_vecCalibrateResults[i] == FALSE)
            {
                return FALSE;
            }
        }
    }
    else
    {
        return m_vecCalibrateResults[0];
    }

    return TRUE;

}


//@功能:开启光斑采集功能
void CIWBSensorManager::StartLightSpotSampling(HWND hNotifyWindow, int nSensorID)
{
	if(nSensorID > -1)
	{
		m_nCurrentSensorID = nSensorID;
	}
	else
	{
        m_nCurrentSensorID = 0;

		//所有传感器禁用光笔
		for (size_t i = 0; i<m_vecSensors.size(); i++)
		{
			m_vecSensors[i]->EnableOpticalPen(FALSE);
		}
	}
    RECT rcArea;
    m_vecSensors[m_nCurrentSensorID]->GetAttachedScreenArea(rcArea);

	TCHAR szProfileFullPath[MAX_PATH];
	m_vecSensors[m_nCurrentSensorID]->GetCollectSpotShowPath(szProfileFullPath,_countof(szProfileFullPath));

    m_wndLightSpotSampling.StartCollectSpotSize(&rcArea, 1, hNotifyWindow, 3,3,nSensorID, szProfileFullPath, m_vecSensors.size());

    //传感器进入光斑采样状态
    m_vecSensors[m_nCurrentSensorID]->StartLightSpotSampling(m_wndLightSpotSampling.m_hWnd);


    m_hNotifyWindow = hNotifyWindow;
}

//@功能:光斑采集结束事件的响应函数
//@参数:bSuccess, 成功失败标志
void CIWBSensorManager::OnIWBSensorLightSpotSamplingDone(BOOL bSuccess, int nSensorId)
{

    const ALL_LIGHTSPOT_SAMPLE_SIZE&  screenSamples = m_wndLightSpotSampling.GetScreenSamples();
    m_vecSensors[m_nCurrentSensorID]->OnLightSpotSamplingDone(screenSamples, bSuccess);
	if (nSensorId >-1)
	{
		//说明是单屏采集。
		return;
	}

    m_nCurrentSensorID  ++;
	if (m_nCurrentSensorID == m_vecSensors.size())
	{
		//所有传感器禁用光笔
		for (size_t i = 0; i<m_vecSensors.size(); i++)
		{
			m_vecSensors[i]->EnableOpticalPen(TRUE);
		}
		return;
	}
    RECT rcArea;
    m_vecSensors[m_nCurrentSensorID]->GetAttachedScreenArea(rcArea);
	TCHAR szProfileFullPath[MAX_PATH];
	m_vecSensors[m_nCurrentSensorID]->GetCollectSpotShowPath(szProfileFullPath, _countof(szProfileFullPath));

    m_wndLightSpotSampling.StartCollectSpotSize(&rcArea, 1, m_hNotifyWindow, 3, 3, nSensorId, szProfileFullPath, m_vecSensors.size());

    //传感器进入光斑采样状态
    m_vecSensors[m_nCurrentSensorID]->StartLightSpotSampling(m_wndLightSpotSampling.m_hWnd);

}


//@功能:判断所有的采样已经完成
BOOL CIWBSensorManager::AllSamplingIsDone()
{
    return TRUE;
}



//@功能:判断传感器设备是否在运行
//@参数;无
//@说明:只要有一个传感器在运行，就认为处于运行状态
BOOL CIWBSensorManager::IsRunning()const
{
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {   
        if(m_vecSensors[i]->IsDetecting())
        {
            return TRUE;
        }
    }//for

    return FALSE;
}

//@功能:判断所有传感器设备停止运行
BOOL CIWBSensorManager::IsAllSensorStopped()const
{
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {   
        if(m_vecSensors[i]->IsDetecting())
        {
            return FALSE;
        }
    }//for

    return TRUE;
}


//调试工具相关函数
//@功能:判断是否正在录像
BOOL CIWBSensorManager::IsRecording() const
{
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {   

        //只要有一个传感器在录像即认为处于录像状态。
        if(m_vecSensors[i]->GetInterceptFilter()->IsRecording())
        {
            return TRUE;
        }

    }//for

    return FALSE;
}

//调试工具函数
//@功能:开始录像
BOOL CIWBSensorManager::StartRecording()
{
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {   
            m_vecSensors[i]->GetInterceptFilter()->StartRecording();
    }//for
    return TRUE;
}


//调试工具函数
//@功能:停止录像
BOOL CIWBSensorManager::StopRecording()
{
    for(size_t i=0; i<m_vecSensors.size(); i++)
    {   
        m_vecSensors[i]->GetInterceptFilter()->StopRecording();

    }//for
    
    return TRUE;
}

//调试工具函数
//@功能:设置视频数据为AVI录像文件
//@参数:lpszVideoPath1,传感器1的路径
//      lpszVideoPath2,传感器2的路径
void CIWBSensorManager::LoadSrcFromAVI(LPCTSTR lpszVideoPath1, LPCTSTR lpszVideoPath2)
{
	int nSensorID = 0;
    if(lpszVideoPath1 && _tcslen(lpszVideoPath1))
    {
		CString strpath1 = lpszVideoPath1;
		int pos = strpath1.ReverseFind(_T('Sensor'));
		if (pos > -1)
		{
			strpath1 = strpath1.Mid(pos + _tcslen(_T("Sensor")), 1);
		    nSensorID = _ttoi(strpath1);
		}

        if(m_vecSensors[nSensorID] && m_vecSensors[nSensorID]->IsDetecting())
        {
            m_vecSensors[nSensorID]->GetInterceptFilter()->InputFromAVIFile(lpszVideoPath1);
        }
    }

    if (m_vecSensors.size() > 1)//2019/10/15
    {
	    nSensorID = 1;
        if (lpszVideoPath2 && _tcslen(lpszVideoPath2))
        {
			CString strpath2 = lpszVideoPath1;
			int pos = strpath2.ReverseFind(_T('Sensor'));
			if (pos > -1)
			{
				strpath2 = strpath2.Mid(pos + _tcslen(_T("Sensor")), 1);
			    nSensorID = _ttoi(strpath2);
			}
            if (m_vecSensors[nSensorID] && m_vecSensors[nSensorID]->IsDetecting())
            {
                m_vecSensors[nSensorID]->GetInterceptFilter()->InputFromAVIFile(lpszVideoPath2);
            }
        }
    }
}

////@功能:交换两个图像传感器的显示画面
//void CIWBSensorManager::SwapSensorImage()
//{
//    //传感器数目少于2,则不作任何操作。
//    if(m_vecSensors.size() < 2) return;
//
//    TSensorConfig  cfgForSensor0 = this->m_vecSensors[0]->GetCfgData();
//    TSensorConfig  cfgForSensor1 = this->m_vecSensors[1]->GetCfgData();
//    
//    //两个传感器交换配置数据
//    //说明:关键这里通过交换配置数据，使得两个传感器对象的设备路径发生交换, 
//    this->m_vecSensors[0]->SetCfgData(cfgForSensor1);
//    this->m_vecSensors[1]->SetCfgData(cfgForSensor0);
//    //停止
//    StopRunning();
//    //重新运行
//    StartRunning();
//}


//@功能:交换两个图像传感器的显示画面
//@参数:第一个图像传感器的Id
//      第二个图像传感器的id
void CIWBSensorManager::SwapSensorImage(UINT nFirstSensorId, UINT nSecondSensorId)
{
    LOG_INF("CIWBSensorManager::SwapSensorImage(%d,%d).\r\n", nFirstSensorId, nSecondSensorId);
    if (nFirstSensorId < 0  || nFirstSensorId  >= m_vecSensors.size()) return;
    if (nSecondSensorId < 0 || nSecondSensorId >= m_vecSensors.size()) return;


    TSensorConfig  cfgForSensorFirst  = this->m_vecSensors[nFirstSensorId ]->GetCfgData();
    TSensorConfig  cfgForSensorSecond = this->m_vecSensors[nSecondSensorId]->GetCfgData();
	
	TCaptureDeviceInstance devInstFirst = this->m_vecSensors[nFirstSensorId]->GetDeviceInfo();
	TCaptureDeviceInstance devInstSecond = this->m_vecSensors[nSecondSensorId]->GetDeviceInfo();

    //两个传感器交换配置数据
    //说明:关键这里通过交换配置数据，使得两个传感器对象的设备路径发生交换, 
    this->m_vecSensors[nFirstSensorId ]->SetCfgData (cfgForSensorSecond);
    this->m_vecSensors[nSecondSensorId]->SetCfgData (cfgForSensorFirst );

	
	this->m_vecSensors[nFirstSensorId ]->SetDeviceInfo(devInstSecond);
	this->m_vecSensors[nSecondSensorId]->SetDeviceInfo(devInstFirst);


	//获取所有图像传感器的配置信息
	GetCfgData(g_tSysCfgData);

    //停止
    StopRunning();

    //重新运行
    StartRunning();
}




//@功能:通过读取录像文件再现自动校正过程
//@参数:nSensorId, 传感器Id
//      hNotifyWnd, 校正完毕后的消息通知窗体
//      lpszAviFilePath, 录像文件
//      cBrightness, 校正时的画面平均亮度
//      eGridNumber, 校正棋盘格列数
BOOL CIWBSensorManager::DoSimulateAutoCalibrate(int nSensorId, HWND hNotifyWnd, LPCTSTR lpszAviFilePath, SIZE szScreen, BYTE cBrightness, E_AutoCalibratePattern eGridsNumber)
{
    if(nSensorId >= int(m_vecSensors.size())) return FALSE;

     m_eOperationMode      = E_MODE_SINGLE_SENSOR;
     m_nCurrentSensorID   = nSensorId;

    m_vecCalibrateResults.resize(1);
    m_vecCalibrateResults[0] = FALSE;

    m_vecSensors[nSensorId]->GetAutoCalibrator()->DoSimulateCalibrate(
        lpszAviFilePath,
        hNotifyWnd,
        szScreen.cx,
        szScreen.cy,
        cBrightness,
        eGridsNumber,
        E_CALIB_DEBUG_LEVEL_VERBOSE);

    return TRUE;
}

void CIWBSensorManager::ApplyScreenLayout()
{
    UINT nScreenAreaCount = 0;
    const RECT* pScreenAreas = m_oScreenLayoutDesigner.GetScreenAbsoluteLayouts(&nScreenAreaCount);


    for (UINT i = 0; i < m_vecSensors.size() && i < nScreenAreaCount; i++)
    {
        m_vecSensors[i]->SetAttachedScreenArea(*pScreenAreas);
        pScreenAreas++;
    }

    UINT nMergeAreaCount;
    const RECT* pMergeAreas = m_oScreenLayoutDesigner.GetAbsoluteMergeAreas(&nMergeAreaCount);
    if (pMergeAreas && nMergeAreaCount)
    {
        //this->m_oSpotListProcessor.GetSpotMerger().SetMergeAreas(pMergeAreas, nMergeAreaCount);
        this->m_pSpotListProcessor->GetSpotMerger().SetMergeAreas(pMergeAreas, nMergeAreaCount);
    }

}

//@功能:屏幕分辨率变化事件响应函数
//@参数:nScreenWidth, 新的屏幕宽度
//      nScreenHeight,新的屏幕高度
void CIWBSensorManager::OnDisplayChange(int nScreenWidth, int nScreenHeight)
{
    //for(size_t i=0; i< m_vecSensors.size(); i++)
    //{
    //    //const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(i);
    //    //if(pDisplayDevInfo)
    //    //{
    //    //    m_vecSensors[i]->OnMonitorResolutionChange(pDisplayDevInfo->rcMonitor);
    //    //}
    //    m_vecSensors[i]->OnMonitorResolutionChange();
    //}

    //this->m_oSpotListProcessor.OnDisplayChange(nScreenWidth, nScreenHeight);
    this->m_pSpotListProcessor->OnDisplayChange(nScreenWidth, nScreenHeight);

    //通知光斑采样模块, 屏幕分辨率发生了变化
    this->m_wndLightSpotSampling.OnDisplayChange(nScreenWidth, nScreenHeight);

    //更新手势识别的屏幕分辨率和物理尺寸
    m_execContext.OnPhysicalLengthChange(g_tSysCfgData.globalSettings.fScreenDiagonalPhysicalLength);

    m_execContext.OnDisplayChange(nScreenWidth, nScreenHeight);

    //重构屏幕布局
    m_oScreenLayoutDesigner.OnDisplayChange(nScreenWidth, nScreenHeight);

    ApplyScreenLayout();
}

/*
@功能：开启智能检测挡板是否消失
*/
void CIWBSensorManager::StartDetectBackSplashVanished()
{
    for(size_t i=0; i< m_vecSensors.size(); i++)
    {
        const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(i);
        if(pDisplayDevInfo)
        {
            m_vecSensors[i]->OnStartDetectBackSplashVanished();
        }
    }
}


/*
@功能：停止智能检测挡板是否消失
*/
void CIWBSensorManager::StopDetectBackSplashVanished()
{
    for(size_t i=0; i< m_vecSensors.size(); i++)
    {
        const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(i);
        if(pDisplayDevInfo)
        {
            m_vecSensors[i]->OnStopDetectBackSplashVanished();
        }
    }
}


//@功能:查看校正符号位置
//@参数:bShow,TRUE,显示符号
//            FALSE, 隐藏符号
//@Author:15077727@qq.com, 2015/09/09
BOOL CIWBSensorManager::CheckCalibrateSymbols(BOOL bShow)
{
    theApp.GetMonitorFinder().SearchDisplayDev();
    BOOL bRet = FALSE;
    for(size_t i=0; i< m_vecSensors.size(); i++)
    {
        bRet |= m_vecSensors[i]->CheckCalibrateSymbolPos(bShow);
    }

    return bRet;
}


//判断是否校正符号是否已经显示
BOOL CIWBSensorManager::IsCalibrateSymbolVisible()const
{
    for(size_t i=0; i< m_vecSensors.size(); i++)
    {
        if(m_vecSensors[i]->IsCalibrateSymbolVisible()) return TRUE;
    }
    
    return FALSE;

}


BOOL CIWBSensorManager::CalibrateSymbolManualResample()
{
    if(m_vecSensors.size() == 0) return FALSE;

    m_vecSensors[0]->CalibrateSymbolManualResample();

    return TRUE;
}


//Added by 15077726@qq.com
//2015/11/9
//功能:判断是否已经完成校正
BOOL CIWBSensorManager::IsCalibrated()const
{
     if(m_vecSensors.size() == 0) return FALSE;
     for(size_t i=0; i < m_vecSensors.size(); i++)
     {
         if(!m_vecSensors[i]->IsCalibrated()) return FALSE;
     }

     return TRUE;
}

//功能:判断是否有校正成功的相机存在
BOOL CIWBSensorManager::HasCalibratedCamera()const
{
    for (size_t i = 0; i < m_vecSensors.size(); i++)
    {
        if (m_vecSensors[i]->IsCalibrated()) return TRUE;
    }

    return FALSE;
}


void CIWBSensorManager::OnTimer(LPVOID lpCtx)
{
    for (size_t i = 0; i < m_vecSensors.size(); i++)
    {
        if (m_vecSensors[i]->IsDetecting() && E_NORMAL_USAGE_MODE == m_vecSensors[i]->GetLensMode())
        {
            m_vecSensors[i]->OnTimer(lpCtx);

        }//if

    }//for-each(i)
}

//@功能:开始四点标定
//@参数:hNotifyWindow, 消息接收窗体
//      nSensorID, -1, 全部传感器标定
void  CIWBSensorManager::Start4BasePointMarking(HWND hNotifyWindow, int nSensorID)
{
    if (nSensorID == -1)
    {
        m_vecCalibrateResults.resize(m_vecSensors.size());

        for (size_t i = 0; i < m_vecCalibrateResults.size(); i++)
        {
            m_vecCalibrateResults[i] = FALSE;
        }

        //所有传感器禁用光笔
        for (size_t i = 0; i < m_vecSensors.size(); i++)
        {
            m_vecSensors[i]->EnableOpticalPen(FALSE);
        }

        if (m_vecSensors.size() >= 1)
        {
            m_nCurrentSensorID = 0;
            m_vecSensors[0]->Start4BasePointMarking(hNotifyWindow);
        }

        m_eOperationMode = E_MODE_ALL_SENSOR;
    }
    else if (0 <= nSensorID && nSensorID < (int)m_vecSensors.size())
    {
        m_vecSensors[nSensorID]->EnableOpticalPen(FALSE);
        m_vecSensors[nSensorID]->Start4BasePointMarking(hNotifyWindow);
        
        m_eOperationMode = E_MODE_SINGLE_SENSOR;
        m_nCurrentSensorID = nSensorID;
        m_vecCalibrateResults.resize(1);
        m_vecCalibrateResults[0] = FALSE;
   }

}

//@功能:4点标定结束响应函数
//@参数:bSuccess, TRUE,成功
void CIWBSensorManager::OnIWBSensor4BasePointMarkingDone(BOOL bSuccess)
{
    if (m_eOperationMode == E_MODE_ALL_SENSOR)
    {//
        m_vecCalibrateResults[m_nCurrentSensorID] = bSuccess;

        if (m_nCurrentSensorID < (int)m_vecSensors.size() - 1)
        {//继续下一屏幕的4点标定
            m_nCurrentSensorID ++;
            m_vecSensors[m_nCurrentSensorID]->Start4BasePointMarking(m_hNotifyWindow);
            return;
        }
    }

    if (m_eOperationMode == E_MODE_SINGLE_SENSOR)
    {
        m_vecCalibrateResults[0] = bSuccess;
        m_vecSensors[m_nCurrentSensorID]->On4BasePointMarkingDone(bSuccess);
    }
    else
    {
        for (unsigned int i = 0; i<m_vecCalibrateResults.size(); i++)
        {
            m_vecSensors[i]->On4BasePointMarkingDone(m_vecCalibrateResults[i]);
        }
    }

}
/////////////////////
void CIWBSensorManager::EnableOnlineScreenArea(BOOL bEnable)
{
	for (size_t i = 0; i < m_vecSensors.size(); i++)
	{
		m_vecSensors[i]->SetOnlineScreenArea(bEnable?true:false);
	}
}

BOOL CIWBSensorManager::IsEnableOnlineScreenArea()
{
	BOOL bIsControlling = TRUE;
	for (size_t i = 0; i < m_vecSensors.size(); i++)
	{
		//判断运行中的，不运行就不计算了
		if (m_vecSensors[i]->IsDetecting()) 
		{
		    bIsControlling &= m_vecSensors[i]->IsEnableOnlineScreenArea();
		}
	}
	return bIsControlling;
}

//@功能:更新校正公差分布。
void CIWBSensorManager::UpdateCalibrateToleranceDistribute()
{
    const UINT nRows = 10;
    const UINT nCols = 10;
    const UINT nPtCount = nRows*nCols;
#if 0
    int nCx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int nCy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
#else
    int nCx = GetSystemMetrics(SM_CXSCREEN);
    int nCy = GetSystemMetrics(SM_CYSCREEN);
#endif

    std::vector<TPoint2D> vecPtsOnScreen;
    vecPtsOnScreen.resize(nPtCount);
    
    //Y坐标采样点
    std::vector<double> vecY;
    vecY.resize(nRows);
   
    int interval  = nCy / nRows;
    int remainder = nCy % nRows;
    int y0 = 0;
    int remainderCount = 0;
    for (UINT r = 0; r < nRows; r++)
    {
        int y1 = y0 + interval;
        
        remainderCount += remainder;

        if (remainderCount > nRows)
        {
            remainderCount -= nRows;
            y1 ++;
        }

        vecY[r] = double(y0 + y1) / 2.0;

        y0 = y1;
    }
    
    //X坐标采样点
    std::vector<double> vecX;
    vecX.resize(nCols);
    interval  = nCx / nCols;
    remainder = nCx % nCols;
    remainderCount = 0;
    int x0 = 0;
    for (UINT c = 0; c < nCols; c++)
    {
        int x1 = x0 + interval;

        remainderCount += remainder;
        if (remainderCount > nCols)
        {
            x1 ++;
            remainderCount -= nCols;
        }

        vecX[c] = double(x0 + x1) / 2.0;

        x0 = x1;
    }


    for (UINT r = 0; r < nRows; r++)
    {
        TPoint2D pt;
        pt.d[1] = vecY[r];

        for (UINT c = 0; c < nCols; c++)
        {            
            pt.d[0] = vecX[c];
            vecPtsOnScreen[r*nCols + c] = pt;
        }
    }

    std::vector<double> vecTolX;//X坐标方向的定位公差
    std::vector<double> vecTolY;//Y坐标方向的定位公差
    vecTolX.resize(nPtCount);
    vecTolY.resize(nPtCount);

    for (size_t i = 0; i < nPtCount; i++)
    {
        vecTolX[i] = 0.0;
        vecTolY[i] = 0.0;
    }

    //计算每个采样点处的校正公差
    for (UINT i = 0; i < vecPtsOnScreen.size(); i++)
    {
        TPoint3D  pt3DWorld;
        pt3DWorld.d[0] = vecPtsOnScreen[i].d[0];
        pt3DWorld.d[1] = vecPtsOnScreen[i].d[1];
        pt3DWorld.d[2] = 0;

        for (UINT j = 0; j < m_vecSensors.size(); j++)
        {
            CIWBSensor* pSensor = m_vecSensors[j];
            if (pSensor == NULL)
            {
                continue;
            }
            RECT rcMonitorArea = {0, 0, 0, 0};
            pSensor->GetAttachedScreenArea(rcMonitorArea);

            POINT pt;
            pt.x = pt3DWorld.d[0];
            pt.y = pt3DWorld.d[1];

            if (!::PtInRect(&rcMonitorArea, pt))
            {
                continue;
            }

            //屏幕上的点位于传感器的管辖范围内,由该传感器负责计算
            CalibrateAlgo&  calibrateAlgo = pSensor->GetPenPosDetector()->GetVideoToScreenMap().GetCalibAlog();

            TPoint2D pt2DImage;
            calibrateAlgo.MapWorldToImage(&pt3DWorld, 1, &pt2DImage, 0);

            
            TPoint2D pt2DScreen;
            calibrateAlgo.GetScreenCoord(pt2DImage, &pt2DScreen,0, TRUE/*bWithoutAutoCalibCompensate*/);

            TPoint2D pt2DImageDeviate;

            //
            double deviation = 1.0;

            //
            //相机图像坐标在水平和垂直方向分别偏移0.5个像素
            pt2DImageDeviate.d[0] = pt2DImage.d[0] + deviation;
            pt2DImageDeviate.d[1] = pt2DImage.d[1] + deviation;


            TPoint2D pt2DScreenDeviate;
            calibrateAlgo.GetScreenCoord(pt2DImageDeviate, &pt2DScreenDeviate,0, TRUE/*bWithoutAutoCalibCompensate*/);

            vecTolX[i] = fabs(pt2DScreenDeviate.d[0] - pt2DScreen.d[0]);
            vecTolY[i] = fabs(pt2DScreenDeviate.d[1] - pt2DScreen.d[1]);
            break;
        }

    }

    CToleranceDistribute& toleranceDistribute = m_pSpotListProcessor->GetToleranceDistribute();

    toleranceDistribute.UpdateToleranceDistribute(nRows, nCols, &vecTolX[0], &vecTolY[0]);

}