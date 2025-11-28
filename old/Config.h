#pragma once
#include "../inc/CameraSpecs.h"
#include "../inc/Log.h"
//#define SENSOR_NUMBER 6

//屏幕模式
enum EScreenMode
{
    EScreenModeSingle = 0,//单屏模式
    EScreenModeDouble = 1,//双屏模式
    EScreenModeTriple = 2,//三屏模式
    EScreenModeQuad = 3,//四屏模式
    EScreenModeQuint = 4,//五屏模式
    EScreenModeHexa = 5,//六屏模式
    EScreenModeNumber = 6,//屏幕模式个数
    EScreenModeUnknown = (UINT)-1
};

#define  SENSOR_NUMBER int(EScreenModeNumber)

struct TScreenLayout
{
    std::vector<RectF> vecScreens   ;//屏幕区域数组
    std::vector<RectF> vecMergeAreas;//光斑融合区数组
};


//屏幕分割模式
enum ESplitScreeMode
{
	E_SPLIT_SCREEN_VERT = 0,//沿垂直方向切割屏幕
	E_SPLIT_SCREEN_HORZ = 1,//沿水平方向切割屏幕
	E_SPLIT_SCREEN_MODE_COUNT = 2
};


struct LayoutCollection
{
	ESplitScreeMode eSplitScreenModel;         //选择的屏幕切割方式
	std::vector<TScreenLayout> allScreenLayout;//所有的屏幕布局
	
	LayoutCollection()
	{
		eSplitScreenModel = E_SPLIT_SCREEN_VERT;
	}
};
//屏幕布局管理器
struct ScreenLayoutManager
{
	std::map<ESplitScreeMode, LayoutCollection*> allLayoutCollection;
	
	ESplitScreeMode eSelectedSplitScreenMode;//当前选择的屏幕分割模式

	ScreenLayoutManager()
	{
		eSelectedSplitScreenMode = E_SPLIT_SCREEN_VERT;
	}

	~ScreenLayoutManager()
	{
		for (auto it = allLayoutCollection.begin(); it != allLayoutCollection.end(); it++)
		{
			delete it->second;
		}
	}

	void Reset()
	{
		eSelectedSplitScreenMode = E_SPLIT_SCREEN_VERT;
		allLayoutCollection.clear();
	}

	ESplitScreeMode GetSelectedSplitScreenMode()const
	{
		return eSelectedSplitScreenMode;
	}

	//@功能:返回指定屏幕分割模式下的指定屏幕个数的屏幕布局数据
	TScreenLayout* GetScreenLayout(ESplitScreeMode eScreenSplitMode, UINT nScreenCount)
	{
		TScreenLayout* pScreenLayout = NULL;

		LayoutCollection* pLayoutCollection = NULL;

		auto it = allLayoutCollection.find(eScreenSplitMode);

		if (it != allLayoutCollection.end())
		{
			pLayoutCollection = it->second;
		}
		else 
		{
			pLayoutCollection = new LayoutCollection();

			pLayoutCollection->eSplitScreenModel = eScreenSplitMode;
			allLayoutCollection[eScreenSplitMode] = pLayoutCollection;
		}
				

		auto& allScreenLayout = pLayoutCollection->allScreenLayout;

		for (UINT i = 0; i < allScreenLayout.size(); i++)
		{
			TScreenLayout& screenLayout = allScreenLayout[i];
			if (screenLayout.vecScreens.size() == nScreenCount)
			{
				pScreenLayout = &screenLayout;
				break;
			}
		}

		return pScreenLayout;
	}



	//功能:更新布局数据
	void SetScreenLayout(ESplitScreeMode eNewSelectedScreenSplitMode, const TScreenLayout& newScreenLayout)
	{

		LayoutCollection* pLayoutCollection = NULL;

		auto it = allLayoutCollection.find(eNewSelectedScreenSplitMode);

		if (it != allLayoutCollection.end())
		{
			pLayoutCollection = it->second;
		}
		else
		{
			pLayoutCollection = new LayoutCollection();

			pLayoutCollection->eSplitScreenModel = eNewSelectedScreenSplitMode;
			allLayoutCollection[eNewSelectedScreenSplitMode] = pLayoutCollection;
		}


		auto& allScreenLayout = pLayoutCollection->allScreenLayout;

		BOOL bUpdateDone = FALSE;
		for (UINT i = 0; i < allScreenLayout.size(); i++)
		{
			TScreenLayout& screenLayoutExists = allScreenLayout[i];
			if (screenLayoutExists.vecScreens.size() == newScreenLayout.vecScreens.size())
			{
				screenLayoutExists = newScreenLayout;
				bUpdateDone = TRUE;
				break;
			}
		}

		if (!bUpdateDone)
		{//未发现既有布局，则作为新的布局插入
			allScreenLayout.push_back(newScreenLayout);
		}


		this->eSelectedSplitScreenMode = eNewSelectedScreenSplitMode;
	}

	

};

//传感器镜头模式
enum ESensorLensMode
{
    E_VIDEO_TUNING_MODE,//图像调试模式
    E_LASER_TUNING_MODE,//激光器调试模式
    E_NORMAL_USAGE_MODE,//正常使用模式
};

//触控类型
enum EDeviceTouchType
{
    E_DEVICE_PEN_TOUCH_WHITEBOARD    = 0, //笔触控电子白板（精准触控）
    E_DEVICE_FINGER_TOUCH_WHITEBOARD = 1, //手触控电子白板（精准触控）
	E_DEVICE_PALM_TOUCH_CONTROL      = 2, //手掌互动
	E_DEVICE_FINGER_TOUCH_CONTROL    = 3, //手指触控(大画面精准触控)
	E_DEVICE_NOT_FOUND                = -1, //没发现加密狗
};

//触控方式类型
enum EAIROPERATE_CLICKMODE
{
	E_MODE_CLICK,
	E_MODE_DOUBLE_CLICK,
};

/////手指触控的类型
enum EFingerTouchControlType
{
	E_FINGER_TOUCHCONTROL_F0 = 0,
	E_FINGER_TOUCHCONTROL_F1 = 1,
	E_FINGER_TOUCHCONTROL_F2 = 2,
	E_FINGER_TOUCHCONTROL_F3 = 3,
	E_FINGER_TOUCHCONTROL_F4 = 4,
	E_FINGER_TOUCHCONTROL_F5 = 5,
    E_FINGER_TOUCHCONTROL_UNKNOWN = -1,
};

/////手掌互动触控的类型
enum EPalmTouchControlType
{
	///////P系列是30帧的，精度需要做偏差
	///////T系列是60帧的，精度不需要做偏差
	E_PLAM_TOUCHCONTROL_P0 = 0, //保留。   
	E_PLAM_TOUCHCONTROL_P1 = 1, //(这个是标清的摄像头)手掌互动触控的类型，激光器是放在左上角的位置的。
	E_PLAM_TOUCHCONTROL_P2 = 2, //保留。
	E_PLAM_TOUCHCONTROL_P3 = 3, //(这个是标清的摄像头)手掌互动触控的类型，激光器是放在中间位置的。
	E_PLAM_TOUCHCONTROL_P4 = 4, //(这个是高清摄像头)手掌互动触控的类型，激光器是放在中间位置的。
	E_PLAM_TOUCHCONTROL_P5 = 5, //(这个是高清摄像头)手掌互动触控的类型，激光器是放在中间位置的。
	E_PLAM_TOUCHCONTROL_T0 = 6, //保留。
	E_PLAM_TOUCHCONTROL_T1 = 7, //保留。
	E_PLAM_TOUCHCONTROL_T2 = 8, //保留。
	E_PLAM_TOUCHCONTROL_T3 = 9, //(这个是标清的摄像头)手掌互动触控的类型，激光器是放在中间位置的。
	E_PLAM_TOUCHCONTROL_T4 = 10, //(这个是高清摄像头)手掌互动触控的类型，激光器是放在中间位置的。
	E_PLAM_TOUCHCONTROL_T5 = 11, //(这个是高清摄像头)手掌互动触控的类型，激光器是放在中间位置的。
	E_PLAM_TOUCHCONTROL_TX1 =12,
	E_PLAM_TOUCHCONTROL_TX2 = 13,
	E_PLAM_TOUCHCONTROL_UNKNOWN = -1,
};

enum EHIDDeviceMode
{
	E_DEV_MODE_MOUSE,   //鼠标
	E_DEV_MODE_TOUCHSCREEN,//触屏
};

enum EProjectionMode
{
	E_PROJECTION_DESKTOP = 0, //桌面模式
	E_PROJECTION_WALL   = 1,   //墙面模式
	E_PROJECTION_COUNT = 2
};

inline const TCHAR* GetProjectModeString(EProjectionMode eProjectionMode)
{
    switch (eProjectionMode)
    {
    case E_PROJECTION_WALL:
        return _T("WallMode");
        break;


    case E_PROJECTION_DESKTOP:
        return _T("DesktopMode");
        break;

    default:
        LOG_ERR("Unknown Project Mode, eProjectionMode=%d", (int)eProjectionMode);
        break;

    }//switch

    return _T("");
}

enum ECameraType
{
	E_CAMERA_MODEL_0 = 0,   //PID=0x9186,VID=0x18ec  ,正常摄像头
	E_CAMERA_MODEL_1 = 1,   //PID=0x3390,VID=0x18ec  ,1/6摄像头
	E_CAMERA_MODEL_2 = 2,   //PID=0x9230,VID= 0x05a3 ,高清摄像头
	E_CAMERA_MODEL_COUNT = 3
};

inline const TCHAR*  GetCameraTypeString(ECameraType eCameraType)
{
    switch (eCameraType)
    {
        case E_CAMERA_MODEL_0:
            return _T("OV7725");

        case E_CAMERA_MODEL_1:
            return _T("QC0308");

        case E_CAMERA_MODEL_2:
            return _T("OV2710");

        default:
            LOG_ERR("Unknown Camera Type, eCameraType=%d", (int)eCameraType);
            break;

    }//switch

    return _T("");
}

inline ECameraType GetCameraType(UINT uPID, UINT uVID)
{
	ECameraType eCameraType = E_CAMERA_MODEL_0;

	if (uPID == 0x9186 && uVID == 0x18ec)
	{
		eCameraType = E_CAMERA_MODEL_0;
	}
	else if(uPID == 0x3390 && uVID == 0x18ec)
	{
		eCameraType = E_CAMERA_MODEL_1;
	}
	else if (uPID == 0x9230 && uVID == 0x05a3)
	{
		eCameraType = E_CAMERA_MODEL_2;
	}
    else
    {
        LOG_ERR("GetCameraType(pid=0x%x, vid=0x%x) is unexpected!", uPID, uVID);
    }
	return eCameraType;
}

enum ETouchScreenAspectRatio
{
    E_TOUCH_SCREEN_ASPECT_RATIO_AUTO  ,//自动决定宽高比
    E_TOUCH_SCREEN_ASPECT_RATIO_16_9 ,//宽高比16:9
    E_TOUCH_SCREEN_ASPECT_RATIO_16_10 ,//宽高比16:10
    E_TOUCH_SCREEN_ASPECT_RATIO_4_3   ,//宽高比4:3
};
#ifdef PICO_PROJECTOR
#define DEFAULT_SCREEN_DIAGONAL_LENGTH (20.0f*25.4f)
#else
#define DEFAULT_SCREEN_DIAGONAL_LENGTH (80.0f*25.4f)
#endif


//<added by Jiqw 201505191446
#define DEFAULT_SCREEN_DIAGONAL_LOGICAL_LENGTH (1270) //1024 * 768
//>

//全局设定
struct GlobalSettings
{
    CAtlString              langCode             ;   //语言ID
	EProjectionMode         eProjectionMode      ;   //墙面还是桌面
    BOOL                    bDebugMode           ;   //调试模式
    int                     nDebugLevel          ;   //调试级别
    BOOL                    bSaveIntermediateFile;   //保存中间文件标志
    ESensorLensMode         eLensMode            ;   //镜头工作模式
	BOOL                    bTouchTUIOMode       ;   //TUIO触控方式
	BOOL                    bTouchHIDMode        ;   //HID触控方式
    EHIDDeviceMode          eHIDDeviceMode       ;   //鼠标/触屏模式
    ETouchScreenAspectRatio eTouchScreenAspectRatio; //触屏宽高比
    BOOL                    bEnableOnlineRegister;   //使能在线注册功能

    BOOL                    bRecordVideo          ; //是否在自动校正时保存画面录像
    BOOL                    bDoubleScreenTouchMerge;//双屏是否融合标志
//  ELensFocalType          eLensFocalType      ;//镜头焦距类型

    float                   fScreenDiagonalPhysicalLength;//触屏对角线物理尺寸,单位:毫米(mm);
    int                     nMaxTimeInSearchDevice;//程序启动时,搜索设备时最大允许花费的时间， 单位:毫秒。
    TCMOSChipSpecification  CMOSChipSpecification;//CMOS芯片规格信息

	//ECameraType             eCameraType;         //攝像頭的類型

    EScreenMode            eScreenMode;//屏接的屏幕数目
	BOOL                   bSinglePointMode;
    BOOL                   bEnable4PointsCalibrate;//使能4点标定
	BOOL                   bModifyResolution      ;//是否可以修改分辨率

	BOOL                   bAirOperatePermission  ;
	EAIROPERATE_CLICKMODE   eClickMode             ;
    uint32_t                circleCalibrateXScale;//圆形校准X坐标缩放尺寸(50~100)
    uint32_t                circleCalibrateYScale;//圆形校准X坐标缩放尺寸(50~100)

    GlobalSettings()
    {
        langCode                 = _T("");
		//eProjectionMode          = E_PROJECTION_DESKTOP;
        //默认模式为墙面
        eProjectionMode          = E_PROJECTION_WALL;
        nDebugLevel              = 0;
        bDebugMode               = FALSE;
        bSaveIntermediateFile    = FALSE;
        eLensMode                = E_VIDEO_TUNING_MODE;
        eHIDDeviceMode           = E_DEV_MODE_TOUCHSCREEN;
        eTouchScreenAspectRatio  = E_TOUCH_SCREEN_ASPECT_RATIO_AUTO;
        bEnableOnlineRegister    = TRUE;

        bRecordVideo             = FALSE;
        bDoubleScreenTouchMerge  = TRUE;
        //eLensFocalType         = E_LENS_FOCAL_NEAR;//缺省为"短焦"镜头。

        //fScreenDiagonalPhysicalLength = 80.0f*25.4f;//缺省为80英寸
        fScreenDiagonalPhysicalLength = DEFAULT_SCREEN_DIAGONAL_LENGTH;//缺省为80英寸
        //2017/03/14>>
        nMaxTimeInSearchDevice = 8000;

        //<<2017/03/14
        //缺省为OV7225的规格
        CMOSChipSpecification.pixel_size      = 0.006;//6um
        CMOSChipSpecification.width_in_mm     = 3.984;//3984um
        CMOSChipSpecification.height_in_mm    = 2.952;//2952um
        CMOSChipSpecification.width_in_pixel  = 656  ;//像素
        CMOSChipSpecification.height_in_pixel = 488  ;//像素

        //屏接的屏幕数目
        eScreenMode = EScreenModeUnknown;
        bSinglePointMode   = FALSE;
        bEnable4PointsCalibrate = FALSE;
		bModifyResolution = FALSE;

		bAirOperatePermission = FALSE;
		eClickMode = E_MODE_CLICK;

        circleCalibrateXScale = 100;
        circleCalibrateYScale = 100;
    }
};

//手指触控设备正常使用时的设定值
#define FINGER_TOUCH_NORMAL_USAGE_BRIGHTNESS  150

//笔触设备正常使用时的设定值
#define PEN_TOUCH_NORMAL_USAGE_BRIGHTNESS     25

struct NormalUsageSettings
{
    BYTE           cYThreshold            ;//亮度门限,范围(0~255)
    int            nDynamicMaskThresholdPercentage;//动态屏蔽门限占触发门限的百分比(50~100)
    int            nLightSpotMinimumWidth ;//最小光斑的宽度
    int            nLightSpotMinimumHeight;//最小光斑的高度

    //相机参数
    TVideoProcAmpProperty   cameraParams;

    //<<added by 15077726@qq.com
    //缺省值
    BYTE            cYThresholdDefault            ;//缺省亮度门限
    int             nDynamicMaskThresholdPercentageDefault;//动态屏蔽门限占触发门限的百分比(50~100)
    BYTE            nLightSpotMinimumWidthDefault ;//缺省的最小光斑的宽度
    BYTE            nLightSpotMinimumHeightDefault;//缺省的最小光斑的高度

    TVideoProcAmpProperty defaultParams;
    //>>

    NormalUsageSettings()
    {
        cYThreshold = 205;
        nDynamicMaskThresholdPercentage = 80;//动态屏蔽门限占触发门限的百分比(50~100)
        nLightSpotMinimumWidth = 2;
        nLightSpotMinimumHeight = 2;

        cameraParams.Prop_VideoProcAmp_Brightness           = FINGER_TOUCH_NORMAL_USAGE_BRIGHTNESS;//亮度
        cameraParams.Prop_VideoProcAmp_Contrast             = 255    ;//对比度
        cameraParams.Prop_VideoProcAmp_Hue                  = 0      ;//色调
        cameraParams.Prop_VideoProcAmp_Satuation            = 64     ;//饱和度
        cameraParams.Prop_VideoProcAmp_Sharpness            = 1      ;//锐利度
        cameraParams.Prop_VideoProcAmp_Gamma                = 1      ;//Gamma
        cameraParams.Prop_VideoProcAmp_ColorEnable          = 0      ;//颜色
        cameraParams.Prop_VideoProcAmp_WhiteBalance         = 5000   ;//白平衡
        cameraParams.Prop_VideoProcMap_BacklightCompensation= 2      ;//背光补偿
        cameraParams.Prop_VideoProcMap_Gain                 = 0x44AA ;//增益
		cameraParams.Prop_CameraControl_Exposure = -7                ;//曝光时间


        //缺省值初始化
        cYThresholdDefault = 205;
        nDynamicMaskThresholdPercentageDefault = 80;
        nLightSpotMinimumWidthDefault = 2;
        nLightSpotMinimumHeightDefault = 2;

        defaultParams.Prop_VideoProcAmp_Brightness           = FINGER_TOUCH_NORMAL_USAGE_BRIGHTNESS;//亮度
        defaultParams.Prop_VideoProcAmp_Contrast             = 255    ;//对比度
        defaultParams.Prop_VideoProcAmp_Hue                  = 0      ;//色调
        defaultParams.Prop_VideoProcAmp_Satuation            = 64     ;//饱和度
        defaultParams.Prop_VideoProcAmp_Sharpness            = 1      ;//锐利度
        defaultParams.Prop_VideoProcAmp_Gamma                = 1      ;//Gamma
        defaultParams.Prop_VideoProcAmp_ColorEnable          = 0      ;//颜色
        defaultParams.Prop_VideoProcAmp_WhiteBalance         = 5000   ;//白平衡
        defaultParams.Prop_VideoProcMap_BacklightCompensation= 2      ;//背光补偿
        defaultParams.Prop_VideoProcMap_Gain                 = 0x44AA ;//增益
		defaultParams.Prop_CameraControl_Exposure = -7;//曝光时间

    }
};

//安装调试时的摄像头参数
struct InstallTunningSettings
{
    //相机参数
    TVideoProcAmpProperty   cameraParams;

    //相机参数
    //<<added by 15077726@qq.com, 2015/11/6
    //<<
    TVideoProcAmpProperty   defaultParams;
    //>>


    InstallTunningSettings()
    {
        cameraParams.Prop_VideoProcAmp_Brightness           = 150    ;//亮度
        cameraParams.Prop_VideoProcAmp_Contrast             = 150    ;//对比度
        cameraParams.Prop_VideoProcAmp_Hue                  = 0      ;//色调
        cameraParams.Prop_VideoProcAmp_Satuation            = 64     ;//饱和度
        cameraParams.Prop_VideoProcAmp_Sharpness            = 1      ;//锐利度
        cameraParams.Prop_VideoProcAmp_Gamma                = 1      ;//Gamma
        cameraParams.Prop_VideoProcAmp_ColorEnable          = 0      ;//颜色
        cameraParams.Prop_VideoProcAmp_WhiteBalance         = 5000   ;//白平衡
        cameraParams.Prop_VideoProcMap_BacklightCompensation= 2      ;//背光补偿
        cameraParams.Prop_VideoProcMap_Gain                 = 0x33AA ;//增益
		cameraParams.Prop_CameraControl_Exposure = -7;

        //缺省值初始化
        defaultParams.Prop_VideoProcAmp_Brightness           = 150    ;//亮度
        defaultParams.Prop_VideoProcAmp_Contrast             = 150    ;//对比度
        defaultParams.Prop_VideoProcAmp_Hue                  = 0      ;//色调
        defaultParams.Prop_VideoProcAmp_Satuation            = 64     ;//饱和度
        defaultParams.Prop_VideoProcAmp_Sharpness            = 1      ;//锐利度
        defaultParams.Prop_VideoProcAmp_Gamma                = 1      ;//Gamma
        defaultParams.Prop_VideoProcAmp_ColorEnable          = 0      ;//颜色
        defaultParams.Prop_VideoProcAmp_WhiteBalance         = 5000   ;//白平衡
        defaultParams.Prop_VideoProcMap_BacklightCompensation= 2      ;//背光补
        defaultParams.Prop_VideoProcMap_Gain                 = 0x33AA ;//增益
		defaultParams.Prop_CameraControl_Exposure = -7;
    }

};


//激光器调试模式下的设定值
struct LaserTunningSettings
{
    //相机参数
    TVideoProcAmpProperty   cameraParams;

    //缺省参数
    //相机参数
    //<<added by 15077726@qq.com, 2015/11/6
    //<<
    TVideoProcAmpProperty   defaultParams;
    //>>

    LaserTunningSettings()
    {
        cameraParams.Prop_VideoProcAmp_Brightness           = 120    ;//亮度
        cameraParams.Prop_VideoProcAmp_Contrast             = 150    ;//对比度
        cameraParams.Prop_VideoProcAmp_Hue                  = 0      ;//色调
        cameraParams.Prop_VideoProcAmp_Satuation            = 64     ;//饱和度
        cameraParams.Prop_VideoProcAmp_Sharpness            = 1      ;//锐利度
        cameraParams.Prop_VideoProcAmp_Gamma                = 1      ;//Gamma
        cameraParams.Prop_VideoProcAmp_ColorEnable          = 0      ;//颜色
        cameraParams.Prop_VideoProcAmp_WhiteBalance         = 5000   ;//白平衡
        cameraParams.Prop_VideoProcMap_BacklightCompensation= 2      ;//背光补偿
        cameraParams.Prop_VideoProcMap_Gain                 = 0x44AA ;//增益
		cameraParams.Prop_CameraControl_Exposure = -7;

        defaultParams.Prop_VideoProcAmp_Brightness           = 120    ;//亮度
        defaultParams.Prop_VideoProcAmp_Contrast             = 150    ;//对比度
        defaultParams.Prop_VideoProcAmp_Hue                  = 0      ;//色调
        defaultParams.Prop_VideoProcAmp_Satuation            = 64     ;//饱和度
        defaultParams.Prop_VideoProcAmp_Sharpness            = 1      ;//锐利度
        defaultParams.Prop_VideoProcAmp_Gamma                = 1      ;//Gamma
        defaultParams.Prop_VideoProcAmp_ColorEnable          = 0      ;//颜色
        defaultParams.Prop_VideoProcAmp_WhiteBalance         = 5000   ;//白平衡
        defaultParams.Prop_VideoProcMap_BacklightCompensation= 2      ;//背光补偿 偿
        defaultParams.Prop_VideoProcMap_Gain                 = 0x44AA ;//增益
		defaultParams.Prop_CameraControl_Exposure = -7;

    }
};



//自动校正时的设定参数值
struct AutoCalibrateSettings
{
    //BYTE autoCalibrateExpectedBrightness; //自动校正亮度自动控制时期望的平均画面亮度(范围0~255)
    //BYTE autoCalibrateHilightGray; //自动校正图案中高亮块的灰度值(0~255)
    //相机参数
    TVideoProcAmpProperty   cameraParams;
    //自动校正时的画面参数,每一组参数对应一次校正尝试的画面参数
	AutoCalibrateImageParams   calibrateImageParams;

    //缺省参数
    //相机参数
    //<<added by 15077726@qq.com, 2015/11/6
    //<<
    TVideoProcAmpProperty     defaultParams;
	AutoCalibrateImageParams  defaultCalibrateImageParams;
    //>>

    AutoCalibrateSettings()
    {
        cameraParams.Prop_VideoProcAmp_Brightness           = 120    ;//亮度
        cameraParams.Prop_VideoProcAmp_Contrast             = 50     ;//对比度
        cameraParams.Prop_VideoProcAmp_Hue                  = 0      ;//色调
        cameraParams.Prop_VideoProcAmp_Satuation            = 64     ;//饱和度
        cameraParams.Prop_VideoProcAmp_Sharpness            = 1      ;//锐利度
        cameraParams.Prop_VideoProcAmp_Gamma                = 1      ;//Gamma
        cameraParams.Prop_VideoProcAmp_ColorEnable          = 0      ;//颜色
        cameraParams.Prop_VideoProcAmp_WhiteBalance         = 5000   ;//白平衡
        cameraParams.Prop_VideoProcMap_BacklightCompensation= 2      ;//背光补偿
        cameraParams.Prop_VideoProcMap_Gain                 = 0x33AA ;//增益
		cameraParams.Prop_CameraControl_Exposure = -7;

		calibrateImageParams.autoCalibrateExpectedBrightness = 100;
		calibrateImageParams.autoCalibrateHilightGray        = 255;//940 nm 激光器
        calibrateImageParams.autoCalibrateSpeed = 1;

        //缺省参数
        defaultParams.Prop_VideoProcAmp_Brightness           = 120    ;//亮度
        defaultParams.Prop_VideoProcAmp_Contrast             = 50     ;//对比度
        defaultParams.Prop_VideoProcAmp_Hue                  = 0      ;//色调
        defaultParams.Prop_VideoProcAmp_Satuation            = 64     ;//饱和度
        defaultParams.Prop_VideoProcAmp_Sharpness            = 1      ;//锐利度
        defaultParams.Prop_VideoProcAmp_Gamma                = 1      ;//Gamma
        defaultParams.Prop_VideoProcAmp_ColorEnable          = 0      ;//颜色
        defaultParams.Prop_VideoProcAmp_WhiteBalance         = 5000   ;//白平衡
        defaultParams.Prop_VideoProcMap_BacklightCompensation= 2      ;//背光补偿
        defaultParams.Prop_VideoProcMap_Gain                 = 0x33AA ;//增益
		defaultParams.Prop_CameraControl_Exposure = -7;

		defaultCalibrateImageParams.autoCalibrateExpectedBrightness = 100;
		defaultCalibrateImageParams.autoCalibrateHilightGray        = 255;//940 nm 激光?
        defaultCalibrateImageParams.autoCalibrateSpeed = 1;
    }

};
/////自动校正参数列表
typedef std::vector<AutoCalibrateSettings> AutoCalibrateSettingsList;



//自动屏蔽时的设定参数值
#define DEFAULT_MASK_AREA_EROSIE_SIZE 12
struct AutoMaskSettings
{
    BYTE cAutoMaskDetectThreshold;//自动屏蔽目标检测门限0~255, 值越小则屏蔽的区域越多
    int  nMaskAreaEroseSize      ;//自动屏蔽区域腐蚀半径

    //<<添加缺省值
    //added by 15077726@qq.com, 2015/11/6
    BYTE  cAutoMaskDetectThresholdDefault;
    int   nMaskAreaEroseSizeDefault;
    //>>

    AutoMaskSettings()
    {
        cAutoMaskDetectThreshold = 180;
        nMaskAreaEroseSize = DEFAULT_MASK_AREA_EROSIE_SIZE;

        cAutoMaskDetectThresholdDefault = 180;
        nMaskAreaEroseSizeDefault = DEFAULT_MASK_AREA_EROSIE_SIZE;

    }
};

//手动校正时的设定参数值
struct ManualCalibrateSettings
{
    int nPtNumInEachRow;//手动校正每行的校正点个数(=列数),最小值=2
    int nPtNumInEachCol;//手动校正每列的校正点个数(=行数),最小值=2
    ManualCalibrateSettings()
    {
        nPtNumInEachRow = 5;
        nPtNumInEachCol = 5;
    }
};


//自动校正时的设定参数值
struct AutoCalibratePatternSettings
{
    int nPatternRowCount;//自动校图案的行数
    AutoCalibratePatternSettings()
    {
        nPatternRowCount = 5;
    }
};


//高级设定
struct TAdvancedSettings
{
    int nAutoMaskDilateRadius  ;//自动屏蔽膨胀半径
    int nXDeviationCompensationValue;//X方向偏差修正值
    int nYDeviationCompensationValue;//Y方向偏差修正值

    int nSpotProportion            ;//光斑响应的比例
    int nMultEraser                ;//板擦的响应倍数
    int nFixedBlobSetTime          ;//定义为固定光斑的设置时间

	BOOL  bEnableStrokeInterpolate; //使能插值标志
	EDeviceTouchType m_eTouchType;  //触控类型
   //BOOL bGuestureRecognition     ;//手势识别检测功能使能标志
	BOOL bIsRearProjection;         //是否开启背投模式
	BOOL bIsDynamicMaskFrame;      //是否开启动态屏蔽功能
	BOOL bIsAntiJamming;           //是否开启抗干扰功能
	BOOL bIsOnLineScreenArea;      //是否开启手动绘制的静态屏蔽图
	BOOL bDisableReflectionSpot;   //反射点是否响应
	int  nSmoothCoefficient;       //设置平滑系数

    BOOL bUsingScreenPhysicalDimensions;//使用屏幕物理尺寸标志(Yes/No);
    LONG nScreenWidthInmm;//屏幕宽度(单位:mm)
    LONG nScreenHeightInmm;//屏幕高度(单位:mm)

    TAdvancedSettings()
    {
        nAutoMaskDilateRadius        = 1             ;
        nXDeviationCompensationValue = 0             ;
        nYDeviationCompensationValue = 0             ;
        nSpotProportion              = 30            ;
        nMultEraser                  = 3             ;
        nFixedBlobSetTime            = 5             ;
		bEnableStrokeInterpolate     = FALSE         ;
		m_eTouchType = E_DEVICE_PEN_TOUCH_WHITEBOARD ;
		bIsRearProjection = FALSE                    ;
		bIsDynamicMaskFrame = FALSE;
		bIsAntiJamming = FALSE;
		bIsOnLineScreenArea = FALSE;
		bDisableReflectionSpot = FALSE;
		nSmoothCoefficient = 0;

        bUsingScreenPhysicalDimensions = false;
        nScreenWidthInmm = 1920;
        nScreenHeightInmm = 1080;
    }
};



//镜头配置参数, 每个镜头关联以下参数
//1.安装调试时的参数
//2.激光器打板调试时.参数
//3.自动校正时的参数设置
//4.自动屏蔽时的参数设置
//5.手动校正时的参数
//6.手指触控模式下正常使用时的参数
//7.笔触控模式下的正常使用时的参数设定。

struct TLensConfig
{
    TInternalAndSymmetricDistortParams   lensInternalAndSymmetricDistortParams;//镜头内部和对称畸变参数
    BOOL                    bInternalAndSymmetricDistortParamsIsValid;         //镜头内部和对称畸变参数有效标志
    TLensSpecification      lensSpecification                        ;         //镜头规格参数
    TAutoCalibCompCoefs     autoCalibCompCoefs                       ;         //自动校正补偿系数
    NormalUsageSettings     normalUsageSettings_FingerTouchWhiteBoard;         //手指触控电子白板模式下的正常使用时的参数设定
    NormalUsageSettings     normalUsageSettings_PenTouchWhiteBoard   ;         //笔触控电子白板模式下的正常使用时的参数设定
	NormalUsageSettings     normalUsageSettings_FingerTouchControl   ;         //手指触控模式下的正常使用时的参数设定
	NormalUsageSettings     normalUsageSettings_PalmTouchControl     ;         //手掌触控模式下的正常使用时的参数设定

    InstallTunningSettings  installTunningSettings                   ;         //安装调试时的参数设定

    LaserTunningSettings    laserTunningSettings_WhiteBoard          ;         //电子白板激光器打板调试时的参数设定
	LaserTunningSettings    laserTunningSettings_FingerTouchControl  ;         //手指触控激光器打板调试时的参数设定
	LaserTunningSettings    laserTunningSettings_PalmTouchControl    ;         //手掌互动激光器打板调试时的参数设定

	AutoCalibrateSettingsList   autoCalibrateSettingsList            ;         //自动校正时的参数设置
    AutoMaskSettings            autoMaskSettings                     ;         //自动屏蔽时的参数设置


    //引导框
	RectF  rcGuideRectangle     ;//引导框位置
	float   Referwidth         ;//设置引导框所参考的分辨率(归一化后的值是1)
	float   ReferHeight        ;
    DWORD dwGuideRectangleColor;//引导框颜色
    BOOL  bRectangleVisible    ;//引导框可见标志

	TLensConfig()
	{
		bInternalAndSymmetricDistortParamsIsValid = FALSE;

		//2017/11/16, 新增引导框信息
		//<<
		float image_Default_Width = 1;
		float image_Default_Height = 1;
		rcGuideRectangle.left   = 0.2f;
		rcGuideRectangle.right  = 0.8f;
		rcGuideRectangle.top    = 0.2f;
		rcGuideRectangle.bottom = 0.8f;

		Referwidth = image_Default_Width;
		ReferHeight = image_Default_Height;

		dwGuideRectangleColor = 0xFF0000;//RGB
		bRectangleVisible = TRUE;
		//>>
		this->autoCalibrateSettingsList.resize(3);	
		{
			AutoCalibrateSettings &autocalibratesettings = autoCalibrateSettingsList[0];

			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Brightness = 120;//亮度
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Contrast = 50;//对比度
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
			autocalibratesettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
			autocalibratesettings.cameraParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
			autocalibratesettings.cameraParams.Prop_CameraControl_Exposure = -7;

			autocalibratesettings.calibrateImageParams.autoCalibrateExpectedBrightness = 100;
			autocalibratesettings.calibrateImageParams.autoCalibrateHilightGray = 255;//940 nm 激光器
		}
		{
			AutoCalibrateSettings &autocalibratesettings = autoCalibrateSettingsList[1];

			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Brightness = 120;//亮度
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Contrast = 50;//对比度
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
			autocalibratesettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
			autocalibratesettings.cameraParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
			autocalibratesettings.cameraParams.Prop_CameraControl_Exposure = -7;

			autocalibratesettings.calibrateImageParams.autoCalibrateExpectedBrightness = 70;
			autocalibratesettings.calibrateImageParams.autoCalibrateHilightGray = 255;//940 nm 激光器
		}
		{
			AutoCalibrateSettings &autocalibratesettings = autoCalibrateSettingsList[2];

			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Brightness = 120;//亮度
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Contrast = 50;//对比度
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			autocalibratesettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
			autocalibratesettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
			autocalibratesettings.cameraParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
			autocalibratesettings.cameraParams.Prop_CameraControl_Exposure = -7;

			autocalibratesettings.calibrateImageParams.autoCalibrateExpectedBrightness = 30;
			autocalibratesettings.calibrateImageParams.autoCalibrateHilightGray = 255;//940 nm 激光器
		}

		lensSpecification.backFocalLength = 2.7;
		lensSpecification.FOV_horz = 146;
		lensSpecification.FOV_vert = 105;
		lensSpecification.FOV_diagonal = 175;
		lensSpecification.throwRatio = 0.15;

    }
};

//镜头类型
enum ELensType
{
    E_LENS_TR_0_DOT_15   = 0,//投射比0.15
    E_LENS_TR_0_DOT_19   = 1,//投射比0.19
    E_LENS_TR_0_DOT_21   = 2,//投射比0.21
    E_LENS_TR_0_DOT_25   = 3,//投射比0.25
    E_LENS_TR_0_DOT_28   = 4,//投射比0.28
    E_LENS_TR_0_DOT_34   = 5,//投射比0.34
    E_LENS_TR_0_DOT_55   = 6,//投射比0.55
    E_LENS_TR_0_DOT_70   = 7,//投射比0.70
    E_LENS_TR_0_DOT_86   = 8,//投射比0.86
    E_LENS_TR_1_DOT_34   = 9,//投射比1.36
    E_LENS_TYPE_COUNT    = 10
};

//投射比列表

_declspec(selectany) extern const double TRHOW_RATIO_LIST[] =
{
    0.15,
    0.19,
    0.21,
    0.25,
    0.28,
    0.34,
    0.55,
    0.70,
    0.86,
    1.36,
};

//屏幕区域类型
enum EMonitorAreaType
{
    E_MONITOR_AREA_TYPE_FULLSCREEN = 0,//全屏
    E_MONITOR_AREA_TYPE_LEFT_HALF  = 1,//左半屏
    E_MONITOR_AREA_TYPE_RIGHT_HALF = 2 //右半部
};

struct TSensorModeConfig
{
	TAdvancedSettings       advanceSettings;//高级设置
	TLensConfig             lensConfigs[E_CAMERA_MODEL_COUNT][E_LENS_TYPE_COUNT];
	ManualCalibrateSettings manualCalibrateSetting;//手动校正时的参数设置
    AutoCalibratePatternSettings autoCalibratePatternSettings;//自动校正图案的参数设置
	TCalibParams            calibParam;//校正参数

	TSensorModeConfig()
	{
	    calibParam.szImage.cx = 640;
	    calibParam.szImage.cy = 480;

		 //投射比为0.15的超短焦镜头的参数配置
		 //==================================
		{
			TLensConfig&  lens = lensConfigs[0][E_LENS_TR_0_DOT_15];
			//电子白板正常手指触控模式下的使用参数
			{
				NormalUsageSettings& normalUsageSettings = lens.normalUsageSettings_FingerTouchWhiteBoard;
				normalUsageSettings.cYThreshold = 205;
				normalUsageSettings.nLightSpotMinimumWidth = 2;
				normalUsageSettings.nLightSpotMinimumHeight = 2;

				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Brightness = 150;//亮度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Contrast = 255;//对比度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Gamma = 2;//Gamma     ★
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				normalUsageSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.cameraParams.Prop_VideoProcMap_Gain = 0x44AA;//增益
				normalUsageSettings.cameraParams.Prop_CameraControl_Exposure = -7;

				//缺省值初始化
				normalUsageSettings.cYThresholdDefault = 205;
				normalUsageSettings.nLightSpotMinimumWidthDefault = 2;
				normalUsageSettings.nLightSpotMinimumHeightDefault = 2;

				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Brightness = 150;//亮度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Contrast = 255;//对比度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Gamma = 2;//Gamma    ★
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				normalUsageSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.defaultParams.Prop_VideoProcMap_Gain = 0x44AA;//增益
				normalUsageSettings.defaultParams.Prop_CameraControl_Exposure = -7;
			}
			//电子白板正常笔触触控模式下的使用参数
			{

				NormalUsageSettings& normalUsageSettings = lens.normalUsageSettings_PenTouchWhiteBoard;
				normalUsageSettings.cYThreshold = 225;
				normalUsageSettings.nLightSpotMinimumWidth = 2;
				normalUsageSettings.nLightSpotMinimumHeight = 2;

				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Brightness = 60;//亮度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Contrast = 255;//对比度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				normalUsageSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.cameraParams.Prop_VideoProcMap_Gain = 0x44AA;//增益
				normalUsageSettings.cameraParams.Prop_CameraControl_Exposure = -7;

				//缺省值初始化
				normalUsageSettings.cYThresholdDefault = 225;
				normalUsageSettings.nLightSpotMinimumWidthDefault = 2;
				normalUsageSettings.nLightSpotMinimumHeightDefault = 2;

				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Brightness = 60;//亮度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Contrast = 255;//对比度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				normalUsageSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.defaultParams.Prop_VideoProcMap_Gain = 0x44AA;//增益
				normalUsageSettings.defaultParams.Prop_CameraControl_Exposure = -7;
			}
			//手指触控正常模式下的使用参数
			{

				NormalUsageSettings& normalUsageSettings = lens.normalUsageSettings_FingerTouchControl;
				normalUsageSettings.cYThreshold = 225;
				normalUsageSettings.nLightSpotMinimumWidth = 2;
				normalUsageSettings.nLightSpotMinimumHeight = 2;

				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				normalUsageSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
				normalUsageSettings.cameraParams.Prop_CameraControl_Exposure = -7;

				//缺省值初始化
				normalUsageSettings.cYThresholdDefault = 225;
				normalUsageSettings.nLightSpotMinimumWidthDefault = 2;
				normalUsageSettings.nLightSpotMinimumHeightDefault = 2;

				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Contrast = 46;//对比度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				normalUsageSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
				normalUsageSettings.defaultParams.Prop_CameraControl_Exposure = -7;
			}
			//手掌互动正常模式下的使用参数
			{

				NormalUsageSettings& normalUsageSettings = lens.normalUsageSettings_PalmTouchControl;
				normalUsageSettings.cYThreshold = 225;
				normalUsageSettings.nLightSpotMinimumWidth = 2;
				normalUsageSettings.nLightSpotMinimumHeight = 2;

				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				normalUsageSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
				normalUsageSettings.cameraParams.Prop_CameraControl_Exposure = -7;

				//缺省值初始化
				normalUsageSettings.cYThresholdDefault = 225;
				normalUsageSettings.nLightSpotMinimumWidthDefault = 2;
				normalUsageSettings.nLightSpotMinimumHeightDefault = 2;

				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Contrast = 46;//对比度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				normalUsageSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
				normalUsageSettings.defaultParams.Prop_CameraControl_Exposure = -7;
			}


			//安装调试模式下的参数
			{
				InstallTunningSettings& installTunningSettings = lens.installTunningSettings;
				installTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness = 150;//亮度
				installTunningSettings.cameraParams.Prop_VideoProcAmp_Contrast = 150;//对比度
				installTunningSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				installTunningSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				installTunningSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				installTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				installTunningSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				installTunningSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				installTunningSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				installTunningSettings.cameraParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
				installTunningSettings.cameraParams.Prop_CameraControl_Exposure = -7;

																					//缺省值初始化
				installTunningSettings.defaultParams.Prop_VideoProcAmp_Brightness = 150;//亮度
				installTunningSettings.defaultParams.Prop_VideoProcAmp_Contrast = 150;//对比度
				installTunningSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				installTunningSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				installTunningSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				installTunningSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				installTunningSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				installTunningSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				installTunningSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补
				installTunningSettings.defaultParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
				installTunningSettings.defaultParams.Prop_CameraControl_Exposure = -7;
			}

			//电子白板激光器调试模式参数
			{
				LaserTunningSettings& laserTunningSettings = lens.laserTunningSettings_WhiteBoard;

				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness = 150;//亮度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Contrast = 150;//对比度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma = 2;//Gamma
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				laserTunningSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				laserTunningSettings.cameraParams.Prop_VideoProcMap_Gain = 0x44AA;//增益
				laserTunningSettings.cameraParams.Prop_CameraControl_Exposure = -7;

				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Brightness = 150;//亮度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Contrast = 150;//对比度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Gamma = 2;//Gamma
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				laserTunningSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿 偿
				laserTunningSettings.defaultParams.Prop_VideoProcMap_Gain = 0x44AA;//增益
				laserTunningSettings.defaultParams.Prop_CameraControl_Exposure = -7;
			}

			//手指触控激光器调试模式参数
			{
				LaserTunningSettings& laserTunningSettings = lens.laserTunningSettings_FingerTouchControl;

				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma = 2;//Gamma
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				laserTunningSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				laserTunningSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
				laserTunningSettings.cameraParams.Prop_CameraControl_Exposure = -7;

				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Gamma = 2;//Gamma
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				laserTunningSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿 偿
				laserTunningSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
				laserTunningSettings.defaultParams.Prop_CameraControl_Exposure = -7;
			}

			//手掌互动激光器调试模式参数
			{
				LaserTunningSettings& laserTunningSettings = lens.laserTunningSettings_PalmTouchControl;

				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness = 150;//亮度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Contrast = 150;//对比度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma = 2;//Gamma
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				laserTunningSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				laserTunningSettings.cameraParams.Prop_VideoProcMap_Gain = 0x44AA;//增益
				laserTunningSettings.cameraParams.Prop_CameraControl_Exposure = -7;

				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Brightness = 150;//亮度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Contrast = 150;//对比度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Gamma = 2;//Gamma
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				laserTunningSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿 偿
				laserTunningSettings.defaultParams.Prop_VideoProcMap_Gain = 0x44AA;//增益
				laserTunningSettings.defaultParams.Prop_CameraControl_Exposure = -7;
			}

			//自动校正参数列表
			{
				AutoCalibrateSettingsList& autoCalibrateSettingsList = lens.autoCalibrateSettingsList;
				{
					//第一组自动校正参数
					AutoCalibrateSettings &autoCalibrateSettings = autoCalibrateSettingsList[0];

					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Brightness = 120;//亮度
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Contrast = 50;//对比度
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
					autoCalibrateSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
					autoCalibrateSettings.cameraParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
					autoCalibrateSettings.cameraParams.Prop_CameraControl_Exposure = -7;//曝光时间

																						   //第一组,对应较亮的光线环境
					autoCalibrateSettings.calibrateImageParams.autoCalibrateExpectedBrightness = 100;
					autoCalibrateSettings.calibrateImageParams.autoCalibrateHilightGray = 255;//

					//缺省参数
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Brightness = 120;//亮度
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Contrast = 50;//对比度
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
					autoCalibrateSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
					autoCalibrateSettings.defaultParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
					autoCalibrateSettings.defaultParams.Prop_CameraControl_Exposure = -7; //曝光时间

					//第一组,对应较亮的光线环境
					autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateExpectedBrightness = 100;
					autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateHilightGray = 255;//
				}
				{
					//第二组自动校正参数
					AutoCalibrateSettings &autoCalibrateSettings = autoCalibrateSettingsList[1];

					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Brightness = 120;//亮度
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Contrast = 50;//对比度
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
					autoCalibrateSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
					autoCalibrateSettings.cameraParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
					autoCalibrateSettings.cameraParams.Prop_CameraControl_Exposure = -7;//曝光时间

					//第二组,对应适当的光线环境
					autoCalibrateSettings.calibrateImageParams.autoCalibrateExpectedBrightness = 100;
					autoCalibrateSettings.calibrateImageParams.autoCalibrateHilightGray = 180;

					//缺省参数
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Brightness = 120;//亮度
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Contrast = 50;//对比度
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
					autoCalibrateSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
					autoCalibrateSettings.defaultParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
					autoCalibrateSettings.defaultParams.Prop_CameraControl_Exposure = -7; //曝光时间

					autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateExpectedBrightness = 100;
					autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateHilightGray = 180;
				}
				{
					//第三组自动校正参数
					AutoCalibrateSettings &autoCalibrateSettings = autoCalibrateSettingsList[2];

					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Brightness = 120;//亮度
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Contrast = 50;//对比度
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
					autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
					autoCalibrateSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
					autoCalibrateSettings.cameraParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
					autoCalibrateSettings.cameraParams.Prop_CameraControl_Exposure = -7;//曝光时间

					//第三组, 对应较暗的光线环境
					autoCalibrateSettings.calibrateImageParams.autoCalibrateExpectedBrightness = 70;
					autoCalibrateSettings.calibrateImageParams.autoCalibrateHilightGray = 120;

					//缺省参数
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Brightness = 120;//亮度
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Contrast = 50;//对比度
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
					autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
					autoCalibrateSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
					autoCalibrateSettings.defaultParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
					autoCalibrateSettings.defaultParams.Prop_CameraControl_Exposure = -7; //曝光时间

					//第三组, 对应较暗的光线环境
					autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateExpectedBrightness = 70;
					autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateHilightGray = 120;
				}
			}
			//自动屏蔽参数
			{
				AutoMaskSettings& autoMaskSettings = lens.autoMaskSettings;
				autoMaskSettings.cAutoMaskDetectThreshold = 180;
				autoMaskSettings.nMaskAreaEroseSize = DEFAULT_MASK_AREA_EROSIE_SIZE;

				autoMaskSettings.cAutoMaskDetectThresholdDefault = 180;
				autoMaskSettings.nMaskAreaEroseSizeDefault = DEFAULT_MASK_AREA_EROSIE_SIZE;
			}
			//镜头规格
			{
				TLensSpecification& lensSpecification = lens.lensSpecification;
				lensSpecification.backFocalLength = 2.7;//像方焦距,   单位:mm
				lensSpecification.FOV_horz = 146;//水平视角，  单位:度
				lensSpecification.FOV_vert = 105;//垂直视角，  单位:度
				lensSpecification.FOV_diagonal = 175;//对角线视角，单位:度
				lensSpecification.throwRatio = 0.15;//投射比
			}

			//自动校正补偿系数
			{
				TAutoCalibCompCoefs& autoCalibCompCoefs = lens.autoCalibCompCoefs;
#ifdef CCD_ROTATE_90
				//u0 = 323.727170, v0 = 221.434748, k1 = 7.331750, k2 = -10.172566, k3 = 20.331750
				//autoCalibCompCoefs.u0 = 323.727170 / 640.0;
				//autoCalibCompCoefs.v0 = 221.434748 / 480.0;

				//autoCalibCompCoefs.k[0] = 7.331750;
				//autoCalibCompCoefs.k[1] = -10.172566;
				//autoCalibCompCoefs.k[2] = 20.331750;


				//u0 = 326.159754, v0 = 212.770324, k1 = 8.035979, k2 = -18.892648, k3 = 44.544014
				autoCalibCompCoefs.u0 = 326.159754 / 640.0;
				autoCalibCompCoefs.v0 = 212.770324 / 480.0;

				autoCalibCompCoefs.k[0] = 8.035979;
				autoCalibCompCoefs.k[1] = -18.892648;
				autoCalibCompCoefs.k[2] = 44.544014;
#else

                 autoCalibCompCoefs.u0 = 321.239992 / 640.0;
                 autoCalibCompCoefs.v0 = 224.913703 / 480.0;

                 autoCalibCompCoefs.k[0] = 5.012811;
                 autoCalibCompCoefs.k[1] = 12.998592;
                 autoCalibCompCoefs.k[2] = -18.748065;
//2018/01/11
#endif
		}
			/*
			lens.bInternalAndSymmetricDistortParamsIsValid = TRUE;

			//相机内部参数
			//mu = 1.22754, mv = 1.22628
			//u0 = 318.521, v0 = 247.995
			//k(1) = 202.660577, k(2) = -1.567474, k(3) = 4.009577, k(4) = -3.274776, k(5) = 0.924704
			lens.lensInternalAndSymmetricDistortParams.mu = 1.22754;
			lens.lensInternalAndSymmetricDistortParams.mv = 1.22628;
			lens.lensInternalAndSymmetricDistortParams.u0 = 318.521;
			lens.lensInternalAndSymmetricDistortParams.v0 = 247.995;

			//相机对称畸变参数
			lens.lensInternalAndSymmetricDistortParams.radialDistrt[0] = 202.660577;
			lens.lensInternalAndSymmetricDistortParams.radialDistrt[1] = -1.567474;
			lens.lensInternalAndSymmetricDistortParams.radialDistrt[2] = 4.009577;
			lens.lensInternalAndSymmetricDistortParams.radialDistrt[3] = -3.274776;
			lens.lensInternalAndSymmetricDistortParams.radialDistrt[4] = 0.924704;
            */

		}//投射比为0.15镜头参数
		{
		    TLensConfig&  lens = lensConfigs[1][E_LENS_TR_0_DOT_15];
		    //电子白板正常手指触控模式下的使用参数
		    {
			    NormalUsageSettings& normalUsageSettings = lens.normalUsageSettings_FingerTouchWhiteBoard;
			    normalUsageSettings.cYThreshold = 205;
			    normalUsageSettings.nLightSpotMinimumWidth = 2;
			    normalUsageSettings.nLightSpotMinimumHeight = 2;

			    normalUsageSettings.cameraParams.Prop_VideoProcAmp_Brightness = 120;//亮度
			    normalUsageSettings.cameraParams.Prop_VideoProcAmp_Contrast = 255;//对比度
			    normalUsageSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
			    normalUsageSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			    normalUsageSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			    normalUsageSettings.cameraParams.Prop_VideoProcAmp_Gamma = 2;//Gamma    ★
			    normalUsageSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			    normalUsageSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
			    normalUsageSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
			    normalUsageSettings.cameraParams.Prop_VideoProcMap_Gain = 0x44AA;//增益
			    normalUsageSettings.cameraParams.Prop_CameraControl_Exposure = -7;


			    //缺省值初始化
			    normalUsageSettings.cYThresholdDefault = 205;
			    normalUsageSettings.nLightSpotMinimumWidthDefault = 2;
			    normalUsageSettings.nLightSpotMinimumHeightDefault = 2;

			    normalUsageSettings.defaultParams.Prop_VideoProcAmp_Brightness = 120;//亮度
			    normalUsageSettings.defaultParams.Prop_VideoProcAmp_Contrast = 255;//对比度
			    normalUsageSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
			    normalUsageSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			    normalUsageSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			    normalUsageSettings.defaultParams.Prop_VideoProcAmp_Gamma = 2;//Gamma    ★
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				normalUsageSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.defaultParams.Prop_VideoProcMap_Gain = 0x44AA;//增益
				normalUsageSettings.defaultParams.Prop_CameraControl_Exposure = -7;
			}

		    //电子白板正常笔触触控模式下的使用参数
		    {
				NormalUsageSettings& normalUsageSettings = lens.normalUsageSettings_PenTouchWhiteBoard;
				normalUsageSettings.cYThreshold = 205;
				normalUsageSettings.nLightSpotMinimumWidth = 2;
				normalUsageSettings.nLightSpotMinimumHeight = 2;

				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Brightness = 60;//亮度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Contrast = 255;//对比度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				normalUsageSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.cameraParams.Prop_VideoProcMap_Gain = 0x44AA;//增益
				normalUsageSettings.cameraParams.Prop_CameraControl_Exposure = -7;


				//缺省值初始化
				normalUsageSettings.cYThresholdDefault = 205;
				normalUsageSettings.nLightSpotMinimumWidthDefault = 2;
				normalUsageSettings.nLightSpotMinimumHeightDefault = 2;

				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Brightness = 60;//亮度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Contrast = 255;//对比度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				normalUsageSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.defaultParams.Prop_VideoProcMap_Gain = 0x44AA;//增益
				normalUsageSettings.defaultParams.Prop_CameraControl_Exposure = -7;

			}

		    //手指触控正常模式下的使用参数
		    {
				NormalUsageSettings& normalUsageSettings = lens.normalUsageSettings_FingerTouchControl;
				normalUsageSettings.cYThreshold = 205;
				normalUsageSettings.nLightSpotMinimumWidth = 2;
				normalUsageSettings.nLightSpotMinimumHeight = 2;

				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				normalUsageSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
				normalUsageSettings.cameraParams.Prop_CameraControl_Exposure = -7;


				//缺省值初始化
				normalUsageSettings.cYThresholdDefault = 205;
				normalUsageSettings.nLightSpotMinimumWidthDefault = 2;
				normalUsageSettings.nLightSpotMinimumHeightDefault = 2;

				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				normalUsageSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
				normalUsageSettings.defaultParams.Prop_CameraControl_Exposure = -7;
			}

			//手掌互动正常模式下的使用参数
		    {
				NormalUsageSettings& normalUsageSettings = lens.normalUsageSettings_PalmTouchControl;
				normalUsageSettings.cYThreshold = 205;
				normalUsageSettings.nLightSpotMinimumWidth = 2;
				normalUsageSettings.nLightSpotMinimumHeight = 2;

				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				normalUsageSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
				normalUsageSettings.cameraParams.Prop_CameraControl_Exposure = -7;


				//缺省值初始化
				normalUsageSettings.cYThresholdDefault = 205;
				normalUsageSettings.nLightSpotMinimumWidthDefault = 2;
				normalUsageSettings.nLightSpotMinimumHeightDefault = 2;

				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				normalUsageSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
				normalUsageSettings.defaultParams.Prop_CameraControl_Exposure = -7;

		    }

		    //安装调试模式下的参数
		    {
				InstallTunningSettings& installTunningSettings = lens.installTunningSettings;
				installTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness = 150;//亮度
				installTunningSettings.cameraParams.Prop_VideoProcAmp_Contrast = 150;//对比度
				installTunningSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				installTunningSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				installTunningSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				installTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				installTunningSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				installTunningSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				installTunningSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				installTunningSettings.cameraParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
				installTunningSettings.cameraParams.Prop_CameraControl_Exposure = -7;

				//缺省值初始化
				installTunningSettings.defaultParams.Prop_VideoProcAmp_Brightness = 150;//亮度
				installTunningSettings.defaultParams.Prop_VideoProcAmp_Contrast = 150;//对比度
				installTunningSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				installTunningSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				installTunningSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				installTunningSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				installTunningSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				installTunningSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				installTunningSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补
				installTunningSettings.defaultParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
				installTunningSettings.defaultParams.Prop_CameraControl_Exposure = -7;
		   }

		    //电子白板激光器调试模式参数
		    {
				LaserTunningSettings& laserTunningSettings = lens.laserTunningSettings_WhiteBoard;

				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness = 150;//亮度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Contrast = 150;//对比度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma = 2;//Gamma    ★
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				laserTunningSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				laserTunningSettings.cameraParams.Prop_VideoProcMap_Gain = 0x44AA;//增益
				laserTunningSettings.cameraParams.Prop_CameraControl_Exposure = -7;


				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Brightness = 150;//亮度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Contrast = 150;//对比度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Gamma = 2;//Gamma    ★
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				laserTunningSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿 偿
				laserTunningSettings.defaultParams.Prop_VideoProcMap_Gain = 0x44AA;//增益
				laserTunningSettings.defaultParams.Prop_CameraControl_Exposure = -7;
		  }

		    //手指触控激光器调试模式参数
		    {
				LaserTunningSettings& laserTunningSettings = lens.laserTunningSettings_FingerTouchControl;

				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Contrast = 0;//对比度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma = 2;//Gamma    ★
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				laserTunningSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				laserTunningSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				laserTunningSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
				laserTunningSettings.cameraParams.Prop_CameraControl_Exposure = -7;


				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_Gamma = 2;//Gamma    ★
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				laserTunningSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				laserTunningSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿 偿
				laserTunningSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
				laserTunningSettings.defaultParams.Prop_CameraControl_Exposure = -7;
		   }

		    //手掌互动激光器调试模式参数
		    {
			LaserTunningSettings& laserTunningSettings = lens.laserTunningSettings_PalmTouchControl;

			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Contrast = 0;//对比度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma = 2;//Gamma    ★
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			laserTunningSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
			laserTunningSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
			laserTunningSettings.cameraParams.Prop_CameraControl_Exposure = -7;


			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Gamma = 2;//Gamma    ★
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			laserTunningSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿 偿
			laserTunningSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
			laserTunningSettings.defaultParams.Prop_CameraControl_Exposure = -7;
		}
		    {
			   AutoCalibrateSettingsList& autoCalibrateSettingsList = lens.autoCalibrateSettingsList;
		    	{
				//第一组自动校正参数
				AutoCalibrateSettings &autoCalibrateSettings = autoCalibrateSettingsList[0];

				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Brightness = 120;//亮度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Contrast = 50;//对比度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				autoCalibrateSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				autoCalibrateSettings.cameraParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
				autoCalibrateSettings.cameraParams.Prop_CameraControl_Exposure = -7;//曝光时间

																					//第一组,对应较亮的光线环境
				autoCalibrateSettings.calibrateImageParams.autoCalibrateExpectedBrightness = 100;
				autoCalibrateSettings.calibrateImageParams.autoCalibrateHilightGray = 255;//

																						  //缺省参数
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Brightness = 120;//亮度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Contrast = 50;//对比度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				autoCalibrateSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				autoCalibrateSettings.defaultParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
				autoCalibrateSettings.defaultParams.Prop_CameraControl_Exposure = -7; //曝光时间

																					  //第一组,对应较亮的光线环境
				autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateExpectedBrightness = 100;
				autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateHilightGray = 255;//
			}
			{
				//第二组自动校正参数
				AutoCalibrateSettings &autoCalibrateSettings = autoCalibrateSettingsList[1];

				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Brightness = 120;//亮度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Contrast = 50;//对比度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				autoCalibrateSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				autoCalibrateSettings.cameraParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
				autoCalibrateSettings.cameraParams.Prop_CameraControl_Exposure = -7;//曝光时间

																					//第二组,对应适当的光线环境
				autoCalibrateSettings.calibrateImageParams.autoCalibrateExpectedBrightness = 100;
				autoCalibrateSettings.calibrateImageParams.autoCalibrateHilightGray = 180;

				//缺省参数
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Brightness = 120;//亮度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Contrast = 50;//对比度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				autoCalibrateSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				autoCalibrateSettings.defaultParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
				autoCalibrateSettings.defaultParams.Prop_CameraControl_Exposure = -7; //曝光时间

				autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateExpectedBrightness = 100;
				autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateHilightGray = 180;
			}
			{
				//第三组自动校正参数
				AutoCalibrateSettings &autoCalibrateSettings = autoCalibrateSettingsList[2];

				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Brightness = 120;//亮度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Contrast = 50;//对比度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				autoCalibrateSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				autoCalibrateSettings.cameraParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
				autoCalibrateSettings.cameraParams.Prop_CameraControl_Exposure = -7;//曝光时间

																	//第三组, 对应较暗的光线环境
				autoCalibrateSettings.calibrateImageParams.autoCalibrateExpectedBrightness = 70;
				autoCalibrateSettings.calibrateImageParams.autoCalibrateHilightGray = 120;

				//缺省参数
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Brightness = 120;//亮度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Contrast = 50;//对比度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 5000;//白平衡
				autoCalibrateSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				autoCalibrateSettings.defaultParams.Prop_VideoProcMap_Gain = 0x33AA;//增益
				autoCalibrateSettings.defaultParams.Prop_CameraControl_Exposure = -7; //曝光时间

																					  //第三组, 对应较暗的光线环境
				autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateExpectedBrightness = 70;
				autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateHilightGray = 120;
			}
		}

		//自动屏蔽参数
		{
			AutoMaskSettings& autoMaskSettings = lens.autoMaskSettings;
			autoMaskSettings.cAutoMaskDetectThreshold = 180;
			autoMaskSettings.nMaskAreaEroseSize = DEFAULT_MASK_AREA_EROSIE_SIZE;

			autoMaskSettings.cAutoMaskDetectThresholdDefault = 180;
			autoMaskSettings.nMaskAreaEroseSizeDefault = DEFAULT_MASK_AREA_EROSIE_SIZE;
		 }
		//镜头规格
		{
			TLensSpecification& lensSpecification = lens.lensSpecification;
			lensSpecification.backFocalLength = 2.7;//像方焦距,   单位:mm
			lensSpecification.FOV_horz = 146;//水平视角，  单位:度
			lensSpecification.FOV_vert = 105;//垂直视角，  单位:度
			lensSpecification.FOV_diagonal = 175;//对角线视角，单位:度
			lensSpecification.throwRatio = 0.15;//投射比
		}

		//自动校正补偿系数
		{
			TAutoCalibCompCoefs& autoCalibCompCoefs = lens.autoCalibCompCoefs;
#ifdef CCD_ROTATE_90
			//u0 = 323.727170, v0 = 221.434748, k1 = 7.331750, k2 = -10.172566, k3 = 20.331750
			//autoCalibCompCoefs.u0 = 323.727170 / 640.0;
			//autoCalibCompCoefs.v0 = 221.434748 / 480.0;

			//autoCalibCompCoefs.k[0] = 7.331750;
			//autoCalibCompCoefs.k[1] = -10.172566;
			//autoCalibCompCoefs.k[2] = 20.331750;


			//u0 = 326.159754, v0 = 212.770324, k1 = 8.035979, k2 = -18.892648, k3 = 44.544014
			autoCalibCompCoefs.u0 = 326.159754 / 640.0;
			autoCalibCompCoefs.v0 = 212.770324 / 480.0;

			autoCalibCompCoefs.k[0] = 8.035979;
			autoCalibCompCoefs.k[1] = -18.892648;
			autoCalibCompCoefs.k[2] = 44.544014;
#else

			autoCalibCompCoefs.u0 = 321.239992 / 640.0;
			autoCalibCompCoefs.v0 = 224.913703 / 480.0;

			autoCalibCompCoefs.k[0] = 5.012811;
			autoCalibCompCoefs.k[1] = 12.998592;
			autoCalibCompCoefs.k[2] = -18.748065;
			//2018/01/11
#endif
		}
			//
			//lens.bInternalAndSymmetricDistortParamsIsValid = TRUE;

			////相机内部参数
			////mu = 1.22754, mv = 1.22628
			////u0 = 318.521, v0 = 247.995
			////k(1) = 202.660577, k(2) = -1.567474, k(3) = 4.009577, k(4) = -3.274776, k(5) = 0.924704
			//lens.lensInternalAndSymmetricDistortParams.mu = 1.22754;
			//lens.lensInternalAndSymmetricDistortParams.mv = 1.22628;
			//lens.lensInternalAndSymmetricDistortParams.u0 = 318.521;
			//lens.lensInternalAndSymmetricDistortParams.v0 = 247.995;

			////相机对称畸变参数
			//lens.lensInternalAndSymmetricDistortParams.radialDistrt[0] = 202.660577;
			//lens.lensInternalAndSymmetricDistortParams.radialDistrt[1] = -1.567474;
			//lens.lensInternalAndSymmetricDistortParams.radialDistrt[2] = 4.009577;
			//lens.lensInternalAndSymmetricDistortParams.radialDistrt[3] = -3.274776;
			//lens.lensInternalAndSymmetricDistortParams.radialDistrt[4] = 0.924704;

		}
		{
		   TLensConfig&  lens = lensConfigs[2][E_LENS_TR_0_DOT_15];

		   //电子白板正常手指触控模式下的使用参数
		   {
			    NormalUsageSettings& normalUsageSettings = lens.normalUsageSettings_FingerTouchWhiteBoard;
			    normalUsageSettings.cYThreshold = 205;
			    normalUsageSettings.nLightSpotMinimumWidth = 2;
			    normalUsageSettings.nLightSpotMinimumHeight = 2;

				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_Gamma = 72;//Gamma    ★
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				normalUsageSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
				normalUsageSettings.cameraParams.Prop_CameraControl_Exposure = -7;

				//缺省值初始化
				normalUsageSettings.cYThresholdDefault = 205;
				normalUsageSettings.nLightSpotMinimumWidthDefault = 2;
				normalUsageSettings.nLightSpotMinimumHeightDefault = 2;

				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_Gamma = 72;//Gamma    ★
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				normalUsageSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				normalUsageSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				normalUsageSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
				normalUsageSettings.defaultParams.Prop_CameraControl_Exposure = -7;
		    }

		   //电子白板正常笔触触控模式下的使用参数
		   {
			NormalUsageSettings& normalUsageSettings = lens.normalUsageSettings_PenTouchWhiteBoard;
			normalUsageSettings.cYThreshold = 205;
			normalUsageSettings.nLightSpotMinimumWidth = 2;
			normalUsageSettings.nLightSpotMinimumHeight = 2;

			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Contrast = 38;//对比度
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			normalUsageSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
			normalUsageSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
			normalUsageSettings.cameraParams.Prop_CameraControl_Exposure = -7;


			//缺省值初始化
			normalUsageSettings.cYThresholdDefault = 205;
			normalUsageSettings.nLightSpotMinimumWidthDefault = 2;
			normalUsageSettings.nLightSpotMinimumHeightDefault = 2;

			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			normalUsageSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
			normalUsageSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
			normalUsageSettings.defaultParams.Prop_CameraControl_Exposure = -7;

		}

		   //手指触控正常模式下的使用参数
		   {
			NormalUsageSettings& normalUsageSettings = lens.normalUsageSettings_FingerTouchControl;
			normalUsageSettings.cYThreshold = 205;
			normalUsageSettings.nLightSpotMinimumWidth = 2;
			normalUsageSettings.nLightSpotMinimumHeight = 2;

			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Contrast = 38;//对比度
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			normalUsageSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
			normalUsageSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
			normalUsageSettings.cameraParams.Prop_CameraControl_Exposure = -7;


			//缺省值初始化
			normalUsageSettings.cYThresholdDefault = 205;
			normalUsageSettings.nLightSpotMinimumWidthDefault = 2;
			normalUsageSettings.nLightSpotMinimumHeightDefault = 2;

			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			normalUsageSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
			normalUsageSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
			normalUsageSettings.defaultParams.Prop_CameraControl_Exposure = -7;

		}

		   //手掌互动正常模式下的使用参数
		   {
			NormalUsageSettings& normalUsageSettings = lens.normalUsageSettings_PalmTouchControl;
			normalUsageSettings.cYThreshold = 205;
			normalUsageSettings.nLightSpotMinimumWidth = 2;
			normalUsageSettings.nLightSpotMinimumHeight = 2;

			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Contrast = 38;//对比度
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			normalUsageSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			normalUsageSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
			normalUsageSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
			normalUsageSettings.cameraParams.Prop_CameraControl_Exposure = -7;


			//缺省值初始化
			normalUsageSettings.cYThresholdDefault = 205;
			normalUsageSettings.nLightSpotMinimumWidthDefault = 2;
			normalUsageSettings.nLightSpotMinimumHeightDefault = 2;

			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma    ★
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			normalUsageSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			normalUsageSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
			normalUsageSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
			normalUsageSettings.defaultParams.Prop_CameraControl_Exposure = -7;

		}

		   //安装调试模式下的参数
		   {
			InstallTunningSettings& installTunningSettings = lens.installTunningSettings;
			installTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			installTunningSettings.cameraParams.Prop_VideoProcAmp_Contrast = 38;//对比度
			installTunningSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
			installTunningSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			installTunningSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			installTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
			installTunningSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			installTunningSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			installTunningSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
			installTunningSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
			installTunningSettings.cameraParams.Prop_CameraControl_Exposure = -7;

			//缺省值初始化
			installTunningSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			installTunningSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
			installTunningSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
			installTunningSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			installTunningSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			installTunningSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
			installTunningSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			installTunningSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			installTunningSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补
			installTunningSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
			installTunningSettings.defaultParams.Prop_CameraControl_Exposure = -7;
		}

		   //电子白板激光器调试模式参数
		   {
			LaserTunningSettings& laserTunningSettings = lens.laserTunningSettings_WhiteBoard;

			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Contrast = 38;//对比度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma = 2;//Gamma    ★
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			laserTunningSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
			laserTunningSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
			laserTunningSettings.cameraParams.Prop_CameraControl_Exposure = -7;


			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Gamma = 2;//Gamma    ★
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			laserTunningSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿 偿
			laserTunningSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
			laserTunningSettings.defaultParams.Prop_CameraControl_Exposure = -7;
		}

		   //手指触控激光器调试模式参数
		   {
			LaserTunningSettings& laserTunningSettings = lens.laserTunningSettings_FingerTouchControl;

			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Contrast = 38;//对比度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma = 2;//Gamma    ★
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			laserTunningSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
			laserTunningSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
			laserTunningSettings.cameraParams.Prop_CameraControl_Exposure = -7;


			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Gamma = 2;//Gamma    ★
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			laserTunningSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿 偿
			laserTunningSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
			laserTunningSettings.defaultParams.Prop_CameraControl_Exposure = -7;
		}

		   //手掌互动激光器调试模式参数
		   {
			LaserTunningSettings& laserTunningSettings = lens.laserTunningSettings_PalmTouchControl;

			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Contrast = 0;//对比度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_Gamma = 2;//Gamma    ★
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			laserTunningSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			laserTunningSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
			laserTunningSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
			laserTunningSettings.cameraParams.Prop_CameraControl_Exposure = -7;


			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_Gamma = 2;//Gamma    ★
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
			laserTunningSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
			laserTunningSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿 偿
			laserTunningSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
			laserTunningSettings.defaultParams.Prop_CameraControl_Exposure = -7;
		}
		   {
			AutoCalibrateSettingsList& autoCalibrateSettingsList = lens.autoCalibrateSettingsList;
			{
				//第一组自动校正参数
				AutoCalibrateSettings &autoCalibrateSettings = autoCalibrateSettingsList[0];

				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				autoCalibrateSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				autoCalibrateSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
				autoCalibrateSettings.cameraParams.Prop_CameraControl_Exposure = -7;//曝光时间

				//第一组,对应较亮的光线环境
				autoCalibrateSettings.calibrateImageParams.autoCalibrateExpectedBrightness = 100;
				autoCalibrateSettings.calibrateImageParams.autoCalibrateHilightGray = 255;//

																						  //缺省参数
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				autoCalibrateSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				autoCalibrateSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
				autoCalibrateSettings.defaultParams.Prop_CameraControl_Exposure = -7; //曝光时间

				//第一组,对应较亮的光线环境
				autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateExpectedBrightness = 100;
				autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateHilightGray = 255;//
			}
			{
				//第二组自动校正参数
				AutoCalibrateSettings &autoCalibrateSettings = autoCalibrateSettingsList[1];

				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				autoCalibrateSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				autoCalibrateSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
				autoCalibrateSettings.cameraParams.Prop_CameraControl_Exposure = -7;//曝光时间

				//第二组,对应适当的光线环境
				autoCalibrateSettings.calibrateImageParams.autoCalibrateExpectedBrightness = 100;
				autoCalibrateSettings.calibrateImageParams.autoCalibrateHilightGray = 180;

				//缺省参数
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				autoCalibrateSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				autoCalibrateSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
				autoCalibrateSettings.defaultParams.Prop_CameraControl_Exposure = -7; //曝光时间

				autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateExpectedBrightness = 100;
				autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateHilightGray = 180;
			}
			{
				//第三组自动校正参数
				AutoCalibrateSettings &autoCalibrateSettings = autoCalibrateSettingsList[2];

				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Hue = 0;//色调
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				autoCalibrateSettings.cameraParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				autoCalibrateSettings.cameraParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				autoCalibrateSettings.cameraParams.Prop_VideoProcMap_Gain = 0;//增益
				autoCalibrateSettings.cameraParams.Prop_CameraControl_Exposure = -7;//曝光时间

				//第三组, 对应较暗的光线环境
				autoCalibrateSettings.calibrateImageParams.autoCalibrateExpectedBrightness = 70;
				autoCalibrateSettings.calibrateImageParams.autoCalibrateHilightGray = 120;

				//缺省参数
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Brightness = 0;//亮度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Contrast = 38;//对比度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Hue = 0;//色调
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Satuation = 64;//饱和度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Sharpness = 1;//锐利度
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_Gamma = 1;//Gamma
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_ColorEnable = 0;//颜色
				autoCalibrateSettings.defaultParams.Prop_VideoProcAmp_WhiteBalance = 4600;//白平衡
				autoCalibrateSettings.defaultParams.Prop_VideoProcMap_BacklightCompensation = 2;//背光补偿
				autoCalibrateSettings.defaultParams.Prop_VideoProcMap_Gain = 0;//增益
				autoCalibrateSettings.defaultParams.Prop_CameraControl_Exposure = -7; //曝光时间

				 //第三组, 对应较暗的光线环境
				autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateExpectedBrightness = 70;
				autoCalibrateSettings.defaultCalibrateImageParams.autoCalibrateHilightGray = 120;
			}
		}

		   //自动屏蔽参数
		   {
				AutoMaskSettings& autoMaskSettings = lens.autoMaskSettings;
				autoMaskSettings.cAutoMaskDetectThreshold = 180;
				autoMaskSettings.nMaskAreaEroseSize = DEFAULT_MASK_AREA_EROSIE_SIZE;

				autoMaskSettings.cAutoMaskDetectThresholdDefault = 180;
				autoMaskSettings.nMaskAreaEroseSizeDefault = DEFAULT_MASK_AREA_EROSIE_SIZE;
		   }

		   //镜头规格
		   {
				TLensSpecification& lensSpecification = lens.lensSpecification;
				lensSpecification.backFocalLength = 2.7;//像方焦距,   单位:mm
				lensSpecification.FOV_horz = 146;//水平视角，  单位:度
				lensSpecification.FOV_vert = 105;//垂直视角，  单位:度
				lensSpecification.FOV_diagonal = 175;//对角线视角，单位:度
				lensSpecification.throwRatio = 0.15;//投射比
		   }

		   //自动校正补偿系数
		   {
		     	TAutoCalibCompCoefs& autoCalibCompCoefs = lens.autoCalibCompCoefs;
#ifdef CCD_ROTATE_90
			//u0 = 323.727170, v0 = 221.434748, k1 = 7.331750, k2 = -10.172566, k3 = 20.331750
			//autoCalibCompCoefs.u0 = 323.727170 / 640.0;
			//autoCalibCompCoefs.v0 = 221.434748 / 480.0;

			//autoCalibCompCoefs.k[0] = 7.331750;
			//autoCalibCompCoefs.k[1] = -10.172566;
			//autoCalibCompCoefs.k[2] = 20.331750;


			//u0 = 326.159754, v0 = 212.770324, k1 = 8.035979, k2 = -18.892648, k3 = 44.544014
			autoCalibCompCoefs.u0 = 326.159754 / 640.0;
			autoCalibCompCoefs.v0 = 212.770324 / 480.0;

			autoCalibCompCoefs.k[0] = 8.035979;
			autoCalibCompCoefs.k[1] = -18.892648;
			autoCalibCompCoefs.k[2] = 44.544014;
#else

				autoCalibCompCoefs.u0 = 321.239992 / 640.0;
				autoCalibCompCoefs.v0 = 224.913703 / 480.0;

				autoCalibCompCoefs.k[0] = 5.012811;
				autoCalibCompCoefs.k[1] = 12.998592;
				autoCalibCompCoefs.k[2] = -18.748065;
				//2018/01/11
#endif
		   }
				//
				//lens.bInternalAndSymmetricDistortParamsIsValid = TRUE;

				////相机内部参数
				////mu = 1.22754, mv = 1.22628
				////u0 = 318.521, v0 = 247.995
				////k(1) = 202.660577, k(2) = -1.567474, k(3) = 4.009577, k(4) = -3.274776, k(5) = 0.924704
				//lens.lensInternalAndSymmetricDistortParams.mu = 1.22754;
				//lens.lensInternalAndSymmetricDistortParams.mv = 1.22628;
				//lens.lensInternalAndSymmetricDistortParams.u0 = 318.521;
				//lens.lensInternalAndSymmetricDistortParams.v0 = 247.995;

				////相机对称畸变参数
				//lens.lensInternalAndSymmetricDistortParams.radialDistrt[0] = 202.660577;
				//lens.lensInternalAndSymmetricDistortParams.radialDistrt[1] = -1.567474;
				//lens.lensInternalAndSymmetricDistortParams.radialDistrt[2] = 4.009577;
				//lens.lensInternalAndSymmetricDistortParams.radialDistrt[3] = -3.274776;
				//lens.lensInternalAndSymmetricDistortParams.radialDistrt[4] = 0.924704;
        }
    }
};

//传感器配置信息
struct TSensorConfig
{
    ESensorLensMode  eLensMode              ;//镜头工作模式
    CAtlString       strFavoriteDevicePath  ; //优先选择的视频设备路径
    CAtlString       strFavoriteMediaType   ; //选择的视频格式名称
    ELensType        eSelectedLensType      ; //当前选中的镜头类型

    //关联屏幕信息
    BOOL             bAutoAttachMonitor  ;   //自动关联屏幕标志
    int              nAttachedMonitorId  ;   //Sensor手动设置的关联屏幕id
    EMonitorAreaType eMonitorAreaType    ;   //手动设置的关联屏幕区域类型
 
	std::vector<TSensorModeConfig>    vecSensorModeConfig ; //传感器的模式

    TSensorConfig()
    {
        eLensMode = E_VIDEO_TUNING_MODE;//缺省图像调试模式

        //attachedMonitorIds.resize(1);
        //attachedMonitorIds[0] = 0;//缺省关联0号屏幕。
        strFavoriteDevicePath = _T("");
        strFavoriteMediaType  = _T("640 X 480 MJPG");

        //手触和笔触模式下的摄像头亮度系数是不同的.
       // normalUsageSettings_FingerTouchWhiteBoard.cameraParams.Prop_VideoProcAmp_Brightness = 150;
        //normalUsageSettings_PenTouchWhiteBoard   .cameraParams.Prop_VideoProcAmp_Brightness = 30 ;
        //当前选中的镜头类型
        eSelectedLensType = E_LENS_TR_0_DOT_34;
        //自动关联屏幕标志
        bAutoAttachMonitor = true;
        //Sensor手动设置的关联屏幕id
        nAttachedMonitorId = 0;//Sensor
        //手动设置的关联屏幕区域类型
        eMonitorAreaType = E_MONITOR_AREA_TYPE_FULLSCREEN;
		//////////用来存放墙面和桌面的参数Add by zhaown 2019.7.17
		vecSensorModeConfig.resize(2);

    }//构造函数

};


//系统配置信息
struct TSysConfigData
{
	GlobalSettings               globalSettings;//全局配置信息
    std::vector<TSensorConfig>  vecSensorConfig;//投影模式配置
    
	//std::vector<TScreenLayout>  vecScreenLayouts;//多屏屏接屏幕布局
	ScreenLayoutManager          screenLayoutManger;//屏幕布局管理器


	TSysConfigData()
	{
		vecSensorConfig.resize(SENSOR_NUMBER);
	}

};

//typedef std::vector<TSensorConfig> AllSensorsConfig;

_declspec(selectany) extern const VideoMediaType DEFAULT_VIDEO_FORMAT
=
{
    MEDIATYPE_Video,
    MEDIASUBTYPE_MJPG,
    FORMAT_VideoInfo,

    //union
    {
        {//
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            147456000u,
            0u,
            333333,
            {
                40,
                    640,
                    480,
                    1,
                    16,
                    844715353,
                    614400,
                    0,
                    0,
                    0,
                    0
            }
        }

    }
};

// \\?\USB#Vid_a088&Pid_082
#define DEV_ID_LENGTH 25
_declspec(selectany) extern const TCHAR* DEFAULT_DEV_IDS[] = {
    _T("\\\\?\\USB#Vid_a088&Pid_0820"),
    _T("\\\\?\\USB#Vid_0ac8&Pid_3450")

};

//@功能:载入配置文件
//@参数:lpszConfigFilePath, 配置文件的完整路路径
BOOL LoadConfig(LPCTSTR lpszConfigFilePath, TSysConfigData& sysCfgData);
//@功能:保存配置文件
//@参数:lpszConfigFilePath, 配置文件的完成路径
BOOL SaveConfig(LPCTSTR lpszConfigFilePath, const TSysConfigData& sysCfgData,int nSensorCount);

extern TSysConfigData g_tSysCfgData;



