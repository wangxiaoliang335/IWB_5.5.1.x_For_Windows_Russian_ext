 // SpotSettingDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "headers.h"

// CSpotSettingDlg dialog

IMPLEMENT_DYNAMIC(CSpotSettingDlg, CDialog)

CSpotSettingDlg::CSpotSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpotSettingDlg::IDD, pParent)
// 	, m_y_Spot(0)
// 	, m_s_Spot(0)
// 	, m_x_Spot(0)
, m_x_Spot(0)
, m_y_Spot(0)
, m_s_Spot(0)
{

}

CSpotSettingDlg::~CSpotSettingDlg()
{
}

void CSpotSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	// 	DDX_Text(pDX, IDC_X_EDIT, m_x_Spot);
	// 	DDX_Text(pDX, IDC_Y_EDIT, m_y_Spot);
	// 	DDX_Text(pDX, IDC_S_EDIT, m_s_Spot);

	DDX_Text(pDX, IDC_X_EDIT, m_x_Spot);
	DDX_Text(pDX, IDC_Y_EDIT, m_y_Spot);
	DDX_Text(pDX, IDC_S_EDIT, m_s_Spot);
}


BEGIN_MESSAGE_MAP(CSpotSettingDlg, CDialog)
END_MESSAGE_MAP()


// CSpotSettingDlg message handlers
BOOL CSpotSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	::SetWindowPos(m_hwnd,
		HWND_TOPMOST,50,50,100,100,
		SWP_SHOWWINDOW|SWP_NOSIZE);

	return TRUE;
}