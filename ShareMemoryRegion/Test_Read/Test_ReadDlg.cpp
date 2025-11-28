
// Test_ReadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Test_Read.h"
#include "Test_ReadDlg.h"

#include "../inc/ShareMemoryRegion.h"
#ifdef _DEBUG
#pragma comment(lib, "../lib/Debug/ShareMemoryRegion.lib")
#else
#pragma comment(lib, "../lib/Release/ShareMemoryRegion.lib")
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTest_ReadDlg dialog




CTest_ReadDlg::CTest_ReadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTest_ReadDlg::IDD, pParent)
	, m_edit(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTest_ReadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_EDIT_READ, m_edtRead);
	DDX_Control(pDX, IDC_EDIT_READ2, m_CeditRead);
}

BEGIN_MESSAGE_MAP(CTest_ReadDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
//	ON_EN_CHANGE(IDC_EDIT1, &CTest_ReadDlg::OnEnChangeEdit1)
/*	ON_BN_CLICKED(IDOK, &CTest_ReadDlg::OnBnClickedOk)*/
	ON_BN_CLICKED(IDCANCEL, &CTest_ReadDlg::OnBnClickedCancel)
	ON_WM_TIMER()

	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CTest_ReadDlg message handlers

BOOL CTest_ReadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	SMR_Init(0);
	SetTimer(1, 100, NULL);


	return TRUE;  // return TRUE  unless you set the focus to a control
}
void CTest_ReadDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	SMR_Uninit(0);
	CDialog::OnClose();
}
void CTest_ReadDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTest_ReadDlg::OnPaint()
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
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTest_ReadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 
// void CTest_ReadDlg::OnEnChangeEdit1()
// {
// 	// TODO:  If this is a RICHEDIT control, the control will not
// 	// send this notification unless you override the CDialog::OnInitDialog()
// 	// function and call CRichEditCtrl().SetEventMask()
// 	// with the ENM_CHANGE flag ORed into the mask.
// 
// 	// TODO:  Add your control notification handler code here
// }

// void CTest_ReadDlg::OnBnClickedOk()
// {
// 	// TODO: Add your control notification handler code here
// 
// 
// 	unsigned char sz[18];
// 	memset(sz, 0, sizeof(sz));
// 
// 	 int nRet = SMR_Read(NULL,NULL,sz,16,TRUE);
// 
// 	CString  m_Tempedit(sz);
// //	m_Tempedit(sz);
// 	
// 	GetDlgItem(IDC_EDIT1)->SetWindowText(m_Tempedit);
// 
// 
// //	OnOK();
// }

void CTest_ReadDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	SMR_Uninit(0);
	OnCancel();
}

void CTest_ReadDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	DWORD dwWaitResult = SMR_WaitForWriteEvent(0);

	if(WAIT_OBJECT_0 == dwWaitResult)
	{
	//	BYTE buf[32];
		CString m_str(_T("TBorad"));
		CString m_Vstr(_T("1.1.4"));

		char m_Namesz[30];
		memset(m_Namesz,0,sizeof(m_Namesz));
		char m_versionsz[10];
		memset(m_versionsz,0,sizeof(m_versionsz));
		UINT nDataLen = 0 ;


		nDataLen = SMR_ReadDataBlockInfo(m_Namesz,sizeof(m_Namesz),m_versionsz,sizeof(m_versionsz));

		BYTE * buf = new BYTE[nDataLen];

		int length = SMR_Read((char*)CT2A(m_str), (char*)CT2A(m_Vstr) ,buf,nDataLen, 0);

		CString strAllData;
		CString strAllData1;

		for(int i=0; i<length; i++)
		{
			CString strVal;
			CString strVal1;

			strVal.Format(_T("%02x "), buf[i]);
			strVal1.Format(_T("%c") , buf[i]);


			strAllData += strVal;
			strAllData1 += strVal1;

			if((i+1)%16 == 0)
			{
				strAllData += _T("\r\n");
			}
		}	
			
		this->m_edtRead.SetSel(this->m_edtRead.GetWindowTextLength(),-1);
    	this->m_edtRead.ReplaceSel(strAllData);

		this->m_CeditRead.SetSel(this->m_CeditRead.GetWindowTextLength(),-1);

		this->m_CeditRead.ReplaceSel(strAllData1);

		delete[]buf;

	}
	

	CDialog::OnTimer(nIDEvent);
}



