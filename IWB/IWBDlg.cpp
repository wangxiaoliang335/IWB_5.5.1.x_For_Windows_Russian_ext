// OpticalPenDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "headers.h"
#include "IWBDlg.h"
#include "WGRConfigDlg.h"
#include "GLBGRSettingDlg.h"
#include "DebugVideoLoaderDlg.h"
#include "../inc/MyApi.h"
#include "OnlineRegisterDlg.h"
#include "UpdateFirmwareDlg.h"
#include <hidsdi.h>

#include "DlgScreenLayoutSettings.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif



void DbgCursor(HCURSOR hCursor)
{//debug
    ICONINFO iconInfo;
    BOOL bRet = FALSE;
    bRet = GetIconInfo(hCursor, &iconInfo);

    BITMAP bitmapMask;
    BITMAP bitmapColor;
    if (bRet)
    {
        GetObject(iconInfo.hbmMask, sizeof(bitmapMask), &bitmapMask);

        GetObject(iconInfo.hbmColor, sizeof(bitmapColor), &bitmapColor);
    }

    DWORD pixel = 0x00000000;
    if (bitmapMask.bmBits)
    {
        pixel = *((DWORD*)bitmapMask.bmBits + (bitmapMask.bmHeight >> 1) * bitmapMask.bmWidth + (bitmapMask.bmWidth >> 1));
    }

    if (bitmapColor.bmBits)
    {
        pixel = *((DWORD*)bitmapColor.bmBits + (bitmapColor.bmHeight >> 1) * bitmapColor.bmWidth + (bitmapColor.bmWidth >> 1));
    }

}


const TCHAR* GetAppName()
{
    static TCHAR szModuleFileName[MAX_PATH];
    DWORD dwRet = GetModuleFileName(theApp.m_hInstance, szModuleFileName, _countof(szModuleFileName));

    if (dwRet == 0)
    {
        LOG_ERR("GetModuleFileName failed 0x%x", dwRet);
        return _T("IWB");
    }


    PathRemoveExtension(szModuleFileName);
    const TCHAR* lpszAppName = PathFindFileName(szModuleFileName);

    return lpszAppName;
}

//#include "../inc/HotkeyAPI.h"

//#define TIMER_OPEN_CLOSE_TEST 1
//#define TIMER_AUTO_CLUTTER_DETECTION_AFTER_AUTO_CALIBRATION 2
// CAboutDlg dialog used for App About
//@功能:获取自动运行菜单位置
//@参数:bAllUser, 为TRUE时返回所有用户的启动菜单路径
//                为FALSE时,返回当前用户的启动菜单路径
//
CString GetStarupDirPath(BOOL bAllUser = FALSE)
{
    CString strShortcutPath;

    TCHAR szStartUpFolder[MAX_PATH];
    HRESULT hr =
        SHGetFolderPath (
        NULL,
        bAllUser?CSIDL_COMMON_STARTUP:CSIDL_STARTUP,// Start Menu\Programs\Startup CSIDL_STARTMENU,//CSIDL_COMMON_STARTMENU,
        NULL,
        SHGFP_TYPE_CURRENT,
        szStartUpFolder);

    if(hr != S_OK)
    {
        return _T("");
    }
    //strShortcutPath.Format(_T("%s\\%s.lnk"), szStartUpFolder, g_oResStr[IDS_STRING127]);
    strShortcutPath.Format(_T("%s\\%s.lnk"), szStartUpFolder, GetAppName());
    return strShortcutPath;
}


//https://msdn.microsoft.com/en-us/library/windows/desktop/bb762494(v=vs.85).aspx
//CSIDL_COMMON_ALTSTARTUP
//FOLDERID_CommonStartup
//The file system directory that corresponds to the nonlocalized Startup program group for all users. 
//This value is recognized in Windows Vista for backward compatibility, but the folder itself no longer exists.
//Win7:C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Startup
//WinXP:SHGetFolderPath failed

//CSIDL_COMMON_STARTUP
//The file system directory that contains the programs that appear in the Startup folder for all users. 
//A typical path is C:\Documents and Settings\All Users\Start Menu\Programs\Startup.
//Win7 :C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Startup
//WinXP:C:\Documents and Settings\All Users\「开始」菜单\程序\启动

CString GetAutoStarupLnkPathCommon()
{
    CString strShortcutPath;

    TCHAR szStartUpFolder[MAX_PATH];
    HRESULT hr =
        SHGetFolderPath (
        NULL,
        CSIDL_COMMON_STARTUP,//CSIDL_COMMON_ALTSTARTUP,// Start Menu\Programs\Startup CSIDL_STARTMENU,//CSIDL_COMMON_STARTMENU,
        NULL,
        SHGFP_TYPE_CURRENT,
        szStartUpFolder);

    if(hr != S_OK)
    {
        return _T("");
    }

    //strShortcutPath.Format(_T("%s\\%s.lnk"), szStartUpFolder,g_oResStr[IDS_STRING127]);	
    strShortcutPath.Format(_T("%s\\%s.lnk"), szStartUpFolder, GetAppName());
    return strShortcutPath;
}


//@功能:根据指定的Locale Identifier从对应语言的资源库中提取
//      自动启动快捷方式的路径
CString GetStartupLnkPath(LCID lcid)
{
    CString strShortcutPath;

    TCHAR szStartUpFolder[MAX_PATH];
    HRESULT hr =
        SHGetFolderPath (
        NULL,
        CSIDL_STARTUP,// Start Menu\Programs\Startup CSIDL_STARTMENU,//CSIDL_COMMON_STARTMENU,
        NULL,
        SHGFP_TYPE_CURRENT,
        szStartUpFolder);

    if(hr != S_OK)
    {
        return _T("");
    }

    TCHAR szLangDLL[_MAX_PATH+14];

    TCHAR szLangCode[4];
    TCHAR szFormat[MAX_PATH];
    _tcscpy_s(szFormat,_countof(szFormat), _T("%s_Res.dll"));

    int  nResult = ::GetLocaleInfo(lcid, LOCALE_SABBREVLANGNAME, szLangCode, _countof(szLangCode));
    if(nResult)
    {

        int nRet = _sntprintf_s(szLangDLL, _countof(szLangDLL), _countof(szLangDLL) - 1, szFormat,szLangCode);

        if(nRet != -1 && nRet <= sizeof(szLangDLL))
        {
            CResStr resStr;
            HMODULE hRes = LoadLibrary(szLangDLL);
            if(hRes)
            {
                resStr.SetResInst(hRes);
                //strShortcutPath.Format(_T("%s\\%s.lnk"), szStartUpFolder, resStr[IDS_STRING127]);
                strShortcutPath.Format(_T("%s\\%s.lnk"), szStartUpFolder, GetAppName());
                
                FreeLibrary(hRes);
            }
        }
    }

    return strShortcutPath;
}

//中文简体版版自动启动快捷方式
CString GetAutoStarupLnkPathCHS()
{
    LCID lcid = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_DEFAULT);
    CString strShortcutPath = GetStartupLnkPath(lcid);
    return strShortcutPath;
}

//中文繁体版自动启动快捷方式
CString GetAutoStarupLnkPathCHT()
{
    LCID lcid = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL), SORT_DEFAULT);
    CString strShortcutPath = GetStartupLnkPath(lcid);
    return strShortcutPath;
}

//韩语版自动启动快捷方式
CString GetAutoStarupLnkPathKOR()
{
    LCID lcid = MAKELCID(MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN),  SORT_DEFAULT);
    CString strShortcutPath = GetStartupLnkPath(lcid);
    return strShortcutPath;
}

CString GetAutoStarupLnkPathESP()
{
    LCID lcid = MAKELCID(MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH),  SORT_DEFAULT);
    CString strShortcutPath = GetStartupLnkPath(lcid);
    return strShortcutPath;
}

CString GetAutoStarupLnkPathDEU()
{
    LCID lcid = MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN),  SORT_DEFAULT);
    CString strShortcutPath = GetStartupLnkPath(lcid);
    return strShortcutPath;
}

CString GetAutoStarupLnkPathRUS()
{
    LCID lcid = MAKELCID(MAKELANGID(LANG_RUSSIAN, SUBLANG_RUSSIAN_RUSSIA),  SORT_DEFAULT);
    CString strShortcutPath = GetStartupLnkPath(lcid);
    return strShortcutPath;
}


CString GetAutoStarupLnkPathPTG()
{
    LCID lcid = MAKELCID(MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE),  SORT_DEFAULT);
    CString strShortcutPath = GetStartupLnkPath(lcid);
    return strShortcutPath;
}


CString GetAutoStarupLnkPathFRA()
{
    LCID lcid = MAKELCID(MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH),  SORT_DEFAULT);
    CString strShortcutPath = GetStartupLnkPath(lcid);
    return strShortcutPath;
}


CString GetAutoStarupLnkPathITA()
{
    LCID lcid = MAKELCID(MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN),  SORT_DEFAULT);
    CString strShortcutPath = GetStartupLnkPath(lcid);
    return strShortcutPath;
}

//英文版自动启动快捷方式
CString GetAutoStarupLnkPathEN()
{
    CString strShortcutPath;

    TCHAR szStartUpFolder[MAX_PATH];
    HRESULT hr =
        SHGetFolderPath (
        NULL,
        CSIDL_STARTUP,// Start Menu\Programs\Startup CSIDL_STARTMENU,//CSIDL_COMMON_STARTMENU,
        NULL,
        SHGFP_TYPE_CURRENT,
        szStartUpFolder);

    if(hr != S_OK)
    {
        return _T("");
    }

    CResStr resStr;
    resStr.SetResInst(theApp.m_hInstance);
    //strShortcutPath.Format(_T("%s\\%s.lnk"), szStartUpFolder,resStr[IDS_STRING127]);
    strShortcutPath.Format(_T("%s\\%s.lnk"), szStartUpFolder, GetAppName());
    
    return strShortcutPath;
}

class CAboutDlg: public CDialog
{
public:
    CAboutDlg();
    // Dialog Data
#ifdef NO_COMPANY_INFORMATION //中性版本
    enum{ IDD = IDD_ABOUTBOX_NEUTRAL };
#else
    enum{ IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
    virtual BOOL OnInitDialog();

    // Implementation
protected:
    DECLARE_MESSAGE_MAP()

public:
    CHyperLink  m_cHyperHttp;
};

CAboutDlg::CAboutDlg(): CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

#ifndef NO_COMPANY_INFORMATION //非中性版本
    DDX_Control(pDX, IDC_LINKURL, m_cHyperHttp);
#endif
}

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
#ifndef NO_COMPANY_INFORMATION //非中性版本
    CString cStrUrl = _T("www.easitech.com.cn");
    m_cHyperHttp.GetWindowText(cStrUrl)/*_T("www.zzvcom.com.cn")*/;

    if (cStrUrl.IsEmpty())
    {
        cStrUrl = _T("www.easitech.com.cn");
    }

    m_cHyperHttp.SetURL(cStrUrl);
    m_cHyperHttp.SetUnderline(TRUE);
#endif
    return TRUE;
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

const int StatusPaneCountEachSensor = 3;
const int PANE_FPS    = 0;
const int PANE_STATE  = 1;
const int PANE_DONGLE = 2;
const int PANE_STATE2 = 3;
const int PANE_STATE3 = 4;
const int PANE_STATE4 = 5;
const int PANE_STATE5 = 6;
const int PANE_STATE6 = 7;

static UINT indicators[] ={
    IDS_STRING438,
    IDS_STRING439,
    IDS_STRING440,
    IDS_STRING439,
    IDS_STRING439,
    IDS_STRING439,
    IDS_STRING439,
    IDS_STRING439,
};


//静态变量
//3CIWBSensorManager CIWBDlg::m_oIWBSensorManager;
//任务栏重新生成事件消息
UINT CIWBDlg::m_uTaskbarRestartMsg = 0;
UINT CIWBDlg::m_uAppCommMsg = 0U;
// CIWBDlg dialog
#define  DEFAULT_CALIBRATE_PATTERN E_CALIBRATE_PATTERN_5_COLS
CIWBDlg::CIWBDlg(CWnd* pParent /*=NULL*/)
: CDialog(CIWBDlg::IDD, pParent),
m_bAllWndCreated(FALSE),
//m_bEnableOpticalPenControl(FALSE),
m_pUSBDevDetector(NULL),
m_bVisible(TRUE),
m_uOpenCloseTestTimer(0),
m_hArrowCursor(NULL),
m_bScreenMaskAreaSelectDragging(FALSE),
m_eScreenMaskAreaEditMode(e_SCREEN_MASK_AREA_EDIT_MODE_IDLE),
m_eManualCalibrateMode(e_MANUALCALIBRATE_MODE_5),
m_nRawInputBufSize(0),
m_pRawInputBuf(NULL),
m_AutoCalibrateColsType(DEFAULT_CALIBRATE_PATTERN),
m_bIsRecordVideo(FALSE),
m_bIsOpenNoProjectorMode(FALSE),
m_bIsRearProjection(FALSE),
m_pSelectedSensor(NULL),
m_uScreenRecognitionCloseTimer(0u),
m_hDispWnd(NULL),
m_pCurInstalledSensor(NULL),
m_hUCShieldBitmap(NULL),
m_bStartDrawOnlineScreenArea(false),
m_bPreGuideRectangleVisible(false),
m_pUSBDevDetector_HID(NULL),
m_nDrawOnlineAreaCount(-1),
m_nActiveDetectCameraId(0)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hScreenMaskAreaSelectCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_DRAW_SCREEN_AREA);
    m_hArrowCursor = ::LoadCursor(NULL, IDC_ARROW);
    m_hHandCursor  = ::theApp.LoadCursor(IDC_CURSOR_HAND);

    m_EraseSize.cx = 16; m_EraseSize.cy = 16;
    m_FillSize.cx = 16; m_FillSize.cy = 16;
    m_hCurrentEditToolCursor = m_hArrowCursor;

     SHSTOCKICONINFO shsii;
     shsii.cbSize = sizeof(SHSTOCKICONINFO);
     HRESULT hr =  SHGetStockIconInfo(
                        SIID_SHIELD,
                        SHGSI_ICON | SHGSI_SMALLICON,
                        &shsii);

     if(SUCCEEDED(hr))
     {
        ICONINFO ii;
        GetIconInfo(shsii.hIcon, &ii);

        //ReplaceDIBColor(m_shieldIconInfo.hbmColor, RGB(0,0,0), GetSysColor(COLOR_MENU));
        //用菜单背景色替换图标的背景色, 为了让图标看起来是透明的。
        m_hUCShieldBitmap = ReplaceColor(ii.hbmColor, RGB(0,0,0), GetSysColor(COLOR_MENU), NULL);
        DeleteObject(ii.hbmMask);
        DeleteObject(ii.hbmColor);
     }

    //m_nAutoMaskDetectThreshold = PROFILE::GetAutoMaskingTheshold();
    
    //客户app通信消息
    m_uAppCommMsg = RegisterWindowMessage(_T("EASI_8701715C-217D-440d-9404-F63C9CBC671B"));

}

void CIWBDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    //DDX_Control(pDX, IDC_STATIC_VIDEO_AREA, m_PlayWnd);
}

BEGIN_MESSAGE_MAP(CIWBDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_MESSAGE(TRAY_NOTIFY_MSG, OnTrayNotifyMsg)
    //}}AFX_MSG_MAP
    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_MESSAGE(WM_CLOSE, OnClose)
    //ON_MESSAGE(WM_CALC_ADJUST_MATRIX, OnCalcAdjustMatrix)
    ON_MESSAGE(WM_FINISH_MAUNUAL_CALIBRATING, OnManualCalibrationDone)
    //ON_MESSAGE(WM_ABORT_MANUAL_CALIBRATING, OnAbortManualCalibrating)

    ON_COMMAND(ID_CTXMENU_EXIT, &CIWBDlg::OnMenuExit)
    ON_COMMAND(ID_MENU_EXIT, &CIWBDlg:: OnMenuExit)
    ON_COMMAND(ID_CTXMENU_SHOW_MAIN_WINDOW, &CIWBDlg::OnCtxmenuShowMainWindow)
    ON_COMMAND(ID_CTXMENU_AUTOCALIBRATE, &CIWBDlg::OnCtxmenuAutoCalibrate)
    //ON_BN_CLICKED(IDC_BUTTON_COORD_ADJUST, &CIWBDlg::OnBnClickedButtonCoordAdjust)

    ON_MESSAGE(WM_FPSNOTIFY, OnFpsNotify)
    ON_MESSAGE(WM_CAMERA_STATUS_NOTIFY, OnCameraStatusNotify)

    ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
    ON_COMMAND(ID_MENU_PARAMETER_SETTINGS, &CIWBDlg::OnMenuParameterSettings)
    ON_COMMAND(ID_MENU_RUN, &CIWBDlg::OnMenuRun)
    ON_COMMAND(ID_MENU_STOP, &CIWBDlg::OnMenuStop)
    ON_COMMAND(ID_MENU_MANUAL_CALIBRATE, &CIWBDlg::OnMenuManualCalibrate)

    ON_MESSAGE(WM_SET_DETECT_THRESHOLD, OnSetDetectThreshold)
    ON_MESSAGE(WM_DEVICECHANGE, OnDeviceChange)

    ON_MESSAGE(WM_BREAK_COLLECTSPOT,OnBreakSpotSetting)
    ON_MESSAGE(WM_FINISH_COLLECTSPOT,OnFinshSpotSetting)
    // ON_MESSAGE(WM_SET_DETECT_BOLB,OnSetDetectSpot)


    ON_WM_ENTERIDLE()
    ON_COMMAND(ID_MENU_ABOUT, &CIWBDlg::OnMenuAbout)
    ON_COMMAND(ID_MENU_ONLINE_REGISTER, &CIWBDlg::OnMenuOnlineRegister)

    ON_COMMAND(ID_MENU_STATUS, &CIWBDlg::OnMenuStatus)

    ON_COMMAND(ID_MENU_DISABLE_OPTICAL_PEN_CONTROL, &CIWBDlg::OnMenuDisableOpticalPenControl)
    //ON_COMMAND(ID_OPEN_CLOSE_TEST, &CIWBDlg::OnOpenCloseTest)
    ON_WM_TIMER()
    ON_COMMAND(ID_PROPERY_PAGE, &CIWBDlg::OnProperyPage)
    ON_COMMAND(ID_LANG_SELECT, &CIWBDlg::OnLangSelect)

    ON_MESSAGE(WM_AUTO_CALIBRATE_DONE, &CIWBDlg::OnAutoCalibrateDone)
    ON_COMMAND(ID_CTXMENU_AUTORUN_AT_SYSTEM_STARTUP, &CIWBDlg::OnCtxmenuAutorunAtSystemStartup)
    ON_WM_SHOWWINDOW()
    ON_WM_WINDOWPOSCHANGING()
    ON_WM_INITMENUPOPUP()
    ON_WM_CONTEXTMENU()
    //ON_COMMAND(ID_SCREEN_AREA_SPECIFY, &CIWBDlg::OnMaskAreaSpecify)
    //ON_COMMAND(ID_DISABLE_MANUAL_SREEN_AREA, &CIWBDlg::OnDisableManualSreenArea)
    ON_WM_CAPTURECHANGED()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_SETCURSOR()
    ON_WM_KEYDOWN()

    //ON_COMMAND_RANGE(ID_AUTOCALIBRATION_13X13GRIDS,ID_AUTOCALIBRATION_17X17GRIDS, &CIWBDlg::OnMenuAutoCalibrate)
    ON_COMMAND(ID_MENU_AUTO_CALIBRATE, &CIWBDlg::OnStartMenuAutoCalibrate)
    ON_COMMAND_RANGE(ID_ADD_MASK_RECTANGLE_1X, ID_ADD_MASK_RECTANGLE_3X, &CIWBDlg::OnAddMaskArea)
    ON_COMMAND_RANGE(ID_ERASE_MASK_RECTANGLE_1X, ID_ERASE_MASK_RECTANGLE_3X, &CIWBDlg::OnEraseMaskArea)

    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()
    ON_COMMAND(ID_MANUALMASKAREA_ENDEDITING, &CIWBDlg::OnEndManualMaskAreaEdit)
    ON_COMMAND(ID_CLEAR_MASK_AREA, &CIWBDlg::OnClearMaskArea)
    ON_WM_DESTROY()
    ON_MESSAGE(WM_HOTKEY, OnHotKey)
    ON_COMMAND(ID_MASK_AREA_EDIT_ACTION_REDO, &CIWBDlg::OnMaskAreaEditActionRedo)
    ON_COMMAND(ID_MASK_AREA_EDIT_ACTION_UNDO, &CIWBDlg::OnMaskAreaEditActionUndo)
    ON_MESSAGE(WM_KICKIDLE, OnKickIdle)

    ON_WM_INPUT()
    ON_COMMAND(ID_AUTO_ADD_MASK_AREA, &CIWBDlg::OnMenuAutoAddScreenMask)
    //ON_MESSAGE(CLUTTER_DETECTION_DONE_MSG, OnClutterDetectionDone)

    ON_MESSAGE(WM_INITIAL_STATGE_DISABLE_OPTICAL_PEN_FOR_CLUTTER, OnInitialStateDisableOpticalPenForClutter)
    ON_WM_NCDESTROY()

    ON_MESSAGE(WM_STATGE_DISABLE_OPTICAL_PEN_FOR_CLUTTER, OnStateDisableOpticalPenForClutter)

    ON_MESSAGE(WM_CHANGE_GUIDELINE_Y_POS, OnChangeGuidelineYPos)
    ON_MESSAGE(WM_SHOW_GUIDELINE, OnShowGuideline)
    ON_MESSAGE(WM_MANUAL_COLLECT_SPOT, OnSpotCollectionSetting_Par)
    ON_MESSAGE(WM_RECORD_VIDEO, OnRecordvideo)
    ON_MESSAGE(WM_CHANGE_DISPLAY_WINDOW, OnChangeDisplayWindow)

    ON_MESSAGE(WM_PUT_DETECT_BACKSPLASH_DATA, OnProcessDetectBackSplashData)
    ON_MESSAGE(WM_START_DETECT_BACKSPLASH_VANISH, OnStartDetectBackSplashVanished)
    ON_MESSAGE(WM_STOP_DETECT_BACKSPLASH_VANISH, OnStopDetectBackSplashVanished)

    ON_MESSAGE(WM_ENABLEORDISABLE_WINDOW, OnEnableOrDisableTheWindow)

    ON_COMMAND(ID_MENU_COLLECT_BOLBSETTING, &CIWBDlg::OnSpotCollectionSetting)
    ON_COMMAND(ID_MENU_SIMULATION_MANUAL_CALIBRATION, &CIWBDlg::OnSimulationManualCalibration)
    ON_COMMAND(ID_BOLB_CALIBRATION_SETTING, &CIWBDlg::OnSpotCalibrationSetting)
    //ON_COMMAND(ID_AUTO_SEARCH_SCREEN_AREA, &CIWBDlg::OnAutoSearchScreenArea)

    ON_MESSAGE(WM_SEARCH_SCREEN_AREA_DONE, OnSearchScreenAreaDone)
    ON_MESSAGE(WM_CHANGE_SPOTPROPORTION, OnChangeSpotProportion)
    ON_MESSAGE(WM_CHANGE_MULTERASER, OnChangeMultEraser)
    ON_MESSAGE(WM_CHANGE_FIXEDBOLBSETTIME, OnChangeFixedbolbsettime)
    ON_MESSAGE(WM_CHANGE_NORMALUSER_BRIGHTNESS, OnChangeNormalUserBrightness)
    ON_MESSAGE(WM_CHANGE_AUTOMASKDETECTTHRESHOLD, OnChangeAutoMaskDetectThreshold)
    ON_MESSAGE(WM_CHANGE_AUTOCALIBRIATION_AVERAGE_BRIGHTNESS, OnChangeAutoCalibrateAveBrightness)
    ON_MESSAGE(WM_CHANGE_AUTOCALIBRIATION_LIGHTGRAY, OnChangeAutoCalibrateLightGray)
    ON_MESSAGE(WM_REAR_PROJECTION, OnRearProjection)
    ON_MESSAGE(WM_CHANGE_VIDEODISPLAYDELAY,OnChangeVideoDisplayDelay)


//ON_COMMAND(ID_MENU_MANUAL_CORRECT_AFTER_AUTO_CALIRATION, &CIWBDlg::OnMenuAutoCalibrationWithHumanIntervention)
//ON_MESSAGE(WM_MANUAL_CORRECT_DONE, &CIWBDlg::OnManualCorrectDone)
//ON_MESSAGE(WM_DISPLAYWINDOW,&CIWBDlg::OnDisPlayWindow)


    //ON_MESSAGE(WM_CHANGE_ENABLE_GESTURE_RECOGNITION, OnChangeEnableGestrueRecognition)
    //ON_MESSAGE(WM_CHANGE_ENABLE_GESTURE_TOUCH, OnChangeEnableGestureTouch)
    //ON_MESSAGE(WM_CHANGE_ENABLE_PEN_TOUCH, OnChangeEnablePenTouch)

    ON_WM_QUERYENDSESSION()

    ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphNotify)

    ON_REGISTERED_MESSAGE(m_uTaskbarRestartMsg, OnTaskbarRestartMsg)
    ON_COMMAND(ID_WORKMODE_IMAGE_TUNNING, &CIWBDlg::OnVideoTuningMode)
    ON_COMMAND(ID_WORKMODE_NORMAL_USAGE, &CIWBDlg::OnNormalUsageMode)
    //	ON_COMMAND(ID_AUTO_MASK_CLUTTER_TEST, &CIWBDlg::OnAutoMaskClutterTest)
    ON_COMMAND(ID_CONTROL_ON, &CIWBDlg::OnControlOn)
    ON_COMMAND(ID_CONTROL_OFF, &CIWBDlg::OnControlOff)
    ON_COMMAND(ID_WORKMODE_LASER_TUNNING, &CIWBDlg::OnLaserTunningModel)
    //ON_COMMAND(ID_MENU_MANUAL_CALIBRATE25, &CIWBDlg::OnMenuManualCalibrate25)
    ON_COMMAND(ID_MENU_MANUAL_CALIBRATE, &CIWBDlg::OnMenuManualCalibrate)
    ON_COMMAND(ID_TRAYCTXMENU_MANUALCALIBRATION, &CIWBDlg::OnMenuManualCalibrate25)
    //ON_COMMAND(ID_MENU_MANUAL_CALIBRATE36, &CIWBDlg::OnMenuManualCalibrate36)
    ON_WM_NCLBUTTONDBLCLK()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDBLCLK()
  //  ON_COMMAND_RANGE(ID_SENSORCTXMENU_RUN, ID_SENSORCTXMENU_INSTALL_TIP, OnSensorCtxMenu)
  //  ON_COMMAND_RANGE(ID_SENSORCTXMENU_RUN,ID_SENSORCTXMENU_FOURPOINTCALIBRATION, OnSensorCtxMenu)
      ON_COMMAND_RANGE(ID_SENSORCTXMENU_RUN, ID_SENSORCTXMENU_DRAWMASKFRAME_CLEAR, OnSensorCtxMenu)

    //ON_COMMAND_RANGE(ID_GUESTURESETTINGS_GLBOARDGESTURESETTINGS, ID_GUESTURESETTINGS_WINDOWSGESTURESETTINGS, OnGestureSettingMenu)

    ON_COMMAND(ID_OPERATION_LIGHTSPOTSAMPLING, &CIWBDlg::OnOperationLightspotsampling)
    ON_COMMAND(ID_INSTALLATIONANDDEBUGGING_MOUSE, &CIWBDlg::OnInstallationanddebuggingMouse)
    ON_COMMAND(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, &CIWBDlg::OnInstallationanddebuggingTouchpad)

    ON_COMMAND_RANGE(ID_TOUCHSCREENASPECTRATIO_AUTO, ID_TOUCHSCREENASPECTRATIO_4_3, &CIWBDlg::OnChangeTouchScreenAspectRatio)
    ON_COMMAND(ID_SWAP_SENSOR_IMAGE, &CIWBDlg::OnSwapSensorImage)
    ON_COMMAND_RANGE(ID_SWAP_WITH_SENSOR0, ID_SWAP_WITH_SENSOR0 + (int)EScreenModeNumber, &CIWBDlg::OnSwapImageWithSensor)

    ON_REGISTERED_MESSAGE(m_uAppCommMsg, OnAppCommMsg)
    ON_COMMAND_RANGE(ID_SWTICH_SCREENMODE_ONE, ID_SWTICH_SCREENMODE_ONE + (int)EScreenModeNumber, &CIWBDlg::OnSwitchToFusionScreenMode)


    ON_MESSAGE(WM_APPLY_SENSOR_CONFIG, &CIWBDlg::OnApplySensorConfig)
    ON_COMMAND(ID_INSTALLATIONANDDEBUGGING_UPDATEFIRMWARE, &CIWBDlg::OnInstallationanddebuggingUpdatefirmware)
    ON_WM_ENDSESSION()
    ON_COMMAND(ID_INSTALLATIONANDDEBUGGING_ENABLEINTERPOLATE, &CIWBDlg::OnInstallationanddebuggingEnableinterpolate)
    ON_COMMAND(ID_MENU_ADVANCESSETTING, &CIWBDlg::OnMenuAdvancessetting)

    ON_COMMAND(ID_MENU_DRAWMASKFRAME_START, &CIWBDlg::OnMenuStartDrawOnlineScreenArea)
    ON_COMMAND(ID_MENU_DRAWMASKFRAME_CLEAR, &CIWBDlg::OnMenuClearDrawOnlineScreenArea)
    ON_COMMAND(ID_MENU_DRAWMASKFRAME_DISABLE, &CIWBDlg::OnMenuEnableDrawOnlineScreenArea)


    ON_COMMAND(ID_MENU_TOUCHSREEEN_LAYOUT_DESIGNER, &CIWBDlg::OnMenuTouchScreenLayoutDesigner)
    ON_MESSAGE(WM_CREEN_LAYOUT_DESIGN_BUTTON_CLICK, &CIWBDlg::OnScreenLayoutDesignBtnEvent)
    ON_MESSAGE(WM_END_4_BASE_POINT_CALIBRATE, &CIWBDlg::OnEnd4BasePointCalibrate)
    ON_WM_RBUTTONUP()
    ON_COMMAND(ID_MENU_FOURPOINTCALIBRATION, &CIWBDlg::OnOperationFourpointcalibration)

    ON_MESSAGE(WM_POWERBROADCAST, &CIWBDlg::OnPowerBroadcast)

    ON_REGISTERED_MESSAGE(theApp.GetBetweenInstanceMsg(), &CIWBDlg::OnBetweenInstanceMsg)
    ON_COMMAND(ID_MENU_CIRCLE_SCREEN_MANUAL_CALIBRATE, &CIWBDlg::OnMenuCircleScreenManualCalibrate)

    ON_COMMAND_RANGE(ID_SET_SENSOR_SCREEN_AREA_1, ID_SET_SENSOR_SCREEN_AREA_36, &CIWBDlg::OnChangeSensorAreaNo)
        
    END_MESSAGE_MAP()


void CIWBDlg::InitMenu()
{
    m_oMenu.DestroyMenu();

    m_oMenu.LoadMenu(IDR_MENU_MAIN);

    InsertParamSettingMenuItem(&m_oMenu, 3);

    if(NULL == m_oMenu.GetSafeHmenu()) return;

    if (theApp.GetScreenModeFromUSBKey() >= EScreenModeDouble)
    {
        AppendScreenModeSubmenu();
    }

    this->SetMenu(&m_oMenu);

    m_oOwnerDrawMenu.Attach(m_oMenu.GetSafeHmenu());
    m_oOwnerDrawMenu.SetMenuOwnerDrawBitmap(ID_ADD_MASK_RECTANGLE_1X, (HBITMAP)m_aryMenuBmp[e_BMP_BRUSH_1X], RGB(1, 0, 0));
    m_oOwnerDrawMenu.SetMenuOwnerDrawBitmap(ID_ADD_MASK_RECTANGLE_1D5X, (HBITMAP)m_aryMenuBmp[e_BMP_BRUSH_1D5X], RGB(1, 0, 0));
    m_oOwnerDrawMenu.SetMenuOwnerDrawBitmap(ID_ADD_MASK_RECTANGLE_2X, (HBITMAP)m_aryMenuBmp[e_BMP_BRUSH_2X], RGB(1, 0, 0));
    m_oOwnerDrawMenu.SetMenuOwnerDrawBitmap(ID_ADD_MASK_RECTANGLE_3X, (HBITMAP)m_aryMenuBmp[e_BMP_BRUSH_4X], RGB(1, 0, 0));

    m_oOwnerDrawMenu.SetMenuOwnerDrawBitmap(ID_ERASE_MASK_RECTANGLE_1X, (HBITMAP)m_aryMenuBmp[e_BMP_ERASE_1X], RGB(1, 0, 0));
    m_oOwnerDrawMenu.SetMenuOwnerDrawBitmap(ID_ERASE_MASK_RECTANGLE_1D5X, (HBITMAP)m_aryMenuBmp[e_BMP_ERASE_1D5X], RGB(1, 0, 0));
    m_oOwnerDrawMenu.SetMenuOwnerDrawBitmap(ID_ERASE_MASK_RECTANGLE_2X, (HBITMAP)m_aryMenuBmp[e_BMP_ERASE_2X], RGB(1, 0, 0));
    m_oOwnerDrawMenu.SetMenuOwnerDrawBitmap(ID_ERASE_MASK_RECTANGLE_3X, (HBITMAP)m_aryMenuBmp[e_BMP_ERASE_3X], RGB(1, 0, 0));

}

void CIWBDlg::AppendScreenModeSubmenu()
{
    CMenu* pInstallMenu = m_oMenu.GetSubMenu(1);

    if (pInstallMenu == NULL) return;

    MENUITEMINFO mnuiteminfo;
    memset(&mnuiteminfo, 0, sizeof(MENUITEMINFO));
    mnuiteminfo.cbSize = sizeof(MENUITEMINFO);
    mnuiteminfo.fMask = MIIM_FTYPE;
    mnuiteminfo.fType = MFT_SEPARATOR;

    //添加菜单分割条
    pInstallMenu->InsertMenuItem(pInstallMenu->GetMenuItemCount(), &mnuiteminfo, TRUE);

    //添加结束屏幕编辑菜单项
    mnuiteminfo.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
    mnuiteminfo.fType = MFT_STRING;
    mnuiteminfo.dwTypeData = const_cast<LPTSTR>(g_oResStr[IDS_STRING482]);
    mnuiteminfo.wID = ID_MENU_TOUCHSREEEN_LAYOUT_DESIGNER;
    pInstallMenu->InsertMenuItem(pInstallMenu->GetMenuItemCount(), &mnuiteminfo, TRUE);

    //添加屏幕融合模式子菜单
    m_oSubMenuMergMode.DestroyMenu();
    m_oSubMenuMergMode.CreatePopupMenu();


    mnuiteminfo.fMask = MIIM_SUBMENU | MIIM_ID | MIIM_STRING;
    mnuiteminfo.dwTypeData = const_cast<LPTSTR>(g_oResStr[IDS_STRING483]);
    mnuiteminfo.hSubMenu = m_oSubMenuMergMode.GetSafeHmenu();
    mnuiteminfo.wID = ID_SWTICH_SCREENMODE;
    pInstallMenu->InsertMenuItem(pInstallMenu->GetMenuItemCount(), &mnuiteminfo, TRUE);

    for (EScreenMode eScreenMode = EScreenModeSingle; eScreenMode <= theApp.GetScreenModeFromUSBKey(); eScreenMode = EScreenMode(eScreenMode + 1))
    {
        CString strText = const_cast<LPTSTR>(g_oResStr[IDS_STRING484 + eScreenMode]);

        mnuiteminfo.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
        mnuiteminfo.fType = MFT_STRING;
        mnuiteminfo.dwTypeData = const_cast<LPTSTR>(strText.GetString());
        mnuiteminfo.wID = ID_SWTICH_SCREENMODE_ONE + (int)eScreenMode;
        m_oSubMenuMergMode.InsertMenuItem(m_oSubMenuMergMode.GetMenuItemCount(), &mnuiteminfo, TRUE);
    }//for
}

// CIWBDlg message handlers

BOOL CIWBDlg::OnInitDialog()
{
    LOG_FUNC_CALL;
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);    

    //<<added by toxuke@gmail.com, 2013/05/06
    //任务栏重新生成事件消息,
    //当资源管理器崩溃后重新启动时，会发送该消息给所有的顶层窗体。
    //我们利用这个消息，将图标重新加入到托盘中。
    m_uTaskbarRestartMsg = RegisterWindowMessage(TEXT("TaskbarCreated"));
    //>>

    CMenu* pSysMenu = GetSystemMenu(FALSE);

    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
        BOOL bDebugModeTemp;
        bDebugModeTemp = g_tSysCfgData.globalSettings.bDebugMode;
        

        if(bDebugModeTemp)
        {//调试模式下，在系统菜单添加调试菜单项

            //检查校正过程
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_CHECK_AUTO_CALIBRATE_PROCESS, _T("Check Auto Calibrate Process..."));

            //查看校正符号
            pSysMenu->AppendMenu(MF_STRING, IDM_CHECK_CALIBRATE_SYMBOLS, _T("Show Calibrate Symbols On Screen"));

            //查看校正符号
            pSysMenu->AppendMenu(MF_STRING, IDM_CALIBRATE_MANUAL_RESAMPLE, _T("Calibrate Symbols Resample"));

            //从文件中校正
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_CALIBRATE_FORM_FILE, _T("Calibrate From File..."));

            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_DEBUG_MODE_RECORD_VIDEO, _T("Start Record"));

            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_DEBUG_MODE_READ_VIDEO, _T("Read From AVI File...."));

            //抓图功能
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_CAPTURE_PICTURE, _T("Capture Picture"));

            //GPIO功能
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_GPIO_ON, _T("GPIO On"));
            pSysMenu->AppendMenu(MF_STRING, IDM_GPIO_OFF, _T("GPIO Off"));

            //测试30个触控点
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_DEBUG_MODE_TEST30POINT, _T("Start Test 30 TouchPoint"));

        }
    }

    // Set the icon for this dialog. The framework does this automatically
    // when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE); // Set big icon
    SetIcon(m_hIcon, FALSE); // Set small icon

    m_ctlStatusBar.Create(this);

    m_ctlStatusBar.SetIndicators(indicators, _countof(indicators));
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);

    //
    //m_aryBrushBmp[E_BMP_SMALL].LoadBitmap(IDB_BITMAP_BRUSH_SMALL);
    // TODO: Add extra initialization here
    m_bAllWndCreated = TRUE;

    m_aryMenuBmp[e_BMP_BRUSH_1X ].LoadBitmap(IDB_BITMAP_BRUSH_1X);
    m_aryMenuBmp[e_BMP_BRUSH_1D5X ].LoadBitmap(IDB_BITMAP_BRUSH_2X);
    m_aryMenuBmp[e_BMP_BRUSH_2X ].LoadBitmap(IDB_BITMAP_BRUSH_3X);
    m_aryMenuBmp[e_BMP_BRUSH_4X ].LoadBitmap(IDB_BITMAP_BRUSH_4X);

    m_aryMenuBmp[e_BMP_ERASE_1X ].LoadBitmap(IDB_BITMAP_ERASER_1X);
    m_aryMenuBmp[e_BMP_ERASE_1D5X ].LoadBitmap(IDB_BITMAP_ERASER_2X);
    m_aryMenuBmp[e_BMP_ERASE_2X ].LoadBitmap(IDB_BITMAP_ERASER_3X);
    m_aryMenuBmp[e_BMP_ERASE_3X ].LoadBitmap(IDB_BITMAP_ERASER_4X);

    InitMenu();

    CreateOwnerCursor();

    m_mnuManualScreenAreaSettings.LoadMenu(IDR_MENU_MANUAL_SREEN_AREA_SETTINGS);

    CRect rcClient;
    GetClientRect(&rcClient);

    //托盘对象
    m_hAppIcon = ::LoadIcon(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
    m_oTray.SetWnd(m_hWnd);
    m_oTray.SetTrayID(this->GetDlgCtrlID());
    m_oTray.SetIcon(m_hAppIcon);
    m_oTray.SetCallBackMessage(TRAY_NOTIFY_MSG);
    m_oTray.SetBallonTip(_T("提示信息"),_T(""),1000*15,NIIF_INFO);

    //安装提示处理模块
    m_tipProcServer.Attached2TheWnd(m_hWnd);
    m_tipProcServer.SetSceenMode(theApp.GetScreenMode() == EScreenModeDouble? E_SCREEN_Double : E_SCREEN_Single);
    m_tipProcServer.SetCurrentLangCode(::g_tSysCfgData.globalSettings.langCode);

    //初始化
    //m_oIWBSensorManager.Init(theApp.GetScreenType() == EDoubleScreenMode?2:1);
    //根据注册的信息决定
    m_oIWBSensorManager.Init(theApp.GetScreenCount());

    //载入配置信息
    LoadConfig();

    m_oUSBCameraDeviceList.UpdateDeviceList();
    
    this->m_oIWBSensorManager.SetCfgData(g_tSysCfgData);

    //CIWBSensor对象分配摄像头设备路径
    m_oIWBSensorManager.AssignCamera(m_oUSBCameraDeviceList);

    this->m_oIWBSensorManager.SetGlobalCfgData(g_tSysCfgData);

    //通知各个模块更改屏幕物理尺寸和屏幕分辨率
    OnDisplayChangeHelper(::GetActualScreenControlSize());

    LoadResolutionConfig();
    

    //KSCATEGORY_VIDEO     :{6994AD05-93EF-11D0-A3CC-00A0C9223196}
    //KSCATEGORY_CAPTURE   :{65E8773D-8F56-11D0-A3B9-00A0C9223196}
    //AM_KSCATEGORY_CAPTURE:{65E8773D-8F56-11D0-A3B9-00A0C9223196}
    m_pUSBDevDetector = new CUSBDevDetector(KSCATEGORY_CAPTURE, this->GetSafeHwnd());

    GUID hidguid;
    HidD_GetHidGuid(&hidguid);
    m_pUSBDevDetector_HID = new CUSBDevDetector(hidguid, this->GetSafeHwnd());

    //Register RAW INPUT Device, added by toxuke@gmail.com, 2012/05/23
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 1;//Generic Desktop
    rid.usUsage     = 0x06;//Usage Name:keyboard
    rid.hwndTarget = this->GetSafeHwnd();//0;
    rid.dwFlags = RIDEV_INPUTSINK;

    RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE));

    BOOL bRet = FALSE;
    m_atomLaserPwrOffHotkey = GlobalAddAtom(_T("EasiOpticalPenLaserPowerOffKey"));
    bRet = RegisterHotKey(this->GetSafeHwnd(), m_atomLaserPwrOffHotkey, MOD_CONTROL, '8');

    m_atomLaserPwrOnHotkey = GlobalAddAtom(_T("EasiOpticalPenLaserPowerOnKey"));
    bRet = RegisterHotKey(this->GetSafeHwnd(), m_atomLaserPwrOnHotkey, MOD_CONTROL, '9');


    //m_atomTogglePenContolHotkey = GlobalAddAtom(_T("EasiSwitchPenControlHotkey"));
    //AddHotKey(this->GetSafeHwnd(), m_atomTogglePenContolHotkey, MOD_CONTROL | MOD_ALT, 'P');

    //bRet = RegisterHotKey(this->GetSafeHwnd(), m_atomTogglePenContolHotkey, MOD_CONTROL | MOD_ALT, 'P');

    //m_atomTogglePenContolHotkey_2 = GlobalAddAtom(_T("EasiSwitchPenControlHotkey_2"));
    //AddHotKey(this->GetSafeHwnd(), m_atomTogglePenContolHotkey_2, MOD_CONTROL | MOD_ALT, 'S');
    //bRet = RegisterHotKey(this->GetSafeHwnd(), m_atomTogglePenContolHotkey_2, MOD_CONTROL | MOD_ALT, 'S');

    m_bVisible = FALSE;//隐藏窗体
    m_hDispWnd = this->GetSafeHwnd();//默认视频画在本窗体上
    if (StartRunning())
    {
        //if(this->m_oIWBSensorManager.HasCalibratedCamera() && this->m_oIWBSensorManager.HasNormalUsageCamera())
        if (this->m_oIWBSensorManager.HasCalibratedCamera() && this->m_oIWBSensorManager.GetLensMode() == E_NORMAL_USAGE_MODE)
        {
            MinimizeToTray();
            m_bVisible = FALSE;
        }
        else
        {
            m_bVisible = TRUE;
        }
    }
    else
    {
        //"显示播放视频失败!" 提示信息
        CString strInfo;
        strInfo = g_oResStr[IDS_STRING436];

        //MessageBox(strInfo, g_oResStr[IDS_STRING109], MB_OK |MB_ICONERROR);
        m_bVisible = TRUE;
    }


    ShowTaskBar(TRUE);


    //是自动运行的模式,自动最小化到托盘中
    if(theApp.IsStartFromAutoRun())
    {
        MinimizeToTray();
        m_bVisible = FALSE;
    }

    SetTimer(TIMER_FOR_SENSOR_MANAGER, TIMER__FOR_SENSOR_MANAGER_TIMEOUT, NULL);

    //更新状态栏的软件狗注册信息
    UpdateInfoAboutDongle();
    return TRUE; // return TRUE unless you set the focus to a control
}

void CIWBDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else if(IDM_DEBUG_MODE_RECORD_VIDEO == nID)//录像
    {

        if(!this->m_oIWBSensorManager.IsRecording())
        {//原来没有录像，现在开始录像
            m_oIWBSensorManager.StartRecording();

            CMenu* pSysMenu = GetSystemMenu(FALSE);
            if (pSysMenu != NULL)
            {
                MENUITEMINFO mii;
                memset(&mii, 0, sizeof(mii));
                mii.cbSize = sizeof(MENUITEMINFO);

                mii.fMask = MIIM_STRING | MIIM_STATE;
                mii.fState = MFS_ENABLED;
                mii.dwTypeData = _T("Stop Record");
                pSysMenu->SetMenuItemInfo(IDM_DEBUG_MODE_RECORD_VIDEO, &mii, FALSE);
            }
        }
        else
        {//正在录像，停止录像
            m_oIWBSensorManager.StopRecording();

            CMenu* pSysMenu = GetSystemMenu(FALSE);
            if (pSysMenu != NULL)
            {
                MENUITEMINFO mii;
                memset(&mii, 0, sizeof(mii));

                mii.cbSize = sizeof(MENUITEMINFO);
                mii.fMask = MIIM_STRING | MIIM_STATE;
                mii.fState = MFS_ENABLED;
                mii.dwTypeData = _T("Start Record");
                pSysMenu->SetMenuItemInfo(IDM_DEBUG_MODE_RECORD_VIDEO, &mii, FALSE);
            }
        }
    }
    else if(IDM_DEBUG_MODE_READ_VIDEO == nID)//读取录像文件
    {
        CDebugVideoLoaderDlg debugVideoLoader;
        if(debugVideoLoader.DoModal() == IDOK)
        {
            this->m_oIWBSensorManager.LoadSrcFromAVI(debugVideoLoader.Video1Path(), debugVideoLoader.Video2Path());
        }
    }
    else if(nID  == IDM_CHECK_AUTO_CALIBRATE_PROCESS)
    {

        CSimulateCalibarationParamDlg dlg;

        if(dlg.DoModal() == IDOK)
        {
            this->m_oIWBSensorManager.DoSimulateAutoCalibrate(
                dlg.GetSelectSensorId(),
                this->GetSafeHwnd(),
                dlg.GetAVIFilePath(),
                CSize(dlg.GetScreenWidth(), dlg.GetScreenHeight()),
                dlg.GetImageAvgBrightness(),
                dlg.GetAutoCalibrateGridsNumber());
        }
    }
    else if(nID  == IDM_CALIBRATE_FORM_FILE)
    {

        CIWBSensor* pSensor0 = this->m_oIWBSensorManager.GetSensor();
        if(pSensor0)
        {
            CFileDialog fileDlg(TRUE, _T("*.*"));
            if(fileDlg.DoModal() == IDOK)
            {
                CString strDataFilePath = fileDlg.GetPathName();
                pSensor0->GetPenPosDetector()->GetVideoToScreenMap().CalibrateFromDataFile(strDataFilePath);
            }
        }
    }
    else if(nID == IDM_CHECK_CALIBRATE_SYMBOLS)
    {//add by 15077726@qq.com， 2015/09/09
        //搜集系统屏幕信息
        if(this->m_oIWBSensorManager.IsCalibrateSymbolVisible())
        {//原来有显示，现在关闭
            if(this->m_oIWBSensorManager.CheckCalibrateSymbols(FALSE))
            {
                CMenu* pSysMenu = GetSystemMenu(FALSE);
                if (pSysMenu != NULL)
                {
                    MENUITEMINFO mii;
                    memset(&mii, 0, sizeof(mii));

                    mii.cbSize = sizeof(MENUITEMINFO);
                    mii.fMask = MIIM_STRING | MIIM_STATE;
                    mii.fState = MFS_ENABLED;
                    mii.dwTypeData = _T("Show Calibrate Symbols On Screen");
                    pSysMenu->SetMenuItemInfo(IDM_CHECK_CALIBRATE_SYMBOLS, &mii, FALSE);
                }
            }
        }
        else
        {//原来无显示，现在打开
           
            if(this->m_oIWBSensorManager.CheckCalibrateSymbols(TRUE))
            {
                CMenu* pSysMenu = GetSystemMenu(FALSE);
                if (pSysMenu != NULL)
                {
                    MENUITEMINFO mii;
                    memset(&mii, 0, sizeof(mii));

                    mii.cbSize = sizeof(MENUITEMINFO);
                    mii.fMask = MIIM_STRING | MIIM_STATE;
                    mii.fState = MFS_ENABLED;
                    mii.dwTypeData = _T("Remove Calibrate Symbols From Screen");
                    pSysMenu->SetMenuItemInfo(IDM_CHECK_CALIBRATE_SYMBOLS, &mii, FALSE);
                }
            }
        }
     }
    else if(nID == IDM_CALIBRATE_MANUAL_RESAMPLE)
    {//调试功能， 自动校正后的校正符号再次人工采样。
        this->m_oIWBSensorManager.CalibrateSymbolManualResample();
    }
    else if(nID == IDM_CAPTURE_PICTURE)
    {
        CIWBSensor* pSensor = this->m_oIWBSensorManager.GetSensor();
        if(pSensor)
        {
            //pSensor->GetPenPosDetector()->StartImageCapture(0000);
            pSensor->GetInterceptFilter()->CaptureImage();
        }
        
    }
    else if (nID == IDM_GPIO_ON)
    {
        CIWBSensor* pSensor = this->m_oIWBSensorManager.GetSensor();
        if (pSensor)
        {
            GPIOControl(pSensor->GetVideoPlayer()->GetCaptureFilter(), TRUE);

        }
        
    }
    else if (nID == IDM_GPIO_OFF)
    {
        CIWBSensor* pSensor = this->m_oIWBSensorManager.GetSensor();
        if (pSensor)
        {
            GPIOControl(pSensor->GetVideoPlayer()->GetCaptureFilter(), FALSE);
        }
    }
    else if (nID == IDM_DEBUG_MODE_TEST30POINT)
    {
        if (!m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().GetTest30Point())
        {
            m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().SetTest30Point(TRUE);

            CMenu* pSysMenu = GetSystemMenu(FALSE);
            if (pSysMenu != NULL)
            {
                MENUITEMINFO mii;
                memset(&mii, 0, sizeof(mii));
                mii.cbSize = sizeof(MENUITEMINFO);

                mii.fMask = MIIM_STRING | MIIM_STATE;
                mii.fState = MFS_ENABLED;
                mii.dwTypeData = _T("Stop Test 30 TouchPoint");
                pSysMenu->SetMenuItemInfo(IDM_DEBUG_MODE_TEST30POINT, &mii, FALSE);
            }
        }
        else
        {
            m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().SetTest30Point(FALSE);

            CMenu* pSysMenu = GetSystemMenu(FALSE);
            if (pSysMenu != NULL)
            {
                MENUITEMINFO mii;
                memset(&mii, 0, sizeof(mii));
                mii.cbSize = sizeof(MENUITEMINFO);

                mii.fMask = MIIM_STRING | MIIM_STATE;
                mii.fState = MFS_ENABLED;
                mii.dwTypeData = _T("Start Test 30 TouchPoint");
                pSysMenu->SetMenuItemInfo(IDM_DEBUG_MODE_TEST30POINT, &mii, FALSE);
            }
        }

    }
    else if(nID == SC_CLOSE)
    {
        MinimizeToTray();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}


// If you add a minimize button to your dialog, you will need the code below
// to draw the icon. For MFC applications using the document/view model,
// this is automatically done for you by the framework.
void CIWBDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width()  - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CPaintDC dc(this); // device context for paintin
        CRect rcDisplayArea;

        GetClientRect(&rcDisplayArea);

        //m_PlayWnd.GetWindowRect(&rcDisplayArea);
        //this->ScreenToClient(&rcDisplayArea);

        //COLORREF clrBkgnd = RGB(16,0,16);//key color for overlay surface
        //
        //COLORREF clrBkgnd = RGB(255,0,0);//key color for overlay surface
        //dc.FillSolidRect(&rcDisplayArea,clrBkgnd);
        //DrawLayoutFrame(this->GetSafeHwnd());

        if (this->GetSafeHwnd() == m_hDispWnd)
        {
            m_oIWBSensorManager.DrawLayoutFrame(this->GetSafeHwnd());
        }

        CDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
// the minimized window.
HCURSOR CIWBDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


void CIWBDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    // TODO: Add your message handler code here

    if(!m_bAllWndCreated)
    {
        return ;
    }

    CRect rcClient;
    GetClientRect(&rcClient);
    ///add by vera_zhao 2109.12.18
    AdjustStatusBar(cx, cy);

    //调整状态栏的位置
//	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposDefault, NULL, NULL, TRUE);
//
//
//	int borders[3];
//	//The first element receives the width of the horizontal border, 
//	//the second receives the width of the vertical border, 
//	//and the third receives the width of the border between rectangles.
//	::SendMessage(m_ctlStatusBar.GetSafeHwnd(), SB_GETBORDERS, 0, (LPARAM)borders);
//
//	int nSensorCount = this->m_oIWBSensorManager.GetSensorCount();
//
//	int nSensorStatusWidth = cx;
//	if (nSensorCount != 0)
//	{
//		nSensorStatusWidth = cx / nSensorCount;
//	}
//
//	//reference D:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\src\mfc\barstat.cpp
//	const int  CX_PANE_BORDER = 8;//4    //3 pixels on each side of each pane
//
//	const int PANE1_MIN_WIDTH = 48 * 4;
//	int nPaneWidth_1 = nSensorStatusWidth / 10 - CX_PANE_BORDER - borders[2];
//	//当窗体很小时, 可能为负值。
//	if (nPaneWidth_1 < 0) nPaneWidth_1 = 0;
//
//
//	const int PANE2_MIN_WIDTH = 48 * 4;
//	int nPaneWidth_2 = nSensorStatusWidth / 3 - CX_PANE_BORDER - borders[2];
//	//当窗体很小时,可能为负值。
//	if (nPaneWidth_2 < 0) nPaneWidth_2 = 0;
//
//
//	int nPaneWidth_3 = nSensorStatusWidth - nPaneWidth_1 - nPaneWidth_2 - CX_PANE_BORDER - borders[2] * StatusPaneCountEachSensor;
//	//当窗体很小时,可能为负值。
//	if (nPaneWidth_3 < 0) nPaneWidth_3 = 0;
//
//	int nPaneWidth_4 = nSensorStatusWidth;
//	int nPaneWidth_5 = nSensorStatusWidth * 2;
//
//	//for(int i=0; i < nSensorCount; i++)
//	//  for (int i = 0; i < 1; i++)
//	//  {		
//	//      this->m_ctlStatusBar.SetPaneInfo(i*StatusPaneCountEachSensor + 0, indicators[i*StatusPaneCountEachSensor + 0], SBPS_NORMAL, nPaneWidth_1);
//	//      this->m_ctlStatusBar.SetPaneInfo(i*StatusPaneCountEachSensor + 1, indicators[i*StatusPaneCountEachSensor + 1], SBPS_NORMAL, nPaneWidth_2);
//	//      this->m_ctlStatusBar.SetPaneInfo(i*StatusPaneCountEachSensor + 2, indicators[i*StatusPaneCountEachSensor + 2], SBPS_OWNERDRAW, nPaneWidth_3);
//	//      this->m_ctlStatusBar.SetPaneInfo(i*StatusPaneCountEachSensor + 3, indicators[i*StatusPaneCountEachSensor + 3], SBPS_NORMAL, nPaneWidth_4);
//	//      this->m_ctlStatusBar.SetPaneInfo(i*StatusPaneCountEachSensor + 4, indicators[i*StatusPaneCountEachSensor + 4], SBPS_NORMAL, nPaneWidth_5);
//	//   }

    //查询除状态栏以外的客户区
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0,reposQuery, &rcClient, NULL, TRUE);

    this->InvalidateRect(&rcClient,FALSE);

    if (m_hDispWnd == this->GetSafeHwnd())
    {    
        //在当前窗口上显示视频时
        this->m_oIWBSensorManager.SetVideoDisplayArea(rcClient);
    }    
}

void CIWBDlg::AdjustStatusBar(int cx, int cy)
{
    //调整状态栏的位置
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposDefault, NULL, NULL, TRUE);


    int borders[3];
    //The first element receives the width of the horizontal border, 
    //the second receives the width of the vertical border, 
    //and the third receives the width of the border between rectangles.
    ::SendMessage(m_ctlStatusBar.GetSafeHwnd(), SB_GETBORDERS, 0, (LPARAM)borders);

    int nSensorCount = this->m_oIWBSensorManager.GetSensorCount();

    int nSensorStatusWidth = cx;
    if (nSensorCount != 0)
    {
        nSensorStatusWidth = cx / nSensorCount;
    }

    //reference D:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\src\mfc\barstat.cpp
    const int  CX_PANE_BORDER = 8;//4    //3 pixels on each side of each pane

    const int PANE1_MIN_WIDTH = 48 * 4;
    int nPaneWidth_1 = 0;
    if (nSensorCount < 2)
    {
        nPaneWidth_1 = nSensorStatusWidth / 10 - CX_PANE_BORDER - borders[2];
        //当窗体很小时, 可能为负值。
        if (nPaneWidth_1 < 0) nPaneWidth_1 = 0;
    }

    const int PANE2_MIN_WIDTH = 48 * 4;
    int nPaneWidth_2 = nSensorStatusWidth / 3 - CX_PANE_BORDER - borders[2];
    //当窗体很小时,可能为负值。
    if (nPaneWidth_2 < 0) nPaneWidth_2 = 0;


    int nPaneWidth_3 = nSensorStatusWidth - nPaneWidth_1 - nPaneWidth_2 - CX_PANE_BORDER - borders[2] * StatusPaneCountEachSensor;
    //当窗体很小时,可能为负值。
    if (nPaneWidth_3 < 0) nPaneWidth_3 = 0;

    int nPaneWidth_4 = nSensorStatusWidth- CX_PANE_BORDER - borders[2] * StatusPaneCountEachSensor;
    if (nPaneWidth_4 < 0)  nPaneWidth_4 = 0;
    int nPaneWidth_5 = nSensorStatusWidth- CX_PANE_BORDER - borders[2] * StatusPaneCountEachSensor;
    if (nPaneWidth_5 < 0)  nPaneWidth_5 = 0;
    int nPaneWidth_6 = nSensorStatusWidth- CX_PANE_BORDER - borders[2] * StatusPaneCountEachSensor;
    if (nPaneWidth_6 < 0)  nPaneWidth_6 = 0;
    int nPaneWidth_7 = nSensorStatusWidth- CX_PANE_BORDER - borders[2] * StatusPaneCountEachSensor;
    if (nPaneWidth_7 < 0)  nPaneWidth_7 = 0;
    int nPaneWidth_8 = nSensorStatusWidth- CX_PANE_BORDER - borders[2] * StatusPaneCountEachSensor;
    if (nPaneWidth_8 < 0)  nPaneWidth_8 = 0;
    //for(int i=0; i < nSensorCount; i++)
        
    this->m_ctlStatusBar.SetPaneInfo(0, indicators[0], SBPS_NORMAL, nPaneWidth_1);
    this->m_ctlStatusBar.SetPaneInfo(1, indicators[1], SBPS_NORMAL, nPaneWidth_2);
    this->m_ctlStatusBar.SetPaneInfo(2, indicators[2], SBPS_OWNERDRAW, nPaneWidth_3);

    this->m_ctlStatusBar.SetPaneInfo(3, indicators[3], SBPS_NORMAL, nPaneWidth_4);
    this->m_ctlStatusBar.SetPaneInfo(4, indicators[4], SBPS_NORMAL, nPaneWidth_5);
    this->m_ctlStatusBar.SetPaneInfo(5, indicators[5], SBPS_NORMAL, nPaneWidth_6);
    this->m_ctlStatusBar.SetPaneInfo(6, indicators[6], SBPS_NORMAL, nPaneWidth_7);
    this->m_ctlStatusBar.SetPaneInfo(7, indicators[7], SBPS_NORMAL, nPaneWidth_8);
    
}

//void CIWBDlg::AdjustStatusBar(int left, int top, int cx, int cy)
//{
//
//
//    //this->m_ctlStatusBar.SetPaneInfo(0, 1,
//    //CWnd* pWnd = NULL;
//    //CRect rcWnd;
//    //int nControlBarWidth;
//
//    //控制栏
//    //pWnd = GetDlgItem(IDC_STATIC_CONTROL_BAR);
//    //pWnd->GetClientRect(&rcWnd);
//    //nControlBarWidth = rcWnd.Width();
//
//    //rcWnd.left = left;
//    //rcWnd.top = top;
//    //rcWnd.right = rcWnd.left + nControlBarWidth;
//    //rcWnd.bottom = cy;
//    //pWnd->MoveWindow(&rcWnd);
//
//
//    //视频区域
//    //m_PlayWnd.GetClientRect(&rcWnd);
//
//    //rcWnd.left = left ;
//    //rcWnd.right = cx;
//    //rcWnd.top = top;
//    //rcWnd.bottom = cy;
//    //m_PlayWnd.MoveWindow(&rcWnd);
//
//    /*
//    if(m_oFilterGraphBuilder.IsDetecting())
//    {
//        m_PlayWnd.GetClientRect(&rcWnd);
//        m_oFilterGraphBuilder.ResizeVideo(rcWnd);
//    }*/
//
//
//    ////调整按钮位置
//    //int nBtnSpace = 5;//20 pixel
//    //int nBtnTop = top + nBtnSpace;
//    //int nBtnLeft = 0;
//    //int nBtnRight = 0;
//    //int nBtnBottom = 0;
//    //int nBtnWidth = 0;
//    //int nBtnHeight = 0;
//    //for(int nID = IDC_STATIC_CAMERA_LIST; nID <= IDC_STATIC_AVG_FPS; nID++)
//    //{
//
//    // pWnd = GetDlgItem(nID);
//
//    // if(pWnd == NULL) continue;
//
//    // CRect rcBtn;
//    // pWnd->GetClientRect(&rcBtn);
//
//    // nBtnWidth = rcBtn.Width();
//    // nBtnHeight = rcBtn.Height();
//
//    // nBtnLeft = left + (nControlBarWidth - nBtnWidth)/2;
//    // nBtnRight = nBtnLeft + nBtnWidth;
//
//    // nBtnBottom = nBtnTop + nBtnHeight;
//
//    // pWnd->MoveWindow(nBtnLeft, nBtnTop, nBtnWidth, nBtnHeight);
//
//    // if(nID < IDC_STATIC_Y_DETECT_MAX)
//    // {
//    // nBtnTop = nBtnBottom + nBtnSpace;
//    // }
//    // else
//    // {
//    // nBtnTop= nBtnBottom;
//    // }
//
//    //}
//}


HRESULT CIWBDlg::OnTrayNotifyMsg(WPARAM wParam,LPARAM lParam)
{
    UINT uIconID = (UINT)wParam;

    switch(lParam)
    {
    case WM_MOUSEMOVE:

        TRACE("OnTrayNotifyMsg::WM_MOUSE_MOVE\n");

        break;

    case WM_LBUTTONDOWN:

        TRACE("OnTrayNotifyMsg::WM_LBUTTONDOWN\n");
        break;

    case WM_LBUTTONUP:

        TRACE("OnTrayNotifyMsg::WM_LBUTTONUP\n");
        break;

    case WM_RBUTTONDOWN:

        TRACE("OnTrayNotifyMsg::WM_RBUTTONDOWN\n");
        break;

    case WM_RBUTTONUP:
        {

            //弹出快捷菜单

            CMenu menu;
            menu.LoadMenu(IDR_MENU_CTXMENU);
            InsertParamSettingMenuItem(&menu, 6);
            CMenu* pCtxMenu = menu.GetSubMenu(0);

            if(pCtxMenu)
            {
                InitDeviceUseModeMenuItemWithMenu(pCtxMenu); 
                BOOL isRunning = this->m_oIWBSensorManager.IsRunning();
                POINT ptCursor;
                GetCursorPos(&ptCursor);
                //
                if(PathFileExists(GetAutoStarupLnkPathEN())
                    ||
                    PathFileExists(GetAutoStarupLnkPathCHS()) 
                    || 
                    PathFileExists(GetAutoStarupLnkPathCHT()) 
                    ||
                    PathFileExists(GetAutoStarupLnkPathKOR()) 
                    ||
                    PathFileExists(GetAutoStarupLnkPathESP())
                    ||
                    PathFileExists(GetAutoStarupLnkPathDEU())
                    ||
                    PathFileExists(GetAutoStarupLnkPathFRA())
                    ||
                    PathFileExists(GetAutoStarupLnkPathRUS())
                    ||
                    PathFileExists(GetAutoStarupLnkPathITA())
                    ||
                    PathFileExists(GetAutoStarupLnkPathPTG())
                    ||
                    PathFileExists(GetAutoStarupLnkPathCommon()))
                {
                    pCtxMenu->CheckMenuItem(ID_CTXMENU_AUTORUN_AT_SYSTEM_STARTUP, MF_CHECKED | MF_BYCOMMAND);
                }
                else
                {
                    if(IsWin10OrGreater() && m_hUCShieldBitmap)
                    {
                        SetMenuItemBitmaps(
                            pCtxMenu->GetSafeHmenu(),
                            ID_CTXMENU_AUTORUN_AT_SYSTEM_STARTUP,
                            MF_BYCOMMAND,
                            m_hUCShieldBitmap,
                            m_hUCShieldBitmap);
                    }



                }
                SetForegroundWindow();



                if(this->m_oIWBSensorManager.IsOpticalPenControlling())
                {
                    pCtxMenu->CheckMenuItem(ID_MENU_DISABLE_OPTICAL_PEN_CONTROL, MF_BYCOMMAND|MF_UNCHECKED);
                }
                else
                {
                    pCtxMenu->CheckMenuItem(ID_MENU_DISABLE_OPTICAL_PEN_CONTROL, MF_BYCOMMAND|MF_CHECKED);
                }
                pCtxMenu->EnableMenuItem(ID_MENU_RUN, MF_BYCOMMAND|(isRunning ? MF_GRAYED:MF_ENABLED));
                pCtxMenu->EnableMenuItem(ID_MENU_STOP, MF_BYCOMMAND|(isRunning ? MF_ENABLED:MF_GRAYED));
                pCtxMenu->EnableMenuItem(ID_AUTO_ADD_MASK_AREA, MF_BYCOMMAND|(isRunning ? MF_ENABLED:MF_GRAYED));
                //pCtxMenu->EnableMenuItem(ID_MENU_MANUAL_CALIBRATE25, MF_BYCOMMAND|(isRunning ? MF_ENABLED:MF_GRAYED));
                //pCtxMenu->EnableMenuItem(ID_MENU_MANUAL_CALIBRATE36, MF_BYCOMMAND|(isRunning ? MF_ENABLED:MF_GRAYED));
                pCtxMenu->EnableMenuItem(ID_MENU_MANUAL_CALIBRATE, MF_BYCOMMAND | (isRunning ? MF_ENABLED : MF_GRAYED));
                pCtxMenu->EnableMenuItem(ID_CTXMENU_AUTOCALIBRATE, MF_BYCOMMAND|(isRunning ? MF_ENABLED:MF_GRAYED));


                pCtxMenu->TrackPopupMenu(TPM_RIGHTALIGN|TPM_BOTTOMALIGN,ptCursor.x,ptCursor.y,this);

            }
            TRACE("OnTrayNotifyMsg::WM_RBUTTONUP\n");
        }
        break;

    case WM_LBUTTONDBLCLK :

        m_bVisible = TRUE;
        ShowWindow(SW_SHOW);
        m_oTray.Remove();

        TRACE("OnTrayNotifyMsg::WM_LBUTTONDBLCLK\n");
        break;

    case WM_CONTEXTMENU://

        TRACE("OnTrayNotifyMsg::WM_CONTEXTMENU\n");
        break;



    case NIN_KEYSELECT:
        TRACE("OnTrayNotifyMsg::NIN_KEYSELECT\n");
        break;


    case NIN_BALLOONSHOW://Sent when the balloon is shown

        TRACE("OnTrayNotifyMsg::NIN_BALLOONSHOW\n");
        break;

    case NIN_BALLOONHIDE://Sent when the balloon disappears—for example, when the icon is deleted. This message is not sent if the balloon is dismissed because of a timeout or a mouse click.
        TRACE("OnTrayNotifyMsg::NIN_BALLOONHIDE\n");
        break;


    case NIN_BALLOONTIMEOUT:// Sent when the balloon is dismissed because of a timeout.
        TRACE("OnTrayNotifyMsg::NIN_BALLOONTIMEOUT\n");
        break;

    case NIN_BALLOONUSERCLICK:// Sent when the balloon is dismissed because of a mouse click.
        TRACE("OnTrayNotifyMsg::NIN_BALLOONUSERCLICK\n");
        break;

    }
    return 0;
}

LRESULT CIWBDlg::OnClose(WPARAM wParam,LPARAM lParam)
{
    // TODO: Add your message handler code here and/or call default
    this->StopRunning();
    if(m_pUSBDevDetector)
    {
        delete m_pUSBDevDetector;
        m_pUSBDevDetector = NULL;
    }
    if(m_pUSBDevDetector_HID)
    {
        delete m_pUSBDevDetector_HID;
        m_pUSBDevDetector_HID = NULL;
    }

    /*
    CIWBSensor* pSensor = this->m_oIWBSensorManager.GetSensor();
    if (pSensor)
    {
        const TCaptureDeviceInstance& devInfo = pSensor->GetDeviceInfo();
        if (!devInfo.m_strDevPath.IsEmpty())
        {   //如果为空的话，那么PID.VID就为空了，不不要保存
            SaveConfig();
        }
    }
    */

    //Removed by xuke, 2020/10/15
    //避免计算机正在关机，IWB执行到SaveConfig,还未退出，这时如果客户突然断电
    //则导致生成的配置文件格式不完整。在下次开机启动时，载入配置文件失败, 高清
    //摄像头直接使用标清的参数。
    //<<但如果不保存则界面上的一些状态无法保存。
    SaveConfig();
    //>>

    DeleteObject(m_hUCShieldBitmap);

    CDialog::OnClose();

    LOG_INF("CIWBDlg::OnClose()");
    return 0;

}

//void CIWBDlg::OnBnClickedButtonExit()
//{
//    // TODO: Add your control notification handler code here
//    Exit();
//}

void CIWBDlg::OnMenuExit()
{
    // TODO: Add your command handler code here
    LOG_INF("CIWBDlg::OnMenuExit()");
    Exit();

}

void CIWBDlg::OnCtxmenuShowMainWindow()
{
    // TODO: Add your command handler code here
    m_oTray.Remove();
    m_bVisible = TRUE;
    this->ShowWindow(SW_SHOW);
}

void CIWBDlg::Exit()
{
    LOG_INF("CIWBDlg::Exit()");
    //<added by jiqw:20150421
    //退出安装向导：
    m_tipProcServer.Stop();
    //>

    m_oTray.Remove();
    HWND hWnd = this->GetSafeHwnd();

    SendMessage(WM_CLOSE, 0xFF, 0);
}

//void CIWBDlg::OnBnClickedButtonCoordAdjust()
//{
// // TODO: Add your control notification handler code here
// OnCtxMenuManualCalibrate();
//}

void CIWBDlg::OnCtxMenuManualCalibrate()
{

    // TODO: Add your command handler code here
    this->m_oIWBSensorManager.StartManualCalibrate(this->GetSafeHwnd());
    /*
    if(HWND(m_oManualCalibrateWnd) == NULL)
    {
    m_oManualCalibrateWnd.Create();
    m_oManualCalibrateWnd.SetNotifyWnd(this->GetSafeHwnd());
    }

    //记录校正前的光笔控制状态。
    m_tOldState.bEnableOpticalPen = g_oMouseEventGen.IsOpticalPenControlling();

    g_oMouseEventGen.EnterCalibrateMode(HWND(m_oManualCalibrateWnd), CALIBRATE_MODE_MANUAL);


    //this->m_oFilterGraphBuilder.ResetVideoClipArea();
    // g_oMouseEventGen.m_oVideo2ScreenMap.SetAdjustingTag(TRUE);

    //2012/07/25, 作废手动补正数据
    //g_oVideo2ScreenMap.InvalidateManualCorrectData();

    m_oManualCalibrateWnd.StartCalibrate();  
    */
}

//void CIWBDlg::OnBnClickedButtonStartStop()
//{
// // TODO: Add your control notification handler code here
//
// if(!m_oFilterGraphBuilder.IsDetecting())
// {
// BOOL bRet = StartDetectRunning();
// if(bRet)
// {
// m_oFilterGraphBuilder.SetNotifyMessage(this->GetSafeHwnd(),WM_FPSNOTIFY);
//
//
// //Threshhold
// BYTE cThreshold = (m_SysCfgData.cYThreshold * 255 + 50/*四舍五入*/)/100;
// m_oFilterGraphBuilder.SetYThreshold(cThreshold);
//
//
// this->m_oFilterGraphBuilder.SetVideoClipArea(g_oMouseEventGen.m_oVideo2ScreenMap.GetVideoClipArea());
// g_oMouseEventGen.EnableOpticalPenControl(m_bEnableOpticalPenControl);
// GetDlgItem(IDC_BUTTON_START_STOP)->SetWindowText(_T("停止"));
//
// GetDlgItem(IDC_COMBO_CAMERA)->EnableWindow(FALSE);
// GetDlgItem(IDC_COMBO_IMAGE_FORMAT)->EnableWindow(FALSE);
// }
// }
// else
// {
// m_oFilterGraphBuilder.StopDetect();
// GetDlgItem(IDC_BUTTON_START_STOP)->SetWindowText(_T("启动"));
// GetDlgItem(IDC_COMBO_CAMERA)->EnableWindow(TRUE);
// GetDlgItem(IDC_COMBO_IMAGE_FORMAT)->EnableWindow(TRUE);
// }
//}

////
//HRESULT CIWBDlg::OnCalcAdjustMatrix(WPARAM wParam, LPARAM lParam)
//{
// CWaitCursor waitCursor;
// std::vector<POINT> vecAdjustData;
// vecAdjustData.resize(MANUAL_CALIBRATE_POINT_NUMBER);
//
// for(int i=0; i < MANUAL_CALIBRATE_POINT_NUMBER; i++)
// {
// POINT ptCross;
//
// m_oManualCalibrateWnd.GetCalibrateResult (i, &ptCross, &vecAdjustData[i]);
// g_oMouseEventGen.m_oVideo2ScreenMap.SetCalibrateData(i, ptCross, vecAdjustData[i]);
//
// }//for
//
// BOOL bRet = g_oMouseEventGen.m_oVideo2ScreenMap.DoCalibrate();
//
// if(!bRet)
// {
// return -1;
// }
//
// g_oMouseEventGen.EnableOpticalPenControl(TRUE);
//
// g_oMouseEventGen.LeaveCalibrateMode();
//
// return 0;
//}


HRESULT CIWBDlg::OnManualCalibrationDone (WPARAM wParam,LPARAM lParam)
{
    BOOL bSuccess = (BOOL)wParam;

    m_oIWBSensorManager.OnIWBSensorManualCalibrateDone(bSuccess, (DWORD)lParam);

    if(m_oIWBSensorManager.IsCalibarateOk())
    {
        //保存配置信息
        CIWBSensor* pSensor = this->m_oIWBSensorManager.GetSensor();
        if (pSensor)
        {
            const TCaptureDeviceInstance& devInfo = pSensor->GetDeviceInfo();
            if (!devInfo.m_strDevPath.IsEmpty())
            {
                this->SaveConfig();
            }

        }

        //最小化到托盘
        MinimizeToTray();
    }

    return 0;
}




const char* CONFIG_ROOT_ELMENTS_NAME = "Settings";

//<?xml version="1.0" encoding="utf-8"?>
//<Settings>
//<Global>
//
//</Global>
//<IWBSensors >
//  <IWBSensor>
//      <CameraParams mode="NormalUsage">
//        <!--亮度-->
//        <param name="Brightness", value="25"/>
//        <param name="Contrast", value="150" /> 
//    </CameraParams>
//    </Mode>
//  </IWBSensor>
//</IWBSensors>
//
//
//</Settings>

//@功能:载入配置文件
//@参数:lpszConfigFilePath, 配置文件的完整路路径
BOOL CIWBDlg::LoadConfig()
{
    //载入配置数据
    ::LoadConfig(PROFILE::CONFIG_FILE_NAME, ::g_tSysCfgData);

    //根据加密狗设置实际的笔触和手触模式
    //所有的整成一個加密狗信息
    for (size_t i = 0 ; i < ::g_tSysCfgData.vecSensorConfig.size() ; i++ )
    {
        TSensorConfig& sensorConfig = ::g_tSysCfgData.vecSensorConfig[i];
        for (EProjectionMode eProjectionMode = E_PROJECTION_DESKTOP; eProjectionMode < E_PROJECTION_COUNT; eProjectionMode = EProjectionMode(eProjectionMode + 1))
        {
            sensorConfig.vecSensorModeConfig[eProjectionMode].advanceSettings.m_eTouchType = GetActualTouchType();
        }
    }
    
    return TRUE;
}


//@功能:保存配置文件
//@参数:lpszConfigFilePath, 配置文件的完成路径
BOOL CIWBDlg::SaveConfig()
{
    //获取所有图像传感器的配置信息
    this->m_oIWBSensorManager.GetCfgData(g_tSysCfgData);

    size_t nSensorCount = this->m_oIWBSensorManager.GetSensorCount();
    //写入配置文件
    ::SaveConfig(PROFILE::CONFIG_FILE_NAME, ::g_tSysCfgData, nSensorCount);

    return TRUE;
}


HRESULT CIWBDlg::OnFpsNotify (WPARAM wParam,LPARAM lParam)
{
    UINT     uFPS = wParam;
    int nCameraID = lParam;

    CString strFPS;
    strFPS.Format(_T("%s:%d"), g_oResStr[IDS_STRING438],uFPS);

    if(0 == nCameraID)
    {
        this->m_ctlStatusBar.SetPaneText(PANE_FPS, strFPS, TRUE);
    }

    return 0;
}

HRESULT CIWBDlg::OnCameraStatusNotify(WPARAM wParam,LPARAM lParam)
{
    LPCTSTR lpszText =(LPCTSTR)wParam;
    int    nCameraID = lParam;
    int nSensorCount = this->m_oIWBSensorManager.GetSensorCount();
    if(0 == nCameraID)
    {
        this->m_ctlStatusBar.SetPaneText(PANE_STATE, lpszText, TRUE);
    }
    switch (nSensorCount)
    {
       case 2:
          if (1 == nCameraID)
          {
               this->m_ctlStatusBar.SetPaneText(PANE_STATE2, lpszText, TRUE);
          }
          break;
       case 3:
           if (1 == nCameraID)
           {
               this->m_ctlStatusBar.SetPaneText(PANE_STATE2, lpszText, TRUE);
           }
           else if (2 == nCameraID)
           {
               this->m_ctlStatusBar.SetPaneText(PANE_STATE3, lpszText, TRUE);
           }
           else{}
           break;
       case 4:
           if (1 == nCameraID)  {
               this->m_ctlStatusBar.SetPaneText(PANE_STATE2, lpszText, TRUE);
           }
           else if (2 == nCameraID) {
               this->m_ctlStatusBar.SetPaneText(PANE_STATE3, lpszText, TRUE);
           }
           else if (3 == nCameraID) {
               this->m_ctlStatusBar.SetPaneText(PANE_STATE4, lpszText, TRUE);
           }
           else{}
           break;
       case 5:
           if (1 == nCameraID) {
               this->m_ctlStatusBar.SetPaneText(PANE_STATE2, lpszText, TRUE);
           }
           else if (2 == nCameraID) {
               this->m_ctlStatusBar.SetPaneText(PANE_STATE3, lpszText, TRUE);
           }
           else if (3 == nCameraID) {
               this->m_ctlStatusBar.SetPaneText(PANE_STATE4, lpszText, TRUE);
           }
           else if(4 == nCameraID) {
               this->m_ctlStatusBar.SetPaneText(PANE_STATE5, lpszText, TRUE);
           }
           else { }
           break;
       case 6:
           if (1 == nCameraID) {
               this->m_ctlStatusBar.SetPaneText(PANE_STATE2, lpszText, TRUE);
           }
           else if (2 == nCameraID) {
               this->m_ctlStatusBar.SetPaneText(PANE_STATE3, lpszText, TRUE);
           }
           else if (3 == nCameraID) {
               this->m_ctlStatusBar.SetPaneText(PANE_STATE4, lpszText, TRUE);
           }
           else if (4 == nCameraID) {
               this->m_ctlStatusBar.SetPaneText(PANE_STATE5, lpszText, TRUE);
           }
           else if(5 == nCameraID){
               this->m_ctlStatusBar.SetPaneText(PANE_STATE6, lpszText, TRUE);
           }
           break;


    }

    return 0;
}


//@功能:开始检测
BOOL CIWBDlg::StartRunning()
{
    BOOL bRet = FALSE;
    RECT rcDispWnd;
    if (this->GetSafeHwnd() == m_hDispWnd)
    {
        RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0,reposQuery, &rcDispWnd, NULL, TRUE);
    }    
    else
    {
        ::GetClientRect(m_hDispWnd, &rcDispWnd);
    }
    m_oIWBSensorManager.SetVideoDispParams(this->GetSafeHwnd(), this->GetSafeHwnd(), rcDispWnd);
    bRet = this->m_oIWBSensorManager.StartRunning();

    return bRet;
}

//@功能:停止检测
BOOL CIWBDlg::StopRunning()
{
    this->m_oIWBSensorManager.StopRunning();
    return TRUE;
}


//@功能:屏幕分辨率变化事件响应
HRESULT CIWBDlg::OnDisplayChange (WPARAM wParam, LPARAM lParam)
{

    //参数:
    //wParam, the new image depth of the display, in bits per pixel.
    //lParam, The low-order word specifies the horizontal resolution of the screen.
    //        The high-order word specifies the vertical resolution of the screen.

    INT nScreenWidth  = LOWORD(lParam);
    INT nScreenHeight = HIWORD(lParam);

    //搜索屏幕信息
    theApp.GetMonitorFinder().SearchDisplayDev();

    //通知屏幕尺寸发生变化
    OnDisplayChangeHelper(::GetActualScreenControlSize());

    return 0;
}

HRESULT CIWBDlg::OnEnableOrDisableTheWindow(WPARAM wParam, LPARAM lParam)
{
    DWORD wndID = (DWORD)wParam;
    BOOL bEnable = (BOOL)lParam;

    CWnd *pWnd = GetDlgItem(wndID);

    if (pWnd)
    {
        pWnd->EnableWindow(bEnable);
    }

    return 0;
}

void CIWBDlg::OnDisplayChangeHelper(const SIZE& sizeNewScreen)
{
    //屏幕分辨率事件响应函数
    m_oIWBSensorManager.OnDisplayChange(sizeNewScreen.cx, sizeNewScreen.cy);
}


HRESULT CIWBDlg::OnDeviceChange(WPARAM wParam, LPARAM lParam)
{
    switch(wParam)//device event
    {
    case DBT_DEVICEARRIVAL://
        AtlTrace(_T("DBT_DEVICEARRIVAL\r\n"));
        {
            DEV_BROADCAST_HDR* pDevBroadcastHdr = reinterpret_cast<DEV_BROADCAST_HDR*>(lParam);

            switch(pDevBroadcastHdr->dbch_devicetype)
            {
            case DBT_DEVTYP_DEVICEINTERFACE:
                AtlTrace(_T("\tDBT_DEVTYP_DEVICEINTERFACE\r\n"));
                {
                    DEV_BROADCAST_DEVICEINTERFACE * pDevInterface =
                        reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE *>(lParam);
                    //
                    //KSCATEGORY_VIDEO     :{6994AD05-93EF-11D0-A3CC-00A0C9223196}
                    //KSCATEGORY_CAPTURE   :{65E8773D-8F56-11D0-A3B9-00A0C9223196}
                    //AM_KSCATEGORY_CAPTURE:{65E8773D-8F56-11D0-A3B9-00A0C9223196}
                    AtlTrace(_T("\tclass guid={%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\r\n"),
                        pDevInterface->dbcc_classguid.Data1,
                        pDevInterface->dbcc_classguid.Data2,
                        pDevInterface->dbcc_classguid.Data3,
                        pDevInterface->dbcc_classguid.Data4[0],
                        pDevInterface->dbcc_classguid.Data4[1],
                        pDevInterface->dbcc_classguid.Data4[2],
                        pDevInterface->dbcc_classguid.Data4[3],
                        pDevInterface->dbcc_classguid.Data4[4],
                        pDevInterface->dbcc_classguid.Data4[5],
                        pDevInterface->dbcc_classguid.Data4[6],
                        pDevInterface->dbcc_classguid.Data4[7]
                    ); 
                    AtlTrace(_T("\t\tInterface name %s\r\n"), pDevInterface->dbcc_name);
                    
                    if(m_oUSBCameraDeviceList.IsCandidateDevice(pDevInterface->dbcc_name))
                    {
                        //更新设备列表
                        this->m_oUSBCameraDeviceList.UpdateDeviceList();

                        const TCaptureDeviceInstance* pDevInst = m_oUSBCameraDeviceList.GetCaptureDeviceInstance(pDevInterface->dbcc_name);
                        if(pDevInst)
                        {
                            //在检测到新设备时读取新插入的加密狗
                            CW2A strDevpath(pDevInterface->dbcc_name);
                            theApp.OnPlugInUSBKey(strDevpath);

                            this->m_oIWBSensorManager.OnCameraPlugIn(*pDevInst);
                            
                            //更新状态信息
                            UpdateInfoAboutDongle();
                        }
                    }
                    else
                    {
                        UINT uVID = 0; 
                        UINT uPID = 0;
                        int ret = _stscanf_s(pDevInterface->dbcc_name, _T("\\\\?\\HID#VID_%04x&PID_%04x"), &uVID, &uPID);
                        if(this->IsEligibleUSBKey(uVID, uPID))
                        {
                            //在检测到新设备时读取新插入的加密狗
                            CW2A strDevpath(pDevInterface->dbcc_name);
                            theApp.OnPlugInUSBKey(strDevpath);
                            
                            //更新状态信息
                           UpdateInfoAboutDongle();
                        }

                    }
                }
                break;

            case DBT_DEVTYP_HANDLE:
                AtlTrace(_T("\tDBT_DEVTYP_HANDLE\r\n"));
                break;

            case DBT_DEVTYP_OEM:
                AtlTrace(_T("\tDBT_DEVTYP_OEM\r\n"));

                break;

            case DBT_DEVTYP_PORT:
                AtlTrace(_T("\tDBT_DEVTYP_PORT\r\n"));
                break;

            case DBT_DEVTYP_VOLUME:
                AtlTrace(_T("\tDBT_DEVTYP_VOLUME\r\n"));
                break;
            }
        }
        break;

    case DBT_DEVICEREMOVECOMPLETE://USB设备拔离USB端口
        AtlTrace(_T("DBT_DEVICEREMOVECOMPLETE\r\n"));
        {
            DEV_BROADCAST_HDR* pDevBroadcastHdr = reinterpret_cast<DEV_BROADCAST_HDR*>(lParam);

            switch(pDevBroadcastHdr->dbch_devicetype)
            {
            case DBT_DEVTYP_DEVICEINTERFACE:
                AtlTrace(_T("\tDBT_DEVTYP_DEVICEINTERFACE\r\n"));
                {
                    DEV_BROADCAST_DEVICEINTERFACE * pDevInterface =
                        reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE *>(lParam);
                    //{6BDD1FC6-810F-11D0-BEC7-08002BE2092F}
                    AtlTrace(_T("\tclass guid={%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\r\n"),
                        pDevInterface->dbcc_classguid.Data1,
                        pDevInterface->dbcc_classguid.Data2,
                        pDevInterface->dbcc_classguid.Data3,
                        pDevInterface->dbcc_classguid.Data4[0],
                        pDevInterface->dbcc_classguid.Data4[1],
                        pDevInterface->dbcc_classguid.Data4[2],
                        pDevInterface->dbcc_classguid.Data4[3],
                        pDevInterface->dbcc_classguid.Data4[4],
                        pDevInterface->dbcc_classguid.Data4[5],
                        pDevInterface->dbcc_classguid.Data4[6],
                        pDevInterface->dbcc_classguid.Data4[7]
                    );

                    AtlTrace(_T("\t\tInterface name %s\r\n"), pDevInterface->dbcc_name);

                    if(m_oUSBCameraDeviceList.IsCandidateDevice(pDevInterface->dbcc_name))
                    {
                        //更新设备列表
                        this->m_oUSBCameraDeviceList.UpdateDeviceList();

                        //响应PlugOut事件
                        this->m_oIWBSensorManager.OnCameraPlugOut(pDevInterface->dbcc_name);
                        
                        //在检测到新设备时读取新插入的加密狗
                        CW2A strDevpath(pDevInterface->dbcc_name);
                        theApp.OnPlugOutUSBKey(strDevpath);

                        //更新状态信息
                        UpdateInfoAboutDongle();
                    }
                    else
                    {
                        int nVID = 0;
                        int nPID = 0;
                        int ret = _stscanf_s(pDevInterface->dbcc_name, _T("\\\\?\\HID#VID_%04x&PID_%04x"), &nVID, &nPID);
                        if (nVID == 13961 && nPID == 34658)
                        {

                            //在检测到新设备时读取新插入的加密狗
                            CW2A strDevpath(pDevInterface->dbcc_name);
                            theApp.OnPlugOutUSBKey(strDevpath);

                            //更新状态信息
                            UpdateInfoAboutDongle();
                        }
                    }
                }

                break;

            case DBT_DEVTYP_HANDLE:
                AtlTrace(_T("\tDBT_DEVTYP_HANDLE\r\n"));
                break;

            case DBT_DEVTYP_OEM:
                AtlTrace(_T("\tDBT_DEVTYP_OEM\r\n"));

                break;

            case DBT_DEVTYP_PORT:
                AtlTrace(_T("\tDBT_DEVTYP_PORT\r\n"));
                break;

            case DBT_DEVTYP_VOLUME:
                AtlTrace(_T("\tDBT_DEVTYP_VOLUME\r\n"));
                break;

            }
        }

        break;

    case DBT_DEVNODES_CHANGED:
        AtlTrace(_T("DBT_DEVNODES_CHANGED\r\n"));
        break;

    default:
        AtlTrace(_T("Other event %d\r\n"), wParam);
        break;
    }//swtich

    return 0;
}


void CIWBDlg::OnMenuParameterSettings()
{
    CIWBSensor *pSensor = m_oIWBSensorManager.GetSensor();
    CParamsSettingPropertySheet paramsSettingSheet(const_cast<LPTSTR>(g_oResStr[IDS_STRING452]));

    paramsSettingSheet.SetIWBSensorInfo(pSensor, 1, false);
    paramsSettingSheet.SetGlobalSettingInfo(g_tSysCfgData.globalSettings);

    if(paramsSettingSheet.DoModal() == IDOK)
    {
        pSensor->SetCfgData(paramsSettingSheet.GetSensorConfig(), &paramsSettingSheet.GetGlobalSettings());

        g_tSysCfgData.vecSensorConfig[pSensor->GetID()] = paramsSettingSheet.GetSensorConfig();
        g_tSysCfgData.globalSettings = paramsSettingSheet.GetGlobalSettings();

        const TCaptureDeviceInstance& devInfo  = pSensor->GetDeviceInfo();
        if (!devInfo.m_strDevPath.IsEmpty())
        {
             int nCount = this->m_oIWBSensorManager.GetSensorCount();
             ::SaveConfig(PROFILE::CONFIG_FILE_NAME, ::g_tSysCfgData, nCount);
        }

        ////////////把设置的是否动态屏蔽传到需要的地方去
        TSensorModeConfig* TSensorModeConfig = NULL;
        EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;

        TSensorModeConfig = &g_tSysCfgData.vecSensorConfig[pSensor->GetID()].vecSensorModeConfig[eProjectionMode];
        //如果选择0.15，那么在手指和白板触控的时候插值是需要打开的。
        if (  g_tSysCfgData.vecSensorConfig[pSensor->GetID()].eSelectedLensType == E_LENS_TR_0_DOT_15
            || g_tSysCfgData.vecSensorConfig[pSensor->GetID()].eSelectedLensType == E_LENS_TR_0_DOT_19
            || g_tSysCfgData.vecSensorConfig[pSensor->GetID()].eSelectedLensType == E_LENS_TR_0_DOT_21
            )
        {
            if (TSensorModeConfig->advanceSettings.m_eTouchType != E_DEVICE_PALM_TOUCH_CONTROL)
            {
                 TSensorModeConfig->advanceSettings.bEnableStrokeInterpolate = TRUE;
                 pSensor->SetStrokeInterpolate(TRUE);
            }
        }

        /////设置是否开启自动屏蔽功能
        if (TSensorModeConfig->advanceSettings.bIsDynamicMaskFrame)
        {
            pSensor->GetPenPosDetector()->EnableDynamicMasking(TRUE);
        }
        else
        {
            pSensor->GetPenPosDetector()->EnableDynamicMasking(FALSE);
        }
        ////////////设置是否开启抗干扰功能
        if (TSensorModeConfig->advanceSettings.bIsAntiJamming)
        {
            pSensor->GetPenPosDetector()->EnableAntiJamming(TRUE);
        }
        else
        {
            pSensor->GetPenPosDetector()->EnableAntiJamming(FALSE);
        }
        /////////设置是否启用手动绘制的静态屏蔽图
        if (TSensorModeConfig->advanceSettings.bIsOnLineScreenArea)
        {
            pSensor->GetPenPosDetector()->EnableOnlineScreenArea(TRUE);
        }
        else
        {
            pSensor->GetPenPosDetector()->EnableOnlineScreenArea(FALSE);
        }
        ///////////////////
        if (TSensorModeConfig->advanceSettings.bDisableReflectionSpot)
        {
            pSensor->GetPenPosDetector()->DisableReflectionPoint(TRUE);
        }
        else {
            pSensor->GetPenPosDetector()->DisableReflectionPoint(FALSE);
        }
        //是否单点操作
        if (g_tSysCfgData.globalSettings.bSinglePointMode)
        {
            this->m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().SetSinglePointMode(true);
        }
        else
        { 
            this->m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().SetSinglePointMode(false);
        }

        /////是否进行隔空操作
        EAIROPERATE_CLICKMODE eClickMode = g_tSysCfgData.globalSettings.eClickMode;
        if(g_tSysCfgData.globalSettings.bAirOperatePermission)
        {
            this->m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().SetAirOperateMode(true, eClickMode);
        }
        else
        {
            this->m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().SetAirOperateMode(false, eClickMode);
        }

        //设置平滑系数
        this->m_oIWBSensorManager.GetSpotListProcessor().SetSmoothCoefficient(TSensorModeConfig->advanceSettings.nSmoothCoefficient);

    }//if
}

void CIWBDlg::OnMenuRun()
{
    // TODO: Add your command handler code here
    StartRunning();
}

void CIWBDlg::OnMenuStop()
{
    // TODO: Add your command handler code here
    StopRunning();
}

//void CIWBDlg::OnMenuManualCalibrate()
//{
//    // TODO: Add your command handler code here
//    OnCtxMenuManualCalibrate();
//}

void CIWBDlg::OnStartMenuAutoCalibrate()
{
    //<<2014/04/21
    //OnMenuAutoCalibrate(ID_AUTOCALIBRATION_17X17GRIDS);
       m_AutoCalibrateColsType = DEFAULT_CALIBRATE_PATTERN;
      //m_AutoCalibrateColsType = E_CALIBRATE_PATTERN_9_COLS;
      DoAutoCalibrate();
    //OnMenuAutoCalibrate(ID_AUTOCALIBRATION_15X15GRIDS);
    //2014/04/21>>
}


void CIWBDlg::DoAutoCalibrate()
{
    this->m_oIWBSensorManager.StartAutoCalibrate(m_AutoCalibrateColsType, this->GetSafeHwnd());
}

void CIWBDlg::OnEnterIdle(UINT nWhy, CWnd* pWho)
{
    CDialog::OnEnterIdle(nWhy, pWho);

    // TODO: Add your message handler code here
    if(MSGF_MENU == nWhy)
    {

    }

}


LRESULT CIWBDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
    return 0L;
}


HRESULT CIWBDlg::OnSetDetectThreshold(WPARAM wParam, LPARAM lParam)
{
    return 0;
}

void CIWBDlg::OnMenuAbout()
{
    // TODO: Add your command handler code here
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}

void CIWBDlg::OnMenuOnlineRegister()
{
    // TODO: Add your command handler code here
    COnlineRegisterDlg onlineRegisterDlg;
    onlineRegisterDlg.DoModal();
}


void CIWBDlg::OnMenuStatus()
{
    // TODO: Add your command handler code here
    this->m_ctlStatusBar.ShowWindow(SW_SHOW);
}


void CIWBDlg::OnMenuDisableOpticalPenControl()
{
    // TODO: Add your command handler code here
    this->m_oIWBSensorManager.EnableOpticalPen(!this->m_oIWBSensorManager.IsOpticalPenControlling());
}


void CIWBDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    switch(nIDEvent)
    {
        //case TIMER_OPEN_CLOSE_TEST:
        //    /*
        //    if(m_oFilterGraphBuilder.IsDetecting())
        //    {
        //    StopRunning();

        //    }
        //    else
        //    {
        //    StartRunning();
        //    }
        //    */
        //    break;
        //case TIMER_AUTO_CLUTTER_DETECTION_AFTER_AUTO_CALIBRATION://
        //    {
        //        /*
        //        CCountdownTimer::StartCountdownTimer(
        //        5,
        //        300,
        //        NULL,
        //        RGB(0,0,255),
        //        this->GetSafeHwnd(),
        //        CLUTTER_DETECTION_DONE_MSG);

        //        CPenPosDetector* pPenPosDetector = this->m_oFilterGraphBuilder.GetPenPosDetector();
        //        if(pPenPosDetector)
        //        {
        //        pPenPosDetector->EnableDynamicMasking(FALSE);//关闭动态屏蔽
        //        //       pPenPosDetector->StartClutterDetection(TRUE, PROFILE::GetAutoMaskingTheshold());   //delete by zwn 2013.06.21
        //        /////m_nAutoMaskDetectThreshold为自动屏蔽的门限
        //        pPenPosDetector->StartClutterDetection(TRUE, m_nAutoMaskDetectThreshold);
        //        }

        //        KillTimer(TIMER_AUTO_CLUTTER_DETECTION_AFTER_AUTO_CALIBRATION);
        //        */
        //    }
        //    break;

    case TIMER_FOR_SENSOR_MANAGER:
        this->m_oIWBSensorManager.OnTimer((LPVOID)this);
        break;

    case TIMER_EVENT_SCREEN_RECOGNITION_CLOSE:
        m_oScreenRecognition.StopRecognition();
        KillTimer(TIMER_EVENT_SCREEN_RECOGNITION_CLOSE);
        m_uScreenRecognitionCloseTimer = 0u;
        break;

    }//swtich

    CDialog::OnTimer(nIDEvent);
}

void CIWBDlg::OnProperyPage()
{
    // TODO: Add your command handler code here
    // m_oFilterGraphBuilder.ShowPropertyPage(this->GetSafeHwnd());
}

void CIWBDlg::OnLangSelect()
{
    // TODO: Add your command handler code here
    //CLangSelectDlg langSelectDlg(PROFILE::GetCfg_LCID());
    CLangSelectDlg langSelectDlg(::g_tSysCfgData.globalSettings.langCode);
    langSelectDlg.DoModal();

    g_tSysCfgData.globalSettings.langCode = langSelectDlg.GetSelectedLangCode();

    //PROFILE::SaveLCD(langSelectDlg.GetSelectLangLCID());
    //if(langSelectDlg.IsLangChanged())

    if(_tcsicmp(theApp.GetLangeCode() , langSelectDlg.GetSelectedLangCode()) !=0)
    {
        MessageBox(g_oResStr[IDS_STRING110], g_oResStr[IDS_STRING107], MB_ICONINFORMATION|MB_OK);
    }

}

//@功能:最小化到托盘中
void CIWBDlg::MinimizeToTray()
{    
    this->ShowWindow(SW_HIDE);
    m_oTray.AddToSystemTray();
}



//@功能:自动启动设定
void CIWBDlg::OnCtxmenuAutorunAtSystemStartup()
{
    // TODO: Add your command handler code here

    //if(!PathFileExists(GetAutoStarupLnkPathCN()) && !PathFileExists(GetAutoStarupLnkPathEN()) && !PathFileExists(GetAutoStarupLnkPathCommon()))
    if(PathFileExists(GetAutoStarupLnkPathEN())
        ||
        PathFileExists(GetAutoStarupLnkPathCHS()) 
        || 
        PathFileExists(GetAutoStarupLnkPathCHT()) 
        ||
        PathFileExists(GetAutoStarupLnkPathKOR()) 
        ||
        PathFileExists(GetAutoStarupLnkPathESP())
        ||
        PathFileExists(GetAutoStarupLnkPathDEU())
        ||
        PathFileExists(GetAutoStarupLnkPathFRA())
        ||
        PathFileExists(GetAutoStarupLnkPathRUS())
        ||
        PathFileExists(GetAutoStarupLnkPathITA())
        ||
        PathFileExists(GetAutoStarupLnkPathPTG())
        ||
        PathFileExists(GetAutoStarupLnkPathCommon()))
    {
        DeleteFile(GetAutoStarupLnkPathEN());
        DeleteFile(GetAutoStarupLnkPathCHS());
        DeleteFile(GetAutoStarupLnkPathCHT());
        DeleteFile(GetAutoStarupLnkPathKOR());
        DeleteFile(GetAutoStarupLnkPathESP());
        DeleteFile(GetAutoStarupLnkPathDEU());
        DeleteFile(GetAutoStarupLnkPathFRA());
        DeleteFile(GetAutoStarupLnkPathITA());
        DeleteFile(GetAutoStarupLnkPathPTG());
        DeleteFile(GetAutoStarupLnkPathRUS());
        DeleteFile(GetAutoStarupLnkPathCommon());
    }
    else
    {
        CString strShortcutPath = GetStarupDirPath(theApp.IsForAllUsers());
        //LOG_INF("startup menu path:%s", CT2CA(strShortcutPath));
        TCHAR szModuleFileName[MAX_PATH];

        DWORD dwRet = GetModuleFileName(theApp.m_hInstance, szModuleFileName, _countof(szModuleFileName));

        if(dwRet == 0)
        {
            LOG_ERR("GetModuleFileName failed 0x%x", dwRet);
            return;
        }

        TCHAR szWorkingDirectory[MAX_PATH];
        ZeroMemory(szWorkingDirectory, sizeof(szWorkingDirectory));


        _tcscpy_s(szWorkingDirectory, _countof(szWorkingDirectory), szModuleFileName);

        TCHAR* pLastBackslash= _tcsrchr(szWorkingDirectory, _T('\\'));

        *pLastBackslash=_T('\0');

        
        TCHAR szFileBaseName[MAX_PATH];
        _tcscpy_s(szFileBaseName, _countof(szFileBaseName), szModuleFileName);

        PathRemoveExtension(szFileBaseName);

        //<<2017,解决Win10删除动态链接库的问题,用IWBProxy.exe代替IWb.exe自动启动
        _stprintf_s(
            szModuleFileName,
            _countof(szModuleFileName),
            //_T("%s\\%sProxy.exe"),
            _T("%s\\%s.exe"),
            szWorkingDirectory,
            PathFindFileName(szFileBaseName));
        //>>

        CString strCmdLine;
        strCmdLine.Format(_T("-AutoRun -%s"),theApp.IsForAllUsers()?_T("AllUsers"):_T("CurrentUser"));

        HRESULT hr =
            CreateShortcut(
            szModuleFileName,//快捷方式指向的模块名称,
            g_oResStr[IDS_STRING124],//Description, 快捷方式备注
            szWorkingDirectory,//工作路径
            strShortcutPath,//
            strCmdLine);


        if(FAILED(hr))
        {
            //LOG_ERR("Create Shortcut %s failed!Error Code:0x%x", CT2CA(strShortcutPath), hr);

            CString appFullPath;
           appFullPath.Format(_T("%s\\%s"), szWorkingDirectory, _T("IWBCreateShortcut.exe"));
            /*
            
            //开启客户端进程
            PROCESS_INFORMATION processInfo;
            STARTUPINFO si;

            ZeroMemory( &si, sizeof(si) );
            si.cb = sizeof(si);



            BOOL bResult = CreateProcess(
            appFullPath,
            _T(""),
            NULL,//Process handle not inheritable
            NULL,//Thread handle not inheritable.
            TRUE,//Set handle inheritance to TRUE
            0,   //no creation flag
            NULL,//Use parent's environment block. 
            szWorkingDirectory,//Use parent's starting directory
            &si, // Pointer to STARTUPINFO structure.
            &processInfo);// Pointer to PROCESS_INFORMATION structure.

            if (!bResult)
            {
                DWORD dwErr = GetLastError();

                if( dwError == ERROR_ELEVATION_REQUIRED)
                {
                    LOG_ERR("Create Process %s failed!Error Code, ", (LPCTSTR)appFullPath);
                }
                else
                {
                     LOG_ERR("Create Process %s failed!Error Code:0x%x", (LPCTSTR)appFullPath, GetLastError());

                }
                return ; //开启客户端进程失败，直接退出
            }

            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);*/

           //参数
           ///app-path "C:\IWB\Whiteboard Calibration\IWB.exe" /arguments "-allUser" /shortcut-path "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Startup\IWB.lnk" /work-directory "C:\IWB\Whiteboard Calibration" /description "description"
            
            CString parameters;
            parameters.Format(
               _T("/app-path \"%s\" /arguments \"%s\" /shortcut-path \"%s\"  /work-directory  \"%s\"   /description \"%s\""),
              szModuleFileName,
              (LPCTSTR)strCmdLine,
              (LPCTSTR)strShortcutPath,
              szWorkingDirectory,
               g_oResStr[IDS_STRING124]);

            //关键在于runas这个Verb, 用来实现UAC Elevation(UAC提升)
            SHELLEXECUTEINFO shExInfo = {0};
            shExInfo.cbSize = sizeof(shExInfo);
            shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
            shExInfo.hwnd = 0;
            shExInfo.lpVerb = _T("runas");        // Operation to perform
            shExInfo.lpFile = (LPCTSTR)appFullPath;       // Application to start
            shExInfo.lpParameters = parameters;          // Additional parameters
            shExInfo.lpDirectory = 0;
            shExInfo.nShow = SW_SHOW;
            shExInfo.hInstApp = 0;  

            if (ShellExecuteEx(&shExInfo))
            {
                WaitForSingleObject(shExInfo.hProcess, INFINITE);
                CloseHandle(shExInfo.hProcess);
            }//if
            else
            {
                 LOG_ERR("ShellExecuteEx %s failed! Error Code:0x%x", (LPCTSTR)appFullPath, GetLastError());
            }//else

        }

    }

}

void CIWBDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);
    // TODO: Add your message handler code here
    /*	TRACE(_T("bShow=%d,nStatus=%d\n"),bShow, nStatus);*/
}

void CIWBDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
    CDialog::OnWindowPosChanging(lpwndpos);

    // TODO: Add your message handler code here
    if(!m_bVisible)
    {
        lpwndpos->flags &= ~SWP_SHOWWINDOW;
    }


}

void CIWBDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
    CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

    if (pPopupMenu != m_oMenu.GetSubMenu(0) && pPopupMenu != m_oMenu.GetSubMenu(1))	return;
    
    // TODO: Add your message handler code here

    if(this->m_oIWBSensorManager.IsRunning())
    {
        m_oMenu.EnableMenuItem(ID_MENU_RUN, MF_BYCOMMAND| MF_GRAYED );//灰化运行菜单
        m_oMenu.EnableMenuItem(ID_MENU_STOP, MF_BYCOMMAND|MF_ENABLED);//使能停止菜单


        //使能"自动屏蔽"菜单项
        m_oMenu.EnableMenuItem(ID_AUTO_ADD_MASK_AREA, MF_BYCOMMAND| MF_ENABLED);

        //使能"光斑采样"菜单项
        m_oMenu.EnableMenuItem(ID_OPERATION_LIGHTSPOTSAMPLING, MF_BYCOMMAND | MF_ENABLED);

        //使能“4点标定"菜单项
        m_oMenu.EnableMenuItem(ID_MENU_AUTO_CALIBRATE, MF_BYCOMMAND | MF_ENABLED);

        //使能"自动校正"菜单项
        m_oMenu.EnableMenuItem(ID_MENU_AUTO_CALIBRATE, MF_BYCOMMAND| MF_ENABLED);

        //使能"手动校正"菜单项
        //m_oMenu.EnableMenuItem(ID_MENU_MANUAL_CALIBRATE25, MF_BYCOMMAND| MF_ENABLED);
        //m_oMenu.EnableMenuItem(ID_MENU_MANUAL_CALIBRATE36, MF_BYCOMMAND| MF_ENABLED);
        m_oMenu.EnableMenuItem(ID_MENU_MANUAL_CALIBRATE, MF_BYCOMMAND | MF_ENABLED);

        //使能相机镜头模式
        m_oMenu.EnableMenuItem(ID_WORKMODE_NORMAL_USAGE,  MF_BYCOMMAND|MF_ENABLED); 
        m_oMenu.EnableMenuItem(ID_WORKMODE_IMAGE_TUNNING, MF_BYCOMMAND|MF_ENABLED); 
        m_oMenu.EnableMenuItem(ID_WORKMODE_LASER_TUNNING, MF_BYCOMMAND|MF_ENABLED);


        //<<added by toxuke@gmail.com, 2013/06/14
        switch(this->m_oIWBSensorManager.GetLensMode())
        {
           case E_NORMAL_USAGE_MODE://正常使用模式
                m_oMenu.CheckMenuItem(ID_WORKMODE_NORMAL_USAGE,  MF_BYCOMMAND|MF_CHECKED  ); 
                m_oMenu.CheckMenuItem(ID_WORKMODE_IMAGE_TUNNING, MF_BYCOMMAND|MF_UNCHECKED); 
                m_oMenu.CheckMenuItem(ID_WORKMODE_LASER_TUNNING, MF_BYCOMMAND|MF_UNCHECKED);

                //使能"光斑采样"菜单项
                if (GetActualTouchType() == E_DEVICE_PALM_TOUCH_CONTROL)
                {
                     m_oMenu.EnableMenuItem(ID_OPERATION_LIGHTSPOTSAMPLING, MF_BYCOMMAND | MF_GRAYED);
                }
                else
                {
                     m_oMenu.EnableMenuItem(ID_OPERATION_LIGHTSPOTSAMPLING, MF_BYCOMMAND| MF_ENABLED);
                }

                //使能"手动编辑屏蔽区"子菜单
                m_oMenu.EnableMenuItem(ID_ADD_MASK_RECTANGLE_2X,   MF_BYCOMMAND| MF_ENABLED);
                m_oMenu.EnableMenuItem(ID_ERASE_MASK_RECTANGLE_2X, MF_BYCOMMAND| MF_ENABLED); 
                m_oMenu.EnableMenuItem(ID_MANUALMASKAREA_ENDEDITING, MF_BYCOMMAND| MF_ENABLED);
                
                
                //使能"4点标定"子菜单
                if (theApp.IsFourPointCalibrateEnabled())
                {
                    m_oMenu.EnableMenuItem(ID_MENU_FOURPOINTCALIBRATION, MF_BYCOMMAND | MF_ENABLED);
                }
                else
                {
                    m_oMenu.EnableMenuItem(ID_MENU_FOURPOINTCALIBRATION, MF_BYCOMMAND | MF_GRAYED);
                }

                //使能"手动校正"子菜单
                //m_oMenu.EnableMenuItem(ID_MENU_MANUAL_CALIBRATE25,  MF_BYCOMMAND| MF_ENABLED);
                //m_oMenu.EnableMenuItem(ID_MENU_MANUAL_CALIBRATE36,  MF_BYCOMMAND| MF_ENABLED);
                m_oMenu.EnableMenuItem(ID_MENU_MANUAL_CALIBRATE, MF_BYCOMMAND | MF_ENABLED);



                ////////绘制屏蔽图只有在摄像头模式下才可以使用，其他模式下直接是灰掉的
                m_oMenu.EnableMenuItem(ID_MENU_DRAWMASKFRAME_START, MF_BYCOMMAND | MF_GRAYED);
                m_oMenu.EnableMenuItem(ID_MENU_DRAWMASKFRAME_CLEAR, MF_BYCOMMAND | MF_GRAYED);
                m_oMenu.EnableMenuItem(ID_MENU_DRAWMASKFRAME_DISABLE, MF_BYCOMMAND | MF_GRAYED);

                break;

          case E_VIDEO_TUNING_MODE://图像调试模式

               m_oMenu.CheckMenuItem(ID_WORKMODE_NORMAL_USAGE,  MF_BYCOMMAND|MF_UNCHECKED); 
               m_oMenu.CheckMenuItem(ID_WORKMODE_IMAGE_TUNNING, MF_BYCOMMAND|MF_CHECKED  );
               m_oMenu.CheckMenuItem(ID_WORKMODE_LASER_TUNNING, MF_BYCOMMAND|MF_UNCHECKED);

               //灰化"光斑采样"菜单项
               m_oMenu.EnableMenuItem(ID_OPERATION_LIGHTSPOTSAMPLING, MF_BYCOMMAND| MF_GRAYED);

               //灰化"手动编辑屏蔽区"子菜单
               m_oMenu.EnableMenuItem(ID_ADD_MASK_RECTANGLE_2X,   MF_BYCOMMAND| MF_GRAYED);
               m_oMenu.EnableMenuItem(ID_ERASE_MASK_RECTANGLE_2X, MF_BYCOMMAND| MF_GRAYED); 
               m_oMenu.EnableMenuItem(ID_MANUALMASKAREA_ENDEDITING, MF_BYCOMMAND| MF_GRAYED);

               //灰化"4点标定"子菜单
               m_oMenu.EnableMenuItem(ID_MENU_FOURPOINTCALIBRATION, MF_BYCOMMAND | MF_GRAYED);

               //灰化"手动校正"子菜单
               //m_oMenu.EnableMenuItem(ID_MENU_MANUAL_CALIBRATE25,  MF_BYCOMMAND| MF_GRAYED);
               //m_oMenu.EnableMenuItem(ID_MENU_MANUAL_CALIBRATE36,  MF_BYCOMMAND| MF_GRAYED);
               m_oMenu.EnableMenuItem(ID_MENU_MANUAL_CALIBRATE, MF_BYCOMMAND | MF_GRAYED);

               ////////绘制屏蔽图只有在摄像头模式下才可以使用，其他模式下直接是灰掉的
               m_oMenu.EnableMenuItem(ID_MENU_DRAWMASKFRAME_START, MF_BYCOMMAND | MF_ENABLED);
               m_oMenu.EnableMenuItem(ID_MENU_DRAWMASKFRAME_CLEAR, MF_BYCOMMAND | MF_ENABLED);
               m_oMenu.EnableMenuItem(ID_MENU_DRAWMASKFRAME_DISABLE,MF_BYCOMMAND| MF_ENABLED);

               break;

           case  E_LASER_TUNING_MODE://激光器调试模式
                m_oMenu.CheckMenuItem(ID_WORKMODE_NORMAL_USAGE,  MF_BYCOMMAND|MF_UNCHECKED); 
                m_oMenu.CheckMenuItem(ID_WORKMODE_IMAGE_TUNNING, MF_BYCOMMAND|MF_UNCHECKED);
                m_oMenu.CheckMenuItem(ID_WORKMODE_LASER_TUNNING, MF_BYCOMMAND|MF_CHECKED  );

                //灰化"光斑采样"菜单项
                m_oMenu.EnableMenuItem(ID_OPERATION_LIGHTSPOTSAMPLING, MF_BYCOMMAND| MF_GRAYED);

                //灰化"手动编辑屏蔽区"子菜单
                m_oMenu.EnableMenuItem(ID_ADD_MASK_RECTANGLE_2X,   MF_BYCOMMAND| MF_GRAYED);
                m_oMenu.EnableMenuItem(ID_ERASE_MASK_RECTANGLE_2X, MF_BYCOMMAND| MF_GRAYED); 
                m_oMenu.EnableMenuItem(ID_MANUALMASKAREA_ENDEDITING, MF_BYCOMMAND| MF_GRAYED);

                //灰化"4点标定"子菜单
                m_oMenu.EnableMenuItem(ID_MENU_FOURPOINTCALIBRATION, MF_BYCOMMAND | MF_GRAYED);


                //灰化"手动校正"子菜单
                //m_oMenu.EnableMenuItem(ID_MENU_MANUAL_CALIBRATE25,  MF_BYCOMMAND| MF_GRAYED);
                //m_oMenu.EnableMenuItem(ID_MENU_MANUAL_CALIBRATE36,  MF_BYCOMMAND| MF_GRAYED);
                m_oMenu.EnableMenuItem(ID_MENU_MANUAL_CALIBRATE, MF_BYCOMMAND | MF_GRAYED);

                ////////绘制屏蔽图只有在摄像头模式下才可以使用，其他模式下直接是灰掉的
                m_oMenu.EnableMenuItem(ID_MENU_DRAWMASKFRAME_START, MF_BYCOMMAND | MF_GRAYED);
                m_oMenu.EnableMenuItem(ID_MENU_DRAWMASKFRAME_CLEAR, MF_BYCOMMAND | MF_GRAYED);
                m_oMenu.EnableMenuItem(ID_MENU_DRAWMASKFRAME_DISABLE,MF_BYCOMMAND| MF_GRAYED);

                break;
        }

        ////插值
        m_oMenu.EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_ENABLEINTERPOLATE, MF_BYCOMMAND | MF_ENABLED);	
        ///高级设置
        ///如果是手触或者笔触电子白板的话，那么就灰掉高级设置
        //if (theApp.GetUSBKeyTouchType() == E_DEVICE_FINGER_TOUCH_WHITEBOARD || theApp.GetUSBKeyTouchType() == E_DEVICE_PEN_TOUCH_WHITEBOARD)
        //{
        //	m_oMenu.EnableMenuItem(ID_MENU_ADVANCESSETTING, MF_BYCOMMAND | MF_GRAYED);
        //}
        //else
        {
             m_oMenu.EnableMenuItem(ID_MENU_ADVANCESSETTING, MF_BYCOMMAND | MF_ENABLED);
        }

    }
    else
    {   
        m_oMenu.EnableMenuItem(ID_MENU_RUN, MF_BYCOMMAND|MF_ENABLED );//使能运行菜单
        m_oMenu.EnableMenuItem(ID_MENU_STOP, MF_BYCOMMAND|MF_GRAYED );//灰化停止菜单

        //灰化"自动屏蔽"菜单项
        m_oMenu.EnableMenuItem(ID_AUTO_ADD_MASK_AREA, MF_BYCOMMAND| MF_GRAYED);

        //灰化"光斑采样"菜单项
        m_oMenu.EnableMenuItem(ID_OPERATION_LIGHTSPOTSAMPLING, MF_BYCOMMAND| MF_GRAYED);

        //灰化"自动校正"菜单项
        m_oMenu.EnableMenuItem(ID_MENU_AUTO_CALIBRATE, MF_BYCOMMAND| MF_GRAYED);

        //灰化"手动校正"菜单项
        m_oMenu.EnableMenuItem(ID_MENU_MANUAL_CALIBRATE, MF_BYCOMMAND| MF_GRAYED);
        //m_oMenu.EnableMenuItem(ID_MENU_MANUAL_CALIBRATE36, MF_BYCOMMAND| MF_GRAYED);

        m_oMenu.EnableMenuItem(ID_ADD_MASK_RECTANGLE_2X, MF_BYCOMMAND | MF_GRAYED);
        m_oMenu.EnableMenuItem(ID_ERASE_MASK_RECTANGLE_2X, MF_BYCOMMAND | MF_GRAYED);

        m_oMenu.EnableMenuItem(ID_MANUALMASKAREA_ENDEDITING, MF_BYCOMMAND| MF_GRAYED);



        //灰化相机镜头模式
        m_oMenu.EnableMenuItem(ID_WORKMODE_NORMAL_USAGE,  MF_BYCOMMAND|MF_GRAYED); 
        m_oMenu.EnableMenuItem(ID_WORKMODE_IMAGE_TUNNING, MF_BYCOMMAND|MF_GRAYED); 
        m_oMenu.EnableMenuItem(ID_WORKMODE_LASER_TUNNING, MF_BYCOMMAND|MF_GRAYED);

        ////插值
        m_oMenu.EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_ENABLEINTERPOLATE, MF_BYCOMMAND | MF_GRAYED);
        ///高级设置
        m_oMenu.EnableMenuItem(ID_MENU_ADVANCESSETTING, MF_BYCOMMAND | MF_GRAYED);

        ////////绘制屏蔽图只有在摄像头模式下才可以使用，其他模式下直接是灰掉的
        m_oMenu.EnableMenuItem(ID_MENU_DRAWMASKFRAME_START, MF_BYCOMMAND | MF_GRAYED);
        m_oMenu.EnableMenuItem(ID_MENU_DRAWMASKFRAME_CLEAR, MF_BYCOMMAND | MF_GRAYED);
        m_oMenu.EnableMenuItem(ID_MENU_DRAWMASKFRAME_DISABLE, MF_BYCOMMAND | MF_GRAYED);

        //更新固件。。
        m_oMenu.EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_UPDATEFIRMWARE, MF_BYCOMMAND | MF_GRAYED);

    }

    if(this->m_oIWBSensorManager.IsOpticalPenControlling())
    {
        m_oMenu.CheckMenuItem(ID_MENU_DISABLE_OPTICAL_PEN_CONTROL, MF_BYCOMMAND|MF_UNCHECKED);
        //AtlTrace(_T("OnInitMenuPopup, MF_UNCHECKED\r\n"));
    }
    else
    {
        m_oMenu.CheckMenuItem(ID_MENU_DISABLE_OPTICAL_PEN_CONTROL, MF_BYCOMMAND|MF_CHECKED);
        //AtlTrace(_T("OnInitMenuPopup, MF_CHECKED\r\n"));
    }

    if (this->m_oIWBSensorManager.GetSensor()->GetCameraType() == E_CAMERA_MODEL_2)
    {
        m_oMenu.EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_UPDATEFIRMWARE, MF_BYCOMMAND | MF_GRAYED);
    }
    else
    {
        m_oMenu.EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_UPDATEFIRMWARE, MF_BYCOMMAND | MF_ENABLED);
    }

    ////add by vera_zhao 2018.12.04
    ETouchScreenAspectRatio eScreenRatio;

    eScreenRatio = g_tSysCfgData.globalSettings.eTouchScreenAspectRatio;	

    ///>>end
    switch(eScreenRatio)
    {
        case E_TOUCH_SCREEN_ASPECT_RATIO_16_9:
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_AUTO,  MF_BYCOMMAND|MF_UNCHECKED  ); 
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_16_09, MF_BYCOMMAND|MF_CHECKED    );
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_16_10, MF_BYCOMMAND|MF_UNCHECKED  );
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_4_3,   MF_BYCOMMAND|MF_UNCHECKED  );
            break;

        case E_TOUCH_SCREEN_ASPECT_RATIO_16_10:
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_AUTO,  MF_BYCOMMAND|MF_UNCHECKED  ); 
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_16_09, MF_BYCOMMAND|MF_UNCHECKED  );
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_16_10, MF_BYCOMMAND|MF_CHECKED    );
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_4_3,   MF_BYCOMMAND|MF_UNCHECKED  );
            break;

        case E_TOUCH_SCREEN_ASPECT_RATIO_4_3:
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_AUTO,  MF_BYCOMMAND|MF_UNCHECKED  ); 
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_16_09, MF_BYCOMMAND|MF_UNCHECKED  );
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_16_10, MF_BYCOMMAND|MF_UNCHECKED  );
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_4_3,   MF_BYCOMMAND|MF_CHECKED    );
            break;

        default:
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_AUTO,  MF_BYCOMMAND|MF_CHECKED    ); 
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_16_09, MF_BYCOMMAND|MF_UNCHECKED  );
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_16_10, MF_BYCOMMAND|MF_UNCHECKED  );
            m_oMenu.CheckMenuItem(ID_TOUCHSCREENASPECTRATIO_4_3,   MF_BYCOMMAND|MF_UNCHECKED  );

    }//switch



    DEVMODE dm;
    //initialize the DEVMODE structure
    //ZeroMemory(&dm, sizeof(dm));
    dm.dmSize = sizeof(dm);

    
    /*
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
    if (!(dm.dmFields & DM_DISPLAYFIXEDOUTPUT) || dm.dmDisplayFixedOutput == DMDFO_CENTER || dm.dmDisplayFixedOutput == DMDFO_STRETCH)
    {//dmDisplayFixedOutput未置位或者其值等于DMDFO_CENTER或者DMDFO_STRETCH时，禁用"Touch Screen Aspect Ratio"菜单项
        
     //灰化"自动设置宽高比"菜单项
     m_oMenu.EnableMenuItem(ID_TOUCHSCREENASPECTRATIO_AUTO,  MF_BYCOMMAND | MF_GRAYED);
     m_oMenu.EnableMenuItem(ID_TOUCHSCREENASPECTRATIO_16_09, MF_BYCOMMAND | MF_GRAYED);
     m_oMenu.EnableMenuItem(ID_TOUCHSCREENASPECTRATIO_16_10, MF_BYCOMMAND | MF_GRAYED);
     m_oMenu.EnableMenuItem(ID_TOUCHSCREENASPECTRATIO_4_3,   MF_BYCOMMAND | MF_GRAYED);

    }
    else
    {
        //使能"自动设置宽高比"菜单项
        m_oMenu.EnableMenuItem(ID_TOUCHSCREENASPECTRATIO_AUTO, MF_BYCOMMAND  | MF_ENABLED);
        m_oMenu.EnableMenuItem(ID_TOUCHSCREENASPECTRATIO_16_09, MF_BYCOMMAND | MF_ENABLED);
        m_oMenu.EnableMenuItem(ID_TOUCHSCREENASPECTRATIO_16_10, MF_BYCOMMAND | MF_ENABLED);
        m_oMenu.EnableMenuItem(ID_TOUCHSCREENASPECTRATIO_4_3, MF_BYCOMMAND   | MF_ENABLED);
    }
    */
    ///////插值标志
    BOOL bEnableStrokeInterpolateTemp= FALSE ;
    BOOL bEnableOnlineScreenArea = FALSE;

    EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;

    CIWBSensor* lpSensor = this->m_oIWBSensorManager.GetSensor();
    if (lpSensor)
    {
        TAdvancedSettings& advanceSettings = g_tSysCfgData.vecSensorConfig[lpSensor->GetID()].vecSensorModeConfig[eProjectionMode].advanceSettings;
        bEnableStrokeInterpolateTemp = advanceSettings.bEnableStrokeInterpolate;
    }
    ////是否进行插值
    if (bEnableStrokeInterpolateTemp)
    {
        m_oMenu.CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_ENABLEINTERPOLATE, MF_BYCOMMAND | MF_CHECKED);
    }
    else
    {
        m_oMenu.CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_ENABLEINTERPOLATE, MF_BYCOMMAND | MF_UNCHECKED);
    }
    //////是否启用绘制的外部勾勒图
    if (this->m_oIWBSensorManager.IsEnableOnlineScreenArea())
    {
        m_oMenu.CheckMenuItem(ID_MENU_DRAWMASKFRAME_DISABLE, MF_BYCOMMAND | MF_UNCHECKED);
    }
    else
    {
        m_oMenu.CheckMenuItem(ID_MENU_DRAWMASKFRAME_DISABLE, MF_BYCOMMAND | MF_CHECKED);
    }

    //如果"虚拟驱动"已打开, 则使能"Mouse"和"TouchPad"菜单项"
    InitDeviceUseModeMenuItemWithMenu(&m_oMenu);

    //如果未使能"在线注册功能", 则隐藏帮助菜单下的“注册"菜单项
    BOOL bEnableOnlineRegisterTemp;
    bEnableOnlineRegisterTemp = g_tSysCfgData.globalSettings.bEnableOnlineRegister;
    if(!bEnableOnlineRegisterTemp)
    {
        m_oMenu.RemoveMenu(ID_MENU_ONLINE_REGISTER, MF_BYCOMMAND);
    }


    //如果是多屏屏接, 添加"触屏布局设计工具"菜单项
    //modified by xuke, 2024/06/07, 单屏也可能需要编辑屏幕区域
    //if (theApp.GetScreenMode() >= EScreenModeDouble)
    if(1)
     {
        CMenu* pInstallMenu = m_oMenu.GetSubMenu(1);
        MENUITEMINFO menuItemInfo;
        memset(&menuItemInfo, 0, sizeof(MENUITEMINFO));
        menuItemInfo.cbSize = sizeof(MENUITEMINFO);

        if (pInstallMenu && !pInstallMenu->GetMenuItemInfo(ID_SWTICH_SCREENMODE, &menuItemInfo))
        {
            AppendScreenModeSubmenu();
        }

        BOOL bIsVisible = this->m_oIWBSensorManager.GetScreenLayoutDesigner().IsVisible();
        m_oMenu.CheckMenuItem(ID_MENU_TOUCHSREEEN_LAYOUT_DESIGNER, MF_BYCOMMAND | bIsVisible?MF_CHECKED: MF_UNCHECKED);
    }

    if (theApp.GetScreenModeFromUSBKey() >= EScreenModeDouble)
    {
        for (EScreenMode eScreenMode = EScreenModeSingle; eScreenMode <= theApp.GetScreenModeFromUSBKey(); eScreenMode = EScreenMode(eScreenMode + 1))
        {
            UINT uMenuID = ID_SWTICH_SCREENMODE_ONE + (UINT)eScreenMode;
            int nSensorCount = int(eScreenMode) + 1;
            if (nSensorCount == this->m_oIWBSensorManager.GetSensorCount())
            {//勾选当前的拼接模式
                m_oMenu.CheckMenuItem(uMenuID, MF_BYCOMMAND | MF_CHECKED );
            }
            else
            {
                m_oMenu.CheckMenuItem(uMenuID, MF_BYCOMMAND | MF_UNCHECKED);
            }
        }
    }

    //
    if (!g_tSysCfgData.globalSettings.bEnable4PointsCalibrate)
    {
        m_oMenu.RemoveMenu(ID_MENU_FOURPOINTCALIBRATION, MF_BYCOMMAND);
    }
}

void CIWBDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
    // TODO: Add your message handler code here

    LRESULT lr = SendMessage(WM_NCHITTEST, (WPARAM)0, (LPARAM)MAKELONG(point.x, point.y));
    if(lr != HTCLIENT)
    {

        Default(); //目的:不影响系统菜单的正常显示
        return;
    }


    //从鼠标按下时的坐标查找关联的图像传感器对象指针
//    POINT ptClient = point;
//    ScreenToClient(&ptClient);
//    CIWBSensor* pSensor = this->m_oIWBSensorManager.SensorFromPt(ptClient);
    CIWBSensor* pSensor = this->m_oIWBSensorManager.GetSensor();
    if(NULL == pSensor) return;

    m_pSelectedSensor = pSensor;//更新选中的传感器指针
    CMenu menuSwapTargets;
    CMenu mnuSensorCtx;
    CMenu menuAreaNo;
    mnuSensorCtx.LoadMenu(IDR_MENU_SENSOR_CTX);
    CMenu* pSubMenu = mnuSensorCtx.GetSubMenu(0);

    //根据传感器的状态设置传感器右键快捷菜单的状态
    if(pSubMenu)
    {
        //"运行"菜单项
        pSubMenu->EnableMenuItem(
            ID_SENSORCTXMENU_RUN,
            MF_BYCOMMAND|(pSensor->IsDetecting()?MF_GRAYED:MF_ENABLED));

        //"停止"菜单项
        pSubMenu->EnableMenuItem(
            ID_SENSORCTXMENU_STOP,
            MF_BYCOMMAND|(pSensor->IsDetecting()?MF_ENABLED:MF_GRAYED));

        //"禁用"菜单项
        if(!pSensor->IsDetecting())
        {
            pSubMenu->EnableMenuItem(
                ID_SENSORCTXMENU_DISABLE,
                MF_BYCOMMAND|MF_GRAYED);
        }
        else
        {
            pSubMenu->EnableMenuItem(
                ID_SENSORCTXMENU_DISABLE,
                MF_BYCOMMAND|MF_ENABLED);

            if(pSensor->IsOpticalPenControlling())
            {
                pSubMenu->CheckMenuItem(ID_SENSORCTXMENU_DISABLE, MF_BYCOMMAND|MF_UNCHECKED);

            }
            else
            {
                pSubMenu->CheckMenuItem(ID_SENSORCTXMENU_DISABLE, MF_BYCOMMAND|MF_CHECKED);
            }
        }

        //"自动校正..."菜单项
        pSubMenu->EnableMenuItem(
            ID_SENSORCTXMENU_AUTO_CALIBRATE,
            MF_BYCOMMAND|(pSensor->IsDetecting()?MF_ENABLED:MF_GRAYED));

        //"手动校正..."菜单项
        if(FALSE == pSensor->IsDetecting())
        {
            pSubMenu->EnableMenuItem(
                ID_SENSORCTXMENU_MANUAL_CALIBRATE,
                MF_BYCOMMAND|MF_GRAYED);
        }
        else
        {
            //只有在正常使用模式下才使能"手动校正"
            if(pSensor->GetLensMode() == E_NORMAL_USAGE_MODE)
            {
                pSubMenu->EnableMenuItem(
                    ID_SENSORCTXMENU_MANUAL_CALIBRATE,
                    MF_BYCOMMAND|MF_ENABLED);
            }
            else
            {
                pSubMenu->EnableMenuItem(
                    ID_SENSORCTXMENU_MANUAL_CALIBRATE,
                    MF_BYCOMMAND|MF_GRAYED);
            }
        }

        if (g_tSysCfgData.globalSettings.bEnable4PointsCalibrate )
        {
            if (pSensor->IsDetecting())
            {
                pSubMenu->EnableMenuItem(
                       ID_SENSORCTXMENU_FOURPOINTCALIBRATION,
                       MF_BYCOMMAND | MF_ENABLED);
            }
            else
            {
                pSubMenu->EnableMenuItem(
                    ID_SENSORCTXMENU_FOURPOINTCALIBRATION,
                    MF_BYCOMMAND | MF_GRAYED);
            }

        }
        else
        {
            pSubMenu->RemoveMenu(ID_SENSORCTXMENU_FOURPOINTCALIBRATION, MF_BYCOMMAND);
        }

        //"自动屏蔽..."菜单项
        pSubMenu->EnableMenuItem(
            ID_SENSORCTXMENU_AUTOMASK,
            MF_BYCOMMAND|(pSensor->IsDetecting()?MF_ENABLED:MF_GRAYED));

        //"设置..."菜单项ID_SENSORCTXMENU_IMAGE_SENSOR_SETTINGS

        //"设置"菜单项


        //"光斑采集..."菜单项
        pSubMenu->EnableMenuItem(
            ID_SENSORCTXMENU_LIGHTSPOT_SAMPLING,
            MF_BYCOMMAND|(pSensor->IsDetecting()?MF_ENABLED:MF_GRAYED));

        if (GetActualTouchType() == E_DEVICE_PALM_TOUCH_CONTROL)
        {
            pSubMenu->EnableMenuItem(ID_SENSORCTXMENU_LIGHTSPOT_SAMPLING, MF_BYCOMMAND|MF_GRAYED);
        }

        //"安装向导菜单项"
        pSubMenu->EnableMenuItem(
            ID_SENSORCTXMENU_INSTALL_TIP,
            MF_BYCOMMAND|(/*pSensor->IsDetecting()?MF_ENABLED:*/MF_GRAYED));


        //如果镜头模式为"正常使用模式"且处于屏蔽图编辑状态，添加"结束屏蔽图编辑"菜单项。
        if(this->m_oIWBSensorManager.IsRunning()
            &&
            this->m_oIWBSensorManager.GetLensMode() == E_NORMAL_USAGE_MODE 
            && 
            m_eScreenMaskAreaEditMode != e_SCREEN_MASK_AREA_EDIT_MODE_IDLE)
        {
            MENUITEMINFO mnuiteminfo;
            memset(&mnuiteminfo, 0, sizeof(MENUITEMINFO));
            mnuiteminfo.cbSize = sizeof(MENUITEMINFO);
            mnuiteminfo.fMask  = MIIM_FTYPE ;
            mnuiteminfo.fType  = MFT_SEPARATOR;

            //添加分割条
            pSubMenu->InsertMenuItem(0, &mnuiteminfo, TRUE);

            //添加结束屏幕编辑菜单项
            mnuiteminfo.fMask      = MIIM_FTYPE | MIIM_STRING | MIIM_ID ;
            mnuiteminfo.fType      = MFT_STRING;
            mnuiteminfo.dwTypeData = const_cast<LPTSTR>(g_oResStr[IDS_STRING447]);
            mnuiteminfo.wID        = ID_MANUALMASKAREA_ENDEDITING;
            pSubMenu->InsertMenuItem(0, &mnuiteminfo, TRUE);
        }

        if (this->m_oIWBSensorManager.IsRunning()
            &&
            this->m_oIWBSensorManager.GetLensMode() == E_VIDEO_TUNING_MODE
            )
        {

            if(theApp.GetScreenMode() == EScreenModeDouble)
            {//双屏拼接模式下,添加"图像交换"菜单项。
           
                MENUITEMINFO mnuiteminfo;
                memset(&mnuiteminfo, 0, sizeof(MENUITEMINFO));
                mnuiteminfo.cbSize = sizeof(MENUITEMINFO);
                mnuiteminfo.fMask = MIIM_FTYPE;
                mnuiteminfo.fType = MFT_SEPARATOR;
                pSubMenu->InsertMenuItem(
                    pSubMenu->GetMenuItemCount(),
                    &mnuiteminfo,
                    TRUE);

                //"交换画面"菜单项。
                mnuiteminfo.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
                mnuiteminfo.fType = MFT_STRING;
                mnuiteminfo.dwTypeData = const_cast<LPTSTR>(g_oResStr[IDS_STRING448]);
                mnuiteminfo.wID = ID_SWAP_SENSOR_IMAGE;
                pSubMenu->InsertMenuItem(pSubMenu->GetMenuItemCount(), &mnuiteminfo, TRUE);
            }
            else if (theApp.GetScreenMode() >= EScreenModeTriple)
            {
                //多屏拼接模式下, 添加"图像交换"子菜单。
                MENUITEMINFO mnuiteminfo;
                memset(&mnuiteminfo, 0, sizeof(MENUITEMINFO));
                mnuiteminfo.cbSize = sizeof(MENUITEMINFO);
                mnuiteminfo.fMask = MIIM_FTYPE;
                mnuiteminfo.fType = MFT_SEPARATOR;
                pSubMenu->InsertMenuItem(
                    pSubMenu->GetMenuItemCount(),
                    &mnuiteminfo,
                    TRUE);

                menuSwapTargets.CreatePopupMenu();

                //"交换画面"子菜单。
                mnuiteminfo.fMask =  MIIM_SUBMENU | MIIM_ID| MIIM_STRING;
                mnuiteminfo.dwTypeData = const_cast<LPTSTR>(g_oResStr[IDS_STRING448]);
                mnuiteminfo.hSubMenu = menuSwapTargets.GetSafeHmenu();
                mnuiteminfo.wID = ID_SWAP_SENSOR_IMAGE;
                pSubMenu->InsertMenuItem(pSubMenu->GetMenuItemCount(), &mnuiteminfo, TRUE);
  
                int nSensorCount     = m_oIWBSensorManager.GetSensorCount();
                int nCurrentSensorId = pSensor->GetID();

                for (int id = 0; id < nSensorCount; id++)
                {
                    if (id == nCurrentSensorId) continue;
                    CString strID;
                    strID.Format(_T("%d"), id+1);

                    mnuiteminfo.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
                    mnuiteminfo.fType = MFT_STRING;
                    mnuiteminfo.dwTypeData = const_cast<LPTSTR>(strID.GetString());
                    mnuiteminfo.wID = ID_SWAP_WITH_SENSOR0 + id;
                    menuSwapTargets.InsertMenuItem(menuSwapTargets.GetMenuItemCount(), &mnuiteminfo, TRUE);
                }

            }
        }
        /////////只有在摄像头模式下才能进行屏蔽图绘制
        if(FALSE == pSensor->IsDetecting() || pSensor->GetLensMode() != E_VIDEO_TUNING_MODE)
        {
            pSubMenu->EnableMenuItem(
                ID_SENSORCTXMENU_DRAWMASKFRAME_START,
                MF_BYCOMMAND | MF_GRAYED);

            pSubMenu->EnableMenuItem(
                ID_SENSORCTXMENU_DRAWMASKFRAME_DISABLE,
                MF_BYCOMMAND | MF_GRAYED);

            pSubMenu->EnableMenuItem(
                ID_SENSORCTXMENU_DRAWMASKFRAME_CLEAR,
                MF_BYCOMMAND | MF_GRAYED);
        }
        else
        {
            if (pSensor->IsEnableOnlineScreenArea())
            {
                pSubMenu->CheckMenuItem(ID_SENSORCTXMENU_DRAWMASKFRAME_DISABLE, MF_BYCOMMAND | MF_UNCHECKED);
            }
            else
            {
                pSubMenu->CheckMenuItem(ID_SENSORCTXMENU_DRAWMASKFRAME_DISABLE, MF_BYCOMMAND | MF_CHECKED);
            }

        }


        //
        //if (theApp.GetScreenMode() >= EScreenModeDouble)
        //Commented out by xuke, 2024/06/28
        if(1)
        {//多屏模式下，添加区域绑定菜单项

            MENUITEMINFO mnuiteminfo;
            memset(&mnuiteminfo, 0, sizeof(MENUITEMINFO));
            mnuiteminfo.cbSize = sizeof(MENUITEMINFO);
            mnuiteminfo.fMask = MIIM_FTYPE;
            mnuiteminfo.fType = MFT_SEPARATOR;
            pSubMenu->InsertMenuItem(
                pSubMenu->GetMenuItemCount(),
                &mnuiteminfo,
                TRUE);

            menuAreaNo.CreatePopupMenu();

            //屏幕区域列表子菜单
            mnuiteminfo.fMask = MIIM_SUBMENU | MIIM_ID | MIIM_STRING;
            mnuiteminfo.dwTypeData = const_cast<LPTSTR>(g_oResStr[IDS_STRING499]);
            mnuiteminfo.hSubMenu = menuAreaNo.GetSafeHmenu();
            mnuiteminfo.wID = ID_SET_SENSOR_SCREEN_AREA_NO;
            pSubMenu->InsertMenuItem(pSubMenu->GetMenuItemCount(), &mnuiteminfo, TRUE);

            int nSensorCount = m_oIWBSensorManager.GetSensorCount();
            int nCurrentSensorId = pSensor->GetID();

           const SplitMode& splitMode = m_oIWBSensorManager.GetScreenLayoutDesigner().GetScreenLayout().GetSplitMode();
           int nAreaCount = splitMode.rows * splitMode.cols;

            for (int idx = 0; idx < nAreaCount; idx++)
            {

 
                CString strID;
                strID.Format(_T("%d"), idx + 1);

                mnuiteminfo.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID | MIIM_CHECKMARKS;

                mnuiteminfo.fType = MFT_STRING;
                mnuiteminfo.dwTypeData = const_cast<LPTSTR>(strID.GetString());
                mnuiteminfo.wID = ID_SET_SENSOR_SCREEN_AREA_1 + idx;


                menuAreaNo.InsertMenuItem(menuAreaNo.GetMenuItemCount(), &mnuiteminfo, TRUE);
                if (pSensor->GetScreenAreaNo() == idx)
                {
                     menuAreaNo.CheckMenuItem(mnuiteminfo.wID, MF_BYCOMMAND | MF_CHECKED);
                }

            }



        }
        //显示传感器快捷菜单
        pSubMenu->TrackPopupMenu(
            TPM_LEFTALIGN|TPM_TOPALIGN,
            point.x,
            point.y,
            this,
            NULL);
    }

}

void CIWBDlg::OnCaptureChanged(CWnd *pWnd)
{
    CDialog::OnCaptureChanged(pWnd);
}

void CIWBDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    CIWBSensor*  pSensor = this->m_oIWBSensorManager.SensorFromPt(point);
    if (pSensor == NULL) return;

    m_oIWBSensorManager.SelectAsCurrentSensor(pSensor);
    CDialog::OnRButtonUp(nFlags, point);
}

void CIWBDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    TRACE(_T("OnLButtonDown\r\n"));
    CIWBSensor*  pSensor = this->m_oIWBSensorManager.SensorFromPt(point);
    if(pSensor == NULL) return;
    
    m_oIWBSensorManager.SelectAsCurrentSensor(pSensor);

    if(pSensor->IsDetecting())
    {//只有在运行时才能够手动编辑屏蔽图
        RECT rcDisplayArea = pSensor->GetVideoPlayer()->GetDisplayArea();
        int nPlayWndWidth  = rcDisplayArea.right  - rcDisplayArea.left;
        int nPlayWndHeight = rcDisplayArea.bottom - rcDisplayArea.top ;
        int nSrcImgWidth   = pSensor->GetPenPosDetector()->GetSrcImageWidth ();
        int nSrcImgHeight  = pSensor->GetPenPosDetector()->GetSrcImageHeight();

        switch(this->m_eScreenMaskAreaEditMode)
        {
        case e_SCREEN_MASK_AREA_EDIT_MODE_IDLE://空闲状态
            break;

            //case e_SCREEN_MASK_AREA_EDIT_MODE_SELECT_SCREEN_AREA://屏幕区域选择
            //    this->m_ptScreenAreaStart = point;
            //    m_bScreenMaskAreaSelectDragging = TRUE;
            //    break;

        case e_SCREEN_MASK_AREA_EDIT_MODE_ADD_MASK://遮蔽区域添加
            {

                CRect rcAddArea;
                rcAddArea.left   = point.x - this->m_FillSize.cx/2;
                rcAddArea.right  = point.x + this->m_FillSize.cx/2;
                rcAddArea.top    = point.y - this->m_FillSize.cy/2;
                rcAddArea.bottom = point.y + this->m_FillSize.cy/2;

                rcAddArea.left   = rcAddArea.left   * nSrcImgWidth  / nPlayWndWidth ;
                rcAddArea.right  = rcAddArea.right  * nSrcImgWidth  / nPlayWndWidth ;
                rcAddArea.top    = rcAddArea.top    * nSrcImgHeight / nPlayWndHeight;
                rcAddArea.bottom = rcAddArea.bottom * nSrcImgHeight / nPlayWndHeight;

                //增加静态屏蔽区
                pSensor->GetPenPosDetector()->AddStaticMaskArea(&rcAddArea);

                //
                m_tMaskEditAction.vecArea.clear();
                m_tMaskEditAction.eScreenMaskAreaEditActionType = e_SCREEN_MASK_AREA_EDIT_ACTION_TYPE_FILL;
                m_tMaskEditAction.vecArea.push_back(rcAddArea);
            }
            break;

        case e_SCREEN_MASK_AREA_EDIT_MODE_ERASE_MASK://遮蔽区域删除
            {
                CRect rcEraseArea;
                rcEraseArea.left   = point.x - this->m_EraseSize.cx/2;
                rcEraseArea.right  = point.x + this->m_EraseSize.cx/2;
                rcEraseArea.top    = point.y - this->m_EraseSize.cy/2;
                rcEraseArea.bottom = point.y + this->m_EraseSize.cy/2;

                rcEraseArea.left   = rcEraseArea.left   * nSrcImgWidth  / nPlayWndWidth;
                rcEraseArea.right  = rcEraseArea.right  * nSrcImgWidth  / nPlayWndWidth;
                rcEraseArea.top    = rcEraseArea.top    * nSrcImgHeight / nPlayWndHeight;
                rcEraseArea.bottom = rcEraseArea.bottom * nSrcImgHeight / nPlayWndHeight;

                //当次操作的"矩形区域列表"清空
                m_tMaskEditAction.vecArea.clear();
                m_tMaskEditAction.eScreenMaskAreaEditActionType = e_SCREEN_MASK_AREA_EDIT_ACTION_TYPE_ERASE;
                m_tMaskEditAction.vecArea.push_back(rcEraseArea);
            }

            break;

        }//switch
    }

    CDialog::OnLButtonDown(nFlags, point);
}

void CIWBDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    TRACE(_T("OnLButtonUp\r\n"));

    switch(this->m_eScreenMaskAreaEditMode)
    {
       case e_SCREEN_MASK_AREA_EDIT_MODE_IDLE:
            break;

     //case e_SCREEN_MASK_AREA_EDIT_MODE_SELECT_SCREEN_AREA:
     //    this->m_eScreenMaskAreaEditMode = e_SCREEN_MASK_AREA_EDIT_MODE_IDLE;
        //break;

       case e_SCREEN_MASK_AREA_EDIT_MODE_ADD_MASK:
           this->m_stackUndo.push(this->m_tMaskEditAction);
           break;

       case e_SCREEN_MASK_AREA_EDIT_MODE_ERASE_MASK:
            this->m_stackUndo.push(this->m_tMaskEditAction);
            break;
    }//swtich

    if (m_bStartDrawOnlineScreenArea)
    {
        CIWBSensor*  pSensor = this->m_oIWBSensorManager.GetSensor();
        if (pSensor == NULL) return;

        if (pSensor->IsDetecting())
        {
            RECT rcDisplayArea = pSensor->GetVideoPlayer()->GetDisplayArea();
            int nPlayWndWidth = rcDisplayArea.right - rcDisplayArea.left;
            int nPlayWndHeight = rcDisplayArea.bottom - rcDisplayArea.top;
            int nSrcImgWidth = pSensor->GetPenPosDetector()->GetSrcImageWidth();
            int nSrcImgHeight = pSensor->GetPenPosDetector()->GetSrcImageHeight();
            CPoint pt;
            pt.x = (point.x  - rcDisplayArea.left) * nSrcImgWidth /nPlayWndWidth;
            pt.y = (point.y  - rcDisplayArea.top ) * nSrcImgHeight /nPlayWndHeight;

            pSensor->GetPenPosDetector()->SetCurrentOnLineScreenAreaPt(pt);
        }
    }

    CDialog::OnLButtonUp(nFlags, point);
}

void CIWBDlg::OnMouseMove(UINT nFlags, CPoint point)
{

    // TODO: Add your message handler code here and/or call default
    //TRACE(_T("OnMouseMove\r\n"));
    CIWBSensor*  pSensor = this->m_oIWBSensorManager.SensorFromPt(point);
    if(pSensor == NULL) return;

    RECT rcDisplayArea = pSensor->GetVideoPlayer()->GetDisplayArea();
    int nPlayWndWidth  = rcDisplayArea.right  - rcDisplayArea.left;
    int nPlayWndHeight = rcDisplayArea.bottom - rcDisplayArea.top ;
    int nSrcImgWidth   = pSensor->GetPenPosDetector()->GetSrcImageWidth ();
    int nSrcImgHeight  = pSensor->GetPenPosDetector()->GetSrcImageHeight();

    point.x -= rcDisplayArea.left;
    point.y -= rcDisplayArea.top ;


    if(nFlags & MK_LBUTTON)//左键按下
    {
        switch(this->m_eScreenMaskAreaEditMode)
        {
        case e_SCREEN_MASK_AREA_EDIT_MODE_IDLE:
            break;

            //case e_SCREEN_MASK_AREA_EDIT_MODE_SELECT_SCREEN_AREA:
            //    if(this->m_bScreenMaskAreaSelectDragging)
            //    {

            //        CRect rcSelectArea;
            //        this->m_ptScreenAreaEnd = point;
            //        UpdateScreenMaskArea();
            //    }
            // break;

        case e_SCREEN_MASK_AREA_EDIT_MODE_ADD_MASK:
            {
                CRect rcAddArea;
                rcAddArea.left   = point.x - this->m_FillSize.cx/2;
                rcAddArea.right  = point.x + this->m_FillSize.cx/2;
                rcAddArea.top    = point.y - this->m_FillSize.cy/2;
                rcAddArea.bottom = point.y + this->m_FillSize.cy/2;

                rcAddArea.left  = rcAddArea.left   * nSrcImgWidth  / nPlayWndWidth;
                rcAddArea.right = rcAddArea.right  * nSrcImgWidth  / nPlayWndWidth;
                rcAddArea.top   = rcAddArea.top    * nSrcImgHeight / nPlayWndHeight;
                rcAddArea.bottom= rcAddArea.bottom * nSrcImgHeight / nPlayWndHeight;

                //增加遮蔽区域
                pSensor->GetPenPosDetector()->AddStaticMaskArea(&rcAddArea);

                m_tMaskEditAction.vecArea.push_back(rcAddArea);
            }
            break;

        case e_SCREEN_MASK_AREA_EDIT_MODE_ERASE_MASK:
            {
                CRect rcEraseArea;
                rcEraseArea.left   = point.x - this->m_EraseSize.cx/2;
                rcEraseArea.right  = point.x + this->m_EraseSize.cx/2;
                rcEraseArea.top    = point.y - this->m_EraseSize.cy/2;
                rcEraseArea.bottom = point.y + this->m_EraseSize.cy/2;

                rcEraseArea.left   = rcEraseArea.left   * nSrcImgWidth  / nPlayWndWidth;
                rcEraseArea.right  = rcEraseArea.right  * nSrcImgWidth  / nPlayWndWidth;
                rcEraseArea.top    = rcEraseArea.top    * nSrcImgHeight / nPlayWndHeight;
                rcEraseArea.bottom = rcEraseArea.bottom * nSrcImgHeight / nPlayWndHeight;

                pSensor->GetPenPosDetector()->EraseSaticMaskArea(&rcEraseArea);
                m_tMaskEditAction.vecArea.push_back(rcEraseArea);

            }    
            break;

        }//switch
    }//if

    ////////////在移动的时候把把最后一个点传递过去就好了。
    if (m_bStartDrawOnlineScreenArea)
    {
        if (pSensor->IsDetecting())
        {
            RECT rcDisplayArea = pSensor->GetVideoPlayer()->GetDisplayArea();
            int nPlayWndWidth = rcDisplayArea.right - rcDisplayArea.left;
            int nPlayWndHeight = rcDisplayArea.bottom - rcDisplayArea.top;
            int nSrcImgWidth = pSensor->GetPenPosDetector()->GetSrcImageWidth();
            int nSrcImgHeight = pSensor->GetPenPosDetector()->GetSrcImageHeight();
            CPoint pt;
            pt.x = point.x *nSrcImgWidth / nPlayWndWidth;
            pt.y = point.y* nSrcImgHeight / nPlayWndHeight;

            pSensor->GetInterceptFilter()->SetDrawMovePt(pt);
        }
    }

    CDialog::OnMouseMove(nFlags, point);
}


BOOL CIWBDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    // TODO: Add your message handler code here and/or call default

    if(nHitTest == HTCLIENT)
    {
        HCURSOR hCursor = NULL;
        switch(this->m_eScreenMaskAreaEditMode)
        {
        case e_SCREEN_MASK_AREA_EDIT_MODE_IDLE:
            hCursor = m_hArrowCursor;
            break;

      case e_SCREEN_MASK_AREA_EDIT_MODE_ADD_MASK:
            hCursor = m_hCurrentEditToolCursor;
            break;

        case e_SCREEN_MASK_AREA_EDIT_MODE_ERASE_MASK:
            hCursor = m_hCurrentEditToolCursor;
            break;

        }//switch
        int nDebug = 0;
        if (hCursor)
        {
            SetCursor(hCursor);
            return TRUE;
        }
        else
        {
            nDebug = 1;
        }
    }
    return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CIWBDlg::OnCancel()
{
    // TODO: Add your specialized code here and/or call the base class
    //CancelScreenAreadEditMode();
    LOG_INF("CIWBDlg::OnCancel()");
    CDialog::OnCancel();
}


//@功能:取消屏幕矩形区域编辑模式
void CIWBDlg::CancelScreenAreadEditMode()
{
    /*
    CPenPosDetector* pPenPosDetector = m_oFilterGraphBuilder.GetPenPosDetector();
    */
    //if(pPenPosDetector != NULL)
    //{
    // if(pPenPosDetector->GetManualScreenAreaMode() == E_ManualScreenAreaEditMode)
    // {
    // //取消手动指定屏幕区域模式
    // pPenPosDetector->SetManualScreenAreaMode(E_ManualScreenAreaNormalMode);
    // ReleaseCapture();
    // m_bScreenMaskAreaSelectDragging = FALSE;
    // SetCursor(m_hArrowCursor);
    // }
    //}
}


//@功能:更新手动指定的屏幕区域
//void CIWBDlg::UpdateScreenMaskArea()
//{
//    /*
//    RECT rcNewArea;
//    rcNewArea.left = m_ptScreenAreaEnd.x > m_ptScreenAreaStart.x? m_ptScreenAreaStart.x : m_ptScreenAreaEnd.x ;
//    rcNewArea.right = m_ptScreenAreaEnd.x > m_ptScreenAreaStart.x? m_ptScreenAreaEnd.x : m_ptScreenAreaStart.x ;
//    rcNewArea.top = m_ptScreenAreaEnd.y > m_ptScreenAreaStart.y? m_ptScreenAreaStart.y : m_ptScreenAreaEnd.y ;
//    rcNewArea.bottom = m_ptScreenAreaEnd.y > m_ptScreenAreaStart.y? m_ptScreenAreaEnd .y : m_ptScreenAreaStart.y ;
//
//
//    CPenPosDetector* pPenPosDetector = m_oFilterGraphBuilder.GetPenPosDetector();
//    if(pPenPosDetector != NULL)
//    {
//
//    CRect rcClient;
//    this->GetClientRect(&rcClient);
//
//    rcNewArea.left = rcNewArea.left * 640 / rcClient.Width();
//    rcNewArea.right = rcNewArea.right * 640 / rcClient.Width();
//    rcNewArea.top = rcNewArea.top * 480 / rcClient.Height();
//    rcNewArea.bottom = rcNewArea.bottom * 480 / rcClient.Height();
//
//    pPenPosDetector->UpdateScreenMaskArea(rcNewArea);
//
//    }
//    */
//
//
//}

BOOL CIWBDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if(pMsg->message == WM_KEYDOWN
        &&
        (pMsg->hwnd == GetSafeHwnd() || pMsg->hwnd == this->m_ctlStatusBar.GetSafeHwnd())
        )
    {
        //TRACE(_T("WM_KEYDOWN\r\n"));

        if(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
        {
            //按下Esc键，立即取消手动指定屏幕区域模式
            if(pMsg->wParam == VK_ESCAPE)
            {
                //CancelScreenAreadEditMode();
                OnEndManualMaskAreaEdit();
            }
            return TRUE;//Eat this message
        }

    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CIWBDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    // TODO: Add your message handler code here and/or call default

    if(nIDCtl == NULL //is menu
        &&
        lpDrawItemStruct->CtlType == ODT_MENU
        )//
    {
        this->m_oOwnerDrawMenu.OnDrawMenuItem(lpDrawItemStruct);

    }

    CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CIWBDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    // TODO: Add your message handler code here and/or call default
    if(nIDCtl == NULL && lpMeasureItemStruct->CtlType == ODT_MENU )// is menu
    {
        this->m_oOwnerDrawMenu.OnMeasureItem(lpMeasureItemStruct);

    }

    CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}


//@功能:生成自绘光标
void CIWBDlg::CreateOwnerCursor()
{

    int nCxCursor; //= GetSystemMetrics(SM_CXCURSOR);
    int nCyCursor;// = GetSystemMetrics(SM_CYCURSOR);
    //RECT rcSize;
    for(int i=0; i<_countof(m_aryCursor); i++)
    {

        switch(i)
        {
        case e_CURSOR_BRUSH_1X:
            nCxCursor = 16;
            nCyCursor = 16;
            m_aryCursor[i].CreateSolidRectangleCursor(
                nCxCursor,
                nCyCursor,
                RGB(0,255,0),
                RGB(0,255,0),
                0);

            break;

        case e_CURSOR_BRUSH_1D5X:
            nCxCursor = 24;
            nCyCursor = 24;
            m_aryCursor[i].CreateSolidRectangleCursor(
                nCxCursor,
                nCyCursor,
                RGB(0,255,0),
                RGB(0,255,0),
                0);

            DbgCursor(HCURSOR(m_aryCursor[i]));
            break;

        case e_CURSOR_BRUSH_2X:
            nCxCursor = 32;
            nCyCursor = 32;
            m_aryCursor[i].CreateSolidRectangleCursor(
                nCxCursor,
                nCyCursor,
                RGB(0,255,0),
                RGB(0,255,0),
                0);
            break;

        case e_CURSOR_BRUSH_3X:
            nCxCursor = 48;
            nCyCursor = 48;
            m_aryCursor[i].CreateSolidRectangleCursor(
                nCxCursor,
                nCyCursor,
                RGB(0,255,0),
                RGB(0,255,0),
                0);
            break;

        case e_CURSOR_ERASE_1X:
            nCxCursor = 16;
            nCyCursor = 16;
            m_aryCursor[i].CreateSolidRectangleCursor(
                nCxCursor,
                nCyCursor,
                RGB(255,255,255),
                RGB(0,0,0),
                0);
            break;

        case e_CURSOR_ERASE_1D5X:
            nCxCursor = 24;
            nCyCursor = 24;
            m_aryCursor[i].CreateSolidRectangleCursor(
                nCxCursor,
                nCyCursor,
                RGB(255,255,255),
                RGB(0,0,0),
                0);
            break;

        case e_CURSOR_ERASE_2X:
            nCxCursor = 32;
            nCyCursor = 32;
            m_aryCursor[i].CreateSolidRectangleCursor(
                nCxCursor,
                nCyCursor,
                RGB(255,255,255),
                RGB(0,0,0),
                0);
            break;

        case e_CURSOR_ERASE_3X:
            nCxCursor = 48;
            nCyCursor = 48;
            m_aryCursor[i].CreateSolidRectangleCursor(
                nCxCursor,
                nCyCursor,
                RGB(255,255,255),
                RGB(0,0,0),
                0);
            break;
        }//switch

    }//for


}

void CIWBDlg::OnAddMaskArea(UINT uID)
{
    m_eScreenMaskAreaEditMode = e_SCREEN_MASK_AREA_EDIT_MODE_ADD_MASK;
    switch(uID)
    {
    case ID_ADD_MASK_RECTANGLE_1X:
        this->m_FillSize.cx = 16;
        this->m_FillSize.cy = 16;
        m_hCurrentEditToolCursor = m_aryCursor[e_CURSOR_BRUSH_1X];
        break ;

    case ID_ADD_MASK_RECTANGLE_1D5X:
        this->m_FillSize.cx = 24;
        this->m_FillSize.cy = 24;
        m_hCurrentEditToolCursor = m_aryCursor[e_CURSOR_BRUSH_1D5X];
        break;

    case ID_ADD_MASK_RECTANGLE_2X:
        //this->m_FillSize.cx = 32;
        //this->m_FillSize.cy = 32;
        //尺寸减半
        this->m_FillSize.cx = 16;
        this->m_FillSize.cy = 16;

        m_hCurrentEditToolCursor = m_aryCursor[e_CURSOR_BRUSH_2X];
        break;

    case ID_ADD_MASK_RECTANGLE_3X:
        this->m_FillSize.cx = 48;
        this->m_FillSize.cy = 48;
        m_hCurrentEditToolCursor = m_aryCursor[e_CURSOR_BRUSH_3X];
        break;
    }


}

void CIWBDlg::OnEraseMaskArea(UINT uID)
{
    m_eScreenMaskAreaEditMode = e_SCREEN_MASK_AREA_EDIT_MODE_ERASE_MASK;
    switch(uID)
    {
    case ID_ERASE_MASK_RECTANGLE_1X:
        this->m_EraseSize.cx = 16;
        this->m_EraseSize.cy = 16;
        m_hCurrentEditToolCursor = m_aryCursor[e_CURSOR_ERASE_1X];
        break;

    case ID_ERASE_MASK_RECTANGLE_1D5X:
        this->m_EraseSize.cx = 24;
        this->m_EraseSize.cy = 24;
        m_hCurrentEditToolCursor = m_aryCursor[e_CURSOR_ERASE_1D5X];
        
        break;

    case ID_ERASE_MASK_RECTANGLE_2X:
        //this->m_EraseSize.cx = 32;
        //this->m_EraseSize.cy = 32;
        //尺寸减半
        this->m_EraseSize.cx = 16;
        this->m_EraseSize.cy = 16;
        m_hCurrentEditToolCursor = m_aryCursor[e_CURSOR_ERASE_2X];
        break;

    case ID_ERASE_MASK_RECTANGLE_3X:
        this->m_EraseSize.cx = 48;
        this->m_EraseSize.cy = 48;
        m_hCurrentEditToolCursor = m_aryCursor[e_CURSOR_ERASE_3X];
        break;

    }

    //SetCapture();
    //SetCursor(m_hCurrentEditToolCursor);
}
void CIWBDlg::OnEndManualMaskAreaEdit()//结束编辑
{
    // TODO: Add your command handler code here
    m_eScreenMaskAreaEditMode = e_SCREEN_MASK_AREA_EDIT_MODE_IDLE;

    ////清空重做和撤销堆栈
    while(!m_stackRedo.empty()) m_stackRedo.pop();
    while(!m_stackUndo.empty()) m_stackUndo.pop();

    //保存屏蔽图

//    POINT ptCursor;
//    GetCursorPos(&ptCursor);

//    CIWBSensor*  pSensor = this->m_oIWBSensorManager.SensorFromPt(ptCursor);
//    if(pSensor == NULL) return;

    CIWBSensor*  pSensor = this->m_oIWBSensorManager.GetSensor();


    pSensor->GetPenPosDetector()->SaveStaticMaskFrame();

}

void CIWBDlg::OnClearMaskArea()
{
    // TODO: Add your command handler code here
    //CPenPosDetector* pPenPosDetector = m_oFilterGraphBuilder.GetPenPosDetector();
    //if(pPenPosDetector != NULL)
    //{
    //    pPenPosDetector->ClearStaticMaskArea();
    //}

}


LRESULT CIWBDlg::OnHotKey(WPARAM wParam,LPARAM lParam)
{
    if(wParam == m_atomLaserPwrOffHotkey && LOWORD(lParam) == MOD_CONTROL && HIWORD(lParam) == '8')
    {
        //<<tmp
        //g_oMouseEventGen.SetCtrlPenSrc(E_CTRL_PEN_LIGHT_SOURCE_INFRARED);
        //tmp>>
        TRACE(_T("Ctrl + 8\n"));

    }
    else if(wParam == m_atomLaserPwrOnHotkey && LOWORD(lParam) == MOD_CONTROL && HIWORD(lParam) == '9')
    {
        //<<tmp
        //g_oMouseEventGen.SetCtrlPenSrc(E_CTRL_PEN_LIGHT_SOURCE_RED_LASER);
        //tmp>>
        TRACE(_T("Ctrl + 9\n"));
    }

    return S_OK;
}




void CIWBDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: Add your message handler code here
    LOG_INF("CIWBDlg::OnDestroy");
}
void CIWBDlg::OnNcDestroy()
{
    CDialog::OnNcDestroy();

    // TODO: Add your message handler code here
    UnregisterHotKey(this->GetSafeHwnd(), this->m_atomLaserPwrOffHotkey);
    UnregisterHotKey(this->GetSafeHwnd(), this->m_atomLaserPwrOnHotkey);

    if(m_pRawInputBuf)
    {
        free(m_pRawInputBuf);
    }

}

//void CIWBDlg::Undo()
//{
//
//
//}
//
//void CIWBDlg::Redo()
//{
//
//}

void CIWBDlg::OnMaskAreaEditActionRedo()
{
    // TODO: Add your command handler code here
//    Redo();
}

void CIWBDlg::OnMaskAreaEditActionUndo()
{
    // TODO: Add your command handler code here
//    Undo();
}


void CIWBDlg::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class
    LOG_INF("CIWBDlg::OnOK()");
    CDialog::OnOK();
}

void CIWBDlg::OnRawInput(UINT nInputcode, HRAWINPUT hRawInput)
{
    // This feature requires Windows XP or greater.
    // The symbol _WIN32_WINNT must be >= 0x0501.
    // TODO: Add your message handler code here and/or call default
    //HRESULT hResult;
    UINT dwSize;

    GetRawInputData(hRawInput, RID_INPUT, NULL, &dwSize,
        sizeof(RAWINPUTHEADER));


    if(m_pRawInputBuf == NULL)
    {
        m_pRawInputBuf = (BYTE*)malloc(dwSize);
        m_nRawInputBufSize = dwSize;

    }
    else if(dwSize > m_nRawInputBufSize)
    {
        m_pRawInputBuf = (BYTE*)realloc(m_pRawInputBuf, dwSize);
        m_nRawInputBufSize = dwSize;

    }



    UINT dwRet = GetRawInputData(hRawInput, RID_INPUT, m_pRawInputBuf, &dwSize, sizeof(RAWINPUTHEADER));
    if (dwRet!= dwSize )
    {
        TRACE (_T("GetRawInputData does not return correct size !\n"));
    }

    RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(m_pRawInputBuf);

    if (raw->header.dwType == RIM_TYPEKEYBOARD)
    {

        if(raw->data.keyboard.VKey == 'P')
        {
            short P_state = GetAsyncKeyState('P');
            short ctrl_state = GetAsyncKeyState(VK_CONTROL);
            short shift_state = GetAsyncKeyState(VK_SHIFT);
            short alt_state = GetAsyncKeyState(VK_MENU);

            if((P_state & 0x8000) && (ctrl_state & 0x8000) && (shift_state & 0x8000 || alt_state & 0x8000))
            {
                //根据光笔的当前控制状态来禁用/使能光笔
                this->m_oIWBSensorManager.EnableOpticalPen(!this->m_oIWBSensorManager.IsOpticalPenControlling());
                TRACE(_T("Toggle Optical Pen Control Ctrl+Alt+P\n"));
            }

        }
        else if(raw->data.keyboard.VKey == 'S')
        {
            short S_state = GetAsyncKeyState('S');
            short ctrl_state = GetAsyncKeyState(VK_CONTROL);
            short shift_state = GetAsyncKeyState(VK_SHIFT);
            short alt_state = GetAsyncKeyState(VK_MENU);

            if((S_state & 0x8000) &&(ctrl_state & 0x8000) && (shift_state & 0x8000|| alt_state & 0x8000))
            {
                //{begin}
                //根据光笔的当前控制状态来禁用/使能光笔
                this->m_oIWBSensorManager.EnableOpticalPen(!this->m_oIWBSensorManager.IsOpticalPenControlling());
                //{end}

                TRACE(_T("Toggle Optical Pen Control Ctrl+Alt+S\n"));
            }

        }
        else if(raw->data.keyboard.VKey == 'A')
        {
            short A_state = GetAsyncKeyState('A');
            short ctrl_state = GetAsyncKeyState(VK_CONTROL);
            short shift_state = GetAsyncKeyState(VK_SHIFT);
            short alt_state = GetAsyncKeyState(VK_MENU);

            if((A_state & 0x8000) &&(ctrl_state & 0x8000) && (shift_state & 0x8000 || alt_state & 0x8000))
            {
                //g_oMouseEventGen.EnableOpticalPenControl(!g_oMouseEventGen.IsOpticalPenControlling());
                OnMenuAutoAddScreenMask();
                TRACE(_T("Auto Masking Ctrl+Alt+ A\n"));
            }
        }
        //else if(raw->data.keyboard.VKey == 'C')
        //{
        //    short C_state = GetAsyncKeyState('C');
        //    short ctrl_state = GetAsyncKeyState(VK_CONTROL);
        //    short shift_state = GetAsyncKeyState(VK_SHIFT);
        //    short alt_state = GetAsyncKeyState(VK_MENU);

        //    if((C_state & 0x8000) &&(ctrl_state & 0x8000) && (shift_state & 0x8000 || alt_state & 0x8000))
        //    {
        //        //OnMenuAutoCalibrate(ID_AUTOCALIBRATION_17X17GRIDS);

        //        m_AutoCalibrateColsType = DEFAULT_CALIBRATE_PATTERN;
        //        this->DoAutoCalibrate();
        //    }
        //}

    }

    CDialog::OnRawInput(nInputcode, hRawInput);
}

//@功能:按下菜单项"自动屏蔽干扰点"后作出的操作。
//
//      该操作包括先自动查找屏幕区域,而后再屏蔽干扰点。
//       其中,查找屏幕区域结束后,主窗体会接收到WM_SEARCH_SCREEN_AREA_DONE消息
//       
void CIWBDlg::OnMenuAutoAddScreenMask()
{
    // TODO: Add your command handler code here
    this->m_oIWBSensorManager.StartSearchMaskArea(this->GetSafeHwnd());
    
    /*
    CInterceptFilter* pFilter = m_oFilterGraphBuilder.GetInterceptFilter();

    if(pFilter)
    {
    CPenPosDetector * pPenPosDetector = m_oFilterGraphBuilder.GetPenPosDetector();
    if(pPenPosDetector)
    {
    pPenPosDetector->ClearStaticMaskArea();//清空屏蔽区		
    //关闭动态屏蔽
    pPenPosDetector->EnableDynamicMasking(FALSE);
    }

    //<<added by toxuke@gmail.com, 2013/05/12
    //禁用光笔
    g_oMouseEventGen.EnableOpticalPenControl(FALSE);
    //>>

    //采用自动校正时的摄像头参数。
    SetAutoCalibrationCameraParams();

    //通知IR CUT移除滤光片
   // ::IRCUTSwtich(m_oFilterGraphBuilder.GetCaptureFilter(),FALSE);


    //<<commented out by toxuke@gmail.com, 2014/02/24
    //this->m_oScreenAreaDetector.StartDetect(this->GetSafeHwnd());
    //pFilter->AddEventSink(this->m_oScreenAreaDetector);
    //>>
    //期望的画面平均亮度
    int nExpectedBrightness = PROFILE::GetAutoCalibrateImageAvgBrightness();

    //使用自动校正的自动屏蔽功能
    m_oAutoCalibrator.StartCalibrating(
    this->GetSafeHwnd(),
    E_CALIBRATE_SEARCH_SCREEN_AREA_ONLY,//只搜索屏幕区域模式
    nExpectedBrightness, 
    OnAutoCalibChangeCameraParams, 
    this,
    RGB(255,255,255),//该参数对自动屏蔽功能不起作用
    (ECalibDebugLevel)PROFILE::GetDebugLevel(), 
    PROFILE::IsSaveIntermidateFile(),
    m_bIsRecordVideo);

    g_oMouseEventGen.EnterCalibrateMode(m_oAutoCalibrator.GetCalibrateHWnd(), CALIBRATE_MODE_AUTO);
    }
    */

}


HRESULT CIWBDlg::OnAutoCalibrateDone(WPARAM wParam, LPARAM lParam)
{

    EAutoCalibrateError eError = (EAutoCalibrateError)wParam;//校正成功标志
    BOOL bSimulateMode = BOOL(lParam);

    this->m_oIWBSensorManager.OnIWBSensorAutoCalibrateDone(
          E_AUTO_CALIBRATOR_OK == eError,
          bSimulateMode);
    
    if(m_oIWBSensorManager.IsCalibarateOk())
    {

        CIWBSensor* pSensor = this->m_oIWBSensorManager.GetSensor();
        if (pSensor)
        {
            const TCaptureDeviceInstance& devInfo = pSensor->GetDeviceInfo();
            if (!devInfo.m_strDevPath.IsEmpty())
            {
                this->SaveConfig();
            }

        }

//        //保存配置信息
//        this->SaveConfig();

        //<<added by Jiqw 2015/04/15
        //<<added reason:当处于安装调试模式时，自动定位后不最小化到托盘
        if (false == m_tipProcServer.IsStarted())
        {
            //最小化到托盘
            MinimizeToTray();
        }        
        //>>

        //延时100ms后再作"屏幕内干扰光斑"的静态屏蔽
        //SetTimer(TIMER_AUTO_CLUTTER_DETECTION_AFTER_AUTO_CALIBRATION, 100, NULL);
    }
    else
    {
        //20170921 自动校正失败, 无条件弹出主窗体,显示校正失败
        m_oTray.Remove();
        m_bVisible = TRUE;
        this->ShowWindow(SW_SHOW);
    }
    m_tipProcServer.OnAutoCalibrateDone(E_AUTO_CALIBRATOR_OK == eError);

    return 0L;
}

//@功能;接收到WM_SEARCH_SCREEN_AREA_DONE消息后的响应函数
// 在该响应函数中, 如果屏幕区域搜索成功, 则开启定位程序的杂波屏蔽功能
//
LRESULT CIWBDlg::OnSearchScreenAreaDone(WPARAM wParam, LPARAM lParam)
{
    BOOL bSuccess = (BOOL)lParam;

    //wParam, 保留暂时不用
    this->m_oIWBSensorManager.OnIWBSensorSearchMaskAreaDone(bSuccess);
    return 0L;
}



//void CIWBDlg::OnMenuAutoCalibrationWithHumanIntervention()
//{
//    m_bNeedHumanIntervention = TRUE;
//
//    //开启自动校正
//    OnMenuAutoCalibrate(ID_AUTOCALIBRATION_11X11GRIDS);
//}



LRESULT CIWBDlg::OnInitialStateDisableOpticalPenForClutter(WPARAM, LPARAM lParam)
{

    /*
    if(g_oMouseEventGen.IsOpticalPenControlling())
    {
    g_oMouseEventGen.EnableOpticalPenControl(FALSE);

    MessageBox(g_oResStr[IDS_STRING126], g_oResStr[107], MB_ICONINFORMATION|MB_OK);
    }
    */

    //显示窗体提示有大的干扰光斑存在
    m_bVisible = TRUE;    
    this->ShowWindow(SW_SHOW);

    return 0;
}

LRESULT CIWBDlg::OnStateDisableOpticalPenForClutter(WPARAM, LPARAM lParam)
{
    /*
    if(g_oMouseEventGen.IsOpticalPenControlling())
    {
    g_oMouseEventGen.EnableOpticalPenControl(FALSE);
    MessageBox(g_oResStr[IDS_STRING434], g_oResStr[107], MB_ICONINFORMATION|MB_OK);

    }
    */

    return 0 ;
}

LRESULT CIWBDlg::OnChangeGuidelineYPos(WPARAM wParam, LPARAM lParam)
{
    /*
    CPenPosDetector* pPenPosDetector = m_oFilterGraphBuilder.GetPenPosDetector();
    if(pPenPosDetector)
    {
    if(wParam == 0)
    {
    pPenPosDetector->SetFirstGuidelineYPos(int(lParam & 0x0000FFFF));
    }
    else if(wParam == 1)
    {
    pPenPosDetector->SetSecondGuidelineYPos(int(lParam & 0x0000FFFF));
    }

    }*/

    return 0;
}

LRESULT CIWBDlg::OnShowGuideline(WPARAM wParam, LPARAM lParam)
{
    /*
    CPenPosDetector* pPenPosDetector = m_oFilterGraphBuilder.GetPenPosDetector();
    if(pPenPosDetector)
    {
    if(wParam == 0)
    {
    pPenPosDetector->ShowFirstGuideLine(lParam);
    }
    else if(wParam == 1)
    {
    pPenPosDetector->ShowSecondGuideLine(lParam);
    }
    }*/

    return 0;
}
///////////////////响应手动采集光斑的菜单
/////////////////////////////////////////
void CIWBDlg::OnSpotCollectionSetting()
{
    // TODO: Add your command handler code here
    /*
    if (m_oCollectSpotWnd.m_hWnd ==NULL)
    {
    m_oCollectSpotWnd.Create();
    m_oCollectSpotWnd.SetOwnerWnd(this->GetSafeHwnd());
    }
    g_oMouseEventGen.EnterSpotSamplingMode(m_oCollectSpotWnd.m_hWnd,COLLECTSPOT_MODE_COLLECT);
    m_oCollectSpotWnd.StartCollectSpotSize();
    */

    if(m_pSelectedSensor)
    {
        this->m_oIWBSensorManager.StartLightSpotSampling(this->GetSafeHwnd(), m_pSelectedSensor->GetID());
    }
}

LRESULT CIWBDlg::OnSpotCollectionSetting_Par(WPARAM wParam, LPARAM lParam)
{
    OnSpotCollectionSetting();
    return 0;

}
//////////////////响应模拟手动采集光斑的菜单
////////////////////////////////////////////
void CIWBDlg::OnSimulationManualCalibration()
{
    // TODO: Add your command handler code here
    //CSimulateCalibarationParamDlg dlg;
    //if (dlg.DoModal() == IDOK)
    //{
    // CPenPosDetector* pPenPosDetector = this->m_oFilterGraphBuilder.GetPenPosDetector();
    // if(pPenPosDetector == NULL)
    // {
    // return;
    // }
    // if (m_oCollectSpotWnd.m_hWnd == NULL)
    // {
    // m_oCollectSpotWnd.Create();
    // m_oCollectSpotWnd.SetOwnerWnd(this->GetSafeHwnd());
    // }
    // g_oMouseEventGen.EnterSpotSamplingMode(m_oCollectSpotWnd.m_hWnd,COLLECTSPOT_MODE_SIMULATECOLLECT);
    // pPenPosDetector->SimulateMannalCalibrate(dlg.GetAVIFilePath());
    //}
    /*
    CFileDialog fileDlg(TRUE, _T(".avi"), NULL, 4|2, _T("AVI FIle(*.avi)|*.avi"), this);

    if(fileDlg.DoModal() == IDOK)
    {
    CString strAVIFilePath = fileDlg.GetPathName();
    CInterceptFilter* pInterceptFilter = m_oFilterGraphBuilder.GetInterceptFilter();
    if(pInterceptFilter != NULL)
    {
    pInterceptFilter->InputFromAVIFile(strAVIFilePath);

    if (m_oCollectSpotWnd.m_hWnd == NULL)
    {
    m_oCollectSpotWnd.Create();
    m_oCollectSpotWnd.SetOwnerWnd(this->GetSafeHwnd());
    }
    g_oMouseEventGen.EnterSpotSamplingMode(m_oCollectSpotWnd.m_hWnd,COLLECTSPOT_MODE_SIMULATECOLLECT);

    }
    }
    */
}

//////////////////响应人为设置光斑的大小的菜单
/////////////////////////////////////////////
void CIWBDlg::OnSpotCalibrationSetting()
{
    // TODO: Add your command handler code here
    /*
    if (m_oCollectSpotWnd.m_hWnd == NULL)
    {
    m_oCollectSpotWnd.Create();
    m_oCollectSpotWnd.SetOwnerWnd(this->GetSafeHwnd());
    }

    g_oMouseEventGen.EnterSpotSamplingMode(m_oCollectSpotWnd.m_hWnd,COLLECTSPOT_MODE_MANUAL);
    //this->m_oFilterGraphBuilder.ResetVideoClipArea();

    m_oCollectSpotWnd.StartCollectSpotSize();
    */
}

///在没有设置完的情况下退出设置的界面的。
HRESULT CIWBDlg::OnBreakSpotSetting(WPARAM wParam, LPARAM lParam)
{
    int nSId = int(wParam);
    this->m_oIWBSensorManager.OnIWBSensorLightSpotSamplingDone(FALSE, nSId);

    return 0;

}

//@功能:完成设置后调用的函数。是有WM_FINISH_BOLBSETTING消息响应的。
HRESULT CIWBDlg::OnFinshSpotSetting(WPARAM wParam, LPARAM lParam)
{
    int nSId = int(lParam);
    this->m_oIWBSensorManager.OnIWBSensorLightSpotSamplingDone(TRUE, nSId);

    if(this->m_oIWBSensorManager.AllSamplingIsDone())
    {
        MinimizeToTray();
    }

    /*
    CPenPosDetector * pPenPosDetector = m_oFilterGraphBuilder.GetPenPosDetector();

    if (pPenPosDetector !=NULL)
    {
    pPenPosDetector->SetLightSpotSampleSize(m_oCollectSpotWnd.GetAllSampleSize());

    }
    g_oMouseEventGen.EnableOpticalPenControl(TRUE);
    //m_oFilterGraphBuilder.SetVideoClipArea(g_oMouseEventGen.m_oVideo2ScreenMap.GetVideoClipArea());

    g_oMouseEventGen.LeaveSpotSettingMode();
    MinimizeToTray();

    //     if (g_oMouseEventGen.GetCollectSpotMode() == COLLECTSPOT_MODE_COLLECT)
    //     {
    //         m_oCollectSpotWnd.m_oAVIRecorder.Close();
    //     }
    */
    return 0;
}

// HRESULT CIWBDlg::OnSetDetectSpot(WPARAM wParam, LPARAM lParam)
// {
// CPenPosDetector * pPenPosDetector = m_oFilterGraphBuilder.GetPenPosDetector();
//
// if (pPenPosDetector != NULL)
// {
// float m_Spot = (float)wParam/lParam;
// pPenPosDetector->SetSpotProportion1(m_Spot);
// }
// return 0;
// }

LRESULT CIWBDlg::OnChangeSpotProportion(WPARAM wParam, LPARAM lParam)
{
    /*
    CPenPosDetector* pPenposDetector = m_oFilterGraphBuilder.GetPenPosDetector();
    if (pPenposDetector)
    {
    pPenposDetector->SetSpotProportion(int(lParam & 0x0000FFFF));
    }
    */
    return 0;
}


LRESULT CIWBDlg::OnChangeMultEraser(WPARAM wParam, LPARAM lParam)
{
    /*
    CPenPosDetector* pPenposDetector = m_oFilterGraphBuilder.GetPenPosDetector();
    if (pPenposDetector)
    {
    pPenposDetector->SetMultiEraser(int(lParam & 0x0000FFFF));
    }
    */
    return 0;

}

LRESULT CIWBDlg::OnChangeFixedbolbsettime(WPARAM wParam, LPARAM lParam)
{
    /*
    CPenPosDetector* pPenposDetector = m_oFilterGraphBuilder.GetPenPosDetector();
    if (pPenposDetector)
    {
    pPenposDetector->SetFixedBlobSetTime(int(lParam & 0x0000FFFF));
    }
    */
    return 0 ;
}


/*
LRESULT CIWBDlg::OnDisPlayWindow(WPARAM wParam, LPARAM lParam)
{
    if (!theApp.IsStartFromAutoRun())
    {
        m_bVisible = TRUE;
        this->ShowWindow(SW_SHOW);
    }
    return 0;
}
*/

BOOL CIWBDlg::OnQueryEndSession()
{
    //From MSDN:
    //WM_QUERYENDSESSION
    //Each application should return TRUE or FALSE immediately upon receiving this message, 
    //and defer any cleanup operations until it receives the WM_ENDSESSION message.

    LOG_INF("OnQueryEndSession Begin.\n");
    /*
    if (!CDialog::OnQueryEndSession())
    {
        LOG_INF("OnQueryEndSession return false.\n");
        return FALSE;
    }*/
    return TRUE;



    //LOG_INF("OnQueryEndSession. Exit\n");

    return TRUE;
}

void CIWBDlg::OnEndSession(BOOL bEnding)
{
    //MSDN:
    //Applications that have unsaved data could save the data to a temporary location 
    //and restore it the next time the application starts.
    //It is recommended that applications save their data and state frequently; 
    //for example, automatically save data between save operations initiated 
    //by the user to reduce the amount of data to be saved at shutdown.
    //The application need not call the DestroyWindow or PostQuitMessage 
    //function when the session is ending.

    CDialog::OnEndSession(bEnding);
    
    // TODO:  Add your specialized query end session code here


    //PostMessage(WM_CLOSE, 0, 0);
    //此处如果使用PostMessage, 则不会调用
    //OnClose函数，而直接退出
    LOG_INF("OnQueryEndSession. Send WM_CLOSE\n");

    SendMessage(WM_CLOSE, 0, 0);
    // TODO: Add your message handler code here
    LOG_INF("CIWBDlg::OnEndSession. bEnding=%d\n", bEnding);
}

LRESULT CIWBDlg::OnGraphNotify(WPARAM wParam, LPARAM lParam)
{
    TRACE(_T("[OnGraphNotify]wParam=0x%x, lParam=0x%x\n"), wParam, lParam);
    return 0;
}

//=======2013/04/13=============
//Added by toxuke@gmail.com, 2013/04/03
//<<
//@功能:设置校正时摄像头参数
//void CIWBDlg::SetAutoCalibrationCameraParams()
//{
//    //摄像头Gamma值调整到校正时的最优Gamma值。
//    //this->m_oFilterGraphBuilder.ChangeGammaValue(PROFILE::GetAutoCalibrateOptimumGammaValue());
//    /*
//    TVideoProcAmpProperty cameraParams;
//    PROFILE::CameraParametersForAutoCalibration(cameraParams);
//    this->m_oFilterGraphBuilder.SetCameraParams(cameraParams);
//    m_eVideoParamsMode = E_VIDEO_PARAMS_MODE_AUTO_CALIBRATION;
//    */
//
//}

//@功能:设置正常使用时的摄像头参数
//void CIWBDlg::SetNormalCameraParams()
//{
//    //this->m_oFilterGraphBuilder.ChangeGammaValue(PROFILE::GetNormalOptimumGammaValue());
//    /*
//    TVideoProcAmpProperty cameraParams;
//
//    if (PROFILE::IsGestureRecognitionAvailable())
//    {
//    PROFILE::LoadCameraParamsForNormalUsageFP(cameraParams);
//    }
//    else
//    {
//    PROFILE::LoadCameraParamsForNormalUsage3D(cameraParams);
//    }
//
//    this->m_oFilterGraphBuilder.SetCameraParams(cameraParams);
//    m_eVideoParamsMode = E_VIDEO_PARAMS_MODE_NORMAL_USAGE;
//
//    PROFILE::SaveVideoParamsType(int(m_eVideoParamsMode));
//    */
//}

//@功能:设置调节挡板时的摄像头参数
//void CIWBDlg::SetAutoInstallationCameraParams1()
//{
//    /*
//    TVideoProcAmpProperty cameraParams;
//    PROFILE::CameraParametersForInstallation1(cameraParams);
//    this->m_oFilterGraphBuilder.SetCameraParams(cameraParams);
//    m_eVideoParamsMode = E_VIDEO_PARAMS_MODE_INTALLATION1;
//
//    PROFILE::SaveVideoParamsType(int(m_eVideoParamsMode));
//    */
//
//}
//@功能:设置添加屏蔽时的摄像头参数
//void CIWBDlg::SetAutoMaskCameraParams()
//{
//    //this->m_oFilterGraphBuilder.ChangeGammaValue(PROFILE::GetAutoMaskGammaValue());
//    /*
//    TVideoProcAmpProperty cameraParams;
//    PROFILE::CameraParametersForAutoMasking(cameraParams);
//    this->m_oFilterGraphBuilder.SetCameraParams(cameraParams);
//    m_eVideoParamsMode = E_VIDEO_PARAMS_MODE_AUTO_MASKING;
//    */
//}


//@功能:设置安装调试时的摄像头参数
//void CIWBDlg::SetAutoInstallationCameraParams()
//{
//    //this->m_oFilterGraphBuilder.ChangeGammaValue(PROFILE::GetAutoMaskGammaValue());
//    /*
//    TVideoProcAmpProperty cameraParams;
//    PROFILE::CameraParametersForInstallation(cameraParams);
//    this->m_oFilterGraphBuilder.SetCameraParams(cameraParams);
//    m_eVideoParamsMode = E_VIDEO_PARAMS_MODE_INTALLATION;
//
//    PROFILE::SaveVideoParamsType(int(m_eVideoParamsMode));
//    */
//}


//added by toxuke@gmail.com, 2013/04/03
LRESULT CIWBDlg::OnTaskbarRestartMsg(WPARAM wParam, LPARAM lParam)
{
    m_oTray.OnTaskbarCreated();
    return 0L;
}

LRESULT CIWBDlg::OnChangeNormalUserBrightness(WPARAM wParam, LPARAM lParam)
{
    return 0L;
}

LRESULT CIWBDlg::OnChangeAutoCalibrateAveBrightness(WPARAM wParam, LPARAM lParam)
{
    return 0L;
}

LRESULT CIWBDlg::OnChangeAutoCalibrateLightGray(WPARAM wParam, LPARAM lParam)
{
    return 0L;
}

LRESULT CIWBDlg::OnChangeVideoDisplayDelay(WPARAM wParam, LPARAM lParam)
{
    return 0L;
}


LRESULT CIWBDlg::OnRearProjection(WPARAM wParam, LPARAM lParam)
{
    m_bIsRearProjection = (BOOL)lParam;
    return 0L;
}


LRESULT CIWBDlg::OnChangeAutoMaskDetectThreshold(WPARAM wParam, LPARAM lParam)
{
    m_nAutoMaskDetectThreshold = int(lParam & 0x0000FFFF);
    return 0L;
}

//@功能:图像调试模式
void CIWBDlg::OnVideoTuningMode()
{
    // TODO: Add your command handler code here
    this->m_oIWBSensorManager.SwitchToMode(E_VIDEO_TUNING_MODE);

    //如果处于双屏拼接模式，则开启屏幕识别显示
    if(theApp.GetScreenMode() >= EScreenModeDouble)
    {
        //m_oScreenRecognition.DoRecoginition(&m_oIWBSensorManager);
        //m_uScreenRecognitionCloseTimer = SetTimer(TIMER_EVENT_SCREEN_RECOGNITION_CLOSE, SCREEN_RECOGNITION_CLOSE_TIMEOUT, NULL);

    }

}

//@功能:正常使用模式
void CIWBDlg::OnNormalUsageMode()
{
    // TODO: Add your command handler code here
    //如果正在绘制时切换到正常模式下，那么就需要退出绘制编辑状态，恢复正常模式状态
    if (m_bStartDrawOnlineScreenArea)
    {
        m_bStartDrawOnlineScreenArea = false;
        CIWBSensor* pSensor = this->m_oIWBSensorManager.GetSensor();
        if (pSensor)
        {
            pSensor->GetInterceptFilter()->SetStartDrawMaskFrame(false);
            pSensor->GetPenPosDetector()->SaveOnLineScreenArea();
            pSensor->GetPenPosDetector()->ShowGuideRectangle(m_bPreGuideRectangleVisible);
            pSensor->GetVideoPlayer()->ClearOSDText(E_OSDTEXT_TYPE_SHOW_INFO);
        }
    }

    this->m_oIWBSensorManager.SwitchToMode(E_NORMAL_USAGE_MODE);
    //如果处于双屏拼接模式，则开启屏幕识别显示
    if(theApp.GetScreenMode() >= EScreenModeDouble)
    {
        //m_oScreenRecognition.StopRecognition();
    }

}

//@功能:激光器调试模式
void CIWBDlg::OnLaserTunningModel()
{
    // TODO: Add your command handler code here
    //如果正在绘制时切换到激光器模式下，那么就需要退出绘制编辑状态，恢复激光器模式状态
    if (m_bStartDrawOnlineScreenArea)
    {
        m_bStartDrawOnlineScreenArea = false;
        CIWBSensor* pSensor = this->m_oIWBSensorManager.GetSensor();
        if (pSensor)
        {
            pSensor->GetInterceptFilter()->SetStartDrawMaskFrame(false);
            pSensor->GetPenPosDetector()->SaveOnLineScreenArea();
            pSensor->GetPenPosDetector()->ShowGuideRectangle(m_bPreGuideRectangleVisible);
            pSensor->GetVideoPlayer()->ClearOSDText(E_OSDTEXT_TYPE_SHOW_INFO);
        }
    }

    this->m_oIWBSensorManager.SwitchToMode(E_LASER_TUNING_MODE);

    if(theApp.GetScreenMode() >= EScreenModeDouble)
    {
        //m_oScreenRecognition.StopRecognition();
    }
}


LRESULT CIWBDlg::OnRecordvideo(WPARAM wParam, LPARAM lParam)
{
    m_bIsRecordVideo = (BOOL)lParam;
    return 0L;
}

void CIWBDlg::OnControlOn()
{
    // TODO: Add your command handler code here
}

void CIWBDlg::OnControlOff()
{
    // TODO: Add your command handler code here
}

void CIWBDlg::OnMenuManualCalibrate()
{
    // TODO: Add your command handler code here
    this->m_oIWBSensorManager.StartManualCalibrate(this->GetSafeHwnd(), -1, -1, -1);
}

void CIWBDlg::OnMenuManualCalibrate25()
{
    // TODO: Add your command handler code here
    this->m_oIWBSensorManager.StartManualCalibrate(this->GetSafeHwnd(), 5, 5, -1);
}

void CIWBDlg::OnMenuManualCalibrate36()
{
    // TODO: Add your command handler code here
    //36点校正
    this->m_oIWBSensorManager.StartManualCalibrate(this->GetSafeHwnd(), 6, 6, -1);
}


//静态函数
//自动校正过程中变更摄像头参数的回调函数
//
BOOL  CIWBDlg::OnAutoCalibChangeCameraParams(EChangeCalibCameraParams eCtrlMode, LPVOID lpCtx, BYTE param1,BYTE param2)
{
    return TRUE;
}

void CIWBDlg::OnCtxmenuAutoCalibrate()
{
    // TODO: Add your command handler code here
    //OnMenuAutoCalibrate(ID_AUTOCALIBRATION_17X17GRIDS);

    m_AutoCalibrateColsType  = DEFAULT_CALIBRATE_PATTERN;
   DoAutoCalibrate();
}

void CIWBDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    if(nHitTest == HTSYSMENU) return;
    CDialog::OnNcLButtonDblClk(nHitTest, point);
}

BOOL CIWBDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    return TRUE;
    return CDialog::OnEraseBkgnd(pDC);
}

void CIWBDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    ///双击响应的函数
    CIWBSensor* pSensor = this->m_oIWBSensorManager.GetSensor();
    if(pSensor)
    {
        if (m_bStartDrawOnlineScreenArea)
        {
            //m_nDrawOnlineAreaCount等于-1的话说明是单个屏绘制的，绘制完直接退出，如果不等于0说明不是单屏绘制，
            if(m_nDrawOnlineAreaCount == -1)
            {
                 m_bStartDrawOnlineScreenArea = false;
            }
            else
            {
                m_nDrawOnlineAreaCount++;
                if (m_nDrawOnlineAreaCount >= m_nActiveDetectCameraId)
                {
                    m_bStartDrawOnlineScreenArea = false;
                }
            }			
            ////加上引导框
            pSensor->GetInterceptFilter()->SetStartDrawMaskFrame(false);

            pSensor->GetPenPosDetector()->SaveOnLineScreenArea();
            pSensor->GetPenPosDetector()->ShowGuideRectangle(m_bPreGuideRectangleVisible);
            pSensor->GetVideoPlayer()->ClearOSDText(E_OSDTEXT_TYPE_SHOW_INFO);

        }
        else
        {
            char szBUf[128] = { 0 };
            sprintf(szBUf, " CIWBDlg OnLButtonDblClk pSensor:%p\n", pSensor);
            OutputDebugStringA(szBUf);
             OnAdvancedSettings(pSensor);
        }

        //CString strAdditionalCaption;

        //strAdditionalCaption.Format(_T("%s%d"), g_oResStr[IDS_STRING446], pSensor->GetID());

        //CAdvancedSettingsDlg dlg(pSensor->GetCfgData(), g_tSysCfgData.globalSettings,  this, strAdditionalCaption);

        //if(dlg.DoModal() == IDOK)
        //{
        //    pSensor->SetCfgData(dlg.m_tSensorConfig);           

        //}//if
    }

    CDialog::OnLButtonDblClk(nFlags, point);
}



//{begin}, 2014/07/09
//图像传感器快捷菜单处理函数
void CIWBDlg::OnSensorCtxMenu(UINT uID)
{
    if(this->m_pSelectedSensor == NULL) return;

    switch(uID)
    {
    case ID_SENSORCTXMENU_RUN:
        {
            RECT rcDispWnd;
            if (this->GetSafeHwnd() == m_hDispWnd)
            {
                RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0,reposQuery, &rcDispWnd, NULL, TRUE);
            }    
            else
            {
                ::GetClientRect(m_hDispWnd, &rcDispWnd);
            }
            //m_oIWBSensorManager.SetVideoDispParams(this->GetSafeHwnd(), this->GetSafeHwnd(), rcDispWnd);
            m_oIWBSensorManager.SetVideoDispParams(m_hDispWnd, this->GetSafeHwnd(), rcDispWnd);
            m_oIWBSensorManager.StartRunning(m_pSelectedSensor->GetID());

        }
        break;

    case ID_SENSORCTXMENU_STOP:
         m_oIWBSensorManager.StopRunning(m_pSelectedSensor->GetID());
        break;

    case ID_SENSORCTXMENU_DISABLE:
         m_pSelectedSensor->EnableOpticalPen(!m_pSelectedSensor->IsOpticalPenControlling());
        break;

    case ID_SENSORCTXMENU_AUTO_CALIBRATE:
         this->m_oIWBSensorManager.StartAutoCalibrate(m_AutoCalibrateColsType, this->GetSafeHwnd(), m_pSelectedSensor->GetID());
        break;

    case ID_SENSORCTXMENU_MANUAL_CALIBRATE:
         this->m_oIWBSensorManager.StartManualCalibrate(this->GetSafeHwnd(), -1, -1, m_pSelectedSensor->GetID());
        break;

    case ID_SENSORCTXMENU_FOURPOINTCALIBRATION:

         this->m_oIWBSensorManager.Start4BasePointMarking(this->GetSafeHwnd(), m_pSelectedSensor->GetID());
        break;


    case ID_SENSORCTXMENU_AUTOMASK:
         this->m_oIWBSensorManager.StartSearchMaskArea(this->GetSafeHwnd(), m_pSelectedSensor->GetID());
        break;

    case ID_SENSORCTXMENU_IMAGE_SENSOR_SETTINGS:
        {
            OnAdvancedSettings(m_pSelectedSensor);
        }
        break;

    case ID_SENSORCTXMENU_LIGHTSPOT_SAMPLING:
          this->m_oIWBSensorManager.StartLightSpotSampling(this->GetSafeHwnd(), m_pSelectedSensor->GetID());
        break;    

    case ID_SENSORCTXMENU_INSTALL_TIP:
        {            
            OnStartIntallerWithTips(m_pSelectedSensor);
        }
        break;
    case ID_SENSORCTXMENU_DRAWMASKFRAME_START:
    {
        m_bStartDrawOnlineScreenArea = true;
        m_nDrawOnlineAreaCount = -1 ;
        m_bPreGuideRectangleVisible =m_pSelectedSensor->GetPenPosDetector()->IsGuideRectangleVisible()?true:false;
        m_pSelectedSensor->OnStartDrawOnlineScreenArea();
    }
        break;

    case ID_SENSORCTXMENU_DRAWMASKFRAME_DISABLE:
        m_pSelectedSensor->OnEnableDrawOnlineScreenArea(!m_pSelectedSensor->IsEnableOnlineScreenArea());
        break;

    case ID_SENSORCTXMENU_DRAWMASKFRAME_CLEAR:
        m_pSelectedSensor->OnClearDrawOnlineScreenArea();
         m_bStartDrawOnlineScreenArea = false;
        break;

    }//switch

}

void CIWBDlg::OnGestureSettingMenu(UINT uID)
{

    if(this->m_pSelectedSensor == NULL) return;

    switch(uID)
    {
    case ID_GUESTURESETTINGS_GLBOARDGESTURESETTINGS:
        {   
            CGLBGRSettingDlg dlg;
            dlg.DoModal();
        }
        break;

    case ID_GUESTURESETTINGS_WINDOWSGESTURESETTINGS:
        {
            CWGRConfigDlg dlg;
            dlg.DoModal();
        }
        break;
    }
}


void CIWBDlg::OnOperationLightspotsampling()
{
    // TODO: Add your command handler code here
    //对所有屏幕进行光斑采样
    this->m_oIWBSensorManager.StartLightSpotSampling(this->GetSafeHwnd(), -1);
}

void CIWBDlg::OnInstallationanddebuggingMouse()
{
    // TODO: Add your command handler code here

    m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().SetHIDMode(E_DEV_MODE_MOUSE);

    //
    //<Added by Jiqw 201412041914
    //<Added Reason: 解决触屏模式下，windows两触点手势与windows下手势的冲突问题/>
    g_oWGRConfig.SetHIDMode(false);
    g_oGLBoardGR.SetIsTouchPadMode(false);
    //Added by Jiqw 201412041914>
}

void CIWBDlg::OnInstallationanddebuggingTouchpad()
{
    // TODO: Add your command handler code here
    m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().SetHIDMode(E_DEV_MODE_TOUCHSCREEN);

    //<Added by Jiqw 201412041914
    //<Added Reason: 解决触屏模式下，windows两触点手势与windows下手势的冲突问题/>
    g_oWGRConfig.SetHIDMode(true);
    g_oGLBoardGR.SetIsTouchPadMode(true);
    //Added by Jiqw 201412041914>
}


//<<added by toxuke@gmail.com, 2014/12/18
void CIWBDlg::OnAdvancedSettings(CIWBSensor* pSensor)
{
    if(pSensor == NULL) return;

    //设置对话框标题栏文字
    /*CString strAdditionalCaption;      
    strAdditionalCaption.Format(_T("%s%d"), g_oResStr[IDS_STRING446], pSensor->GetID() + 1);*/
    CParamsSettingPropertySheet paramsSettingSheet(const_cast<LPTSTR>(g_oResStr[IDS_STRING452]));

    paramsSettingSheet.SetIWBSensorInfo(pSensor, 1, theApp.GetScreenMode() >= EScreenModeDouble ?BST_CHECKED:BST_UNCHECKED);

    paramsSettingSheet.SetGlobalSettingInfo(g_tSysCfgData.globalSettings);

    //DOMODAL_RET ret = paramsSettingSheet.DoModal();

    DOMODAL_RET ret = paramsSettingSheet.DoModal();

    if(ret == IDOK)
    {
        pSensor->SetCfgData(paramsSettingSheet.GetSensorConfig(), &paramsSettingSheet.GetGlobalSettings());
        //保存配置文件
         g_tSysCfgData.vecSensorConfig[pSensor->GetID()] = paramsSettingSheet.GetSensorConfig();
         g_tSysCfgData.globalSettings = paramsSettingSheet.GetGlobalSettings();
        
        //写入配置文件
         const TCaptureDeviceInstance& devInfo = pSensor->GetDeviceInfo();
         if (!devInfo.m_strDevPath.IsEmpty())
         {
             int nCount = this->m_oIWBSensorManager.GetSensorCount();
            ::SaveConfig(PROFILE::CONFIG_FILE_NAME, ::g_tSysCfgData, nCount);
         }

         ////////////////////////////////////////////
         TSensorModeConfig* TSensorModeConfig = NULL;
         EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;

         TSensorModeConfig = &g_tSysCfgData.vecSensorConfig[pSensor->GetID()].vecSensorModeConfig[eProjectionMode];

         //如果选择0.15，那么在手指和白板触控的时候插值是需要打开的。   //wxl modify
         /*if (  g_tSysCfgData.vecSensorConfig[pSensor->GetID()].eSelectedLensType == E_LENS_TR_0_DOT_15
             || g_tSysCfgData.vecSensorConfig[pSensor->GetID()].eSelectedLensType == E_LENS_TR_0_DOT_19
             || g_tSysCfgData.vecSensorConfig[pSensor->GetID()].eSelectedLensType == E_LENS_TR_0_DOT_21
            )
         {
             if (TSensorModeConfig->advanceSettings.m_eTouchType != E_DEVICE_PALM_TOUCH_CONTROL)
             {
                 TSensorModeConfig->advanceSettings.bEnableStrokeInterpolate = TRUE;
                 pSensor->SetStrokeInterpolate(TRUE);
             }
         }*/

         /////设置是否开启自动屏蔽功能
         if (TSensorModeConfig->advanceSettings.bIsDynamicMaskFrame)
         {
             pSensor->GetPenPosDetector()->EnableDynamicMasking(TRUE);
         }
         else
         {
             pSensor->GetPenPosDetector()->EnableDynamicMasking(FALSE);
         }
         ////////////设置是否开启抗干扰功能
         if (TSensorModeConfig->advanceSettings.bIsAntiJamming)
         {
             pSensor->GetPenPosDetector()->EnableAntiJamming(TRUE);
         }
         else
         {
             pSensor->GetPenPosDetector()->EnableAntiJamming(FALSE);
         }
         /////////设置是否启用手动绘制的静态屏蔽图
         if (TSensorModeConfig->advanceSettings.bIsOnLineScreenArea)
         {
             pSensor->GetPenPosDetector()->EnableOnlineScreenArea(TRUE);
         }
         else
         {
             pSensor->GetPenPosDetector()->EnableOnlineScreenArea(FALSE);
         }
         ///////////////////
         if (TSensorModeConfig->advanceSettings.bDisableReflectionSpot)
         {
             pSensor->GetPenPosDetector()->DisableReflectionPoint(TRUE);
         }
         else {
             pSensor->GetPenPosDetector()->DisableReflectionPoint(FALSE);
         }
         if (g_tSysCfgData.globalSettings.bSinglePointMode)
         {
             this->m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().SetSinglePointMode(true);
         }
         else
         {
             this->m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().SetSinglePointMode(false);
         }

         /////是否进行隔空操作
         EAIROPERATE_CLICKMODE eClickMode = g_tSysCfgData.globalSettings.eClickMode;
         if (g_tSysCfgData.globalSettings.bAirOperatePermission)
         {
             this->m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().SetAirOperateMode(true, eClickMode);
         }
         else
         {
             this->m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().SetAirOperateMode(false, eClickMode);
         }

         //设置平滑系数
         this->m_oIWBSensorManager.GetSpotListProcessor().SetSmoothCoefficient(TSensorModeConfig->advanceSettings.nSmoothCoefficient);

    }//if

}


//@功能:应用传感器的配置
HRESULT CIWBDlg::OnApplySensorConfig(WPARAM wParam, LPARAM lParam)
{
    CAdvancedSettingsDlg* pAdvSettingsDlg = reinterpret_cast<CAdvancedSettingsDlg*>(wParam);
    CIWBSensor* pSensor = reinterpret_cast<CIWBSensor*>(lParam);

    pSensor->SetCfgData(pAdvSettingsDlg->GetSensorConfig(), &pAdvSettingsDlg->GetGlobalSettings());

    //保存配置文件
    g_tSysCfgData.vecSensorConfig[pSensor->GetID()] = pAdvSettingsDlg->GetSensorConfig();
    g_tSysCfgData.globalSettings = pAdvSettingsDlg->GetGlobalSettings();

    const TCaptureDeviceInstance& devInfo = pSensor->GetDeviceInfo();
    if (!devInfo.m_strDevPath.IsEmpty())
    {
       //写入配置文件
        int nCount = this->m_oIWBSensorManager.GetSensorCount();
       ::SaveConfig(PROFILE::CONFIG_FILE_NAME, ::g_tSysCfgData, nCount);
    }
    return 0L;
}

void CIWBDlg::OnStartIntallerWithTips(CIWBSensor* pSensor)
{
    if (pSensor == NULL) return;

    m_pCurInstalledSensor = pSensor;

    m_tipProcServer.Start();
}

/*
@功能：从一个指定区域FR中获取一个特定子区域SR,SR满足如下条件
      a.SR居中
      b.SR的宽度wSR和高度hSR满足给定的比例wr : hr，其中wr和hr都是正数
      c.SR是满足条件a和b中面积最大的区域
@参数：rcRegion [in, out], in——指定区域，out——找到的特定区域；
       wRatio, hRatio,宽高比
*/
void GetMaxAreaCenterRegionRatioBasedFromTheRegion(RECT &rcInOut, const int &wRatio, const int &hRatio)
{
    if (wRatio <= 0 || hRatio <= 0) return;

    int nGivenRegionWidth = rcInOut.right - rcInOut.left;
    int nGivenRegionHeight = rcInOut.bottom - rcInOut.top;

    int nNewRegionLeft = rcInOut.left;
    int nNewRegionTop = rcInOut.top;
    int nNewRegionWidth = nGivenRegionWidth;
    int nNewRegionHeight = nGivenRegionHeight;

    if(wRatio > hRatio)
    {
        //要求新区域的宽度更大，则新区域的计算方式如下
        /*
          (1)给定区域的宽度小于其高度，则新区域的宽度直接取给定区域的整个宽度，然后再根据比值求新区域的高度
        */
        if(nGivenRegionWidth < nGivenRegionHeight)
        {
            nNewRegionHeight = (nGivenRegionWidth * hRatio + wRatio - 1) / wRatio;            
        }
        /*
            (2)给定区域的宽度大于其高度，则在nGivenRegionHeight找一个值nNewRegionHeight，使其满足：
               nNewRegionHeight * wRatio 最接近nGivenRegionWidth
        */
        else
        {
            //
            int tmp = nGivenRegionWidth * hRatio;
            for (; nNewRegionHeight > 0; nNewRegionHeight--)
            {
                if (nNewRegionHeight * wRatio <= tmp)
                    break;
            }
        }

        nNewRegionTop = rcInOut.top + ((nGivenRegionHeight - nNewRegionHeight + 1) >> 1);
    }
    else
    {
        //要求新区域的宽度更大，则新区域的计算方式如下
        /*
        (1)给定区域的高度小于其宽度，则新区域的高度直接取给定区域的整个高度，然后再根据比值求新区域的宽度
        */
        if(nGivenRegionHeight < nGivenRegionWidth)
        {
            nNewRegionWidth = (nNewRegionHeight * wRatio + hRatio - 1) / hRatio;
        }
        /*
        (2)给定区域的宽度大于其高度，则在nGivenRegionHeight找一个值nNewRegionHeight，使其满足：
        nNewRegionHeight * wRatio 最接近nGivenRegionWidth
        */
        else
        {
            //
            int tmp = nGivenRegionHeight * wRatio;
            for (; nNewRegionWidth > 0; nNewRegionWidth--)
            {
                if (nNewRegionWidth * hRatio <= tmp)
                    break;
            }
        }

        nNewRegionLeft = rcInOut.left + ((nGivenRegionWidth - nNewRegionWidth + 1) >> 1);
    }

    rcInOut.left = nNewRegionLeft;
    rcInOut.top = nNewRegionTop;
    rcInOut.right = rcInOut.left + nNewRegionWidth;
    rcInOut.bottom = rcInOut.top + nNewRegionHeight;
}

LRESULT CIWBDlg::OnChangeDisplayWindow(WPARAM wParam, LPARAM lParam)
{
    if(m_pCurInstalledSensor)
    {
        HWND hDisplayWnd = (HWND)wParam;
        if (hDisplayWnd && (m_hDispWnd != hDisplayWnd))
        {              
            RECT rcDispWnd;
            if (this->GetSafeHwnd() == hDisplayWnd)
            {
                RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0,reposQuery, &rcDispWnd, NULL, TRUE);
            }    
            else
            {
                MinimizeToTray(); //视频将绘在其他（安装帮助程序）窗口时，将首先把IWB最小化
                /*
                @:说明：在安装助手上显示视频时，按4:3的比例显示，故会在
                        hDisplayWnd窗口上找一个居中的且面积最大的区域显示视频
                */
                ::GetClientRect(hDisplayWnd, &rcDispWnd);

                /*
                */
                const int KWRATIO = 4;
                const int KHRATIO = 3;
                GetMaxAreaCenterRegionRatioBasedFromTheRegion(rcDispWnd, KWRATIO, KHRATIO);
            }

            m_hDispWnd = hDisplayWnd;            
            m_oIWBSensorManager.SetVideoDispParams(m_hDispWnd, this->GetSafeHwnd(), rcDispWnd);               
        }
    }

    return 0;
}

HRESULT CIWBDlg::OnProcessDetectBackSplashData(WPARAM wParam,LPARAM lParam)
{
    if (m_tipProcServer.IsStarted())
    {
        m_tipProcServer.OnDetectBackSlashVanish((int)wParam, (int)lParam);
       // AtlTrace(_T("OnDetectBackSlashVanish : Threshold=%d, nTotalSpots=%d, nTotalSpotArea=%d \n"), (((int)lParam)<<6), (int)lParam, (int)wParam);
    }
    return 0;
}

HRESULT CIWBDlg::OnStartDetectBackSplashVanished(WPARAM wParam,LPARAM lParam)
{
    m_oIWBSensorManager.StartDetectBackSplashVanished();
    return 0;
}

HRESULT CIWBDlg::OnStopDetectBackSplashVanished(WPARAM wParam,LPARAM lParam)
{
    m_oIWBSensorManager.StopDetectBackSplashVanished();
    return 0;
}

void CIWBDlg::OnSwapSensorImage()
{
    this->m_oIWBSensorManager.SwapSensorImage(0, 1);
}


void CIWBDlg::OnSwapImageWithSensor(UINT uID)
{
    /*注意到有时菜单项在窗体客户区以外了。
    POINT ptCursor;
    GetCursorPos(&ptCursor);
    ScreenToClient(&ptCursor);
    CIWBSensor*  pSensor = this->m_oIWBSensorManager.SensorFromPt(ptCursor);
    */
    CIWBSensor*  pSensor = this->m_oIWBSensorManager.GetSensor();
    if (pSensor == NULL)
    {
        return;
    }

    UINT uSensorID = uID - ID_SWAP_WITH_SENSOR0;
    if (uSensorID < 0 || uSensorID >= m_oIWBSensorManager.GetSensorCount()) return;

    UINT uCurrentSensorID = pSensor->GetID();
    if (uCurrentSensorID != uSensorID)
    {
        this->m_oIWBSensorManager.SwapSensorImage(pSensor->GetID(), uSensorID);
    }

}


void CIWBDlg::InsertMenuItem2TheMenu(CMenu *pMenu, const int &nInsertIndex, const DWORD &dwIMenuItemID, LPTSTR lpszMenuItemName)
{
    if(NULL == pMenu || nInsertIndex < 0 || lpszMenuItemName == NULL) return;
    MENUITEMINFO mnuiteminfo;
    memset(&mnuiteminfo, 0, sizeof(MENUITEMINFO));
    mnuiteminfo.cbSize = sizeof(MENUITEMINFO);
    mnuiteminfo.fMask      = MIIM_FTYPE | MIIM_STRING | MIIM_ID ;
    mnuiteminfo.fType      = MFT_STRING;
    mnuiteminfo.dwTypeData = lpszMenuItemName;
    mnuiteminfo.wID        = dwIMenuItemID;
    pMenu->InsertMenuItem(nInsertIndex, &mnuiteminfo, TRUE);
}

void CIWBDlg::InsertParamSettingMenuItem(CMenu *pMenu, const int &nInsertIndex)
{
    if (theApp.GetScreenMode() != EScreenModeSingle) return;//只在单屏情况下主菜单加入“Parameter Settings ...”菜单项
    if(NULL == pMenu || nInsertIndex < 0) return;
    CMenu *pSubMenu = pMenu->GetSubMenu(0);
    DWORD dwItemID = ID_MENU_PARAMETER_SETTINGS;
    InsertMenuItem2TheMenu(pSubMenu, nInsertIndex, dwItemID, const_cast<LPTSTR>(g_oResStr[IDS_STRING453]));
}


void CIWBDlg::InitDeviceUseModeMenuItemWithMenu(CMenu *pMenu)
{
    if (NULL == pMenu) return;
    if(m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().IsVirtualDriverOpen())
    {
        pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE,    MF_BYCOMMAND| MF_ENABLED);
        pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND| MF_ENABLED);
        EHIDDeviceMode eDeviceMode = m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().GetHIDMode();
        if(eDeviceMode == E_DEV_MODE_MOUSE)
        {
            pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE,    MF_BYCOMMAND|MF_CHECKED);
            pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND|MF_UNCHECKED);
        }
        else if(eDeviceMode == E_DEV_MODE_TOUCHSCREEN)
        {
            pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE,    MF_BYCOMMAND|MF_UNCHECKED);
            pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND|MF_CHECKED);
        }
    }
    else
    {//当"虚拟驱动"未打开, 则强制使用Windows API模拟鼠标消息。
     //强制使用鼠标模式
        pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE,    MF_BYCOMMAND|MF_CHECKED);
        pMenu->CheckMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND|MF_UNCHECKED);

 //       pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE,    MF_BYCOMMAND| MF_GRAYED);
        pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND| MF_GRAYED);
    }

    ////////////////add by zhaown if是HID模式，按原来的走，如果只是TUIO模式，那么鼠标和触屏选项要灰掉
    bool bHIDMode = m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().GetTouchHIDMode();
    bool bTUIOMode = m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().GetTouchTUIOMode();
    //只有TUIO模式，HID模式选择全部灰掉
    if (!bHIDMode && bTUIOMode)
    {
        pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_MOUSE, MF_BYCOMMAND | MF_GRAYED);
        pMenu->EnableMenuItem(ID_INSTALLATIONANDDEBUGGING_TOUCHPAD, MF_BYCOMMAND | MF_GRAYED);
    }

}

void CIWBDlg::OnChangeTouchScreenAspectRatio(UINT uID)
{
   g_tSysCfgData.globalSettings.eTouchScreenAspectRatio = ETouchScreenAspectRatio(uID - ID_TOUCHSCREENASPECTRATIO_AUTO);
}
//>>

//@功能:与APP通信的协议
HRESULT CIWBDlg::OnAppCommMsg(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case 1:
        if (lParam == 1)
        {//禁用触控
            this->m_oIWBSensorManager.EnableOpticalPen(FALSE);
        }
        else if (lParam == 2)
        {
            //进入正常使用模式
            this->m_oIWBSensorManager.SwitchToMode(E_NORMAL_USAGE_MODE);

            //使能触控
            this->m_oIWBSensorManager.EnableOpticalPen(TRUE);

        }
        break;

    case 2:
        //自动定位
        OnStartMenuAutoCalibrate();
        break;
    case  3:
        //控制IRCUT
        if(lParam == 1)
        {
            CIWBSensor* lpSensor = this->m_oIWBSensorManager.GetSensor();
            if (lpSensor)
            {
                IRCUTSwtich(lpSensor->GetVideoPlayer()->GetCaptureFilter(),FALSE,lpSensor->GetDeviceInfo().m_nPID,lpSensor->GetDeviceInfo().m_nVID);
            }
        }
        else if (lParam == 2)
        {
            CIWBSensor* lpSensor = this->m_oIWBSensorManager.GetSensor();
            if (lpSensor)
            {
                IRCUTSwtich(lpSensor->GetVideoPlayer()->GetCaptureFilter(), TRUE, lpSensor->GetDeviceInfo().m_nPID, lpSensor->GetDeviceInfo().m_nVID);
            }
        }
        break;

    default:
        break;
    }
    return 0L;
}


void CIWBDlg::OnInstallationanddebuggingUpdatefirmware()
{
    // TODO: Add your command handler code here
    CUpdateFirmwareDlg  updateFirmwareDlg;
    updateFirmwareDlg.DoModal();

}

//2018/01/26
//更新加密锁的状态
void CIWBDlg::UpdateInfoAboutDongle()
{
    //CString strAboutDongle =
    //	theApp.IsHardwareKeyExist() ?
    //	_T("Dongle Ok/color:black") :
    //	_T("Dongle missing/color:red");
    CString strAboutDongle =
        theApp.IsHardwareKeyExist() ?
        g_oResStr[IDS_STRING476]:(theApp.IsOnlineRegistered()?_T(""):g_oResStr[IDS_STRING477]);

    strAboutDongle += theApp.IsHardwareKeyExist() ? _T("/color:black") : _T("/color:red");

    m_ctlStatusBar.SetPaneText(PANE_DONGLE, strAboutDongle, TRUE);

    int nSensorCount = this->m_oIWBSensorManager.GetSensorCount();
    if (nSensorCount > 1)
    {
        //m_ctlStatusBar.SetPaneText(StatusPaneCountEachSensor + PANE_DONGLE, strAboutDongle, TRUE);
    }

    //201680629,硬件加密狗丢失，不要弹出对话框，因为客户有的设备只有软注册，没有硬件加密狗
    /*
    //<<弹出对话框提示
    if(!theApp.IsHardwareKeyExist())
    {
        ::MessageBox(NULL, g_oResStr[IDS_STRING477], g_oResStr[IDS_STRING103], MB_ICONERROR | MB_OK);
    }
    */
    //>>
}

void CIWBDlg::OnInstallationanddebuggingEnableinterpolate()
{
    // TODO: Add your command handler code here
    /////需要把改变的设置进行保存
    EProjectionMode eProjectionMode = g_tSysCfgData.globalSettings.eProjectionMode;
    /////插值的设定情况是在不管在哪个屏幕下，只要点击设置所有的屏上的插值都进行设置
    int nCount = this->m_oIWBSensorManager.GetSensorCount();
    for (int i = 0; i < nCount; i++)
    {
         CIWBSensor* pSensor = this->m_oIWBSensorManager.GetSensor(i);
         if (pSensor)
         {
             TAdvancedSettings &AdvancedSettings = g_tSysCfgData.vecSensorConfig[i].vecSensorModeConfig[eProjectionMode].advanceSettings;
             AdvancedSettings.bEnableStrokeInterpolate = !AdvancedSettings.bEnableStrokeInterpolate;
             pSensor->SetStrokeInterpolate(AdvancedSettings.bEnableStrokeInterpolate?TRUE:FALSE);
         }
    }	
}

void CIWBDlg::OnMenuAdvancessetting()
{
    // TODO: Add your command handler code here
    // TODO: Add your command handler code here
    CIWBSensor* pSensor = this->m_oIWBSensorManager.GetSensor();
    std::vector<CAtlString>  vecCameraInfo;
    if (pSensor)
    {
        const TCaptureDeviceInstance& devInfo = pSensor->GetDeviceInfo();
        for (size_t i = 0; i < devInfo.m_vecVideoFmt.size(); i++)
        {
            CAtlString strFormatName = GetVideoFormatName(devInfo.m_vecVideoFmt[i]);
            for (size_t j =0; j< m_aryCandidateResolution.size(); j++)
            {
                if (_tcsicmp(strFormatName,m_aryCandidateResolution[j]) == 0)
                {
                     vecCameraInfo.push_back(strFormatName);
                     break;
                }
            }
        }
    }
    CAtlString CurrentUserResolution = pSensor->GetVideoPlayer()->CurrentCameraResolution();

    CameraFmtDialog  camerafmtdlg;
    camerafmtdlg.SetCameraResolution(vecCameraInfo, CurrentUserResolution);

    DWORD dIP = m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().GetIPadress();
    int nPort = m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().GetPort();
    int Width = m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().GetScreenWidth();
    int Height= m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().GetScreenHeight();

    camerafmtdlg.SetTUIOParams(dIP, nPort, Width, Height);
    ///////////////////////////////////
    CAtlString  SelectValue;
    if (camerafmtdlg.DoModal() == IDOK)
    {
        SelectValue = camerafmtdlg.GetSelectComboxvalue();
        if (pSensor)
        {
            const TCaptureDeviceInstance& devInfo = pSensor->GetDeviceInfo();
            for (size_t j = 0; j < devInfo.m_vecVideoFmt.size(); j++)
            {
                CAtlString strFName = GetVideoFormatName(devInfo.m_vecVideoFmt[j]);
                if (strFName == SelectValue)
                {
                    pSensor->SetFavoriateMediaType(devInfo.m_vecVideoFmt[j]);
                    break;
                }
            }
        }

        //得到触控方式，如果两个都是不选择的话，就强行的置为HID触控模式。
        bool bHIDMode = camerafmtdlg.GetTouchHIDMode();
        bool bTUIOMode = camerafmtdlg.GetTouchTUIOMode();
        if (!bHIDMode && !bTUIOMode)
        {
            g_tSysCfgData.globalSettings.bTouchHIDMode = true;
            //2020/10/19, uncommented out by xuke
            bHIDMode = true;
        }
        else
        {
            g_tSysCfgData.globalSettings.bTouchHIDMode= bHIDMode;
        }
        g_tSysCfgData.globalSettings.bTouchTUIOMode  = bTUIOMode;

        m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().SetTouchTUIOMode(bTUIOMode);
        m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().SetTouchHIDMode(bHIDMode);

        m_oIWBSensorManager.GetSpotListProcessor().GetVirtualHID().SetTUIOParams(camerafmtdlg.GetIPAddress(),camerafmtdlg.GetPort(), camerafmtdlg.GetScreenWidth(), camerafmtdlg.GetScreenHeight());

        ////////////////////////保存数据
        int nCount = this->m_oIWBSensorManager.GetSensorCount();
        if (nCount >0)
        {
            pSensor->SetResolutionType(SelectValue);			
        }
        //写入配置文件
        SaveConfig();
    }
}
void CIWBDlg::OnMenuStartDrawOnlineScreenArea()
{
    // TODO: Add your command handler code here
    int nSensorCount = m_oIWBSensorManager.GetSensorCount();
    m_bStartDrawOnlineScreenArea = true;
    m_nDrawOnlineAreaCount = 0;   //绘制第几个屏蔽图
    m_nActiveDetectCameraId = 0;  //实际运行的摄像头个数
    for (int i = 0; i < nSensorCount; i++)
    {
       ////去掉引导框
       CIWBSensor* lpSensor = this->m_oIWBSensorManager.GetSensor(i);
       if (lpSensor)
       {
           ////如果没有运行，那么就不存在需要绘制屏蔽图了。
           if (lpSensor->IsDetecting())
           {
               m_nActiveDetectCameraId++;  //实际检测到打开的摄像头个数，因为有几个sensor不一定都打开。
                m_bPreGuideRectangleVisible = lpSensor->GetPenPosDetector()->IsGuideRectangleVisible() ? true : false;
               lpSensor->OnStartDrawOnlineScreenArea();
           }
       }
    }
}

void CIWBDlg::OnMenuClearDrawOnlineScreenArea()
{
    // TODO: Add your command handler code here
    m_bStartDrawOnlineScreenArea = false;
    int nSensorCount = m_oIWBSensorManager.GetSensorCount();
    for (int i = 0; i < nSensorCount; i++)
    {
        CIWBSensor* lpSensor = this->m_oIWBSensorManager.GetSensor(i);
        if (lpSensor)
        {
            lpSensor->GetPenPosDetector()->DeleteOnLineScreenArea();
    }
    }
}

void CIWBDlg::OnMenuEnableDrawOnlineScreenArea()
{
    // TODO: Add your command handler code here
    this->m_oIWBSensorManager.EnableOnlineScreenArea(!this->m_oIWBSensorManager.IsEnableOnlineScreenArea());

}

void CIWBDlg::OnMenuTouchScreenLayoutDesigner()
{
    CMenu* pInstallMenu = m_oMenu.GetSubMenu(1);
    if (NULL == pInstallMenu) return;

    BOOL bIsVisible = this->m_oIWBSensorManager.GetScreenLayoutDesigner().IsVisible();
    this->m_oIWBSensorManager.GetScreenLayoutDesigner().DoDesign(!bIsVisible);

}


//@功能:结束屏幕布局设计
HRESULT CIWBDlg::OnScreenLayoutDesignBtnEvent(WPARAM wParam, LPARAM lParam)
{
    UINT uID = UINT(lParam);

    switch (uID)
    {
    case BUTTON_ID_OK:
    {//
        //隐藏屏幕布局编辑工具
        this->m_oIWBSensorManager.GetScreenLayoutDesigner().DoDesign(FALSE);

        this->m_oIWBSensorManager.ApplyScreenLayout();
        SaveConfig();
    }
    break;

    case BUTTON_ID_CANCEL:
    {//
        //隐藏屏幕布局编辑工具
        this->m_oIWBSensorManager.GetScreenLayoutDesigner().DoDesign(FALSE);

        /*
        for (UINT uLayoutIndex = 0; uLayoutIndex < g_tSysCfgData.vecScreenLayouts.size(); uLayoutIndex++)
        {
            const TScreenLayout&layout = g_tSysCfgData.vecScreenLayouts[uLayoutIndex];

            if (layout.vecScreens.size() == this->m_oIWBSensorManager.GetSensorCount())
            {

                this->m_oIWBSensorManager.GetScreenLayoutDesigner().SetScreenRelativeLayouts(&layout.vecScreens[0], layout.vecScreens.size());
                this->m_oIWBSensorManager.GetScreenLayoutDesigner().SetRelativeMergeAreas(&layout.vecMergeAreas[0], layout.vecMergeAreas.size());
                break;
            }
        }
        */

            //const ESplitScreeMode& eSelectedSplitScreenMode = g_tSysCfgData.screenLayoutManger.GetSelectedSplitScreenMode();
            //const SplitMode& splitMode = g_tSysCfgData.screenLayoutManger.GetSelectedSplitScreenMode();
            SplitMode splitMode;
            bool bRet = g_tSysCfgData.screenLayoutManger.GetSplitMode(this->m_oIWBSensorManager.GetSensorCount(), splitMode);
            
            if (bRet)
            {
                //const TScreenLayout* pScreenLayout = g_tSysCfgData.screenLayoutManger.GetScreenLayout(eSelectedSplitScreenMode, this->m_oIWBSensorManager.GetSensorCount());
                const TScreenLayout* pScreenLayout = g_tSysCfgData.screenLayoutManger.GetScreenLayout(splitMode);
                if (pScreenLayout)
                {
                    //this->m_oIWBSensorManager.GetScreenLayoutDesigner().SetScreenLayout(eSelectedSplitScreenMode, pScreenLayout);
                    this->m_oIWBSensorManager.GetScreenLayoutDesigner().SetScreenLayout(*pScreenLayout);
                }
            }
            

     }
        break;

        case BUTTON_ID_RESET:
    {

        this->m_oIWBSensorManager.GetScreenLayoutDesigner().Reset();
        }
        break;

        case BUTTON_ID_ROTATE_90:
        //{
        //    ESplitScreeMode eSelectedSplitScreenMode = this->m_oIWBSensorManager.GetScreenLayoutDesigner().GetSplitScreenMode();
        //    
        //    //交换水平分割模式和垂直分割模式
        //    if (E_SPLIT_SCREEN_VERT == eSelectedSplitScreenMode)
        //    {
        //        eSelectedSplitScreenMode = E_SPLIT_SCREEN_HORZ;
        //    }
        //    else if (E_SPLIT_SCREEN_HORZ == eSelectedSplitScreenMode)
        //    {
        //        eSelectedSplitScreenMode = E_SPLIT_SCREEN_VERT;
        //    }
        //    
        //    const TScreenLayout* pScreenLayout = g_tSysCfgData.screenLayoutManger.GetScreenLayout(eSelectedSplitScreenMode, this->m_oIWBSensorManager.GetSensorCount());

        //    this->m_oIWBSensorManager.GetScreenLayoutDesigner().SetScreenLayout(eSelectedSplitScreenMode, pScreenLayout);

        //}
        break;


        case BUTTON_ID_CONFIG:
        {
            const TScreenLayout& screenLayout = this->m_oIWBSensorManager.GetScreenLayoutDesigner().GetScreenLayout();
            CDlgScreenLayoutSettings dlg(screenLayout.GetSplitMode(), this->m_oIWBSensorManager.GetScreenLayoutDesigner().GetScreenTargetType());
            if (IDOK == dlg.DoModal())
            {

                EScreenTargetType eScreenTargetType = dlg.GetScreenTargetType();

                this->m_oIWBSensorManager.GetScreenLayoutDesigner().SetScreenTargetType(eScreenTargetType);


                const SplitMode& splitMode = dlg.GetSplitMode();
                const TScreenLayout* pScreenLayout = g_tSysCfgData.screenLayoutManger.GetScreenLayout(splitMode);
                if (pScreenLayout)
                {
                    this->m_oIWBSensorManager.GetScreenLayoutDesigner().SetScreenLayout(*pScreenLayout);
                }
                else
                {
                    TScreenLayout newScreenLayout(splitMode);
                    this->m_oIWBSensorManager.GetScreenLayoutDesigner().SetScreenLayout(newScreenLayout);

                }

            }

        }
    }
    return 0L;

}

void CIWBDlg::OnSwitchToFusionScreenMode(UINT uID)
{
    EScreenMode eScreenMode = EScreenMode(uID - ID_SWTICH_SCREENMODE_ONE);
    int nSensorCount = int(eScreenMode) + 1;

    g_tSysCfgData.globalSettings.eScreenMode = eScreenMode;

    //反初始化
    m_oIWBSensorManager.Uninit();
    //初始化
    //m_oIWBSensorManager.Init(theApp.GetScreenType() == EDoubleScreenMode?2:1);
    //根据注册的信息决定
    m_oIWBSensorManager.Init(nSensorCount);
    //载入配置信息
    //LoadConfig();

    //将配置数据设置到传感器中去
    this->m_oIWBSensorManager.SetCfgData(g_tSysCfgData);

    m_oUSBCameraDeviceList.UpdateDeviceList();

    //CIWBSensor对象分配摄像头设备路径
    m_oIWBSensorManager.AssignCamera(m_oUSBCameraDeviceList);

    //通知各个模块更改屏幕物理尺寸和屏幕分辨率
    OnDisplayChangeHelper(::GetActualScreenControlSize());

    StartRunning();

    CRect rcDisplayArea;
    GetClientRect(&rcDisplayArea);
    InvalidateRect(&rcDisplayArea, TRUE);
    UpdateWindow();

    int cx = rcDisplayArea.Width();
    int cy = rcDisplayArea.Height();
    AdjustStatusBar(cx,cy);

}


//@功能:”结束4点标定“消息响应函数
HRESULT CIWBDlg::OnEnd4BasePointCalibrate(WPARAM wParam, LPARAM lParam)
{
    BOOL bSuccess = (BOOL)wParam;

    m_oIWBSensorManager.OnIWBSensor4BasePointMarkingDone(bSuccess);

    if (m_oIWBSensorManager.IsCalibarateOk())
    {
        //保存配置信息
        CIWBSensor* pSensor = this->m_oIWBSensorManager.GetSensor();
        
        if (pSensor)
        {
            const TCaptureDeviceInstance& devInfo = pSensor->GetDeviceInfo();
            if (!devInfo.m_strDevPath.IsEmpty())
            {
                this->SaveConfig();
            }

        }

        //最小化到托盘
        MinimizeToTray();
    }

    return 0L;
}

void CIWBDlg::OnOperationFourpointcalibration()
{
    // TODO: Add your command handler code here
    this->m_oIWBSensorManager.Start4BasePointMarking(this->GetSafeHwnd(), -1);
}

BOOL CIWBDlg::LoadResolutionConfig()
{
    const TCHAR* lpszXMLFileName = _T("DeviceResolution.xml");
    TiXmlDocument oXMLDoc;
    if (!oXMLDoc.LoadFile(CT2CA(lpszXMLFileName), TIXML_ENCODING_UTF8))
    {
        return FALSE;
    }
    TiXmlElement* pRootElement = oXMLDoc.RootElement();
    if (pRootElement == NULL) return FALSE;
    TiXmlNode* pChild = NULL;
    while (pChild = pRootElement->IterateChildren("USBCamera", pChild))
    {
        const char* NodeName = pChild->Value();//节点名称
        const char* lpszResolution = ((TiXmlElement*)pChild)->Attribute("Resolution");
        if (lpszResolution == NULL || strlen(lpszResolution) == 0)
        {
            break;
        }

        CAtlString strDeviceResolution;
        strDeviceResolution.Format(_T("%s"), (LPCTSTR)(CA2T(lpszResolution)));
        m_aryCandidateResolution.push_back(strDeviceResolution);
    }
    return TRUE;
}


HRESULT CIWBDlg::OnPowerBroadcast(WPARAM wParam, LPARAM lParam)
{
    if (wParam == PBT_APMRESUMEAUTOMATIC)
    {
        Sleep(500);
        int nSensorCount  = this->m_oIWBSensorManager.GetSensorCount();
        for (int i = 0; i < nSensorCount; i++)
        {
            CIWBSensor* pSensor = this->m_oIWBSensorManager.GetSensor(i);
            pSensor->SwitchLensMode(pSensor->GetLensMode());
        }
    }
    return S_OK;
}


//@功能:其他运行实例启动时, 检测到已有实例存在，要求既有实例显示主窗体。
HRESULT CIWBDlg::OnBetweenInstanceMsg(WPARAM wParam, LPARAM lParam)
{
    OnCtxmenuShowMainWindow();
    return 0;
}

//@功能:判断是否是合法的USBkey
BOOL CIWBDlg::IsEligibleUSBKey(UINT uVID, UINT uPID)
{
    BOOL bMatched = FALSE;
    UINT uDeviceCount = m_oUSBCameraDeviceList.GetDeviceInstanceCount();
    for (UINT uIndex = 0; uIndex < uDeviceCount; uIndex++)
    {
        const TCaptureDeviceInstance* pDevInst = m_oUSBCameraDeviceList.GetCaptureDeviceInstance(uIndex);
        if (pDevInst->m_nVID == uVID && pDevInst->m_nVID == uPID)
        {
            bMatched = TRUE;
            break;
        }
    }
    
    if (!bMatched)
    {//查看是否等于外置加密狗的VID和PID
        if (uVID == 0x3689 && uPID == 0x8762)
        {
            bMatched = TRUE;
        }

    }


    return bMatched;
}




void CIWBDlg::OnMenuCircleScreenManualCalibrate()
{
    // TODO: Add your command handler code here
    this->m_oIWBSensorManager.StartManualCalibrate(this->GetSafeHwnd(), -1, -1, -1, E_MUNUAL_CALIB_CIRCLE);
}


void CIWBDlg::OnChangeSensorAreaNo(UINT uID)
{
    CIWBSensor*  pSensor = this->m_oIWBSensorManager.GetSensor();
    if (pSensor == NULL)
    {
        return;
    }

    int AreaNo = uID - ID_SET_SENSOR_SCREEN_AREA_1;

    pSensor->SetScreenAreaNo(AreaNo);
    this->m_oIWBSensorManager.ApplyScreenLayout();
   


}