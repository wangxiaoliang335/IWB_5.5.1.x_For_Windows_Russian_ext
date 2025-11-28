//#include "StdAfx.h"
#include "AutoLock.h"
#include "LocalMutex.h"
#include "../../inc/wgr/WinShellToolKit.h"
#include <ShlDisp.h>
#include <atlbase.h>

class CWinShellInstances
{
private:
    CWinShellInstances(void);
    ~CWinShellInstances(void);

public:
    static CWinShellInstances *GetInstance();
    static void Release();

public:
    //并列显示窗口
    bool ShowWindowsSidebySide();
    //最小化全部窗口
    bool MinimizeAllWindows();
    bool UndoMinizeAllWindows();

    //显示桌面
    bool DisplayDesktop();

    //窗口切换
    bool SwitchWindows(char *shortCut);

private:
    static CWinShellInstances   *m_win_shell_tool_kit;
    static int                  m_nRefs;

    static  LocalMutex          m_LocalMutex;

private:
    IShellDispatch5              *m_pShellDisp;
    HRESULT                     m_sucCreateShellInterface;
};


CWinShellInstances  *CWinShellInstances::m_win_shell_tool_kit = NULL;
int                 CWinShellInstances::m_nRefs              = 0;
LocalMutex          CWinShellInstances::m_LocalMutex;

CWinShellInstances::CWinShellInstances(void)
{
    //CoInitialize(NULL);

    m_pShellDisp = NULL;
    //m_sucCreateShellInterface = S_OK;

    m_sucCreateShellInterface = CoCreateInstance( CLSID_Shell, NULL, CLSCTX_SERVER, __uuidof(IShellDispatch5), (LPVOID *) &m_pShellDisp);
}

CWinShellInstances::~CWinShellInstances(void)
{
    if (SUCCEEDED(m_sucCreateShellInterface))
    {
        m_pShellDisp->Release();
    }

    //CoUninitialize();
}

CWinShellInstances *CWinShellInstances::GetInstance()
{
    AutoLock l(&m_LocalMutex);

    if (!m_win_shell_tool_kit)
    {
        m_win_shell_tool_kit = new CWinShellInstances();
    }

    m_nRefs++;    

    return m_win_shell_tool_kit;
}

void CWinShellInstances::Release()
{
    AutoLock l(&m_LocalMutex);

    m_nRefs--;

    if (m_nRefs < 0)
    {
        m_nRefs = 0;
    }


    if (m_nRefs == 0 && m_win_shell_tool_kit)
    {
        delete m_win_shell_tool_kit;
        m_win_shell_tool_kit = NULL;
    }
}

//并列显示窗口
bool CWinShellInstances::ShowWindowsSidebySide()
{
    if(!SUCCEEDED(m_sucCreateShellInterface)) return false;
    
    return m_pShellDisp->TileVertically() == S_OK;
}

//最小化全部窗口
bool CWinShellInstances::MinimizeAllWindows()
{
    if(!SUCCEEDED(m_sucCreateShellInterface)) return false;
    
    return m_pShellDisp->MinimizeAll() == S_OK;
}

bool CWinShellInstances::UndoMinizeAllWindows()
{
    if(!SUCCEEDED(m_sucCreateShellInterface)) return false;

    return m_pShellDisp->UndoMinimizeALL() == S_OK;
}

bool CWinShellInstances::DisplayDesktop()
{
    if(!SUCCEEDED(m_sucCreateShellInterface)) return false;

    return SUCCEEDED(m_pShellDisp->ToggleDesktop());
}

bool CWinShellInstances::SwitchWindows(char *shortCut)
{
    if(!SUCCEEDED(m_sucCreateShellInterface)) return false;

    HRESULT hr = m_pShellDisp->WindowSwitcher();

    return true;
    /*char curDir[512];
    memset(curDir, 0, sizeof(char) * 512);

    DWORD dwActualLen = GetCurrentDirectoryA(511, curDir);
    if (dwActualLen == 0) return false;

    char shortCutPath[1024];
    memset(shortCutPath, 0, sizeof(char) * 1024);

    sprintf_s(shortCutPath, _countof(shortCutPath), "%s\\%s", curDir, shortCut);
    

    BSTR bstrShortCurt = CComBSTR(shortCutPath);

    VARIANT vArguments;
    VariantInit(&vArguments);
    vArguments.vt = VT_BSTR;
    vArguments.bstrVal = CComBSTR("");  

    VARIANT vDirectory;
    VariantInit(&vDirectory);
    vDirectory.vt = VT_BSTR;
    vDirectory.bstrVal = CComBSTR("");

    VARIANT vOperation;
    VariantInit(&vOperation);
    vOperation.vt =  VT_BSTR;
    vOperation.bstrVal = CComBSTR("");

    VARIANT vShow;
    VariantInit(&vShow);
    vShow.vt = VT_INT;
    vShow.intVal = 1;

    HRESULT hRes = m_pShellDisp->ShellExecute(bstrShortCurt, vArguments, vDirectory, vOperation, vShow);
    return hRes;*/
}

//////////////////////////////////////////////////////////////////////////
CWinShellUtility::CWinShellUtility()
{
    m_shell_inst = CWinShellInstances::GetInstance();
}

CWinShellUtility::~CWinShellUtility()
{
    CWinShellInstances::Release();
    m_shell_inst = NULL;
}
//并列显示窗口
bool CWinShellUtility::ShowWindowsSidebySide()
{
    if(!m_shell_inst) return false;

    return m_shell_inst->ShowWindowsSidebySide();
}
//最小化全部窗口
bool CWinShellUtility::MinimizeAllWindows()
{
    if(!m_shell_inst) return false;

    return m_shell_inst->MinimizeAllWindows();
}
bool CWinShellUtility::UndoMinizeAllWindows()
{
    if(!m_shell_inst) return false;

    return m_shell_inst->UndoMinizeAllWindows();
}

//水平滚动最前端窗口
bool CWinShellUtility::HScroolForegroundWindow(E_ScroolType scollDir)
{
    POINT ptCur;
    GetCursorPos(&ptCur);
    HWND hForeGroundWnd = WindowFromPoint(ptCur);

    if (hForeGroundWnd)
    {
        if (scollDir == ESCRLT_LEFT)
        {
            PostMessage(hForeGroundWnd, WM_HSCROLL, SB_LINELEFT, NULL);
        }

        if (scollDir == ESCRLT_RIGHT)
        {
            PostMessage(hForeGroundWnd, WM_HSCROLL, SB_LINERIGHT, NULL);
        }

    }

    return true;
}

//竖直滚动最前端窗口
bool CWinShellUtility::VScroolForegroundWindow(E_ScroolType scollDir)
{
    POINT ptCur;
    GetCursorPos(&ptCur);
    HWND hForeGroundWnd = WindowFromPoint(ptCur);

    if (hForeGroundWnd)
    {
        if (scollDir == ESCRLT_UP)
        {
            PostMessage(hForeGroundWnd, WM_VSCROLL, SB_LINEUP, NULL);
        }

        if (scollDir == ESCRLT_DOWN)
        {
            PostMessage(hForeGroundWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
        }

    }

    return true;
}

//最小化当前窗口
bool CWinShellUtility::MiniCurrentWindow(const POINT &ptCur)
{
    HWND hWnd = GetTopParent(ptCur);    

    if (hWnd)
    {
        LONG wndStyle = GetWindowLong(hWnd, GWL_STYLE);
        if (wndStyle & WS_MINIMIZEBOX)
        {
            SetFocus(hWnd);
            BOOL bPosRes = PostMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
            int err = GetLastError();
            AtlTrace("\r\n Post MINIMIZE Windows messege!! post return value = %d err = %d", bPosRes, err);
        }        
    }

    AtlTrace("\r\n Enter into MiniCurrentWindow Function hWnd = %d", hWnd);

    return true;
}

//最大化当前窗口
bool CWinShellUtility::MaxiCurrentWindow(const POINT &ptCur)
{
    HWND hWnd = GetTopParent(ptCur);

    if (hWnd)
    {
        LONG wndStyle = GetWindowLong(hWnd, GWL_STYLE);
        if (wndStyle & WS_MAXIMIZEBOX)
        {
            SetFocus(hWnd);
            //SetForegroundWindow(hWnd);
            if (wndStyle & WS_MAXIMIZE)
            {                
                ::PostMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, NULL);
            }
            else
            {
                ::PostMessage(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, NULL);
            }
            //int err = GetLastError();

            //AtlTrace("\r\n Post MINIMIZE Windows messege!! post return value = %d err = %d", bPosRes, err);
        }        
    }

    return true;
}

bool CWinShellUtility::DisplayDesktop(const POINT &ptCur)
{
    if(!m_shell_inst) return false;

    return m_shell_inst->DisplayDesktop();
}

bool CWinShellUtility::SwitchWindows(char *shortCut)
{
    if(!m_shell_inst) return false;    

    return m_shell_inst->SwitchWindows(shortCut);
}

//////////////////////////////////////////////////////////////////////////
HWND CWinShellUtility::GetTopParent(const POINT &ptCur)
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
