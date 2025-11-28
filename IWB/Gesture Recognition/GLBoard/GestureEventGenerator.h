#pragma once

//#include "assert.h"
//#include <vector>
//
////#include "headers.h"
//#include "WGestureRecPublic.h"
#include <GlobalDefine.h>
#include "GLBoardPublic.h"

/////长时间的停留在一个点上的类的处理
class CLongTimeClickTrigger
{
public:
	CLongTimeClickTrigger()
	{
		m_ptRangeCenter.x = (std::numeric_limits<long>::min)();
		m_ptRangeCenter.y = (std::numeric_limits<long>::min)();
		m_dwRangeUpdateTime = GetTickCount();
	}
	void Reset()
	{
		m_ptRangeCenter.x = (std::numeric_limits<long>::min)();
		m_ptRangeCenter.y = (std::numeric_limits<long>::min)();
		m_dwRangeUpdateTime = GetTickCount();

	}

	///@功能：检测鼠标位置是否在一区域内长时间的停留
	///@返回值：TRUE 如果触发了，说明是聚光灯的响应
	////////////FALUSE,未触发是说明是板擦的响应
	BOOL DetectLongTimeStay(const POINT& ptNow)
	{
		LONG dx = abs(ptNow.x - m_ptRangeCenter.x) ;
		LONG dy = abs(ptNow.y - m_ptRangeCenter.y) ;

		///说明这个鼠标的位置在范围之外
		if (dx > LONGTIME_CLICK_RANGE || dy > LONGTIME_CLICK_RANGE)
		{
			m_ptRangeCenter = ptNow ;
			m_dwRangeUpdateTime = GetTickCount();
		}
		else
		{
			DWORD dwNow = GetTickCount();
			if ( (dwNow - m_dwRangeUpdateTime) > LONGTIME_EVENT_TRIGGER_TIMER)
			{
				return TRUE;
			}
		}
		return FALSE;
	}

protected:
	POINT m_ptRangeCenter;
	static const DWORD LONGTIME_CLICK_RANGE  = 15;

	DWORD  m_dwRangeUpdateTime;
	static const DWORD LONGTIME_EVENT_TRIGGER_TIMER = 1500u;
};


////////////////////手掌双击类的实现
class CHandDoubleClickChecker
{
public:
	CHandDoubleClickChecker()
	{
		m_ptLastPos.x = (std::numeric_limits<long>::min)();
		m_ptLastPos.y = (std::numeric_limits<long>::min)();
		m_pLastPosTime = 0;
		m_StartHandState = TRUE  ;
	}

	void SetHandDoubleClickStartState(BOOL start)
	{
		m_StartHandState = start ;

	}


	BOOL HandDoubleClick(const POINT& ptPos/*,const POINT& preptPos*/)
	{
	//	AtlTrace(_T("ptPos.x = %d , ptPos.y  = %d \r\n") ,ptPos.x,ptPos.y );
	//    AtlTrace(_T("m_ptLastPos.x = %d , m_ptLastPos.y = %d \r\n") ,m_ptLastPos.x,m_ptLastPos.y );
		if (m_StartHandState)
		{
			DWORD dwNow = GetTickCount();

			m_StartHandState = FALSE    ;

			if ((dwNow - m_pLastPosTime) < DOUBLE_CLICK_MAX_INTERVAL)
			{
				LONG dx = abs(ptPos.x - m_ptLastPos.x) ;
				LONG dy = abs(ptPos.y - m_ptLastPos.y) ;

				if (dx < DWORD_HAND_STATIC_MAX_SHIFT && dy <DWORD_HAND_STATIC_MAX_SHIFT)
				{
					return TRUE;
				}
			}

			m_ptLastPos = ptPos;
			m_pLastPosTime = dwNow;
		}


		return FALSE;

	}
protected:

	POINT  m_ptLastPos     ;
	DWORD  m_pLastPosTime  ;
	BOOL   m_StartHandState    ;
	static const UINT DOUBLE_CLICK_MAX_INTERVAL = 1200u;
	static const DWORD DWORD_HAND_STATIC_MAX_SHIFT  = 20;

};


//手指双击类的实现
class CFingerDoubleClickCHecker
{
public:
	CFingerDoubleClickCHecker()
	{
		m_ptLastPos.x = (std::numeric_limits<long>::min)() ;
		m_ptLastPos.y = (std::numeric_limits<long>::min)() ;
		m_pLastPosTime = 0 ;
		m_StartState = TRUE ;
	}

	void SetDoubleClickStartState(BOOL start)
	{
		m_StartState = start ;

	}

	BOOL FingerDoubleClick(const POINT& ptPos)
	{
		if (m_StartState)
		{
			DWORD dwNow = GetTickCount();
			m_StartState = FALSE ;

			if ((dwNow - m_pLastPosTime) <DOUBLE_CLICK_MAX_INTERVAL)
			{
				LONG dx = abs(ptPos.x - m_ptLastPos.x ) ;
				LONG dy = abs(ptPos.y - m_ptLastPos.y ) ;

				if (dx < DWORD_HAND_STATIC_MAX_SHIFT && dy < DWORD_HAND_STATIC_MAX_SHIFT)
				{
					return TRUE ;
				}
			}

			m_ptLastPos = ptPos ;
			m_pLastPosTime = dwNow ;
		}

		return FALSE ;

	}

protected:
	POINT m_ptLastPos;
	DWORD m_pLastPosTime ;
	BOOL  m_StartState   ;
	static const UINT DOUBLE_CLICK_MAX_INTERVAL = 350u ;
	static const DWORD DWORD_HAND_STATIC_MAX_SHIFT = 10 ;

};

////////////////////////////////////////////////////////////////////////////
////两个光点移动方式
//typedef enum
//{
//    E_2TOUCHS_MOVES_FALSE       , //非两点
//    E_2TOUCHS_MOVES_NONE        , //未移动
//    E_2TOUCHS_MOVES_SAME        , //同向移动，漫游时的移动形式
//    E_2TOUCHS_MOVES_ROAM        ,
//    E_2TOUCHS_MOVES_SELECTION   , 
//    //E_2TOUCHS_MOVES_BACKWARD    , //背向移动，放大时的移动形式
//    //E_2TOUCHS_MOVES_OPPOSITE    , //相向移动，缩小时的运动形式
//    E_2TOUCHS_MOVES_ZOOM        , //缩放移动
//    E_2TOUCHS_MOVES_RORATE      , //旋转移动
//}E_2TOUCHS_MOVES;

class CGestureEventGenerator
{
public:
	CGestureEventGenerator(void);

	~CGestureEventGenerator(void);
	//@功能：得到光斑比例的函数
	void  SetSpotProportion(int nSpot);
	//@功能：设置板擦倍数的函数
	void  SetMultiEraser(int nEraser);
    int  GetMultiEraser();
	//@功能：发现白板窗体的函数
	BOOL  FindTboardWnd(const TLightSpot * aryLightSpots,  const int &nCount);

    //
    bool DoDetection(const TLightSpot * aryLightSpots,  const int &nCount/*, POINT* pptMouse = NULL*/);   

    //使能GLBoard手势识别
    void EnableGestureRecognition(bool bEnable);
    bool IsGREnabled();

    //设置日志开许可制
    void SetLogPermit(bool bPermit) {m_isLogPermit = bPermit;}

    //
    bool IsInputInGLBorad() {return m_isInuptInGLBoard;}

    //
    void SetMultiPenPermission(bool bPermission) {m_isMultiPenPermited = bPermission;}
    bool IsMultiPenPermitted() {return m_isMultiPenPermited;}

    //
    void SetIsTouchPadMode(bool bTouchPadMode) {m_isTouchPadMode = bTouchPadMode;}
    bool IsTouchPadMode() {return m_isTouchPadMode;}

    void SetIsPenTouchDevice(bool bPenTouchDevice) {m_isPenTouchDevice = bPenTouchDevice;}

    bool IsMultiPenMode() {GetGLboardButtonState(); m_isMultiPenMode = (StrStatus ==_T("multipen")); return m_isMultiPenMode;}

    void ResetSmartMathch();

    //@功能:设置触屏的尺寸事件响应函数
    //@参数:nPhysicalDiagonalLength, 屏幕尺寸,对角线长度, 单位:毫米
    //      szLogicalDimension, 屏幕逻辑尺寸，单位:像素
    void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension);

public:
	int      m_nSpotProportion    ; //光斑响应时的最小比例。
	int      m_nMultiEraser       ;//光斑被认为是板擦时, 光斑的面积与面积门限的最低之比
    int      m_CollectCount       ;//在进行手势之间的切换时，需要先采集几帧来判断是否真的需要切换，而不是产生的干扰点。

	BOOL     m_bGetstureDone      ;//TRUE:手势判断完毕,可以模拟鼠标事件; FALSE：手势判断中, 还不能够模拟鼠标事件。

    bool     m_bHadResetAllGRState; //是否已经通过输入0个点重置了所有的GLBoard手势

	POINT    m_ptInputCoord;       //在进行采集的时候用来纪录前一次的坐标

	POINT     m_Start_MousePoint[10];   //在进行鼠标滑轮的判断时，用来记录刚开始时鼠标的位置。'0'x坐标小的坐标值。    '1'//x坐标大的坐标值 
	POINT     m_Start_MidPoint     ;   //纪录开始时两个坐标之间的中间坐标值
	POINT     m_Last_Multipoint[10] ;   //在进行鼠标滑轮的判断中，纪录上一次的坐标的值
	POINT     m_MultiPoint[10]     ;    //中间临时存储的坐标数据 ；
	int       m_Pre_CollectCount    ;    //预采集的数目，前面几次的采集不采用，等到后面稳定了数据在采用。

	BOOL   m_bCollectWheelState;       //在进行鼠标滑轮的判断中。正在采集的状态。
	int    m_nCollectTotileCount;      //在进行鼠标滑轮的判断中。采集的总的数目。

	BOOL  m_bMouseWheelState;          //检测出来是鼠标的状态；
	int   m_nOneMissCount   ;          //一个点的误操作个数
	int   m_nOnePointCount  ;          //纪录一个点的采集的个数，如果超过一定的数量时，说明是一个手指在操作，如果出现2个或者2个以上的点的话，说明是干扰光斑。 

    LONG  m_lFirstDistance;            //最开始两个点之间的距离。
	LONG  m_lSecondDistance;           //第二次计算两点之间的距离；
	int   m_nDistanceCount_Big;        //如果是点之间的距离变大的数的累加值
	int   m_nDistanceCount_Small;      //如果是点之间的距离变小的数的累加值。
	int   m_nMobileDistance;           //在进行滑轮的计算中，这是滑轮移动的距离。

	HWND        m_hwnd;                           //白板的窗体句柄
    bool        m_isGLBoardProcExsit;                //白板进程是否存在
	CString     StrStatus;                        //纪录白板按钮的当前状态
	CString     PreStrStatus;                     //纪录白板按钮前一个状态
	POINT       ptErase;
	int  m_nCollectMissCount;                           //纪录采集鼠标的不正确数据的次数。

	CLongTimeClickTrigger    m_oLongTimeClickTrigger;
	CHandDoubleClickChecker  m_oHandDoubleClickChecker;
	CFingerDoubleClickCHecker m_oFingerDoubleClickChecker ;
	BOOL  IsMouseLeftEdge  ;     //出现大光斑的时候，鼠标的位置是否在屏幕的左边缘。
	int m_nScreenX;
	int m_nScreenY;

	int   m_Vsumx[2] ;          //纪录x轴的移动距离。
	int   m_Vsumy[2] ;          //纪录Y轴移动的距离。

    //
    bool m_isGREnabled;      //是否允许手势识别
    bool m_isMultiPenPermited; //是否允许多笔
    bool m_isTouchPadMode;      //当前是否是触屏模式，主要一些手势在触屏下直接使用微软的，比如图片操作

    bool m_isPenTouchDevice; //笔触模式，不支持手势识别

    bool m_isMultiPenMode;

    bool m_isMouseUp;
    POINT   m_ptResponse;      //识别后，需要鼠标响应点
    bool m_isVMouseSelfCotrol;

    //<added reason:当找到GLBoard，但GLBoard手势禁用且在单笔模式时，就会把操作设备交付给SpotListProcessor，
    //而在SpotListProcessor控制设备之前又会调用Windows手势，这样就会在GLBoard中误触发windows手势
    bool m_isInuptInGLBoard;      //是否找到了GLBoard

    bool m_isLogPermit;

    //智能光笔匹配
    static const int MAXMATCH_NUM = MAX_SUPPORT_TOUCH_COUNT;
    static const int MAX_CAMERA_NUMBER = 6;
    CSmartMatch<MAXMATCH_NUM> m_oSmartMatch;//
    CStrokeFilter<MAXMATCH_NUM>  m_oStrokFilter;//笔迹过滤器
    CMultiPenGestureRecognition m_GRMultiPen; //多笔模式下手势识别

    bool m_isSmartMatchReset;

    //bool m_isContinueWhenNoInput ;  //当判断是否在GLBoard输入，且输入点为0，若此标志为false，则返回未在GLBoard中输入，否则，返回，用来清空以前的输入；

    int m_conNonOneTouch_when_penstate;

	enum E_ERASERTOPEN_STATE               //纪录的是笔和板擦转换时采集的状态的变化。也是采集几次才能确定是否是需要的点。
	{
		E_START,           // 纪录开始
		E_IS_FINSHED,      //纪录是否完成
	}m_eRecordState;

	enum E_COLLECT_STATE    //采集
	{
		E_COLLECT_START ,
		E_COLLECT_CONTINUE ,
		E_COLLECT_END ,
	}m_eCollectState;

	enum  E_MOUSEWHEEL_STATE       //采集时的状态变化。采集几次才能确定是否是需要的点。
	{
		E_GESTUREEVENT_START_COLLECT,
		E_GESTUREEVENT_CONTINUE_COLLECT,
		E_GESTUREEVENT_DATA_CALCULATE,
	}m_eMouseWheelState;

  //  //added by toxuke@gmail.com,  2014/01/08
  //  //白板软件所处状态的枚举值
  //  typedef enum 
  //  {
		//E_WBS_IDLE             ,    //无状态
		//E_WBS_ERASER           ,    //板擦状态
		//E_WBS_PEN              ,    //笔状态
		//E_WBS_ROAM             ,    //漫游状态
		//E_WBS_SPOTLIGHT        ,    //探照灯状态
		//E_WBS_RETURNTODISKTOP  ,    //返回桌面状态
		//E_WBS_NEXTPAGE         ,    //下一页状态
		//E_WBS_PROPAGE          ,    //上一页状态
		//E_WBS_ZOOM             ,    //缩放
		//E_WBS_RORATE           ,    //旋转
		//E_WBS_MASK             ,    //幕布
		//E_WBS_SELECTION        ,    //选择笔画对象

  //  }E_WBS;    

    E_WBS   m_eWBS;
    E_WBS   m_ePreWBS;
    //    

private:
       //@功能:在白板软件中，笔和板擦之间的转换函数
       //@参数:aryLightSpots,按照大小从大到小排列的光斑数组
       //      nCount, 光斑个数
       //      pptMouse, 指向保存"推荐鼠标"位置的内存空间的指针
       BOOL     EraserTransPen(const TLightSpot * aryLightSpots,  int nCount/*, POINT* pptMouse = NULL*/);  
       //得到白板正在操作的是那个按钮。
       void     GetGLboardButtonState();
       //@功能：多笔书写的函数
       BOOL     MultiplePenExecute(const  TLightSpot *aryLightSpots, int &nCount);	
       //@功能：在系统条件下，滑轮滚动时的手势响应函数
       BOOL     DetectMouseWheelAction(TLightSpot * aryLightSpots, int nCount);
       //@功能：读取共享内存中的数据的函数
       CString  ReadShareMemory();
       //@功能：得到手指滑动时的距离的函数
       int      GetMouseWheelLength();


private:
        /*
        @功能：计算两个光点时，光点的移动方向
        @参数：matchResult, Match2Touchs返回结果
        @参数：pptMouse, 指向保存"推荐鼠标"位置的内存空间的指针
        @返回值：见E_2TOUCHS_MOVES定义
        */
        E_2TOUCHS_MOVES Calc2TouchsMoveDirection(bool matchResult = false, POINT* pptMouse = NULL);
        
        /*
        @功能：两个光点移动匹配
        @参数：见EraserTransPen
        @返回值：true,完成匹配；false，匹配失败
        */
        bool Match2Touchs(const TLightSpot * aryLightSpots,  int nCount);

        bool LeastDistanceMatch(const TLightSpot * aryLightSpots, int nCount, POINT &pt);

        void Restore2OldPenState(); 

        void CancelMouseButtonUp(const TLightSpot * aryLightSpots,  int nCount);
        void SimulHIDReset(const POINT &pt);
        void SimulHIDDown(const POINT &pt);

        bool IsReponseByMyself(E_WBS eState)
        {            
             return !(eState == E_WBS_IDLE || eState == E_WBS_PEN || eState == E_WBS_ERASER ||eState == E_WBS_SPOTLIGHT || eState == E_WBS_MASK);
        }

        void Input2Device(const POINT *ptArry, const int &nActualCount, bool needMatch = true);        
};