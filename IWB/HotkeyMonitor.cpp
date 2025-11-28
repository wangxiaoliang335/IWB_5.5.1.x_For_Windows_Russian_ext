#include "stdafx.h"
#include "HotkeyMonitor.h"

LRESULT CALLBACK CHotkeyMonitor::LowLevelKeyboardProc( int nCode   ,
									  WPARAM wParam,
									  LPARAM lParam
									  )
{

	if(nCode<0)
	{
		return CallNextHookEx(NULL,nCode,wParam,lParam);
	}

	PKBDLLHOOKSTRUCT pStruct = (PKBDLLHOOKSTRUCT)lParam;


	{
		CComCritSecLock<CComAutoCriticalSection> lock(CHotkeyMonitor::m_csForHotkey);

		std::map<int, THotkeyInfo>::iterator it;
		for( it = m_mapHotkey.begin(); it != m_mapHotkey.end(); it++)
		{
			if(it->vk == pStruct->vkCode)
			{
				if(it->fsModifiers & MOD_ALT)
				{
					if(pStruct->flags != 5)
					{
						break;
					}
				}

				if(it->fsModifiers & MOD_CONTROL)
				{
					short state = GetAsyncKeyState(VK_CONTROL);
					if(state & 0x8000 == 0x0000)
					{
						break;
					}

				}

				if(it->fsModifiers & MOD_SHIFT)
				{

					short state = GetAsyncKeyState(VK_SHIFT);
					if(state & 0x8000 == 0x0000)
					{
						break;
					}

					

				}

				if(it->fsModifiers & MOD_WIN)
				{
					short  lWinState = GetAsyncKeyState(VK_LWIN);
					short  rWinState = GetAsyncKeyState(VK_RWIN);

					if(lWinState & 0x8000 == 0x0000 && rWinState & 0x8000 == 0x0000)
					{
						break;
					}
				}


			}




				

			break;

		}


	}




	return CallNextHookEx(NULL,nCode,wParam,lParam);

}

std::queue<int>     CHotkeyMonitor::m_queHotkey;
std::map<int, THotkeyInfo>  CHotkeyMonitor::m_mapHotkey;
HANDLE                  CHotkeyMonitor::m_hMonitorThread = NULL;
HANDLE                  CHotkeyMonitor::m_hHotkeyEvent   = NULL;
CComAutoCriticalSection CHotkeyMonitor::m_csForHotkey;   
