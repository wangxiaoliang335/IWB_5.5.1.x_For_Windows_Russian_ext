#include "stdafx.h"
#include "DispMonitorFinder.h"
#include "EDID.h"
//EnumDisplayMonitors ---> hMonitor -- >MONITORINFOEX
//EnumDisplayDevices  ----------------->DISPLAY_DEVICE ==>DISPLAY_DEVICE.DeviceName == MONITORINFOEX.szDevice
//SetupDiGetClassDevsEx --->HDEVINFO --->SetupDiEnumDeviceInfo ---> SP_DEVINFO_DATA -->SetupDiGetDeviceInstanceId


#include <SetupApi.h>
#include <cfgmgr32.h>   // for MAX_DEVICE_ID_LEN
#pragma comment(lib, "setupapi.lib")

#include <limits>
#include "md5.h"

const GUID GUID_CLASS_MONITOR = { 0x4d36e96e, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 };


void _cdecl DBGPrint(const TCHAR* szFormat, ...)
{

	TCHAR text[1024];
	va_list vlist;
	va_start(vlist, szFormat);

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	_vstprintf_s(text, _countof(text), szFormat, vlist);


	//在Debug Output中输出
	OutputDebugString(text);

	//在控制台中输出
//   _tprintf(text);

	va_end(vlist);
}


//@功能:根据IntefaceName读取显示器的EDID数据
BOOL GetEDIDByIntefaceName(const TCHAR* szInterfaceName, CEDID* pEDID)
{
	DBGPrint(_T("GetEDIDByIntefaceName.\n"));
	HDEVINFO hDevInfoSet = SetupDiGetClassDevsEx(
		&GUID_CLASS_MONITOR, //class GUID
		NULL, //enumerator
		NULL, //HWND
		DIGCF_PRESENT | DIGCF_PROFILE, // Flags //DIGCF_ALLCLASSES|
		NULL, // device info, create a new one.
		NULL, // machine name, local machine
		NULL);// reserved

	if (NULL == hDevInfoSet)
		return FALSE;

	BOOL bRes = FALSE;


	DBGPrint(_T("[%s]szInterfaceName=%s\n"), (LPCTSTR)CA2CT(__FUNCTION__), szInterfaceName);

	for (ULONG i = 0; ERROR_NO_MORE_ITEMS != GetLastError(); ++i)
	{
		SP_DEVINFO_DATA devInfoData;
		memset(&devInfoData, 0, sizeof(devInfoData));
		devInfoData.cbSize = sizeof(devInfoData);

		if (SetupDiEnumDeviceInfo(hDevInfoSet, i, &devInfoData))
		{
			TCHAR szInstance[MAX_DEVICE_ID_LEN];
			DWORD dwRequiredSize;

			bRes = SetupDiGetDeviceInstanceId(hDevInfoSet, &devInfoData, szInstance, _countof(szInstance), &dwRequiredSize);


			if (!bRes)
				continue;


			//szInstance: DISPLAY\SECA609\5&C42F4D2&0&UID4353
			//szInterfaceName: \\?\DISPLAY\SECA609\5&c42f4d2&0&UID4353\{e6f07b5f-ee97-4a90-b076-33f57bf4eaa7}
			DBGPrint(_T("[%s]szInstance=%s\n"), (LPCTSTR)CA2CT(__FUNCTION__), szInstance);
			if (_tcsstr(szInterfaceName, szInstance) == NULL)
			{
				DBGPrint(_T("[%s] _tcsstr return null\n"), (LPCTSTR)CA2CT(__FUNCTION__));
				continue;
			}


			HKEY hEDIDRegKey = SetupDiOpenDevRegKey(hDevInfoSet, &devInfoData,
				DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);

			if (!hEDIDRegKey || (hEDIDRegKey == INVALID_HANDLE_VALUE))
				continue;

			DWORD dwEDIDSize = pEDID->GetSize();
			bRes = (ERROR_SUCCESS == RegQueryValueEx(hEDIDRegKey, _T("EDID"), NULL, NULL, pEDID->GetData(), &dwEDIDSize)) ? TRUE : FALSE;

			RegCloseKey(hEDIDRegKey);
			break;

		}//if

	}//for


	SetupDiDestroyDeviceInfoList(hDevInfoSet);
	return bRes;
}


//@功能:从监视器句柄读取对应的
BOOL DisplayDeviceFromHMonitor(HMONITOR hMonitor, DISPLAY_DEVICE& ddMonOut)
{
	MONITORINFOEX mi;
	mi.cbSize = sizeof(MONITORINFOEX);

	//读取监视器信息
	GetMonitorInfo(hMonitor, &mi);

	DISPLAY_DEVICE dd;
	dd.cb = sizeof(dd);
	DWORD devIdx = 0; // device index

	BOOL bFoundDevice = FALSE;
	while (EnumDisplayDevices(0, devIdx, &dd, 0))
	{
		devIdx++;

		if (0 != _tcscmp(dd.DeviceName, mi.szDevice))
			continue;

		if (dd.StateFlags & DISPLAY_DEVICE_ACTIVE &&
			dd.StateFlags & DISPLAY_DEVICE_ATTACHED) // for ATI, Windows XP
			break;


		DISPLAY_DEVICE ddMon;
		ZeroMemory(&ddMon, sizeof(ddMon));
		ddMon.cb = sizeof(ddMon);
		DWORD MonIdx = 0;

		while (EnumDisplayDevices(dd.DeviceName, MonIdx, &ddMon, 0))
		{

			//flags == 0时ddMon.DeviceID = MONITOR\SECA609\{4d36e96e-e325-11ce-bfc1-08002be10318}\0004
			 //while (EnumDisplayDevices(dd.DeviceName, MonIdx, &ddMon, EDD_GET_DEVICE_INTERFACE_NAME))
			//flags == EDD_GET_DEVICE_INTERFACE_NAME
			//ddMon.DeviceID:\\?\DISPLAY#SECA609#5&c42f4d2&0&UID4353#{e6f07b5f-ee97-4a90-b076-33f57bf4eaa7}
			//比较
			//SetupDiGetDeviceInstanceId(hDevInfoSet, &devInfoData, szInstance, _countof(szInstance), &dwRequiredSize);
			//返回的szInstance:DISPLAY\SECA609\5&C42F4D2&0&UID4353
			//https://msdn.microsoft.com/en-us/library/windows/desktop/dd162609(v=vs.85).aspx
			//EnumDisplayDevices function
			//The resulting device interface name can be used with SetupAPI functions and serves as a link between GDI monitor devices and SetupAPI monitor devices.

			MonIdx++;

			ddMonOut = ddMon;

			bFoundDevice = TRUE;
			ZeroMemory(&ddMon, sizeof(ddMon));
			ddMon.cb = sizeof(ddMon);


		}

		ZeroMemory(&dd, sizeof(dd));
		dd.cb = sizeof(dd);
	}

	return bFoundDevice;
}

//@功能:在注册表中读取显卡输出信号格式
BOOL ReadSignalFormat(const CEDID& edid, MonitorSignalFormat& msf)
{
	//HKLM\SYSTEM\CurrentControlSet\Control\GraphicsDrivers\Configuration\SAM030CHVCQ105428_02_07D8_39^F1380378CC88595A40A570C9B0B88CB9\00\00
	//如何匹配显示器和注册表键值
	//注册表键项的名称示例如下:
	//SAM030CHVCQ105428_02_07D8_39^F1380378CC88595A40A570C9B0B88CB9
	//SAM030CHVCQ105428_02_07D8_39^24A2EF10DE3641BA178BD45E316F2CAB
	//SAM030CHVCQ105428_02_07D8_39由EDID中的如下内容组成
	//ID Manufacture Name:ASM
	//ID Serial Number : 030
	//Alphanumeric Data String Descriptor Definition : CHVCQ105428
	//WEEK : 02
	//YEAR : 07D8(2008)
	//Checsum:39
	//F1380378CC88595A40A570C9B0B88CB9=MD5("SAM030CHVCQ105428_02_07D8_39");
	EDIDInfo result;

	edid.Parse(result);


	char szKey[0124];
	sprintf_s(
		szKey,
		_countof(szKey),
		"%c%c%c%04X%s_%02X_%04X_%02X",
		result.szvendor[0],
		result.szvendor[1],
		result.szvendor[2],
		result.wProductId,
		result.GetSerialNumber(),
		result.cWeek,
		result.wYear,
		result.checkSum);


	std::string strMD5 = md5(szKey);

	strcat_s(szKey, _countof(szKey), "^");
	strcat_s(szKey, _countof(szKey), strMD5.c_str());


	TCHAR szRegKeyPath[1024];
	_stprintf_s(
		szRegKeyPath,
		_countof(szRegKeyPath),
		_T("SYSTEM\\CurrentControlSet\\Control\\GraphicsDrivers\\Configuration\\%s\\00\\00"),
		(LPCTSTR)CA2CT(szKey));

	HKEY hRegKey = 0;
	LONG lResult =
		RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			szRegKeyPath,
			0,
			KEY_READ,
			&hRegKey);

	if (lResult != ERROR_SUCCESS)
	{
		return FALSE;
	}


	DWORD dwPrimSurfSize_cx, dwPrimSurfSize_cy;

	DWORD dwSize = sizeof(dwPrimSurfSize_cx);
	lResult =
		RegQueryValueEx(
			hRegKey,
			_T("PrimSurfSize.cx"),
			NULL,
			NULL,
			(LPBYTE)&dwPrimSurfSize_cx,
			&dwSize);

	if (lResult != ERROR_SUCCESS)
	{
		return FALSE;
	}

	dwSize = sizeof(dwPrimSurfSize_cy);

	lResult =
		RegQueryValueEx(
			hRegKey,
			_T("PrimSurfSize.cy"),
			NULL,
			NULL,
			(LPBYTE)&dwPrimSurfSize_cy,
			&dwSize);
	if (lResult != ERROR_SUCCESS)
	{
		return FALSE;
	}


	RegCloseKey(hRegKey);



	return TRUE;
}

//@功能:从监视器句柄读取对应的物理显示屏的信息
BOOL DisplayDeviceFromHMonitor(HMONITOR hMonitor, std::vector<DISPLAY_DEVICE>& vecDisplayDevices)
{
	MONITORINFOEX mi;
	mi.cbSize = sizeof(MONITORINFOEX);

	//读取监视器信息
	GetMonitorInfo(hMonitor, &mi);

	DISPLAY_DEVICE dd;
	dd.cb = sizeof(dd);
	DWORD devIdx = 0; // device index

	BOOL bFoundDevice = FALSE;
	while (EnumDisplayDevices(0, devIdx, &dd, 0))
	{
		devIdx++;

		if (0 != _tcscmp(dd.DeviceName, mi.szDevice))
			continue;

		if (dd.StateFlags & DISPLAY_DEVICE_ACTIVE &&
			dd.StateFlags & DISPLAY_DEVICE_ATTACHED) // for ATI, Windows XP
			break;


		DISPLAY_DEVICE ddMon;
		ZeroMemory(&ddMon, sizeof(ddMon));
		ddMon.cb = sizeof(ddMon);
		DWORD MonIdx = 0;

		//while (EnumDisplayDevices(dd.DeviceName, MonIdx, &ddMon, 0))
	   //flags == 0时ddMon.DeviceID = MONITOR\SECA609\{4d36e96e-e325-11ce-bfc1-08002be10318}\0004		
		UINT32 flags = EDD_GET_DEVICE_INTERFACE_NAME;

		while (EnumDisplayDevices(dd.DeviceName, MonIdx, &ddMon, flags))

			//ddMon.DeviceID:\\?\DISPLAY#SECA609#5&c42f4d2&0&UID4353#{e6f07b5f-ee97-4a90-b076-33f57bf4eaa7}
			//比较
			//SetupDiGetDeviceInstanceId(hDevInfoSet, &devInfoData, szInstance, _countof(szInstance), &dwRequiredSize);
			//返回的szInstance:DISPLAY\SECA609\5&C42F4D2&0&UID4353
			//https://msdn.microsoft.com/en-us/library/windows/desktop/dd162609(v=vs.85).aspx
			//EnumDisplayDevices function
			//The resulting device interface name can be used with SetupAPI functions and serves as a link between GDI monitor devices and SetupAPI monitor devices.
		{
			MonIdx++;

			if ((ddMon.StateFlags & DISPLAY_DEVICE_ATTACHED) && (ddMon.StateFlags & DISPLAY_DEVICE_ACTIVE))
			{
				vecDisplayDevices.push_back(ddMon);
			}

			bFoundDevice = TRUE;
			ZeroMemory(&ddMon, sizeof(ddMon));
			ddMon.cb = sizeof(ddMon);
		}

		ZeroMemory(&dd, sizeof(dd));
		dd.cb = sizeof(dd);
	}

	return bFoundDevice;
}

CDispDevFinder::CDispDevFinder()
	:
	m_bScreenMergeMode(FALSE)
{
	SearchDisplayDev();//搜索屏幕信息
}


CDispDevFinder::~CDispDevFinder()
{

}



//@功  能:搜索显示器
//@返回之:成功, TRUE
//        失败, FALSE
BOOL CDispDevFinder::SearchDisplayDev()
{

	m_vecDisplayDevInfo.clear();

	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)this);

	//<<2017/10/20
	//改用新的API获取显示器信息
	HRESULT hr = S_OK;
	UINT32 NumPathArrayElements = 0;
	UINT32 NumModeInfoArrayElements = 0;
	DISPLAYCONFIG_PATH_INFO* PathInfoArray = nullptr;
	DISPLAYCONFIG_MODE_INFO* ModeInfoArray = nullptr;
	DISPLAYCONFIG_TOPOLOGY_ID currentTopologyId = DISPLAYCONFIG_TOPOLOGY_FORCE_UINT32;
	do
	{
		delete[] PathInfoArray;
		PathInfoArray = nullptr;

		delete[] ModeInfoArray;
		ModeInfoArray = nullptr;

		//hr = HRESULT_FROM_WIN32(GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &NumPathArrayElements, &NumModeInfoArrayElements));
		hr = HRESULT_FROM_WIN32(GetDisplayConfigBufferSizes(QDC_DATABASE_CURRENT, &NumPathArrayElements, &NumModeInfoArrayElements));
		
		if (FAILED(hr))
		{
			break;
		}

		PathInfoArray = new(std::nothrow) DISPLAYCONFIG_PATH_INFO[NumPathArrayElements];
		if (PathInfoArray == nullptr)
		{
			hr = E_OUTOFMEMORY;
			break;
		}

		ModeInfoArray = new(std::nothrow) DISPLAYCONFIG_MODE_INFO[NumModeInfoArrayElements];
		if (ModeInfoArray == nullptr)
		{
			hr = E_OUTOFMEMORY;
			break;
		}

		//hr = HRESULT_FROM_WIN32(QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &NumPathArrayElements, PathInfoArray, &NumModeInfoArrayElements, ModeInfoArray, nullptr));
		hr = HRESULT_FROM_WIN32(QueryDisplayConfig(QDC_DATABASE_CURRENT, &NumPathArrayElements, PathInfoArray, &NumModeInfoArrayElements, ModeInfoArray, &currentTopologyId));
	} while (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));


	if (FAILED(hr))
	{
		return FALSE;
	}



	DISPLAYCONFIG_SOURCE_DEVICE_NAME sourceName = {};
	DISPLAYCONFIG_TARGET_DEVICE_NAME targetName = {};

	for (size_t monitorInfoIdx = 0; monitorInfoIdx < m_vecDisplayDevInfo.size(); monitorInfoIdx++)
	{
		DisplayDevInfo& refMonitorInfo = m_vecDisplayDevInfo[monitorInfoIdx];
        refMonitorInfo.displayAdapterInfos.clear();
		
		DWORD dwMaxOptimalWidth = 0, dwMaxOptimalHeight = 0;

		for (UINT PathIdx = 0; PathIdx < NumPathArrayElements; PathIdx ++)
		{
            DiplayAdpaterInfo displayAdapterInfo;
			sourceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
			sourceName.header.size = sizeof(sourceName);
			sourceName.header.adapterId = PathInfoArray[PathIdx].sourceInfo.adapterId;
			sourceName.header.id = PathInfoArray[PathIdx].sourceInfo.id;

			hr = HRESULT_FROM_WIN32(DisplayConfigGetDeviceInfo(&sourceName.header));
			if (SUCCEEDED(hr))
			{
				
			}

			//目标Monitor信息
			targetName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
			targetName.header.size = sizeof(targetName);
			targetName.header.adapterId = PathInfoArray[PathIdx].targetInfo.adapterId;
			targetName.header.id = PathInfoArray[PathIdx].targetInfo.id;
			hr = HRESULT_FROM_WIN32(DisplayConfigGetDeviceInfo(&targetName.header));
			if (SUCCEEDED(hr))
			{
				
			}

			int sourceModeIdx = PathInfoArray[PathIdx].sourceInfo.modeInfoIdx;
			int targetModeIdx = PathInfoArray[PathIdx].targetInfo.modeInfoIdx;
			
			//在同屏复制模式下，选择ActiveSize尺寸最大作为触屏尺寸,此策略经过测试发现是错误的
			//尝试，以第一个硬件屏幕的信号格式为准,例如
			//如下情形:
			//ESPON PJ : source=1024*768 Active Size = 1366*768(16:9)
			//SMD1920NW: source=1024*768 Active Size = 1440*900(16:10)
			//选择第一块屏16:9的宽高比，触控位置是精准的,选择第二块屏16:10的宽高比，则触控水平位置
			//由屏幕中间向屏幕两边偏移。
			//
			//但在一台带内置显示器的微型win10计算机上，
			//屏幕0的 Active.Size=1280*800, PrimSurf.size=1280*800
			//屏幕1的 Active.Size=1920*1080, PrimSurf.size=1280*800
			//如果使用第一个屏幕，则宽高比应该为16:10, 但实际使用时, 触屏宽高比为16：9才能正确地触控。
			//
			//南非客户反映:在屏幕分辨率为1366*768(16:9)的笔记本上输出到xvga格式1024*768(4:3)，SVGA格式800*600(4:3)的投影机上,发现
			//触屏不准，但鼠标是准确的。
			//Monitor[0]: souce=1024*768  Active Size=1024*768(12:9)
			//Monitor[1]: source=1024*768 Active Size=1366*768(16:9)
			//
			
			if (_tcsicmp(refMonitorInfo.szDevice, sourceName.viewGdiDeviceName) == 0)
			{
				//if (ModeInfoArray[targetModeIdx].targetMode.targetVideoSignalInfo.activeSize.cx > dwMaxOptimalWidth)
				{
					dwMaxOptimalWidth = ModeInfoArray[targetModeIdx].targetMode.targetVideoSignalInfo.activeSize.cx;

				}

				//if (ModeInfoArray[targetModeIdx].targetMode.targetVideoSignalInfo.activeSize.cy > dwMaxOptimalHeight)
				{
					dwMaxOptimalHeight = ModeInfoArray[targetModeIdx].targetMode.targetVideoSignalInfo.activeSize.cy;
				}


                displayAdapterInfo.targetMode = ModeInfoArray[targetModeIdx].targetMode;
                displayAdapterInfo.sourceMode = ModeInfoArray[sourceModeIdx].sourceMode;

                _tcsupr_s(
                    targetName.monitorDevicePath,
                    _tcslen(targetName.monitorDevicePath) + 1
                );

                _tcscpy_s(
                    displayAdapterInfo.monitorDevicePath,
                    _countof(displayAdapterInfo.monitorDevicePath),
                    targetName.monitorDevicePath);


                _tcscpy_s(
                    displayAdapterInfo.moniotrName,
                    _countof(displayAdapterInfo.moniotrName),
                    targetName.monitorFriendlyDeviceName);

                refMonitorInfo.displayAdapterInfos.push_back(displayAdapterInfo);


				refMonitorInfo.targetMode = ModeInfoArray[targetModeIdx].targetMode;
				refMonitorInfo.sourceMode = ModeInfoArray[sourceModeIdx].sourceMode;
				refMonitorInfo.targetInfo = PathInfoArray[PathIdx].targetInfo;

				const int XVGA_WIDTH  = 1024;
				const int XVGA_HEIGHT = 768;
				if (dwMaxOptimalWidth <= XVGA_WIDTH && dwMaxOptimalHeight <= XVGA_HEIGHT)
				{//针对南非客户反映, 如果第一个屏幕的尺寸小于等于XVGA则继续查找下一个屏幕。
					continue;
				}
				else
				{
					//break;
				}
				//if(currentTopologyId == DISPLAYCONFIG_TOPOLOGY_CLONE)
			}

		}//for

		if (dwMaxOptimalWidth != 0x00 && dwMaxOptimalHeight != 0x00)
		{
			refMonitorInfo.OptimalWidthInPixel  = dwMaxOptimalWidth;
			refMonitorInfo.OptimalHeightInPixel = dwMaxOptimalHeight;
		}

	}//for



	delete[] PathInfoArray;
	PathInfoArray = nullptr;

	delete[] ModeInfoArray;
	ModeInfoArray = nullptr;

	//20171020>>





	//按照屏幕的水平坐标排序, 确保从左到右升序排列
	for (size_t i = 0; i < m_vecDisplayDevInfo.size() - 1; i++)
	{
		for (size_t j = m_vecDisplayDevInfo.size() - 1; j > i; j--)
		{
			DisplayDevInfo monitor1 = m_vecDisplayDevInfo[j - 1];
			DisplayDevInfo monitor2 = m_vecDisplayDevInfo[j];

			if (monitor1.rcMonitor.left > monitor2.rcMonitor.left)
			{
				m_vecDisplayDevInfo[j    ] = monitor1;
				m_vecDisplayDevInfo[j - 1] = monitor2;
			}

		}

	}


	return TRUE;

}

//@功  能:搜索显示器
int CDispDevFinder::GetDisplayDevCount()const
{

	return m_vecDisplayDevInfo.size();
}

//@功能:返回指定编号的显示器信息。
//@参数:nNum, 从0开始的显示器编号,
//@说明:当输入的编号参数大于实际检测到的显示器个数则返回NULL
const DisplayDevInfo* CDispDevFinder::GetDisplayDevInfo(int nId)const
{
	if (nId < 0 || nId >= (int)m_vecDisplayDevInfo.size()) return NULL;
	return &m_vecDisplayDevInfo[nId];
}

//@功能:提取坐标点所在屏幕的信息
const DisplayDevInfo* CDispDevFinder::GetDisplayDevInfo(int x, int y) const
{
	if (0 == m_vecDisplayDevInfo.size()) return NULL;

	for (size_t i = 0; i < m_vecDisplayDevInfo.size(); i++)
	{
		const DisplayDevInfo& mi = m_vecDisplayDevInfo[i];

		const RECT& rcMonitor = mi.rcMonitor;

		if (mi.rcMonitor.left < x && x < mi.rcMonitor.right && mi.rcMonitor.top < y && y < mi.rcMonitor.bottom)
		{
			return &mi;
		}


	}//for-each(i)

	return NULL;
}


//静态函数
BOOL CALLBACK CDispDevFinder::MonitorEnumProc(HMONITOR hMonitor,  // handle to display monitor
	HDC hdcMonitor,     // handle to monitor DC
	LPRECT lprcMonitor, // monitor intersection rectangle
	LPARAM dwData)     // data;
{
	CDispDevFinder* lpThis = reinterpret_cast<CDispDevFinder*>(dwData);

	MONITORINFOEX  miex;
	memset(&miex, 0, sizeof(MONITORINFOEX));
	miex.cbSize = sizeof(MONITORINFOEX);
	BOOL bRet = ::GetMonitorInfo(hMonitor, &miex);

	if (!bRet) return FALSE;

	DisplayDevInfo info;
	//info.hMonitor = hMonitor;


	//enum all display settings

	int iModeNum = 0;
	std::vector<DEVMODE> vecDisplaySettings;

	DEVMODE devMode;
	devMode.dmSize = sizeof(DEVMODE);
	devMode.dmDriverExtra = 0;
	while (EnumDisplaySettings(
		miex.szDevice,
		iModeNum,
		&devMode
	))
	{

		vecDisplaySettings.push_back(devMode);
		iModeNum++;
	}

	//搜索最大分辨率,认为在最大分辨率下是全屏显示的
	DWORD dwMaxPelsWidth = 0, dwMaxPelsHeight = 0;

	for (size_t i = 0; i < vecDisplaySettings.size(); i++)
	{
		const DEVMODE& mode = vecDisplaySettings[i];
		if ((mode.dmFields & DM_PELSWIDTH) && (mode.dmFields & DM_PELSHEIGHT))
		{
			if (mode.dmPelsWidth * mode.dmPelsHeight > dwMaxPelsWidth*dwMaxPelsHeight)
			{
				dwMaxPelsWidth = mode.dmPelsWidth;
				dwMaxPelsHeight = mode.dmPelsHeight;
			}
		}
	}//for


	//DEVMODE devMode;
	devMode.dmSize = sizeof(DEVMODE);
	devMode.dmDriverExtra = 0;
	EnumDisplaySettings(
		miex.szDevice,
		ENUM_CURRENT_SETTINGS,
		&devMode
	);




	//DMDFO_DEFAULT	The display's default setting.Aspect-Ratio-Preserving Stretched
	//DMDFO_CENTER	The low-resolution image is centered in the larger screen space.
	//DMDFO_STRETCH	The low-resolution image is stretched to fill the larger screen space.
	//https://docs.microsoft.com/en-us/windows-hardware/drivers/display/scaling-the-desktop-image
	//Currently, on tablet systems, the default is stretched. On non-tablet systems with graphics adapters that support the Windows Display Driver Model (WDDM), 
	//the default is defined by the driver. On non-tablet systems with graphics adapters that support the Windows Display Driver Model (WDDM) with features new 
	//for Windows 7, the default is DC_ASPECTRATIOCENTEREDMAX.
	info.CurrentSetings = devMode;
	info.MaxWidthInPixel = dwMaxPelsWidth;
	info.MaxHeightInPixel = dwMaxPelsHeight;




	RECT& rcMonitor = miex.rcMonitor;
	int nMonitorWidth = rcMonitor.right - rcMonitor.left;
	int nMonitorHeight = rcMonitor.bottom - rcMonitor.top;

	_tcscpy_s(info.szDevice, _countof(info.szDevice), miex.szDevice);


	info.rcMonitor = rcMonitor;
	//info.physicalMaxWidth     = -1;
	//info.physicalMaxHeight    = -1;
	//info.physicalActualWidth  = -1;
	//info.physicalActualHeight = -1;

#if 0  //禁用代码块

/*
	//屏幕复制模式下可能有多个屏幕
	std::vector<DISPLAY_DEVICE> vecDisplayDevices;


	//Retrieves info all the attached monitor
	bRet = DisplayDeviceFromHMonitor(hMonitor, vecDisplayDevices);



	info.OptimalWidthInPixel = 0;
	info.OptimalHeightInPixel = 0;

	if (bRet)
	{

		std::vector<CEDID> vecEDIDs;


		for (size_t i = 0; i < vecDisplayDevices.size(); i++)
		{
			//DEVICE_ID of structure DISPLAY_DEVICE stores the device interface name
			//\\?\DISPLAY#SECA609#5&c42f4d2&0&UID4353#{e6f07b5f-ee97-4a90-b076-33f57bf4eaa7}
			TCHAR szInterfaceName[MAX_DEVICE_ID_LEN];
			_tcscpy_s(szInterfaceName, _countof(szInterfaceName), vecDisplayDevices[i].DeviceID);


			TCHAR* pChar = szInterfaceName;
			while (*pChar)
			{
				if (*pChar == _T('#'))
				{////Replace '#' with '\'
					*pChar = _T('\\');
				}
				else if (_T('a') <= *pChar && *pChar <= _T('z'))
				{//Capitialized
					*pChar = *pChar - 0x20;

				}
				pChar++;
			}//while


			CEDID edid;


			bRet = GetEDIDByIntefaceName(szInterfaceName, &edid);
			if (bRet)
			{
				MonitorSignalFormat msf;
				ReadSignalFormat(edid, msf);
				vecEDIDs.push_back(edid);
			}//if(bRet)

		}//for




		DWORD dwMaxOptimalWidth = 0, dwMaxOptimalHeight = 0;

		//选取最大最优分辨率
		for (size_t i = 0; i < vecEDIDs.size(); i++)
		{
			WORD wOptimalWidth = 0;
			WORD wOptimalHeight = 0;
			vecEDIDs[i].GetOptimalImageSize(&wOptimalWidth, &wOptimalHeight);

			if (((DWORD)wOptimalWidth * (DWORD)wOptimalHeight) > dwMaxOptimalWidth * dwMaxOptimalHeight)
			{
				dwMaxOptimalWidth = (DWORD)wOptimalWidth;
				dwMaxOptimalHeight = (DWORD)wOptimalHeight;
			}

		}


		//在TabletPC的情形下, 屏幕可以翻转, 从EDID读取的最佳分辨率是不旋转时的
		//分辨率， 因此最佳分辨率的宽高在旋转90度和270度时，宽高需要交换。
		switch (devMode.dmDisplayOrientation)
		{
		case DMDO_DEFAULT:
		case DMDO_180:
			info.OptimalWidthInPixel = dwMaxOptimalWidth;
			info.OptimalHeightInPixel = dwMaxOptimalHeight;
			break;

		case DMDO_90:
		case DMDO_270:
			info.OptimalWidthInPixel = dwMaxOptimalHeight;
			info.OptimalHeightInPixel = dwMaxOptimalWidth;
			break;

		}//switch

	}//if
*/
#endif


	info.bIsPrimary = (miex.dwFlags &  MONITORINFOF_PRIMARY) ? TRUE : FALSE;

	lpThis->m_vecDisplayDevInfo.push_back(info);

	return TRUE;
}

//@功能:使能屏幕拼接模式
//@参数:bEnable,是否使能拼接模式
//void CDispDevFinder::EnableScreenMergeMode(BOOL bEnable)
//{
//    m_bScreenMergeMode = bEnable;
//}

//@功能:获取扩展屏的信息
const DisplayDevInfo* CDispDevFinder::GetFirstExtendMonitorInfo()const
{

	for (size_t nId = 0; nId < m_vecDisplayDevInfo.size(); nId++)
	{
		if (!m_vecDisplayDevInfo[nId].bIsPrimary)
		{
			return &m_vecDisplayDevInfo[nId];
		}
	}

	return NULL;
}

//@功能:获取主屏幕信息·
const DisplayDevInfo* CDispDevFinder::GetPrimaryMonitorInfo()const
{
	for (size_t nId = 0; nId < m_vecDisplayDevInfo.size(); nId++)
	{
		if (m_vecDisplayDevInfo[nId].bIsPrimary)
		{
			return &m_vecDisplayDevInfo[nId];
		}
	}

	return NULL;

}

