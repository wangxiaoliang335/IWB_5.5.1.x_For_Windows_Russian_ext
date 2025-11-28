#pragma once
#include "CalibrationDataDef.h"
#include "../inc/AVI/AVIWriter.h"

#include <vector>
#include "../inc/DispMonitorFinder.h"
#include <queue>
/////////////////////////得到最大光斑的结构体
//光斑采样数据
typedef struct TLightSpotSampleSize
{
	POINT ptCenter;    //中心点的坐标
	LONG  lSize;       //光斑的面积的大小
	float fPercentage ;

}LightSpotSampleSize;


//保存一个监视器的光斑最大尺寸信息的一维数组。
typedef std::vector<LightSpotSampleSize> MONITOR_LIGHTSPOT_SAMPLE_SIZE;
struct  TMonitorLightSpotSampleSize
{
    RECT rcMonitor;
    MONITOR_LIGHTSPOT_SAMPLE_SIZE vecSampleSize; 
};

//

//保存所有监视器的光斑最大尺寸信息的二维数组
typedef std::vector<TMonitorLightSpotSampleSize> ALL_LIGHTSPOT_SAMPLE_SIZE;


typedef struct 
{
	double x;
	double y;
}DoublePoint;

//光斑采样位置归一化到(0,1)后的采样信息
typedef struct TNormalizedLightSpotSampleSize
{
	DoublePoint dptNormalized;
	LONG lSize;
	FLOAT fPercentage;

}NormalizedLightSpotSampleSize;

typedef std::vector<NormalizedLightSpotSampleSize> MONITOR_NORMALIZED_SMAPLE_SIZE;
typedef std::vector<MONITOR_NORMALIZED_SMAPLE_SIZE> NORMALIZED_SMAPLE_SIZE;


//采样符号信息
typedef struct 
{
    POINT ptDisplay         ;//显示位置坐标

    SIZE  size              ;       //采样符号的尺寸尺寸
    COLORREF clrSampleBefore;       //采样前十字的颜色
    COLORREF clrSampleAfter ;       //采样后的十字的颜色
	COLORREF clrSampleAdjustment;   //采样调整的十字的颜色
    BOOL     bSampled       ;//已采样标志
    POINT ptCenter          ;//双线性插值是使用的坐标,为了更准确地进行双线性插值，
                             //我们将靠近屏幕边界的采样位置挪到屏幕边界处。
	RECT    rcRect          ; //这个点所在的区域

}TSampleSymbol;


//光斑采样模式
//enum ESampleCollectPattern
//{
//    E_SAMPLE_COLLECT_PATTERN_9_Points, //9点采样，单屏模式的采样
//    E_SAMPLE_COLLECT_PATTERN_15_Points,//15点采样，双屏模式的采样
//};

struct ValidParamsInfo
{
	RECT Area;
	int mass;
};

struct SpotManualCollectInfo
{
	TPoint2D ptPos;
	RECT  rcArea;
	int   mass;
};
class  SpotManualCollectInfoManager
{
public:
	void Put(const SpotManualCollectInfo& data)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csForQueue);
		m_queManualCollectInfo.push(data);
	}

	 BOOL Fetch(SpotManualCollectInfo* data)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csForQueue);
		if(m_queManualCollectInfo.empty())
		{
			return FALSE;
		}
		if (data)
		{
		    *data = m_queManualCollectInfo.front();
		}
		m_queManualCollectInfo.pop();
		return TRUE;
	}
protected:
	CComAutoCriticalSection m_csForQueue;
	std::queue<SpotManualCollectInfo> m_queManualCollectInfo;
};

class CCollectSpotSize
{
public:
	CCollectSpotSize(E_COLLECTSPOT_MODE eCollectSpotMode = COLLECTSPOT_MODE_COLLECT, HWND hOwner = NULL);   //构造函数
	~CCollectSpotSize(void);                //析构函数

    //@功能:开始光斑采样
    //@参数:pMonitors, 屏幕区域数组
    //      nMonitorCount, 屏幕个数
    //BOOL  StartCollectSpotSize(const RECT* pMonitorAreas, int nAreaCount, HWND hNotifyWnd, ESampleCollectPattern ePattern = E_SAMPLE_COLLECT_PATTERN_9_Points);
    BOOL  StartCollectSpotSize(const RECT* pMonitorAreas, int nAreaCount, HWND hNotifyWnd, int nSampleNumEachRow, int nSampleNumEachCol,int nSensorId, TCHAR *lpszbuf, int nSensorCount);

    

    //@功能:判断是否放弃光斑采用    
    //@返回值:TRUE, 结束采样
    //        FALSE, 继续采样
    BOOL AbortCollectSpotSize();//结束

    const ALL_LIGHTSPOT_SAMPLE_SIZE& GetScreenSamples() const {return m_ScreenLightspotSample;}


    //@功能:屏幕分辨率变化事件响应函数
    //@参数:nScreenWidth, 新的屏幕宽度
    //      nScreenHeight,新的屏幕高度
    void OnDisplayChange(int nScreenWidth, int nScreenHeight);


    //added by toxuke@gmail.com, 201/5/03/09
    //<<判断光斑采样窗体是否可见
    BOOL IsVisible() { return ::IsWindowVisible(m_hWnd);}
    //>>

    //<<added by toxuke@gmail.com, 201/5/03/09
    //@功能:设备丢失时的事件响应函数
    void OnDeviceMissing();
    //>>

	BOOL LoadCollectSpotPoint();
	BOOL SaveCollectSpotPoint();

protected:
	BOOL Create();                          //创建窗体

	void FullScreen(BOOL bFull);
	BOOL IsFullScreen(){ return m_bFullScreen;}

	HWND GetOwnerWnd()const;
	void SetOwnerWnd(HWND hWnd);
	

	static LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);   //回调函数
	LRESULT  InternalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);       //运行函数
	void  DrawCross(HDC hDC, const POINT& ptCenter, COLORREF color,  const SIZE& size); //画出点击点
	void  DrawLine(HDC hDC, const POINT& ptStart ,const POINT& ptEnd, COLORREF color);
public:

       HWND      m_hWnd;

protected:

	HINSTANCE m_hInstacne;

	HWND      m_hOwner;                                    //所有者窗体,用来进行事件通知

	const TCHAR*   m_szClassName;
	WINDOWPLACEMENT   m_OldWndPlacement;
	
	BOOL        m_bFullScreen;

	static const int MAX_SPOT_SAMPLING_TIMES  = 30;  //一个点上采集12次在同一个点上。说明就是需要采集到的信息
	static int s_nSpotSettingCount;


//int DoubleScreenMergeCount ; //双拼和非双拼采集的个数不一样。

//	CAviWriter  m_oAVIRecorder;          //保存图像的对象


protected:
    //方法
    
     //初始化采样位置
    void InitSamplePosition(const RECT &rcMonitor);

    //变量
	int                               m_nSampleTimes     ;//单个点的采样次数计数器

//	POINT                             m_ptSampleStartPos  ;//单个采样点,第一次采样时的位置

    RECT                              m_rcCurrentMonitor  ;//当前显示器的屏幕坐标
    int                               m_nCxVScreen        ;//虚拟桌面的宽度
	int                               m_nCyVScreen        ;//虚拟桌面的高度
	int                               m_nSampleNumEachRow ;//每行的采样次数
	int                               m_nSampleNumEachCol ;//每列的采样次数
    int                               m_nCurrentSampleNo  ;//当前校正符号编号,从0开始
	std::vector<TSampleSymbol>        m_vecSampleSymbols  ;//校正符号信息

	//std::vector<POINT>                m_vecStandardCross;//标准的坐标点
	//std::vector<LightSpotSampleSize>  m_vecMaxSpot      ;//得到设置的点的坐标和面积。

	std::vector<ValidParamsInfo>         m_vecSampleData     ;//保存MAX_SPOT_SAMPLING_TIMES次采样的数据的数组

	int  m_nSensorID ;                 //在进行第几个的sensor

    ALL_LIGHTSPOT_SAMPLE_SIZE         m_ScreenLightspotSample;//所有监视器的光斑采样点, 一个监视器对应一组采样点


	std::vector<RECT>                m_vecMonitorAreas    ;//屏幕显示器信息
    int                              m_nCurMonitorAreaId  ;//当前显示器Id

	int                             m_nSelectDragIndex    ;//选择拖拽的点的索引
	std::vector<POINT>              m_vecConfigCross;//标准的坐标点

	enum E_SPOTSAMPLING_STATE
	{
        E_ALL_SPOT_SAMPLING_START         , //开始所有光斑的采集
        E_CURRENT_SYMBOL_SAMPLING_START   , //当前符号处的光斑采样开始
        E_CURRENT_SYMBOL_CONTINUE_SAMPLING, //当前符号处的光斑采集中
        E_CURRENT_SYMBOL_SAMPLING_END     , //当前符号处的光斑采集结束
        E_ALL_SPOT_SAMPLEING_END          , //所有光斑的采集结束
	}m_eSpotSamplingState;

    //E_COLLECTSPOT_MODE m_eSpotSamplingMode    ;//光斑采集模式,手动,模拟等
    //ESampleCollectPattern m_eSpotSamplePattern;//

    int    m_nSymbolHorzInterval ;//校正符号水平间隔
    int    m_nSymbolVertInterval ;//校正符号垂直间隔
    

    int    m_nLightSpotDitherOffsetX;//水平方向的抖动偏移距离
    int    m_nLightSpotDitherOffsetY;//垂直方向的抖动偏移距离

	TCHAR     CollectSpotDragPath[MAX_PATH];
	int       m_nCollectSensorCount;                 //需要采集光斑的sensor个数
	BOOL      m_ShowFlag           ;

	public:
	  static SpotManualCollectInfoManager m_oSpotManualCollectInfoManager;
	
};


