	#pragma once

//手动校正参数
enum EManualCalibrateType
{
    E_MUNUAL_CALIB_FULL_SCREEN = 0,//
    E_MUNUAL_CALIB_CIRCLE      = 1,//原型校正
};

struct TManualCalibrateParameters
{
    HWND                     hNotifyWnd;//通知消息接收窗体
    std::vector<TScreenInfo> vecScreenInfos ;//关联的显示器信息
    int                      nCalibratePointsInRow         ;//每行的校正点个数
    int                      nCalibratePointsInCol         ;//每列的校正点个数
    EManualCalibrateType    eManualCalibrateType;//手动校准类型
};


class CManualCalibrateWnd:public CCalibrationWndBase
{
public:
	CManualCalibrateWnd();

	~CManualCalibrateWnd();


	//@功能:开始手动校正
    //@参数:parameters, 手动校正相关参数
	//@说明:全屏显示校正窗口,并计算校正点的坐标。
	void StartCalibrate(const TManualCalibrateParameters& parameters, int ImageWidth, int ImageHaight);
   
	//@功能:放弃手动校正
	BOOL AbortCalibrate();

    const ALL_MOITOR_CALIB_DATA& GetCalibrateData()const {return m_oAllCalibMap;}

	int GetImageWidth() { return  m_nImageWidth; }

	int GetImageHeight() { return m_nImageHeight; }

    //@功能:设备丢失事件响应函数
    void OnDeviceMissing();


    //@功能:返回手动校正生成的屏幕屏蔽图
    const CImageFrame& GetScreenAreaMask()const { return m_oScreenAreaMask;}

    const TManualCalibrateParameters& GetCalibrateParams() const{ return m_calibrateParameters; }
protected:
	//@功能:绘制校正十字符号
	//@参数:hDC,
	//      ptCenter, 十字架中心的坐标
	//      color, 十字的颜色
	//      size, 十字的长宽和高
	//
	void DrawCarlibrationMark(HDC hDC, const POINT& ptCenter, COLORREF color, const SIZE& size);

	virtual LRESULT  WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

	//@功能:初始化校正点坐标	
	void  InitMonitorCalibrateSymCoord();

    void  InitRectCalibrateSymCoord();
    void  InitCircleCalibrateSymCoord();


	std::vector<TCrossSymbol> m_vecCrossSymbol            ;//校正点坐标
    

	//DWORD       m_dwTimeBegin           ;//校准动作起始时刻
    DWORD       m_dwLastSpotAppearTime;//上一次光斑显示时刻
	int         m_nCurrentCalibratePos    ;//当前校正点编号

	POINT       m_ptFirstInputImageCoord ;//得到的校正数据,为按比例放大的光斑图像坐标。

    //POINT       m_ptLastCalibrationCoord;//上一个校正符号的图像坐标

    
	
    //点校正状态
    enum E_PointCalibrateState
    {
        E_CURRENT_POINT_BEGIN_CALIBRATING   ,//当前点校正开始
        E_CURRENT_POINT_CALIBRATING         ,//当前点校正中
        E_WAIT_CURRENT_POINT_CALIBRATING_END,//等待当前校正点结束
        E_ALL_CALIB_DONE                    ,//全部校正完毕
    }m_ePtCalibState;//当前校正点校正状态机状态


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
            POINT ptLightSpot;//光斑位置
            UINT  uTimerId   ;//定时器Id
        };
    };


    //@功能:状态机过程函数
    //@参数:stimulus:输入的激励
    void RunCalibStateMachineProc(const TStateMachineInputStimulus& stimulus);

    int m_nLightSpotDetectionCount;//光斑检测次数

    static const int MAX_PERMIT_U_DITHER_DEVIATION = 2 << INT_SCALE_SHIFT_SIZE;//最大允许的由于光斑抖动引起的水平偏移量, 单位:视频坐标
    static const int MAX_PERMIT_V_DITHER_DEVIATION = 2 << INT_SCALE_SHIFT_SIZE;//最大允许的由于光斑抖动引起的垂直偏移量, 单位:视频坐标

    static const int MAX_DETECT_COUNT  = 20;//每个校正点从开始到坐标采集完毕时需要采集的次数
    static const int MIN_CALIB_PT_DISTANCE  = 25 << INT_SCALE_SHIFT_SIZE;//校正点间的最小距离
    
    static const int CALIB_END_TIME_THRESHOLD = 300;//判断"校正点校正结束"的光斑消失时间门限,单位:ms

        
    static const int PERIODIC_CHECK_EVENT_ID  = 1 ;//周期检测定时器Id
    static const int PERIODIC_CHECK_INTERVAL = 100;//100ms定时器
    
    UINT m_uPeriodicCheckTimerId;//校正时的定时器

    int             m_nCurMonitorId       ;//当前显示器Id

    ALL_MOITOR_CALIB_DATA  m_oAllCalibMap ;//所有屏幕的校正点图像坐标和屏幕坐标映射数组。

    TManualCalibrateParameters m_calibrateParameters;//校正参数
	int   m_nImageWidth ;        //手动校正时的图像的分辨率
	int   m_nImageHeight;

    CImageFrame m_oScreenAreaMask;//屏幕影像区域的屏蔽图,像素值为0xFF处对应屏幕区域, 像素值为0x00处对应屏蔽区域

    //@功能:根据手动校正点勾勒的轮廓更新屏幕影像屏蔽图
    //@参数:无
    void UpdateScreenAreaMask();
    void UpdateRectScreenAreaMask();
    void UpdateCircleScreenAreaMask();

	BOOL m_bRestoreTaskbar;//恢复状态栏标志

	DWORD m_dwCtxData;


    RECT m_rcBoundary;


};
