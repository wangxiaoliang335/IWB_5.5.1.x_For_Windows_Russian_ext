#include "stdafx.h"
#include "..\..\headers.h"

CVirtualMouse &g_oVirtualMouse = GetVirtualMouse();
CVirtualHID &g_oRefVirtualHID = CSpotListProcessor::GetVirtualHID();

//POINT g_oResetPoint[m]

CWGRUtilContextImpl::CWGRUtilContextImpl(void)
{    
    m_switchWndFail_by_shell = false;
}

CWGRUtilContextImpl::~CWGRUtilContextImpl(void)
{
}

void CWGRUtilContextImpl::Input2Device(const POINT *ptArry, const int &nActualCount)
{
    m_oSmartMatch.DoMatch(ptArry, nActualCount);

    int nElementCount = 0;
    const TMatchInfo* pMatchInfo =  m_oSmartMatch.GetAllMatchInfo(&nElementCount);

    TContactInfo penInfo[MAXMATCH_NUM];

    if(nElementCount > _countof(penInfo)) nElementCount = _countof(penInfo);

    for(int i = 0; i < nElementCount; i++)
    {
        penInfo[i].ePenState = (pMatchInfo[i].eMatchState == E_MISMATCHED)?E_PEN_STATE_UP:E_PEN_STATE_DOWN;
        penInfo[i].uId       = pMatchInfo[i].uId;
        penInfo[i].pt        = pMatchInfo[i].ptPos;
    }


    g_oRefVirtualHID.InputPoints(penInfo, nElementCount);
}


void CWGRUtilContextImpl::TriggerHIDDown(const POINT *pt)
{
    Input2Device(pt, 1);
}

void CWGRUtilContextImpl::TriggerHIDUp(const POINT *pt)
{
    //do nothing
    g_oRefVirtualHID.Reset();    
}

void CWGRUtilContextImpl::TriggerHMouseWheelEvent(POINT ptCur, int dir, int rollDistance /* = 120 */)
{    
    if (dir == -1)
    {//向左        
        //m_shellUtility.HScroolForegroundWindow(ESCRLT_LEFT);
        g_oVirtualMouse.GenEvent(ptCur, E_HORZ_WHEEL, -120);
    }

    if (dir == 1)
    {//向右        
        //m_shellUtility.HScroolForegroundWindow(ESCRLT_RIGHT);
        g_oVirtualMouse.GenEvent(ptCur, E_HORZ_WHEEL, 120);
    }
}

void CWGRUtilContextImpl::TriggerVMouseWheelEvent(POINT ptCur, int dir, int rollDistance)
{    
    //if (dir == -1)
    //{//向上
    //    m_shellUtility.VScroolForegroundWindow(ESCRLT_UP);
    //}

    //if (dir == 1)
    //{//向下
    //    m_shellUtility.VScroolForegroundWindow(ESCRLT_DOWN);
    //}

    //由于有些窗口与ScrollBar是分离的，所以采用往窗口发送WM_VSCROLL是失败的，故选择模拟鼠标滑轮
    if (dir == -1)
    {//向上
        //g_oVirtualMouse.GenMouseWheelEvent(&ptCur, 120);
        g_oVirtualMouse.GenEvent(ptCur, E_VERT_WHEEL, 120);
    }

    if (dir == 1)
    {//向下
        //g_oVirtualMouse.GenMouseWheelEvent(&ptCur, -120);
        g_oVirtualMouse.GenEvent(ptCur, E_VERT_WHEEL, -120);
    }
}

void CWGRUtilContextImpl::TriggerAltLArrowEvent(const POINT &ptCur)
{        
    /*if (Get_TriggerEvent_Permission(E_TEF_35TOUCHS_ALTLARROW) == false) return;

    SimulateKeyDown(VK_MENU);
    SimulateKeyDown(VK_LEFT);

    SimulateKeyUp(VK_LEFT);
    SimulateKeyUp(VK_MENU);*/
}

void CWGRUtilContextImpl::TriggerAltRArrowEvent(const POINT &ptCur)
{  
    SimulateKeyDown(VK_MENU);
    SimulateKeyDown(VK_RIGHT);

    SimulateKeyUp(VK_RIGHT);
    SimulateKeyUp(VK_MENU);
}

void CWGRUtilContextImpl::TriggerMiniCurWindow(const POINT &ptCur)
{
    //g_oMouseEventGen.CancelMouseLeftDown(&ptCur);  
    m_shellUtility.MiniCurrentWindow(ptCur);
}

void CWGRUtilContextImpl::TriggerMaxiCurWindow(const POINT &ptCur)
{
    //g_oMouseEventGen.CancelMouseLeftDown(&ptCur);
    m_shellUtility.MaxiCurrentWindow(ptCur);
}

void CWGRUtilContextImpl::TriggerDisplayDesktop(const POINT &ptCur)
{
    //g_oMouseEventGen.CancelMouseLeftDown(&ptCur);

    /*WORD vk[2];
    vk[0] = VK_LWIN;
    vk[1] = 0x44;

    SimulateKeyDown(vk, 2);
    SimulateKeyUp(vk, 2);*/

    if (!m_shellUtility.DisplayDesktop(ptCur))
    {//如果使用Shell执行回到桌面失败，则使用快捷键win+D
        SimulateKeyDown(VK_LWIN);
        SimulateKeyDown(0x44);

        SimulateKeyUp(0x44);
        SimulateKeyUp(VK_LWIN);        
    }    
}

void CWGRUtilContextImpl::TriggerShowWndSBS(const POINT &ptCur)
{
    //g_oMouseEventGen.CancelMouseLeftDown(&ptCur);

    m_shellUtility.ShowWindowsSidebySide();
}

void CWGRUtilContextImpl::TriggerRefreshWnd(const POINT &ptCur)
{
    //g_oMouseEventGen.CancelMouseLeftDown(&ptCur);    
    SimulateKeyDown(VK_F5);

    SimulateKeyUp(VK_F5);
}

void CWGRUtilContextImpl::TriggerSwitchWindowEvent(const POINT &ptCur, E_SWHWND_MODE eSWMode)
{    
    switch(eSWMode)
    {
    case E_SWHWND_STARTMODE:
        //{//开启窗口切换
        //    /*
        //     按下ALT+TAB键，然后弹起TAB键
        //    */
        //    SimulateKeyDown(VK_MENU);
        //    SimulateKeyDown(VK_TAB);

        //    SimulateKeyUp(VK_TAB);
        //    SimulateKeyUp(VK_MENU);
        //}
        {
            HWND hCur = GetFirstParentWithCaption(ptCur);
            HWND hFocus = GetFocus();
            if(hCur == hFocus)
            {
                SetFocus(NULL);
            }

            //<<2016/04/13, by 15077726@qq.com
            //在Win10下，系统COM接口函数IShellDispatch5::WindowSwitcher返回S_OK但看不到任何效果。
            //>>
            if(IsWin10OrGreater())
            {
                /*
                    按下ALT+TAB键，然后弹起TAB键
                */
                SimulateKeyDown(VK_MENU);
                
                SimulateKeyDown(VK_TAB);
                SimulateKeyUp(VK_TAB);

                m_switchWndFail_by_shell = true;
            }
            else
            {
                m_switchWndFail_by_shell = !(m_shellUtility.SwitchWindows("Window Switcher"));
                if (m_switchWndFail_by_shell)
                {//采用ALT+TAB开启窗口切换
                    /*
                    按下ALT+TAB键，然后弹起TAB键
                    */
                    SimulateKeyDown(VK_MENU);
                    SimulateKeyDown(VK_TAB);

                    SimulateKeyUp(VK_TAB);
                    //SimulateKeyUp(VK_MENU);
                }
            }

            AtlTrace(_T("\r\n SwitchWindow mode = E_SWHWND_STARTMODE"));

        }

        //HWND hCur = GetFirstParentWithCaption(ptCur);
        //HWND hFocus = GetFocus();
       
        //m_switchWndFail_by_shell = !(m_shellUtility.SwitchWindows("Window Switcher"));
        //if (m_switchWndFail_by_shell)
        //{//采用ALT+TAB开启窗口切换
        //    /*
        //     按下ALT+TAB键，然后弹起TAB键
        //    */
        //    SimulateKeyDown(VK_MENU);
        //    SimulateKeyDown(VK_TAB);

        //    SimulateKeyUp(VK_TAB);
        //    //SimulateKeyUp(VK_MENU);
        //}

        //AtlTrace(_T("\r\n SwitchWindow mode = E_SWHWND_STARTMODE"));

        break;

    case E_SWHWND_EXECMODE:
        //{//执行窗口切换
        //    /*
        //     按下TAB键，然后弹起TAB键
        //    */
        //    SimulateKeyDown(VK_TAB);

        //    SimulateKeyUp(VK_TAB);
        //}
        //m_shellUtility.SwitchWindows();
        {
            //if (m_switchWndFail_by_shell)
            //{
            //        /*
            //         按下TAB键，然后弹起TAB键
            //        */
            //        SimulateKeyDown(VK_TAB);

            //        SimulateKeyUp(VK_TAB);
            //}
            //else
            {
                SimulateKeyDown(VK_RIGHT);
                SimulateKeyUp(VK_RIGHT);
            }
        }

        //AtlTrace(_T("\r\n SwitchWindow mode = E_SWHWND_EXECMODE"));
        break;

    case E_SWHWND_EXELMODE:
        {
            //if (m_switchWndFail_by_shell)
            //{
            //    /*
            //    按下TAB键，然后弹起TAB键
            //    */
            //    SimulateKeyDown(VK_TAB);

            //    SimulateKeyUp(VK_TAB);
            //}
            //else
            {
                SimulateKeyDown(VK_LEFT);
                SimulateKeyUp(VK_LEFT);
            }
        }

        break;

    case E_SWHWND_EXERMODE:
        {
            //if (m_switchWndFail_by_shell)
            //{
            //    /*
            //    按下TAB键，然后弹起TAB键
            //    */
            //    SimulateKeyDown(VK_TAB);

            //    SimulateKeyUp(VK_TAB);
            //}
            //else
            {
                SimulateKeyDown(VK_RIGHT);
                SimulateKeyUp(VK_RIGHT);
            }            
        }

        break;

    case E_SWHWND_CANCLEMODE:
        {
            if(m_switchWndFail_by_shell)
            {
                SimulateKeyUp(VK_MENU);
            }
            else
            {
                SimulateKeyDown(VK_LEFT);
                SimulateKeyUp(VK_LEFT);

                SimulateKeyDown(VK_RETURN);
                SimulateKeyUp(VK_RETURN);
            }

            m_switchWndFail_by_shell = false;
        }

        //AtlTrace(_T("\r\n SwitchWindow mode = E_SWHWND_CANCLEMODE"));
        break;

    case E_SWHWND_OKMODE:
    case E_SWHWND_ENDMODE:
        {//结束窗口切换
            /*
             弹起ALT键
            */
            if(m_switchWndFail_by_shell)
            {
                SimulateKeyUp(VK_MENU);
            }
            else
            {
                SimulateKeyDown(VK_RETURN);
                SimulateKeyUp(VK_RETURN);
            }    

            m_switchWndFail_by_shell = false;
        }

        //AtlTrace(_T("\r\n SwitchWindow mode = E_SWHWND_ENDMODE"));
        break;

    default:
        break;
    }    
}

void CWGRUtilContextImpl::TriggerSwitchWindowHintEvent(POINT ptCur, bool bHint)
{
    if (bHint)
    {
        ptCur.y = ptCur.y - 50;
        SetCursorPos(ptCur.x, ptCur.y);

        SetCursor(LoadCursorFromFile(_T("res\\cursor_switchwindow.cur")));
    }
    else
    {
        SetCursor(LoadCursor(NULL, IDC_ARROW));
    }
}

void CWGRUtilContextImpl::TriggerDocSaveEvent(const POINT &ptCur)
{

    //模拟CTR-S
    SimulateKeyDown(VK_CONTROL);
    SimulateKeyDown(83);

    SimulateKeyUp(83);
    SimulateKeyUp(VK_CONTROL);
}

void CWGRUtilContextImpl::TriggerCloseCurrentWindowEvent(const POINT &ptCur)
{    
    //模拟ALT-F4
    SimulateKeyDown(VK_MENU);
    SimulateKeyDown(VK_F4);

    SimulateKeyUp(VK_F4);
    SimulateKeyUp(VK_MENU);
}

void CWGRUtilContextImpl::TriggerMoveWindowHintEvent(POINT ptCur, bool bHint)
{
    //////////////////////////////////////////////////////////////////////////
    if (bHint)
    {
        //只有具有标题栏的窗口才支持移动
        HWND hTmpWnd = GetFirstParentWithCaption(ptCur);

        if (hTmpWnd)
        {
            SetFocus(hTmpWnd);

            PostMessage(hTmpWnd, WM_SYSCOMMAND, SC_MOVE, NULL);

            Sleep(8);

            /*int yPos = ptCur.y > 500 ? (ptCur.y - 250) : (ptCur.y - 100);

            if(yPos <= 0) yPos = 50;

            SetCursorPos(ptCur.x, yPos);
            ShowCursor(TRUE);*/
            ptCur.y = ptCur.y > 500 ? (ptCur.y - 250) : (ptCur.y - 100);

            if(ptCur.y <= 0) ptCur.y = 50;

            /*SetCursorPos(ptCur.x, ptCur.y);
            ShowCursor(TRUE);*/

            g_oVirtualMouse.GenEvent(ptCur, E_MOUSE_MOVE);
            /*SimulateKeyDown(VK_DOWN);
            SimulateKeyUp(VK_UP);

            SetCursorPos(ptCur.x - 100, ptCur.y - 100);
            SetCursorPos(ptCur.x + 100, ptCur.y + 100);
            SetCursorPos(ptCur.x + 100, ptCur.y + 100);
            SetCursorPos(ptCur.x - 100, ptCur.y - 100);*/

            //AtlTrace(_T("SWitchWindow Hint Begin \r\n"));
        }
    }
    else
    {        
        g_oVirtualMouse.GenEvent(ptCur, E_LBUTTON_UP);
        //AtlTrace(_T("SWitchWindow Hint End \r\n"));
    }
}

void CWGRUtilContextImpl::TriggerMoveWindowEvent(POINT ptCur, POINT ptLast, E_MOVWND_MODE eMode)
{
    switch(eMode)
    {
    case E_MOVWND_STARTMODE:
        {
            SimulateKeyDown(VK_DOWN);
            SimulateKeyUp(VK_UP);
        }

        break;

    case E_MOVWND_EXECMODE:
        {
            //g_oMouseEventGen.GenMouseMoveEvent(&ptCur);
            POINT ptMouse;
            GetCursorPos(&ptMouse);

            ptMouse.x = ptMouse.x + ptCur.x - ptLast.x;
            ptMouse.y = ptMouse.y + ptCur.y - ptLast.y;

            /*AtlTrace(_T("\r\nTriggerMoveWndEvent: ptMouse.x = %d , ptMouse.y = %d; ptCur.x = %d; ptCur.y = % d; ptLast.x = %d; ptLast.y = % d")
                , ptMouse.x, ptMouse.y, ptCur.x, ptCur.y, ptLast.x, ptLast.y);*/

            SetCursorPos(ptMouse.x, ptMouse.y);
        }

        break;

    case E_MOVWND_ENDMODE:
        {
            SimulateKeyDown(VK_RETURN);
            SimulateKeyUp(VK_RETURN);
        }
        break;

    default:
        break;
    }
}

//////////////////////////////////////////////////////////////////////////
void CWGRUtilContextImpl::SimulateKeyDown(WORD wVk)
{
    INPUT keyBoardInput = {0};

    keyBoardInput.type = INPUT_KEYBOARD; 
    keyBoardInput.ki.wVk = wVk;

    SendInput(1, &keyBoardInput, sizeof(INPUT));
}

void CWGRUtilContextImpl::SimulateKeyUp(WORD wVk)
{
    INPUT keyBoardInput = {0};

    keyBoardInput.type = INPUT_KEYBOARD; 
    keyBoardInput.ki.wVk = wVk;
    keyBoardInput.ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(1, &keyBoardInput, sizeof(INPUT));
}

//////////////////////////////////////////////////////////////////////////
HWND CWGRUtilContextImpl::GetTopParent(const POINT &ptCur)
{
    HWND hTopParent = NULL;

    //hTopParent = (HWND)0x104FC;
    hTopParent = WindowFromPoint(ptCur);

    while(true)
    {
        HWND tmp = GetParent(hTopParent);
        if (tmp)
        {
            hTopParent = tmp;
        }
        else
        {
            break;
        }
    }

    return hTopParent;
}

HWND CWGRUtilContextImpl::GetFirstParentWithCaption(const POINT &ptCur)
{
    HWND hTopParent = NULL;

    //hTopParent = (HWND)0x104FC;
    HWND tmp = WindowFromPoint(ptCur); 
    
    while(true)
    {        
        if (tmp)
        {
            LONG wndStyle = GetWindowLong(tmp, GWL_STYLE);
            if (wndStyle & WS_POPUP)
            {
                hTopParent = tmp;

                break;
            }

            hTopParent = tmp;
        }
        else
        {
            break;
        }

        tmp = GetParent(tmp);        
    }

    return hTopParent;
}

bool CWGRUtilContextImpl::Get_TriggerEvent_Permission(E_TRIGGEREVENT eEvent, const POINT &pt)
{    
    DWORD dwPermission = g_oWGRConfig.GetPermission(pt);

    return CWGRPermissionParse::IsTheEventPermited(eEvent, dwPermission);
}

bool CWGRUtilContextImpl::IsAllGestureClosed()
{
    return g_oWGRConfig.IsAllGestureClosed();
}

bool CWGRUtilContextImpl::IsAllPalmBasedGestureClosed()
{
    return g_oWGRConfig.IsAllPalmBasedGestureClosed();
}

bool CWGRUtilContextImpl::IsAllFingerBasedGestureClosed()
{
    return g_oWGRConfig.IsAllFingerBasedGestureClosed();
}

bool CWGRUtilContextImpl::IsLogPermited()
{
    return g_oWGRConfig.IsLogPermitted();
}

int CWGRUtilContextImpl::GetMultiOfPalm2Std()
{
    return g_oWGRConfig.GetMultiplePalm2Std();
}

void CWGRUtilContextImpl::OnDisplayChange(int nScreenWidth, int nScreenHeight)
{
    g_oWGRConfig.UpdateScreenResolutionParams(nScreenWidth, nScreenHeight);
}

void CWGRUtilContextImpl::OnPhysicalLengthChange(float fPhysicalLength)
{
    g_oWGRConfig.UpdateScreenDiagonalPhysicalLength(fPhysicalLength);
}

int CWGRUtilContextImpl::MapStandX2Actual(int xStandard)
{
    return g_oWGRConfig.MapStandardX2ActualValue(xStandard);
}

int CWGRUtilContextImpl::MapStandY2Actual(int yStandard)
{
    return g_oWGRConfig.MapStandardY2ActualValue(yStandard);
}

int CWGRUtilContextImpl::GetWeightedSpotMass(const int &nActualMass, const int &nExtRectArea, const int &nStdAreaMulti)
{
    return g_oWGRConfig.GetWeightedSpotMass(nActualMass, nExtRectArea, nStdAreaMulti);
}

