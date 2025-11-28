#include "stdafx.h"

#include "../inc/Log.h"
#include "..\MorphologyAlgo\inc\MorphologyAlgo.h"
#include "../inc/FillPolygon.h"
#include <locale>
//#pragma comment(lib, "..\lib\MorphologyAlgo.lib")
//#include "headers.h"

//静态对象
//CAutoCalibrator CIWBSensor::m_oAutoCalibrator;
//CDispMonitorFinder CIWBSensor::m_oDispMonitorFinder;


//光斑采集对象
//CCollectSpotSize   CIWBSensor::m_wndLightSpotSampling;

//@功能:静态屏蔽开始回调处理函数。
//@参数:lpCtx, 回调的上下文数据。    
BOOL  CALLBACK CIWBSensor::OnPreStaticMasking(LPVOID lpCtx)
{
    CIWBSensor* lpSensor = reinterpret_cast<CIWBSensor*>(lpCtx);

    //合上滤光片。
    IRCUTSwtich(lpSensor->m_oVideoPlayer.GetCaptureFilter(), FALSE, lpSensor->m_tDeviceInfo.m_nPID, lpSensor->m_tDeviceInfo.m_nVID);

    //载入正常使用时的相机参数
    EDeviceTouchType eTouchType;
    TSensorModeConfig* TSensorModeConfig = NULL;

    EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;
    TSensorModeConfig = &lpSensor->m_tCfgData.vecSensorModeConfig[eProjectionMode];

    eTouchType = TSensorModeConfig->advanceSettings.m_eTouchType;
    TLensConfig lensCfg = TSensorModeConfig->lensConfigs[lpSensor->m_eCameraType][lpSensor->m_tCfgData.eSelectedLensType];
    switch (eTouchType)
    {
    case E_DEVICE_PEN_TOUCH_WHITEBOARD:

        lpSensor->m_oVideoPlayer.SetCameraParams(lensCfg.normalUsageSettings_PenTouchWhiteBoard.cameraParams);
        break;
    case E_DEVICE_FINGER_TOUCH_WHITEBOARD:
        lpSensor->m_oVideoPlayer.SetCameraParams(lensCfg.normalUsageSettings_FingerTouchWhiteBoard.cameraParams);
        break;
    case E_DEVICE_FINGER_TOUCH_CONTROL:
        lpSensor->m_oVideoPlayer.SetCameraParams(lensCfg.normalUsageSettings_FingerTouchControl.cameraParams);
        break;
    case E_DEVICE_PALM_TOUCH_CONTROL:
        lpSensor->m_oVideoPlayer.SetCameraParams(lensCfg.normalUsageSettings_PalmTouchControl.cameraParams);
        break;
    default:
        break;

    }

    return TRUE;
}


//静态函数
//自动校正过程中变更摄像头参数的回调函数
BOOL  CIWBSensor::OnAutoCalibChangeCameraParams(EChangeCalibCameraParams eCtrlMode, LPVOID lpCtx, BYTE param1, BYTE param2)
{

    CIWBSensor* lpThis = reinterpret_cast<CIWBSensor*>(lpCtx);

    TSensorModeConfig* pSensorModeConfig = NULL;
    EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;

    pSensorModeConfig = &lpThis->m_tCfgData.vecSensorModeConfig[eProjectionMode];

    const TLensConfig& lensCfg = pSensorModeConfig->lensConfigs[lpThis->m_eCameraType][lpThis->m_tCfgData.eSelectedLensType];

    TVideoProcAmpProperty cameraParams;

    switch (eCtrlMode)
    {
    case E_CAMERA_SHARPNESS_MAX://设置最大锐利度
        lpThis->m_oVideoPlayer.GetCameraParams(cameraParams);

        IRCUTSwtich(lpThis->m_oVideoPlayer.GetCaptureFilter(), FALSE, lpThis->m_tDeviceInfo.m_nPID, lpThis->m_tDeviceInfo.m_nVID);

        //cameraParams.Prop_VideoProcMap_Gain  = IRCUT_OFF;//保持滤光片打开
        return lpThis->m_oVideoPlayer.SetCameraParams(cameraParams);

        break;


    case E_CAMERA_SHARPNESS_MIN://显示校正图案阶段
        //PROFILE::CameraParametersForAutoCalibration(cameraParams);

        cameraParams = lensCfg.autoCalibrateSettingsList[param2].cameraParams;

        IRCUTSwtich(lpThis->m_oVideoPlayer.GetCaptureFilter(), FALSE, lpThis->m_tDeviceInfo.m_nPID, lpThis->m_tDeviceInfo.m_nVID);

        return lpThis->m_oVideoPlayer.SetCameraParams(cameraParams);
        break;

    case E_CAMERA_BRIGHTNESS_INCREASE://增加亮度
    {
        LONG lBrightness, lMax, lMin, lSteppingDelta, lDefault;
        BYTE absDiff = param1;

        //按照亮度差的绝对值大小,按彼列调节
        BYTE K = 2;//比例调节系数Kp = 1/K
        if (!lpThis->m_oVideoPlayer.GetCameraBrightness(lBrightness, lMax, lMin, lSteppingDelta, lDefault))
        {
            return FALSE;
        }
        if (lSteppingDelta < absDiff / K)
        {
            lSteppingDelta = absDiff / K;
        }

        if (lBrightness > lMax - lSteppingDelta)
        {
            lBrightness = lMax;
        }
        else
        {
            lBrightness += lSteppingDelta;
        }

        //lpThis->m_oFilterGraphBuilder.GetCameraParams(cameraParams);
        /*
        lpThis->m_oVideoPlayer.GetCameraParams(cameraParams);
        cameraParams.Prop_VideoProcAmp_Brightness = lBrightness;
        cameraParams.Prop_VideoProcMap_Gain       = IRCUT_OFF  ;//保持滤光片打开
        */

        //AtlTrace(_T("[Brightness]delta =%d, coef=%d, increase\n"), lSteppingDelta, lBrightness);
        return lpThis->m_oVideoPlayer.SetCameraParams(lBrightness, VideoProcAmp_Brightness);
    }


    break;

    case E_CAMERA_BRIGHTNESS_DECREASE://减少亮度
    {
        LONG lBrightness, lMax, lMin, lSteppingDelta, lDefault;
        BYTE absDiff = param1;

        //按照亮度差的绝对值大小,按彼列调节
        BYTE K = 2;//比例调节系数Kp = 1/K
        if (!lpThis->m_oVideoPlayer.GetCameraBrightness(lBrightness, lMax, lMin, lSteppingDelta, lDefault))
        {
            return FALSE;
        }


        if (lSteppingDelta < absDiff / K)
        {
            lSteppingDelta = absDiff / K;
        }

        if (lBrightness < lSteppingDelta)
        {
            lBrightness = lMin;
        }
        else
        {
            lBrightness -= lSteppingDelta;
        }


        //AtlTrace(_T("[Brightness]delta =%d, coef=%d,decrease\n"), lSteppingDelta, lBrightness);

        /*
        lpThis->m_oVideoPlayer.GetCameraParams(cameraParams);
        cameraParams.Prop_VideoProcAmp_Brightness = lBrightness;
        cameraParams.Prop_VideoProcMap_Gain       = IRCUT_OFF  ;//保持滤光片打开
        */

        return lpThis->m_oVideoPlayer.SetCameraParams(lBrightness, VideoProcAmp_Brightness);
    }
    break;


    case E_CAMERA_CONTRAST_MAX:
    {
        if (!lpThis->m_oVideoPlayer.GetCameraParams(cameraParams)) return FALSE;

        LONG lValue, lMax, lMin, lSteppingDelta, lDefault;
        lMax = 255;
        lpThis->m_oVideoPlayer.GetCameraContrast(lValue, lMax, lMin, lSteppingDelta, lDefault);
        cameraParams.Prop_VideoProcAmp_Contrast = lMax;
        return lpThis->m_oVideoPlayer.SetCameraParams(cameraParams);
    }
    break;


    case E_CAMERA_CONTRAST_DEFAULT:
    {
        if (!lpThis->m_oVideoPlayer.GetCameraParams(cameraParams))return FALSE;

        LONG lValue, lMax, lMin, lSteppingDelta, lDefault;
        if (!lpThis->m_oVideoPlayer.GetCameraContrast(lValue, lMax, lMin, lSteppingDelta, lDefault))
        {
            return FALSE;
        }
        cameraParams.Prop_VideoProcAmp_Contrast = lDefault;
        return lpThis->m_oVideoPlayer.SetCameraParams(cameraParams);
    }

    case E_CAMERA_CONTRAST:
    {
        if (!lpThis->m_oVideoPlayer.GetCameraParams(cameraParams)) return FALSE;
        if (lpThis->m_tDeviceInfo.m_nPID == SONIX_PID && lpThis->m_tDeviceInfo.m_nVID == SONIX_VID)
        {
            cameraParams.Prop_VideoProcAmp_Contrast = 40;
        }
        else
        {
            cameraParams.Prop_VideoProcAmp_Contrast = param1;
        }

        return lpThis->m_oVideoPlayer.SetCameraParams(cameraParams);
    }


    case E_CAMERA_AUTO_CALIBRATE:

        cameraParams = lensCfg.autoCalibrateSettingsList[param2].cameraParams;

        IRCUTSwtich(lpThis->m_oVideoPlayer.GetCaptureFilter(), FALSE, lpThis->m_tDeviceInfo.m_nPID, lpThis->m_tDeviceInfo.m_nVID);

        //        cameraParams.Prop_VideoProcMap_Gain     = IRCUT_OFF  ;//保持滤光片打开
        return lpThis->m_oVideoPlayer.SetCameraParams(cameraParams);
        break;

    case E_CAMERA_AUTO_MASK:

        EDeviceTouchType eTouchType;
        eTouchType = pSensorModeConfig->advanceSettings.m_eTouchType;
        switch (eTouchType)
        {
        case E_DEVICE_PEN_TOUCH_WHITEBOARD:

            cameraParams = lensCfg.normalUsageSettings_PenTouchWhiteBoard.cameraParams;
            break;
        case E_DEVICE_FINGER_TOUCH_WHITEBOARD:

            cameraParams = lensCfg.normalUsageSettings_FingerTouchWhiteBoard.cameraParams;
            break;
        case E_DEVICE_FINGER_TOUCH_CONTROL:
            cameraParams = lensCfg.normalUsageSettings_FingerTouchControl.cameraParams;
            break;
        case E_DEVICE_PALM_TOUCH_CONTROL:
            cameraParams = lensCfg.normalUsageSettings_PalmTouchControl.cameraParams;
            break;
        default:
            break;
        }

        //        if(eTouchType == E_DEVICE_PEN_TOUCH_WHITEBOARD)
        //        {
        //            cameraParams = lensCfg.normalUsageSettings_PenTouchWhiteBoard.cameraParams;
        //        }
        //        else
        //        {
        //            cameraParams = lensCfg.normalUsageSettings_FingerTouchWhiteBoard.cameraParams;
        //        }

                //合上滤光片 
        IRCUTSwtich(lpThis->m_oVideoPlayer.GetCaptureFilter(), TRUE, lpThis->m_tDeviceInfo.m_nPID, lpThis->m_tDeviceInfo.m_nVID);

        //        cameraParams.Prop_VideoProcMap_Gain     = IRCUT_ON ;//拉上滤光片
        return lpThis->m_oVideoPlayer.SetCameraParams(cameraParams);

        break;

    }//switch

    return TRUE;
}


CIWBSensor::CIWBSensor(int nID)
    :
    m_tFavoriteMediaType(DEFAULT_VIDEO_FORMAT),
    m_nID(nID),
    m_oPenPosDetector(nID),
    m_oVideoPlayer(nID),
    m_oTimerActionExecuter(*this),//,
    m_eCameraType(E_CAMERA_MODEL_0)
    //m_hVideoDispWnd(NULL),
    //m_hNotifyWnd(NULL)
{
    //memset(&m_rcVideoDispArea, 0, sizeof(RECT));

    //m_pInterceptFilter = new CInterceptFilter(&this->m_oPenPosDetector, &m_oVideoPlayer);
    m_pInterceptFilter = new CInterceptFilter(this);
    m_pInterceptFilter->AddRef();

    this->m_oVideoPlayer.SetInterceptFilter(m_pInterceptFilter);

    m_tDeviceInfo.m_nPID = 0x9186;
    m_tDeviceInfo.m_nVID = 0x18ec;
    m_tDeviceInfo.m_strDevPath = "";
    m_tDeviceInfo.m_strName = "";

    m_rcMonintorArea.left = 0;
    m_rcMonintorArea.top = 0;
    m_rcMonintorArea.right = ::GetSystemMetrics(SM_CXSCREEN);
    m_rcMonintorArea.bottom = ::GetSystemMetrics(SM_CYSCREEN);


    this->m_tCfgData.nScreenAreaNo = nID;

}


CIWBSensor::~CIWBSensor()
{
    m_oVideoPlayer.StopDetect();
    this->m_oVideoPlayer.SetInterceptFilter(NULL);
    m_pInterceptFilter->Release();
}

void CIWBSensor::SetFavoriateMediaType(const VideoMediaType& vmt)
{
    m_tFavoriteMediaType = vmt;
}


//@功能:设置视频显显示相关信息
//@参数:hDispWnd, 视频播放窗体的句柄
//      rcDispArea, 视频绘制的区域  
//      hNotifyWnd, 接收消息接收窗体句柄
void CIWBSensor::SetVideoDispParams(HWND hDispWnd, RECT& rcDispArea, HWND hNotifyWnd)
{
    //m_hVideoDispWnd   = hDispWnd;
    //m_rcVideoDispArea = rcDispArea;
    //m_hNotifyWnd      = hNotifyWnd;

    m_oVideoPlayer.SetDisplayWnd(hDispWnd);
    m_oVideoPlayer.SetDisplayArea(rcDispArea);
    m_oVideoPlayer.SetNotifyWnd(hNotifyWnd);
}


//@功能:运行传感器
//@参数:
BOOL CIWBSensor::Run()
{
    LOG_FUNC_CALL;
    //<<added by toxuke@gmail.com,2014/12/14
    //如果设备路径为空则不播放。
    if (m_tDeviceInfo.m_strDevPath.IsEmpty())
    {
        ShowMissStatusInfo();
        return FALSE;
    }
    //>>

    BOOL bRet = m_oVideoPlayer.StartDetect(m_tDeviceInfo.m_strDevPath, &m_tFavoriteMediaType.videoInfoHeader);

    //<<added by toxuke@gmail.com, 2014/12/12    
    //说明:为何延迟500ms?
    //测试发现，不延迟500ms，则SwitchLensMode函数中设置相机参数虽然返回TRUE但实际并未起作用。
    //延时500ms
    Sleep(500);
    //end>


    //来回推拉IRCut一次确保IRCut正常工作
    //拉开滤光片禁用光笔控制，避免光斑乱跳
    BOOL bIsPenControlling = this->m_oPenPosDetector.IsOpticalPenControlling();

    if (bIsPenControlling)
    {//
        m_oPenPosDetector.EnableOpticalPenControl(FALSE);//
    }

    //拉开虑光片
    IRCUTSwtich(m_oVideoPlayer.GetCaptureFilter(), FALSE, m_tDeviceInfo.m_nPID, m_tDeviceInfo.m_nVID);

    //等待300ms
    //1280*720sleep(300)的时候会出现设置参数卡死的情况出现
    Sleep(300);

    //合上滤光片 
    IRCUTSwtich(m_oVideoPlayer.GetCaptureFilter(), TRUE, m_tDeviceInfo.m_nPID, m_tDeviceInfo.m_nVID);

    //等待300ms
    Sleep(300);

    //使能光笔。
    m_oPenPosDetector.EnableOpticalPenControl(bIsPenControlling);

    RECT rcMonitor;
    BOOL bGet = this->GetAttachedScreenArea(rcMonitor);
    if (bGet)
    {
        m_oPenPosDetector.SetAttachedMonitorSize(rcMonitor);
    }

    //切换到已设置的镜头模式
    SwitchLensMode(this->m_tCfgData.eLensMode);

    return bRet;
}

BOOL CIWBSensor::Stop()
{
    return m_oVideoPlayer.StopDetect();
}

void CIWBSensor::ShowMissStatusInfo()
{
    m_oVideoPlayer.MissStatusInfo();
}
//@功能:进入指定的工作模式
//@参数:eMode, 模式枚举值
void CIWBSensor::SwitchLensMode(ESensorLensMode eMode)
{
    ////////////判断是墙面模式还是桌面模式
    TSensorModeConfig* TSensorModeConfig = NULL;
    EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;
    TSensorModeConfig = &m_tCfgData.vecSensorModeConfig[eProjectionMode];

    BOOL bRet = FALSE;
    const TLensConfig& lensCfg = TSensorModeConfig->lensConfigs[this->m_eCameraType][m_tCfgData.eSelectedLensType];

    switch (eMode)
    {
    case E_VIDEO_TUNING_MODE:

        //禁用光笔
        //g_oMouseEventGen.EnableOpticalPenControl(FALSE);
        EnableOpticalPen(FALSE);

        TVideoProcAmpProperty cameraParams;
        bRet = m_oVideoPlayer.GetCameraParams(cameraParams);

        //打开滤光片
        IRCUTSwtich(m_oVideoPlayer.GetCaptureFilter(), FALSE, m_tDeviceInfo.m_nPID, m_tDeviceInfo.m_nVID);

        //载入安装调试时的相机参数
        bRet = m_oVideoPlayer.SetCameraParams(lensCfg.installTunningSettings.cameraParams);

        bRet = m_oVideoPlayer.GetCameraParams(cameraParams);

        if (!bRet)
        {
            AtlTrace(_T("Set CameraParam Failed!\n"));
        }
        {
            SIZE videoSize;
            bRet = m_oVideoPlayer.GetVideoSize(videoSize);

            if (bRet && videoSize.cx > 0 && videoSize.cy > 0)
            {
                //TOSDText::RectF rcText;
                RectF rcText;
                rcText.left = (float)lensCfg.rcGuideRectangle.left;  // / (float)videoSize.cx;
                rcText.top = (float)lensCfg.rcGuideRectangle.top;   // / (float)videoSize.cy;

                rcText.right = (float)lensCfg.rcGuideRectangle.right;  // / (float)videoSize.cx;
                rcText.bottom = (float)lensCfg.rcGuideRectangle.bottom;  // / (float)videoSize.cy;

                m_oVideoPlayer.AddOSDText(
                    E_OSDTEXT_TYPE_GUIDE_BOX,
                    g_oResStr[IDS_STRING465],
                    rcText,
                    DT_BOTTOM | DT_CENTER | DT_SINGLELINE,
                    8,
                    _T("Times New Roman"),
                    -1);
            }
        }

        //禁用"静态屏蔽"操作
        m_oPenPosDetector.EnableStaticMasking(FALSE);

        //停止"动态屏蔽"操作
        m_oPenPosDetector.EnableDynamicMasking(FALSE);

        //停止“抗干扰”操作
        m_oPenPosDetector.EnableAntiJamming(FALSE);

        //不显示光斑外接矩形
        m_oPenPosDetector.ShowSpotRect(FALSE);

        //不显示光斑尺寸信息
        m_oPenPosDetector.ShowSpotSizeInfo(FALSE);

        //使能画面亮度自动调节
        this->m_pInterceptFilter->EnableBrightnessAutoRegulating(TRUE);
        //
        break;

    case E_LASER_TUNING_MODE:
        //载入激光调试时的相机参数
        switch (GetActualTouchType())
        {
        case E_DEVICE_FINGER_TOUCH_CONTROL:
            m_oVideoPlayer.SetCameraParams(lensCfg.laserTunningSettings_FingerTouchControl.cameraParams);
            break;
        case E_DEVICE_PALM_TOUCH_CONTROL:
            m_oVideoPlayer.SetCameraParams(lensCfg.laserTunningSettings_PalmTouchControl.cameraParams);
            break;
        default:
            m_oVideoPlayer.SetCameraParams(lensCfg.laserTunningSettings_WhiteBoard.cameraParams);
            break;
        }

        m_oVideoPlayer.ClearOSDText(E_OSDTEXT_TYPE_GUIDE_BOX);
        //使能光笔
        //g_oMouseEventGen.EnableOpticalPenControl(FALSE);
        EnableOpticalPen(FALSE);

        //合上滤光片
        IRCUTSwtich(m_oVideoPlayer.GetCaptureFilter(), TRUE, m_tDeviceInfo.m_nPID, m_tDeviceInfo.m_nVID);

        //开启"静态屏蔽"操作
        m_oPenPosDetector.EnableStaticMasking(FALSE);

        //停止动态屏蔽操作
        m_oPenPosDetector.EnableDynamicMasking(FALSE);

        //停止“抗干扰”操作
        m_oPenPosDetector.EnableAntiJamming(FALSE);

        //显示光斑外接矩形
        m_oPenPosDetector.ShowSpotRect(TRUE);

        //不显示光斑尺寸信息
        m_oPenPosDetector.ShowSpotSizeInfo(TRUE);


        //禁用画面亮度自动调节
        this->m_pInterceptFilter->EnableBrightnessAutoRegulating(FALSE);

        break;

    case E_NORMAL_USAGE_MODE:
        //载入正常使用时的相机参数

        EDeviceTouchType eTouchType;
        eTouchType = TSensorModeConfig->advanceSettings.m_eTouchType;
        TVideoProcAmpProperty  camer;
        m_oVideoPlayer.GetCameraParams(camer);
        LOG_INF("Prop_VideoProcAmp_Brightness =%d, Prop_VideoProcAmp_Contrast =%d,Prop_VideoProcAmp_Gamma=%d,Prop_VideoProcAmp_Satuation=%d..",
            camer.Prop_VideoProcAmp_Brightness,
            camer.Prop_VideoProcAmp_Contrast,
            camer.Prop_VideoProcAmp_Gamma,
            camer.Prop_VideoProcAmp_Satuation);

        switch (eTouchType)
        {
        case E_DEVICE_PEN_TOUCH_WHITEBOARD:

            m_oVideoPlayer.SetCameraParams(lensCfg.normalUsageSettings_PenTouchWhiteBoard.cameraParams);
            break;
        case E_DEVICE_FINGER_TOUCH_WHITEBOARD:

            m_oVideoPlayer.SetCameraParams(lensCfg.normalUsageSettings_FingerTouchWhiteBoard.cameraParams);
            break;
        case E_DEVICE_FINGER_TOUCH_CONTROL:

            m_oVideoPlayer.SetCameraParams(lensCfg.normalUsageSettings_FingerTouchControl.cameraParams);
            break;
        case E_DEVICE_PALM_TOUCH_CONTROL:
            m_oVideoPlayer.SetCameraParams(lensCfg.normalUsageSettings_PalmTouchControl.cameraParams);
            break;
        default:
            break;
        }

        m_oVideoPlayer.ClearOSDText(E_OSDTEXT_TYPE_GUIDE_BOX);
        //合上滤光片
        IRCUTSwtich(m_oVideoPlayer.GetCaptureFilter(), TRUE, m_tDeviceInfo.m_nPID, m_tDeviceInfo.m_nVID);
        //使能光笔
        //g_oMouseEventGen.EnableOpticalPenControl(TRUE);
        EnableOpticalPen(TRUE);

        //开启"静态屏蔽"操作
        m_oPenPosDetector.EnableStaticMasking(TRUE);

        ///如果是正常模式的话那么就根据设置来判断是否需要开启动态屏蔽
        /////////MODIFY BY  VERA_ZHAO 2019.10.24
        if (TSensorModeConfig->advanceSettings.bIsDynamicMaskFrame)
        {
            //开启动态屏蔽操作
            m_oPenPosDetector.EnableDynamicMasking(TRUE);
        }
        else
        {
            //停止动态屏蔽操作
            m_oPenPosDetector.EnableDynamicMasking(FALSE);
        }

        //显示光斑外接矩形
        m_oPenPosDetector.ShowSpotRect(TRUE);

        //不显示光斑尺寸信息
        m_oPenPosDetector.ShowSpotSizeInfo(TRUE);

        this->m_pInterceptFilter->SetFrameSkipCount(10);
        //禁用画面亮度自动调节
        this->m_pInterceptFilter->EnableBrightnessAutoRegulating(FALSE);

        break;
    }

    //m_eLensMode = eMode;
    m_tCfgData.eLensMode = eMode;


    //add by vera_zhao 2109.12.18
    //全局工作模式和第一个相机的相机的工作模式保持一致。
    //g_tSysCfgData.globalSettings.eLensMode = m_eLensMode;
}

//@功能:设置视频捕获设备信息
void CIWBSensor::SetDeviceInfo(const TCaptureDeviceInstance& devInfo)
{
    m_tDeviceInfo = devInfo;

    /////如果是高清摄像头的话，如果设置中不是1080*720或者1920*1080的话就默认为1080*720
    /////SONIX_PID是720P的摄像头PID，720P的摄像头需要显示640*480MJPG。
    if (m_tDeviceInfo.m_nPID == SONIX_PID && m_tDeviceInfo.m_nVID == SONIX_VID)
    {
        if (theApp.GetPalmTouchType() == E_PLAM_TOUCHCONTROL_TX1 || theApp.GetPalmTouchType() == E_PLAM_TOUCHCONTROL_TX2)
        {
            m_tCfgData.strFavoriteMediaType = "640 X 480 MJPG";
        }
        else
        {
            if (m_tCfgData.strFavoriteMediaType != "1280 X 720 MJPG" && m_tCfgData.strFavoriteMediaType != "1920 X 1080 MJPG")
            {
                m_tCfgData.strFavoriteMediaType = "1280 X 720 MJPG";
            }
        }
        //        if (m_tCfgData.strFavoriteMediaType != "1280 X 720 MJPG" && m_tCfgData.strFavoriteMediaType != "1920 X 1080 MJPG")
        //        {
        //            m_tCfgData.strFavoriteMediaType = "1280 X 720 MJPG";
        //        }
    }

    //选取最合适的视频格式
    for (size_t i = 0; i < m_tDeviceInfo.m_vecVideoFmt.size(); i++)
    {
        CAtlString strFormatName = GetVideoFormatName(m_tDeviceInfo.m_vecVideoFmt[i]);

        if (strFormatName == m_tCfgData.strFavoriteMediaType)
        {
            m_tFavoriteMediaType = m_tDeviceInfo.m_vecVideoFmt[i];
            break;
        }
    }

    m_tCfgData.strFavoriteDevicePath = m_tDeviceInfo.m_strDevPath;
    m_tCfgData.strFavoriteMediaType = GetVideoFormatName(m_tFavoriteMediaType);

    

    m_eCameraType = ::GetCameraType(m_tDeviceInfo.m_nPID, m_tDeviceInfo.m_nVID);


    //每次更新设备路径, 就需要更新来自固件的"自动校准补偿系数"
    EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;
    TSensorModeConfig* pSensorModeConfig = &m_tCfgData.vecSensorModeConfig[eProjectionMode];
    const TLensConfig& lensCfg = pSensorModeConfig->lensConfigs[this->m_eCameraType][m_tCfgData.eSelectedLensType];
    UpdateAutoCalibrateCompensateCoefs(lensCfg);
    
    LOG_INF("CIWBSensor::SetDeviceInfo Sensor[%d].devPath=%s\r\n", this->m_nID, (const char*)CT2A(m_tDeviceInfo.m_strDevPath));
}

//@功能:返回视频捕获设备信息
const TCaptureDeviceInstance& CIWBSensor::GetDeviceInfo()const
{
    return m_tDeviceInfo;
}

//@功能:载入配置数据
//@参数:cfgData, 图像传感器的配置信息
//      pGlobalSettings, 输入参数, 指向全局配置信息的指针
void CIWBSensor::SetCfgData(const TSensorConfig& cfgData, const GlobalSettings* pGlobalSettings)
{

    UINT oldAreaNo = m_tCfgData.nScreenAreaNo;
    m_tCfgData = cfgData;

    if (m_tCfgData.nScreenAreaNo == UNDEFEIN_AREA_NO)
    {
        m_tCfgData.nScreenAreaNo = oldAreaNo;
    }

    m_ThrowRatioInfo.m_dbThrowRatioSelected = TRHOW_RATIO_LIST[m_tCfgData.eSelectedLensType];
    UpdateThrowRatioDisplayInfo();


    m_tDeviceInfo.m_strDevPath = cfgData.strFavoriteDevicePath;

    int nPID = 0, nVID = 0;
    int ret = _stscanf_s(m_tDeviceInfo.m_strDevPath, _T("\\\\?\\usb#vid_%04x&pid_%04x"), &nVID, &nPID);
    if (2 == ret)
    {
        m_tDeviceInfo.m_nPID = nPID;
        m_tDeviceInfo.m_nVID = nVID;

        m_eCameraType = ::GetCameraType(m_tDeviceInfo.m_nPID, m_tDeviceInfo.m_nVID);
    }

    //选取最合适的视频格式
    for (size_t i = 0; i < m_tDeviceInfo.m_vecVideoFmt.size(); i++)
    {
        CAtlString strFormatName = GetVideoFormatName(m_tDeviceInfo.m_vecVideoFmt[i]);

        if (strFormatName == m_tCfgData.strFavoriteMediaType)
        {
            m_tFavoriteMediaType = m_tDeviceInfo.m_vecVideoFmt[i];
            break;
        }
    }


    SetGlobalCfgData(pGlobalSettings);

}

void CIWBSensor::SetGlobalCfgData(const GlobalSettings* pGlobalSettings)
{
    TSensorModeConfig* pSensorModeConfig = NULL;
    EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;

    pSensorModeConfig = &m_tCfgData.vecSensorModeConfig[eProjectionMode];

    const TLensConfig& lensCfg = pSensorModeConfig->lensConfigs[this->m_eCameraType][m_tCfgData.eSelectedLensType];
    //设置画面自动调节时的平均亮度 == 自动校正时的第一组画面的平均亮度
    this->m_pInterceptFilter->SetImageAverageBrightness(lensCfg.autoCalibrateSettingsList[0].calibrateImageParams.autoCalibrateExpectedBrightness);

    const NormalUsageSettings* pNormalUsageSettings = NULL;

    //全局配置信息
    switch (theApp.GetUSBKeyTouchType())
    {
    case E_DEVICE_PEN_TOUCH_WHITEBOARD:

        //加密狗为笔触模式, 强制工作模式为笔触模式
        m_oPenPosDetector.SetTouchType(E_DEVICE_PEN_TOUCH_WHITEBOARD);
        pNormalUsageSettings = &lensCfg.normalUsageSettings_PenTouchWhiteBoard;

        break;
    case E_DEVICE_FINGER_TOUCH_WHITEBOARD:

        //加密狗为手触模式, 选用用户选择的触控模式
        m_oPenPosDetector.SetTouchType(pSensorModeConfig->advanceSettings.m_eTouchType);

        if (pSensorModeConfig->advanceSettings.m_eTouchType == E_DEVICE_PEN_TOUCH_WHITEBOARD)
        {
            pNormalUsageSettings = &lensCfg.normalUsageSettings_PenTouchWhiteBoard;
        }
        else
        {
            pNormalUsageSettings = &lensCfg.normalUsageSettings_FingerTouchWhiteBoard;
        }
        break;
    case E_DEVICE_FINGER_TOUCH_CONTROL:

        //加密狗为手指触控模式, 强制工作模式为手指触控模式
        m_oPenPosDetector.SetTouchType(E_DEVICE_FINGER_TOUCH_CONTROL);
        pNormalUsageSettings = &lensCfg.normalUsageSettings_FingerTouchControl;

        break;
    case E_DEVICE_PALM_TOUCH_CONTROL:

        //加密狗为手掌互动模式, 强制工作模式为手掌互动模式
        m_oPenPosDetector.SetTouchType(E_DEVICE_PALM_TOUCH_CONTROL);
        pNormalUsageSettings = &lensCfg.normalUsageSettings_PalmTouchControl;
        break;
    default:
        break;

    }

    RECT rcMonitor;
    GetAttachedScreenArea(rcMonitor);
    m_oPenPosDetector.SetAttachedMonitorSize(rcMonitor);

    //设置光斑检测门限
    if (pNormalUsageSettings)
    {
        m_oPenPosDetector.SetYThreshold(pNormalUsageSettings->cYThreshold);

        //设置动态屏蔽门限
        int nPercenttage = pNormalUsageSettings->nDynamicMaskThresholdPercentage;
        BYTE cDynamicMaskThreshold = BYTE(int(pNormalUsageSettings->cYThreshold) * nPercenttage / 100);
        m_oPenPosDetector.SetDynamicMaskThreshold(cDynamicMaskThreshold);


        //设置最小光斑尺寸
        SIZE minimumLightSpotSize;
        minimumLightSpotSize.cx = pNormalUsageSettings->nLightSpotMinimumWidth;
        minimumLightSpotSize.cy = pNormalUsageSettings->nLightSpotMinimumHeight;
        m_oPenPosDetector.SetMinimumLightSpotSize(minimumLightSpotSize);
    }

    ////初始化默认是640*480
    RECT rcGuideRectangle;
    rcGuideRectangle.left = lensCfg.rcGuideRectangle.left * 640;
    rcGuideRectangle.right = lensCfg.rcGuideRectangle.right * 640;
    rcGuideRectangle.top = lensCfg.rcGuideRectangle.top * 480;
    rcGuideRectangle.bottom = lensCfg.rcGuideRectangle.bottom * 480;

    m_oPenPosDetector.SetGuideRectangle(
        rcGuideRectangle, lensCfg.dwGuideRectangleColor);

    m_oPenPosDetector.ShowGuideRectangle(lensCfg.bRectangleVisible);

    //设置镜头焦距类型
    m_oPenPosDetector.SetLensFocalType(m_tCfgData.eSelectedLensType);

    //设置是否是背投模式
    m_oPenPosDetector.GetVideoToScreenMap().SetRearProjectMode(pSensorModeConfig->advanceSettings.bIsRearProjection);

    //设置自动校正补偿系数
    //此处不需要，因为在SetDeviceInfo函数内部会调用UpdateAutoCalibrateCompensateCoefs
    //UpdateAutoCalibrateCompensateCoefs(lensCfg);

    //设置CMOS芯片规格数据
    if (pGlobalSettings)
    {
        m_eCameraType = ::GetCameraType(m_tDeviceInfo.m_nPID, m_tDeviceInfo.m_nVID);
        if (m_eCameraType == E_CAMERA_MODEL_2)
        {
            TCMOSChipSpecification CMOSChipSpecification;
            CMOSChipSpecification.pixel_size = 0.003;
            CMOSChipSpecification.height_in_mm = pGlobalSettings->CMOSChipSpecification.height_in_mm;
            CMOSChipSpecification.height_in_pixel = pGlobalSettings->CMOSChipSpecification.height_in_pixel;
            CMOSChipSpecification.width_in_mm = pGlobalSettings->CMOSChipSpecification.width_in_mm;
            CMOSChipSpecification.width_in_pixel = pGlobalSettings->CMOSChipSpecification.width_in_pixel;
            m_oPenPosDetector.GetVideoToScreenMap().GetCalibAlog().SetCMOSChipSpecification(CMOSChipSpecification);
        }
        else
        {
            m_oPenPosDetector.GetVideoToScreenMap().GetCalibAlog().SetCMOSChipSpecification(pGlobalSettings->CMOSChipSpecification);
        }

    }

    //设置镜头规格数据
    m_oPenPosDetector.GetVideoToScreenMap().GetCalibAlog().SetLensSpecification(lensCfg.lensSpecification);

    //设置镜头的内部初始参数
    m_oPenPosDetector.GetVideoToScreenMap().GetCalibAlog().SetLensInternalAndSymmmetricDistortParams(
        lensCfg.bInternalAndSymmetricDistortParamsIsValid ?
        &lensCfg.lensInternalAndSymmetricDistortParams :
        NULL
    );
    //设置校正参数
    m_oPenPosDetector.GetVideoToScreenMap().SetCalibParams(pSensorModeConfig->calibParam);


    //设置Sensor实际关联的屏幕区域
    RECT rcArea;
    BOOL bRet = GetAttachedScreenArea(rcArea);
    if (bRet)
    {
        this->m_oPenPosDetector.GetVideoToScreenMap().SetActualMonitorResolution(rcArea);
    }

    //根据工作模式立即生效摄像头参数
    //switch (m_eLensMode)
    switch (this->m_tCfgData.eLensMode)
    {
    case E_VIDEO_TUNING_MODE:
        m_oVideoPlayer.SetCameraParams(lensCfg.installTunningSettings.cameraParams);
        break;

    case E_LASER_TUNING_MODE:

        switch (GetActualTouchType())
        {
        case E_DEVICE_FINGER_TOUCH_CONTROL:
            m_oVideoPlayer.SetCameraParams(lensCfg.laserTunningSettings_FingerTouchControl.cameraParams);
            break;
        case E_DEVICE_PALM_TOUCH_CONTROL:
            m_oVideoPlayer.SetCameraParams(lensCfg.laserTunningSettings_PalmTouchControl.cameraParams);
            break;
        default:
            m_oVideoPlayer.SetCameraParams(lensCfg.laserTunningSettings_WhiteBoard.cameraParams);
            break;
        }
        break;

    case E_NORMAL_USAGE_MODE:
        //m_oVideoPlayer.SetCameraParams(this->m_tCfgData.normalUsageSettings.cameraParams);
        if (pNormalUsageSettings) m_oVideoPlayer.SetCameraParams(pNormalUsageSettings->cameraParams);
        break;
    }

}

//@功能:获取配置数据
//@参数:cfgData, 图像传感器的配置信息
const TSensorConfig& CIWBSensor::GetCfgData()const
{
    return m_tCfgData;
}

//@功能:判断传感器是否正在工作
BOOL CIWBSensor::IsDetecting()
{
    return this->m_oVideoPlayer.IsDetecting();

}


//@功能:使能光笔
//@参数:bEnable,使能标志
void CIWBSensor::EnableOpticalPen(BOOL bEnable)
{
    //this->m_oPenPosDetector.GetMouseEventGenerator().EnableOpticalPenControl(bEnable);
    this->m_oPenPosDetector.EnableOpticalPenControl(bEnable);
}

//@功能:判断光笔是否在控制中
//@说明:只要有一支光笔在控制中,则返回TRUE
BOOL CIWBSensor::IsOpticalPenControlling()
{
    //return this->m_oPenPosDetector.GetMouseEventGenerator().IsOpticalPenControlling();
    return this->m_oPenPosDetector.IsOpticalPenControlling();
}

//@功能:开始图像传感器的自动校正流程
//@参数:ePattern, 校正图案模式
//      hNotifyWnd, 校正结束后的同志消息的接收窗体
void  CIWBSensor::StartAutoCalibrate(E_AutoCalibratePattern ePattern, HWND hNotifyWnd)
{
    if (!this->IsDetecting()) return;//未在运行则立即退出

    TSensorModeConfig* pSensorModeConfig = NULL;
    EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;
    pSensorModeConfig = &m_tCfgData.vecSensorModeConfig[eProjectionMode];

    const TLensConfig& lensCfg = pSensorModeConfig->lensConfigs[this->m_eCameraType][m_tCfgData.eSelectedLensType];

    //设置自动校正时的参数
    //刚开始摄像头参数是第一个参数
    m_oVideoPlayer.SetCameraParams(lensCfg.autoCalibrateSettingsList[0].cameraParams);


    //<<added by xuke, 2016/11/12
    //禁用画面亮度自动调节
    this->m_pInterceptFilter->EnableBrightnessAutoRegulating(FALSE);
    //>>


    //打开滤光片
    IRCUTSwtich(m_oVideoPlayer.GetCaptureFilter(), FALSE, m_tDeviceInfo.m_nPID, m_tDeviceInfo.m_nVID);


    //停止"静态屏蔽"操作
    m_oPenPosDetector.EnableStaticMasking(FALSE);

    //停止"动态屏蔽"操作
    m_oPenPosDetector.EnableDynamicMasking(FALSE);

    //停止“抗干扰”操作
    m_oPenPosDetector.EnableAntiJamming(FALSE);

    //不显示光斑外接矩形
    m_oPenPosDetector.ShowSpotRect(FALSE);

    //不显示光斑尺寸信息
    m_oPenPosDetector.ShowSpotSizeInfo(FALSE);


    TAutoCalibrateParams autoCalibrateParams;

    autoCalibrateParams.hNotifyWnd = hNotifyWnd;


    //autoCalibrateParams.ePattern = ePattern;
    autoCalibrateParams.ePattern = GetAutoCalibratePattern(pSensorModeConfig->autoCalibratePatternSettings.nPatternRowCount);




    //autoCalibrateParams.cAvgBrightness         = m_tCfgData.autoCalibrateSettings.autoCalibrateExpectedBrightness;
    autoCalibrateParams.ChangeCameraParamsProc = OnAutoCalibChangeCameraParams;
    autoCalibrateParams.lpCtx = (LPVOID)this;
    autoCalibrateParams.eDebugLevel = (ECalibDebugLevel)g_tSysCfgData.globalSettings.nDebugLevel;

    //高亮块对应的灰度值
    //BYTE  cHilightGray = m_tCfgData.autoCalibrateSettings.autoCalibrateHilightGray;
    //autoCalibrateParams.clrGridHighlight       = RGB(cHilightGray,cHilightGray,cHilightGray);
    ///delete by vera_zhao
//    autoCalibrateParams.imageParamsList        = lensCfg.autoCalibrateSettings.calibrateImageParamsList;

    autoCalibrateParams.bSaveInermediatFile = g_tSysCfgData.globalSettings.bSaveIntermediateFile;
    autoCalibrateParams.bRecordVideo = g_tSysCfgData.globalSettings.bRecordVideo;



    //在线手动设置的屏蔽区域
    autoCalibrateParams.bEnableOnlineScreenArea = this->m_oPenPosDetector.IsEnableOnlineScreenArea();
    std::vector<CPoint> vecOnlineVertices;
    this->m_oPenPosDetector.GetCurrentOnLineScreenAreaPt(vecOnlineVertices);
    int nOnlineVerticesCount = vecOnlineVertices.size();

    autoCalibrateParams.vecOnlineAreaVertices.resize(nOnlineVerticesCount);
    for (int i = 0; i < nOnlineVerticesCount; i++)
    {
        autoCalibrateParams.vecOnlineAreaVertices[i].x = vecOnlineVertices[i].x;
        autoCalibrateParams.vecOnlineAreaVertices[i].y = vecOnlineVertices[i].y;

    }

    autoCalibrateParams.autocalibrateparamslist.clear();
    int nCount = lensCfg.autoCalibrateSettingsList.size();


    for (int i = 0; i < nCount; i++)
    {
        AutoCalibrateParams  params;
        params.autoCalibrateImageParams = lensCfg.autoCalibrateSettingsList[i].calibrateImageParams;
        params.videoprocampproperty = lensCfg.autoCalibrateSettingsList[i].cameraParams;
        autoCalibrateParams.autocalibrateparamslist.push_back(params);
    }



    TScreenInfo tScreenInfo;
    BOOL bRet = this->GetAttachedScreenArea(tScreenInfo.rcArea);

    if (!bRet) return;
    autoCalibrateParams.vecScreenInfos.push_back(tScreenInfo);

    //准备静态屏蔽参数
    TStaticMaskingParams staticMaskingParams;
    staticMaskingParams.fpPreStaticMaskingProc = CIWBSensor::OnPreStaticMasking;
    staticMaskingParams.lpPreStaticMaskingCtx = this;
    staticMaskingParams.cStaticMaskThreshold = lensCfg.autoMaskSettings.cAutoMaskDetectThreshold;
    staticMaskingParams.nMaskEroseSize = lensCfg.autoMaskSettings.nMaskAreaEroseSize;

    m_oAutoCalibrator.StartCalibrating(autoCalibrateParams, staticMaskingParams);


    BOOL bRecordVideoTemp;

    bRecordVideoTemp = g_tSysCfgData.globalSettings.bRecordVideo;

    wchar_t szTmp[1024] = { 0 };
    wsprintf(szTmp, L" xxxx bRecordVideoTemp:%d, VideoFilePath:%s\n", bRecordVideoTemp, m_oAutoCalibrator.GetDebugVideoFilePath());
    OutputDebugString(szTmp);
    if (bRecordVideoTemp)
    {
        this->m_pInterceptFilter->StartRecording(m_oAutoCalibrator.GetDebugVideoFilePath());
    }

    m_oPenPosDetector.EnterCalibrateMode(m_oAutoCalibrator.GetCalibrateHWnd(), CALIBRATE_MODE_AUTO);

}


//@功能:开始自动屏蔽
//@参数:hNotifyWnd, 校正结束后的同志消息的接收窗体
void  CIWBSensor::StartAutoMasking(HWND hNotifyWnd)
{
    if (!this->IsDetecting()) return;//未在运行则立即退出

    TSensorModeConfig* TSensorModeConfig = NULL;

    EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;
    TSensorModeConfig = &m_tCfgData.vecSensorModeConfig[eProjectionMode];

    const TLensConfig& lensCfg = TSensorModeConfig->lensConfigs[this->m_eCameraType][m_tCfgData.eSelectedLensType];

    //设置自动校正时的参数
    //m_oVideoPlayer.SetCameraParams(lensCfg.autoCalibrateSettings.cameraParams);
    //>>

    //合上滤光片
    IRCUTSwtich(m_oVideoPlayer.GetCaptureFilter(), TRUE, m_tDeviceInfo.m_nPID, m_tDeviceInfo.m_nVID);

    //<<added by toxuke, 2016/11/12
    //禁用画面亮度自动调节
    this->m_pInterceptFilter->EnableBrightnessAutoRegulating(FALSE);
    //>>

    //停止"静态屏蔽"操作
    m_oPenPosDetector.EnableStaticMasking(FALSE);

    //停止"动态屏蔽"操作
    m_oPenPosDetector.EnableDynamicMasking(FALSE);

    //停止“抗干扰”操作
    m_oPenPosDetector.EnableAntiJamming(FALSE);

    //不显示光斑外接矩形
    m_oPenPosDetector.ShowSpotRect(FALSE);

    //不显示光斑尺寸信息
    m_oPenPosDetector.ShowSpotSizeInfo(FALSE);



    TAutoMaskingParams autoMaskingParams;

    autoMaskingParams.hNotifyWnd = hNotifyWnd;
    autoMaskingParams.ChangeCameraParamsProc = OnAutoCalibChangeCameraParams;
    autoMaskingParams.lpCtx = (LPVOID)this;
    autoMaskingParams.eDebugLevel = (ECalibDebugLevel)g_tSysCfgData.globalSettings.nDebugLevel;

    //高亮块对应的灰度值
    //BYTE  cHilightGray = m_tCfgData.autoCalibrateSettings.autoCalibrateHilightGray;
    //autoCalibrateParams.clrGridHighlight       = RGB(cHilightGray,cHilightGray,cHilightGray);
  //  autoMaskingParams.imageParamsList        = lensCfg.autoCalibrateSettings.calibrateImageParamsList;

    //delete by vera_zhao
//	autoMaskingParams.imageParamsList = calibrateImageParamsList;

    autoMaskingParams.bSaveInermediatFile = g_tSysCfgData.globalSettings.bSaveIntermediateFile;
    autoMaskingParams.bRecordVideo = g_tSysCfgData.globalSettings.bRecordVideo;//m_tCfgData.advanceSettings.bRecordVideo;


    autoMaskingParams.bDoStaticMaskingOnly = TRUE;//仅作静态屏蔽

    int nCount = lensCfg.autoCalibrateSettingsList.size();
    for (int i = 0; i < nCount; i++)
    {
        AutoCalibrateParams params;
        params.autoCalibrateImageParams = lensCfg.autoCalibrateSettingsList[i].calibrateImageParams;
        params.videoprocampproperty = lensCfg.autoCalibrateSettingsList[i].cameraParams;
        autoMaskingParams.autocalibrateparamslist.push_back(params);
    }

    //搜索系统屏幕个数
    //theApp.GetMonitorFinder().SearchDisplayDev();
    //if(this->m_nID >= theApp.GetMonitorFinder().GetDisplayDevCount()) return;
    //const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(this->m_nID);
    //if(pDisplayDevInfo)
    //{
    //    autoMaskingParams.monitors.push_back(*pDisplayDevInfo);
    //}
    TScreenInfo tScreenInfo;
    BOOL bRet = this->GetAttachedScreenArea(tScreenInfo.rcArea);
    if (!bRet) return;
    autoMaskingParams.vecScreenInfos.push_back(tScreenInfo);


    //准备静态屏蔽参数
    TStaticMaskingParams staticMaskingParams;
    staticMaskingParams.fpPreStaticMaskingProc = CIWBSensor::OnPreStaticMasking;
    staticMaskingParams.lpPreStaticMaskingCtx = this;
    staticMaskingParams.cStaticMaskThreshold = lensCfg.autoMaskSettings.cAutoMaskDetectThreshold;
    staticMaskingParams.nMaskEroseSize = lensCfg.autoMaskSettings.nMaskAreaEroseSize;

    int Width = m_oPenPosDetector.GetSrcImageWidth();
    int Height = m_oPenPosDetector.GetSrcImageHeight();
    m_oAutoCalibrator.StartMasking(autoMaskingParams, staticMaskingParams, Width, Height);

    //m_oPenPosDetector.GetMouseEventGenerator().EnterCalibrateMode(m_oAutoCalibrator.GetCalibrateHWnd(), CALIBRATE_MODE_AUTO);
    m_oPenPosDetector.EnterCalibrateMode(m_oAutoCalibrator.GetCalibrateHWnd(), CALIBRATE_MODE_AUTO);


}

//@功能:在视频画面中显示错误信息
void CIWBSensor::ShowErrInfo(LPCTSTR lpszInfo)
{
    m_oVideoPlayer.SetDisplayInfo(lpszInfo);
}

//@功能:传感器自动校正完成后的事件响应函数
//@参数:bSuccess, 成功/失败标志
void CIWBSensor::OnAutoCalibrateDone(BOOL bSuccess)
{

    if (!bSuccess)//失败
    {
        //如果调试输出目录不为空则弹出该文件夹
        if (!PathIsDirectoryEmpty(m_oAutoCalibrator.GetCalibrateIntermediataDirName()))
        {
            ShellExecute(NULL, NULL, m_oAutoCalibrator.GetCalibrateIntermediataDirName(), NULL, NULL, SW_SHOW);
        }

        //m_oVideoPlayer.SetDisplayInfo(_T("自动校正失败！"));
        //m_oVideoPlayer.SetDisplayInfo(_T("Auto Calibration Failed!"));
        m_oVideoPlayer.SetDisplayInfo(g_oResStr[IDS_STRING480]);
        m_oPenPosDetector.SetCalibrateFailed(TRUE);
        SwitchLensMode(E_VIDEO_TUNING_MODE);
    }
    else//成功
    {
        //m_oVideoPlayer.SetDisplayInfo(_T("Auto Calibrate Succeeded."));
        m_oVideoPlayer.SetDisplayInfo(g_oResStr[IDS_STRING481]);
        m_oPenPosDetector.SetCalibrateFailed(FALSE);

        CVideToScreenMap& vtsm = m_oPenPosDetector.GetVideoToScreenMap();

        const TCalibData* pCalibData = m_oAutoCalibrator.GetCalibrateData();

        if (pCalibData && pCalibData->allMonitorCalibData.size() > 0)
        {
            TCalibData calibData = *pCalibData;
            //vtsm.SetCalibrateData(*pCalibData, m_tCfgData.advanceSettings.bIsRearProjection);
            calibData.eCalibrateModel = E_CALIBRATE_MODEL_GENERICAL_CAMERA;
            calibData.eCalibType = E_CALIBRATE_TYPE_AUTO;
            calibData.lpCtx = const_cast<LPVOID>((const LPVOID)m_oAutoCalibrator.GetMaskFrame().GetData());

            EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;
            TSensorModeConfig* pSensorModeConfig = NULL;
            pSensorModeConfig = &m_tCfgData.vecSensorModeConfig[eProjectionMode];

            const TAdvancedSettings& advSettings = pSensorModeConfig->advanceSettings;
            calibData.bUsingScreenPhysicalDimensions = advSettings.bUsingScreenPhysicalDimensions;//
            calibData.nScreenWidthInmm = advSettings.nScreenWidthInmm;
            calibData.nScreenHeightInmm = advSettings.nScreenHeightInmm;

            vtsm.SetCalibrateData(calibData);

            //重新初始化校正实例
            ReinitCalibrateInst(calibData.eCalibrateModel);

            if (vtsm.DoCalibrate())
            {
                pSensorModeConfig->calibParam = *vtsm.GetCalibParams();
            }

            //CImageFrame  maskFrame = m_oAutoCalibrator.GetMaskFrame();
            CImageFrame  maskFrame;
            unsigned char init_value = 0x00;
            maskFrame.SetSize(m_oAutoCalibrator.GetMaskFrame().Width(), m_oAutoCalibrator.GetMaskFrame().Height(), 1, &init_value);

            //RECT Mrect = m_oPenPosDetector.GettSpotListProcessor()->GetVisibleScreenArea(m_nID, pCalibData->allMonitorCalibData[0].rcMonitor);
            const RECT& maskRect = this->m_rcMonintorArea;
            if (GenerateMaskFrameWithCalibrateData(maskFrame, maskRect))
            {
                //把计算好的屏蔽图和动态静态屏蔽图进行与运算。
                CImageFrame AllMaskFrame = m_oAutoCalibrator.GetMaskFinderFrame(maskFrame);

                m_oPenPosDetector.SetStaticMaskFrame(AllMaskFrame);
                m_oPenPosDetector.SaveStaticMaskFrame();
            }
            else
            {
                //设置静态屏蔽图
                m_oPenPosDetector.SetStaticMaskFrame(m_oAutoCalibrator.GetMaskFrame());
                //保存静态屏蔽图
                m_oPenPosDetector.SaveStaticMaskFrame();
            }
        }

        //设置静态屏蔽图
    //	m_oPenPosDetector.SetStaticMaskFrame(m_oAutoCalibrator.GetMaskFrame());
        //保存静态屏蔽图
    //	m_oPenPosDetector.SaveStaticMaskFrame();

        //设置光点到屏幕编号的映射信息
        m_oPenPosDetector.GetVideoToScreenMap().SetScreenMap(m_oAutoCalibrator.GetScreenMap());

        //保存屏幕映射图
        m_oPenPosDetector.GetVideoToScreenMap().SaveScreenMap();

        //进入正常使用模式
        SwitchLensMode(E_NORMAL_USAGE_MODE);

    }
    BOOL bRecordVideoTemp = g_tSysCfgData.globalSettings.bRecordVideo;
    if (bRecordVideoTemp)
    {
        if (m_pInterceptFilter->IsRecording())
        {
            this->m_pInterceptFilter->StopRecording();
        }
    }


    //注意一定要等校正完成后再离开校正模式，避免"光斑
    //离开校正模式
    //this->m_oPenPosDetector.GetMouseEventGenerator().LeaveCalibrateMode();
    m_oPenPosDetector.LeaveCalibrateMode();
}



//@功能:开始图像传感器的手动校正流程
//@参数:
//      nPtsInRow, 每行的校正点个数
//      nPtsInCol, 每列的校正点个数
//      hNotifyWnd, 校正结束后的同志消息的接收窗体
void  CIWBSensor::StartManualCalibrate(HWND hNotifyWnd, int nPtsInRow, int nPtsInCol, EManualCalibrateType eManualCalibType)
{
    TManualCalibrateParameters parameters;

    //搜索系统屏幕个数
    TScreenInfo tScreenInfo;
    BOOL bRet = this->GetAttachedScreenArea(tScreenInfo.rcArea);
    if (!bRet)
    {
        return;
    }
    parameters.vecScreenInfos.push_back(tScreenInfo);


    //合上滤光片
    IRCUTSwtich(m_oVideoPlayer.GetCaptureFilter(), TRUE, m_tDeviceInfo.m_nPID, m_tDeviceInfo.m_nVID);

    TSensorModeConfig* pSensorModeConfig = NULL;

    EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;
    pSensorModeConfig = &m_tCfgData.vecSensorModeConfig[eProjectionMode];

    parameters.nCalibratePointsInRow = (nPtsInRow == -1) ? pSensorModeConfig->manualCalibrateSetting.nPtNumInEachRow : nPtsInRow;
    parameters.nCalibratePointsInCol = (nPtsInCol == -1) ? pSensorModeConfig->manualCalibrateSetting.nPtNumInEachCol : nPtsInCol;
    parameters.hNotifyWnd = hNotifyWnd;
    parameters.eManualCalibrateType = eManualCalibType;


    //把图像的高和宽传递进手动校正中
    //add by vera_zhao 2018.11.30
    int ImageWidth = this->m_oPenPosDetector.GetSrcImageWidth();
    int ImageHeight = this->m_oPenPosDetector.GetSrcImageHeight();

    this->m_oManualCalibrator.StartCalibrate(parameters, ImageWidth, ImageHeight);

    m_oPenPosDetector.EnterCalibrateMode(this->m_oManualCalibrator, CALIBRATE_MODE_MANUAL);


}


//@功能:传感器手动校正完成后的事件响应函数
//@参数:bSuccess, 成功/失败标志
void CIWBSensor::OnManualCalibrateDone(BOOL bSuccess)
{
    if (bSuccess)
    {
        CVideToScreenMap& vtsm = m_oPenPosDetector.GetVideoToScreenMap();

        TCalibData calibData;
        calibData.allMonitorCalibData = m_oManualCalibrator.GetCalibrateData();
        calibData.szImage.cx = m_oManualCalibrator.GetImageWidth();
        calibData.szImage.cy = m_oManualCalibrator.GetImageHeight();

        calibData.eCalibrateModel = E_CALIBRATE_MODEL_GENERICAL_CAMERA;
        calibData.eCalibType = E_CALIBRATE_TYPE_MANUAL;

        EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;
        TSensorModeConfig* pSensorModeConfig = NULL;
        pSensorModeConfig = &m_tCfgData.vecSensorModeConfig[eProjectionMode];

        const TAdvancedSettings& advSettings = pSensorModeConfig->advanceSettings;
        calibData.bUsingScreenPhysicalDimensions = advSettings.bUsingScreenPhysicalDimensions;//
        calibData.nScreenWidthInmm              = advSettings.nScreenWidthInmm;
        calibData.nScreenHeightInmm             = advSettings.nScreenHeightInmm;

        

        vtsm.SetCalibrateData(calibData);


        //重新初始化校正实例
        ReinitCalibrateInst(calibData.eCalibrateModel);

        if (vtsm.DoCalibrate())
        {
            pSensorModeConfig->calibParam = *vtsm.GetCalibParams();
            CImageFrame maskFrame;
            unsigned char init_value = 0x00;
            maskFrame.SetSize(m_oManualCalibrator.GetScreenAreaMask().Width(), m_oManualCalibrator.GetScreenAreaMask().Height(), 1, &init_value);
            ///如果计算出来的屏蔽图有错误的话，那么就按以前找屏蔽图的方式。
            EManualCalibrateType eManualCalibrateType = m_oManualCalibrator.GetCalibrateParams().eManualCalibrateType;

            if (E_MUNUAL_CALIB_FULL_SCREEN == eManualCalibrateType)
            {

                RECT Mrect = m_oPenPosDetector.GettSpotListProcessor()->GetVisibleScreenArea(m_nID, calibData.allMonitorCalibData[0].rcMonitor);

                if (!GenerateMaskFrameWithCalibrateData(maskFrame, Mrect))
                {
                    maskFrame = m_oManualCalibrator.GetScreenAreaMask();
                    const TLensConfig& lensCfg = pSensorModeConfig->lensConfigs[this->m_eCameraType][m_tCfgData.eSelectedLensType];

                    //适当腐蚀屏蔽区，扩到屏幕区域
                    for (int r = 0; r < lensCfg.autoMaskSettings.nMaskAreaEroseSize; r++)
                    {
                        Morph_Dilate8(
                            maskFrame.GetData(),
                            maskFrame.GetData(),
                            maskFrame.Width(),
                            maskFrame.Height());
                    }
                }
            }
            else
            {
                maskFrame = m_oManualCalibrator.GetScreenAreaMask();
                const TLensConfig& lensCfg = pSensorModeConfig->lensConfigs[this->m_eCameraType][m_tCfgData.eSelectedLensType];

                //适当腐蚀屏蔽区，扩到屏幕区域
                for (int r = 0; r < lensCfg.autoMaskSettings.nMaskAreaEroseSize; r++)
                {
                    Morph_Dilate8(
                        maskFrame.GetData(),
                        maskFrame.GetData(),
                        maskFrame.Width(),
                        maskFrame.Height());
                }
            }

            //将手动校正找到的屏幕区域屏蔽图合并到静态屏蔽图中去
            m_oPenPosDetector.MergeManualCalibScreenMask(maskFrame);

            //保存静态屏蔽图
            m_oPenPosDetector.SaveStaticMaskFrame();
        }
    }

    //无论成功失败都进入正常使用模式
    //如果失败了,则自动使用原有的校正数据
    SwitchLensMode(E_NORMAL_USAGE_MODE);

    m_oPenPosDetector.LeaveCalibrateMode();
}


//@功能:传感器自动搜索屏蔽区域结束事件响应函数
//@参数:bSuccess, 成功/失败标志
void CIWBSensor::OnAutoSearchMaskAreaDone(BOOL bSuccess)
{
    if (!bSuccess)//失败
    {
        //显示自动屏蔽失败信息
        m_oVideoPlayer.SetDisplayInfo(_T("Auto Mask Failed！"));

        //打开调试输出目录
        ShellExecute(NULL, NULL, m_oAutoCalibrator.GetCalibrateIntermediataDirName(), NULL, NULL, SW_SHOW);

        SwitchLensMode(E_VIDEO_TUNING_MODE);
    }
    else//成功
    {
        //设置静态屏蔽图
        m_oPenPosDetector.SetStaticMaskFrame(m_oAutoCalibrator.GetMaskFrame());
        //保存静态屏蔽图
        m_oPenPosDetector.SaveStaticMaskFrame();

        //设置光点到屏幕编号的映射信息
        m_oPenPosDetector.GetVideoToScreenMap().SetScreenMap(m_oAutoCalibrator.GetScreenMap());

        //进入正常使用模式
        SwitchLensMode(E_NORMAL_USAGE_MODE);

    }

    //注意一定要等校正完成后再离开校正模式，避免"光斑
    //离开校正模式
    this->m_oPenPosDetector.LeaveCalibrateMode();
}

//@功能:开启光斑采集功能
//@参数: hSampleWnd, 采样窗体句柄
BOOL CIWBSensor::StartLightSpotSampling(HWND hSampleWnd)
{
    //if(bRet)
    //{
    //    //this->m_oPenPosDetector.GetMouseEventGenerator().EnterSpotSamplingMode(m_wndLightSpotSampling.m_hWnd, COLLECTSPOT_MODE_COLLECT);
    this->m_oPenPosDetector.EnterSpotSamplingMode(hSampleWnd);
    //}

    return TRUE;
}


//@功能:光斑采集结束事件的响应函数
//@参数:bSuccess, 成功失败标志
void CIWBSensor::OnLightSpotSamplingDone(const ALL_LIGHTSPOT_SAMPLE_SIZE& allSampleSize, BOOL bSuccess)
{

    this->m_oPenPosDetector.LeaveSpotSamplingMode();

    if (bSuccess)
    {
        this->m_oPenPosDetector.SetLightSpotSampleSize(allSampleSize);
    }

}


//@功能:屏幕分辨率发生变化时的事件响应函数
//@参数:rcNewMonitorResolution, 关联的屏幕分辨率信息

//void CIWBSensor::OnMonitorResolutionChange(const RECT&  rcNewMonitorResolution)
void CIWBSensor::OnMonitorResolutionChange()
{
    RECT rcArea;
    BOOL bRet = GetAttachedScreenArea(rcArea);
    if (bRet)
    {
        this->m_oPenPosDetector.GetVideoToScreenMap().SetActualMonitorResolution(rcArea);
        //		this->m_oPenPosDetector.SetScreenSize((rcArea.right - rcArea.left),(rcArea.bottom- rcArea.top));
    }
}

/*
@功能：开启智能检测挡板是否消失
*/
void CIWBSensor::OnStartDetectBackSplashVanished()
{
    this->m_oPenPosDetector.SetHasDetectSVStarted(true);
}

/*
@功能：停止智能检测挡板是否消失
*/
void CIWBSensor::OnStopDetectBackSplashVanished()
{
    this->m_oPenPosDetector.SetHasDetectSVStarted(true);
}


//@功能:设备丢失事件响应函数
void CIWBSensor::OnDeviceIsMissing()
{
    if (this->m_oPenPosDetector.IsManualCalibrating())
    {
        this->m_oManualCalibrator.OnDeviceMissing();
    }
    if (this->m_oPenPosDetector.IsAutoCalibrating())
    {
        this->m_oAutoCalibrator.OnDeviceMissing();
    }
}



//@功能:查看校正符号位置
//@参数:bShow,TRUE,显示符号
//            FALSE, 隐藏符号
//@Author:15077727@qq.com, 2015/09/09
BOOL CIWBSensor::CheckCalibrateSymbolPos(BOOL bShow)
{
    if (bShow)
    {
        if (this->m_nID >= theApp.GetMonitorFinder().GetDisplayDevCount()) return FALSE;
        const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(this->m_nID);
        if (NULL == pDisplayDevInfo) return FALSE;

        //const TCalibData* pCalibData = m_oAutoCalibrator.GetCalibrateData();
        const TCalibData& calibData = m_oPenPosDetector.GetVideoToScreenMap().GetCalibrateData();

        if (calibData.allMonitorCalibData.size() == 0)
        {
            MessageBox(NULL, _T("No Calibrate Data found"), _T("Error"), MB_OK | MB_ICONERROR);
            return FALSE;
        }


        int nSymbolCount = calibData.allMonitorCalibData[0].calibData.size();

        std::vector<POINT> symbolPos;
        symbolPos.resize(nSymbolCount);

        for (int i = 0; i < nSymbolCount; i++)
        {
            symbolPos[i] = calibData.allMonitorCalibData[0].calibData[i].ptScreenCoord;
        }

        std::vector<POINT> projectPos;
        projectPos.resize(nSymbolCount);

        for (int i = 0; i < nSymbolCount; i++)
        {
            const TPoint2D& pt2dImage = calibData.allMonitorCalibData[0].calibData[i].pt2DImageCoord;
            bool bIsOutside = FALSE;

            TPoint2D ptScreen;
            m_oPenPosDetector.GetVideoToScreenMap().GetPt(pt2dImage, &ptScreen, &bIsOutside, TRUE/*不做自动校正补偿*/);

            projectPos[i].x = long(ptScreen.d[0] + .5);
            projectPos[i].y = long(ptScreen.d[1] + .5);
        }

        m_oCalibSymbolChecker.ShowCalibSymbols(
            pDisplayDevInfo->rcMonitor,
            &symbolPos[0],
            nSymbolCount,
            RGB(255, 0, 0),
            &projectPos[0]
        );

        m_oPenPosDetector.ShowCalibratePoints(TRUE);

    }
    else
    {
        m_oCalibSymbolChecker.Hide();
        m_oPenPosDetector.ShowCalibratePoints(FALSE);
    }

    return TRUE;

}

//判断是否校正符号是否已经显示
BOOL CIWBSensor::IsCalibrateSymbolVisible()const
{
    return m_oCalibSymbolChecker.IsVisible();
}


void _stdcall CIWBSensor::OnManualSampleDoneCallBackProc(LPVOID lpCtx, BOOL bSuccess)
{
    CIWBSensor* lpSensor = reinterpret_cast<CIWBSensor*>(lpCtx);

    if (bSuccess)
    {
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);
        char szFileName[256];
        sprintf_s(
            szFileName,
            _countof(szFileName),
            "ReSampleData_%04d%02d%02d_%02d%02d%02d%03d.txt",
            sysTime.wYear,
            sysTime.wMonth,
            sysTime.wDay,
            sysTime.wHour,
            sysTime.wMinute,
            sysTime.wSecond,
            sysTime.wMilliseconds);

        FILE* file;

        errno_t err = fopen_s(&file, szFileName, "w");
        if (err == 0)
        {
            char szData[1256];

            const std::vector<TPoint2D> & vecSymbolsInVideo = lpSensor->GetCalibrateSymbolManualSampler().GetSymbolsCoordInVideo();
            const std::vector<TPoint2D> & vecLightSpotsInVideo = lpSensor->GetCalibrateSymbolManualSampler().GetLightSpotsCoordInVideo();

            const std::vector<POINT>& vecActualTouchPosInScreen = lpSensor->GetCalibrateSymbolManualSampler().GetActualTouchPosInScreen();
            const std::vector<POINT>& vecSymbolPosInScreen = lpSensor->GetCalibrateSymbolManualSampler().GetSymbolPosInScreen();
            int nCount = vecSymbolsInVideo.size();
            for (int i = 0; i < nCount; i++)
            {
                const TPoint2D& ptSymbol = vecSymbolsInVideo[i];
                const TPoint2D& ptLightSpot = vecLightSpotsInVideo[i];

                const POINT&  symbolPos = vecSymbolPosInScreen[i];
                const POINT&  actualTouch = vecActualTouchPosInScreen[i];
                sprintf_s(
                    szData,
                    _countof(szData),
                    "%f,%f,%f,%f,%d,%d,%d,%d\n",
                    (double)ptSymbol.d[0],
                    (double)ptSymbol.d[1],
                    (double)ptLightSpot.d[0],
                    (double)ptLightSpot.d[1],
                    symbolPos.x,
                    symbolPos.y,
                    actualTouch.x,
                    actualTouch.y
                );

                fwrite(szData, 1, strlen(szData), file);
            }

            fclose(file);
        }

    }

    lpSensor->GetPenPosDetector()->LeaveManualResample();

}


//@功能:对校正符号进行手动再采样
BOOL CIWBSensor::CalibrateSymbolManualResample()
{
    if (this->m_nID >= theApp.GetMonitorFinder().GetDisplayDevCount()) return FALSE;
    const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(this->m_nID);
    if (NULL == pDisplayDevInfo) return FALSE;


    const TCalibData& calibData = m_oPenPosDetector.GetVideoToScreenMap().GetCalibrateData();

    if (calibData.allMonitorCalibData.size() == 0)
    {
        MessageBox(NULL, _T("No Calibrate Data found"), _T("Error"), MB_OK | MB_ICONERROR);
        return FALSE;
    }


    int nSymbolCount = calibData.allMonitorCalibData[0].calibData.size();

    std::vector<POINT> symbolPosOnScreen;
    symbolPosOnScreen.resize(nSymbolCount);

    std::vector<TPoint2D> symbolPosOnImage;
    symbolPosOnImage.resize(nSymbolCount);

    for (int i = 0; i < nSymbolCount; i++)
    {
        symbolPosOnScreen[i] = calibData.allMonitorCalibData[0].calibData[i].ptScreenCoord;
        symbolPosOnImage[i] = calibData.allMonitorCalibData[0].calibData[i].pt2DImageCoord;
    }

    m_oCalibSymbolManualSampler.DoManualResample(
        pDisplayDevInfo->rcMonitor,
        &symbolPosOnScreen[0],
        &symbolPosOnImage[0],
        nSymbolCount,
        RGB(255, 0, 0),
        &CIWBSensor::OnManualSampleDoneCallBackProc,
        this
    );

    m_oPenPosDetector.EnterManualResample(m_oCalibSymbolManualSampler.GetHwnd());

    return TRUE;
    return FALSE;
}




//功能;判断传感器是否有校正数据
BOOL CIWBSensor::IsCalibrated()const
{
    const TCalibParams*  pCalibParams = m_oPenPosDetector.GetVideoToScreenMap().GetCalibParams();
    if (pCalibParams)
    {
        if (pCalibParams->allCalibCoefs.size() > 0)
        {
            if (pCalibParams->allCalibCoefs[0].calibCoefs.size() > 0)
                return TRUE;
        }
    }
    return FALSE;
}


/*
//@功能:获取关联的屏幕区域尺寸
BOOL CIWBSensor::GetAttachedScreenArea(RECT& rcMonitor)const
{
    theApp.GetMonitorFinder().SearchDisplayDev();

    int nMonitorCount = theApp.GetMonitorFinder().GetDisplayDevCount();
    if (nMonitorCount == 0) return FALSE;

    if (m_tCfgData.bAutoAttachMonitor)
    {//自动关联屏幕
        EScreenMode screenMode = theApp.GetScreenMode();

        if (screenMode == EScreenModeSingle)
        {//
            if (this->m_nID >= nMonitorCount) return FALSE;
            {
                const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetPrimaryMonitorInfo();
                rcMonitor = pDisplayDevInfo->rcMonitor;
            }

        }
        else if (screenMode >= EScreenModeDouble)
        {//多屏拼接模式
            //EScreenModeDouble://双屏模式
            //EScreenModeTriple://三屏模式
            //EScreenModeQuad  ://四屏模式
            //EScreenModeQuint ://五屏模式
            //EScreenModeHexa  ://六屏模式
            if (nMonitorCount == 1)
            {//只有一个屏幕
                //将屏幕等比例均分
                const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(0);

                LONG nWidth = pDisplayDevInfo->rcMonitor.right - pDisplayDevInfo->rcMonitor.left;
                rcMonitor = pDisplayDevInfo->rcMonitor;
                LONG left = rcMonitor.left;

                //每个屏幕的宽度
                LONG perWidth = nWidth / (int(screenMode) + 1);

                rcMonitor.left = left + this->m_nID*perWidth;

                rcMonitor.right = rcMonitor.left + perWidth;


            }
            else//多个屏幕
            {


            }
        }//else if

    }
    else
    {//手动指定屏幕
        if (this->m_tCfgData.nAttachedMonitorId >= theApp.GetMonitorFinder().GetDisplayDevCount())
        {
            return FALSE;
        }
        const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(this->m_tCfgData.nAttachedMonitorId);
        switch (this->m_tCfgData.eMonitorAreaType)
        {
        case E_MONITOR_AREA_TYPE_FULLSCREEN://全屏
            rcMonitor = pDisplayDevInfo->rcMonitor;
            break;

        case  E_MONITOR_AREA_TYPE_LEFT_HALF://左半屏
            rcMonitor = pDisplayDevInfo->rcMonitor;
            {
                int nWidth = pDisplayDevInfo->rcMonitor.right - pDisplayDevInfo->rcMonitor.left;
                rcMonitor.right = rcMonitor.left + (nWidth >> 1);
            }
            break;

        case E_MONITOR_AREA_TYPE_RIGHT_HALF://右半部
            rcMonitor = pDisplayDevInfo->rcMonitor;
            {
                int nWidth = pDisplayDevInfo->rcMonitor.right - pDisplayDevInfo->rcMonitor.left;
                rcMonitor.left = rcMonitor.right - (nWidth >> 1);
            }
            break;

        default:
            return FALSE;
        }
    }//else

    return TRUE;
}
*/

void CIWBSensor::SetAttachedScreenArea(const RECT& rcMonitor)
{
    m_rcMonintorArea = rcMonitor;
    this->m_oPenPosDetector.GetVideoToScreenMap().SetActualMonitorResolution(rcMonitor);
}

//@功能:获取关联的屏幕区域尺寸
BOOL CIWBSensor::GetAttachedScreenArea(RECT& rcMonitor)const
{
    rcMonitor = m_rcMonintorArea;
    return TRUE;
}

void CIWBSensor::OnTimer(LPVOID lpCtxData)
{
    m_oTimerActionExecuter.Run();
}

void CIWBSensor::SetlenCfgData(const TLensConfig& lencfgData)
{
    TSensorModeConfig* TSensorModeConfig = NULL;
    EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;
    TSensorModeConfig = &m_tCfgData.vecSensorModeConfig[eProjectionMode];

    TLensConfig& lensCfg = TSensorModeConfig->lensConfigs[this->m_eCameraType][m_tCfgData.eSelectedLensType];
    lensCfg = lencfgData;
}

//@功能：把设置插值的值保存下来
void CIWBSensor::SetStrokeInterpolate(bool bEnableStrokeInterpolate)
{
    TSensorModeConfig* TSensorModeConfig = NULL;

    EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;
    TSensorModeConfig = &m_tCfgData.vecSensorModeConfig[eProjectionMode];

    TSensorModeConfig->advanceSettings.bEnableStrokeInterpolate = bEnableStrokeInterpolate;
}

//@功能：把设置的勾勒外部屏幕区域的值保存下来
void CIWBSensor::SetOnlineScreenArea(bool bEnableOnlineScreenArea)
{
    EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;
    m_tCfgData.vecSensorModeConfig[eProjectionMode].advanceSettings.bIsOnLineScreenArea = bEnableOnlineScreenArea;

    this->m_oPenPosDetector.EnableOnlineScreenArea(bEnableOnlineScreenArea);

}

void CIWBSensor::GetPidVid(INT* pPID, INT* pVID)const
{
    if (pPID) *pPID = m_tDeviceInfo.m_nPID;
    if (pVID) *pVID = m_tDeviceInfo.m_nVID;
}


void CIWBSensor::Start4BasePointMarking(HWND hNotifyWnd)
{

    this->m_oPenPosDetector.EnterMarkStoneBaseMode(hNotifyWnd);
}

//@功能:传感器4点标定结束后的事件响应函数
//@参数:bSuccess, 成功/失败标志
void CIWBSensor::On4BasePointMarkingDone(BOOL bSuccess)
{
    if (bSuccess)
    {
        CVideToScreenMap& vtsm = m_oPenPosDetector.GetVideoToScreenMap();
        TCalibData calibData;
        calibData.allMonitorCalibData.resize(1);

        const TPoint2D* pBasePoints = m_oPenPosDetector.GetBaseStoneMarker().GetBasePoints();

        int nImageWidth = m_oPenPosDetector.GetSrcImageWidth();
        int nImageHeight = m_oPenPosDetector.GetSrcImageHeight();

        const POINT screenPoints[CBaseStoneMarker::BASE_STONE_NUMBER] = {
            { m_rcMonintorArea.left , m_rcMonintorArea.top    },
            { m_rcMonintorArea.right, m_rcMonintorArea.top    },
            { m_rcMonintorArea.right, m_rcMonintorArea.bottom },
            { m_rcMonintorArea.left , m_rcMonintorArea.bottom }
        };

        //屏幕区域
        calibData.allMonitorCalibData[0].rcMonitor = m_rcMonintorArea;

        //图像尺寸
        calibData.szImage.cx = nImageWidth;
        calibData.szImage.cy = nImageHeight;

        std::vector<TCalibCoordPair>& vecCoordPairs = calibData.allMonitorCalibData[0].calibData;
        vecCoordPairs.resize(4);

        for (int i = 0; i < 4; i++)
        {
            vecCoordPairs[i].pt2DImageCoord = pBasePoints[i];
            vecCoordPairs[i].ptScreenCoord = screenPoints[i];
        }

        calibData.eCalibrateModel = E_CALIBRATE_MODEL_4_POINST_PERSPECTIVE;
        calibData.eCalibType = E_CALIBRATE_TYPE_MANUAL;
        vtsm.SetCalibrateData(calibData);

        TSensorModeConfig* pSensorModeConfig = NULL;
        EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;
        pSensorModeConfig = &m_tCfgData.vecSensorModeConfig[eProjectionMode];

        if (vtsm.DoCalibrate())
        {
            pSensorModeConfig->calibParam = *vtsm.GetCalibParams();

            CImageFrame maskFrame;
            BYTE initValue = 0;
            maskFrame.SetSize(nImageWidth, nImageHeight, 1, &initValue);

            POINT aryPointsInImage[CBaseStoneMarker::BASE_STONE_NUMBER];
            for (int i = 0; i < CBaseStoneMarker::BASE_STONE_NUMBER; i++)
            {
                aryPointsInImage[i].x = long(pBasePoints[i].d[0]);
                aryPointsInImage[i].y = long(pBasePoints[i].d[1]);
            }

            FillPolygon(
                maskFrame.GetData(),
                nImageWidth,
                nImageHeight,
                aryPointsInImage,
                _countof(aryPointsInImage),
                255,
                TRUE);

            const TSensorModeConfig&  sensorModeConfig = m_tCfgData.vecSensorModeConfig[eProjectionMode];
            const TLensConfig& lensCfg = pSensorModeConfig->lensConfigs[this->m_eCameraType][m_tCfgData.eSelectedLensType];

            //适当腐蚀屏蔽区，扩到屏幕区域
            for (int r = 0; r < lensCfg.autoMaskSettings.nMaskAreaEroseSize; r++)
            {
                Morph_Dilate8(
                    maskFrame.GetData(),
                    maskFrame.GetData(),
                    maskFrame.Width(),
                    maskFrame.Height());
            }

            //将手动校正找到的屏幕区域屏蔽图合并到静态屏蔽图中去
            m_oPenPosDetector.MergeManualCalibScreenMask(maskFrame);

            //保存静态屏蔽图
            m_oPenPosDetector.SaveStaticMaskFrame();
        }
    }

    //无论成功失败都进入正常使用模式
    //如果失败了,则自动使用原有的校正数据
    SwitchLensMode(E_NORMAL_USAGE_MODE);
    this->m_oPenPosDetector.LeaveMarkStoneBaseMode();
}


//初始化校正实例
void CIWBSensor::ReinitCalibrateInst(E_CALIBRATE_MODEL eCalibrateModel)
{
    CalibrateAlgo& calibAlgo = this->m_oPenPosDetector.GetVideoToScreenMap().GetCalibAlog();

    calibAlgo.GenCalibratorInst(eCalibrateModel);

    if (eCalibrateModel == E_CALIBRATE_MODEL_GENERICAL_CAMERA)
    {

        const GlobalSettings& globalSettings = g_tSysCfgData.globalSettings;

        EProjectionMode eProjectionMode = globalSettings.eProjectionMode;

        const TSensorModeConfig&  sensorModeConfig = m_tCfgData.vecSensorModeConfig[eProjectionMode];

        const TLensConfig& lensCfg = sensorModeConfig.lensConfigs[this->m_eCameraType][m_tCfgData.eSelectedLensType];

        //设置"自动校正补偿系数"
        UpdateAutoCalibrateCompensateCoefs(lensCfg);


        //设置CMOS芯片规格数据 
        m_eCameraType = ::GetCameraType(m_tDeviceInfo.m_nPID, m_tDeviceInfo.m_nVID);
        if (m_eCameraType == E_CAMERA_MODEL_2)
        {
            TCMOSChipSpecification CMOSChipSpecification;
            CMOSChipSpecification.pixel_size = 0.003;
            CMOSChipSpecification.height_in_mm = globalSettings.CMOSChipSpecification.height_in_mm;
            CMOSChipSpecification.height_in_pixel = globalSettings.CMOSChipSpecification.height_in_pixel;
            CMOSChipSpecification.width_in_mm = globalSettings.CMOSChipSpecification.width_in_mm;
            CMOSChipSpecification.width_in_pixel = globalSettings.CMOSChipSpecification.width_in_pixel;
            calibAlgo.SetCMOSChipSpecification(CMOSChipSpecification);
        }
        else
        {
            calibAlgo.SetCMOSChipSpecification(globalSettings.CMOSChipSpecification);
        }



        //设置镜头规格数据
        calibAlgo.SetLensSpecification(lensCfg.lensSpecification);

        //设置镜头的内部初始参数
        calibAlgo.SetLensInternalAndSymmmetricDistortParams(lensCfg.bInternalAndSymmetricDistortParamsIsValid ?
            &lensCfg.lensInternalAndSymmetricDistortParams :
            NULL);
    }

}

void CIWBSensor::SetResolutionType(CAtlString  Value)
{
    m_tCfgData.strFavoriteMediaType = Value;
}

void CIWBSensor::OnStartDrawOnlineScreenArea()
{

    this->m_oPenPosDetector.ShowGuideRectangle(false);
    this->m_pInterceptFilter->SetStartDrawMaskFrame(true);
    /////清除数组中的数据
    this->m_oPenPosDetector.ClearOnLineScreenArea();
    m_oVideoPlayer.ClearOSDText(E_OSDTEXT_TYPE_GUIDE_BOX);

    ///在点击开始画时，如果是禁用状态的话，那么画好之后没有显示，无法判断，因此在开始画时，就要设置为非禁用状态
    this->m_oPenPosDetector.EnableOnlineScreenArea(!this->m_oPenPosDetector.IsEnableOnlineScreenArea());
    SetOnlineScreenArea(true);

    RectF textArea = { 0.0, 0.0, 1.0, 1.0 };

    m_oVideoPlayer.AddOSDText(
        E_OSDTEXT_TYPE_SHOW_INFO,
        g_oResStr[IDS_STRING494],
        textArea,
        DT_BOTTOM | DT_LEFT | DT_SINGLELINE,
        14L,
        _T("Times New Roman"),
        -1);
}

void CIWBSensor::OnClearDrawOnlineScreenArea()
{
    this->m_oPenPosDetector.DeleteOnLineScreenArea();

}
void CIWBSensor::OnEnableDrawOnlineScreenArea(BOOL Enable)
{
    SetOnlineScreenArea(Enable ? true : false);
}


BOOL CIWBSensor::IsEnableOnlineScreenArea()
{
    return this->m_oPenPosDetector.IsEnableOnlineScreenArea();
}

/////@功能;根据校正的数据计算
BOOL CIWBSensor::GenerateMaskFrameWithCalibrateData(CImageFrame& maskFrame, const RECT& rect, int nMonitorId)
{
    int Left = rect.left;
    int Top = rect.top;
    int Right = rect.right;
    int Bottom = rect.bottom;
    int x = Left;
    int y = Top;
    int ScreenWidth = Right - Left;
    int ScreenHeight = Bottom - Top;

    const int  INTERVAL_POINTS_COUNT = 4;
    int  LEVEL_POINTS_COUNT = INTERVAL_POINTS_COUNT + 1;
    int VERTROL_POINT_COUNT = LEVEL_POINTS_COUNT - 1;

    const int BORDER_POINTS_COUNT = 16;
    TPoint3D pt3DInScreen[BORDER_POINTS_COUNT];

    //上一排
    for (int i = 0; i < LEVEL_POINTS_COUNT; i++)
    {
        pt3DInScreen[i].d[0] = x + (i*ScreenWidth) / INTERVAL_POINTS_COUNT;
        pt3DInScreen[i].d[1] = y;
        pt3DInScreen[i].d[2] = 0;
    }

    //右竖一排
    for (int m = 1; m < VERTROL_POINT_COUNT; m++)   //LEVEL_POINTS_COUNT-1 = 4
    {
        pt3DInScreen[m + VERTROL_POINT_COUNT].d[0] = x + ScreenWidth;
        pt3DInScreen[m + VERTROL_POINT_COUNT].d[1] = y + (ScreenHeight*m) / VERTROL_POINT_COUNT;
        pt3DInScreen[m + VERTROL_POINT_COUNT].d[2] = 0;
    }

    //下一排
    for (int j = 0; j < LEVEL_POINTS_COUNT; j++)
    {
        pt3DInScreen[j + LEVEL_POINTS_COUNT + (VERTROL_POINT_COUNT - 1)].d[0] = x + (INTERVAL_POINTS_COUNT - j) *ScreenWidth / INTERVAL_POINTS_COUNT;//x
        pt3DInScreen[j + LEVEL_POINTS_COUNT + (VERTROL_POINT_COUNT - 1)].d[1] = y + ScreenHeight;
        pt3DInScreen[j + LEVEL_POINTS_COUNT + (VERTROL_POINT_COUNT - 1)].d[2] = 0;
    }

    //左竖一排
    for (int k = 1; k < VERTROL_POINT_COUNT; k++)
    {
        pt3DInScreen[k + 2 * LEVEL_POINTS_COUNT + VERTROL_POINT_COUNT - 2].d[0] = x;
        pt3DInScreen[k + 2 * LEVEL_POINTS_COUNT + VERTROL_POINT_COUNT - 2].d[1] = y + (ScreenHeight*(VERTROL_POINT_COUNT - k)) / VERTROL_POINT_COUNT;
        pt3DInScreen[k + 2 * LEVEL_POINTS_COUNT + VERTROL_POINT_COUNT - 2].d[2] = 0;
    }

    TPoint2D pt2DInImage[BORDER_POINTS_COUNT];
    POINT    ptInImage[BORDER_POINTS_COUNT];
    m_oPenPosDetector.GetVideoToScreenMap().GetCalibAlog().MapWorldToImage(&pt3DInScreen[0], BORDER_POINTS_COUNT, &pt2DInImage[0], nMonitorId);
    int nCount = 0;
    for (int i = 0; i < BORDER_POINTS_COUNT; i++)
    {
        if (pt2DInImage[i].d[0] <= 0.00 && pt2DInImage[i].d[1] <= 0.00)
        {
            nCount++;
        }

        ptInImage[i].x = pt2DInImage[i].d[0];
        ptInImage[i].y = pt2DInImage[i].d[1];
    }
    if (nCount > 10)
    {
        return FALSE;
    }
    FillPolygon(maskFrame.GetData(),
        maskFrame.Width(),
        maskFrame.Height(),
        &ptInImage[0],
        BORDER_POINTS_COUNT,
        0xff,
        true);

    //扩大几个像素
    for (int i = 0; i < 5; i++)
    {
        Morph_Dilate8(maskFrame.GetData(), maskFrame.GetData(), maskFrame.Width(), maskFrame.Height());
    }

    return TRUE;
}

void CIWBSensor::GetCollectSpotShowPath(TCHAR *lpszbuf, unsigned int numberOfElements)
{
    if (g_tSysCfgData.globalSettings.eProjectionMode == E_PROJECTION_DESKTOP)
    {
        _stprintf_s(lpszbuf, numberOfElements, _T("%s\\Sensor%02d\\DesktopMode\\%s"), (LPCTSTR)PROFILE::SETTINGS_BASE_DIRECTORY, m_nID, _T("CollectSpotCross.xml"));
    }
    else
    {
        _stprintf_s(lpszbuf, numberOfElements, _T("%s\\Sensor%02d\\WallMode\\%s"), (LPCTSTR)PROFILE::SETTINGS_BASE_DIRECTORY, m_nID, _T("CollectSpotCross.xml"));
    }
}

void CIWBSensor::UpdateTouchTypeFromUSBKey()
{
    EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;
    m_tCfgData.vecSensorModeConfig[eProjectionMode].advanceSettings.m_eTouchType = theApp.GetUSBKeyTouchType();
}



ELensType CIWBSensor::MapThrowRatioToLensType(const double& throwRatio)
{
    ELensType eLensType;
    char szThrowRatio[128];
    sprintf_s(szThrowRatio, _countof(szThrowRatio), "%.2f", throwRatio);
    if (_stricmp(szThrowRatio, "0.15") == 0)
    {
        eLensType = E_LENS_TR_0_DOT_15;
    }
    else if (_stricmp(szThrowRatio, "0.19") == 0)
    {
        eLensType = E_LENS_TR_0_DOT_19;
    }
    else if (_stricmp(szThrowRatio, "0.21") == 0)
    {
        eLensType = E_LENS_TR_0_DOT_21;
    }
    else if (_stricmp(szThrowRatio, "0.25") == 0)
    {
        eLensType = E_LENS_TR_0_DOT_25;
    }
    else if (_stricmp(szThrowRatio, "0.28") == 0)
    {
        eLensType = E_LENS_TR_0_DOT_28;
    }
    else if (_stricmp(szThrowRatio, "0.34") == 0)
    {
        eLensType = E_LENS_TR_0_DOT_34;
    }
    else if (_stricmp(szThrowRatio, "0.55") == 0)
    {
        eLensType = E_LENS_TR_0_DOT_55;
    }
    else if (_stricmp(szThrowRatio, "0.70") == 0)
    {
        eLensType = E_LENS_TR_0_DOT_70;
    }
    else if (_stricmp(szThrowRatio, "0.86") == 0)
    {
        eLensType = E_LENS_TR_0_DOT_86;
    }
    else
    {
        eLensType = E_LENS_TR_1_DOT_34;
    }

    return eLensType;
}

void CIWBSensor::UpdateAutoCalibrateCompensateCoefs(const TLensConfig& lensConfig)
{
    CalibrateAlgo& calibAlgo = this->m_oPenPosDetector.GetVideoToScreenMap().GetCalibAlog();

    //首先设置为配置文件中保存的“自动校准补偿系数"
    calibAlgo.SetAutoCalibCompCoefs(lensConfig.autoCalibCompCoefs);


    std::wstring_convert<std::codecvt<wchar_t, char, mbstate_t>> converter;
    std::string strDevPath = converter.to_bytes(m_tDeviceInfo.m_strDevPath);
    const TAutoCalibrateCompensateData* pCompensateData = theApp.GetCompensateData(strDevPath.c_str());

    //如果在固件中找到"自动校准补偿系数"，并且和当前选择的投射比一致则使用固件中的"自动校准补偿系数"
    if (pCompensateData)
    {

        m_ThrowRatioInfo.m_dbThrowRatioInFirmware = pCompensateData->throwRatioOfLens;



        if (MapThrowRatioToLensType(pCompensateData->throwRatioOfLens) == m_tCfgData.eSelectedLensType)
        {
            calibAlgo.SetAutoCalibCompCoefs(pCompensateData->coefs);
        }

    }
    else
    {//没有找到自校准动补偿系数, 固件中的投射比设为零
        m_ThrowRatioInfo.m_dbThrowRatioInFirmware = 0.0;
    }

    UpdateThrowRatioDisplayInfo();


}




void CIWBSensor::UpdateThrowRatioDisplayInfo()
{
    //"固件中的镜头的投射比"信息显示区域
    RectF rcThrowRatioInfo = { 0.5, 0.0, 1.0, 0.5 };

    TCHAR szThrowRatio[128];
    _stprintf_s(szThrowRatio, _countof(szThrowRatio), _T("%.2f--%.2f"), m_ThrowRatioInfo.m_dbThrowRatioInFirmware, m_ThrowRatioInfo.m_dbThrowRatioSelected);

    m_oVideoPlayer.AddOSDText(
        E_OSDTEXT_TYPE_LENS_INFO,
        szThrowRatio,
        rcThrowRatioInfo,
        DT_TOP | DT_RIGHT | DT_SINGLELINE,
        8,
        _T("Times New Roman"),
        -1);

}


UINT CIWBSensor::GetScreenAreaNo()
{

    return this->m_tCfgData.nScreenAreaNo;
}

void CIWBSensor::SetScreenAreaNo(UINT areaNo)
{
    this->m_tCfgData.nScreenAreaNo = areaNo;
}