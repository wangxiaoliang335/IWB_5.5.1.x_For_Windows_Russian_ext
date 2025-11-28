#pragma once
#include <initguid.h>
#include <DBT.h>

// 6BDD1FC6-810F-11D0-BEC7-08002BE2092F
DEFINE_GUID(GUID_CLASS_IMAGE, 
0x6BDD1FC6, 0x810F, 0x11D0, 0xBE, 0xC7, 0x08, 0x00, 0x2B, 0xE2, 0x09, 0x2F);


//{A5DCBF10-6530-11D2-901F-00C04FB951ED}
DEFINE_GUID(GUID_CLASS_USBDEV, 
0xA5DCBF10, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);



class CUSBDevDetector
{

public:
	CUSBDevDetector(const GUID& guidDevClass, HWND hNotifyWindow)
	{
		DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
		DWORD dwErr;

		ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
		NotificationFilter.dbcc_size = 
			sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		NotificationFilter.dbcc_classguid = guidDevClass;

		
		m_hDevNotify = RegisterDeviceNotification(
						hNotifyWindow, //hRecipient,handle to the window to receive the device events for the device.
						&NotificationFilter,
						DEVICE_NOTIFY_WINDOW_HANDLE /*| DEVICE_NOTIFY_ALL_INTERFACE_CLASSES*/ //The hRecipient parameter is a window handle
				        );
		if(!m_hDevNotify)
		{
			dwErr = GetLastError();
		}
	}

	~CUSBDevDetector()
	{
		if(m_hDevNotify)
		{
			UnregisterDeviceNotification(m_hDevNotify);
		}
	}

protected:
    HDEVNOTIFY m_hDevNotify;

};