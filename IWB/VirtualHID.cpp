#include "stdafx.h"
//#include "headers.h"
#include <VersionHelpers.h>
#pragma comment(lib, "..\\lib\\EASI_TOUCHSCREEN_INTERFACE.lib")

#include "../inc/RtlGetVersion.h"

#include "Hidsdi.h"
#pragma comment(lib,"Hid.lib")

//extern "C"
//{
//    #include <Hidsdi.h>
//}
//#pragma comment(lib, "hid.lib")
//#pragma comment(lib, "setupapi.lib")

//#include <initguid.h>
//DEFINE_GUID(GUID_DEVINTERFACE_EASI_UMDF_TouchScreen,
//    0xecf30a3b,0xd7d1,0x4580,0x9d,0x35,0xd1,0x70,0x0b,0xbf,0xe5,0xbb);
BOOL GetPointerDeviceDevPath(
    LPCTSTR lpszPointerDeviceProductString,
    LPTSTR lpszDevPathBuf,
    UINT uBufSize)
{
    GUID hidguid;
    HDEVINFO hardwareDeviceInfo;
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    SP_DEVINFO_DATA devInfoData;

    int i;

    BOOLEAN bFound = FALSE;
    BOOLEAN bSuccess = FALSE;

    HidD_GetHidGuid(&hidguid);

    hardwareDeviceInfo = SetupDiGetClassDevs(
        (LPGUID)&hidguid,
        NULL,
        NULL, // Define no
        (DIGCF_PRESENT |
            DIGCF_INTERFACEDEVICE));

    if (INVALID_HANDLE_VALUE == hardwareDeviceInfo) {
        return  FALSE;
    }


    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);



    PSP_DEVICE_INTERFACE_DETAIL_DATA    lpDeviceInterfaceDetailData = NULL;


    // Enumerate devices of this interface class
    for (i = 0;
        SetupDiEnumDeviceInterfaces(hardwareDeviceInfo,
            0, // No care about specific PDOs
            (LPGUID)&hidguid,
            i, //
            &deviceInterfaceData);
        i++)
    {
        DWORD predictedLength = 0;
        DWORD requiredLength = 0;
        BOOL deviceFound = FALSE;


        SetupDiGetDeviceInterfaceDetail(
            hardwareDeviceInfo,
            &deviceInterfaceData,
            NULL, // probing so no output buffer yet
            0, // probing so output buffer length of zero
            &requiredLength,
            NULL
        ); // not interested in the specific dev-node

        predictedLength = requiredLength;

        lpDeviceInterfaceDetailData =
            (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(predictedLength);

        if (!lpDeviceInterfaceDetailData)
        {
            break;
        }

        lpDeviceInterfaceDetailData->cbSize =
            sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(
            hardwareDeviceInfo,
            &deviceInterfaceData,
            lpDeviceInterfaceDetailData,
            predictedLength,
            &requiredLength,
            NULL))
        {
            break;
        }

        HANDLE file = INVALID_HANDLE_VALUE;
        file = CreateFile(lpDeviceInterfaceDetailData->DevicePath,
            0,// GENERIC_READ, 注意!!!不要指定任何读写权限，否者会失败
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, // no SECURITY_ATTRIBUTES structure
            OPEN_EXISTING, // No special create flags
            0, // No special attributes
            NULL); // No template file
        DWORD dwError = ERROR_SUCCESS;
        if (INVALID_HANDLE_VALUE == file) {
            //Try Next Device
            dwError = GetLastError();
            goto _check_next_device_;
        }

        TCHAR szProductString[1024];
        bSuccess = HidD_GetProductString(
            file,
            szProductString,
            _countof(szProductString));

        if (!bSuccess)
        {

            goto _check_next_device_;
        }
        else
        {
            if (_tcscmp(lpszPointerDeviceProductString, szProductString) == 0)
            {
                //\\?\hid#hidclass&col01#1&4784345&38&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}
                //驱动的设备描述符中规定了
                //&col01: collection 1, touchscreen;
                //&col02: collection 2, Standard HID device
                //&col03: collection 3, Mouse
                int nDevicePathLength = _tcslen(lpDeviceInterfaceDetailData->DevicePath);

                //字符串全部改为大写
                _tcsupr_s(
                    lpDeviceInterfaceDetailData->DevicePath,
                    nDevicePathLength + 1
                );

                if (_tcsstr(lpDeviceInterfaceDetailData->DevicePath, _T("\\\\?\\HID#HIDCLASS&COL01")))
                {//确保是TouchScreen设备路径
                    _tcscpy_s(lpszDevPathBuf, uBufSize, lpDeviceInterfaceDetailData->DevicePath);

                    bFound = TRUE;
                }

            }

        }

    _check_next_device_:
        if (INVALID_HANDLE_VALUE != file)
        {
            CloseHandle(file);
        }
        if (lpDeviceInterfaceDetailData)
        {
            free(lpDeviceInterfaceDetailData);
            lpDeviceInterfaceDetailData = NULL;
        }

        if (bFound)
        {
            break;
        }

    }//for

    SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);

    return bSuccess;
}


//@功能:判断指定的触控设备是否关联指定的屏幕设置
//@参数:lpszPointerDevicePath, 触屏设备路径，全部大写
//      lpszMonitorDevicePath, 显示器设备路径，全部大写
BOOL IsMonitorAttachedToPointerDevice(
    LPCTSTR lpszPointerDevicePath,
    LPCTSTR lpszMonitorDevicePath
)
{
    HKEY hKey;
    BOOL bAttached = FALSE;
    LSTATUS retCode = RegOpenKey(
        HKEY_LOCAL_MACHINE,
        _T("SOFTWARE\\Microsoft\\Wisp\\Pen\\Digimon"),
        &hKey);

    if (retCode != ERROR_SUCCESS)
    {
        return FALSE;
    }

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

    //  TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    //  DWORD    cbName;                   // size of name string 
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys = 0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 

    DWORD i;

    TCHAR  achValue[MAX_VALUE_NAME];
    DWORD cchValue = MAX_VALUE_NAME;


    TCHAR achData[MAX_VALUE_NAME];
    DWORD cchData = MAX_VALUE_NAME;


    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 

    for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++)
    {
        cchValue = MAX_VALUE_NAME;
        achValue[0] = '\0';
        retCode = RegEnumValue(
            hKey,
            i,
            achValue,
            &cchValue,
            NULL,
            NULL,
            (LPBYTE)achData,
            &cchData);


        if (retCode == ERROR_SUCCESS && _tcsicmp(lpszMonitorDevicePath, achData) == 0)
        {//发现了显示器设备路径了。

            _tcsupr_s(
                achValue,
                cchValue + 1
            );

            if (_tcsstr(achValue, lpszPointerDevicePath))
            {//触控设备路径也对上了
                bAttached = TRUE;
                break;
            }
        }
    }
    RegCloseKey(hKey);

    return bAttached;
}

CVirtualMouse CVirtualHID::m_oVirtualMouse;


CVirtualHID::CVirtualHID()
    :
    m_hDev(INVALID_HANDLE_VALUE),
    //m_eHIDDeviceMode(E_DEV_MODE_TOUCHSCREEN)
    m_eHIDDeviceMode(E_DEV_MODE_MOUSE),
    m_hAutoOpenThread(NULL),
    m_bAutoOpenThreadExit(FALSE),
    m_aspectRatioNominator(16),
    m_aspectRatioDenominator(9),
    m_eTouchDataAdjustModel(E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO),
    m_bTouchHIDMode(true),
    m_bTouchTUIOMode(false),
    m_bSinglePointMode(false),
	m_bStartTest30Point(FALSE),
	m_bAirOperationMode(FALSE),
	m_eClickMode(E_MODE_CLICK)

{
    memset(&m_TouchPoints[0], 0, sizeof(m_TouchPoints));
    RetrievePointerDevices();    
}


CVirtualHID::~CVirtualHID()
{
    if (m_hDev != INVALID_HANDLE_VALUE)
    {
        EASI_CloseDevice(m_hDev);
    }
}


BOOL CVirtualHID::OpenDevice()
{
    UpdateAttachedMonitorInfo();

    //m_hDev = EASI_OpenDevice();
    CloseAutoOpenThread();

    //尝试打开设备
    OpenDeviceThreadSafe();

    //if(m_hDev != INVALID_HANDLE_VALUE)
    //{
    //    //将设备句柄加入监控名单。
    //    m_oDeviceEventDetector.AddHandleMonitor(m_hDev, DeviceHandleEventCallBack, (LPVOID)this);
    //}
    //else
    if (INVALID_HANDLE_VALUE == m_hDev)
    {   //暂时打不开设备, 开始监控HID设备事件，以便HID设备就绪后能够打开设备。
        AtlTrace(_T("EASI_OpenDevice rerurn INVALID_HANDLE_VALUE\n"));
        LOG_ERR("EASI_OpenDevice rerurn INVALID_HANDLE_VALUE\n");
        //GUID guid_HID;
        //HidD_GetHidGuid(&guid_HID);
        //m_oDeviceEventDetector.AddDevIntefaceMonitor(guid_HID, DeviceInterfaceEventCallBack,(LPVOID)this);
        const GUID& interface_guid = EASI_GetDeviceInterfaceGUID();
        m_oDeviceEventDetector.AddDevIntefaceMonitor(interface_guid, DeviceInterfaceEventCallBack, (LPVOID)this);

        //想要的模式
        m_eDesiredHIDMode = m_eHIDDeviceMode;

        //在开机自启动时,如果打开虚拟设备失败，则开启线程尝试自动打开
        CreateAutoOpenThread();

        //驱动设备打不开，则强制使用鼠标模式
        m_eHIDDeviceMode = E_DEV_MODE_MOUSE;
    }
    return INVALID_HANDLE_VALUE == m_hDev;
}

BOOL CVirtualHID::CloseDevice()
{
    CloseAutoOpenThread();
    return CloseDeviceThreadSafe();
}

//@功能:状态复位
//@参数:
//@说明:触屏设备所有按下的点将弹起。
//      鼠标设备的按键全部弹起。
void CVirtualHID::Reset()
{
    switch (m_eHIDDeviceMode)
    {
    case E_DEV_MODE_MOUSE://复位鼠标
        this->m_oVirtualMouse.Reset();
        break;

    case E_DEV_MODE_TOUCHSCREEN://复位触摸屏
        if (INVALID_HANDLE_VALUE == m_hDev) return;

        //复位每个触屏点
        for (int i = 0; i < _countof(m_TouchPoints); i++)
        {
            if (m_TouchPoints[i].bStatus == TIP_DOWN)
            {
                m_TouchPoints[i].bStatus = TIP_UP;
                EASI_WriteVirtualTouchScreen(m_hDev, &m_TouchPoints[i], 1);
            }
        }//for(i)

        break;
    }//switch
}

//@功能:模拟HID输入
//@参数:pPenInfos, 指向光笔数组的指针
//      nPenCount, 光笔支数
BOOL CVirtualHID::InputPoints(TContactInfo* pPenInfos, int nPenCount, DWORD dwCameraId, int nMonitorId)
{
    TSensorModeConfig* pSensorModeConfig = NULL;

    //g_tSysCfgData.vecSensorConfig[nMonitorId].nXCoordOffset;
    //pSensorModeConfig = &m_tSensorConfig.vecSensorModeConfig[m_tGlobalSettings.eProjectionMode];

    BOOL bRet = FALSE;
    //设备未打开，并且自动打开线程未开启
    if (m_hDev == INVALID_HANDLE_VALUE && this->m_hAutoOpenThread == NULL)
    {
        if (!OpenDevice())
        {
            return FALSE;
        }
    }

    //char szBuf[1024] = { 0 };
    //sprintf(szBuf, "aaaa InputPoints pLightSpots nMonitorId:%d\n", nMonitorId);
    //OutputDebugStringA(szBuf);

    //if (nMonitorId >= 0)   //wxl modify 2024_12_3
    //{
    //    for (int ii = 0; ii < nPenCount; ii++)
    //    {
    //        pPenInfos[ii].pt.x += g_tSysCfgData.vecSensorConfig[nMonitorId].nXCoordOffset /** 5*/;
    //        pPenInfos[ii].pt.y += g_tSysCfgData.vecSensorConfig[nMonitorId].nYCoordOffset /** 5*/;
    //    }
    //}

	if (m_bStartTest30Point)
	{
		if (nPenCount>0)
		{
		    m_oTouchTester.Process(pPenInfos);
		    const TContactInfo *pAllContactInfo = m_oTouchTester.GetContactInfo();

		    int nCount = m_oTouchTester.GetContactCount();

		    bRet = InputTouchPoints(pAllContactInfo, nCount, dwCameraId);
		}

		/////说明已经弹起了，做Reset处理
		if (pPenInfos[0].ePenState == E_PEN_STATE_UP)
		{
			m_oTouchTester.Reset();
		}
	}
	else
	{
       if (m_bTouchHIDMode)
       {
          switch (m_eHIDDeviceMode)
          {
            case E_DEV_MODE_MOUSE:
                 //搜索编号为0的笔信息
				if(m_bAirOperationMode)
				{
					for (int i = 0; i < nPenCount; i++)
					{
						if (pPenInfos[i].uId == 0)
						{
							m_oVirtualMouse.Input_AirOperate(pPenInfos[i].ePenState == E_PEN_STATE_DOWN, &pPenInfos[i].pt, m_eClickMode);
							break;
						}
					}
				}
				else
				{
                   for (int i = 0; i < nPenCount; i++)
                   {
                      if (pPenInfos[i].uId == 0 )
                      {
                          m_oVirtualMouse.Input(pPenInfos[i].ePenState == E_PEN_STATE_DOWN, &pPenInfos[i].pt, TRUE);
                          break;
                      }
                   }
				}

                break;
            case E_DEV_MODE_TOUCHSCREEN:
                 ////如果选择的是单点触控的话，只响应一个点就可以了
                 if (m_bSinglePointMode)
                 {
                     //搜索编号为0的笔信息
                     for (int i = 0; i < nPenCount; i++)
                     {
                         if (pPenInfos[i].uId == 0)
                         {
                             InputTouchPoints(&pPenInfos[i], 1, dwCameraId);
                             break;
                         }
					 }
                  }
                  else
                  {
                      bRet = InputTouchPoints(pPenInfos, nPenCount, dwCameraId);
                  }
                  break;
            } //switch  
        }

        if (m_bTouchTUIOMode)
        {
            //模拟虚拟的TUIO
             m_oVirtualTUIOTouch.InputTUIOPoints(pPenInfos, nPenCount, dwCameraId);
        }
	}
    return bRet;
}


//@功能:模拟触屏输入
//@参数:pPenInfos, 指向光笔数组的指针
//      nPenCount, 光笔支数
BOOL CVirtualHID::InputTouchPoints(const TContactInfo* pPenInfos, int nPenCount, int dwCameraId)
{
    //static int temp_debug = 0;
    //if (temp_debug < 10)
    //{
    //    LOG_INF("InputTouchPoints\n");
    //}

    if (nPenCount > _countof(m_TouchPoints))
    {
        nPenCount = _countof(m_TouchPoints);
    }

    if (nPenCount == 0) return FALSE;

    if (m_hDev == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    TContactInfo aryContactInfos[MAX_TOUCH_POINT_COUNT];

    if (nPenCount > _countof(aryContactInfos))
    {
        nPenCount = _countof(aryContactInfos);
    }
    memcpy(&aryContactInfos[0], pPenInfos, nPenCount * sizeof(TContactInfo));

    //位于扩展屏中的触控点
    TContactInfo aryExtendScreenContactInfos[MAX_TOUCH_POINT_COUNT];
    size_t PtCountInExtendScreen = 0;

    //双击检测
    //<<2018/08/03
    //平滑滤波足够，无需双击检测.
    //双击检测的后果是，在画笔中近距离点两笔，两条笔画会连起来。
    this->m_oDblClickChecker.DoChecker(aryContactInfos, nPenCount);
    //>>

    int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
    int nCyScreen = GetSystemMetrics(SM_CYSCREEN);
    int nScreenLeft = 0;
    int nScreenTop = 0;

    int nPrimaryContactCount = 0;
    for (int i = 0; i < nPenCount; i++)
    {
        const POINT& ptContact = aryContactInfos[i].pt;

        const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(ptContact.x, ptContact.y);
        //POINTER_DEVICE_INFO* pPointerDeviceInfo = GetPointerDevice();
        if (NULL == pDisplayDevInfo) return FALSE;

        if (!pDisplayDevInfo->bIsPrimary)
        {//扩展屏
            aryExtendScreenContactInfos[PtCountInExtendScreen] = aryContactInfos[i];
            PtCountInExtendScreen++;
            continue;
        }

        EASI_TouchPoint& touchPoint = m_TouchPoints[nPrimaryContactCount];
        nPrimaryContactCount++;

        touchPoint.ContactId = aryContactInfos[i].uId;
        touchPoint.bStatus   = aryContactInfos[i].ePenState == E_PEN_STATE_DOWN ? TIP_DOWN : TIP_UP;


 


        //if (NULL == pPointerDeviceInfo) return FALSE;

        LONG nMonitorPixelLeft = pDisplayDevInfo->rcMonitor.left;
        LONG nMonitorPixelTop = pDisplayDevInfo->rcMonitor.top;
        LONG nMonitorPixelWidth = pDisplayDevInfo->rcMonitor.right - pDisplayDevInfo->rcMonitor.left;
        LONG nMonitorPixelHeight = pDisplayDevInfo->rcMonitor.bottom - pDisplayDevInfo->rcMonitor.top;


        //确定触屏的宽高比
        int aspectRatioNominator = 16;
        int aspectRatioDenominator = 9;
        ETouchScreenAspectRatio eRatio;

        eRatio = g_tSysCfgData.globalSettings.eTouchScreenAspectRatio;

        switch (eRatio)
        {
        case E_TOUCH_SCREEN_ASPECT_RATIO_AUTO:
            aspectRatioNominator = m_aspectRatioNominator;
            aspectRatioDenominator = m_aspectRatioDenominator;

            break;

        case E_TOUCH_SCREEN_ASPECT_RATIO_16_9:
            aspectRatioNominator = 16;
            aspectRatioDenominator = 9;

            break;

        case E_TOUCH_SCREEN_ASPECT_RATIO_16_10:
            aspectRatioNominator = 16;
            aspectRatioDenominator = 10;

            break;

        case E_TOUCH_SCREEN_ASPECT_RATIO_4_3:
            aspectRatioNominator = 4;
            aspectRatioDenominator = 3;
            break;
        }//switch



        POINT contactPos = aryContactInfos[i].pt;

        long x = contactPos.x;
        long y = contactPos.y;
        long temp = 0;


        //如果屏幕发生旋转则将触控的屏幕坐标转换为未旋转时的屏幕坐标
        //switch (pPointerDeviceInfo->displayOrientation)
        switch (pDisplayDevInfo->targetInfo.rotation)
        {
        case DISPLAYCONFIG_ROTATION_IDENTITY:
            //Keep No Change
            break;

        case DISPLAYCONFIG_ROTATION_ROTATE90:

            temp = nMonitorPixelWidth;
            nMonitorPixelWidth = nMonitorPixelHeight;
            nMonitorPixelHeight = temp;

            contactPos.x = y;
            contactPos.y = nMonitorPixelHeight - x;

            //<<2020/07/02
            temp = nCxScreen;
            nCxScreen = nCyScreen;
            nCyScreen = temp;
            //2020/07/02>>

            break;

        case DISPLAYCONFIG_ROTATION_ROTATE180:
            contactPos.x = nMonitorPixelWidth - x;
            contactPos.y = nMonitorPixelHeight - y;
            break;


        case DISPLAYCONFIG_ROTATION_ROTATE270:
            temp = nMonitorPixelWidth;
            nMonitorPixelWidth = nMonitorPixelHeight;
            nMonitorPixelHeight = temp;

            contactPos.x = nMonitorPixelWidth - y;
            contactPos.y = x;

            //<<2020/07/02
            temp = nCxScreen;
            nCxScreen = nCyScreen;
            nCyScreen = temp;
            //>>

            break;

        }//switch


        BOOL bDone = FALSE;


        if (DISPLAYCONFIG_SCALING_ASPECTRATIOCENTEREDMAX == pDisplayDevInfo->targetInfo.scaling
            ||
            DISPLAYCONFIG_SCALING_IDENTITY == pDisplayDevInfo->targetInfo.scaling
            ||
            DISPLAYCONFIG_SCALING_CENTERED == pDisplayDevInfo->targetInfo.scaling)
        {
            WORD wXData = 0, wYData = 0;
            OSVERSIONINFOEX osvinfex;
            if (IsWin10OrGreater())
            {
                /*Version OS build
                1909    18363.535
                1903    18362.535
                1809    17763.914
                1803    17134.1184
                1709    16299.1565
                */

                m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;

                if (RtlGetVersionWrapper(&osvinfex))
                {
                    if (osvinfex.dwBuildNumber <= 16299)
                    {//版本1709(OS内部版本 16299.125)
                        //m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
                    }
                    else if (osvinfex.dwBuildNumber == 17134)
                    {//版本1803(OS内部版本 17134.1184)
                        if (pDisplayDevInfo->displayAdapterInfos.size() >= 2)
                        {//屏幕复制模式
                            //m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
                        }
                        else
                        {//屏幕嵌入在触屏内部的模型
                            m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_EMEBED_MODEL;
                        }
                    }
                    //else if (osvinfex.dwBuildNumber == 17763)
                    //{   //版本1809(OS内部版本 17763.253)
                        //m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
                    //}

                }
            }
            else
            {
                m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
            }


            switch (m_eTouchDataAdjustModel)
            {
            case E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO:
            {//触屏按指定宽高比拉伸模型
             //Windows内部按照屏幕的物理宽高比对触控位置做了修正
             //屏幕虚拟像素宽度和像素高度就是按照指定的宽高比例计算出的虚拟宽度和高度，
                int nMonitorVirtualPixelWidth = nMonitorPixelWidth;
                int nMonitorVirtualPixelHeight = nMonitorPixelHeight;

                if (aspectRatioNominator * nMonitorPixelHeight > aspectRatioDenominator * nMonitorPixelWidth)
                { //物理宽高比大于实际像素的宽高比,垂直像素数目保持不变, 水平虚拟像素增加
                    nMonitorVirtualPixelWidth = nMonitorVirtualPixelHeight * aspectRatioNominator / aspectRatioDenominator;
                }
                else if (aspectRatioNominator * nMonitorPixelHeight < aspectRatioDenominator * nMonitorPixelWidth)
                {//物理宽高比小于实际像素的宽高比, 水平像素数目保持不变，垂直虚拟像素增加
                    nMonitorVirtualPixelHeight = nMonitorVirtualPixelWidth * aspectRatioDenominator / aspectRatioNominator;
                }

                wXData = USHORT((contactPos.x - nMonitorPixelLeft + ((nMonitorVirtualPixelWidth - nMonitorPixelWidth) >> 1)) * EASI_TOUCH_MAXIMUM_X / nMonitorVirtualPixelWidth);
                wYData = USHORT((contactPos.y - nMonitorPixelTop + ((nMonitorVirtualPixelHeight - nMonitorPixelHeight) >> 1)) * EASI_TOUCH_MAXIMUM_Y / nMonitorVirtualPixelHeight);

                LOG_INF("aspectRatioNominator=%d,aspectRatioDenominator=%d, nMonitorVirtualPixelWidth=%d,nMonitorVirtualPixelHeight=%d\r\n",
                    aspectRatioNominator,
                    aspectRatioDenominator,
                    nMonitorVirtualPixelWidth,
                    nMonitorVirtualPixelHeight);



            }
            break;

            case E_TOUCH_DATA_AJUST_WITH_EMEBED_MODEL:
            {//显示器内容嵌入在触屏中央模型
                const int& nSourceWidth = pDisplayDevInfo->sourceMode.width;
                const int& nSourceHeight = pDisplayDevInfo->sourceMode.height;
                const int& nTargetWidth = pDisplayDevInfo->targetMode.targetVideoSignalInfo.activeSize.cx;
                const int& nTargetHeight = pDisplayDevInfo->targetMode.targetVideoSignalInfo.activeSize.cy;

                wXData = USHORT((contactPos.x - nMonitorPixelLeft + ((nTargetWidth - nSourceWidth) >> 1)) * EASI_TOUCH_MAXIMUM_X / nTargetWidth);
                wYData = USHORT((contactPos.y - nMonitorPixelTop + ((nTargetHeight - nSourceHeight) >> 1)) * EASI_TOUCH_MAXIMUM_Y / nTargetHeight);

                //if (temp_debug < 10)
                //{
                //    LOG_INF("nSourceWidth=%d,nSourceHeight=%d, nTargetWidth=%d,nTargetHeight=%d\r\n",
                //        nSourceWidth, nSourceHeight, nTargetWidth, nTargetHeight);

                //}

            }
            break;
            }//switch(m_eTouchDataAdjustModel)

            touchPoint.wXData = wXData /*+ g_tSysCfgData.globalSettings.nXCoordOffset * 5*/;
            touchPoint.wYData = wYData /*+ g_tSysCfgData.globalSettings.nYCoordOffset * 5*/;

            //touchPoint.wXData = wXData + 150;
            //char szBuf[1024] = { 0 };
            //sprintf(szBuf, "333 touchPoint.wXData:%d, touchPoint.wYData:%d, touchPoint.ContactId:%d, nXCoordOffset:%d, nYCoordOffset:%d\n", touchPoint.wXData, touchPoint.wYData, touchPoint.ContactId, g_tSysCfgData.globalSettings.nXCoordOffset, g_tSysCfgData.globalSettings.nYCoordOffset);
            //OutputDebugStringA(szBuf);
            bDone = TRUE;
        }

        if (!bDone)
        {
            touchPoint.wXData = USHORT((contactPos.x - nMonitorPixelLeft) * EASI_TOUCH_MAXIMUM_X / nCxScreen) /*+ g_tSysCfgData.globalSettings.nXCoordOffset * 5*/;
            touchPoint.wYData = USHORT((contactPos.y - nMonitorPixelTop) * EASI_TOUCH_MAXIMUM_Y / nCyScreen) /*+ g_tSysCfgData.globalSettings.nYCoordOffset * 5*/;
            //char szBuf[1024] = { 0 };
            //sprintf(szBuf, "111 touchPoint.wXData:%d, touchPoint.wYData:%d, touchPoint.ContactId:%d, nXCoordOffset:%d, nYCoordOffset:%d\n", touchPoint.wXData, touchPoint.wYData, touchPoint.ContactId, g_tSysCfgData.globalSettings.nXCoordOffset, g_tSysCfgData.globalSettings.nYCoordOffset);
            //OutputDebugStringA(szBuf);
        }


        
    }//for

    BOOL bRet = EASI_WriteVirtualTouchScreen(m_hDev, &m_TouchPoints[0], nPrimaryContactCount);

    if (PtCountInExtendScreen > 0)
    {//扩展屏注入模拟
        m_oTouchInjector.Input(aryExtendScreenContactInfos, PtCountInExtendScreen);
    }

    return bRet;
}

BOOL CVirtualHID::InputTouchPoints_AirOperate(const TContactInfo* pPenInfos, int nPenCount, EAIROPERATE_CLICKMODE  eClickMode)
{

	if (nPenCount > _countof(m_TouchPoints))
	{
		nPenCount = _countof(m_TouchPoints);
	}

	if (nPenCount == 0) return FALSE;

	if (m_hDev == INVALID_HANDLE_VALUE)
	{
		return FALSE;

	}

	TContactInfo aryContactInfos[MAX_TOUCH_POINT_COUNT];

	if (nPenCount > _countof(aryContactInfos))
	{
		nPenCount = _countof(aryContactInfos);
	}
	memcpy(&aryContactInfos[0], pPenInfos, nPenCount * sizeof(TContactInfo));
	
	int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
	int nCyScreen = GetSystemMetrics(SM_CYSCREEN);
	int nScreenLeft = 0;
	int nScreenTop = 0;

	for (int i = 0; i < nPenCount; i++)
	{
		m_TouchPoints[i].ContactId = aryContactInfos[i].uId;
		m_TouchPoints[i].bStatus = aryContactInfos[i].ePenState == E_PEN_STATE_DOWN ? TIP_UP : TIP_DOWN;

		const POINT& ptContact = aryContactInfos[i].pt;

		const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(ptContact.x, ptContact.y);
		//POINTER_DEVICE_INFO* pPointerDeviceInfo = GetPointerDevice();
		if (NULL == pDisplayDevInfo) return FALSE;
		//if (NULL == pPointerDeviceInfo) return FALSE;

		LONG nMonitorPixelLeft = pDisplayDevInfo->rcMonitor.left;
		LONG nMonitorPixelTop = pDisplayDevInfo->rcMonitor.top;
		LONG nMonitorPixelWidth = pDisplayDevInfo->rcMonitor.right - pDisplayDevInfo->rcMonitor.left;
		LONG nMonitorPixelHeight = pDisplayDevInfo->rcMonitor.bottom - pDisplayDevInfo->rcMonitor.top;


		//确定触屏的宽高比
		int aspectRatioNominator = 16;
		int aspectRatioDenominator = 9;
		ETouchScreenAspectRatio eRatio;

		eRatio = g_tSysCfgData.globalSettings.eTouchScreenAspectRatio;

		switch (eRatio)
		{
		case E_TOUCH_SCREEN_ASPECT_RATIO_AUTO:
			aspectRatioNominator = m_aspectRatioNominator;
			aspectRatioDenominator = m_aspectRatioDenominator;
			break;

		case E_TOUCH_SCREEN_ASPECT_RATIO_16_9:
			aspectRatioNominator = 16;
			aspectRatioDenominator = 9;

			break;

		case E_TOUCH_SCREEN_ASPECT_RATIO_16_10:
			aspectRatioNominator = 16;
			aspectRatioDenominator = 10;

			break;

		case E_TOUCH_SCREEN_ASPECT_RATIO_4_3:
			aspectRatioNominator = 4;
			aspectRatioDenominator = 3;
			break;
		}//switch



		POINT contactPos = aryContactInfos[i].pt;

		long x = contactPos.x;
		long y = contactPos.y;
		long temp = 0;


		//如果屏幕发生旋转则将触控的屏幕坐标转换为未旋转时的屏幕坐标
		//switch (pPointerDeviceInfo->displayOrientation)
		switch (pDisplayDevInfo->targetInfo.rotation)
		{
		case DISPLAYCONFIG_ROTATION_IDENTITY:
			//Keep No Change
			break;

		case DISPLAYCONFIG_ROTATION_ROTATE90:

			temp = nMonitorPixelWidth;
			nMonitorPixelWidth = nMonitorPixelHeight;
			nMonitorPixelHeight = temp;

			contactPos.x = y;
			contactPos.y = nMonitorPixelHeight - x;

            //<<2020/07/02
            temp = nCxScreen;
            nCxScreen = nCyScreen;
            nCyScreen = temp;
            //2020/07/02>>

			break;

		case DISPLAYCONFIG_ROTATION_ROTATE180:
			contactPos.x = nMonitorPixelWidth - x;
			contactPos.y = nMonitorPixelHeight - y;
			break;

		case DISPLAYCONFIG_ROTATION_ROTATE270:
			temp = nMonitorPixelWidth;
			nMonitorPixelWidth = nMonitorPixelHeight;
			nMonitorPixelHeight = temp;

			contactPos.x = nMonitorPixelWidth - y;
			contactPos.y = x;


            //<<2020/07/02
            temp = nCxScreen;
            nCxScreen = nCyScreen;
            nCyScreen = temp;
            //2020/07/02>>

			break;

		}//switch


		BOOL bDone = FALSE;


		if (DISPLAYCONFIG_SCALING_ASPECTRATIOCENTEREDMAX == pDisplayDevInfo->targetInfo.scaling
			||
			DISPLAYCONFIG_SCALING_IDENTITY == pDisplayDevInfo->targetInfo.scaling
			||
			DISPLAYCONFIG_SCALING_CENTERED == pDisplayDevInfo->targetInfo.scaling)
		{
			WORD wXData = 0, wYData = 0;
			OSVERSIONINFOEX osvinfex;
			if (IsWin10OrGreater())
			{
				/*Version OS build
				1909    18363.535
				1903    18362.535
				1809    17763.914
				1803    17134.1184
				1709    16299.1565
				*/

				m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;

				if (RtlGetVersionWrapper(&osvinfex))
				{
					if (osvinfex.dwBuildNumber <= 16299)
					{//版本1709(OS内部版本 16299.125)
					 //m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
					}
					else if (osvinfex.dwBuildNumber == 17134)
					{//版本1803(OS内部版本 17134.1184)
						if (pDisplayDevInfo->displayAdapterInfos.size() >= 2)
						{//屏幕复制模式
						 //m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
						}
						else
						{//屏幕嵌入在触屏内部的模型
							m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_EMEBED_MODEL;
						}
					}
					//else if (osvinfex.dwBuildNumber == 17763)
					//{   //版本1809(OS内部版本 17763.253)
					//m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
					//}

				}
			}
			else
			{
				m_eTouchDataAdjustModel = E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO;
			}

			switch (m_eTouchDataAdjustModel)
			{
			case E_TOUCH_DATA_AJUST_WITH_ASPECT_RATIO:
			{//触屏按指定宽高比拉伸模型
			 //Windows内部按照屏幕的物理宽高比对触控位置做了修正
			 //屏幕虚拟像素宽度和像素高度就是按照指定的宽高比例计算出的虚拟宽度和高度，
				int nMonitorVirtualPixelWidth = nMonitorPixelWidth;
				int nMonitorVirtualPixelHeight = nMonitorPixelHeight;

				if (aspectRatioNominator * nMonitorPixelHeight > aspectRatioDenominator * nMonitorPixelWidth)
				{ //物理宽高比大于实际像素的宽高比,垂直像素数目保持不变, 水平虚拟像素增加
					nMonitorVirtualPixelWidth = nMonitorVirtualPixelHeight * aspectRatioNominator / aspectRatioDenominator;
				}
				else if (aspectRatioNominator * nMonitorPixelHeight < aspectRatioDenominator * nMonitorPixelWidth)
				{//物理宽高比小于实际像素的宽高比, 水平像素数目保持不变，垂直虚拟像素增加
					nMonitorVirtualPixelHeight = nMonitorVirtualPixelWidth * aspectRatioDenominator / aspectRatioNominator;
				}

				wXData = USHORT((contactPos.x - nMonitorPixelLeft + ((nMonitorVirtualPixelWidth - nMonitorPixelWidth) >> 1)) * EASI_TOUCH_MAXIMUM_X / nMonitorVirtualPixelWidth);
				wYData = USHORT((contactPos.y - nMonitorPixelTop + ((nMonitorVirtualPixelHeight - nMonitorPixelHeight) >> 1)) * EASI_TOUCH_MAXIMUM_Y / nMonitorVirtualPixelHeight);
			}
			break;

			case E_TOUCH_DATA_AJUST_WITH_EMEBED_MODEL:
			{//显示器内容嵌入在触屏中央模型
				const int& nSourceWidth = pDisplayDevInfo->sourceMode.width;
				const int& nSourceHeight = pDisplayDevInfo->sourceMode.height;
				const int& nTargetWidth = pDisplayDevInfo->targetMode.targetVideoSignalInfo.activeSize.cx;
				const int& nTargetHeight = pDisplayDevInfo->targetMode.targetVideoSignalInfo.activeSize.cy;

				wXData = USHORT((contactPos.x - nMonitorPixelLeft + ((nTargetWidth - nSourceWidth) >> 1)) * EASI_TOUCH_MAXIMUM_X / nTargetWidth);
				wYData = USHORT((contactPos.y - nMonitorPixelTop + ((nTargetHeight - nSourceHeight) >> 1)) * EASI_TOUCH_MAXIMUM_Y / nTargetHeight);
			}
			break;
			}//switch(m_eTouchDataAdjustModel)

			m_TouchPoints[i].wXData = wXData;
			m_TouchPoints[i].wYData = wYData;
			bDone = TRUE;
		}

		if (!bDone)
		{
			m_TouchPoints[i].wXData = USHORT((contactPos.x - nMonitorPixelLeft) * EASI_TOUCH_MAXIMUM_X / nCxScreen);
			m_TouchPoints[i].wYData = USHORT((contactPos.y - nMonitorPixelTop) * EASI_TOUCH_MAXIMUM_Y / nCyScreen);
		}
	}//for

	BOOL bRet = EASI_WriteVirtualTouchScreen(m_hDev, &m_TouchPoints[0], nPenCount);
	for(int j = 0; j < nPenCount; j++)
	{
		if(m_TouchPoints[j].bStatus == TIP_DOWN)
		{
			m_TouchPoints[j].bStatus = TIP_UP;
		}
	}
    bRet = EASI_WriteVirtualTouchScreen(m_hDev, &m_TouchPoints[0], nPenCount);

	return bRet;
}

//@功能:判断虚拟驱动是否是打开的。
BOOL CVirtualHID::IsVirtualDriverOpen()
{
    return this->m_hDev != INVALID_HANDLE_VALUE ? TRUE : FALSE;
}


void  CVirtualHID::DeviceHandleEventCallBack(const DeviceHandleMonitorEntry* pEntry, WPARAM dbtEvent)
{
    if (!pEntry) return;

    CVirtualHID* lpThis = reinterpret_cast<CVirtualHID*>(pEntry->lpCtx);

    const TCHAR* lpszEvent = _T("Unkown Event");
    switch (dbtEvent)
    {
    case DBT_DEVICEQUERYREMOVE:

        if (lpThis->m_hDev != INVALID_HANDLE_VALUE)
        {
            lpThis->CloseDevice();

            //驱动若关闭则进入鼠标模式
            lpThis->m_eHIDDeviceMode = E_DEV_MODE_MOUSE;


            //开始侦听设备接口状态。
            const GUID& interface_guid = EASI_GetDeviceInterfaceGUID();
            lpThis->m_oDeviceEventDetector.AddDevIntefaceMonitor(interface_guid, CVirtualHID::DeviceInterfaceEventCallBack, (LPVOID)lpThis);
        }

        lpszEvent = _T("DBT_DEVICEQUERYREMOVE");
        break;

    case DBT_DEVICEQUERYREMOVEFAILED:
        lpszEvent = _T("DBT_DEVICEQUERYREMOVEFAILED");
        break;

    case DBT_DEVICEREMOVEPENDING:
        lpszEvent = _T("DBT_DEVICEREMOVEPENDING");
        break;

    case DBT_DEVICEREMOVECOMPLETE:
        lpszEvent = _T("DBT_DEVICEREMOVECOMPLETE");
        break;

    case DBT_DEVICEARRIVAL:
        lpszEvent = _T("DBT_DEVICEARRIVAL");
        break;
    };

    AtlTrace(_T("Device Event 0x%x:%s\n"), dbtEvent, lpszEvent);
    LOG_INF("Device Event 0x%x:%s\n", dbtEvent, lpszEvent);
}


//@功能:HID设备接口关联事件回调函数
void  CVirtualHID::DeviceInterfaceEventCallBack(const DeviceIntetfaceMonitorEntry* pEntry, WPARAM dbtEvent)
{
    if (!pEntry) return;
    CVirtualHID* lpThis = reinterpret_cast<CVirtualHID*>(pEntry->lpCtx);
    const TCHAR* lpszEvent = _T("Unkown Event");


    switch (dbtEvent)
    {
    case DBT_DEVICEARRIVAL:
    {
        int nTryTimes = 0;
        //测试发现立即打开设备会失败 因此尝试多次
        while (lpThis->m_hDev == INVALID_HANDLE_VALUE && nTryTimes < 10)
        {
            lpThis->OpenDevice();

            if (lpThis->m_hDev == INVALID_HANDLE_VALUE)
            {
                Sleep(10);//10ms
            }
            nTryTimes++;
        }
    }
    lpThis->RetrievePointerDevices();
    lpszEvent = _T("DBT_DEVICEARRIVAL");
    break;

    case DBT_DEVICEREMOVECOMPLETE:
        lpThis->RetrievePointerDevices();

        lpszEvent = _T("DBT_DEVICEREMOVECOMPLETE");
        break;

    case DBT_DEVICEREMOVEPENDING:
        lpszEvent = _T("DBT_DEVICEREMOVEPENDING");
        break;

    };

    AtlTrace(_T("Device Event %d:%s\n"), dbtEvent, lpszEvent);

}


//@功能:设置触屏的尺寸事件响应函数
//@参数:nPhysicalDiagonalLength, 屏幕尺寸,对角线长度, 单位:毫米
//      szLogicalDimension, 屏幕逻辑尺寸，单位:像素
void CVirtualHID::OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension)
{
    //给鼠标双击检测器设置屏幕物理尺寸和屏幕分辨率
    this->m_oVirtualMouse.GetDoubleClickChecker().OnSetTouchScreenDimension(nPhysicalDiagonalLength, szLogicalDimension);

    //给触屏双击检测器设置屏幕物理尺寸和屏幕分辨率
    this->m_oDblClickChecker.OnSetTouchScreenDimension(nPhysicalDiagonalLength, szLogicalDimension);

}


#include "..\inc\Win32UDllLibrary.h"


BOOL CVirtualHID::RetrievePointerDevices()
{
    static CWin32UDllLibrary m_oWin32UDllLibrary;
    //Windows版本小于Win8.0
    if (!IsWindows8OrGreater()) return FALSE;


    m_vecPointerDeviceInfos.clear();

    UINT32 dwCount = 0;
    BOOL bRet = m_oWin32UDllLibrary.GetPointerDevices(&dwCount, NULL);
    if (!bRet) return FALSE;

    if (dwCount == 0) return FALSE;

    m_vecPointerDeviceInfos.resize(dwCount);


    if (!m_oWin32UDllLibrary.GetPointerDevices(&dwCount, &m_vecPointerDeviceInfos[0]))
    {
        return FALSE;
    }

    return TRUE;
}


const TCHAR* g_szUMDFTocuhProductString = _T("UMDF Virtual hidmini device Product string");
//@功能:根据产品字符串提取对应的指针设备信息。
//@参数:lpszProductString, 产品字符串
//@返回值:NULL, 未找到。
//        其他，指向POINTER_DEVICE_INFO的数据指针
POINTER_DEVICE_INFO* CVirtualHID::GetPointerDevice(LPCTSTR lpszProductString)
{
    if (lpszProductString == NULL)
    {
        lpszProductString = g_szUMDFTocuhProductString;
    }
    for (size_t i = 0; i < m_vecPointerDeviceInfos.size(); i++)
    {
        if (_tcsicmp(m_vecPointerDeviceInfos[i].productString, lpszProductString) == 0)
        {
            return  &m_vecPointerDeviceInfos[i];
        }

    }//for

    return NULL;

}


BOOL CVirtualHID::OpenDeviceThreadSafe()
{
    CComCritSecLock<CComAutoCriticalSection> lock(m_csForVirtualDevice);
    m_hDev = EASI_OpenDevice();
    m_oVirtualMouse.SetDeviceHandle(m_hDev);
    if (m_hDev != INVALID_HANDLE_VALUE)
    {
        //将设备句柄加入监控名单。
        m_oDeviceEventDetector.AddHandleMonitor(m_hDev, DeviceHandleEventCallBack, (LPVOID)this);

        return TRUE;
    }

    return FALSE;
}

BOOL CVirtualHID::CloseDeviceThreadSafe()
{
    CComCritSecLock<CComAutoCriticalSection> lock(m_csForVirtualDevice);
    if (m_hDev != INVALID_HANDLE_VALUE)
    {
        BOOL bRet = EASI_CloseDevice(m_hDev);

        //将设备句柄移出监控名单
        m_oDeviceEventDetector.RemoveHandleMonitor(m_hDev);

        m_hDev = INVALID_HANDLE_VALUE;
        m_oVirtualMouse.SetDeviceHandle(INVALID_HANDLE_VALUE);

        return bRet;
    }
    return FALSE;
}


BOOL CVirtualHID::CreateAutoOpenThread()
{
    if (m_hAutoOpenThread == NULL)
    {
        DWORD dwThreadId = 0;
        m_bAutoOpenThreadExit = FALSE;
        m_hAutoOpenThread = CreateThread(
            NULL,
            0,
            AutoOpenThreadProc,
            (LPVOID)this,
            0,
            &dwThreadId);
    }

    return m_hAutoOpenThread == NULL ? FALSE : TRUE;

}
void CVirtualHID::CloseAutoOpenThread()
{
    m_bAutoOpenThreadExit = TRUE;
    WaitForSingleObject(m_hAutoOpenThread, 1000);
    m_hAutoOpenThread = NULL;

}

DWORD WINAPI CVirtualHID::AutoOpenThreadProc(LPVOID lpCtx)
{
    CVirtualHID* lpThis = reinterpret_cast<CVirtualHID*>(lpCtx);
    const int nMaxTryNumber = 60;
    const int nTryInterval = 1000;//ms,尝试间隔
    int nTryCount = 0;
    while (!lpThis->m_bAutoOpenThreadExit
        &&
        lpThis->m_hDev == INVALID_HANDLE_VALUE
        &&
        nTryCount < nMaxTryNumber)
    {

        if (lpThis->OpenDeviceThreadSafe())
        {
            LOG_INF("Open device succeeded in AutoOpenThreadProc.\n");
            lpThis->SetHIDMode(lpThis->m_eDesiredHIDMode);
            break;
        }

        nTryCount++;
        Sleep(nTryInterval);
    }
    lpThis->m_hAutoOpenThread = NULL;

    CloseHandle(lpThis->m_hAutoOpenThread);
    return 0U;
}

//@功能:更新触屏关联的屏幕信息
void CVirtualHID::UpdateAttachedMonitorInfo()
{
    TCHAR pointerDevicePath[MAX_DEVICE_ID_LEN];

    BOOL bRet = GetPointerDeviceDevPath(
        g_szUMDFTocuhProductString,
        pointerDevicePath,
        _countof(pointerDevicePath));

    if (!bRet)
    {
        return;
    }

    int nDisplayDevCount = theApp.GetMonitorFinder().GetDisplayDevCount();

    UINT32 nMaxActiveCx = 0, nMaxActiveCy = 0;
    BOOL bMatched = FALSE;
    for (int i = 0; i < nDisplayDevCount && !bMatched; i++)
    {
        const DisplayDevInfo* pDisplayDevInfo = theApp.GetMonitorFinder().GetDisplayDevInfo(i);

        if (pDisplayDevInfo == NULL) continue;
        int nDiplayAdpaterInfoCount = pDisplayDevInfo->displayAdapterInfos.size();

        for (int j = 0; j < nDiplayAdpaterInfoCount; j++)
        {
            const DiplayAdpaterInfo& inf = pDisplayDevInfo->displayAdapterInfos[j];
            //Monitor 1: 1920*1080
            //Minitor 2: 1920*1200
            //系统选择1920:1080作为触屏的宽高比

            //if (IsWin10OrGreater())
            {
                if (IsMonitorAttachedToPointerDevice(
                    pointerDevicePath,
                    inf.monitorDevicePath))
                {
                    m_aspectRatioNominator   = inf.targetMode.targetVideoSignalInfo.activeSize.cx;
                    m_aspectRatioDenominator =  inf.targetMode.targetVideoSignalInfo.activeSize.cy;

                    bMatched = TRUE;
                    break;
                 }
            }

            if (nMaxActiveCx < inf.targetMode.targetVideoSignalInfo.activeSize.cx)
            {
                nMaxActiveCx = inf.targetMode.targetVideoSignalInfo.activeSize.cx;
                nMaxActiveCy = inf.targetMode.targetVideoSignalInfo.activeSize.cy;
            }

        }//fo(j)

    }//for(i)

    //const DiplayAdpaterInfo& inf = theApp.GetMonitorFinder().GetDisplayDevInfo(0)->displayAdapterInfos[0];
    //m_aspectRatioNominator = inf.targetMode.targetVideoSignalInfo.activeSize.cx;
    //m_aspectRatioDenominator = inf.targetMode.targetVideoSignalInfo.activeSize.cy;

    if (!bMatched)
    {
        m_aspectRatioNominator = nMaxActiveCx;
        m_aspectRatioDenominator = nMaxActiveCy;
    }
}

void CVirtualHID::SetTouchTUIOMode(bool  eMode)
{
    m_bTouchTUIOMode = eMode;
}
void CVirtualHID::SetTouchHIDMode(bool  eMode)
{
    /////m_bTouchHIDMode = true; 
    /////eMode = false ;
    if (m_bTouchHIDMode && !eMode)
    {
        ///停用掉HID触控模式，要Reset();
        Reset();
    }
    m_bTouchHIDMode = eMode;
}
void CVirtualHID::SetSinglePointMode(bool eMode)
{
    m_bSinglePointMode = eMode;
}

void  CVirtualHID::SetTUIOParams(DWORD IP, int nPort, int nScreenWindth, int nScreenHeight)
{
    m_oVirtualTUIOTouch.SetTUIOParams(IP, nPort, nScreenWindth, nScreenHeight);
}

void CVirtualHID::OpenTUIOServer(bool bStart)
{
	m_oVirtualTUIOTouch.OpenTUIOServer(bStart);
}

DWORD CVirtualHID::GetIPadress()
{
    return m_oVirtualTUIOTouch.GetIPadress();
}

int CVirtualHID::GetPort()
{
    return m_oVirtualTUIOTouch.GetPort();
}
int CVirtualHID::GetScreenWidth()
{
	return m_oVirtualTUIOTouch.GetScreenWidth();
}

int CVirtualHID::GetScreenHeight()
{
	return m_oVirtualTUIOTouch.GetScreenHeight();
}

void CVirtualHID::SetTUIOScreenDisplayChange(int nScreenX ,int nScreenY)
{
	m_oVirtualTUIOTouch.SetTUIOScreenDisplayChange(nScreenX, nScreenY);
}

void CVirtualHID::SetTest30Point(BOOL bStart)
{
	m_bStartTest30Point = bStart;
}

BOOL CVirtualHID::GetTest30Point()
{
	return m_bStartTest30Point;
}

void CVirtualHID::SetAirOperateMode(BOOL eMode,EAIROPERATE_CLICKMODE  eClickMode)
{
	m_bAirOperationMode = eMode;
	m_eClickMode = eClickMode;
}