#pragma once

class CIWBSensor
{
public:
    CIWBSensor(int nID=0);

    virtual ~CIWBSensor();

    //@功能:设置视频显显示相关信息
    //@参数:hDispWnd, 视频播放窗体的句柄
    //      rcDispArea, 视频绘制的区域  
    //      hNotifyWnd, 接收消息接收窗体句柄
    void SetVideoDispParams(HWND hDispWnd, RECT& rcDispArea, HWND hNotifyWnd);

    //@功能:运行传感器
    BOOL Run();

    BOOL Stop();

    void ShowMissStatusInfo();


    //@功能:设置视频捕获设备信息
    void SetDeviceInfo(const TCaptureDeviceInstance& devInfo);

    //@功能:返回视频捕获设备信息
    const TCaptureDeviceInstance& GetDeviceInfo()const;


    void SetFavoriateMediaType(const VideoMediaType& vmt);

    CVideoPlayer* GetVideoPlayer()
    {
        return &m_oVideoPlayer;
    }

    CPenPosDetector* GetPenPosDetector()
    {
        return &m_oPenPosDetector;
    }

    CInterceptFilter*  GetInterceptFilter()
    {
        return this->m_pInterceptFilter;
    }
    
     CAutoCalibrator* GetAutoCalibrator()
     {
         return &this->m_oAutoCalibrator;
     }
    //@功能:进入指定的镜头模式,并执行系列动作
    //@参数:eMode, 工作模式枚举值
    void SwitchLensMode(ESensorLensMode eMode);


    //@功能:只是设置相机的镜头模式，并不实际执行动作
    //@参数:eMode, 镜头模式枚举值
    void SetLensMode(ESensorLensMode eMode){ m_tCfgData.eLensMode = eMode;}
    ESensorLensMode GetLensMode()const{ return m_tCfgData.eLensMode;}


    
    //@功能:载入配置数据
    //@参数:cfgData, 输入参数, 图像传感器的配置信息
    //      pGlobalSettings, 输入参数, 指向全局配置信息的指针
    void SetCfgData( const TSensorConfig& cfgData, const GlobalSettings* pGlobalSettings = NULL);
    void SetGlobalCfgData(const GlobalSettings* pGlobalSettings = NULL);

    void SetlenCfgData(const TLensConfig& lencfgData);

    //@功能:获取配置数据
    //@参数:cfgData, 输出参数, 图像传感器的配置信息
    //void GetCfgData(TSensorConfig& cfgData)const;
    const TSensorConfig& GetCfgData()const;

    //@功能:判断传感器是否正在工作
    BOOL IsDetecting();//


    //@功能:使能光笔
    //@参数:bEnable,使能标志
    void EnableOpticalPen(BOOL bEnable);

    //@功能:判断光笔是否在控制中
    //@说明:只要有一支光笔在控制中,则返回TRUE
    BOOL IsOpticalPenControlling();

    //@功能:开始图像传感器的自动校正流程
    //@参数:ePattern, 校正图案模式
    //      hNotifyWnd, 校正结束后的同志消息的接收窗体
    void  StartAutoCalibrate(E_AutoCalibratePattern ePattern, HWND hNotifyWnd);



    //@功能:开始自动屏蔽
    //@参数:hNotifyWnd, 校正结束后的同志消息的接收窗体
    void  StartAutoMasking(HWND hNotifyWnd);

    //@功能:传感器自动校正完成后的事件响应函数
    //@参数:bSuccess, 成功/失败标志
    void OnAutoCalibrateDone(BOOL bSuccess);


    //@功能:在视频画面中显示错误信息
    void ShowErrInfo(LPCTSTR lpszInfo);


    //@功能:开始图像传感器的手动校正流程
    //@参数:
    //      nPtsInRow, 每行的校正点个数
    //      nPtsInCol, 每列的校正点个数
    //      hNotifyWnd, 校正结束后的同志消息的接收窗体
    void  StartManualCalibrate(HWND hNotifyWnd, int nPtsInRow = -1, int nPtsInCol = -1,  EManualCalibrateType eManualCalibType = E_MUNUAL_CALIB_FULL_SCREEN);


    //@功能:传感器手动校正完成后的事件响应函数
    //@参数:bSuccess, 成功/失败标志
    void OnManualCalibrateDone(BOOL bSuccess);


    //@功能:传感器自动搜索屏蔽区域结束事件响应函数
    //@参数:bSuccess, 成功/失败标志
    void OnAutoSearchMaskAreaDone(BOOL bSuccess);

    int GetID()const
    {
        return m_nID;
    }

    //@功能:开启光斑采集功能
    //@参数: hSampleWnd, 采样窗体句柄
    BOOL StartLightSpotSampling(HWND hSampleWnd);


    //@功能:光斑采集结束事件的响应函数
    //@参数:allSampleSize, 所有的采样值
    ///     bSuccess, 成功失败标志
    void OnLightSpotSamplingDone(const ALL_LIGHTSPOT_SAMPLE_SIZE& allSampleSize, BOOL bSuccess);



    //@功能:屏幕分辨率发生变化时的事件响应函数
    ///@参数:rcNewMonitorResolution, 关联的屏幕分辨率信息
    //void OnMonitorResolutionChange(const RECT&  rcNewMonitorResolution);
    void OnMonitorResolutionChange();

    /*
    @功能：开启智能检测挡板是否消失
    */
    void OnStartDetectBackSplashVanished();

    /*
    @功能：停止智能检测挡板是否消失
    */
    void OnStopDetectBackSplashVanished();


    //@功能:静态屏蔽开始回调处理函数
    //@参数:lpCtx, 回调的上下文数据。
    static  BOOL CALLBACK OnPreStaticMasking(LPVOID lpCtx);



    //@功能:设备丢失事件响应函数
    void OnDeviceIsMissing();

    static BOOL  OnAutoCalibChangeCameraParams(EChangeCalibCameraParams eCtrlMode, LPVOID lpCtx, BYTE param1, BYTE param2);


      //@功能:查看校正符号位置
      //@参数:bShow,TRUE,显示符号
      //            FALSE, 隐藏符号
      //@Author:15077727@qq.com, 2015/09/09
      BOOL CheckCalibrateSymbolPos(BOOL bShow);

      //判断是否校正符号是否已经显示
      BOOL IsCalibrateSymbolVisible()const;
      //>>

      //<<added by 15077726@qq.com
      BOOL CalibrateSymbolManualResample();
      //>>

      CCalibrateSymbolManualSampler& GetCalibrateSymbolManualSampler() { return this->m_oCalibSymbolManualSampler;}


      //功能;判断传感器是否有校正数据
      BOOL IsCalibrated()const;

      void SetAttachedScreenArea(const RECT& rcMonitor);

      //@功能:获取关联的屏幕区域尺寸
      BOOL GetAttachedScreenArea(RECT& rcMonitor)const;


      //@功能:获取相机看到的屏幕矩形区域, 该区域大于相机实际管辖的屏幕区域。
      //@参数:uID, id of camera sensor
       const RECT& GetVisibleScreenArea() const;


      void  OnTimer(LPVOID lpCtxData);
      void  SetStrokeInterpolate(bool bEnableStrokeInterpolate);
      void  SetOnlineScreenArea(bool bEnableOnlineScreenArea) ;

      void GetPidVid(INT* pPID, INT* pVID)const;

      ECameraType GetCameraType()const { return m_eCameraType; }

      //@功能:开始传感器4点标定
      void Start4BasePointMarking(HWND hNotifyWnd);

      //@功能:传感器4点标定结束后的事件响应函数
      //@参数:bSuccess, 成功/失败标志
      void On4BasePointMarkingDone(BOOL bSuccess);


      //初始化校正实例
      void ReinitCalibrateInst(E_CALIBRATE_MODEL eCalibrateModel);

	  void SetResolutionType(CAtlString  Value);

	  void OnStartDrawOnlineScreenArea();
	  void OnClearDrawOnlineScreenArea();
	  void OnEnableDrawOnlineScreenArea(BOOL bEnable);

	  BOOL IsEnableOnlineScreenArea();

	  BOOL GenerateMaskFrameWithCalibrateData(CImageFrame& maskFrame, const RECT& rect, int nMonitorId = 0);
	  void GetCollectSpotShowPath(TCHAR *lpszbuf, unsigned int numberOfElements);

	  void UpdateTouchTypeFromUSBKey();


	  void UpdateThrowRatioDisplayInfo();


      UINT GetScreenAreaNo();
      void SetScreenAreaNo(UINT areaNo);
protected:
	  
	  ELensType MapThrowRatioToLensType(const double& throwRatio);

	  void UpdateAutoCalibrateCompensateCoefs(const TLensConfig& lensConfig);

protected:
  //视频播放对象。
  CVideoPlayer     m_oVideoPlayer;

  //视频播放参数
  //HWND             m_hVideoDispWnd  ;//视频播放窗体句柄
  //RECT             m_rcVideoDispArea;//视频播放区域
  //HWND             m_hNotifyWnd     ;//视频播放通知接收窗体



  //因为CInterceptFilter继承自CTransformFilter类，
  //而CTransformFilter类中包含禁止了拷贝构造函数和赋值操作符的类
  //CCritSec  
  CInterceptFilter* m_pInterceptFilter;


  //光斑检测对象。
  CPenPosDetector  m_oPenPosDetector;


  //摄像头信息
  TCaptureDeviceInstance m_tDeviceInfo;

  //
  VideoMediaType        m_tFavoriteMediaType;



  //此图像传感器监视的屏幕区域
  RECT  m_rcMonintorArea;


  //视频坐标到屏幕坐标的映射对象.
//  CVideToScreenMap     m_oVideoToScreenMap;


  //鼠标事件生成器
  //CMouseEventGenerator m_oMouseEventGenerator;
  int m_nID;

  TSensorConfig m_tCfgData; 

  //自动校正对象
   CAutoCalibrator m_oAutoCalibrator;


   //手动校校正对象
   CManualCalibrateWnd   m_oManualCalibrator;

  //屏幕查找器
  //static CDispMonitorFinder m_oDispMonitorFinder;

  //光斑采集对象
//  CCollectSpotSize   m_wndLightSpotSampling;

//  ISpotListProcessor* m_pSpotListProcessor;

  //传感器镜头模式
  //ESensorLensMode m_eLensMode;
  ECameraType     m_eCameraType;

  //<<调试工具, 查看校正点
  //2015/09/09, by 15077726@qq.com
  CCalibrateSymbolChecker m_oCalibSymbolChecker;

  //2015/11/13, by 15077726@qq.com
  CCalibrateSymbolManualSampler m_oCalibSymbolManualSampler;

  CSensorTimerActionExecuter m_oTimerActionExecuter;
  static void _stdcall OnManualSampleDoneCallBackProc(LPVOID lpCtx, BOOL bSuccess);
  //>>

  static const int SONIX_PID = 0x9230;
  static const int SONIX_VID = 0x05a3;

  class ThrowRatioInfo
  {
  public:
	  ThrowRatioInfo()
	  {
		  m_dbThrowRatioInFirmware = 0.0;
		  m_dbThrowRatioSelected   = 0.0;
	  }

	  double m_dbThrowRatioInFirmware;
	  double m_dbThrowRatioSelected;

  }m_ThrowRatioInfo;

};
