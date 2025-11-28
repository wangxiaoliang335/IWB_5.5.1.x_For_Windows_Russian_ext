#pragma once

//内存池类
template<int MAX_BUF_NUMBER, class T>
class CMemPool
{
public:
	CMemPool()
	{
		m_nBufCount = MAX_BUF_NUMBER;

		m_vecObjs.resize(m_nBufCount);

		for(unsigned int i=0; i< m_nBufCount; i++)
		{
			m_vecObjs[i] = new T();
		}

	}

	~CMemPool()
	{

		for(unsigned int i=0; i< m_nBufCount; i++)
		{
			m_vecObjs[i]->Release();//
		}
	}

	T* GetFreeBuf()
	{
		T* pObj= NULL;

		for(unsigned int i=0; i< m_vecObjs.size(); i++)
		{


			assert(m_vecObjs[i]);
			if(m_vecObjs[i]->IsFree())
			{
				pObj = m_vecObjs[i];

				break;
			}
		}
		return pObj;
	}


protected:
	unsigned int m_nBufCount;
	std::vector<T*> m_vecObjs;

};

//帧率检测器(Frames Per Second)
class CFPSDetector
{
public:
	CFPSDetector(int StatisticThreshold = 50)
		:
		m_fCurrentFps(0.0f),
		m_fAvgFps(0.0f),
		m_nStatisticThresholdForCurrentFps(StatisticThreshold)
	{

		QueryPerformanceFrequency(&m_liFreq);
		QueryPerformanceCounter  (&m_liStart);
		QueryPerformanceCounter  (&m_liLastThreshold);
	}

	~CFPSDetector()
	{

	}

	void Reset()
	{
		m_fCurrentFps = 0.0f;
		m_fAvgFps     = 0.0f;
		m_nFrameCounterForCurrentFps = 0;
		m_nTotalFrameCount = 0;

		QueryPerformanceFrequency(&m_liFreq);
		QueryPerformanceCounter  (&m_liStart);
		QueryPerformanceCounter  (&m_liLastThreshold);
	}


	float GetCurrentFps()const
	{
		return m_fCurrentFps;
	}

	float GetAvgFps() const
	{
		return m_fAvgFps;
	}


	void Trigger()
	{
		m_nFrameCounterForCurrentFps++;
		m_nTotalFrameCount++;
		
		if(m_nFrameCounterForCurrentFps == m_nStatisticThresholdForCurrentFps)
		{
			LARGE_INTEGER liNow;
			QueryPerformanceCounter(&liNow);
			//计算当前帧率
			m_fCurrentFps = (float)((double)m_nFrameCounterForCurrentFps * m_liFreq.QuadPart /(double)(liNow.QuadPart - m_liLastThreshold.QuadPart));
			
			m_liLastThreshold = liNow;
			m_nFrameCounterForCurrentFps = 0;

			//计算平均帧率
			m_fAvgFps = (float)((double)m_nTotalFrameCount * ((double)m_liFreq.QuadPart /(double)(liNow.QuadPart - m_liStart.QuadPart)));
		
		}
		
	}


private:
	float m_fCurrentFps     ;//当前帧率
	float m_fAvgFps         ;//平均帧率
	int   m_nFrameCounterForCurrentFps      ;//
	INT64   m_nTotalFrameCount              ;//
	int   m_nStatisticThresholdForCurrentFps;//当前帧率统计门限
	LARGE_INTEGER  m_liFreq;
	LARGE_INTEGER  m_liStart;
	LARGE_INTEGER  m_liLastThreshold;
};

class CDib;

enum EManualScreenAreadMode
{
	E_ManualScreenAreaEditMode  ,//编辑模式
	E_ManualScreenAreaNormalMode,//正常模式
};

struct LightSpotBounds
{
	RECT  m_aryLightSpotBounds;
	bool  valid;

public:
	LightSpotBounds()
	{
		memset(&m_aryLightSpotBounds,0,sizeof(m_aryLightSpotBounds));
		valid = true;
	}
};

//运动检测类
class CIWBSensor;
class CPenPosDetector
{
public:
	CPenPosDetector(int nID);

	~CPenPosDetector();


	//开始侦测
	BOOL StartDetect(HWND hNotifyWnd, int nSrcImageWidth, int nSrcImageHeight, CIWBSensor* pSensor);

	//停止侦测
	BOOL StopDetect();

	//写入侦测的原始视频数据
	//BOOL WriteSourceImageData(BYTE* pData, int nWidth, int nHeight, int cBytesPerPixel);

	//@功能:侦测线程
	static ULONG __stdcall MotionDetectorProc(LPVOID lpCtx);

	//BOOL IsDetecting();

	//@功能:显示原始的视频数据
	//@参数:hWnd,显示视频的窗体
	void ViewSrcImage(HWND hWnd, BOOL bShow);

	//@功能:查看背景图像
	//@参数:hWnd,显示视频的窗体
	void ViewBkgndImage(HWND hWnd, BOOL bShow);

	//@功能:查看差分图像
	//@参数:hWnd,显示视频的窗体
	void ViewDiffImage(HWND hWnd, BOOL bShow);


	//@功能:检测函数
	//BOOL DoDetect(const CYUY2Frame* pYUVFrame);
    BOOL DoDetect(const CImageFrame* pGrayFrame, ESensorLensMode eSensorLenMode);

	//@功能:检测函数
	//BOOL DoDetect_RedLeaf(const CYUY2Frame* pYUVFrame);

	//void DrawTrackTargets(const std::vector<CMoveTargets>& targets);

	//@功能:设置通知窗体以便接收检测结果
	void SetNotifyWindow(HWND hWnd, UINT uMsg);

	//
	UINT GetObjCount() const
	{
		return m_nObjCount;
	}

    //条使用函数
//	const RECT* GetLightSpotBounds() const
//	{
//		return &m_aryLightSpotBounds[0];
//	}

	////
	const LightSpotBounds* GetLightSpotInfo() const
	{
		return &m_aryLightSpotBoundsInfo[0];
	}


	//@功能:设置亮度检测门限
	void SetYThreshold(BYTE threshold);

	//@返回:亮度检测门限
	BYTE GetYThreshold()const;

	//@功能:设置动态屏蔽门限
	void SetDynamicMaskThreshold(BYTE threshold);

	//@功能:重置视频剪切区域
	void ResetVideoClipArea();

	//@功能:设置视频剪切区域
	void SetVideoClipArea(const RECT& rcClipArea);

	//@功能:获取视频剪切区域
	const RECT& GetVideoClipArea()const;

	//@功能:获取手动指定的屏幕区域。
	const RECT&  GetManualScreenArea()const;

	//@功能:设定手动指定的屏幕区域。
	void UpdateScreenMaskArea(const RECT& rcNewArea);

	//@功能:返回是否禁用静态屏蔽区
//	BOOL IsDisableStaticScreenMask()const;

	//@功能:禁用手动指定的屏幕区域
//    void DisableManualScreenArea(BOOL bDisable);


	//@功能:返回手动指定的屏幕区域当前模式
	EManualScreenAreadMode GetManualScreenAreaMode()const;

	//@功能:设定"手动指定屏幕区域"的模式
	//void SetManualScreenAreaMode(EManualScreenAreadMode eNewMode);

	//@功能:判断手动指定的屏幕区域是否正确
	//BOOL IsValidManualScreenArea();

	//@功能:获取手动指定屏幕区域屏蔽图
	const CImageFrame& GetScreenAreaMask();

	//@功能:更新手动指定屏幕区域屏蔽图
//	void UpdateManualScreenAreaMask();

	//@功  能:根据8位位图设置YUY2格式的屏蔽位图
	//@参  数:maskFrame,8位屏蔽位图
	//@返回值:成功返回TRUE
	//        失败返回FALSE
	BOOL SetStaticMaskFrame(const CImageFrame& maskFrame);

	//@功能:增加屏蔽区域
	BOOL AddStaticMaskArea(const RECT* pAddArea);

	//@功能:删除屏蔽区域
	BOOL EraseSaticMaskArea(const RECT* pEraseArea);

	//@功能:清空屏蔽区域
	BOOL ClearStaticMaskArea();
	
    //@功  能:载入屏蔽位图
    //@参  数:lpFileName, 屏蔽位图文件名称
    //        maskFrame, 输出的屏蔽帧
	//@返回值:成功返回TRUE,
	//        失败返回FALSE
    //BOOL LoadMaskFrame(LPCTSTR lpszFileName, CYUY2Frame& maskFrame);
    BOOL LoadMaskFrame(LPCTSTR lpszFileName, CImageFrame& maskFrame);
	
    //BOOL InitStatisticsFrame(CYUY2Frame& maskFrame);
    BOOL InitStatisticsFrame(CImageFrame& maskFrame);

	//@功  能:载入屏蔽位图
	//@返回值:成功返回TRUE,
	//        失败返回FALSE
	BOOL LoadMaskFrame();

	//@功  能:保存屏蔽位图
    //@参  数:lpszFileName, 保存的文件名称
    //        maskFrame, 屏蔽帧
	//@返回值:成功返回TRUE,
	//        失败返回FALSE
     //BOOL SaveMaskFrame(LPCTSTR lpszFileName, const CYUY2Frame& maskFrame);
    BOOL SaveMaskFrame(LPCTSTR lpszFileName, const CImageFrame& maskFrame);
	
    BOOL SaveDynamicMaskFrame();
	BOOL SaveStaticMaskFrame();
    

	//@功  能:屏蔽干扰点
	//@参  数:pGrayFrame,灰度实时图片帧。
	//        YThreshold, 亮度门限。
	//@返回值:TRUE, 有新的屏蔽区域加入
	//        FALSE,无新的屏蔽区域加入
    BOOL MaskClutter(const CImageFrame* pGrayFrame, BYTE YThreshold);

	const int GetSrcImageWidth()const { return m_nSrcImageWidth;}
	const int GetSrcImageHeight()const{ return m_nSrcImageHeight;}

	//add by vera_zhao 2018.11.29
	void SetSrcImageSize(int Width, int Height); 

	//@功能:侦测干扰光斑,并更新静态屏蔽图
	void StartClutterDetection(BOOL bStart, BYTE cAutoMaskingThreshold = DEFAULT_AUTO_MAKSING_THRESHOLD);
    
    //@功能:正在干扰点侦测
    BOOL IsClutterDetecting()const {return m_bIsClutterDetecting;}

    //@功能:设置引导矩形框的位置
    void SetGuideRectangle(const RECT& rcGuideRectangel, DWORD dwRGBColor);

    void GetGuideRectangle(RECT* pRect, DWORD* pRGBColor)const;
	void RegulateGuideRectangle(CIWBSensor* pSensor,int ActiveImageWidth, int ActiveImageHaight);
    //@功能:显示或者隐藏引导矩形框
    void ShowGuideRectangle(BOOL bShow);
    //@功能:判别引导矩形框是否可见
    BOOL IsGuideRectangleVisible() const;

	//////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////zhaown
	void SetSpotProportion(int nSpot)                {m_nSpotProportion  = nSpot ;}
	int  GetSpotProportion()const                    {return  m_nSpotProportion  ;}
	void SetMultiEraser(int nEraser)                 {m_nMultiEraser  = nEraser  ;}
	int  GetMultiEraser()const                       { return  m_nMultiEraser    ;}
	void SetFixedBlobSetTime(int nFixedBlobSetTime)  {m_nFixedBlobAutoMaskTime = nFixedBlobSetTime ;}
	int  GetFixedBlobSetTime()const                  {return m_nFixedBlobAutoMaskTime              ;}



    void  EnableDynamicMasking(BOOL bEnableDynamicMasking) {
        m_bIsDynamicMasking = bEnableDynamicMasking; 
        m_bDynamicMaskingChangedEvent = TRUE;
    }
    BOOL  IsDynamicMasking()const                               { return m_bIsDynamicMasking; }

	/////////modify by vera_zhao 2019.10.24
	void  EnableAntiJamming(BOOL bEnableAntiJamming)            { m_bIsAntiJamming = bEnableAntiJamming; }
	BOOL  IsAntiJamming()const                                  { return m_bIsAntiJamming; }

    void EnableStaticMasking(BOOL bEnableStaticMasking)         { m_bIsStaticMasking = bEnableStaticMasking; }
	BOOL IsStaticMasking()const                                 { return m_bIsStaticMasking;}

	void EnableOnlineScreenArea(BOOL bEnableManualMaskFrame) { m_bEnableOnlineScreenArea = bEnableManualMaskFrame; }
	BOOL IsEnableOnlineScreenArea()const                           { return m_bEnableOnlineScreenArea; }

	void DisableReflectionPoint(BOOL bDisableReflectionPoint) { m_bDisableReflectionSpot = bDisableReflectionPoint; }
	BOOL IsDisableReflectionPoint()const                      { return m_bDisableReflectionSpot; }

	void SetCalibrateFailed(int CalibrateFailed)                {  m_bCalibratorFailed = CalibrateFailed ;  }

    //Added by toxuke@gmail.com, 2014/09/10
    //void EnbleGestureRecognition(BOOL bEnable)                  { m_bGestureRecognitionEnable = bEnable;}

    BOOL IsGestureRecognitionEnable() {return (m_eDeviceTouchType == E_DEVICE_FINGER_TOUCH_WHITEBOARD || m_eDeviceTouchType == E_DEVICE_PALM_TOUCH_CONTROL)?TRUE:FALSE; };

    //2014/12/18, added by toxuke@gmail.com
    //@功能:设置触控类型(笔触/手触)
    //@说明:根据不同的触控类型，载入不同的光斑采样文件。
    void SetTouchType(EDeviceTouchType eType);
    //>>


	//@功能:设置关联的屏幕的尺寸
	void SetAttachedMonitorSize(const RECT& rcMonitor);
    
    //const CYUY2Frame& GetStaticMaskFrame() const   { return this->m_oStaticMask;}
    const CImageFrame& GetStaticMaskFrame() const   { return this->m_oStaticMask;}
	//const CYUY2Frame& GetDynamicMaskFrame() const  {return m_oDynamicMask      ;}
    const CImageFrame& GetDynamicMaskFrame() const  {return m_oDynamicMask      ;}
	//CYUY2Frame&  GetStaticMask()                   { return this->m_oStaticMask;}
	const  CImageFrame& GetManualOnLineScreenArea() const { return this->m_oManualOnLineScreenArea; }

    //<<2014/12/16
    //@功能:设置是否显示光斑尺寸信息
    //@参数:bShow, TRUE:显示; FALSE:隐藏。
    void ShowSpotSizeInfo(BOOL bShow) {m_bDisplaySpotSizeInfo = bShow;}

    BOOL IsSpotSizeInfoVisible()const {return m_bDisplaySpotSizeInfo;}

    //@功能设置是否显示光斑外接矩形
    //@参数:bShow, TRUE:显示; FALSE:隐藏。
    void ShowSpotRect(BOOL bShow) {m_bDisplaySpotRect = bShow;}

    //@功能:判断光斑外接矩形是否显示
    BOOL IsSpotRectVisible()const { return m_bDisplaySpotRect;}
    //>>

      CVideToScreenMap& GetVideoToScreenMap()            { return this->m_oVideo2ScreenMap;}
      const CVideToScreenMap& GetVideoToScreenMap()const { return this->m_oVideo2ScreenMap;}
     // CMouseEventGenerator& GetMouseEventGenerator() { return this->m_oMouseEventGen;}

      //added by toxuke@gmail.co, 2014/11/17
      void SetSpotListProcessor(ISpotListProcessor* pSpotListProcessor)
      {
            m_pSpotListProcessor = pSpotListProcessor;
      }

	  ISpotListProcessor* GettSpotListProcessor()
	  {
		  return m_pSpotListProcessor;
	  }


     //<<校正相关函数
    //added by toxuke@gmail.com, 2014/11/21
    //@功能:进入校正模式
	//@参数:
	void EnterCalibrateMode(HWND hCalibrateWnd, E_CALIBRATE_MODE eCalibrateMode);

	//@参数:离开校正模式
	void LeaveCalibrateMode();

   
	void EnableOpticalPenControl(BOOL bEnable)
	{
		m_bPenControl = bEnable;
	}

	BOOL IsOpticalPenControlling() const
	{
		return m_bPenControl;
	}

    
	E_CALIBRATE_MODE GetCalibrateMode()const
	{
		return m_eCalibrateMode;
	}

	HWND GetCalibrateHWnd()const
	{
		return m_hCalibrateWnd;
	}

	BOOL IsCalibrating()const
	{
		return m_bIsCalibrating;
	}


    //=====光斑采样相关函数======
    void EnterSpotSamplingMode(HWND hSamplingWnd/*,E_COLLECTSPOT_MODE eSpotSettingMode*/)
    {
        m_hSpotSamplingWnd = hSamplingWnd;
        m_bIsSamplingSpot =TRUE;
        //m_eSpotSamplingMode = eSpotSettingMode;
    }

	void LeaveSpotSamplingMode()
    {
        m_hSpotSamplingWnd = NULL;
        m_bIsSamplingSpot = FALSE;
    }
	
    //E_COLLECTSPOT_MODE GetCollectSpotMode()const{return m_eSpotSamplingMode;}
	
    HWND  GetSpotHWnd()const {return m_hSpotSamplingWnd;}
	
    BOOL  IsCollectingSpot()const {return m_bIsSamplingSpot;}

    //>>2014/11/21
    void UpdateDynamicMaskFrame(const CImageFrame& refSatisticFrame);

    void  SetLightSpotSampleSize(const ALL_LIGHTSPOT_SAMPLE_SIZE& sampleSize);//设置手动光斑采集时的数据
    void SetMinimumLightSpotSize(SIZE sizeMinimum) {this->m_szLightSpotMinimumSize = sizeMinimum; }
	SIZE GetMinimumLightSpotSize() { return m_szLightSpotMinimumSize;     }

    int GetID()const {return this->m_nID;}

     //======捕获图片帧相关函数======
    BOOL IsImageCaputredDone() const;
    
    //@功能:开始捕获图片帧
    //@参数:nDelay
    void StartImageCapture(DWORD dwDelay);

    //@功能:返回捕获的图片帧
    //const CYUY2Frame& GetCapturedFrame()const;
    const CImageFrame& GetCapturedFrame()const;

    //======附加函数=====
    //@功能:设置镜头焦距类型
    //@参数:eLensFocalType, 镜头焦距类型枚举值
    void SetLensFocalType(ELensType eLensType);

    //<<added by toxuke@gmail.com, 2015/03/09
    //@功能:判断是否处于手动校正状态中
    BOOL IsManualCalibrating() const { return this->m_bIsCalibrating && (this->m_eCalibrateMode == CALIBRATE_MODE_MANUAL);}
    //>>
	BOOL IsAutoCalibrating() const { return this->m_bIsCalibrating && (this->m_eCalibrateMode == CALIBRATE_MODE_AUTO); }


    //<<2015/04/07
    //@功能:合并手动校正得到的屏幕屏蔽图到静态屏蔽图中去
    //@参数:maskFrame, 手动校正得到的屏幕投影屏蔽图
    void MergeManualCalibScreenMask(const CImageFrame& maskFrame);
    //>>

    void SetHasDetectSVStarted(bool bStart)
    {
        m_bHasDetectSVStarted = bStart;
    }

    //2015/11/03
    //<<
    void EnterManualResample(HWND hResampleWnd);

    void LeaveManualResample();
    //>>
    CBlobDetector m_oBlobCounter;

	//@功能，屏幕尺寸的改变
//	void SetScreenSize(int ScreenWidth, int ScreenHeight);
    //<<debug

    //const POINT (& GetContourCrossPoints() const)  [MAX_OBJ_NUMBER][4] 
    //{
    //            return m_aryContourCrossPoints;
    // }
        
    //debug>>
	void ShowCalibratePoints(BOOL bShow)
	 {
		 m_bShowCalibratePoints = bShow;
	 }

	 BOOL IsShowingCalibratePoints()const
	 {
		 return m_bShowCalibratePoints;
	 }

	 const TCameraDebugData* GetCameraDebugData()const
	 {
		 return &m_vecDebugData[0];
	 }

     BOOL IsTriggeringGuesture()const
     {
         if (m_pSpotListProcessor)
         {

             return m_pSpotListProcessor->IsTriggeringGuesture();
         }
         return FALSE;
     }

	 void    SetCurrentOnLineScreenAreaPt(CPoint &point);
	 void    GetCurrentOnLineScreenAreaPt(std::vector<CPoint> &pt);

	 void    DeleteOnLineScreenArea();
	 void    ClearOnLineScreenArea();

	 void    SaveOnLineScreenArea();
	 void    LoadOnLineScreenArea();

	 void    DeleteOnLineFile();
	 bool    LoadOnLinePt();
	 bool    SaveOnLinePt();


     //@功能:进入4点标定模式
     void EnterMarkStoneBaseMode(HWND hNotifyWnd);

     //@功能:离开四点标定模式
     void LeaveMarkStoneBaseMode();

     //@功能:返回标定对象。
     CBaseStoneMarker& GetBaseStoneMarker();

     //@功能:判断是否正在进行4点标定
     BOOL IsMarkingBasestone()const;



protected:

	BOOL SimulateMannalCalibrate(LPCTSTR lpszAVIFilePath);    
	static ULONG __stdcall SimulateMannalCalibrateProc(LPVOID lpCtx);


    //@功能:载入光斑采集数据
    //@参数:szDeviceType, 设备类型,_T("3D"):笔触控; _T("FP"):手指触控
    //BOOL LoadSpotData(LPCTSTR szDeviceType);
    //@功能:载入光斑采集数据
    //@参数:eDeviceTouchType, 设备类型
    //      eLensType, 镜头类型
    BOOL LoadSpotData(EDeviceTouchType eDeviceTouchType, ELensType eLensType);

	BOOL SaveSpotData(const NORMALIZED_SMAPLE_SIZE& normalizedSampleData );

	//CString  m_strszXmlSystemFileName ;   //保存在安装系统下的路径



    //<added by jiqw
    //<added reason:满足可外插值，方式——光斑利用离自己最近的矩形区域插值
    //@功能：获取离某点最近的矩形区域的下标
    //@参数：nMonitorId, 光点所属屏幕编号
    //      ptCurent为当前鼠标的屏幕坐标位置    
    int GetNearestRectIndex(const int &nMonitorId, POINT &ptCurent);
    //addedy by jiqw>
    

    //@功能:根据采样值和光斑屏幕坐标，用双线性插值算法，找出屏幕坐标处的光斑最大面积。
    //@参数:CurrentPt, 光斑位置
    //      nMonitorId, 屏幕Id
    //      nDefaultSize, 如果采样值丢失后的缺省尺寸。
    //@说明:经过计算后得到任意位置的光斑最大面积，用来筛选小光斑。
	LONG  GetLightSpotThreshold(POINT ptCurrent ,int nMonitorId, int nAreaIndex, int nDefaultSize);
	float GetPercentageThreshold(POINT ptCurrent ,int nMonitorId, int nAreaIndex, float DefaultPercentage);
	
    //RECT  m_rcCalculateArea[8];                       //纪录双线性插值的四个点的位置

    typedef std::vector<RECT>  MONITOR_SUBAREA              ;
    typedef std::vector<MONITOR_SUBAREA> ALL_MONITOR_SUBAREA;//所有屏幕划分的子区域
    ALL_MONITOR_SUBAREA m_AllMonitorSubArea                 ;

	//LONG  m_lScreeenAreas_Actual;                     //实际得到屏幕的面积           
	//LONG  m_lScreeenAreas_stardard;                   //标准的屏幕的面积
	//LONG  GetScreenRealArea();                        //得到屏幕的真实值
	//void  SetScreenRealArea(LONG lScreenArea);        //设置屏幕的真实值

  
    //@功能:在窗体中显示视频帧
    //HWND m_hDisplayWnd;//显示窗体
    BOOL DisplayFrame(HWND hWnd, const BYTE* pYUY2Frame, int nWidth, int nHeight);

     //@功能:执行光斑采集任务
    //@参数:pObj, 指向光斑数组的指针
    //      nCount, 光斑个数
    void DoCollectSpot(const TBlobObject* pObjs, size_t nCount);

     //2015/03/05
    //<<调试函数
    void LoadSpotDataFromFile();
    //>>

	//@功能，过滤掉不合适的点
	void  FilterOutUnqualifiedSpot(const CImageFrame& grayFrame ,TBlobObject* pObjs, size_t nObjCount);
	//@功能，寻找有无效光斑有关联的有效光斑置成无效光斑
	void  InvalidateSpotInNeighborhood(TBlobObject* pObjs, size_t nCount);
	//@功能，用新的门限来对有效光斑进行二次判定。
	void  UpdateObjectWithNewThreshold(const CImageFrame& grayFrame, BYTE newYThreshold, TBlobObject& obj);
	void  SplitInValidSpot(RECT rArea, E_SpotDirection  eSplitDirectioon, int nSplitNum,  std::vector<TObjDimension> & splitinvaliddata);
	void  FlashingSpotInvalidityCheck(TBlobObject* pObjs, size_t nCount);
	void  DetectSpotSlopeDirection(const CImageFrame& grayFrame, TBlobObject& pObj,int CalArea);
	void  SpecialInvalidSpot(TBlobObject& pObj);

	void  FilterMaxNeighborhoodSpot(TBlobObject* pObjs, size_t nObjCount);



 protected:
    HANDLE m_hSimulateManualThread;
	CAviReader  m_oAVIInput;            //AVI输入源
	CImageFrame m_SimFrame;
	BOOL  m_bSimCalibrate;


     //int m_nSamplePerRow   ;//每行的光斑采样点个数
     int m_nSamplePerCol ;//每列的光斑采样点个数

    

    ALL_LIGHTSPOT_SAMPLE_SIZE m_allSampleSize;//所有的光斑采样尺寸。
    NORMALIZED_SMAPLE_SIZE    m_allNormalizedSampleSize;//所有光斑采样值对屏幕尺寸归一化后的值。


	static const int MAX_FRAME_BUFFER = 2;
	CMemPool<MAX_FRAME_BUFFER, CYUY2Frame> m_YUV2FramePool;//YUY2图像帧内存池

	//HANDLE m_hEventNewImage;
	//HANDLE m_hEventExit;
	//HANDLE m_hDetectThread;


	//【待处理图像帧序列】
	//std::queue<CYUY2Frame*> m_NewFrameQueue;

	//保护【待处理图像帧序列】的关键段
	//CComAutoCriticalSection m_csForNewFrameQueue;

	//debug参数
	HWND m_hWndImageShow              ;//显示原始视频的窗体句柄
	//HWND m_hWndBkgndShow            ;//显示背景的窗体句柄
	//HWND m_hWndDiffImageShow        ;//显示查分图的窗体句柄
	HWND m_hNotifyWnd                 ;//接收检测结果的窗体句柄
	UINT m_uNotifyMsg                 ;//通知消息

	std::map<HWND,CDib*> m_MapWnd2Dib;//为了加速显示,建立的窗体句柄到Dib对象的映射

    void RestrictArea(RECT& rcArea);
	CImageFrame m_CurrentFrame;

	//CBlobDetector m_oBlobCounter;

	BOOL               m_BkgndInitialized;
	static const int   m_nBkgndPixelChangeStep = 8;
	static const int   m_nDiffMinimumTheshold  = 15;


	HBRUSH  m_hRedBrush;
	HBRUSH  m_hGreenBrush;
	HBRUSH  m_hWhiteBrush;
	HBRUSH  m_hCyanBrush;
	HPEN    m_hRedPen;

	LOGFONT m_LogFont;
	HFONT   m_hFont;

	//每次检测到的结果
    //	RECT  m_aryLightSpotBounds[MAX_OBJ_NUMBER];
	//<<<<<<begin
	//add by vera_zhao 2018.12.24
	//用一个数组来表示，把无效的光斑和有效的光斑用不同的颜色区分开。
	LightSpotBounds m_aryLightSpotBoundsInfo[MAX_OBJ_NUMBER];  
	std::vector <TInvalidSpotList> m_vecInvalidList;
	//<<<<<<end


    //<<debug
    //POINT m_aryContourCrossPoints[MAX_OBJ_NUMBER][4];
    //debug>>

	//检测到的目标个数
	UINT           m_nObjCount;

	//2010-12-18,增加目标亮度门限可调
	BYTE          m_YThreshold;

	//2018/01/03,动态屏蔽的门限, 长时间高于此门限的光斑被自动屏蔽。
	BYTE          m_YDynamicMaskThreshold;
	//

	//2010-12-18 21:00 增加视频窗口的剪切区域
	RECT         m_rcVideoClipArea;

	//CFPSDetector m_oFpsDetector;
		

	int m_nSrcImageWidth;
	int m_nSrcImageHeight;

	//亮斑最小尺寸
	SIZE m_szLightSpotMinimumSize ;


	//2011-11-01  zhaoweina modify 2013-06-18
	BOOL m_bDisableStaticScreenMask           ;//禁用禁态屏蔽区

	RECT m_rcManualScreenArea                 ;//手动指定的屏幕区域
	BOOL m_bValidManualScreenArea             ;//屏幕区域数据

	CComAutoCriticalSection m_csScreenAreaMask;

	//CYUY2Frame m_oStaticMask          ;//屏幕区域静态屏蔽图
    CImageFrame m_oStaticMask           ;//屏幕区域静态屏蔽图
	
    //CYUY2Frame m_oDynamicMask         ;//屏幕区域动态屏蔽图
    CImageFrame m_oDynamicMask         ;//屏幕区域动态屏蔽图

	//CYUY2Frame m_oClutterMask         ;//屏幕区域杂波屏蔽图
    CImageFrame m_oClutterMask         ;//屏幕区域杂波屏蔽图

	//CYUY2Frame m_oStatisticalFrame  ;//统计图
	CImageFrame m_oStatisticalFrame  ;//统计图
	EManualScreenAreadMode m_eManualScreenAreaMode;//手动指定的屏幕区域当前模式


	BOOL m_bMaskingClutter     ;   //自动屏蔽干扰光斑标志
	BYTE m_MaskingClutter      ;   //自动屏蔽干扰光斑时使用的判断门限

	CImageFrame  m_oManualOnLineScreenArea;   //绘制的静态屏蔽图


	//<<added by toxuke@gmail.com
	BOOL m_bInitialStage;//初始阶段
	BOOL m_bInitialFindSpot; //开始发现光斑
	UINT m_nIntialStageCount;//初始阶段计数器
	static const UINT  INITIAL_STAGE_WAIT_COUNT = 50;//初始状态等待次数
	//>>

	//<<水平引导线
	//int  m_nFirstGuidelineYPos ;//第一条水平引导线的位置
	//BOOL m_bShowFirstGuideline ;//显示第一条水平引导线
	//int m_nSecondGuidelineYPos ;//第二条水平引导线的位置
	//BOOL m_bShowSecondGuideline;//显示第二条水平引导线
	RECT   m_rcGuideRectangle    ;//引导矩形框的区域
//  RECT   m_rcGuideAbs          ;//绝对像素坐标引导矩形框的区域
//	RectF  m_rcGuideRel          ;//相对的引导矩形框的区域

    DWORD  m_dwGuideRectangleColor;//引导矩形框的颜色
    BOOL   m_bShowGuideRectangle  ;//引导矩形框是否可见标志


	int  m_nSpotProportion     ;
	int  m_nMultiEraser        ;

	BOOL m_bIsDynamicMasking       ; //是否正在自动屏蔽
    BOOL m_bDynamicMaskingChangedEvent;//自动屏蔽状态切换事件


	/////////modify by vera_zhao   2019.10.24
	BOOL  m_bIsAntiJamming          ; //是否进行抗干扰处理

	BOOL   m_bEnableOnlineScreenArea; //是否启用手动绘制的静态屏蔽图

	BOOL   m_bExistOnlineScreenArea; //是否存在手动绘制静态屏蔽图的数据；

	BOOL   m_bDisableReflectionSpot;   //是否响应反射点

	BOOL m_bIsStaticMasking        ;//是否静态屏蔽干扰点标志

    BOOL m_bIsClutterDetecting       ;//干扰点检测
    BYTE m_nDetectingClutterThreshold;//自动侦测干扰光斑时使用的判断门限


    //added by toxuke@gmail.com, 2013/04/19
    //手势识别使能标志
    //BOOL m_bGestureRecognitionEnable;

    BOOL m_bCalibratorFailed ;//校正失败标志，此时不要保存屏蔽图

    int m_nFPSNotifyCounter;//帧率通知计数器  

    //<added by jiqw 2015/04/28
    //<added reason:
        /*
        在安装向导的调试激光器时，会检测挡板是否消除，检测的方式为：
        （1）该模块Post光斑总面积和光斑个数到通知窗口
        （2）通知窗口会计算平均值，并与给定消除门限t比较，如果小于t，则认为挡板消失

        为了提高效率，不会在每一帧都post数据，而是间隔30帧post一次
        */
    //>
    int m_nBackSlashDataNotifyCounter;
    //>
    

    int m_nDynamicMaskUpdateCounter;//动态屏蔽更新计数器

    //CMouseEventGenerator   m_oMouseEventGen  ;//鼠标事件生成器。
    CVideToScreenMap       m_oVideo2ScreenMap;//视频坐标到屏幕坐标的映射对象。
    //CGestureEventGenerator m_oGestureEventGen;//手势识别器

    //const CIWBSensor* m_pIWBSensor         ;//关联的IWB串感器对象指针
    int m_nID;//编号

    //光斑检测在一个独立线程中完成, 光斑采样数据从主线程中设置。
    //因此需要线程同步
    CComAutoCriticalSection m_csForLightSpotSampleSize;


    //<<added by toxuke@gmail.com, 2014/11/21, 
    //校正相关变量
	BOOL m_bIsCalibrating ;//正在校正标志
	HWND m_hCalibrateWnd  ;//校正窗口句柄
	BOOL m_bPenControl    ;//鼠标的控制状态
    E_CALIBRATE_MODE m_eCalibrateMode;//校正模式

    //光斑采样相关变量
    //E_COLLECTSPOT_MODE m_eSpotSamplingMode;
    BOOL m_bIsSamplingSpot             ;
    HWND m_hSpotSamplingWnd            ;
    //>>

    //CContactPtFinder m_oContactPtFinder;
   
    //POINT m_ptContactVideoPos;//光笔的触板位置(视频坐标), 为了在CInterceptFilter::Transform中显示打印其位置,故声明成成员变量。


    ISpotListProcessor* m_pSpotListProcessor;//指向光斑处理器接口的指针


    //<<added by toxuke@gmail.com, 2014/12/16
    BOOL m_bDisplaySpotSizeInfo ;//是否显示光斑尺寸信息
    BOOL m_bDisplaySpotRect    ;//是否显示光斑外接矩形。
    //>>

  	UINT  m_nFixedBlobAutoMaskTime   ;   //固定光斑自动屏蔽的设置时间

    EDeviceTouchType m_eDeviceTouchType;//触控类型

    //<<2015/01/14
    ELensType   m_eLensType ;//镜头焦距类型
    //>>
  
    //帧捕获功能
    CImageFrame m_oCapturedFrame         ;//捕获帧。
    DWORD       m_dwImageCaptureStartTime;//捕获起始时刻。
    DWORD       m_dwImageCaptureDelayTime;//捕获延迟，单位:毫秒。
    BOOL        m_bImageCaptureDone      ;//捕获完成标志。    
    //
    //<added by jiqw 2015/05/11 15:21
    //在调试激光器时，智能检测光带是否消失
    bool        m_bHasDetectSVStarted;

   //<<debug
    //2015/11/03
    //手动再采样:指自动校正完成后, 再用光笔或者手指在校正符号处按下，采集光斑的视频坐标，以便分析自动校正和实际使用时的位置误差关系。
    HWND m_hManualResampleWnd;//手动再采样窗体句柄
    BOOL m_bManualResample   ;//手动再采样标志    
    //debug>>

    //std::vector<POINT> m_vecLightSpotContourPoints;//保存光斑轮廓点的数组.
	RECT m_rcMonitor;
	//<<2017/11/16
	BOOL m_bShowCalibratePoints;//调试功能, 在画面中显示校正点位置
	//>>

	//<<调试工具
	TCameraDebugData m_vecDebugData[MAX_OBJ_NUMBER];
	//>>
	std::vector<CPoint>  m_vecOnLinePt ;    //用来保存绘制屏蔽图时的采集点

	static const  double SCREEN_DISTANCETWOSPOT_WIDTH_IN_MM;
	double m_ScreenMinDistanceWidthInPixel ;


    BOOL m_bMarkingBaseStone;//4点标定状态标志
    CBaseStoneMarker m_oBaseStoneMarker;//4点标定执行器

    //保存4点标定结果
    void SaveBaseStoneMarker();

    //载入4点标定结果
    void LoadBaseStoneMarker();

    void InitBaseStoneFilePath(TCHAR* buf, int nBufSize);
};

