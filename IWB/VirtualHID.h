#pragma once
#include "..\inc\GlobalDefine.h"
#include "..\inc\EASI_TOUCHSCREEN_INTERFACE.h"
#include "DeviceEventMonitor.h"
#include "TouchScreenDblChecker.h"
#include "VirtualTUIO.h"
#include "TouchTester.h"
#include "TouchInjector.h"

//参考:"Processing a Request to Remove a Device"实时检测设备驱动的状态。
//http://msdn.microsoft.com/en-us/library/windows/desktop/aa363215(v=vs.85).aspx

//虚拟HID设备
class CVirtualHID
{
public:

	CVirtualHID();

	~CVirtualHID();

	BOOL OpenDevice();

	BOOL CloseDevice();

	//@功能:状态复位
	//@参数:
	//@说明:触屏设备所有按下的点将弹起。
	//      鼠标设备的按键全部弹起。
	void Reset();

	//@功能:模拟HID输入
	//@参数:penInfos, 指向光笔数组的指针
	//      nPenCount, 光笔支数
	BOOL InputPoints(TContactInfo* pPenInfos, int nPenCount, DWORD dwCameraId = -1, int nMonitorId = -1);

	//@功能:设置鼠标/触屏模式
	void SetHIDMode(EHIDDeviceMode eMode) 
	{ 
		m_eHIDDeviceMode = eMode; 
		m_eDesiredHIDMode = eMode;
	}
	EHIDDeviceMode GetHIDMode()const { return m_eHIDDeviceMode; }

	//@功能：设置HID触控模式
	void SetTouchHIDMode(bool  eMode);
	bool GetTouchHIDMode()const {return m_bTouchHIDMode;}
	//@功能：设置TUIO触控模式
	void SetTouchTUIOMode(bool  eMode);
	bool GetTouchTUIOMode()const { return m_bTouchTUIOMode; }
	////@功能：设置触屏单点模式
	void SetSinglePointMode(bool eMode);
	bool GetSinglePointMode()const { return m_bSinglePointMode; }

	/////////////////
	void    SetTUIOParams(DWORD IP, int nPort, int nScreenWindth, int nScreenHeight);
	void    OpenTUIOServer(bool bStart);
	DWORD   GetIPadress();
	int     GetPort();
	int     GetScreenWidth();
	int     GetScreenHeight();
	void    SetTUIOScreenDisplayChange(int nScreenX, int nScreenY);

	static CVirtualMouse& GetVirtualMouse() { return m_oVirtualMouse; }

	//@功能:判断虚拟驱动是否是打开的。
	BOOL IsVirtualDriverOpen();

	//@功能:设置触屏的尺寸事件响应函数
	//@参数:nPhysicalDiagonalLength, 屏幕尺寸,对角线长度, 单位:毫米
	//      szLogicalDimension, 屏幕逻辑尺寸，单位:像素
	void OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension);

    //@功能:更新触屏关联的屏幕信息
    void UpdateAttachedMonitorInfo();
	//@功能
	void SetTest30Point(BOOL bStart);
	BOOL GetTest30Point();

	void SetAirOperateMode(BOOL eMode, EAIROPERATE_CLICKMODE  eClickMode);

protected:
	//@功能:模拟触屏输入
	//@参数:penInfos, 指向光笔数组的指针
	//      nPenCount, 光笔支数
	BOOL InputTouchPoints(const TContactInfo* pPenInfos, int nPenCount, int dwCameraId);

	BOOL InputTouchPoints_AirOperate(const TContactInfo* pPenInfos, int nPenCount, EAIROPERATE_CLICKMODE  eClickMode);

	//@功能:检索指针设备信息
	BOOL RetrievePointerDevices();

	//@功能:根据产品字符串提取对应的指针设备信息。
	//@参数:lpszProductString, 产品字符串
	//@返回值:NULL, 未找到。
	//        其他，指向POINTER_DEVICE_INFO的数据指针
	POINTER_DEVICE_INFO* GetPointerDevice(LPCTSTR lpszProductString = NULL);
    std::vector<POINTER_DEVICE_INFO> m_vecPointerDeviceInfos;

	HANDLE m_hDev;
	EHIDDeviceMode m_eHIDDeviceMode;

	//////////////add by zhaown 2019.09.26
	CVirtualTUIO  m_oVirtualTUIOTouch;
	TouchInjector m_oTouchInjector;//Windows API注入触控事件。

	bool     m_bTouchHIDMode  ;
	bool     m_bTouchTUIOMode ;

	bool     m_bSinglePointMode;  //是否开启单点响应
	BOOL     m_bStartTest30Point; //用来测试这个系统是否支持三十点触控
	CTouchTester  m_oTouchTester;

	BOOL     m_bAirOperationMode;
	EAIROPERATE_CLICKMODE  m_eClickMode;


	static const int MAX_TOUCH_POINT_COUNT = MAX_SUPPORT_TOUCH_COUNT;
	EASI_TouchPoint m_TouchPoints[MAX_TOUCH_POINT_COUNT];

	static CVirtualMouse m_oVirtualMouse;//虚拟鼠标对象


	//功能:设备句柄观联事件回调接口
	static void  DeviceHandleEventCallBack(const DeviceHandleMonitorEntry* pEntry, WPARAM dbtEvent);

	//@功能:HID设备接口关联事件回调函数
	static void  DeviceInterfaceEventCallBack(const DeviceIntetfaceMonitorEntry* pEntry, WPARAM dbtEvent);

	CDeviceEventMonitor m_oDeviceEventDetector;

	//<<added by toxuke@gmail.com, 205/01/30
	CTouchScreenDblClickChecker<MAX_TOUCH_POINT_COUNT> m_oDblClickChecker;//双击检测器
	//>>

	//{{
	//2018/01/04
	//为了解决在开机自动运行时, 虚拟触屏设备打开失败的问题
	//在打开设备设备失败时, 开启一个线程不断尝试打开触屏设备，直到成功或者达到最大尝试数目
	CComAutoCriticalSection m_csForVirtualDevice;
	BOOL OpenDeviceThreadSafe();

	BOOL CloseDeviceThreadSafe();


	static DWORD WINAPI AutoOpenThreadProc(LPVOID lpCtx);
	HANDLE m_hAutoOpenThread;
	BOOL   m_bAutoOpenThreadExit;

	EHIDDeviceMode m_eDesiredHIDMode;//想要的模式

	BOOL CreateAutoOpenThread();
	void CloseAutoOpenThread();

	//}}

    //触屏宽高比
    int m_aspectRatioNominator   = 16;
    int m_aspectRatioDenominator = 9;

    //触摸屏数据调整模型
    enum
    {   //按照指定的宽高比进行拉伸
        E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO,
        //显示器嵌入在触屏中
        E_TOUCH_DATA_AJUST_WITH_EMEBED_MODEL
    }m_eTouchDataAdjustModel;
};

inline CVirtualMouse& GetVirtualMouse()
{
    return  CVirtualHID::GetVirtualMouse();
}