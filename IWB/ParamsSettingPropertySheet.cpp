#include "StdAfx.h"
#include "ParamsSettingPropertySheet.h"
//#include "headers.h"

//minimum required win8.1
//#include <shellscalingapi.h>


 int GetSystemDpi()
{
	 
		HDC hDC = GetDC(GetDesktopWindow());
		int PixelsPerInch = GetDeviceCaps(hDC, LOGPIXELSX);
		//double vPixelsPerInch = GetDeviceCaps(hDC, LOGPIXELSY);
		//int virtualWidth = GetDeviceCaps(hDC, HORZRES);
		//int physicalWidth = GetDeviceCaps(hDC, DESKTOPHORZRES);

		ReleaseDC(GetDesktopWindow(), hDC);

		return PixelsPerInch;//(int)(96 * physicalWidth / virtualWidth);

}

UINT GetOptimalFontSize()
{
	const int DEFAULT_FONT_SIZE = 8;//单位:DIP(Device Independent Point)=1/96 Logical Inch.
	
	const int DEFAULT_MONITOR_HEIGHT = 1200;//1200

	const int MINIMUM_FONT_SIZE = 6;

	UINT fontSize = DEFAULT_FONT_SIZE;

	CWnd * pMainWnd = AfxGetMainWnd();
	if (pMainWnd)
	{
		HMONITOR hMonitor = MonitorFromWindow(pMainWnd->GetSafeHwnd(), 0);

		//Note: GetDpiForMonitor only avaialbe on windows 8.1 and above.
		//UINT     dpix = 0, dpiy = 0;
		//HRESULT  hr = E_FAIL;
    	//GetDpiForMonitor,Windows 8.1[desktop apps only]
		//hr = GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy);
		
		int systemDpi = GetSystemDpi();

		/*
		MONITORINFOEX miex;
		miex.cbSize = sizeof(MONITORINFOEX);

		if (GetMonitorInfo(hMonitor, &miex))
		{
			const RECT& rcMonitor = miex.rcWork;
			int nMonitorHeight = miex.rcWork.bottom - miex.rcWork.top;

			
		}*/	

		fontSize = MulDiv(systemDpi, DEFAULT_FONT_SIZE, 96);

		if (fontSize < MINIMUM_FONT_SIZE)
			fontSize = MINIMUM_FONT_SIZE;
	}


	return fontSize;
}

AFX_STATIC_DATA int _afxPropSheetButtons[] = { IDOK, IDCANCEL, ID_APPLY_NOW};

IMPLEMENT_DYNAMIC(CParamsSettingPropertySheet, CPropertySheet)
CParamsSettingPropertySheet::CParamsSettingPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
: CCBPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
    AddPage(&m_advSettingPage);

    //m_advSettingPage.SetParent(this);
    AddPage(&m_gesSettingPage);
    //m_gesSettingPage.SetParent(this);

	AddPage(&m_sparateOperatePage);
}

CParamsSettingPropertySheet::CParamsSettingPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
: CCBPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    AddPage(&m_advSettingPage);
    AddPage(&m_gesSettingPage);

	AddPage(&m_sparateOperatePage);
}

CParamsSettingPropertySheet::~CParamsSettingPropertySheet(void)
{
}

BEGIN_MESSAGE_MAP(CParamsSettingPropertySheet, CCBPropertySheet)
    //{{AFX_MSG_MAP(CMyPropertySheet)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
    ON_BN_CLICKED (ID_APPLY_NOW, OnApply)
    /*ON_BN_CLICKED (IDOK, OnOK)
    ON_BN_CLICKED (IDCANCEL, OnCancel)*/
    ON_WM_CLOSE()
    ON_WM_DESTROY()      	
    ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


void CParamsSettingPropertySheet::OnApply()
{  
    m_advSettingPage.OnApply();
    m_gesSettingPage.OnApply();

	m_sparateOperatePage.OnApply();
}

void CParamsSettingPropertySheet::OnCancel()
{
    CPropertySheet::OnClose();
}

void CParamsSettingPropertySheet::OnOK()
{
    OnApply();
    CPropertySheet::OnClose();
}

BOOL CParamsSettingPropertySheet::OnInitDialog()
{

	//设置“确定“, ”取消”， “应用”按钮文字
    CWnd *hWnd = GetDlgItem(IDOK);   
    if (hWnd) hWnd->SetWindowText(const_cast<LPTSTR>(g_oResStr[IDS_STRING449]));

    hWnd = GetDlgItem(ID_APPLY_NOW);
    if (hWnd) hWnd->SetWindowText(const_cast<LPTSTR>(g_oResStr[IDS_STRING450]));

    hWnd = GetDlgItem(IDCANCEL);
    if (hWnd) hWnd->SetWindowText(const_cast<LPTSTR>(g_oResStr[IDS_STRING451]));
	
	
    // Add all the controls to the resizer
	if (GetDlgItem(ID_APPLY_NOW) != 0)
		m_Resizer.Add(this, ID_APPLY_NOW, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT);

	if (GetDlgItem(ID_WIZBACK) != 0)
		m_Resizer.Add(this, ID_WIZBACK, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT);

	if (GetDlgItem(ID_WIZNEXT) != 0)
		m_Resizer.Add(this, ID_WIZNEXT, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT);

	if (GetDlgItem(ID_WIZFINISH) != 0)
		m_Resizer.Add(this, ID_WIZFINISH, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT);

	if (GetDlgItem(IDOK) != 0)
		m_Resizer.Add(this, IDOK, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT);

	//if (GetDlgItem(IDHELP) != 0)
		//m_Resizer.Add(this, IDHELP, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT);

	if (GetDlgItem(IDCANCEL) != 0)
		m_Resizer.Add(this, IDCANCEL, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT);

	if (GetDlgItem(AFX_IDC_TAB_CONTROL) != 0)
		m_Resizer.Add(this, AFX_IDC_TAB_CONTROL, RESIZE_LOCKALL);

	if (this->IsWizard())
	{
		// Undocumented stuff, resize the two lines in wizard mode
		// Found the ID:s by using the Spy++ utility
		if (GetDlgItem(0x3026) != 0)
			m_Resizer.Add(this, 0x3026, RESIZE_LOCKBOTTOM | RESIZE_LOCKRIGHT | RESIZE_LOCKLEFT);
		if (GetDlgItem(0x3027) != 0)
			m_Resizer.Add(this, 0x3027, RESIZE_LOCKBOTTOM | RESIZE_LOCKRIGHT | RESIZE_LOCKLEFT);
	}

	m_nTabControlMarginLeft   = 0;
	m_nTabControlMarginRight  = 0;
	m_nTabControlMarginTop    = 0;
	m_nTabControlMarginBottom = 0;

	CWnd* pTabCtrl = GetDlgItem(AFX_IDC_TAB_CONTROL);
	if (pTabCtrl)
	{
		if (!m_bStacked)
		pTabCtrl->ModifyStyle(TCS_MULTILINE, TCS_SINGLELINE, 0);

		CRect rcTabCtrl;
		pTabCtrl->GetWindowRect(&rcTabCtrl);
		this->ScreenToClient(&rcTabCtrl);

		CRect rcClient;
		this->GetClientRect(&rcClient);

		m_nTabControlMarginLeft   = (rcTabCtrl.left   > rcClient.left    )? rcTabCtrl.left   - rcClient.left    : 0;
		m_nTabControlMarginRight  = (rcTabCtrl.right  < rcClient.right   )? rcClient.right   - rcTabCtrl.right  : 0;
		m_nTabControlMarginTop    = (rcTabCtrl.top    > rcClient.top     )? rcTabCtrl.top    - rcClient.top     : 0;
		m_nTabControlMarginBottom = (rcTabCtrl.bottom < rcClient.bottom) ? rcClient.bottom   - rcTabCtrl.bottom : 0;
	}

	BOOL bResult = (BOOL)Default();

	if (theApp.GetScreenMode() >= EScreenModeDouble &&  m_advSettingPage.m_pSensor)
	{//在双屏拼接模式下, 需要在属性页标签上显示传感器编号
		TC_ITEM item;
		item.mask = TCIF_TEXT;
		TCHAR labelText[1024];
		m_advSettingPage.GetWindowText(labelText, _countof(labelText));

		TCHAR additionalText[1024];
		_stprintf_s(additionalText, _T("[#%d%s]"), m_advSettingPage.m_pSensor->GetID() + 1, g_oResStr[IDS_STRING446]);

		_tcscat_s(labelText, _countof(labelText), additionalText);

		item.pszText = labelText;

		this->GetTabControl()->SetItem(0, &item);
	}
	
	//center the property sheet relative to the parent window
	if (!(GetStyle() & WS_CHILD))
		CenterWindow();

    
    //让PropertySheet对话框尺寸可调节
    // Set the resizable border
    this->ModifyStyle(0, WS_THICKFRAME, 0);

	//隐藏缺省存在的HELP按钮。
	//<<2107/08/03
	GetDlgItem(IDHELP)->ShowWindow(SW_HIDE);
	//>>

    return bResult;
}

void CParamsSettingPropertySheet::OnDestroy()
{
    CPropertySheet::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    //delete this;
}

void CParamsSettingPropertySheet::SetIWBSensorInfo(CIWBSensor* pSensor, int nSenorCount, bool isDoubleScreen)
{
    if (NULL == pSensor || nSenorCount <= 0) return;

    m_advSettingPage.m_strAdditionalCaption.Format(_T("%s"), g_oResStr[IDS_STRING446]);

    m_advSettingPage.m_pSensor = pSensor;

    m_advSettingPage.m_tSensorConfig = pSensor->GetCfgData();
}

void CParamsSettingPropertySheet::SetGlobalSettingInfo(const GlobalSettings& globalSettings)
{	
	m_advSettingPage.m_tGlobalSettings = globalSettings;

	m_sparateOperatePage.m_IsAirOperationPermitted = globalSettings.bAirOperatePermission;
	m_sparateOperatePage.m_nAPClick = globalSettings.eClickMode ;
}

const TSensorConfig&   CParamsSettingPropertySheet::GetSensorConfig()const
{
    return m_advSettingPage.m_tSensorConfig;
}

 const GlobalSettings& CParamsSettingPropertySheet::GetGlobalSettings()
{
	GetAirOperateInfo();
    return m_advSettingPage.m_tGlobalSettings;
}


void CParamsSettingPropertySheet::OnSize(UINT nType, int cx, int cy)
{
    // Must repeatidly do this to keep the frame from restoring itself
    //!!!让CPropertSheet尺寸可调的关键.
	ModifyStyle(0, WS_THICKFRAME);

    CCBPropertySheet::OnSize(nType, cx, cy);

}

BOOL CParamsSettingPropertySheet::CalcPageRect(RECT *pPageRect)
{
	
	CRect rcActivePage;
	CPropertyPage* pActivePage = GetActivePage();	
	if (NULL == pActivePage)  return FALSE;
	pActivePage->GetWindowRect(&rcActivePage);
	ScreenToClient(&rcActivePage);

	CRect rcTab;
	CalcTabCtrlRect(&rcTab);

	pPageRect->left   = rcTab.left;
	pPageRect->top    = rcActivePage.top;
	pPageRect->right  = rcTab.right;
	pPageRect->bottom = rcTab.bottom;

	return TRUE;
}


BOOL CParamsSettingPropertySheet::CalcTabCtrlRect(RECT* pRectTabCtrl)
{
	CRect rcTab;

	CTabCtrl* pTabCtrl = GetTabControl();
	if (NULL == pTabCtrl) return FALSE;
	pTabCtrl->GetWindowRect(&rcTab);
	ScreenToClient(&rcTab);
	
	CRect rcClient;
	this->GetClientRect(&rcClient);

	if (pRectTabCtrl)
	{
		pRectTabCtrl->left   = rcClient.left   + m_nTabControlMarginLeft;
		pRectTabCtrl->right  = rcClient.right  - m_nTabControlMarginRight;
		pRectTabCtrl->top    = rcClient.top    + m_nTabControlMarginTop;
		pRectTabCtrl->bottom = rcClient.bottom - m_nTabControlMarginBottom;
	}

	return TRUE;
	
}

INT_PTR CParamsSettingPropertySheet::DoModal()
{
	// TODO: Add your specialized code here and/or call the base class
	Init(_T("Microsoft Sans Serif"), GetOptimalFontSize());

	return CPropertySheet::DoModal();
}


void CParamsSettingPropertySheet::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	//CCBPropertySheet::OnWindowPosChanged(lpwndpos);

	// TODO: Add your message handler code here
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	
	CRect rcClient;
	GetClientRect(rcClient);
	
	CTabCtrl* pTabCtrl = GetTabControl();
	if (pTabCtrl)
	{

		CRect rcTabCtrl;
		CalcTabCtrlRect(&rcTabCtrl);

		pTabCtrl->MoveWindow(rcTabCtrl, FALSE);

		CRect rcPage;
		CalcPageRect(rcPage);

		// Resize all pages created 
		for (INT i = 0; i < GetPageCount(); i++)
		{
			CPropertyPage *page = GetPage(i);

			if (IsWindow(page->GetSafeHwnd())) page->MoveWindow(rcPage);
		}

		// Resize other controls
		//实际上时挪动的是"确定", "取消", “应用"按钮。
		m_Resizer.Resize(this);	
	}
}


void CParamsSettingPropertySheet::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	//CCBPropertySheet::OnWindowPosChanging(lpwndpos);

	// TODO: Add your message handler code here
	if (lpwndpos && !(lpwndpos->flags & SWP_NOSIZE))
	{

		HMONITOR hMonitor = MonitorFromWindow(lpwndpos->hwnd, 0);
		MONITORINFOEX miex;
		miex.cbSize = sizeof(MONITORINFOEX);

		if (GetMonitorInfo(hMonitor, &miex))
		{ 
			const RECT& rcWork = miex.rcWork;
			int nWorkWidth = rcWork.right - rcWork.left;
			int nWorkHeight = rcWork.bottom - rcWork.top;

			//限定窗体尺寸不超出工作区域
			if (lpwndpos->cx > nWorkWidth)
			{
				lpwndpos->cx = nWorkWidth;
			}
			

			if (lpwndpos->cy > nWorkHeight)
			{
				lpwndpos->cy = nWorkHeight;
			}
		}
		
	}
}

void CParamsSettingPropertySheet::GetAirOperateInfo()
{
    m_advSettingPage.m_tGlobalSettings.bAirOperatePermission = m_sparateOperatePage.m_IsAirOperationPermitted;
	m_advSettingPage.m_tGlobalSettings.eClickMode = (EAIROPERATE_CLICKMODE)m_sparateOperatePage.m_nAPClick;
}

