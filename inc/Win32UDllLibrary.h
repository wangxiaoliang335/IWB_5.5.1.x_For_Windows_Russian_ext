#pragma once
//
class CWin32UDllLibrary {
public:
	//windows 未公开API函数NtUserSetDisplayMapping的调用
	typedef void  (WINAPI * _NtUserSetDisplayMapping)(HANDLE hPointerDevice, HMONITOR hMonitor);

	typedef BOOL(WINAPI* _GetPointerDevices)(UINT32 *deviceCount, POINTER_DEVICE_INFO *pointerDevices);

	CWin32UDllLibrary():
		m_pfNtUserDisplayMapping(NULL),
		m_pfGetPointerDevices(NULL),
		m_hModule(NULL)
	{

		TCHAR szSystemDirectory[MAX_PATH];

		UINT uRet = GetSystemWow64Directory(
			szSystemDirectory,//_Out_ LPTSTR lpBuffer,
			MAX_PATH
		);

		if (uRet == 0)
		{//32 Bit Os

			uRet = GetSystemDirectory(
				szSystemDirectory,//_Out_ LPTSTR lpBuffer,
				MAX_PATH
			);

		}


		TCHAR szWin32UDllPath[MAX_PATH];
		if (uRet)
		{//64 Bit Os
			_stprintf_s(szWin32UDllPath, _countof(szWin32UDllPath), _T("%s\\win32u.dll"), szSystemDirectory);
		}
		else
		{
			return;
		}


		m_hModule = LoadLibrary(szWin32UDllPath);


		if (m_hModule)
		{
			m_pfNtUserDisplayMapping = (_NtUserSetDisplayMapping)GetProcAddress(m_hModule, "NtUserSetDisplayMapping");
			m_pfGetPointerDevices    = (_GetPointerDevices      )GetProcAddress(m_hModule, "NtUserGetPointerDevices");
		}

	}

	~CWin32UDllLibrary()
	{
		if (m_hModule)
		{
			FreeLibrary(m_hModule);
		}
	}
	
	void  NtUserSetDisplayMapping (HANDLE hPointerDevice, HMONITOR hMonitor)
	{
		if (m_pfNtUserDisplayMapping)
		{
			return m_pfNtUserDisplayMapping(hPointerDevice, hMonitor);
		}
	}

	BOOL	GetPointerDevices(UINT32 *deviceCount, POINTER_DEVICE_INFO *pointerDevices)
	{
		if (m_pfGetPointerDevices)
		{
			return m_pfGetPointerDevices(deviceCount, pointerDevices);
		}

		return FALSE;
	}

protected:
	HMODULE m_hModule;


	_NtUserSetDisplayMapping m_pfNtUserDisplayMapping;
	_GetPointerDevices m_pfGetPointerDevices;
};