// SparateOperationSettingDlg.cpp : implementation file
//
#include "stdafx.h"
#include "SparateOperationSettingDlg.h"

// CSparateOperationSettingDlg dialog

IMPLEMENT_DYNAMIC(CSparateOperationSettingDlg, CScrollablePropertyPage)

CSparateOperationSettingDlg::CSparateOperationSettingDlg()
	: CScrollablePropertyPage(CSparateOperationSettingDlg::IDD)
	, m_IsAirOperationPermitted(FALSE)
	, m_nAPClick(0)
	, m_bInited(false)
{

}

CSparateOperationSettingDlg::~CSparateOperationSettingDlg()
{

}

void CSparateOperationSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_AIROPERATION, m_IsAirOperationPermitted);
	DDX_Radio(pDX, IDC_RADIO_CLICK, m_nAPClick);
}


BEGIN_MESSAGE_MAP(CSparateOperationSettingDlg, CScrollablePropertyPage)

	ON_BN_CLICKED(IDC_CHECK_AIROPERATION, &CSparateOperationSettingDlg::OnBnClickedCheckAiroperation)
	ON_BN_CLICKED(IDC_RADIO_CLICK, &CSparateOperationSettingDlg::OnBnClickedRadioClick)
	ON_BN_CLICKED(IDC_RADIO_DOUBLECLICK, &CSparateOperationSettingDlg::OnBnClickedRadioDoubleclick)

END_MESSAGE_MAP()


// CSparateOperationSettingDlg message handlers
BOOL CSparateOperationSettingDlg::OnInitDialog()
{
	CScrollablePropertyPage::OnInitDialog();

	m_bInited = TRUE;
	UpdateData(FALSE);

	return  TRUE;
}

BOOL CSparateOperationSettingDlg::OnApply()
{
	if (!m_bInited) return FALSE;
	UpdateData(TRUE);
	CPropertyPage::OnApply();
	SetModified(FALSE);
	//通知主窗体应用设置

	return TRUE;
}

void CSparateOperationSettingDlg::OnBnClickedCheckAiroperation()
{

	// TODO: Add your control notification handler code here
	if (!m_bInited) return;
	if (IsDlgButtonChecked(IDC_CHECK_AIROPERATION) == BST_CHECKED)
	{
		m_IsAirOperationPermitted = TRUE;
	}
	else
	{
		m_IsAirOperationPermitted = FALSE;
	}
	//更新到变量中去
	UpdateData(TRUE);
	SetModified(TRUE);
}


void CSparateOperationSettingDlg::OnBnClickedRadioClick()
{
	// TODO: Add your control notification handler code here
	if (!m_bInited) return;
	if (IsDlgButtonChecked(IDC_RADIO_CLICK) == BST_CHECKED)
	{
		m_nAPClick = E_MODE_CLICK;
	}
	//更新到变量中去
	UpdateData(TRUE);
	SetModified(TRUE);
}


void CSparateOperationSettingDlg::OnBnClickedRadioDoubleclick()
{
	// TODO: Add your control notification handler code here
	if (!m_bInited) return;
	if (IsDlgButtonChecked(IDC_RADIO_DOUBLECLICK) == BST_CHECKED)
	{
		m_nAPClick = E_MODE_DOUBLE_CLICK;
	}
	//更新到变量中去
	UpdateData(TRUE);
	SetModified(TRUE);
}
