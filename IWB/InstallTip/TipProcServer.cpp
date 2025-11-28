#include "TipProcServer.h"
#include "../../inc/Mf2StdGf.h"
#include "../../inc/Log.h"
#include "../../inc/MsgDefine.h"
#include "CommandLineWrapper.h"

const DWORD WM_POSTPROC_RECCMD = WM_USER + 1001;
//const DWORD WM_CHANGE_DISPLAY_WINDOW = WM_USER + 36;

const DWORD ID_SENSORCTXMENU_AUTO_CALIBRATE  = 32859;         //自动校正菜单按钮的ID
const DWORD ID_SENSORCTXMENU_LIGHTSPOT_SAMPLING = 32863;       //光斑采样菜单项的ID
const DWORD ID_WORKMODE_IMAGE_TUNNING = 32845;                 //调试摄像头模式菜单项的ID
const DWORD ID_WORKMODE_LASER_TUNNING = 32846;                 //调试激光器模式菜单项的ID
const DWORD ID_SENSORCTXMENU_INSTALL_TIP = 32864;              //安装向导菜单项的ID
const DWORD ID_SENSORCTXMENU_FOURPOINTCALIBRATION = 32865;     //四点标定
const DWORD ID_SENSORCTXMENU_DRAWMASKFRAME_START = 32866;
const DWORD ID_SENSORCTXMENU_DRAWMASKFRAME_DISABLE = 32867;
const DWORD ID_SENSORCTXMENU_DRAWMASKFRAME_CLEAR = 32868;

const int   TIMER_WAITCONNECT_TIMEOUT = 503;
const int   TIMER_WAITCONNECT_TIMEOUT_DELAY = 600 * 1000;

const int   TIMER_LOSTCONNECT      = 500;
const int   TIMER_LOSTCONNECT_DEFAULT_DELAY = 120 * 1000;

const int   TIMER_WAIT_AUTO_CALIBRATE = 501;
const int   TIMER_WAIT_AUTO_CALIBRATE_DELAY = 120 * 1000; //

const int   TIMER_WAIT_SPOT_SAMPLE = 502;
const int   TIMER_WAIT_SPOT_SAMPLE_DELAY = 300 * 1000; //

const int   KSENDMSG_TIMEOUT_DELAY = 30 * 1000; //30秒

const TCHAR *INSTALL_TIP_APP_DIR   = _T("InstallerWithTip");
const TCHAR *INSTALL_TIP_APP_NAME  = _T("IntallerWithTips.exe");

//////////////////////////////////////////////////////////////////////////
//挡板是否存在的阈值
const int KBACKSPLASH_THRESHOLD = 7 * 7;
//

//////////////////////////////////////////////////////////////////////////
//创建客户端时，通过命令行告诉客户端两个信息：
//    (1)语言信息
//    (2)Server端关联的窗口的句柄
//命令行的格式如下：
//     "-@EN@-; -@1234@-"

//////////////////////////////////////////////////////////////////////////

CTipProcServer::CTipProcServer(void)
: m_attachWnd(NULL)
, m_oldAttachWndProc(NULL)
, m_dwAuthToken(0)
, m_clientWnd(NULL)
, m_dwTimeOutDelay(TIMER_LOSTCONNECT_DEFAULT_DELAY)
, m_hadStarted(false)
, m_screenMode(E_SCREEN_Single)
, m_langCode(_T("EN"))
, m_bOnEliminateBackSplashStep(false)
{
}

CTipProcServer::~CTipProcServer(void)
{
    Stop();
}

bool CTipProcServer::Start()
{
    if (m_hadStarted)
    {
        m_hadStarted = false;
        Stop();
    }

    if(NULL == m_attachWnd) return false;

    //开启客户端进程
    PROCESS_INFORMATION processInfo;
    STARTUPINFO si;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);

    //TCHAR comLine[128];
    //memset(comLine, 0, sizeof(comLine));
    ////////////////////////////////////////////////////////////////////////////
    ////把语言编码和AttachWnd封装到CommandLine，并传给客户端进程
    //if(m_langCode == _T("")) m_langCode = _T("EN");
    //m_langCode.MakeUpper();

    //_stprintf_s(comLine, _countof(comLine), _T("-@%s@- -@%d@-"), m_langCode, m_attachWnd);
    ////////////////////////////////////////////////////////////////////////////
    CCommandLineWrapper commLineWrapper;
    commLineWrapper.WrapLangCode(m_langCode.MakeUpper());
    commLineWrapper.WrapServerWnd((DWORD)m_attachWnd);
    commLineWrapper.WrapScreenMode((byte)(m_screenMode == E_SCREEN_Double ? 0 : 1));

    TCHAR appFullPath[MAX_PATH];
    TCHAR curDir[MAX_PATH];
    TCHAR appWorkDir[MAX_PATH];
    memset(appFullPath, 0, sizeof(appFullPath));
    memset(curDir, 0, sizeof(curDir));
    memset(appWorkDir, 0, sizeof(appWorkDir));

    if (0 == GetCurrentDirectory(MAX_PATH, curDir))
    {//failed
        return false;
    }

    _stprintf_s(appWorkDir, _countof(appWorkDir), _T("%s\\%s"), curDir, INSTALL_TIP_APP_DIR);
    _stprintf_s(appFullPath, _countof(appFullPath), _T("%s\\%s"), appWorkDir, INSTALL_TIP_APP_NAME);

    BOOL res = CreateProcess(appFullPath,
        commLineWrapper.GetWrappedCommandLine(),
        NULL,//Process handle not inheritable
        NULL,//Thread handle not inheritable.
        TRUE,//Set handle inheritance to TRUE
        0,   //no creation flag
        NULL,//Use parent's environment block. 
        appWorkDir,//Use parent's starting directory
        &si, // Pointer to STARTUPINFO structure.
        &processInfo);// Pointer to PROCESS_INFORMATION structure.

    if (!res) return false; //开启客户端进程失败，直接退出

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    m_dwAuthToken = processInfo.dwProcessId; //用客户端进程的ID号作为通信验证的Token

    m_hadStarted = true;

    //该实现中，客户端和服务端的通信是通过绑定窗口的WM_COPYDATA方式进行通信的，所以需要预先过滤WM_COPYDATA消息
    //过滤方式：
    //          a.替换绑定窗口的WndProc
    //          b.判定WM_COPYDATA消息的WPARAM是否为m_dwAuthToken
    {
        WNDPROC proc = (WNDPROC)GetWindowLong(m_attachWnd, GWL_WNDPROC);
        WNDPROC subclassProc = MF2GF<WNDPROC>(&CTipProcServer::NewAttachWndProc, this);
        if((LONG)proc != LONG(subclassProc))
        {
            m_oldAttachWndProc = (WNDPROC) SetWindowLong(m_attachWnd, GWL_WNDPROC,  LONG(subclassProc));
        }
    }

    //禁用安装向导菜单项，防止用户再次点击该按钮
    PostMessage(m_attachWnd, WM_ENABLEORDISABLE_WINDOW, (WPARAM)ID_SENSORCTXMENU_INSTALL_TIP, (LPARAM)FALSE);

    SetTimer(m_attachWnd, TIMER_WAITCONNECT_TIMEOUT, TIMER_WAITCONNECT_TIMEOUT_DELAY, NULL);

    return true;
}

bool CTipProcServer::Stop()
{ 
    if (!m_hadStarted) return false;

    /*{
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);

        char szTime[64];
        _snprintf_s(szTime,sizeof(szTime),
            "[%04d/%02d/%02d %02d:%02d:%02d:%03d]",
            sysTime.wYear,
            sysTime.wMonth,
            sysTime.wDay,
            sysTime.wHour,
            sysTime.wMinute,
            sysTime.wSecond,
            sysTime.wMilliseconds);

        AfxMessageBox(szTime);
    }*/

    KillAllTimer();    

    if (m_oldAttachWndProc)
    {
        SetWindowLong(m_attachWnd, GWL_WNDPROC,  LONG(m_oldAttachWndProc));
    }

    SendCloseCommand();

    PostMessage(m_attachWnd, WM_CHANGE_DISPLAY_WINDOW, (WPARAM)m_attachWnd, 0);

    PostMessage(m_attachWnd, WM_ENABLEORDISABLE_WINDOW, (WPARAM)ID_SENSORCTXMENU_INSTALL_TIP, (LPARAM)TRUE);

    m_clientWnd = NULL;    
    //m_attachWnd = NULL;
    m_dwAuthToken = 0;
    m_oldAttachWndProc = NULL;

    m_hadStarted = false;

    m_dwTimeOutDelay = TIMER_LOSTCONNECT_DEFAULT_DELAY;
    //m_screenMode = E_SCREEN_Single;

    //m_langCode = "EN";

    m_bOnEliminateBackSplashStep = false;

    LOG_INF("TipProcServer::Stop()");

    return true;
}

void CTipProcServer::OnAutoCalibrateDone(bool bDoneSucced)
{
    if (!m_hadStarted) return;

    KillTimer(m_attachWnd, TIMER_WAIT_AUTO_CALIBRATE);

    AUTOCALI_ACK_INFO ackInfo;
    ackInfo.res = bDoneSucced ? E_ACK_Succed : E_ACK_Failed;

    SendAutoCaliACKCommand(ackInfo);
}

void CTipProcServer::OnSpotSampleDone(bool bDoneSucced)
{
    if (!m_hadStarted) return;

    KillTimer(m_attachWnd, TIMER_WAIT_SPOT_SAMPLE);

    SAMPLE_ACK_INFO ackInfo;
    ackInfo.res = bDoneSucced ? E_ACK_Succed : E_ACK_Failed;

    SendSampleACKCommand(ackInfo);
}

void CTipProcServer::OnDetectBackSlashVanish(const int &nTotalSpotArea, const int &nTotalSpots)
{
    if(!m_bOnEliminateBackSplashStep) return;

    BACKSPLASHVANISHED_INFO info;
    info.isVanished = false;
    if(KBACKSPLASH_THRESHOLD * nTotalSpots >= nTotalSpotArea) info.isVanished = true;

    SendBackSplashVanishedCommand(info);
}

bool CTipProcServer::SendConnectACKCommand(const CONN_ACK_INFO &ackInfo)
{
    IPC_CMD_INFO cmdInfo;
    cmdInfo.dwAuthToken = m_dwAuthToken;
    cmdInfo.cmd = E_IPC_CMD_Conn_ACK;
    cmdInfo.info.connAckInfo = ackInfo;

    WrapAndSendCommand(cmdInfo);

    return true;
}

bool CTipProcServer::SendRelayoutACKCommand(const RELAYOUT_ACK_INFO &ackInfo)
{
    IPC_CMD_INFO cmdInfo;
    cmdInfo.dwAuthToken = m_dwAuthToken;
    cmdInfo.cmd = E_IPC_CMD_ReLayout_ACK;
    cmdInfo.info.relayoutAckInfo = ackInfo;

    WrapAndSendCommand(cmdInfo);

    return true;
}

bool CTipProcServer::SendTuningWebcamACKCommand(const TUNINGWEBCAM_ACK_INFO &ackInfo)
{
    IPC_CMD_INFO cmdInfo;
    cmdInfo.dwAuthToken = m_dwAuthToken;
    cmdInfo.cmd = E_IPC_CMD_TuningWebcam_ACK;
    cmdInfo.info.tuningWebcamAckInfo = ackInfo;

    WrapAndSendCommand(cmdInfo);

    return true;
}

bool CTipProcServer::SendTuningLaserACKCommand(const TUNINGLASER_ACK_INFO &ackInfo)
{
    IPC_CMD_INFO cmdInfo;
    cmdInfo.dwAuthToken = m_dwAuthToken;
    cmdInfo.cmd = E_IPC_CMD_TuningLaser_ACK;
    cmdInfo.info.tuningLaserAckInfo = ackInfo;

    WrapAndSendCommand(cmdInfo);
    return true;
}

bool CTipProcServer::SendAutoCaliACKCommand(const AUTOCALI_ACK_INFO &ackInfo)
{
    IPC_CMD_INFO cmdInfo;
    cmdInfo.dwAuthToken = m_dwAuthToken;
    cmdInfo.cmd = E_IPC_CMD_AutoCali_ACK;
    cmdInfo.info.autoCaliAckInfo = ackInfo;

    WrapAndSendCommand(cmdInfo);

    return true;
}

bool CTipProcServer::SendSampleACKCommand(const SAMPLE_ACK_INFO &ackInfo)
{
    IPC_CMD_INFO cmdInfo;
    cmdInfo.dwAuthToken = m_dwAuthToken;
    cmdInfo.cmd = E_IPC_CMD_Sample_ACK;
    cmdInfo.info.sampleAckInfo = ackInfo;

    WrapAndSendCommand(cmdInfo);

    return true;
}

bool CTipProcServer::SendTestExistedACKCommand(const EXIST_ACK_INFO &ackInfo)
{
    IPC_CMD_INFO cmdInfo;
    cmdInfo.dwAuthToken = m_dwAuthToken;
    cmdInfo.cmd = E_IPC_CMD_Exist_ACK;
    cmdInfo.info.existAckInfo = ackInfo;

    WrapAndSendCommand(cmdInfo);

    return true;
}

bool CTipProcServer::SendCloseCommand()
{
    PostMessage(m_clientWnd, WM_CLOSE, m_dwAuthToken, 0);

    return true;
}

bool CTipProcServer::SendBackSplashVanishedCommand(const BACKSPLASHVANISHED_INFO &info)
{
    IPC_CMD_INFO cmdInfo;
    cmdInfo.dwAuthToken = m_dwAuthToken;
    cmdInfo.cmd = E_IPC_CMD_BackSplashVanished;
    cmdInfo.info.backsplashvanishedInfo = info;

    WrapAndSendCommand(cmdInfo);

    return true;
}


//////////////////////////////////////////////////////////////////////////
LRESULT CTipProcServer::NewAttachWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_POSTPROC_RECCMD)
    {//
        return OnPostProcRecCommand();
    }

    ParseAndProcRecieveMsg(uMsg, wParam, lParam);

    if (uMsg == WM_TIMER)
    {
        this->OnTimer(wParam);
    }

    if(m_oldAttachWndProc)
    {
        return  m_oldAttachWndProc(hWnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void CTipProcServer::ParseAndProcRecieveMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (wParam != m_dwAuthToken) return;
    
    m_curRecCMDInfo.cmd = E_IPC_CMD_Unknown;
    switch(uMsg)
    {
    case WM_COPYDATA:
        {
            COPYDATASTRUCT *pcds = reinterpret_cast<COPYDATASTRUCT *>(lParam);
            if (pcds)
            {
                m_curRecCMDInfo = *((IPC_CMD_INFO *)pcds->lpData);                
            }
        }
        break;

    case WM_IPC_TOSERVER:
        {
            m_curRecCMDInfo.cmd = (E_IPC_CMD)lParam;            
        }
        break;

    default:
        break;
    }

    if (m_curRecCMDInfo.cmd >= E_IPC_CMD_Conn && m_curRecCMDInfo.cmd <= E_IPC_CMD_Close)
    {
        ::PostMessage(m_attachWnd, WM_POSTPROC_RECCMD, 0, 0);
    }
}

bool CTipProcServer::WrapAndSendCommand(const IPC_CMD_INFO &cmdInfo)
{
    if (NULL == m_clientWnd) return false;

    COPYDATASTRUCT cds;
    memset(&cds, 0, sizeof(cds));

    cds.cbData = sizeof(cmdInfo);
    cds.lpData = (PVOID)&cmdInfo;

    LRESULT lResult = 0;
    DWORD   dwResult = 0;
    lResult = ::SendMessageTimeout(m_clientWnd, WM_COPYDATA, m_dwAuthToken, reinterpret_cast<LPARAM>(&cds), SMTO_NORMAL, KSENDMSG_TIMEOUT_DELAY, &dwResult);

    if (!lResult) return false;    

    return true;
}

LRESULT CTipProcServer::OnPostProcRecCommand()
{
    switch(m_curRecCMDInfo.cmd)
    {
    case E_IPC_CMD_Conn:
        {
            KillTimer(m_attachWnd, TIMER_WAITCONNECT_TIMEOUT);
            m_clientWnd = m_curRecCMDInfo.info.connInfo.hClientWnd;
            m_dwTimeOutDelay = m_curRecCMDInfo.info.connInfo.dwTimeout;            
            SetTimer(m_attachWnd, TIMER_LOSTCONNECT, m_dwTimeOutDelay, NULL);           

            PostMessage(m_attachWnd, WM_CHANGE_DISPLAY_WINDOW, (WPARAM)(m_curRecCMDInfo.info.connInfo.hDisplayWnd), 0);

            CONN_ACK_INFO ackInfo;
            ackInfo.res = E_ACK_Succed;

            SendConnectACKCommand(ackInfo);
        }
        break;

    case E_IPC_CMD_ReLayout:
        {
            //KillTimer(m_attachWnd, TIMER_LOSTCONNECT);
            //const RELAYOUT_INFO &layoutInfo = m_curRecCMDInfo.info.relayoutInfo;
            //
            //MoveWindow(m_attachWnd, layoutInfo.l, layoutInfo.t, layoutInfo.w, layoutInfo.h, TRUE);

            ////SetTimer(m_attachWnd, TIMER_LOSTCONNECT, m_curRecCMDInfo.info.connInfo.dwTimeout, NULL);

            //RELAYOUT_ACK_INFO ackInfo;
            //ackInfo.res = E_ACK_Succed;

            //SendRelayoutACKCommand(ackInfo);
        }
        break;

    case E_IPC_CMD_TuningWebcam:
        {
            PostMessage(m_attachWnd, WM_COMMAND, static_cast<WPARAM>(ID_WORKMODE_IMAGE_TUNNING), 0);

            TUNINGWEBCAM_ACK_INFO ackInfo;
            ackInfo.res = E_ACK_Succed;
            //ackInfo.screenMode = m_screenMode;            

            SendTuningWebcamACKCommand(ackInfo);
        }
        break;

    case E_IPC_CMD_TuningLaser:
        {
            PostMessage(m_attachWnd, WM_COMMAND, static_cast<WPARAM>(ID_WORKMODE_LASER_TUNNING), 0);

            TUNINGLASER_ACK_INFO ackInfo;
            ackInfo.res = E_ACK_Succed;            

            SendTuningLaserACKCommand(ackInfo);
        }
        break;

    case E_IPC_CMD_MakeBackSplash:
        {
            m_bOnEliminateBackSplashStep = false;
            PostMessage(m_attachWnd, WM_STOP_DETECT_BACKSPLASH_VANISH, 0, 0);
        }
        break;

    case E_IPC_CMD_EliminateBackSplash:
        {
            m_bOnEliminateBackSplashStep = true;

            PostMessage(m_attachWnd, WM_START_DETECT_BACKSPLASH_VANISH, 0, 0);
        }
        break;

    case E_IPC_CMD_AutoCali:
        {
            m_bOnEliminateBackSplashStep = false;
            PostMessage(m_attachWnd, WM_COMMAND, static_cast<WPARAM>(ID_SENSORCTXMENU_AUTO_CALIBRATE), 0);
            PostMessage(m_attachWnd, WM_STOP_DETECT_BACKSPLASH_VANISH, 0, 0);
            SetTimer(m_attachWnd, TIMER_WAIT_AUTO_CALIBRATE, TIMER_WAIT_AUTO_CALIBRATE_DELAY, NULL);
        }
        break;     

    case E_IPC_CMD_Sample:
        {
            PostMessage(m_attachWnd, WM_COMMAND, static_cast<WPARAM>(ID_SENSORCTXMENU_LIGHTSPOT_SAMPLING), 0);
            SetTimer(m_attachWnd, TIMER_WAIT_SPOT_SAMPLE, TIMER_WAIT_SPOT_SAMPLE_DELAY, NULL);
        }
        break;

    case E_IPC_CMD_Exist:
        {
            KillTimer(m_attachWnd, TIMER_LOSTCONNECT);

            EXIST_ACK_INFO ackInfo;
            ackInfo.res = E_ACK_Succed;

            SendTestExistedACKCommand(ackInfo);

            SetTimer(m_attachWnd, TIMER_LOSTCONNECT, m_dwTimeOutDelay, NULL);
        }
        break;

    case E_IPC_CMD_Close:
        {
            Stop();
        }
        break;

    }
    return 0;
}

bool CTipProcServer::OnTimer(WPARAM idTimer)
{
    switch (idTimer)
    {
    case TIMER_WAITCONNECT_TIMEOUT:
        {
            KillTimer(m_attachWnd, TIMER_WAITCONNECT_TIMEOUT);
            Stop();
        }
        return true;

    case TIMER_LOSTCONNECT:
        {
            KillTimer(m_attachWnd, TIMER_LOSTCONNECT);
            Stop();
        }
        return true;  

    case TIMER_WAIT_AUTO_CALIBRATE:
        {
            KillTimer(m_attachWnd, TIMER_WAIT_AUTO_CALIBRATE);
            AUTOCALI_ACK_INFO ackInfo;
            ackInfo.res = E_ACK_Failed;

            SendAutoCaliACKCommand(ackInfo);
        }
        return true;

    case TIMER_WAIT_SPOT_SAMPLE:
        {
            KillTimer(m_attachWnd, TIMER_WAIT_SPOT_SAMPLE);
            SAMPLE_ACK_INFO ackInfo;
            ackInfo.res = E_ACK_Failed;
            SendSampleACKCommand(ackInfo);
        }
        return true;

    default:
        return false;
    }
}

void CTipProcServer::KillAllTimer()
{
    KillTimer(m_attachWnd, TIMER_WAITCONNECT_TIMEOUT);
    KillTimer(m_attachWnd, TIMER_LOSTCONNECT);
    KillTimer(m_attachWnd, TIMER_WAIT_AUTO_CALIBRATE);
    KillTimer(m_attachWnd, TIMER_WAIT_SPOT_SAMPLE);
}
