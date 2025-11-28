// WGRConfigDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "headers.h"
#include "WGRConfigDlg.h"
#include "AddExceptionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWGRConfigDlg dialog
CWGRConfigDlg::CWGRConfigDlg(CWnd* pParent /*=NULL*/)
: CDialog(CWGRConfigDlg::IDD, pParent)
, m_curSelException(-1)
, m_curSelExceptionProcName("")
, m_nCurCheckedItem(-1)
, m_radioGeneralHasChecked(false)
, m_bInited(false)
//, m_nMultPalm(4)
, m_hasGeneralPermitChanged(false)
/* , m_bGeneralHasChanged(false)
, m_bExceptionalHasChanged(false)*/
{
    m_generalPermission = INIT_WITH_ALL_PERMITED;
    m_curSelExceptionPermission = INIT_WITH_ALL_PROHIBITED;

    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWGRConfigDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_EXCEPTION, m_exceptionList);       

    DDX_Control(pDX, IDC_RADIO_GENERAL, m_radioGeneral); 
    DDX_Control(pDX, IDC_RADIO_Exceptional, m_radioExceptional);
    DDX_Control(pDX, IDC_BTN_ADDEXCEPTION, m_btnAddException);
    DDX_Control(pDX, IDC_BTN_DELEXCEPTION, m_btnDeletException);

    //DDX_Text(pDX, IDC_EDIT_MULT_PALM, m_nMultPalm);
    //DDX_Control(pDX, IDC_SPIN_MULT_PALM, m_sMultPalm);
}

BEGIN_MESSAGE_MAP(CWGRConfigDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP    
    ON_BN_CLICKED(IDC_RADIO_GENERAL, &CWGRConfigDlg::OnBnClickedRadioGeneral)
    ON_BN_CLICKED(IDC_RADIO_Exceptional, &CWGRConfigDlg::OnBnClickedRadioExceptional)  

    ON_BN_CLICKED(IDOK, &CWGRConfigDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CWGRConfigDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_CHK_PALL, &CWGRConfigDlg::OnBnClickedChkPall)
    ON_BN_CLICKED(IDC_CHK_PCLSWND, &CWGRConfigDlg::OnBnClickedChkPclswnd)
    ON_BN_CLICKED(IDC_CHK_PTITLEWNDS, &CWGRConfigDlg::OnBnClickedChkPtitlewnds)
    ON_BN_CLICKED(IDC_CHK_PMOVWND, &CWGRConfigDlg::OnBnClickedChkPmovwnd)
    ON_BN_CLICKED(IDC_CHK_PREFWND, &CWGRConfigDlg::OnBnClickedChkPrefwnd)
    ON_BN_CLICKED(IDC_CHK_FALL, &CWGRConfigDlg::OnBnClickedChkFall)
    ON_BN_CLICKED(IDC_CHK_FMOUWHEEL, &CWGRConfigDlg::OnBnClickedChkFmouwheel)
    ON_BN_CLICKED(IDC_CHK_FDISDESKTOP, &CWGRConfigDlg::OnBnClickedChkFdisdesktop)
    ON_BN_CLICKED(IDC_CHK_FMAXICURWND, &CWGRConfigDlg::OnBnClickedChkFmaxicurwnd)
    ON_BN_CLICKED(IDC_CHK_FMINICURWND, &CWGRConfigDlg::OnBnClickedChkFminicurwnd)
    ON_BN_CLICKED(IDC_CHK_FSWITCHWND, &CWGRConfigDlg::OnBnClickedChkFswitchwnd)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_EXCEPTION, &CWGRConfigDlg::OnLvnItemchangedListException)
    ON_BN_CLICKED(IDC_BTN_ADDEXCEPTION, &CWGRConfigDlg::OnBnClickedBtnAddexception)
    ON_BN_CLICKED(IDC_BTN_DELEXCEPTION, &CWGRConfigDlg::OnBnClickedBtnDelexception)
    ON_NOTIFY(NM_CLICK, IDC_LIST_EXCEPTION, &CWGRConfigDlg::OnNMClickListException)
    ON_BN_CLICKED(IDC_CHK_LOG, &CWGRConfigDlg::OnBnClickedChkLog)

    ON_EN_CHANGE(IDC_EDIT_MULT_PALM, &CWGRConfigDlg::OnEnChangeEditMultPalm)
END_MESSAGE_MAP()


// CWGRConfigDlg message handlers

BOOL CWGRConfigDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    //m_sMultPalm.SetRange(3,7);

    //初始化General和Exceptional列表
    InitRadioButton();
    UpdateCheckStateByGeneralPermission();

    //初始化其他设置相关控件
    InitOtherSetting();

    //初始化Exceptional列表
    InitExceptionList();
    UpdateExceptionListState();

    UpdateAddExceptionButtonState();
    UpdateDelExceptionButtonState();

    m_hasGeneralPermitChanged = false;

    // TODO: Add extra initialization here    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWGRConfigDlg::OnPaint()
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
HCURSOR CWGRConfigDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CWGRConfigDlg::InitRadioButton(void)
{
    m_radioGeneral.SetCheck(TRUE);
    m_radioGeneralHasChecked = true;
    EnableTheButton(IDOK, false);
    //初始化General手势相关的控件
}

void CWGRConfigDlg::InitOtherSetting(void)
{
    int nCheck = g_oWGRConfig.IsLogPermitted() ? 1 : 0;
    UpdateTheCheckButtonState(IDC_CHK_LOG, nCheck);

    //m_nMultPalm = g_oWGRConfig.GetMultiplePalm2Std(); 
    //m_nMultPalm = m_nMultPalm >= MULT_ERASER_MIN ? m_nMultPalm : MULT_ERASER_MIN;
    //m_nMultPalm = m_nMultPalm <= MULT_ERASER_MAX ? m_nMultPalm : MULT_ERASER_MAX;

    UpdateData(FALSE);

    m_bInited = true;
}

void CWGRConfigDlg::InitExceptionList(void)
{
    RECT rect;
    m_exceptionList.GetClientRect(&rect);

    int nColWidth = (rect.right - rect.left) >> 1;

    //
    m_exceptionList.SetExtendedStyle(m_exceptionList.GetExtendedStyle() | LVS_EX_CHECKBOXES);

    LVCOLUMNW lvCol;
    lvCol.fmt = LVCFMT_CENTER;
    lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH |LVCF_SUBITEM;
    lvCol.pszText = _T("Process Name");
    lvCol.cx = nColWidth;
    lvCol.iSubItem = -1;

    m_exceptionList.InsertColumn(0, &lvCol);

    lvCol.fmt = LVCFMT_CENTER;
    lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH |LVCF_SUBITEM;
    lvCol.pszText = _T("Application Name");
    lvCol.cx = nColWidth;
    lvCol.iSubItem = -1;

    m_exceptionList.InsertColumn(1, &lvCol);    

    const std::list<ST_WGR_EXCEPTION> &lst = g_oWGRConfig.GetExceptionalProcessList();

    std::list<ST_WGR_EXCEPTION>::const_iterator itr = lst.begin();

    int nIndex = 0;
    for (; itr != lst.end(); itr++)
    {    
        if ((*itr).procName != "")
        {            
            m_exceptionList.InsertItem(nIndex, CA2T((*itr).procName.c_str()));
            m_exceptionList.SetItemText(nIndex, 1, CA2T((*itr).apppName.c_str()));
            nIndex++;
        }
    }
}

void CWGRConfigDlg::UpdateExceptionListState(void)
{   
    m_exceptionList.EnableWindow(!m_radioGeneral.GetCheck());
}

void CWGRConfigDlg::UpdateAddExceptionButtonState()
{
    BOOL checkState = m_radioGeneral.GetCheck();
    m_btnAddException.EnableWindow(!checkState);  
}

void CWGRConfigDlg::UpdateDelExceptionButtonState(void)
{
    BOOL checkState = m_radioGeneral.GetCheck();    

    if (checkState)
    {
        m_btnDeletException.EnableWindow(!checkState); 
    }
    else
    {
        m_btnDeletException.EnableWindow((m_nCurCheckedItem != -1));  
    }
}

void CWGRConfigDlg::UpdateCheckStateByPermission(const DWORD &perm)
{
    bool isPermited = true;

    isPermited = CWGRPermissionParse::IsTheEventPermited(E_TEP_ALL, perm);
    if (isPermited)
    {
        UpdateTheCheckButtonState(IDC_CHK_PALL, 1);
        UpdateTheCheckButtonState(IDC_CHK_PCLSWND, 1);
        UpdateTheCheckButtonState(IDC_CHK_PMOVWND, 1);
        UpdateTheCheckButtonState(IDC_CHK_PREFWND, 1);
        UpdateTheCheckButtonState(IDC_CHK_PTITLEWNDS, 1);        
    }
    else
    {
        UpdateTheCheckButtonState(IDC_CHK_PALL, 0);

        isPermited = CWGRPermissionParse::IsTheEventPermited(E_TEP_LOWLIM_ALL, perm);
        UpdateTheCheckButtonState(IDC_CHK_PTITLEWNDS, isPermited ? 1 : 0);

        isPermited = CWGRPermissionParse::IsTheEventPermited(E_TEP_NSPLOC_CLSCURWND, perm);
        UpdateTheCheckButtonState(IDC_CHK_PCLSWND, isPermited ? 1 : 0);

        isPermited = CWGRPermissionParse::IsTheEventPermited(E_TEP_NSPLOC_REFRESHWND, perm);
        UpdateTheCheckButtonState(IDC_CHK_PREFWND, isPermited ? 1 : 0);

        isPermited = CWGRPermissionParse::IsTheEventPermited(E_TEP_NSPLOC_MOVWND, perm);
        UpdateTheCheckButtonState(IDC_CHK_PMOVWND, isPermited ? 1 : 0);
    }

    isPermited = CWGRPermissionParse::IsTheEventPermited(E_TEF_ALL, perm);
    if (isPermited)
    {
        UpdateTheCheckButtonState(IDC_CHK_FALL, 1);
        UpdateTheCheckButtonState(IDC_CHK_FMOUWHEEL, 1);
        UpdateTheCheckButtonState(IDC_CHK_FDISDESKTOP, 1);
        UpdateTheCheckButtonState(IDC_CHK_FMAXICURWND, 1);
        UpdateTheCheckButtonState(IDC_CHK_FMINICURWND, 1);        
        UpdateTheCheckButtonState(IDC_CHK_FSWITCHWND, 1); 
    }
    else
    {
        UpdateTheCheckButtonState(IDC_CHK_FALL, 0);

        isPermited = CWGRPermissionParse::IsTheEventPermited(E_TEF_2TOUCHS_ALL, perm);
        UpdateTheCheckButtonState(IDC_CHK_FMOUWHEEL, isPermited ? 1 : 0);

        isPermited = CWGRPermissionParse::IsTheEventPermited(E_TEF_6MTOUCHS_ALL, perm);
        UpdateTheCheckButtonState(IDC_CHK_FDISDESKTOP, isPermited ? 1 : 0);

        isPermited = CWGRPermissionParse::IsTheEventPermited(E_TEF_35TOUCHS_MAXICURWND, perm);
        UpdateTheCheckButtonState(IDC_CHK_FMAXICURWND, isPermited ? 1 : 0);

        isPermited = CWGRPermissionParse::IsTheEventPermited(E_TEF_35TOUCHS_MINICURWND, perm);
        UpdateTheCheckButtonState(IDC_CHK_FMINICURWND, isPermited ? 1 : 0);

        isPermited = CWGRPermissionParse::IsTheEventPermited(E_TEF_35TOUCHS_SWHWND, perm);
        UpdateTheCheckButtonState(IDC_CHK_FSWITCHWND, isPermited ? 1 : 0);
    }    
}

void CWGRConfigDlg::UpdateCheckStateByGeneralPermission(void)
{
    m_generalPermission = g_oWGRConfig.GetGeneralPermission();

    UpdateCheckStateByPermission(m_generalPermission);

    EnableAllCheckButton(true);
}

void CWGRConfigDlg::UpdateGeneralPermissionByCheckState(void)
{}

void CWGRConfigDlg::UpdateCheckStateByCurSelExceptionPermission(void)
{
    do
    {
        int nItemCount = m_exceptionList.GetItemCount();
        if (nItemCount <= 0)
        {
            m_curSelExceptionPermission = INIT_WITH_ALL_PROHIBITED;
            EnableAllCheckButton(false);
            break;
        }

        if (m_nCurCheckedItem == -1)
        {
            EnableAllCheckButton(false);
            break;
        }

        EnableAllCheckButton(true);

        TCHAR procNameBuffer[256];
        m_exceptionList.GetItemText(m_nCurCheckedItem, 0, procNameBuffer, 256);

        m_curSelExceptionProcName = CT2A(procNameBuffer);
        m_curSelExceptionPermission = g_oWGRConfig.GetExceptionPermission(m_curSelExceptionProcName);

    }while(false);

    UpdateCheckStateByPermission(m_curSelExceptionPermission);
    UpdateExceptionListState();
    UpdateAddExceptionButtonState();
    UpdateDelExceptionButtonState();    
}

void CWGRConfigDlg::UpdateCurSelExceptionByCheckState(void)
{}

void CWGRConfigDlg::EnableAllCheckButton(bool bEnable)
{
    EnableTheCheckButton(IDC_CHK_PALL, bEnable);
    //EnableTheCheckButton(IDC_CHK_PCLSWND, bEnable);
    EnableTheCheckButton(IDC_CHK_PMOVWND, bEnable);
    //EnableTheCheckButton(IDC_CHK_PREFWND, bEnable);
    EnableTheCheckButton(IDC_CHK_PTITLEWNDS, bEnable);   

    EnableTheCheckButton(IDC_CHK_FALL, bEnable);
    EnableTheCheckButton(IDC_CHK_FMOUWHEEL, bEnable);
    EnableTheCheckButton(IDC_CHK_FDISDESKTOP, bEnable);
    EnableTheCheckButton(IDC_CHK_FMAXICURWND, bEnable);
    EnableTheCheckButton(IDC_CHK_FMINICURWND, bEnable);        
    EnableTheCheckButton(IDC_CHK_FSWITCHWND, bEnable); 
}

void CWGRConfigDlg::OnBnClickedRadioGeneral()
{
    // TODO: Add your control notification handler code here
    if (m_radioGeneralHasChecked) return;
    m_radioGeneralHasChecked = true;

    UpdateExceptionListState();
    UpdateAddExceptionButtonState();
    UpdateDelExceptionButtonState();
    UpdateCheckStateByGeneralPermission();
    EnableTheButton(IDOK, false);
}

void CWGRConfigDlg::OnBnClickedRadioExceptional()
{
    // TODO: Add your control notification handler code here
    if (!m_radioGeneralHasChecked) return;
    m_radioGeneralHasChecked = false;

    UpdateExceptionListState();
    UpdateAddExceptionButtonState();
    UpdateDelExceptionButtonState();
    UpdateCheckStateByCurSelExceptionPermission();
    EnableTheButton(IDOK, false);
}

void CWGRConfigDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here 
    UpdateData();
    g_oWGRConfig.SetGeneralPermission(m_generalPermission);
    g_oWGRConfig.SetExcetionPermission(m_curSelExceptionPermission, m_curSelExceptionProcName);
    g_oWGRConfig.SetLogPermitted(IsTheCheckButtonChecked(IDC_CHK_LOG));
    //g_oWGRConfig.SetMultiplePalm2Std(m_nMultPalm);

    EnableTheButton(IDOK, false);
    //OnOK();
}

void CWGRConfigDlg::OnBnClickedCancel()
{
    // TODO: Add your control notification handler code here
    OnCancel();
}

void CWGRConfigDlg::OnBnClickedChkPall()
{
    // TODO: Add your control notification handler code here 
    AfterTheCheckButtonClicked(IDC_CHK_PALL, E_TEP_ALL);

    if(m_radioGeneralHasChecked)
    {
        m_hasGeneralPermitChanged = true;
    }
}

void CWGRConfigDlg::OnBnClickedChkPclswnd()
{
    // TODO: Add your control notification handler code here
    AfterTheCheckButtonClicked(IDC_CHK_PCLSWND, E_TEP_NSPLOC_CLSCURWND);

    if(m_radioGeneralHasChecked)
    {
        m_hasGeneralPermitChanged = true;
    }
}

void CWGRConfigDlg::OnBnClickedChkPtitlewnds()
{
    // TODO: Add your control notification handler code here
    AfterTheCheckButtonClicked(IDC_CHK_PTITLEWNDS, E_TEP_LOWLIM_ALL);

    if(m_radioGeneralHasChecked)
    {
        m_hasGeneralPermitChanged = true;
    }
}

void CWGRConfigDlg::OnBnClickedChkPmovwnd()
{
    // TODO: Add your control notification handler code here
    AfterTheCheckButtonClicked(IDC_CHK_PMOVWND, E_TEP_NSPLOC_MOVWND);

    if(m_radioGeneralHasChecked)
    {
        m_hasGeneralPermitChanged = true;
    }
}

void CWGRConfigDlg::OnBnClickedChkPrefwnd()
{
    // TODO: Add your control notification handler code here
    AfterTheCheckButtonClicked(IDC_CHK_PREFWND, E_TEP_NSPLOC_REFRESHWND);

    if(m_radioGeneralHasChecked)
    {
        m_hasGeneralPermitChanged = true;
    }
}

void CWGRConfigDlg::OnBnClickedChkFall()
{
    // TODO: Add your control notification handler code here
    AfterTheCheckButtonClicked(IDC_CHK_FALL, E_TEF_ALL);

    if(m_radioGeneralHasChecked)
    {
        m_hasGeneralPermitChanged = true;
    }
}

void CWGRConfigDlg::OnBnClickedChkFmouwheel()
{
    // TODO: Add your control notification handler code here
    AfterTheCheckButtonClicked(IDC_CHK_FMOUWHEEL, E_TEF_2TOUCHS_ALL);

    if(m_radioGeneralHasChecked)
    {
        m_hasGeneralPermitChanged = true;
    }
}

void CWGRConfigDlg::OnBnClickedChkFdisdesktop()
{
    // TODO: Add your control notification handler code here
    AfterTheCheckButtonClicked(IDC_CHK_FDISDESKTOP, E_TEF_6MTOUCHS_ALL);

    if(m_radioGeneralHasChecked)
    {
        m_hasGeneralPermitChanged = true;
    }
}

void CWGRConfigDlg::OnBnClickedChkFmaxicurwnd()
{
    // TODO: Add your control notification handler code here
    AfterTheCheckButtonClicked(IDC_CHK_FMAXICURWND, E_TEF_35TOUCHS_MAXICURWND);

    if(m_radioGeneralHasChecked)
    {
        m_hasGeneralPermitChanged = true;
    }
}

void CWGRConfigDlg::OnBnClickedChkFminicurwnd()
{
    // TODO: Add your control notification handler code here
    AfterTheCheckButtonClicked(IDC_CHK_FMINICURWND, E_TEF_35TOUCHS_MINICURWND);

    if(m_radioGeneralHasChecked)
    {
        m_hasGeneralPermitChanged = true;
    }
}

void CWGRConfigDlg::OnBnClickedChkFswitchwnd()
{
    // TODO: Add your control notification handler code here
    AfterTheCheckButtonClicked(IDC_CHK_FSWITCHWND, E_TEF_35TOUCHS_SWHWND);

    if(m_radioGeneralHasChecked)
    {
        m_hasGeneralPermitChanged = true;
    }
}

void CWGRConfigDlg::OnLvnItemchangedListException(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    //UpdateCheckStateByCurSelExceptionPermission();
    *pResult = 0;
}

void CWGRConfigDlg::OnBnClickedBtnAddexception()
{
    // TODO: Add your control notification handler code here
    CAddExceptionDlg dlg;

    char procNameArry[256][256];
    memset(procNameArry, 0, sizeof(procNameArry));
    int nActualProcNameCount = g_oWGRConfig.GetCurrentProcessInfo(procNameArry, 256);

    dlg.SetProcessInfo(procNameArry, nActualProcNameCount);

    INT_PTR nRes = dlg.DoModal();
    if (nRes != IDOK) return;

    std::string procName = CT2A(dlg.m_procName);
    std::string appName = CT2A(dlg.m_appName);

    if (procName != "")
    {
        DWORD perm = INIT_WITH_ALL_PROHIBITED;
        bool success = g_oWGRConfig.AddExceptionalProcess(procName, appName, perm);

        if (success)
        {
            int nCount = m_exceptionList.GetItemCount();
            m_exceptionList.InsertItem(nCount, dlg.m_procName);
            m_exceptionList.SetItemText(nCount, 1, dlg.m_appName);
        }

        UpdateCheckStateByCurSelExceptionPermission();
    }
}

void CWGRConfigDlg::OnBnClickedBtnDelexception()
{
    // TODO: Add your control notification handler code here
    int nItemCount = m_exceptionList.GetItemCount();
    if (nItemCount <= 0)
    {
        return;
    }

    if (m_nCurCheckedItem == -1) return;

    TCHAR procNameBuffer[256];
    m_exceptionList.GetItemText(m_nCurCheckedItem, 0, procNameBuffer, 256);

    std::string procName = CT2A(procNameBuffer);
    g_oWGRConfig.DeleteExceptionalProcess(procName);

    m_exceptionList.DeleteItem(m_nCurCheckedItem);

    UpdateCheckStateByCurSelExceptionPermission();
}

void CWGRConfigDlg::MakeExceptionListCtrlSingleChecked(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLISTVIEW *pNMListView = (NMLISTVIEW *)pNMHDR;
    LVHITTESTINFO hitinfo;
    *pResult = 0;

    bool bChecked = false;

    //Copy click point
    hitinfo.pt = pNMListView->ptAction;

    //Make the hit test...
    int nItem = m_exceptionList.HitTest(&hitinfo);

    if (nItem == -1) return;    

    if ((hitinfo.flags != LVHT_ONITEMLABEL) && (hitinfo.flags != LVHT_ONITEMSTATEICON)) return;

    if (m_nCurCheckedItem == nItem)
    {//已经checked
        m_exceptionList.SetCheck(nItem, FALSE); //取消check
        m_exceptionList.SetItemState(nItem, LVIS_SELECTED, 0);
        m_nCurCheckedItem = -1;
    }
    else
    {                
        if (m_nCurCheckedItem != -1)
        {//先撤销check的Item
            m_exceptionList.SetCheck(m_nCurCheckedItem, FALSE);
            m_exceptionList.SetItemState(m_nCurCheckedItem, LVIS_SELECTED, 0);
        }

        m_exceptionList.SetCheck(nItem, TRUE);
        m_nCurCheckedItem = nItem;

        if (hitinfo.flags == LVHT_ONITEMSTATEICON)
        {
            m_exceptionList.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
        }
    }

    *pResult = 1;

    m_exceptionList.Invalidate();
    m_exceptionList.SetRedraw();
}

void CWGRConfigDlg::OnNMClickListException(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here
    //

    MakeExceptionListCtrlSingleChecked(pNMHDR, pResult);

    UpdateCheckStateByCurSelExceptionPermission();
}
void CWGRConfigDlg::OnBnClickedChkLog()
{
    // TODO: Add your control notification handler code here 
    EnableTheButton(IDOK, true);
}

void CWGRConfigDlg::OnEnChangeEditMultPalm()
{
    if (!m_bInited) return;

    CString strText;
    GetDlgItem(IDC_EDIT_MULT_PALM)->GetWindowText(strText);

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