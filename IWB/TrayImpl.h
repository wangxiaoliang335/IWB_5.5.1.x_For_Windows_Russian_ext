#pragma once

//说明:主要用的API函数Shell_NotifyIcon
//托盘类(taskbar notification area)
#include <strsafe.h>
#pragma comment(lib,"strsafe.lib")
class CTrayImpl
{

public:
	CTrayImpl();

   ~CTrayImpl();

   
	//@功能:往任务栏状态区添加图标
	BOOL AddToSystemTray();

	//@功能:
	BOOL Modify();

	//@功能:从任务栏状态区删除图标
	BOOL Remove();

	//@功能:设置焦点
	BOOL SetFocus();



	//@功能:设置接收托盘消息的窗体的句柄
	void SetWnd(HWND hWnd)
	{
		m_tNotifyIconData.hWnd = hWnd;
	}

	//@功能:设置托盘的ID
	void SetTrayID(UINT uID)
	{
		m_tNotifyIconData.uID = uID;
	}

	//@功能:设置用户程序定义的消息
	void SetCallBackMessage(UINT uMsg)
	{
		m_tNotifyIconData.uCallbackMessage = uMsg;
		m_tNotifyIconData.uFlags |= NIF_MESSAGE;
	}

	//@功能:设置图标句柄
	void SetIcon(HICON hIcon)
	{
		m_tNotifyIconData.hIcon = hIcon;
		m_tNotifyIconData.uFlags |= NIF_ICON;
	}

	//@参数:lpszText,气球提示的文本
	//      lpszTitle,气球提示的标题栏
	//      uTimeout,超时时间设定,单位:ms,范围(10*1000~30*1000)
	//      dwTipIconType,提示图标类型
	void SetBallonTip(LPCTSTR lpszText,LPCTSTR lpszTitle,UINT uTimeout, DWORD dwTipIconType)
	{
		StringCbCopyN(m_tNotifyIconData.szInfo,sizeof(m_tNotifyIconData.szInfo),lpszText,sizeof(m_tNotifyIconData.szInfo));

		StringCbCopyN(m_tNotifyIconData.szInfoTitle,sizeof(m_tNotifyIconData.szInfoTitle),lpszTitle,sizeof(m_tNotifyIconData.szInfoTitle));

		m_tNotifyIconData.uTimeout    = uTimeout;
		m_tNotifyIconData.dwInfoFlags = dwTipIconType;

	}


	void Refresh()
	{
		::Shell_NotifyIcon(NIM_MODIFY,&m_tNotifyIconData);
	}


    //当资源管理器(explore.exe)崩溃后发送用RegisterWindowMessage(TEXT("TaskbarCreated")注册的系统消息
    //到所有应用程序的顶层窗体中, 用来重新启动时重新添加图标到系统托盘中
    //
    //m_uTaskbarRestartMsg = RegisterWindowMessage(TEXT("TaskbarCreated"));
    //ON_REGISTERED_MESSAGE(m_uTaskbarRestartMsg, OnTaskbarRestartMsg)
    //
    //LRESULT COpticalPenDlg::OnTaskbarRestartMsg(WPARAM wParam, LPARAM lParam)
    //{
    //  return 0L;
    //}

    BOOL OnTaskbarCreated();
private:
	NOTIFYICONDATA  m_tNotifyIconData;
	BOOL            m_bInSysTray     ;//图标加入系统托盘标志


};



inline CTrayImpl::CTrayImpl()
:
m_bInSysTray(FALSE)
{
	m_tNotifyIconData.cbSize = sizeof(NOTIFYICONDATA);

	//接收托盘图标通知消息的窗体句柄
	m_tNotifyIconData.hWnd = NULL;

	//用户程序定义的托盘图标的ID号,一个用户窗体可以关联多个图标,通过ID号来区分
	//使用的是那个图标。
	m_tNotifyIconData.uID = 0;

	//标识数据成员有效的标志
	//NIF_ICON    ,hIcon有效
	//NIF_MESSAGE ,uCallBackMessage有效
	//NIF_TIP     ,szTip有效
	//NIF_STATE   ,dwState和dwStateMask成员有效
	//NIF_INFO    ,使用气球提示,szInfo,uTimeout,szInfoTitle,wdInfoFlags,正确。
	m_tNotifyIconData.uFlags = 0;

	//用户程序定义的消息标识符,
	m_tNotifyIconData.uCallbackMessage = 0;

	//添加，修改,删除的图标句柄
	m_tNotifyIconData.hIcon = NULL;

    //标准提示的字符串内容
	StringCbCopyN(m_tNotifyIconData.szTip,sizeof(m_tNotifyIconData.szTip),_T(""),sizeof(m_tNotifyIconData.szTip));

	//图标的状态
	//NIS_HIDDEN,图标隐藏
	//NIS_SHAREDICON,图标是共享的
	m_tNotifyIconData.dwState = NIS_SHAREDICON;

	//控制dwState哪位被检索或者修改的屏蔽字
	//NIS_HIDDEN,则隐藏状态被检索
	m_tNotifyIconData.dwStateMask = NIS_HIDDEN;

	//气球提示的字符串内容
	StringCbCopyN(m_tNotifyIconData.szInfo,sizeof(m_tNotifyIconData.szInfo),_T(""),sizeof(m_tNotifyIconData.szInfo));

	//气球提示的超时时间,单位毫秒,
	//系统限定了范围(10秒~30秒)
	m_tNotifyIconData.uTimeout = 15*1000;

	//版本信息
	//0,Win95行为
	//NOTIFYICON_VERSION,Win2000行为,Win2000以后的操作系统使用
	m_tNotifyIconData.uVersion = NOTIFYICON_VERSION;

	//气球提示的标题栏,以粗体显示在气球提示文本上方
	StringCbCopyN(m_tNotifyIconData.szInfoTitle,sizeof(m_tNotifyIconData.szInfoTitle),_T(""),sizeof(m_tNotifyIconData.szInfoTitle));

	//气球提示的图标类型
	//NIIF_ERROR    : An error icon.
	//NIIF_INFO     : An information icon.
	//NIIF_NONE     : No icon.
	//NIIF_WARNING  : A warning icon.
	//NIIF_ICON_MASK:Version 6.0. Reserved.
	//NIIF_NOSOUND  :Version 6.0. Do not play the associated sound. Applies only to balloon ToolTips.
	m_tNotifyIconData.dwInfoFlags = NIIF_NONE;


	//保留字段
//	m_tNotifyIconData.guidItem = GUID_NULL;

}

inline CTrayImpl::~CTrayImpl()
{
	if(m_bInSysTray)
	{
		Remove();
	}
}

//@功能:往任务栏状态区添加图标
inline BOOL CTrayImpl::AddToSystemTray()
{
    LOG_INF("Enter Function CTrayImpl::AddToSystemTray With m_bInSysTray = %d \n", m_bInSysTray);
	if(m_bInSysTray) return TRUE;
	BOOL bRes = ::Shell_NotifyIcon(NIM_ADD,&m_tNotifyIconData);
	if(bRes)
	{
		m_bInSysTray = TRUE;
	}
		            
	return bRes;
}

//@功能:从任务栏状态区删除图标
inline BOOL CTrayImpl::Remove()
{
    LOG_INF("Enter Function CTrayImpl::Remove with m_bInSysTray = %d \n", m_bInSysTray);
	if(!m_bInSysTray) return TRUE;

	BOOL bRes = ::Shell_NotifyIcon(NIM_DELETE,&m_tNotifyIconData);
	           

	m_bInSysTray = FALSE;

	return bRes;

}
//初始化托盘对象
//m_hVTSIcon = ::LoadIcon(theApp.m_hInstance,MAKEINTRESOURCE(IDI_ICON_VTS));
//m_oTray.SetWnd(m_hWnd);
//m_oTray.SetTrayID(this->GetDlgCtrlID());
//m_oTray.SetIcon(m_hVTSIcon);
//m_oTray.SetCallBackMessage(TRAY_NOTIFY_MSG);
//m_oTray.SetBallonTip(_T("提示信息"),_T(""),1000*15,NIIF_INFO);

//示例
//HRESULT CMainDlg::OnTrayNotifyMsg(WPARAM wParam,LPARAM lParam)
//{
//	UINT uIconID = (UINT)wParam;
// 
//	switch(lParam)
//	{
//	case WM_MOUSEMOVE:
//
//		TRACE("OnTrayNotifyMsg::WM_MOUSE_MOVE\n");
//
//		break;
//
//	case WM_LBUTTONDOWN:
//
//		TRACE("OnTrayNotifyMsg::WM_LBUTTONDOWN\n");
//		break;
//
//	case WM_LBUTTONUP:
//
//		TRACE("OnTrayNotifyMsg::WM_LBUTTONUP\n");
//		break;
//
//	case  WM_RBUTTONDOWN:
//
//
//		TRACE("OnTrayNotifyMsg::WM_RBUTTONDOWN\n");
//		break;
//
//	case WM_RBUTTONUP:
//		{
//
//		//弹出快捷菜单
//	    
//		CMenu menu;
//		menu.LoadMenu(IDR_MENU_CONTEXT);
//		CMenu* pCtxMenu = menu.GetSubMenu(0);
//
//		if(pCtxMenu)
//		{
//			POINT ptCursor;
//			GetCursorPos(&ptCursor);
//			pCtxMenu->TrackPopupMenu(TPM_RIGHTALIGN|TPM_BOTTOMALIGN,ptCursor.x,ptCursor.y,this);
//
//		}
//		TRACE("OnTrayNotifyMsg::WM_RBUTTONUP\n");
//		}
//		break;
//
//	case WM_LBUTTONDBLCLK :
//
//		ShowWindow(SW_SHOW);
//		m_oTray.Remove();
//
//		TRACE("OnTrayNotifyMsg::WM_LBUTTONDBLCLK\n");
//		break;
//
//	case WM_CONTEXTMENU://
//
//		TRACE("OnTrayNotifyMsg::WM_CONTEXTMENU\n");
//		break;
//
//
//
//	case NIN_KEYSELECT:
//		TRACE("OnTrayNotifyMsg::NIN_KEYSELECT\n");
//		break;
//
//
//	case NIN_BALLOONSHOW://Sent when the balloon is shown 
//
//		TRACE("OnTrayNotifyMsg::NIN_BALLOONSHOW\n");
//		break;
//
//		
//
//	case NIN_BALLOONHIDE://Sent when the balloon disappears—for example, when the icon is deleted. This message is not sent if the balloon is dismissed because of a timeout or a mouse click.
//		TRACE("OnTrayNotifyMsg::NIN_BALLOONHIDE\n");
//		break;
//
//
//	case NIN_BALLOONTIMEOUT:// Sent when the balloon is dismissed because of a timeout. 
//		TRACE("OnTrayNotifyMsg::NIN_BALLOONTIMEOUT\n");
//		break;
//
//	case NIN_BALLOONUSERCLICK:// Sent when the balloon is dismissed because of a mouse click.
//		TRACE("OnTrayNotifyMsg::NIN_BALLOONUSERCLICK\n");
//		break;
//
//
//
//	}
//
//	return 0;
//}


//2013/05/06
//<<
//@功能:当任务栏重新生成时的处理函数
//@说明:这里仅仅在托盘中重新添加
inline BOOL CTrayImpl::OnTaskbarCreated()
{    
    //托盘已消失
    m_bInSysTray = FALSE;

	BOOL bRes = ::Shell_NotifyIcon(NIM_ADD,&m_tNotifyIconData);
	if(bRes)
	{
		m_bInSysTray = TRUE;

        LOG_INF("OnTaskbarCreated m_bInSysTray = %d \n", m_bInSysTray);
	}

    return bRes;

}
//>>