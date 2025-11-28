#pragma once
//说明:视频中检测到的光笔坐标到屏幕坐标的转化类
//双击事件检测器
class CDoubleClickChecker
{
public:
		CDoubleClickChecker()
		{
			m_ptLastLeftDownPos.x = 0;
			m_ptLastLeftDownPos.y = 0;
			m_pLastLeftDownTime  = 0;
		}

		void Check(const POINT& ptLeftDownPos)
		{

			//输入数据
			INPUT input;
			memset(&input, 0 ,sizeof(input));
			input.type = INPUT_MOUSE;

			DWORD dwNow = GetTickCount();

			if((dwNow - m_pLastLeftDownTime) < DOUBLE_CLICK_MAX_INTERVAL)
			{

				LONG dx = abs(ptLeftDownPos.x - m_ptLastLeftDownPos.x);
				LONG dy = abs(ptLeftDownPos.y - m_ptLastLeftDownPos.y);

				if(dx < MOUSE_LEFT_STATIC_PERMIT_MAX_SHIFT && dy < MOUSE_LEFT_STATIC_PERMIT_MAX_SHIFT  && !(dx ==0 && dy == 0)/*自动产生*/)
				{

					//用连续两次单击来模拟双击事件		
					input.mi.dwFlags  =  MOUSEEVENTF_LEFTUP;
					SendInput(1, &input, sizeof(INPUT));

					input.mi.dwFlags  =  MOUSEEVENTF_LEFTDOWN;
					SendInput(1, &input, sizeof(INPUT));
				}
			}

			m_ptLastLeftDownPos  = ptLeftDownPos;
			m_pLastLeftDownTime = dwNow;
		};

protected:
	POINT m_ptLastLeftDownPos;
	DWORD m_pLastLeftDownTime;//上一次左键按下时刻。
    static const UINT DOUBLE_CLICK_MAX_INTERVAL = 1000u;
	static const DWORD MOUSE_LEFT_STATIC_PERMIT_MAX_SHIFT  = 20*65535/768;//连续两次左击(归一化到0~65535)小于该值,则认为鼠标位置未发生变化
																		  //在同一处连续单击，触发鼠标双击事件。
};



//右键事件触发器
class CRightClickTrigger
{
public:
	CRightClickTrigger()
	{
		m_ptRangeCenter.x   = (std::numeric_limits<long>::min)();
		m_ptRangeCenter.y   = (std::numeric_limits<long>::min)();
		m_dwRangeUpdateTime = GetTickCount();

	}

	void Reset()
	{
		m_ptRangeCenter.x   = (std::numeric_limits<long>::min)();
		m_ptRangeCenter.y   = (std::numeric_limits<long>::min)();
		m_dwRangeUpdateTime = GetTickCount();
	}

	//@功  能:检测鼠标是否在一区域内长时间停留
	//@返回值:TRUE, 如果触发了鼠标右键事件
	//        FALSE, 未触发鼠标右键事件
	BOOL Process(const POINT& ptNow)
	{

		LONG dx = abs(ptNow.x - m_ptRangeCenter.x);
		LONG dy = abs(ptNow.y - m_ptRangeCenter.y);
		if(dx > MOUSE_RIGHT_CLICK_RANGE || dy > MOUSE_RIGHT_CLICK_RANGE)
		{
			m_ptRangeCenter = ptNow;//区域移动
			m_dwRangeUpdateTime = GetTickCount();
		}
		else
		{
			DWORD dwNow = GetTickCount();
			int  x = MOUSE_RIGHTDOWN_EVENT_TRIGGER_TIMER;

			if( (dwNow - m_dwRangeUpdateTime) > MOUSE_RIGHTDOWN_EVENT_TRIGGER_TIMER)
			{
				INPUT input;
				memset(&input, 0 ,sizeof(input));
				input.type = INPUT_MOUSE;

				input.mi.dx       = 0;
				input.mi.dy       = 0;
				input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
				SendInput(1, &input, sizeof(INPUT));

                AtlTrace(_T("[Thread(0x%08x)]A.Left Up\n"), GetCurrentThreadId());
								

				input.mi.dwFlags  = MOUSEEVENTF_RIGHTDOWN;
				SendInput(1, &input, sizeof(INPUT));

                AtlTrace(_T("[Thread(0x%08x)]B.Right Down\n"), GetCurrentThreadId());
	

				input.mi.dwFlags  = MOUSEEVENTF_RIGHTUP;
				SendInput(1, &input, sizeof(INPUT));

                AtlTrace(_T("[Thread(0x%08x)]C.Right Up\n"), GetCurrentThreadId());

				return TRUE;
			}


		}

		return FALSE;
	}

protected:
	POINT m_ptRangeCenter;
	static const DWORD MOUSE_RIGHT_CLICK_RANGE = 2*65535/768;//在检测右键触发事件时要求鼠标停留的区域的半径。
	                                                         //当鼠标在该区域范围内超过一段时间即认为可触发右键
	                                                         //单击事件
	DWORD m_dwRangeUpdateTime;
	static const DWORD MOUSE_RIGHTDOWN_EVENT_TRIGGER_TIMER = 1500;//鼠标右键按下事件触发的时间
};


//光笔控制源
enum ECtrlPenLightSource
{
	E_CTRL_PEN_LIGHT_SOURCE_RED_LASER,     //红色激光笔
	E_CTRL_PEN_LIGHT_SOURCE_INFRARED,      //红外光笔
};

class CVideToScreenMap;
class CMouseEventGenerator
{
public:

	CMouseEventGenerator(CVideToScreenMap* pVideo2ScreenMap);
	
	~CMouseEventGenerator();

	//@功能:产生鼠标事件
	//@参数:pPtVideo, 指向目标屏幕坐标的指针,为NULL则意味着没有检测到目标
	//      bDebug, 调试标志。
	void GenMouseEvent(const POINT* pPtScreen, BOOL bDebug = FALSE);

    //@功能:产生滚轮事件
    //@参数:pPtScreen, 指向目标屏幕坐标的指针
    //      MouseWheelLength,滚动长度
	void GenMouseWheelEvent( const POINT* pPtScreen, int MouseWheelLength);

	//@功能:进入校正模式
	//@参数:
	//void EnterCalibrateMode(HWND hAdjustWnd, E_CALIBRATE_MODE eCalibrateMode);

	//@参数:离开校正模式
	//void LeaveCalibrateMode();

	/*void EnableOpticalPenControl(BOOL bEnable)
	{
		m_bPenControl = bEnable;
	}

	BOOL IsOpticalPenControlling() const
	{
		return m_bPenControl;
	}*/


	//CVideToScreenMap m_oVideo2ScreenMap;



	/*E_CALIBRATE_MODE GetCalibrateMode()const
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
	}*/

	//@功能:手动校正
	//@参数:校正亮斑位置输入
	//void DoManualCalibrate(POINT pt);

	//@功能:手动采集光斑的大小
	//@参数:采集亮斑位置输入和面积输入 //add by zhaown
    /*
	void DoSpotSampling(POINT pt,LONG nlArea);
	void DoSimulateCollectSpotSetting(POINT pt,LONG m_nlArea);
//	void GetImageFrame(const CYUY2Frame* pImage);//得到图像的信息
	void EnterSpotSamplingMode(HWND hSettingWnd,E_COLLECTSPOT_MODE eSpotSettingMode);
	void LeaveSpotSettingMode();
	E_COLLECTSPOT_MODE GetCollectSpotMode()const{return m_eSpotSamplingMode;}
	HWND  GetSpotHWnd()const {return m_hCollectSpotWnd;}
	BOOL  IsCollectingSpot()const {return m_bIsCollectingSpot;}

    */

	//@功能:自动校正
	//@参数:校正图像
	void DoAutoCalibrate(const CYUY2Frame* pImage);



	//@功能:切换光笔控光源
	//@参数:eCtrlPenSrc, 光笔控制源
	void SetCtrlPenSrc(ECtrlPenLightSource eCtrlPenSrc);
	ECtrlPenLightSource GetCtrlPenSrc();

    //@功能:运行状态机
    void RunStateMachine(BOOL bFindPen, const POINT& ptPenPos);

	//@功能:运行激光笔时的状态机
	void RunLaserPenStateMachine(BOOL bFindPen, const POINT& ptPenPos);

	//@功能:运行红外光笔时的状态机
	void RunInfraredPenStateMachine(BOOL bFindPen, const POINT& ptPenPos);


	//@功能:模拟鼠标插值操作
	BOOL SimulateMouseInterpolate(LPCTSTR lpszDataFileName);
	//


    //功能:设置鼠标
   void SetMouseDeviationCorrect(int nXDeviation, int nYDeviation);
   //>>

protected:	
	//@功能:鼠标右键事件鼠标静止判断
	//
	//BOOL IsMouseRightStatinonary(const POINT& ptNow, const POINT& ptLast);

	//@功能:鼠标左键事件鼠标静止判断
	//BOOL IsMouseLeftStatinonary(const POINT& ptNow, const POINT& ptLast);


	//@功能:鼠标事件生成线程
	static ULONG WINAPI GenSysMouseEventThreadProc(LPVOID lpCtx);


	//void QueueMouseData(const INPUT& data);

	POINT m_ptLastPenAbsolutePos;//上一次的目标屏幕坐标
	//DWORD m_dwTimeLButtonDownStationary    ;//鼠标左键按下静止时刻
	//static const DWORD MOUSE_RIGHTDOWN_EVENT_TRIGGER_TIMER = 1500;//鼠标右键按下事件触发的时间

	//static const DWORD MOUSE_RIGHT_STATIC_PERMIT_MAX_SHIFT = 2*65535/768; //鼠标垂直和水平移动距离(归一化到0~65535)小于该值,则认为鼠标静止
	//static const DWORD MOUSE_LEFT_STATIC_PERMIT_MAX_SHIFT  = 100*65535/768;//连续两次左击(归一化到0~65535)小于该值,则认为鼠标位置未发生变化
	//																	  //在同一处连续单击，触发鼠标双击事件。


	BOOL m_bIsCalibrating;//正在校正标志
	HWND m_hCalibrateWnd  ;//校正窗口
	BOOL m_bPenControl;//鼠标的控制状态

	//E_CALIBRATE_MODE m_eCalibrateMode;//校正模式

	CDoubleClickChecker m_oDoubleClickChecker;
	CRightClickTrigger  m_oRightClickTrigger;

	//CBezierInterpolator<5> m_oInterpolater;
    CAkima_Interpolator<5>  m_oAkimaInterpolator;
	//光笔控制源
	ECtrlPenLightSource  m_eCtrlPenSrc;

	//插值调试代码
	//记录左键按下至弹起时间段的光笔的输入位置。
	std::ofstream m_ofLog;//文件对象

	BOOL m_bEnableInterpolate;//使能插值标志

	//在调用Window API函数SendInput实现对鼠标进行线性插值时, 
	//一个插值序列中的两个插值点之间需要延时1ms左右, 结果是只有插值序列的第一个和最后一个起作用,
	//中间的插值都会被系统忽略。	
	//HANDLE m_hGenSysMouseEventThread;//鼠标事件生成线程
	//HANDLE m_hForceExitEvent        ;//强制退出事件
	//HANDLE m_hNewMouseEvent         ;//

	//static const int MAX_MOUSE_NUMBER = 256;
	//INPUT                    m_aryMouseData[MAX_MOUSE_NUMBER];//
	//CComAutoCriticalSection  m_csForMouseData                ;//
	//int                       m_nMouseQueueDataCount              ;//鼠标数据个数


	///////////////////////设置函数中的变量zhaown

   E_COLLECTSPOT_MODE m_eSpotSamplingMode;
   BOOL m_bIsCollectingSpot;
   HWND m_hCollectSpotWnd;

   //CAlphaBetaSmoother<1> m_objMouseSmoother;

   //CStrokeSmoother<2> m_objStrokeSmoother;//笔画平滑器
   CStrokeSmoother<5> m_objStrokeSmoother;//笔画平滑器
   public:

	   enum E_SIMUL_MOUSE_STATE
	   {
		   SIMUL_MOUSE_STATE_IDLE             ,//模拟鼠标空闲状态
		   SIMUL_MOUSE_STATE_LEFT_DOWN        ,//模拟鼠标左键按下状态
		   SIMUL_MOUSE_STATE_RIGHT_CONTROL    ,//模拟鼠标右键控制状态
		   SIMUL_MOUSE_STATE_MOVE             ,//模拟鼠标的移动状态  
	   }m_eSimulMouseState;

	   enum E_SIMUL_MOUSE_POSSAGE_STATE
	   {
		   SIMUL_MOUSE_POSSAGE_STATE_IDLE    ,
		   SIMUL_MOUSE_POSSAGE_STATE_DOWN    ,
		   SIMUL_MOUSE_POSSAGE_STATE_MOVE    ,
	   }m_eSimulMousePossageState;


	   /////功能：设置鼠标的状态
	   void SetSimulMouseResponseState(E_SIMUL_MOUSE_STATE eSimulmouseState);

       //<<调试
       std::ofstream m_filePosData;//保存光笔位置信息的数据文件。
       //>>

	   DWORD m_dwLastMouseEventTime;

	  CVelocityCompensator m_oVelocityCompensator;
      CVideToScreenMap*    m_pVideo2ScreenMap;

};

__inline BOOL operator==(const POINT& right, const POINT& left)
{
   return (right.x == left.x && right.y == left.y);
}

__inline BOOL operator!=(const POINT& right, const POINT& left)
{
   return (right.x != left.x || right.y != left.y);
}

//extern CMouseEventGenerator g_oMouseEventGen;