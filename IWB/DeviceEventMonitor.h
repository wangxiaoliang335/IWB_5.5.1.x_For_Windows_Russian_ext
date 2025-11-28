#pragma once
#pragma pack(push,8)
#include  <dbt.h>
#pragma pack(pop)

#define DEVICE_EVENT_DETECTOR_WND_CLASS_NAME TEXT("DeviceEventDetectorWndClassName")

struct  DeviceHandleMonitorEntry;

typedef void (*DeviceHandleMonitorEventCallBack)(const DeviceHandleMonitorEntry* pEntry, WPARAM dbtEvent);
//设备句柄监控条目
struct  DeviceHandleMonitorEntry
{
    HDEVNOTIFY hDevNotify                        ;//device notification handle
    HANDLE     hDeviceHandle                     ;//设备句柄
    DeviceHandleMonitorEventCallBack callbackProc;//回调函数
    LPVOID     lpCtx                             ;//回调函数的上下文数据
};


struct DeviceIntetfaceMonitorEntry;
typedef void (*DeviceInterfaceMonitorEventCallBack)(const DeviceIntetfaceMonitorEntry* pEntry, WPARAM dbtEvent);
//设备接口监控条目
struct  DeviceIntetfaceMonitorEntry
{
    HDEVNOTIFY hDevNotify                           ;//device notification handle
    GUID       guidInterface                        ;//设备接口GUID
    DeviceInterfaceMonitorEventCallBack callbackProc;//回调函数
    LPVOID     lpCtx                                ;//回调函数的上下文数据
};

inline bool operator <(const GUID& left, const GUID& right)
{
    if(left.Data1 < right.Data1) 
    {
        return true;
    }
    else if(left.Data1 > right.Data1)
    {
        return false;
    }
    else//left.Data1 = right.Data1
    {
        if(left.Data2 < right.Data2)
        {
            return true;
        }
        else if(left.Data2 > right.Data2)
        {
            return false;
        }
        else 
        {
            if(left.Data3 < right.Data3)
            {
                return true;
            }
            else if(left.Data3 > right.Data3)
            {
                return false;
            }
            else
            {
                if(memcmp(left.Data4, right.Data4, sizeof(left.Data4) < 0))
                {
                    return true;
                }
            }
        }

    }

    return false;
}


//设备事件监视器
class CDeviceEventMonitor
{
public:
    CDeviceEventMonitor()
        :
    m_hWnd(NULL)
    {
        Init();
    }

    ~CDeviceEventMonitor()
    {
        if(m_hWnd)
        {
            DestroyWindow(m_hWnd);
            ::FreeThunk(&CDeviceEventMonitor::InternalWndProc, this);
        }

        
        std::map<HANDLE, DeviceHandleMonitorEntry>::iterator it_handle = m_oMapHandle.begin();
        for(; it_handle != m_oMapHandle.end(); it_handle++)
        {
            UnregisterDeviceNotification(it_handle->second.hDevNotify);
            it_handle->second.hDevNotify = NULL;
        }
        
        std::map<GUID, DeviceIntetfaceMonitorEntry>::iterator it_interface =  m_oMapInterface.begin();
        for(; it_interface != m_oMapInterface.end(); it_interface ++)
        {
            UnregisterDeviceNotification(it_interface->second.hDevNotify);
            it_interface->second.hDevNotify = NULL;
            
        }

    }


    //@功能:添加设备句柄到监测名单中去, 通过设备句柄筛选事件。
    BOOL  AddHandleMonitor(HANDLE hDeviceHandle, DeviceHandleMonitorEventCallBack callbackProc, LPVOID lpCtx)
    {
        if(NULL == m_hWnd) return FALSE;

        if(m_oMapHandle.find(hDeviceHandle) != m_oMapHandle.end()) return FALSE;

        DEV_BROADCAST_HANDLE dev_broadcast_handle;
        memset(&dev_broadcast_handle, 0, sizeof(DEV_BROADCAST_HANDLE));
        dev_broadcast_handle.dbch_size   = sizeof(DEV_BROADCAST_HANDLE);
        dev_broadcast_handle.dbch_handle = hDeviceHandle;
        dev_broadcast_handle.dbch_devicetype = DBT_DEVTYP_HANDLE;

        HDEVNOTIFY hDevNotify = 
            RegisterDeviceNotification(
                m_hWnd,
                (LPVOID)&dev_broadcast_handle,
                DEVICE_NOTIFY_WINDOW_HANDLE);
        if(hDevNotify == NULL)
        {
            return FALSE;
        }

        DeviceHandleMonitorEntry entry;
        entry.hDevNotify     = hDevNotify;
        entry.hDeviceHandle  = hDeviceHandle;
        entry.lpCtx          = lpCtx;
        entry.callbackProc   = callbackProc;
        
        m_oMapHandle[hDeviceHandle] = entry;

        return TRUE;
    }

    //@功能:将设备句柄从监控名单中移除。
    BOOL RemoveHandleMonitor(HANDLE hDeviceHandle)
    {
        std::map<HANDLE, DeviceHandleMonitorEntry>::iterator it;
        it = m_oMapHandle.find(hDeviceHandle);

        if(it != m_oMapHandle.end())
        {
            m_oMapHandle.erase(it);
            return TRUE;
        }
        return FALSE;
    }



    //@功能:添加设备接口GUID到监测名单中去，通过设备接口GUID来筛选事件。
    BOOL  AddDevIntefaceMonitor(const GUID& guidDevInterface, DeviceInterfaceMonitorEventCallBack callbackProc, LPVOID lpCtx)
    {
        if(NULL == m_hWnd) return FALSE;
        if(m_oMapInterface.find(guidDevInterface) != m_oMapInterface.end())
        {
            return FALSE;
        }

        DEV_BROADCAST_DEVICEINTERFACE dev_broadcast_interface;
        memset(&dev_broadcast_interface, 0, sizeof(DEV_BROADCAST_DEVICEINTERFACE));
        dev_broadcast_interface.dbcc_size      = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
        dev_broadcast_interface.dbcc_classguid = guidDevInterface;
        dev_broadcast_interface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

        HDEVNOTIFY hDevNotify = 
            RegisterDeviceNotification(
                m_hWnd,
                (LPVOID)&dev_broadcast_interface,
                DEVICE_NOTIFY_WINDOW_HANDLE);
        if(hDevNotify == NULL)
        {
            return FALSE;
        }

        DeviceIntetfaceMonitorEntry entry;
        entry.hDevNotify     = hDevNotify;
        entry.guidInterface  = guidDevInterface;
        entry.lpCtx          = lpCtx;
        entry.callbackProc   = callbackProc;
        
        m_oMapInterface[guidDevInterface] = entry;

        return TRUE;
    }

    //@功能:将设备接口从监控名单中移除。
    BOOL RemoveDevIntefaceMonitor(const GUID& guidDevInterface)
    {
        std::map<GUID, DeviceIntetfaceMonitorEntry>::iterator it;
        it = m_oMapInterface.find(guidDevInterface);

        if(it != m_oMapInterface.end())
        {
            m_oMapInterface.erase(it);
            return TRUE;
        }

        return FALSE;
    }


protected:
    //成员函数
    BOOL InitWindowClass()
    {
        WNDCLASSEX wndClass;

        wndClass.cbSize = sizeof(WNDCLASSEX);
        wndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wndClass.hInstance = reinterpret_cast<HINSTANCE>(GetModuleHandle(0));
        wndClass.lpfnWndProc = reinterpret_cast<WNDPROC>(WinProcCallback);
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;
        wndClass.hIcon = LoadIcon(0,IDI_APPLICATION);
        wndClass.hbrBackground = CreateSolidBrush(RGB(192,192,192));
        wndClass.hCursor = LoadCursor(0, IDC_ARROW);
        wndClass.lpszClassName = DEVICE_EVENT_DETECTOR_WND_CLASS_NAME;
        wndClass.lpszMenuName = NULL;
        wndClass.hIconSm = wndClass.hIcon;


        if ( ! RegisterClassEx(&wndClass) )
        {
            //ErrorHandler(TEXT("RegisterClassEx"));
            return FALSE;
        }
        return TRUE;
    }


    BOOL Init()
    {
        if(!InitWindowClass())
        {
            return FALSE;
        }

        m_hWnd = CreateWindowEx(
            WS_EX_CLIENTEDGE | WS_EX_APPWINDOW,
            DEVICE_EVENT_DETECTOR_WND_CLASS_NAME,
            _T("DeviceEventDetector"),
            WS_OVERLAPPEDWINDOW, // style
            CW_USEDEFAULT, 0, 
            640, 480,
            NULL, NULL, 
            reinterpret_cast<HINSTANCE>(GetModuleHandle(0)), 
            NULL);

        if(m_hWnd == NULL )
        {
            return FALSE;
        }

        
        SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)MF2GF<WNDPROC>(&CDeviceEventMonitor::InternalWndProc, this));

        return TRUE;
    }


   static HRESULT _stdcall WinProcCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    HRESULT InternalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        PDEV_BROADCAST_HDR    pDBHdr;
        PDEV_BROADCAST_HANDLE pDBHandle;
        PDEV_BROADCAST_DEVICEINTERFACE pDBInterface;
        std::map<HANDLE, DeviceHandleMonitorEntry>::iterator it_handle;
        std::map<GUID, DeviceIntetfaceMonitorEntry>::iterator it_interface;

        if(uMsg == WM_DEVICECHANGE)
        {
            pDBHdr = (PDEV_BROADCAST_HDR) lParam;
            
            if(pDBHdr)
            {
                switch(pDBHdr->dbch_devicetype)
                {
                    case DBT_DEVTYP_HANDLE:
                         pDBHandle = (PDEV_BROADCAST_HANDLE)pDBHdr;
                         it_handle = m_oMapHandle.find(pDBHandle->dbch_handle);
                         if(it_handle != m_oMapHandle.end())
                         {
                             if(it_handle->second.callbackProc)
                             {
                                it_handle->second.callbackProc(&it_handle->second, wParam);
                             }
                         }

                    break;

                    case DBT_DEVTYP_DEVICEINTERFACE:
                        pDBInterface = (PDEV_BROADCAST_DEVICEINTERFACE)pDBHdr;
                        it_interface = m_oMapInterface.find(pDBInterface->dbcc_classguid);
                        if(it_interface != m_oMapInterface.end())
                        {
                            if(it_interface->second.callbackProc)
                            {
                                it_interface->second.callbackProc(&it_interface->second, wParam);
                            }
                        }


                    break;
                }//switch
            }

            return TRUE;
        }


        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }


    //成员变量
    HWND m_hWnd;

    std::map<HANDLE, DeviceHandleMonitorEntry>  m_oMapHandle;
    std::map<GUID, DeviceIntetfaceMonitorEntry> m_oMapInterface;

};