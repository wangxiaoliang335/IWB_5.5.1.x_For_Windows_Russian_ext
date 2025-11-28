#pragma once


// CDebugVideoLoaderDlg dialog

class CDebugVideoLoaderDlg : public CDialog
{
	DECLARE_DYNAMIC(CDebugVideoLoaderDlg)

public:
	CDebugVideoLoaderDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDebugVideoLoaderDlg();

    LPCTSTR Video1Path()const{return m_strVideoPath1;}
    LPCTSTR Video2Path()const{return m_strVideoPath2;}
// Dialog Data
	enum { IDD = IDD_DIALOG_DEBUG_VIDEO_LOADER};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonFileSelect01();
    afx_msg void OnBnClickedButtonFileSelect2();

protected:
    CString m_strVideoPath1;
    CString m_strVideoPath2;
public:
    afx_msg void OnBnClickedOk();
};
