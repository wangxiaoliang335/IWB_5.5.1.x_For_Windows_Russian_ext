// SimulateCalibarationParams.cpp : implementation file
//

#include "stdafx.h"
//#include "headers.h"


// CSimulateCalibarationParamDlg dialog

IMPLEMENT_DYNAMIC(CSimulateCalibarationParamDlg, CDialog)

CSimulateCalibarationParamDlg::CSimulateCalibarationParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSimulateCalibarationParamDlg::IDD, pParent)
    ,m_uCx(1024)
    ,m_uCy(768)
    , m_uImageAvgBrightness(50)
    , m_nSelectSensorId(0)
{

}

CSimulateCalibarationParamDlg::~CSimulateCalibarationParamDlg()
{
}

void CSimulateCalibarationParamDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_CALIBRATION_TYPE, m_cbCalibrationType);
    DDX_Text(pDX, IDC_EDIT_SCREEN_CX, m_uCx);
    DDX_Text(pDX, IDC_EDIT_SCREEN_CY, m_uCy);
    DDX_Text(pDX, IDC_EDIT_IMAGE_BRIGHTNESS, m_uImageAvgBrightness);
    DDX_Radio(pDX, IDC_RADIO_SENSOR1, m_nSelectSensorId);
}


BEGIN_MESSAGE_MAP(CSimulateCalibarationParamDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_FOR_FILE_PATH, &CSimulateCalibarationParamDlg::OnBnClickedButtonBrowseForFilePath)
    ON_BN_CLICKED(IDOK, &CSimulateCalibarationParamDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSimulateCalibarationParamDlg message handlers

void CSimulateCalibarationParamDlg::OnBnClickedButtonBrowseForFilePath()
{
	// TODO: Add your control notification handler code here
	CFileDialog fileDlg(TRUE, _T(".avi"));
	
	if(fileDlg.DoModal() == IDOK)
	{
		this->m_strAVIFilePath = fileDlg.GetPathName();
		GetDlgItem(IDC_EDIT_AVI_FILE_PATH)->SetWindowText(this->m_strAVIFilePath);


        const TCHAR* pSeperator = _tcsrchr(m_strAVIFilePath,_T('\\'));

        if(pSeperator)
        {
             const TCHAR* pSquareBracket = _tcsrchr(m_strAVIFilePath,_T('['));
             
             if(pSquareBracket)
             {
                 int pattern, width, height, brightness;
                 int nGet = 
                 _stscanf_s(pSquareBracket,
                     _T("[pattern %d, screen %dX%d,brightness %d]"),
                     &pattern,
                     &width,
                     &height,
                     &brightness);

                 if(nGet == 4)
                 {
                    m_uCx = width;
                    m_uCy = height;
                    m_uImageAvgBrightness = brightness;

                    if(pattern >= 9)
                    {
                        m_cbCalibrationType.SetCurSel( pattern - 9);
                    }
                    UpdateData(FALSE);
                 }
             }//if
        }//if

	}
}

BOOL CSimulateCalibarationParamDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	this->m_cbCalibrationType.SetCurSel(8);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSimulateCalibarationParamDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
    UpdateData(TRUE);
	this->m_eAutoCalibrateGridsNumber = E_AutoCalibratePattern(m_cbCalibrationType.GetCurSel());
	CDialog::OnOK();
}

void CSimulateCalibarationParamDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    OnOK();
}
