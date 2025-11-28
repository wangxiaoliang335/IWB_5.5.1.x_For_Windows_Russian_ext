// GestureSettingDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "headers.h"
#include "GestureSettingDlg.h"

#include "WGRConfigDlg.h"
#include "GLBGRSettingDlg.h"
// CGestureSettingDlg dialog

IMPLEMENT_DYNAMIC(CGestureSettingDlg, CScrollablePropertyPage)

CGestureSettingDlg::CGestureSettingDlg()
	: CScrollablePropertyPage(CGestureSettingDlg::IDD)
    , m_isWGRPermitted(false)
    , m_isGLBGRPeritted(true)
    , m_nEraserMulti(4)
    , m_nPalmMulti(5)
    , m_bInited(false)
    , m_dwWGRPermition(INIT_WITH_ALL_PERMITED)
{

}

CGestureSettingDlg::~CGestureSettingDlg()
{
}

void CGestureSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    DDX_Text(pDX, IDC_EDIT_PBGRRATE, m_nPalmMulti);
    DDX_Control(pDX, IDC_SPIN_PBGRRATE, m_sPalmMulti);

    DDX_Text(pDX, IDC_EDIT_ERASERRRATE, m_nEraserMulti);
    DDX_Control(pDX, IDC_SPIN_ERASERRRATE, m_sEraserMulti);

    DDX_Check(pDX, IDC_CHECK_SALLWGESTURE,   m_isWGRPermitted  );
    DDX_Check(pDX, IDC_CHECK_SALLGLBGESTURE, m_isGLBGRPeritted);
}


BEGIN_MESSAGE_MAP(CGestureSettingDlg, CScrollablePropertyPage)

    ON_EN_CHANGE(IDC_EDIT_PBGRRATE, &CGestureSettingDlg::OnEnChangeEditMultPalm)
    ON_EN_CHANGE(IDC_EDIT_ERASERRRATE, &CGestureSettingDlg::OnEnChangeEditMultEraser)
    ON_BN_CLICKED(IDC_CHECK_SALLWGESTURE, &CGestureSettingDlg::OnBnClickedCheckSallwgesture)
    ON_BN_CLICKED(IDC_CHECK_SALLGLBGESTURE, &CGestureSettingDlg::OnBnClickedCheckSallglbgesture)
    ON_BN_CLICKED(IDC_BUTTON_WGRMORE, &CGestureSettingDlg::OnBnClickedButtonWgrmore)
    ON_BN_CLICKED(IDC_BUTTON_GLBGRMORE, &CGestureSettingDlg::OnBnClickedButtonGlbgrmore)
END_MESSAGE_MAP()


// CGestureSettingDlg message handlers
BOOL CGestureSettingDlg::OnInitDialog()
{
	CScrollablePropertyPage::OnInitDialog();

    m_sPalmMulti.SetRange(3, 7);
    m_sEraserMulti.SetRange(3, 7);

    //SetTheCheckButtonChecked(IDC_CHECK_SALLWGESTURE, g_oWGRConfig.IsGerneralGRPermit());
    //SetTheCheckButtonChecked(IDC_CHECK_SALLGLBGESTURE, g_oGLBoardGR.IsGREnabled());

    m_dwWGRPermition = g_oWGRConfig.GetGeneralPermission();

    m_nPalmMulti = g_oWGRConfig.GetMultiplePalm2Std(); 
    m_nPalmMulti = m_nPalmMulti >= MULT_ERASER_MIN ? m_nPalmMulti : MULT_ERASER_MIN;
    m_nPalmMulti = m_nPalmMulti <= MULT_ERASER_MAX ? m_nPalmMulti : MULT_ERASER_MAX;

    m_nEraserMulti = g_oGLBoardGR.GetMultiEraser();
    m_nEraserMulti = m_nEraserMulti >= MULT_ERASER_MIN ? m_nEraserMulti : MULT_ERASER_MIN;
    m_nEraserMulti = m_nEraserMulti <= MULT_ERASER_MAX ? m_nEraserMulti : MULT_ERASER_MAX;


    //从配置文件中读取Windows手势配置
    m_isWGRPermitted = g_oWGRConfig.IsGerneralGRPermit();

    //从配置文件中读取GLBoard手势配置
    m_isGLBGRPeritted = g_oWGRConfig.IsGLBoardGRPermit();

	if (theApp.GetUSBKeyTouchType() == E_DEVICE_PALM_TOUCH_CONTROL)
	{
		GetDlgItem(IDC_EDIT_PBGRRATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ERASERRRATE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_SALLWGESTURE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_SALLGLBGESTURE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_WGRMORE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_GLBGRMORE)->EnableWindow(FALSE);
	}

    UpdateData(FALSE);

    m_bInited = true;


    return TRUE;
}

void CGestureSettingDlg::OnEnChangeEditMultPalm()
{
    if (!m_bInited) return;

    CString strText;
    GetDlgItem(IDC_EDIT_PBGRRATE)->GetWindowText(strText);

    int npos = _ttoi(strText);
    int bForceValidate = FALSE;

    if (npos < MULT_ERASER_MIN)
    {
        npos = MULT_ERASER_MIN;
        bForceValidate = TRUE;
    }

    if (npos > MULT_ERASER_MAX)
    {
        npos = MULT_ERASER_MAX;
        bForceValidate = TRUE;
    }

    if (bForceValidate)
    {
        strText.Format(_T("%d"),npos);
        GetDlgItem(IDC_EDIT_PBGRRATE)->SetWindowText(strText);

        return;
    }      

    SetModified(TRUE);
    UpdateData(TRUE);  
}

void CGestureSettingDlg::OnEnChangeEditMultEraser()
{
    if (!m_bInited) return;

    CString strText;
    GetDlgItem(IDC_EDIT_ERASERRRATE)->GetWindowText(strText);

    int npos = _ttoi(strText);
    int bForceValidate = FALSE;

    if (npos < MULT_ERASER_MIN)
    {
        npos = MULT_ERASER_MIN;
        bForceValidate = TRUE;
    }

    if (npos > MULT_ERASER_MAX)
    {
        npos = MULT_ERASER_MAX;
        bForceValidate = TRUE;
    }

    if (bForceValidate)
    {
        strText.Format(_T("%d"),npos);
        GetDlgItem(IDC_EDIT_ERASERRRATE)->SetWindowText(strText);

        return;
    }      

    SetModified(TRUE);
    UpdateData(TRUE);  
}

void CGestureSettingDlg::OnBnClickedCheckSallwgesture()
{
    // TODO: Add your control notification handler code here
    m_isWGRPermitted = IsTheCheckButtonChecked(IDC_CHECK_SALLWGESTURE);

    if (m_isWGRPermitted)
    {
        m_dwWGRPermition = INIT_WITH_ALL_PERMITED;
    }
    else
    {
        m_dwWGRPermition = INIT_WITH_ALL_PROHIBITED;
    }

    SetModified(TRUE);
}

void CGestureSettingDlg::OnBnClickedCheckSallglbgesture()
{
    // TODO: Add your control notification handler code here
    m_isGLBGRPeritted = IsTheCheckButtonChecked(IDC_CHECK_SALLGLBGESTURE);

    SetModified(TRUE);
}

BOOL CGestureSettingDlg::OnApply()
{
	//未被初始化，什么也不做
	//OnInitDialog函数未被调用
	//如果打开PropertyShee对话框时,
	//如果某个PropertyPage实例不是PropertySheet的当前页，则此刻还未调用OnInitDialog函数。
	//当切换到那个PropertyPage后, OnInitDialog函数才会被调用。
	
	if (!m_bInited) return FALSE;

    CPropertyPage::OnApply();

    SetModified(FALSE);

    if (!m_isWGRPermitted) 
    {
        m_dwWGRPermition = INIT_WITH_ALL_PROHIBITED;
    }    

    g_oWGRConfig.SetGeneralPermission(m_dwWGRPermition);
    g_oWGRConfig.SetMultiplePalm2Std(m_nPalmMulti);

    g_oGLBoardGR.EnableGestureRecognition(m_isGLBGRPeritted?true:false);
    g_oGLBoardGR.SetMultiEraser(m_nEraserMulti);

	//立即保存配置文件
	g_oWGRConfig.Save();

    return TRUE;
}

void CGestureSettingDlg::OnBnClickedButtonWgrmore()
{
    // TODO: Add your control notification handler code here
    CWGRConfigDlg dlg;

    dlg.DoModal();
    
    if(dlg.HasGeneralPermissionChanged())
    {    
        m_dwWGRPermition = dlg.GetGeneralPermition();

        if (m_dwWGRPermition == INIT_WITH_ALL_PROHIBITED)
        {
            SetTheCheckButtonChecked(IDC_CHECK_SALLWGESTURE, false);

            //m_dwWGRPermition = INIT_WITH_ALL_PROHIBITED;
        } 
        else if (m_dwWGRPermition == INIT_WITH_ALL_PERMITED)
        {
            SetTheCheckButtonChecked(IDC_CHECK_SALLWGESTURE, true);

            //m_dwWGRPermition = INIT_WITH_ALL_PERMITED;
        }

        SetModified(TRUE);
    }
}

void CGestureSettingDlg::OnBnClickedButtonGlbgrmore()
{
    // TODO: Add your control notification handler code here
    //CGLBGRSettingDlg dlg;
}
