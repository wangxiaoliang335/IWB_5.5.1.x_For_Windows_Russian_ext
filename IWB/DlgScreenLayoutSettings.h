#pragma once


// CDlgScreenLayoutSettings dialog
#include "Config.h"
#include "afxwin.h"
class CDlgScreenLayoutSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgScreenLayoutSettings)

public:
	CDlgScreenLayoutSettings(const SplitMode& splitMode, EScreenTargetType eScreenTargetType, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgScreenLayoutSettings();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SCREENLAYOUT_SETTINGS };
#endif

    const SplitMode& GetSplitMode()const;
    EScreenTargetType GetScreenTargetType()const;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    SplitMode m_splitMode;
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    EScreenTargetType m_eScreenTargetType;
};
