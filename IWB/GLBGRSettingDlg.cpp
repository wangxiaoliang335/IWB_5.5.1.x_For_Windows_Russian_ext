// GLBGRSettingDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "headers.h"
#include "GLBGRSettingDlg.h"


// CGLBGRSettingDlg dialog

IMPLEMENT_DYNAMIC(CGLBGRSettingDlg, CDialog)

CGLBGRSettingDlg::CGLBGRSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGLBGRSettingDlg::IDD, pParent)
    , m_bInited(false)
    , m_nMultEraser(4)
    , m_bGREnabled(true)
    , m_bIsLogPermit(false)
{

}

CGLBGRSettingDlg::~CGLBGRSettingDlg()
{
}

void CGLBGRSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    DDX_Text(pDX, IDC_EDIT_MULT_ERASER, m_nMultEraser);
    DDX_Control(pDX, IDC_SPIN_MULT_ERASER, m_sMultEraser);
}


BEGIN_MESSAGE_MAP(CGLBGRSettingDlg, CDialog)
    ON_EN_CHANGE(IDC_EDIT_MULT_ERASER, &CGLBGRSettingDlg::OnEnChangeEditMultEraser)
    ON_BN_CLICKED(IDOK, &CGLBGRSettingDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CGLBGRSettingDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_CHECK_STARTGLBGR, &CGLBGRSettingDlg::OnBnClickedCheckStartglbgr)
END_MESSAGE_MAP()


// CGLBGRSettingDlg message handlers
BOOL CGLBGRSettingDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_bInited = true;

     m_sMultEraser.SetRange(3,7);

     UpdateCheckBtnStateByGLBGRPermission();

     InitOtherSetting();    

    return TRUE;
}

void CGLBGRSettingDlg::OnEnChangeEditMultEraser()
{
    if (!m_bInited) return;

    CString strText;
    GetDlgItem(IDC_EDIT_MULT_ERASER)->GetWindowText(strText);

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
        GetDlgItem(IDC_EDIT_MULT_ERASER)->SetWindowText(strText);
    }  

    EnableTheButton(IDOK, true);
}

void CGLBGRSettingDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    UpdateData();

    g_oGLBoardGR.SetMultiEraser(m_nMultEraser);
    g_oGLBoardGR.EnableGestureRecognition(m_bGREnabled);
    EnableTheButton(IDOK, false);
    //OnOK();
}

void CGLBGRSettingDlg::OnBnClickedCancel()
{
    // TODO: Add your control notification handler code here
    OnCancel();
}

void CGLBGRSettingDlg::UpdateCheckBtnStateByGLBGRPermission()
{
    m_bGREnabled = g_oGLBoardGR.IsGREnabled();
    UpdateTheCheckButtonState(IDC_CHECK_STARTGLBGR, (m_bGREnabled ? 1 : 0));    
    EnableTheButton(IDOK, false);
}

void CGLBGRSettingDlg::InitOtherSetting(void)
{
    m_nMultEraser = g_oGLBoardGR.GetMultiEraser(); 
    m_nMultEraser = m_nMultEraser >= MULT_ERASER_MIN ? m_nMultEraser : MULT_ERASER_MIN;
    m_nMultEraser = m_nMultEraser <= MULT_ERASER_MAX ? m_nMultEraser : MULT_ERASER_MAX;

    UpdateData(FALSE);
}

void CGLBGRSettingDlg::OnBnClickedCheckStartglbgr()
{
    // TODO: Add your control notification handler code here
    AfterTheCheckButtonClicked(IDC_CHECK_STARTGLBGR);
}
