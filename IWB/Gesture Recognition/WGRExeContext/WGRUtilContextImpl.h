#pragma once

class CWGRUtilContextImpl
    : public IWGRUtilContext
{
public:
    CWGRUtilContextImpl(void);
    ~CWGRUtilContextImpl(void);

public:

    virtual void TriggerHIDDown(const POINT *pt);
    virtual void TriggerHIDUp(const POINT *pt);

    virtual void TriggerHMouseWheelEvent(POINT ptCur, int dir, int rollDistance = 120);
    virtual void TriggerVMouseWheelEvent(POINT ptCur, int dir, int rollDistance = 120);   

    virtual void TriggerAltLArrowEvent(const POINT &ptCur);
    virtual void TriggerAltRArrowEvent(const POINT &ptCur);
    virtual void TriggerMiniCurWindow(const POINT &ptCur);
    virtual void TriggerMaxiCurWindow(const POINT &ptCur);
    virtual void TriggerDisplayDesktop(const POINT &ptCur);

    virtual void TriggerShowWndSBS(const POINT &ptCur);
    virtual void TriggerRefreshWnd(const POINT &ptCur);

    virtual void TriggerSwitchWindowEvent(const POINT &ptCur, E_SWHWND_MODE eSWMode = E_SWHWND_NONEMODE);
    virtual void TriggerSwitchWindowHintEvent(POINT ptCur, bool bHint);

    virtual void TriggerDocSaveEvent(const POINT &ptCur);

    virtual void TriggerCloseCurrentWindowEvent(const POINT &ptCur);

    virtual void TriggerMoveWindowHintEvent(POINT ptCur, bool bHint);
    virtual void TriggerMoveWindowEvent(POINT ptCur, POINT ptLast, E_MOVWND_MODE eMode);

public:
    virtual bool Get_TriggerEvent_Permission(E_TRIGGEREVENT eEvent, const POINT &pt);

    /*
    @功能：是否关闭了所有手势
    */
    virtual bool IsAllGestureClosed();

    /*
    @功能：是否关闭了所有基于手掌的手势
    */
    virtual bool IsAllPalmBasedGestureClosed();

    /*
    @功能：是否关闭了所有基于手指的手势
    */
    virtual bool IsAllFingerBasedGestureClosed();

    virtual bool IsLogPermited();

    virtual int GetMultiOfPalm2Std();

    /*
    //@功能:屏幕分辨率变化事件响应函数
    //@参数:nScreenWidth, 新的屏幕宽度
    //      nScreenHeight,新的屏幕高度
    */
    virtual void OnDisplayChange(int nScreenWidth, int nScreenHeight);

    /*
    //@功能:物理尺寸变化时间响应函数
    //@参数:物理尺寸
    */
    virtual void OnPhysicalLengthChange(float fPhysicalLength);

    /*
    @功能：映射x方向的标准距离到本环境的实际距离
    @附加说明：该标准为——80英寸白板、1024 * 768分辨率
    */
    virtual int MapStandX2Actual(int xStandard);

    /*
    @功能：映射y方向的标准距离到本环境的实际距离
    @附加说明：该标准为——80英寸白板、1024 * 768分辨率
    */
    virtual int MapStandY2Actual(int yStandard);

    virtual int GetWeightedSpotMass(const int &nActualMass, const int &nExtRectArea, const int &nStdAreaMulti);

private:
    /*
    @功能：模拟键盘按键
    @参数：wVkArry，虚拟键码数组；
           vkCount, 虚拟键码数目
    */
    void SimulateKeyDown(WORD wVk);
    void SimulateKeyUp(WORD wVk);

    //@功能：获取顶层父窗口
    //@参数：ptCur，位置            
    HWND GetTopParent(const POINT &ptCur); 
    //@功能：获取第一个具有标题栏的父窗体
    HWND GetFirstParentWithCaption(const POINT &ptCur);

    void Input2Device(const POINT *ptArry, const int &nActualCount); 

private:
    CWinShellUtility        m_shellUtility;  

    static const int MAXMATCH_NUM = 10;
    static const int MAX_CAMERA_NUMBER = 6;
    //CSmartMatch<MAXMATCH_NUM, 50> m_oSmartMatch;//
    CSmartMatch<MAXMATCH_NUM> m_oSmartMatch;//

    static POINT m_ptsReset[MAXMATCH_NUM];

    bool m_switchWndFail_by_shell;
};
