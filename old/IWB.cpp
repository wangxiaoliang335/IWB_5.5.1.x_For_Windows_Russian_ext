// OpticalPen.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
//#include "headers.h"

#include "../inc/EasiSDKReg_Interface.h"
#pragma comment(lib,"../lib/EasiSDKReg_Interface.lib")

//#ifdef NDEBUG
#include "../bitanswer/BitAnswerCpp.h"
#include "OnlineRegisterDlg.h"
//<<added by xuke, 2015/10/27
CBitAnswer  g_bitanswer;//比特安索在线注册对象。
//>>
//#endif

#define new DEBUG_NEW
// {451B7BA1-77F6-475E-BC90-D8EA95FAFAA1}
//static const GUID <<name>>=
//{ 0x451b7ba1, 0x77f6, 0x475e,{ 0xbc, 0x90, 0xd8, 0xea, 0x95, 0xfa, 0xfa, 0xa1 } };
static const TCHAR* g_szForBetweenInstanceNotification = _T("451B7BA1-77F6-475E-BC90-D8EA95FAFAA1");

//@功能:返回实际的触控类型
EDeviceTouchType GetActualTouchType()
{
    //return E_DEVICE_FINGER_TOUCH_CONTROL;

    switch (theApp.GetUSBKeyTouchType())
    {
    case  E_DEVICE_PEN_TOUCH_WHITEBOARD:
        //加密狗为笔触模式电子白板, 强制工作模式为笔触电子白板模式
        return E_DEVICE_PEN_TOUCH_WHITEBOARD;
        break;
    case  E_DEVICE_FINGER_TOUCH_CONTROL:
        //加密狗为手指触控时，强制工作模式为手指触控模式。
        return E_DEVICE_FINGER_TOUCH_CONTROL;
        break;
    case E_DEVICE_PALM_TOUCH_CONTROL:
        //加密狗为手掌触控时，强制工作模式是手掌互动模式
        return E_DEVICE_PALM_TOUCH_CONTROL;
        break;
    default:
        //加密狗为手触模式, 选用用户选择的触控模式
        EDeviceTouchType eTouchType;

        TSensorModeConfig* pSensorModeConfig = NULL;
        EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;
        pSensorModeConfig = &g_tSysCfgData.vecSensorConfig[0].vecSensorModeConfig[eProjectionMode];

        eTouchType = pSensorModeConfig->advanceSettings.m_eTouchType;

        if (eTouchType == E_DEVICE_FINGER_TOUCH_CONTROL || eTouchType == E_DEVICE_PALM_TOUCH_CONTROL)
        {
            return E_DEVICE_FINGER_TOUCH_WHITEBOARD;
        }

        return eTouchType;

        break;
    }

    //    if(theApp.GetUSBKeyTouchType() == E_DEVICE_PEN_TOUCH_WHITEBOARD)
    //   {//加密狗为笔触模式, 强制工作模式为笔触模式
    //        return E_DEVICE_PEN_TOUCH_WHITEBOARD;
    //    }
    //    else
    //    { 
    //		//加密狗为手触模式, 选用用户选择的触控模式
    //		if (g_tSysCfgData.globalSettings.eProjectionMode == E_PROJECTION_DESKTOP)
    //		{
    //           return g_tSysCfgData.vecSensorConfig[0].vecSensorModeConfig[0].advanceSettings.m_eTouchType;
    //		}
    //		return g_tSysCfgData.vecSensorConfig[0].vecSensorModeConfig[1].advanceSettings.m_eTouchType;
    //    }

}

//@功  能:返回实际的屏幕控制区域
//@参  数:
//@返回值:
SIZE GetActualScreenControlSize()
{
    SIZE szScreen;

    if (theApp.GetScreenMode() >= EScreenModeDouble)
    {
        //<<temp, 2017/08/22
        //szScreen.cx = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
        //szScreen.cy = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

        szScreen.cx = ::GetSystemMetrics(SM_CXSCREEN);
        szScreen.cy = ::GetSystemMetrics(SM_CYSCREEN);
        //temp, 2017/08/22>>
    }
    else
    {
        szScreen.cx = ::GetSystemMetrics(SM_CXSCREEN);
        szScreen.cy = ::GetSystemMetrics(SM_CYSCREEN);
    }

    return szScreen;
}

// CIWBApp
BEGIN_MESSAGE_MAP(CIWBApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CIWBApp construction

CIWBApp::CIWBApp()
    :
    m_hMutex(NULL),
    m_bAutoRunMode(FALSE),
    m_bForAllUser(FALSE),
    //m_eUSBKeyTouchType(E_DEVICE_FINGER_TOUCH_WHITEBOARD),
    //m_ePalmTouchControlType(E_PLAM_TOUCHCONTROL_P0),
    //m_eScreenModeFromUsbKey(EScreenModeSingle),
    //m_bFoundHardwareUSBKey(FALSE),
    m_uHardwareUSBKeyCount(0),
    m_bIsOnlineRegistered(FALSE),
    m_eScreenMode(EScreenModeSingle)
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
    //提升程序进程优先级为实时
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
}

CIWBApp::~CIWBApp()
{
    if (m_hMutex)
    {
        //Use the CloseHandle function to close the handle. The system closes the handle automatically when the process terminates. 
        //The mutex object is destroyed when its last handle has been closed.
        //主动关闭，不依赖进程的终止。
        CloseHandle(m_hMutex);//2014/04/01
    }
}

// The one and only CIWBApp object
CIWBApp theApp;
//#ifdef EVALUATION_VERSION
//试用时间单位:ms
#define EVALUATION_USE_TIME (30*60*1000)

DWORD g_dwBeginTime;
bool  g_bAlertDlgShowing = false;
VOID CALLBACK  timerProc(
    _In_ HWND     hwnd,
    _In_ UINT     uMsg,
    _In_ UINT_PTR idEvent,
    _In_ DWORD    dwTime)
{

    DWORD dwElapseTime = dwTime - g_dwBeginTime;

    if (dwElapseTime > EVALUATION_USE_TIME)
    {

        if (!g_bAlertDlgShowing)
        {
            g_bAlertDlgShowing = true;
            //MessageBox(NULL, _T("很抱歉,软件试用时间到, 请重新启动应用程序"), _T("通知"), MB_OK);
            MessageBox(NULL, g_oResStr[IDS_STRING478], g_oResStr[IDS_STRING479], MB_OK);
            PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_CLOSE, 0, 0);
        }
    }

}
//#endif

// CIWBApp initialization

BOOL CIWBApp::InitInstance()
{
    LOG_INF("CIWBApp::InitInstance");

    SKDREG_Init();

    LOG_INF("SKDREG_Init");

    //初始化目录信息
    InitDirectoryInformation();

    LOG_INF("InitDirectoryInformation() done!");

    //载入配置信息
    ::LoadConfig(PROFILE::CONFIG_FILE_NAME, g_tSysCfgData);
    m_strLanguageCode = g_tSysCfgData.globalSettings.langCode;

    LOG_INF("LoadConfig done!");
    //事先载入语言资源，因为如果有多个实例时需要给出提示信息
    //查找OEM_RES.dll文件, 如果有则载入, 否则载入
    HINSTANCE hResource = NULL;
    hResource = LoadOEMResource();

    if (!hResource)//载入OEM资源失败,则依照
    {

        //从配置文件中读取配置的语言
        //若未发现配置信息则缺省为系统的默认语言。
        //LCID lcid = PROFILE::GetCfg_LCID();

        //lcid = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
        hResource = LoadResDll(this->m_hInstance, m_strLanguageCode);

        //<added by jiqw 2015/05/11
        g_tSysCfgData.globalSettings.langCode = m_strLanguageCode; //当使用本地默认语言时，把为""的g_tSysCfgData.globalSettings.langCode设置为本地语言编码
        //>
    }

    if (hResource == NULL)
    {
        //载入失败则缺省载入英文资源
        m_strLanguageCode = _T("EN");
        g_oResStr.SetResInst(AfxGetResourceHandle());

    }
    else
    {
        AfxSetResourceHandle(hResource);
        _AtlBaseModule.SetResourceInstance(hResource);
        g_oResStr.SetResInst(hResource);
       
    }
    g_oResStr.Initialize();

    LOG_INF("g_oResStr.Initialize() done!");
    //注册实例间通信消息
    m_uBetweenInstanceMsg = RegisterWindowMessage(g_szForBetweenInstanceNotification);

    m_hMutex = CreateMutex(NULL, TRUE, _T("Global\\OpticalPen_20140630"));
    if (m_hMutex != NULL)
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            LOG_INF("ERROR_ALREADY_EXISTS");
            LPARAM lParam = 0;
            WPARAM wParam = 0;
            PostMessage(HWND_BROADCAST, m_uBetweenInstanceMsg, wParam, lParam);

            return FALSE;
        }
    }
    else
    {
        DWORD dwError = GetLastError();
        //
        if (ERROR_ACCESS_DENIED == dwError)
        {
            MessageBox(NULL, g_oResStr[IDS_STRING106], g_oResStr[IDS_STRING107], MB_ICONINFORMATION);
            return FALSE;
        }
        CAtlString strError = GetErrorMessage(dwError);
        LOG_ERR("CreateMutex Failed(0x%x):%s", dwError, (const char*)CT2CA(strError));

    }

    LOG_INF("CreateMutex done!");
    //AsyncLogInit(_T("OpticalPen.log"));
    //#ifdef _DEBUG
    //	static CMemLeakDetect oMemLeakDetector;
    //#endif 

    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    LOG_INF("WinApp::InitInstance() done!");

    AfxEnableControlContainer();

    LOG_INF("AfxEnableControlContainer done!");
    //
    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization
    SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    LOG_INF("SetRegistryKey done!");
    //Sleep(5000);//延迟启动5秒中
    //m_oUSBCameraList.UpdateDeviceList();

    //int nDeviceCount = m_oUSBCameraList.GetDeviceInstanceCount();
    //说明没有发现任何实例。那么就需要弹出没有摄像头。
    //if (nDeviceCount == 0)
    //{
        //MessageBox(NULL, g_oResStr[IDS_STRING445], g_oResStr[107], MB_ICONINFORMATION | MB_OK);
    //}
    //else
    //{     
    BOOL bFirstTime = TRUE;
    ReadUSBKey(bFirstTime);
    LOG_INF("ReadUSBKey(bFirstTime) done!");
    //}
    //<<del
    ////说明加密狗是双屏拼接，  再就是验证分辨率的长宽比例，如果长宽的比例小于16:10的时候，说明是单屏的，只是双屏的加密狗而已
    //del>>

    int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
    int nCyScreen = GetSystemMetrics(SM_CYSCREEN);


    int nCxVScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int nCyVScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    int nVScreenLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int nVScreenTop = GetSystemMetrics(SM_YVIRTUALSCREEN);

    m_eScreenMode = GetScreenMode();

    LOG_INF("Screen Size = %d X %d, Virtual Screen Size = %d X %d, <Left,Top>=<%d,%d>,  Multiple Screen Merge Eabled:%s\n",
        nCxScreen,
        nCyScreen,
        nCxVScreen,
        nCyVScreen,
        nVScreenLeft,
        nVScreenTop,
        (m_eScreenMode >= EScreenModeDouble) ? "Yes" : "No");

    int nDisplayDevCount = m_oDispMonitorFinder.GetDisplayDevCount();

    UINT32 nMaxActiveCx = 0, nMaxActiveCy = 0;

    for (int i = 0; i < nDisplayDevCount; i++)
    {
        const DisplayDevInfo* pDisplayDevInfo = m_oDispMonitorFinder.GetDisplayDevInfo(i);

        if (pDisplayDevInfo == NULL) continue;

        LOG_INF("Display Monitor Left=%d, top=%d right=%d, bottom=%d",
            pDisplayDevInfo->rcMonitor.left,
            pDisplayDevInfo->rcMonitor.top,
            pDisplayDevInfo->rcMonitor.right,
            pDisplayDevInfo->rcMonitor.bottom);

        int nDiplayAdpaterInfoCount = pDisplayDevInfo->displayAdapterInfos.size();

        for (int j = 0; j < nDiplayAdpaterInfoCount; j++)
        {
            const DiplayAdpaterInfo& inf = pDisplayDevInfo->displayAdapterInfos[j];

            LOG_INF("target-mode:Active Size=%dX%d, Totoal Size=%dX%d",
                inf.targetMode.targetVideoSignalInfo.activeSize.cx,
                inf.targetMode.targetVideoSignalInfo.activeSize.cy,
                inf.targetMode.targetVideoSignalInfo.totalSize.cx,
                inf.targetMode.targetVideoSignalInfo.totalSize.cy);

            LOG_INF("source-mode:width=%d, height =%d",
                inf.sourceMode.width,
                inf.sourceMode.height);

        }//for-each(j)

    }//for-each(i)

    ParseCmdLine(this->m_lpCmdLine);
    LOG_INF("ParseCmdLine");

    CIWBDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with OK
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with Cancel
    }

    //AsyncLogUninit();

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    LOG_INF("Exit CIWBApp::InitInstance()");

    SKDREG_Uninit();

    return FALSE;
}


BOOL CIWBApp::ParseCmdLine(LPCTSTR lpCmdLine)
{
    //将空格或者Tab符看作分隔符,解析命令行参数中的参数,每个参数放在字符串列表中
    std::list<CAtlString> lstParam;
    //
    if (lpCmdLine == NULL) return FALSE;

    //Step1:分割参数
    const TCHAR* pChar = lpCmdLine;//字符串中指向某位置字符的指针
    const TCHAR* pFirstPos = lpCmdLine;//解析的参数字串的第一个字符

    do
    {
        //过滤空格和TAB分隔符
        while (*pChar != _T('\0') && (*pChar == _T(' ') || *pChar == _T('\t')))
        {
            pChar++;
            pFirstPos++;
        }
        if (*pChar != _T('\0'))
        {

            while (*pChar != _T('\0'))
            {
                if (*pChar == _T(' ') || *pChar == _T('\t'))//出现了分隔符
                {
                    if (*pFirstPos != _T(' ') && *pFirstPos != _T('\t'))
                    {
                        //std::string str;
                        //str.resize(pChar - pFirstPos + 1/*结尾NULL*/);
                        CAtlString str;
                        int strLength = pChar - pFirstPos;
                        TCHAR* pBuf = str.GetBuffer(strLength + 1/*结尾NULL*/);
                        _tcsncpy_s(pBuf, strLength + 1, pFirstPos, strLength);

                        pBuf[strLength] = _T('\0');

                        str.ReleaseBuffer();

                        //找到一个参数
                        lstParam.push_back(str);

                        //
                        pFirstPos = pChar;
                        break;
                    }//if

                }//if

                pChar++;
            }//while
        }//if


    } while (*pChar != _T('\0'));

    if ((pChar - pFirstPos) > 0)
    {

        CAtlString str;
        int strLength = pChar - pFirstPos;
        TCHAR* pBuf = str.GetBuffer(strLength + 1/*结尾NULL*/);
        _tcsncpy_s(pBuf, strLength + 1, pFirstPos, strLength);

        pBuf[strLength] = _T('\0');

        str.ReleaseBuffer();

        //找到一个参数
        lstParam.push_back(str);

    }

    for (std::list<CAtlString>::const_iterator it = lstParam.begin(); it != lstParam.end(); it++)
    {
        if (_tcsicmp(*it, _T("-autorun")) == 0)
        {
            m_bAutoRunMode = TRUE;
        }
        if (_tcsicmp(*it, _T("-allusers")) == 0)
        {
            m_bForAllUser = TRUE;
        }

    }

    return TRUE;
}


//@功能: 载入OEM资源。
//@说明:OEM, Original Equipment Manufacture
//OEM,  manufactures products or components that are purchased by a company 
//      and retailed under that purchasing company's brand name.
//
HINSTANCE CIWBApp::LoadOEMResource()
{
    HINSTANCE hInstance = NULL;
    TCHAR szFullFileName[MAX_PATH];
    LPTSTR pszFileName = NULL;

    int ret = ::GetModuleFileName(this->m_hInstance, szFullFileName, MAX_PATH);
    if (ret == 0 || ret == MAX_PATH)
    {
        ASSERT(FALSE);
        return NULL;
    }
    pszFileName = ::PathFindFileName(szFullFileName);

    if (pszFileName)
    {
        *pszFileName = _T('\0');
        _tcscat_s(szFullFileName, _countof(szFullFileName), _T("OEM_RES.dll"));
    }

    hInstance = ::LoadLibrary(szFullFileName);

    return hInstance;
}


//<<added by toxuke@gmail.com, 2013/10/10
//@功能:初始化程序依赖的路径名称
//      包括配置文件的目录路径和调试输出文件保存的目录路径。
void CIWBApp::InitDirectoryInformation()
{
    //获取当前工作路径
    TCHAR szPath[MAX_PATH];
    TCHAR* pcwd = _tgetcwd(szPath, _countof(szPath));
    if (!pcwd)
    {
        //致命错误
        assert(FALSE);
        return;
    }

    //判断是否在U盘或者在固定介质上上运行
    int iDrive = PathGetDriveNumber(pcwd);


    BOOL bIntermediatOutputInSystemTempDir = FALSE;//调试目录在系统临时目录下标志
    TCHAR szDrive[4];

    if (iDrive != -1)
    {
        memset(szDrive, 0, sizeof(szDrive));
        PathBuildRoot(szDrive, iDrive);
        UINT uType = GetDriveType(szDrive);

        switch (uType)
        {
        case DRIVE_UNKNOWN:
            //The drive type cannot be determined.
            bIntermediatOutputInSystemTempDir = TRUE;

            break;

        case DRIVE_NO_ROOT_DIR:
            //The root szPath is invalid; for example, there is no volume is mounted at the szPath.
            bIntermediatOutputInSystemTempDir = TRUE;
            break;

        case DRIVE_REMOVABLE:
            //The drive has removable media; for example, a floppy drive, thumb drive, or flash card reader.
            bIntermediatOutputInSystemTempDir = TRUE;
            break;

        case DRIVE_FIXED:
            //The drive has fixed media; for example, a hard drive or flash drive.
            bIntermediatOutputInSystemTempDir = FALSE;
            break;

        case DRIVE_REMOTE:
            //The drive is a remote (network) drive.
            bIntermediatOutputInSystemTempDir = TRUE;
            break;

        case DRIVE_CDROM:
            //The drive is a CD-ROM drive.
            bIntermediatOutputInSystemTempDir = TRUE;
            break;

        case DRIVE_RAMDISK:
            //The drive is a RAM disk
            bIntermediatOutputInSystemTempDir = TRUE;

            break;
        }//switch

    }

    //
    if (bIntermediatOutputInSystemTempDir)
    {
        TCHAR szTmpPath[MAX_PATH];
        memset(szTmpPath, 0, _countof(szTmpPath));

        //在系统临时目录中创建输出目录
        GetTempPath(_countof(szTmpPath), szTmpPath);//D:\Users\k.xu\AppData\Local\Temp\

        m_strIntermediatOutputDir = szTmpPath;
        m_strIntermediatOutputDir += _T("Intermediate(EASI Gloview)");

    }
    else
    {
        //在当前目录下临时输出目录
        m_strIntermediatOutputDir = pcwd;
        m_strIntermediatOutputDir += _T("\\Intermediate");
    }
    //
    BOOL bConfigUSBDiskFound = FALSE;//查看存放配置文件的U盘是否存在


    //易视
    //const TCHAR* szConfigUSBDiskVolumeName = _T("ISAESY@9168");
    //新加坡
    //const TCHAR* szConfigUSBDiskVolumeName = _T("V-ID2");
    //{{OEM}}
    const TCHAR* szConfigUSBDiskVolumeName = _T("ISAESY@9168");
    //{{OEM}}


    //枚举所有逻辑盘
    DWORD dwMaskBits = GetLogicalDrives();
    iDrive = 0;
    while (dwMaskBits)
    {
        if (dwMaskBits & 0x00000001)
        {
            memset(szDrive, 0, sizeof(szDrive));
            PathBuildRoot(szDrive, iDrive);
            UINT uType = GetDriveType(szDrive);
            if (uType == DRIVE_REMOVABLE)
            {
                TCHAR szVolumeName[MAX_PATH + 1];
                memset(szVolumeName, 0, sizeof(szVolumeName));

                TCHAR szFileSystemNameBuffer[MAX_PATH + 1];
                memset(szFileSystemNameBuffer, 0, sizeof(szFileSystemNameBuffer));

                DWORD dwMaximumComponentLength = 0;
                DWORD dwFileSystemFlags = 0;


                BOOL bRet =
                    GetVolumeInformation(
                        szDrive,                  //lpRootPathName
                        szVolumeName,             //lpVolumeNameBuffer
                        _countof(szVolumeName),   //nVoumeNameSize
                        NULL,                     //lpVolumeSerialNumber
                        &dwMaximumComponentLength,//lpMaximumComponentLength 
                        &dwFileSystemFlags,//lpFileSystemFlags
                        szFileSystemNameBuffer,//lpFileSystemNameBuffer,
                        _countof(szFileSystemNameBuffer)//
                    );

                if (bRet)
                {

                    if (_tcsicmp(szConfigUSBDiskVolumeName, szVolumeName) == 0)
                    {
                        bConfigUSBDiskFound = TRUE;
                        break;
                    }
                }
            }
        }
        iDrive++;
        dwMaskBits >>= 1;
    }//while

    if (bConfigUSBDiskFound)
    {
        m_strSettingsDir = szDrive;
    }
    else
    {
        m_strSettingsDir = pcwd;
    }

    m_strSystemDir = pcwd;
    //设置
    ALGOAPI_SetIntermediateDir(m_strIntermediatOutputDir);
    //设置配置路径
    PROFILE::CONFIG_FILE_NAME = m_strSettingsDir + _T("\\Video.dll");
    //配置信息保存的根目录
    PROFILE::SETTINGS_BASE_DIRECTORY = m_strSettingsDir;

    m_strFirmwareDirectory = m_strSettingsDir + _T("\\firmware");
}

EDeviceTouchType  CIWBApp::GetUSBKeyTouchType()const
{
    EDeviceTouchType eDeviceTouchType = E_DEVICE_NOT_FOUND;

    for (auto it = m_AllUSbKeyInformations.begin(); it != m_AllUSbKeyInformations.end(); it++)
    {
        if (it->second.eUSBKeyTouchType > eDeviceTouchType)
        {
            eDeviceTouchType = it->second.eUSBKeyTouchType;
        }

    }

    return eDeviceTouchType;

    //return this->m_eUSBKeyTouchType;
}

EPalmTouchControlType CIWBApp::GetPalmTouchType()const
{
    EPalmTouchControlType ePalmTouchControlType = E_PLAM_TOUCHCONTROL_P0;
    
    for (auto it = m_AllUSbKeyInformations.begin(); it != m_AllUSbKeyInformations.end(); it++)
    {
        if (it->second.ePalmTouchControlType > ePalmTouchControlType)
        {
            ePalmTouchControlType = it->second.ePalmTouchControlType;
        }

    }
    return ePalmTouchControlType;

    //return this->m_ePalmTouchControlType;
}

EFingerTouchControlType CIWBApp::GetFingerTouchType() const
{
    EFingerTouchControlType eFingerTouchControlType = E_FINGER_TOUCHCONTROL_UNKNOWN;

    for (auto it = m_AllUSbKeyInformations.begin(); it != m_AllUSbKeyInformations.end(); it++)
    {
        if (it->second.eFingerTouchControlType > eFingerTouchControlType)
        {
            eFingerTouchControlType = it->second.eFingerTouchControlType;
        }

    }
    return eFingerTouchControlType;
    //return this->m_eFingerTouchControlType;
}

EScreenMode CIWBApp::GetScreenMode()const
{
    //此处特别耗时
    EScreenMode eScreenMode = GetScreenModeFromUSBKey();
    
    //add by vera_zhao2019.12.12
    if (g_tSysCfgData.globalSettings.eScreenMode == EScreenModeUnknown)
    {
        g_tSysCfgData.globalSettings.eScreenMode = eScreenMode;
    }

    if (g_tSysCfgData.globalSettings.eScreenMode < eScreenMode)
    {
        eScreenMode = g_tSysCfgData.globalSettings.eScreenMode;
    }

    //if (eScreenMode == EScreenModeUnknown)
    //{
    //    eScreenMode  = EScreenModeSingle;
    //}

    return eScreenMode;
}

BOOL CIWBApp::IsFourPointCalibrateEnabled()const
{
    for (auto it = m_AllUSbKeyInformations.begin(); it != m_AllUSbKeyInformations.end(); it++)
    {
        if (it->second.bEnableFourPointCalibrate)
        {
            return TRUE;
        }

    }

    return FALSE;
}

//int  CIWBApp::GetAllUSBKeyTouchTypeCount()
//{
//    return  m_AllUSbKeyInformations.size();
//}
//const USBKeyInformation* CIWBApp::GetAllUSBKeyInformations() const
//{
//    return &m_VecAllUsbKeyTouchType[0];
//}


const  std::unordered_map<std::string, USBKeyInformation>& CIWBApp::GetAllUSBKeyInformations() const
{
    return m_AllUSbKeyInformations;
}


//@功能:从USBKey中读取信息
//@参数:bFirstTime, 第一次检测UsbKey的存在
//@说明:第一次检测UsbKey时允许弹出对话框, 并记录日志信息。
//      第二次及以后则不再弹出对话框。
void CIWBApp::ReadUSBKey(BOOL bFirstTime)
{
    LOG_INF("ReadUSBKey() begin");
    //屏幕模式缺省为单屏模式
    //m_eScreenMode      = EScreenModeSingle;
    //m_eScreenModeFromUsbKey = EScreenModeSingle;

    //手触/笔触模式
    //m_eUSBKeyTouchType = E_DEVICE_PEN_TOUCH_WHITEBOARD;
   
    //手掌互动是计算加密狗的个数；
    //int nUSBKeyTouchCount = 0;

    //BOOL bDoubleScreenTouchMerge = FALSE;//双屏拼接功能检测

    DWORD dwStartTick = GetTickCount();
    //BOOL bFoundUSBKey = FALSE;
    UINT uUsbKeyFound = 0;
    do
    {

        UINT uKeyNum = SDKREG_GetUSBKeyCount();

        LOG_INF("SDKREG_GetUSBKeyCount=%d", uKeyNum);
        
        ResetCompensateData();

        LOG_INF("ResetCompensateData done!");

        //读取所有的注册信息，信息合并在ReadUSBKeyData中完成。
        for (UINT uKeyIndex = 0; uKeyIndex < uKeyNum; uKeyIndex++)
        {
            if (ReadUSBKeyData(uKeyIndex))
            {
                LOG_INF("ReadUSBKeyData %d", uKeyIndex);
                uUsbKeyFound++;
            }
        }//for

        if (uUsbKeyFound == 0)
        {
            //未找到USBKey, 尝试读取在线注册信息
            if (bFirstTime)
            {
                LOG_DBG("nFoundUSBKey=%d,SDKREG_IsBasicFunctionEnabled=%d\n",
                    uUsbKeyFound,
                    SDKREG_IsBasicFunctionEnabled(14)
                );
                DWORD dwNow = GetTickCount();
                DWORD dwElapse = dwNow - dwStartTick;

                if (dwElapse < (DWORD)(g_tSysCfgData.globalSettings.nMaxTimeInSearchDevice))
                {
                    LOG_INF("Not find USBKey then delay 1 second, time has elapsed %fs\n", (float)dwElapse / 1000.0);
                    Sleep(1000);//延迟等待1秒钟
                    continue;
                }
            }


            //判断是否是设备未插入导致的找不到加密信息。
            CUsbCameraDeviceList USBCameraList;
            USBCameraList.UpdateDeviceList();
            int nDeviceCount = USBCameraList.GetDeviceInstanceCount();
            //说明没有发现任何实例。那么就需要弹出没有摄像头。
            if (nDeviceCount == 0)
            {
                MessageBox(NULL, g_oResStr[IDS_STRING445], g_oResStr[107], MB_ICONINFORMATION | MB_OK);
                break;
            }



            BIT_STATUS status = g_bitanswer.Login("", BIT_MODE_AUTO);

            LOG_INF("g_bitanswer.Login done!");

            USBKeyInformation usbKeyInformation;

            if (status == BIT_SUCCESS)
            {//成功了
                //bFoundUSBKey = TRUE;
                uUsbKeyFound++;
                //Read Features
                BIT_UINT32 value;
                BIT_STATUS status = g_bitanswer.ReadFeature(FEATURE_TOUCH_TYPE, &value);


                if (status == BIT_SUCCESS)
                {
                    //m_eUSBKeyTouchType = (value == 0) ? E_DEVICE_PEN_TOUCH_WHITEBOARD : E_DEVICE_FINGER_TOUCH_WHITEBOARD;
                    usbKeyInformation.eUSBKeyTouchType = (value == 0) ? E_DEVICE_PEN_TOUCH_WHITEBOARD : E_DEVICE_FINGER_TOUCH_WHITEBOARD;
                }

                status = g_bitanswer.ReadFeature(FEATURE_SCREEN_TYPE, &value);

                //在线注册的信息读取的屏幕类型就是EScreenMode枚举值
                if (status == BIT_SUCCESS)
                {
                    if (EScreenModeSingle <= (EScreenMode)value && (EScreenMode)value <= EScreenModeHexa)
                    {
                        //if (m_eScreenModeFromUsbKey < (EScreenMode)value)
                        //{
                        //    m_eScreenModeFromUsbKey = (EScreenMode)value;
                        //}

                        usbKeyInformation.eScreenModeFromUsbKey = (EScreenMode)value;
                    }
                }

                std::string map_key = "bitanwser";
                m_AllUSbKeyInformations[map_key] = usbKeyInformation;



            }
            else
            {
                if (bFirstTime)
                {
                    LOG_ERR("bitAnswer login in returns 0x%x\n", status);
                    COnlineRegisterDlg onlineRegisterDlg;
                    onlineRegisterDlg.DoModal();

                    if (onlineRegisterDlg.IsRegisteredOk())
                    {
                        //m_eUSBKeyTouchType = onlineRegisterDlg.GetTouchType();

                        usbKeyInformation.eUSBKeyTouchType = onlineRegisterDlg.GetTouchType();

                        //bDoubleScreenTouchMerge = onlineRegisterDlg.GetScreenType() == EDoubleScreenMode ? TRUE : FALSE;
                        //if (m_eScreenModeFromUsbKey < onlineRegisterDlg.GetScreenMode())
                        //{
                        //    m_eScreenModeFromUsbKey = onlineRegisterDlg.GetScreenMode();
                        //}

                        usbKeyInformation.eScreenModeFromUsbKey = onlineRegisterDlg.GetScreenMode();

                        std::string map_key = "bitanwser";
                        m_AllUSbKeyInformations[map_key] = usbKeyInformation;
                        break;
                    }

                    //开启试用版超时检测器。
                    SetTimer(NULL, 0, 1000, timerProc);
                    g_dwBeginTime = GetTickCount();

                    //m_eUSBKeyTouchType = E_DEVICE_FINGER_TOUCH_WHITEBOARD;

                    LOG_INF("Start Evaluation Timer\n");
                }
            }//else

            break;//跳出大循环
        }//if(!bFoundUSBKey)
    }while (uUsbKeyFound == 0);

    /*
    if (m_eUSBKeyTouchType == E_DEVICE_PALM_TOUCH_CONTROL && m_ePalmTouchControlType == E_PLAM_TOUCHCONTROL_P1)
    {
        m_eScreenModeFromUsbKey = EScreenModeSingle;
    }
    else if (m_eUSBKeyTouchType == E_DEVICE_PALM_TOUCH_CONTROL && m_eScreenModeFromUsbKey == EScreenModeSingle)
    {

        CUsbCameraDeviceList   usbCameraList;//视频设备列表
        usbCameraList.UpdateDeviceList();
        int nSersorcount = usbCameraList.GetDeviceInstanceCount();
        switch (nSersorcount)
        {
        case 2:
            m_eScreenModeFromUsbKey = EScreenModeDouble;
            break;
        case 3:
            m_eScreenModeFromUsbKey = EScreenModeTriple;
            break;
        case 4:
            m_eScreenModeFromUsbKey = EScreenModeQuad;
            break;
        case 5:
            m_eScreenModeFromUsbKey = EScreenModeQuint;
            break;
        case 6:
            m_eScreenModeFromUsbKey = EScreenModeHexa;
            break;
        }
    }
    else
    {
    }
    */

    //m_bFoundHardwareUSBKey = bFoundUSBKey;
    m_uHardwareUSBKeyCount = uUsbKeyFound;

    LOG_INF("ReadUSBKey() end");
}

BOOL CIWBApp::ReadUSBKeyData(UINT uKeyIndex)
{
    USBKeyInformation usbKeyInformation;

    //读校正数据,读数据之前初始化数组
    char szDevPath[MAX_PATH];
    //SDKREG_GetUSBKeyDevPath(uKeyIndex, szDevPath, _countof(szDevPath));

    TAutoCalibrateCompensateData compensateData;

    int nCount = SDKREG_ReadE2PROMCompensateParams((double*)&compensateData, COMPENSATE_PARAM_COUNT, uKeyIndex, szDevPath, _countof(szDevPath));
    _strlwr_s(szDevPath, MAX_PATH);
    std::string strDevPath = szDevPath;
    

    if (nCount == COMPENSATE_PARAM_COUNT && strDevPath != "")
    {
        m_allCompensateCoefs[strDevPath] = compensateData;

        LOG_INF(
            "autocalibrate compensate coefs:\nthrow-ratio=%e\nu0=%e\nv0=%e\nk1=%e\nk2=%e\nk3=%e\n",
            compensateData.throwRatioOfLens,
            compensateData.coefs.u0,
            compensateData.coefs.v0,
            compensateData.coefs.k[0],
            compensateData.coefs.k[1],
            compensateData.coefs.k[2]);
    }



    int nAppType = 0;
    float fVersion = 0.0f;
    int   nPalmType = 0;
    int   nFingerContorlType = 0;
    HRESULT hr = SDKREG_GetVersion(&fVersion, uKeyIndex);

    if (SDKREG_GetVersion(&fVersion, uKeyIndex) != S_OK)
    {
        return FALSE;
    }

    if (fVersion < 0.20111018f)
    {
        return FALSE;
    }
    //AppType各位描述
    //
    //bit9 bit8 bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
    //      │                                      │
    //      │                                      └─0:3D Touch；1:手指触控
    //      └─1:使能双屏拼接功能                                                 
    //
    //
    if (SDKREG_GetAppType(&nAppType, uKeyIndex) == S_OK)
    {
        EDeviceTouchType  eUSBKeyTouchType = E_DEVICE_PEN_TOUCH_WHITEBOARD;
        EPalmTouchControlType ePalmTouchControlType = E_PLAM_TOUCHCONTROL_P0;
        EFingerTouchControlType  eFingerTouchControlType = E_FINGER_TOUCHCONTROL_F0;

        switch (nAppType & 0x0000FF)
        {
        case 0:
            eUSBKeyTouchType = E_DEVICE_PEN_TOUCH_WHITEBOARD;
            break;
        case 1:
            eUSBKeyTouchType = E_DEVICE_FINGER_TOUCH_WHITEBOARD;
            break;
        case 2:
            eUSBKeyTouchType = E_DEVICE_FINGER_TOUCH_CONTROL;
            nFingerContorlType = SDKREG_GetParamType(uKeyIndex);
            switch (nFingerContorlType)
            {
            case 0:
                eFingerTouchControlType = E_FINGER_TOUCHCONTROL_F0;
                break;
            case 1:
                eFingerTouchControlType = E_FINGER_TOUCHCONTROL_F1;
                break;
            case 2:
                eFingerTouchControlType = E_FINGER_TOUCHCONTROL_F2;
                break;
            case 3:
                eFingerTouchControlType = E_FINGER_TOUCHCONTROL_F3;
                break;
            case 4:
                eFingerTouchControlType = E_FINGER_TOUCHCONTROL_F4;
                break;
            case 5:
                eFingerTouchControlType = E_FINGER_TOUCHCONTROL_F5;
                break;
            default:
                break;
            }

            usbKeyInformation.eFingerTouchControlType = eFingerTouchControlType;

            //if (eFingerTouchControlType > m_eFingerTouchControlType)
            //{
            //    m_eFingerTouchControlType = eFingerTouchControlType;
            //}

            break;
        case 3:
            eUSBKeyTouchType = E_DEVICE_PALM_TOUCH_CONTROL;
            nPalmType = SDKREG_GetParamType(uKeyIndex);
            switch (nPalmType)
            {
            case 0:
                ePalmTouchControlType = E_PLAM_TOUCHCONTROL_P0;
                break;
            case 1:
                ePalmTouchControlType = E_PLAM_TOUCHCONTROL_P1;
                break;
            case 2:
                ePalmTouchControlType = E_PLAM_TOUCHCONTROL_P2;
                break;
            case 3:
                ePalmTouchControlType = E_PLAM_TOUCHCONTROL_P3;
                break;
            case 4:
                ePalmTouchControlType = E_PLAM_TOUCHCONTROL_P4;
                break;
            case 5:
                ePalmTouchControlType = E_PLAM_TOUCHCONTROL_P5;
                break;
            case 6:
                ePalmTouchControlType = E_PLAM_TOUCHCONTROL_T0;
                break;
            case 7:
                ePalmTouchControlType = E_PLAM_TOUCHCONTROL_T1;
                break;
            case 8:
                ePalmTouchControlType = E_PLAM_TOUCHCONTROL_T2;
                break;
            case 9:
                ePalmTouchControlType = E_PLAM_TOUCHCONTROL_T3;
                break;
            case 10:
                ePalmTouchControlType = E_PLAM_TOUCHCONTROL_T4;
                break;
            case 11:
                ePalmTouchControlType = E_PLAM_TOUCHCONTROL_T5;
                break;
            case 12:
                ePalmTouchControlType = E_PLAM_TOUCHCONTROL_TX1;
                break;
            case 13:
                ePalmTouchControlType = E_PLAM_TOUCHCONTROL_TX2;
                break;
            default:
                break;
            }

            //nUSBKeyTouchCount++;
            // m_VecAllUsbKeyTouchType[uKeyIndex].ePalmTouchControlType = ePalmTouchControlType;
            usbKeyInformation.ePalmTouchControlType = ePalmTouchControlType;
            //if (ePalmTouchControlType > m_ePalmTouchControlType)
            //{
            //    m_ePalmTouchControlType = ePalmTouchControlType;
            //}
            break;

        default:
            break;
        }//switch

         //m_VecAllUsbKeyTouchType[uKeyIndex].eUSBKeyTouchType = eUSBKeyTouchType;
        usbKeyInformation.eUSBKeyTouchType = eUSBKeyTouchType;

        //if (eUSBKeyTouchType > m_eUSBKeyTouchType)
        //{
        //    m_eUSBKeyTouchType = eUSBKeyTouchType;
        //}
    }

    BOOL bMultipleScreenMode = (nAppType >> 8) & 0x00000001;

    if (bMultipleScreenMode)
    {//多屏屏接模式
        EScreenMode eScreenMode = EScreenModeDouble;

        //获得多屏融合类型
        UINT dwScreenMergeType = SDKREG_GetMultiScreenMergeType(uKeyIndex);

        switch (dwScreenMergeType & 0x00000001F)
        {
        case 1://双屏拼接
            eScreenMode = EScreenModeDouble;
            break;

        case 2://三屏拼接
            eScreenMode = EScreenModeTriple;
            break;

        case 4://四屏拼接
            eScreenMode = EScreenModeQuad;
            break;

        case 8://五屏拼接
            eScreenMode = EScreenModeQuint;
            break;

        case 0x10://六屏拼接
            eScreenMode = EScreenModeHexa;
            break;

        }//switch
         //m_VecAllUsbKeyTouchType[uKeyIndex].eScreenModeFromUsbKey = eScreenMode;
         //选择最多的凭借模式
        //if (eScreenMode > m_eScreenModeFromUsbKey) m_eScreenModeFromUsbKey = eScreenMode;

        usbKeyInformation.eScreenModeFromUsbKey = eScreenMode;
    }

    usbKeyInformation.bEnableFourPointCalibrate = (nAppType >> 11) & 0x00000001;

    std::string map_key = strDevPath;
    m_AllUSbKeyInformations[map_key] = usbKeyInformation;

    return TRUE;
}


void CIWBApp::OnPlugOutUSBKey(const char* szPath)
{
    char szDevPath[MAX_PATH];
    strcpy_s(szDevPath, _countof(szDevPath), szPath);
    _strlwr_s(szDevPath, strlen(szDevPath) + 1);

    auto it= m_AllUSbKeyInformations.find(szDevPath);

    if (it != m_AllUSbKeyInformations.end())
    {
        m_AllUSbKeyInformations.erase(it);

        if (m_uHardwareUSBKeyCount > 0)
        {
            m_uHardwareUSBKeyCount--;
        }
    }
}

void CIWBApp::OnPlugInUSBKey(const char* szPath)
{
    char szDevPath[MAX_PATH];
    strcpy_s(szDevPath, _countof(szDevPath), szPath);
    _strlwr_s(szDevPath, strlen(szDevPath) + 1);

    UINT uKeyNum = SDKREG_GetUSBKeyCount();

    for (UINT uKeyIndex = 0; uKeyIndex < uKeyNum; uKeyIndex++)
    {
        char szUsbKeyDevPath[MAX_PATH];

        HRESULT hr = SDKREG_GetUSBKeyDevPath(uKeyIndex, szUsbKeyDevPath, _countof(szUsbKeyDevPath));
        if (FAILED(hr)) continue;
        _strlwr_s(szUsbKeyDevPath, strlen(szUsbKeyDevPath) + 1);

        if (strcmp(szDevPath, szUsbKeyDevPath) == 0)
        {
            if (ReadUSBKeyData(uKeyIndex))
            {
                //m_bFoundHardwareUSBKey = TRUE;
                m_uHardwareUSBKeyCount++;
            }
            break;
        }
    }

}


int CIWBApp::GetScreenCount() const
{
    int nScreenCount = int(GetScreenMode()) + 1;
    if (nScreenCount < 1) nScreenCount = 1;
    return nScreenCount;
}

int CIWBApp::GetScreenModeFromUSBKeyCount()const
{
    return int(GetScreenModeFromUSBKey()) + 1;
}


EScreenMode CIWBApp::GetScreenModeFromUSBKey()const
{

    EScreenMode eScreenMode = EScreenModeSingle;

    for (auto it = m_AllUSbKeyInformations.begin(); it != m_AllUSbKeyInformations.end(); it++)
    {
        if (it->second.eScreenModeFromUsbKey > eScreenMode)
        {
            eScreenMode = it->second.eScreenModeFromUsbKey;
        }

    }

    if (GetUSBKeyTouchType() == E_DEVICE_PALM_TOUCH_CONTROL && GetPalmTouchType() == E_PLAM_TOUCHCONTROL_P1)
    {
        eScreenMode = EScreenModeSingle;
    }
    else if (GetUSBKeyTouchType() == E_DEVICE_PALM_TOUCH_CONTROL && eScreenMode == EScreenModeSingle)
    {

        CUsbCameraDeviceList   usbCameraList;//视频设备列表
        usbCameraList.UpdateDeviceList();
        int nSersorcount = usbCameraList.GetDeviceInstanceCount();
        switch (nSersorcount)
        {
        case 2:
            eScreenMode = EScreenModeDouble;
            break;
        case 3:
            eScreenMode = EScreenModeTriple;
            break;
        case 4:
            eScreenMode = EScreenModeQuad;
            break;
        case 5:
            eScreenMode = EScreenModeQuint;
            break;
        case 6:
            eScreenMode = EScreenModeHexa;
            break;
        }
    }

    //<<xuke
    eScreenMode = EScreenModeHexa;
    //xuke>>
    return eScreenMode;
}

const TAutoCalibrateCompensateData* CIWBApp::GetCompensateData(const char* strDevPath)const
{
    std::unordered_map<std::string, TAutoCalibrateCompensateData>::const_iterator iter;
    iter = m_allCompensateCoefs.find(strDevPath);
    if (iter != m_allCompensateCoefs.end())
    {
        return &iter->second;
    }

    return NULL;
}

void CIWBApp::ResetCompensateData()
{
    m_allCompensateCoefs.clear();
}

//@功能:获取所有自动补偿校正系数
void CIWBApp::GetAllCompensateData(std::vector<TAutoCalibrateCompensateData>& allCompensateData)
{
    allCompensateData.resize(m_allCompensateCoefs.size());

    std::unordered_map<std::string, TAutoCalibrateCompensateData>::const_iterator iter;

    uint32_t index = 0;

    iter = m_allCompensateCoefs.begin();
    while (iter != m_allCompensateCoefs.end())
    {
        allCompensateData[index] = iter->second;
        index++;
        iter++;
    }

}

