
// Test_ReadDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CTest_ReadDlg dialog
class CTest_ReadDlg : public CDialog
{
// Construction
public:
	CTest_ReadDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TEST_READ_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnEnChangeEdit1();
/*	afx_msg void OnBnClickedOk();*/
	CString m_edit;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CEdit m_edtRead;
	CEdit m_CeditRead;

	afx_msg void OnEnChangeEditRead();
	afx_msg void OnClose();

};
