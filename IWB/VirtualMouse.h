#pragma once

//说明:视频中检测到的光笔坐标到屏幕坐标的转化类
enum EVirtualMouseEvent
{
    E_LBUTTON_DOWN,
    E_LBUTTON_UP,
    E_RBUTTON_DOWN,
    E_RBUTTON_UP,
    E_MOUSE_MOVE,
    E_LBUTTON_DOWN_MOVE,
    E_RBUTTON_DOWN_MOVE,
    E_VERT_WHEEL,//垂直滚动
    E_HORZ_WHEEL //水平滚动
};

class IVirtualMouse
{
public:
    //@功能:产生指定的鼠标事件。
    //@参数:  pos, 鼠标位置
    //     eEvent, 鼠标事件枚举
    //     nMouseData,  E_VERT_WHEEL, 为鼠标滚轮移动量, 是WHEEL_DELTA的整数倍。正数表示向前滚动，负数表示向后滚动。
    //                  E_HORZ_WHEEL, 为鼠标滚轮移动量, 是WHEEL_DELTA的整数倍。正数表示右滚动，负数表示向左滚动。
    //    
    virtual void GenEvent(const POINT& ptMouse, EVirtualMouseEvent eEvent, int nMouseData=0) = 0;
};
//双击事件检测器
class CDoubleClickChecker
{
public:
    CDoubleClickChecker(IVirtualMouse* pVirtualMouse)
        :
    m_pVirtualMouse(pVirtualMouse),
    m_eCheckerState(E_DOUBLE_CLICK_CHECKER_STATE_IDLE)
    {
        m_ptFirstLeftDownPos.x = 0;
        m_ptFirstLeftDownPos.y = 0;
        m_pFristLeftDownTime   = 0;

        int nInitPhysicalDimension = int(DEFAULT_SCREEN_DIAGONAL_LENGTH);//80 inches, 单位:mm
        int nCx = GetSystemMetrics(SM_CXSCREEN);
        int nCy = GetSystemMetrics(SM_CYSCREEN);

        SIZE screenSize;
        screenSize.cx = nCx;
        screenSize.cy = nCy;
        OnSetTouchScreenDimension(nInitPhysicalDimension, screenSize);

        m_nDblClickTime = 500;//毫秒

        //获取系统配置
        m_nSysDblClickTime = GetDoubleClickTime();
        
        m_nSysCxDblClick = ::GetSystemMetrics(SM_CXDOUBLECLK);
        m_nSysCyDblClick = ::GetSystemMetrics(SM_CYDOUBLECLK);

    }

    ~CDoubleClickChecker()
    {
        //SetEvent(m_hExitEvent);
        //WaitForSingleObject(m_hProcessThread, INFINITE);
    }

    enum EMouseState
    {
        E_MOUSE_STATE_LEFT_DOWN       = 0,//鼠标左键按下
        E_MOUSE_STATE_LEFT_UP         = 1,//鼠标左键弹起
        E_MOUSE_STATE_LEFT_DOWN_MOVE  = 2,//鼠标左键按下移动
    };
    //@功能:激励检测状态机的运行，触发双击事件
    //@参数:ptLeftDownPos，鼠标左键按下时的位置
    //      eMouseState， 鼠标状态
    //      
    //      
    void Check(const POINT& ptLeftDownPos, EMouseState eMouseState)
    {
        DWORD dwNow = GetTickCount();//现在时刻
        LONG     dx = 0,dy = 0      ;//
        DWORD    dT = 0             ;//

        //<<debug
        //static int s_nDebug = 0;
        //AtlTrace(_T("Check[%03d]input x=%d,y=%d, mouse state=%d\n"), s_nDebug, ptLeftDownPos.x, ptLeftDownPos.y,eMouseState);
        //>>


        switch(m_eCheckerState)
        {
        case E_DOUBLE_CLICK_CHECKER_STATE_IDLE:
            if(E_MOUSE_STATE_LEFT_DOWN == eMouseState)
            {
                m_ptFirstLeftDownPos = ptLeftDownPos;
                m_pFristLeftDownTime = dwNow;
                m_eCheckerState     = E_DOUBLE_CLICK_CHECKER_STATE_FIRST_DOWN;

                //AtlTrace(_T("Check[%03d]idle --> first down\n"), s_nDebug);
            }
            
            break;

        case E_DOUBLE_CLICK_CHECKER_STATE_FIRST_DOWN://第一次按下状态
            dx =  abs(ptLeftDownPos.x - m_ptFirstLeftDownPos.x);
            dy  = abs(ptLeftDownPos.y - m_ptFirstLeftDownPos.y);
            
            //与第一次按下时的时间差
            dT = dwNow - m_pFristLeftDownTime;

            if((int)dT > m_nDblClickTime)
            {//超时
                if(E_MOUSE_STATE_LEFT_DOWN == eMouseState)
                {
                    m_ptFirstLeftDownPos = ptLeftDownPos;
                    m_pFristLeftDownTime = dwNow;

                }
                
            }
            else
            {//未超时
                if(dx > m_nDblClickLogicalWidth
                    ||
                   dy > m_nDblClickLogicalHeight)
                {//移动范围超出空间容错范围, 检测器回到初始状态
                    m_eCheckerState = E_DOUBLE_CLICK_CHECKER_STATE_IDLE;
                   
                    //AtlTrace(_T("Check[%03d]first down --> idle for out of range\n"), s_nDebug);
                }
                else
                {
                    if(E_MOUSE_STATE_LEFT_UP == eMouseState)
                    {
                        m_eCheckerState = E_DOUBLE_CLICK_CHECKER_STATE_FIRST_UP;

                        //AtlTrace(_T("Check[%03d]first down --> first up\n"), s_nDebug);
                    }
                    else if(E_MOUSE_STATE_LEFT_DOWN_MOVE != eMouseState && E_MOUSE_STATE_LEFT_DOWN != eMouseState)
                    {
                        m_eCheckerState = E_DOUBLE_CLICK_CHECKER_STATE_IDLE;

                        //AtlTrace(_T("Check[%03d]first down --> idlen for unexpected events\n"), s_nDebug);
                    }
                }
            }

            break;


        case E_DOUBLE_CLICK_CHECKER_STATE_FIRST_UP://第一次弹起状态

            dx =  abs(ptLeftDownPos.x - m_ptFirstLeftDownPos.x);
            dy  = abs(ptLeftDownPos.y - m_ptFirstLeftDownPos.y);

            dT = dwNow - m_pFristLeftDownTime;

            if((int)dT > m_nDblClickTime)
            {//超时
                if(E_MOUSE_STATE_LEFT_DOWN == eMouseState)
                {
                    m_ptFirstLeftDownPos = ptLeftDownPos;
                    m_pFristLeftDownTime = dwNow;
                    m_eCheckerState = E_DOUBLE_CLICK_CHECKER_STATE_FIRST_DOWN;//转入第一次按下状态

                     //AtlTrace(_T("Check[%03d]first up -->firt down for timeout\n"), s_nDebug);
                }
                
            }
            else
            {//未超时
                if(dx > m_nDblClickLogicalWidth
                    ||
                   dy > m_nDblClickLogicalHeight)
                {//移动范围超出空间容错范围, 检测器回到初始状态
                    m_eCheckerState = E_DOUBLE_CLICK_CHECKER_STATE_IDLE;

                    //AtlTrace(_T("Check[%03d]first up --> idle for out of range\n"), s_nDebug);
                   
                }
                else
                {
                    if(E_MOUSE_STATE_LEFT_DOWN ==  eMouseState)
                    {
                        //用连续两次单击来模拟双击事件
                        m_pVirtualMouse->GenEvent(ptLeftDownPos, E_LBUTTON_UP);
                        m_pVirtualMouse->GenEvent(ptLeftDownPos, E_LBUTTON_DOWN);

                        // AtlTrace(_T("Check[%03d]Trigger Double Click\n"), s_nDebug);
                      }
    
                    m_eCheckerState = E_DOUBLE_CLICK_CHECKER_STATE_IDLE;

                   //AtlTrace(_T("Check[%03d]first up --> idle\n"), s_nDebug); 


                }//else

            }//else

            break;

        }//switch 


        //<<debug
        //s_nDebug ++;
        //debug>>

    };



    //@功能:设置触屏的尺寸事件响应函数
    //@参数:nPhysicalDiagonalLength, 屏幕尺寸,对角线长度, 单位:毫米
    //      szLogicalDimension, 屏幕逻辑尺寸，单位:像素
    void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension)
    {
        int  nCxScreen = szLogicalDimension.cx;
        int  nCyScreen = szLogicalDimension.cy;

        int  nLogicalDiagonalLength = (int)sqrt(double(nCxScreen *  nCxScreen + nCyScreen * nCyScreen));

        m_nDblClickLogicalWidth  = nLogicalDiagonalLength * m_nDlbClickPhysicalWidth / nPhysicalDiagonalLength;
        m_nDblClickLogicalHeight = nLogicalDiagonalLength * m_nDlbClickPhysicalHeight / nPhysicalDiagonalLength;

    }

protected:

    //双击检测器状态
    enum E_DoubleClickCheckerState
    {
        E_DOUBLE_CLICK_CHECKER_STATE_IDLE       ,//双击检测器空闲状态
        E_DOUBLE_CLICK_CHECKER_STATE_FIRST_DOWN ,//第一次按下状态
        E_DOUBLE_CLICK_CHECKER_STATE_FIRST_UP   ,//第二次弹起状态
    }m_eCheckerState;

    POINT m_ptFirstLeftDownPos ;//两次单击中的第一次左键按下位置(屏幕坐标)。
    DWORD m_pFristLeftDownTime ;//两次单击中的第一次左键按下时刻。


    //双击事件产生门限
    //时间门限    
    int  m_nDblClickTime;

    //空间门限    
    int  m_nDblClickLogicalWidth ;//触发双击事件，第二次单击允许所在的矩形区域范围的水平宽度,单位:像素
    int  m_nDblClickLogicalHeight;//触发双击事件，第二次单击允许所在的矩形区域范围的垂直高度,单位:像素

    static const int m_nDlbClickPhysicalWidth  = 10 ;//触发双击事件，第二次单击允许所在的矩形区域范围的水平宽度,单位:毫米
    static const int m_nDlbClickPhysicalHeight = 10 ;//触发双击事件，第二次单击允许所在的矩形区域范围的垂直高度,单位:毫米

    //系统设置双击的空间门限
    int  m_nSysCxDblClick;//单位:像素
    int  m_nSysCyDblClick;//单位:像素

    //系统设置的双击的时间门限,单位:毫秒
    int  m_nSysDblClickTime;


    IVirtualMouse* m_pVirtualMouse;

};


//右键事件触发器
class CRightClickTrigger
{
public:
    CRightClickTrigger(IVirtualMouse* pVirtualMouse)
        :
    m_pVirtualMouse(pVirtualMouse)
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
                m_pVirtualMouse->GenEvent(ptNow, E_RBUTTON_DOWN);
                m_pVirtualMouse->GenEvent(ptNow, E_RBUTTON_UP);
                return TRUE;
            }


        }

        return FALSE;
    }

protected:
    POINT m_ptRangeCenter;
    static const DWORD MOUSE_RIGHT_CLICK_RANGE = 3;//在检测右键触发事件时要求鼠标停留的区域的半径。
    //当鼠标在该区域范围内超过一段时间即认为可触发右键
    //单击事件
    DWORD m_dwRangeUpdateTime;
    static const DWORD MOUSE_RIGHTDOWN_EVENT_TRIGGER_TIMER = 1500;//鼠标右键按下事件触发的时间
    IVirtualMouse* m_pVirtualMouse;
};



class CVirtualMouse:public IVirtualMouse
{
public:

    CVirtualMouse();

    //功能:给虚拟鼠标输入光笔数据
    //参数:bPenOn, 光笔亮灭标志
    //     ptMouse, 光笔位置,屏幕坐标

    void Input(BOOL bPenOn, const POINT* ptMouse, BOOL bUseWinAPI = FALSE);
	void Input_AirOperate(BOOL bPenOn, const POINT* ptMouse, EAIROPERATE_CLICKMODE eClickMode, BOOL bUseWinAPI = FALSE);

    //功能:状态复位
    void Reset();

    //@功能:产生指定的鼠标事件。
    //@参数:  pos, 鼠标位置
    //     ptMouse, 鼠标事件枚举
    //     nMouseData,  E_VERT_WHEEL, 为鼠标滚轮移动量, 是WHEEL_DELTA的整数倍。正数表示向前滚动，负数表示向后滚动。
    //                  E_HORZ_WHEEL, 为鼠标滚轮移动量, 是WHEEL_DELTA的整数倍。正数表示右滚动，负数表示向左滚动。
	//     bUseWinAPI,是否使用Windows API来模拟鼠标操作
    virtual void GenEvent(const POINT& ptMouse, EVirtualMouseEvent eEvent, int nMouseData = 0);


    //@功能:设置驱动设备句柄
    //@参数:hDriverDevice， 驱动设备句柄
    //@说明:当设备句柄为INVALID_HANDLE_VALUE时，使用Window API模拟鼠标消息。
    void SetDeviceHandle(HANDLE hDriverDevice);



    //@功能;返回双击检测器
    CDoubleClickChecker& GetDoubleClickChecker()
    {
        return this->m_oDoubleClickChecker;
    }
protected:


    //void RunStateMachine()(BOOL bFindPen, const POINT& ptPenPos);
    enum E_VIRTUAL_MOUSE_STATE
    {
        VIRTUAL_MOUSE_STATE_IDLE             ,//模拟鼠标空闲状态
        VIRTUAL_MOUSE_STATE_LEFT_DOWN        ,//模拟鼠标左键按下状态
        //VIRTUAL_MOUSE_STATE_RIGHT_CONTROL  ,//模拟鼠标右键控制状态
        VIRTUAL_MOUSE_STATE_MOVE             ,//模拟鼠标的移动状态  
    }m_eVirutalMouseState;

    CDoubleClickChecker m_oDoubleClickChecker ;
    CRightClickTrigger  m_oRightClickTrigger  ;
    POINT               m_ptLastPenAbsolutePos;
    HANDLE              m_hDriverDevice       ;
	BOOL                m_bForceUseWinAPI     ;//强制使用Windows API模拟鼠标操作


};



__inline BOOL operator==(const POINT& right, const POINT& left)
{
    return (right.x == left.x && right.y == left.y);
}

__inline BOOL operator!=(const POINT& right, const POINT& left)
{
    return (right.x != left.x || right.y != left.y);
}






