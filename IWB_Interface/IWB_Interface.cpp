// OpticalPen_Interface.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "../inc/IWB_API_Def.h"
#include "../inc/IWB_interface.h"


extern "C"
{

//@功能:向"光笔检测程序"注册接收消息
//@参数:hWnd, 接收"光笔检测程序"发送的通知消息
//@说明:如果，hWnd == NULL则"光笔检测程序"像调用OPI_PleaseNotifyMe函数的
//      线程发送通知消息。
static COpticalPenSharedMemory g_oShareMem;

IWB_API UINT OPI_PleaseNotifyMe(HWND hWnd)
{
	UINT uMsg = 0;

	g_oShareMem.InsertNotifySink(hWnd);

	uMsg = RegisterWindowMessage(_T("OpticalVirtualPen_3F3311C0-0ABF-495a-8390-2B6A4E34AAB7"));

	return uMsg;
}



//@功能:告知"光笔检测程序"不再接收通知消息
//@参数:
//@说明:
//      
IWB_API BOOL OPI_StopNotifyMe(HWND hWnd)
{
	g_oShareMem.RemoveNotifySink(hWnd);
	return TRUE;
}



}