// DlgScreenLayoutSettings.cpp : implementation file
//

#include "stdafx.h"
#include "IWB.h"
#include "DlgScreenLayoutSettings.h"
#include "afxdialogex.h"


// CDlgScreenLayoutSettings dialog

IMPLEMENT_DYNAMIC(CDlgScreenLayoutSettings, CDialogEx)

CDlgScreenLayoutSettings::CDlgScreenLayoutSettings(const SplitMode& splitMode, EScreenTargetType eScreenTargetType, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SCREENLAYOUT_SETTINGS, pParent)
    , m_eScreenTargetType(eScreenTargetType)
{
    m_splitMode = splitMode;
}

CDlgScreenLayoutSettings::~CDlgScreenLayoutSettings()
{
}

void CDlgScreenLayoutSettings::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_ROWS, m_splitMode.rows);
    DDX_Text(pDX, IDC_EDIT_COLS, m_splitMode.cols);

    DDV_MinMaxInt(pDX, m_splitMode.rows, 1, 20);
    DDV_MinMaxInt(pDX, m_splitMode.cols, 1, 20);
    DDX_Radio(pDX, IDC_RADIO_SCREENTYPE_PRIMARY, (int&)m_eScreenTargetType);
}


BEGIN_MESSAGE_MAP(CDlgScreenLayoutSettings, CDialogEx)
END_MESSAGE_MAP()


// CDlgScreenLayoutSettings message handlers
const SplitMode&  CDlgScreenLayoutSettings::GetSplitMode()const
{
    return m_splitMode;
}

EScreenTargetType CDlgScreenLayoutSettings::GetScreenTargetType()const
{
    return (EScreenTargetType)m_eScreenTargetType;
}

BOOL CDlgScreenLayoutSettings::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  Add extra initialization here
    CRect windowRect;
    GetWindowRect( &windowRect);

    UINT windowFlags = 0;// SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE;

    this->SetWindowPos(
        &wndTopMost,
        windowRect.left,
        windowRect.top,
        windowRect.Width(),
        windowRect.Height(),
        windowFlags);



    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
