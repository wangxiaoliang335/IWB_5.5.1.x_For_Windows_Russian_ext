#pragma once


class CUsbCameraDeviceList;
class CIWBSensor;


//IWB传感器对象的管理器
class CIWBSensorManager
{
public:

    CIWBSensorManager();


    ~CIWBSensorManager();

    //@功能:初始化
    //@参数: nSensorCount, CMOS传感器个数
    void Init(int nSensorCount);

    //@功能:反初始化
    void Uninit();


    //@功能:给IWBSensor对象分派摄像
    //@参数:Camera设备列表
    void AssignCamera(const CUsbCameraDeviceList& CameraList);


	//@功能:设置视频显示参数
	//@参数:hDispWnd,  视频显示窗体句柄
	//      hNotifyWnd, 播放消息接收窗体句柄
	//      rcVideoDispArea, 视频显示区域
	void SetVideoDispParams(HWND hDispWnd, HWND hNotifyWnd, RECT& rcVideoDispArea);


    //@功能:开始运行
    //@参数:
	//		nSensorID, 传感器编号(0~n), -1时指全体传感器
    BOOL StartRunning(int nSensorID = -1);



    //@功能:停止运行
    //@参数:nSensorID, 传感器编号(0~n), -1时指全体传感器
    BOOL StopRunning(int nSensorID=-1);


    //@功能:改变视频显示区域的尺寸
    void SetVideoDisplayArea(const RECT& rcNewDisplayArea);


    //@功能:切换设想头到指定的模式
    void SwitchToMode(ESensorLensMode eMode, int nSensorID=-1);

    //@功能:获取工作模式
    //@说明:全局的工作模式与第一个相机保持一致
    ESensorLensMode GetLensMode()const;

    //@功能:判断是否有处于正常使用模式下的相机
    BOOL HasNormalUsageCamera()const;

    //功能:执行自动校正操作
    //@参数:nSensorID, 传感器编号(0~n), -1时指全体传感器  
    //BOOL DoAutoCalibrate(int nSensorID);


    //功能:执行手动校正操作
    //@参数:nSensorID, 传感器编号(0~n), -1时指全体传感器
    //BOOL DoManualCliabrate(int nSensorID);



    //@功能:根据视频显示区域确定视频传感器实例。
    CIWBSensor* FindSensorInstance(const RECT& rcDisplayArea);

	const CIWBSensor* GetSensor(int nID) const;
	CIWBSensor* GetSensor(int nID);

    //@参数:绘制布局框架
    //@窗体:hWnd, 窗体句柄
    void DrawLayoutFrame(HWND hWnd = NULL);

    UINT GetSensorCount()const
    {
        return m_vecSensors.size();
    }


    //@功能:载入配置数据
    //@参数:sysCfgData, 所有图像传感器的配置信息
    void SetCfgData( TSysConfigData& sysCfgData);

	void SetGlobalCfgData(TSysConfigData& sysCfgData);

    //@功能:获取配置数据
    //@参数:sysCfgData, 所有图像传感器的配置信息
    BOOL GetCfgData(TSysConfigData& sysCfgData);


    //@功能:USB摄像头插入事件响应
    //@参数:lpszDevicePath, 插入的USB设想头的设备路径
    //void OnCameraPlugIn(LPCTSTR lpszDevicePath);
    void OnCameraPlugIn(const TCaptureDeviceInstance& devInst);

    //@功能:USB摄像头拔出事件响应,  插入的USB设想头的设备路径
    void OnCameraPlugOut(LPCTSTR lpszDevicePath);

    //@功能:根据位置确定图像是属于哪个传感器的
    //@返回:非空，返回指向传感器对象的指针
    //      NULL,未找到。
    CIWBSensor* SensorFromPt(const POINT& ptPos);

    CIWBSensor* GetSensor();
	//CIWBSensor* GetSensor0();
	void SelectAsCurrentSensor(CIWBSensor* pSensor);

	int   CurrentSelectSensor();

    //@功能:使能光笔
    //@参数:bEnable,使能标志
    void EnableOpticalPen(BOOL bEnable);

    //@功能:判断光笔是否在控制中
    //@说明:只要有一支光笔在控制中,则返回TRUE
    BOOL IsOpticalPenControlling();


    //@功能:开始图像传感器的自动校正流程
    //@参数:ePattern, 校正图案模式
    //      hNotifWindow, 校正结束后的通知消息"WM_AUTO_CALIBRATE_DONE"的接收窗体
    //      nSensorID, -1,全部传感器校正
    void  StartAutoCalibrate(E_AutoCalibratePattern ePattern, HWND hNotifWindow, int nSensorID=-1);

    //@功能:一个图像传感器自动校正完成后的事件响应函数
    //@参数:bSuccess, 成功/失败标志
    //      bSimulateMode,模拟校正模式
    void OnIWBSensorAutoCalibrateDone(BOOL bSuccess, BOOL bSimulateMode);


    //@功能:开始图像传感器的手动校正流程
    //@参数:
    //      hNotifWindow, 校正结束后的通知消息"WM_FINISH_MAUNUAL_CALIBRATING"的接收窗体
    //      nPtsInRow, 每行校正点个数， -1: 表示从配置文件中读取
    //      nPtsInCol, 每列校正点个数， -1: 表示从配置文件中读取
    //      nSensorID, -1,全部传感器校正
    void  StartManualCalibrate(HWND hNotifyWindow, int nPtsInRow = -1, int nPtsInCol = -1, int nSensorID=-1, EManualCalibrateType eManualCalibType = E_MUNUAL_CALIB_FULL_SCREEN);

    //@功能:一个图像传感器手动校正正完成后的事件响应函数
    //@参数:bSuccess, 成功/失败标志    
    void OnIWBSensorManualCalibrateDone(BOOL bSuccess, DWORD dwCtxData);

    //@功能:判断校正是否成功
    BOOL IsCalibarateOk();

    //@功能:静态屏蔽屏幕内的干扰光斑
    void StaticMaskClutter();


    //@功能:开启自动屏蔽流程
    //@参数:hNotifWindow, 校正结束后的通知消息"WM_SEARCH_SCREEN_AREA_DONE"的接收窗体
    //      nSensorID, 传感器ID; -1时,为全部传感器校正。
    void StartSearchMaskArea(HWND hNotifWindow, int nSensorID=-1);



    //@功能:对"屏蔽区搜索完毕"事件的响应函数
    //@参数:bSuccess, 成功/失败标志
    void OnIWBSensorSearchMaskAreaDone(BOOL bSuccess);


    //@功能:开启光斑采集功能
    void StartLightSpotSampling(HWND hNotifWindow, int nSensorID=-1);


    //@功能:光斑采集结束事件的响应函数
    //@参数:bSuccess, 成功失败标志
    void OnIWBSensorLightSpotSamplingDone(BOOL bSuccess,int nSensorId);

    //@功能:判断所有的采样已经完成
    BOOL AllSamplingIsDone();


    //@功能:返回光斑列表处理器
    CSpotListProcessor& GetSpotListProcessor(){return *m_pSpotListProcessor;}


    //@功能:判断传感器设备是否在运行
    //@参数;无
    //@说明:只要有一个传感器在运行，就认为处于运行状态
    BOOL IsRunning()const;


    //@功能:判断所有传感器设备停止运行
    BOOL IsAllSensorStopped()const;


    //调试工具相关函数
    //@功能:判断是否正在录像
	BOOL IsRecording() const;

	//@功能:开始录像
	BOOL StartRecording();

	//@功能:停止录像
	BOOL StopRecording();

    //@功能:设置视频数据为AVI录像文件
    //@参数:lpszVideoPath1,传感器1的路径
    //      lpszVideoPath2,传感器2的路径
    void LoadSrcFromAVI(LPCTSTR lpszVideoPath1, LPCTSTR lpszVideoPath2);

    //@功能:交换两个图像传感器的显示画面
    //void SwapSensorImage();


    //@功能:交换两个图像传感器的显示画面
    //@参数:第一个图像传感器的Id
    //      第二个图像传感器的id
    void SwapSensorImage(UINT firstSensorId, UINT secondSensorId);

    //@功能:通过读取录像文件再现自动校正过程
    //@参数:nSensorId, 传感器Id
    //      hNotifyWnd, 校正完毕后的消息通知窗体
    //      lpszAVIFileName, 录像文件
    //      cBrightness, 校正时的画面平均亮度
    //      eGridNumber, 校正棋盘格列数
    BOOL DoSimulateAutoCalibrate(int nSensorId,  HWND hNotifyWnd, LPCTSTR lpszAVIFileName, SIZE szScreen, BYTE cBrightness, E_AutoCalibratePattern eGridsNumber);


    //@功能:屏幕分辨率变化事件响应函数
    //@参数:nScreenWidth, 新的屏幕宽度
    //      nScreenHeight,新的屏幕高度
    void OnDisplayChange(int nScreenWidth, int nScreenHeight);

    /*
    @功能：开启智能检测挡板是否消失
    */
    void StartDetectBackSplashVanished();

    /*
    @功能：停止智能检测挡板是否消失
    */
    void StopDetectBackSplashVanished();


      //@功能:查看校正符号位置
      //@参数:bShow,TRUE,显示符号
      //            FALSE, 隐藏符号
      //@Author:15077727@qq.com, 2015/09/09
      BOOL CheckCalibrateSymbols(BOOL bShow);



      //判断是否校正符号是否已经显示
     BOOL IsCalibrateSymbolVisible()const;

      //>>

    BOOL CalibrateSymbolManualResample();


    //Added by 15077726@qq.com
    //2015/11/9
    //功能:判断是否已经完成校正
    BOOL IsCalibrated()const;

    //功能:判断是否有校正成功的相机存在
    BOOL HasCalibratedCamera()const;


	void OnTimer(LPVOID lpCtx);

    CScreenLayoutDesigner& GetScreenLayoutDesigner() {return m_oScreenLayoutDesigner;}


    void ApplyScreenLayout();

    void  Start4BasePointMarking(HWND hNotifyWindow, int nSensorID);

    //@功能:4点标定结束响应函数
    void OnIWBSensor4BasePointMarkingDone(BOOL bSuccess);
	void EnableOnlineScreenArea(BOOL bEnable);
	BOOL IsEnableOnlineScreenArea();

protected:
    void UpdateCalibrateToleranceDistribute();
    void UpdateVideoLayout(const RECT& rcDisplayArea);
	void DrawSelectBound(HWND hWnd);
	static const int SELECT_BOUND_WIDTH = 5;

    std::vector<CIWBSensor*> m_vecSensors;
    
    //保存上一次传感器状态的数组
    std::vector<ESensorLensMode> m_vecLastSensorLensMode;
    
    //显示其查找类实例
    //CDispDevFinder      m_oDispMonitorFinder;

    //自动校正对象
    //CAutoCalibrator          m_oAutoCalibrator;

    
    //光斑处理器,负责产生手势、鼠标、触屏事件
    //<<避免耗尽1M左右的堆栈区域, 改为在堆(heap)中生成CSpotListProcessor对象
    //CSpotListProcessor      m_oSpotListProcessor;
    CSpotListProcessor*       m_pSpotListProcessor;
    //>>


    //光斑采集对象
    CCollectSpotSize   m_wndLightSpotSampling;


    static const int SPLITTER_WIDTH = 5;

    //相机画面显示窗体的布局
    std::vector<RECT> m_vecVideoLayout;//视频布局
    std::vector<RECT> m_vecSplitter   ;//分割条数组


    //屏幕布局设计工具, 允许每个屏幕大小不一致
     CScreenLayoutDesigner m_oScreenLayoutDesigner;


    HWND m_hVideoDispWnd;//视频显示窗体

    CWGRUtilContextImpl m_execContext;  //windows 系统下手势识别上下文执行环境


    //======自动校正相关参数========================
    enum EOperationMode
    {
        E_MODE_ALL_SENSOR,   //自动校正所有传感器
        E_MODE_SINGLE_SENSOR //校正单个传感器
    };

    EOperationMode  m_eOperationMode;
    HWND m_hNotifyWindow;
    E_AutoCalibratePattern m_eAutoCalibratePattern;

    int m_nAutoCalibrateTryTimes                    ;  //自动校正尝试次数
    int m_nCurrentSensorID                          ;  //正在校正的图像传感器的ID
    static const int MAX_AUTOCALIBRATE_TRY_TIMES = 1;  //自动校正尝试次数。
    std::vector<BOOL> m_vecCalibrateResults         ;  //保存每个传感器校正结果的数组,说明需要预先保存每个传感器的校正结果。因为传感器滤光片需要
                                                       //在所有传感器都校正完成后才能够一齐合上。
    //BOOL m_bAllScreenMergedOnOnePlane               ;//所有屏幕融合在一块平面上的标志
	UINT m_uCurrentSelectSensordId;                    //当前选中的Sensor id


};