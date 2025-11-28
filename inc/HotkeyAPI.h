#pragma once

#ifdef HOTKEYMONITOR_EXPORTS
#define HOTKEY_API _declspec(dllexport)
#else 
#define HOTKEY_API _declspec(dllimport)
#endif

//@功能:注册一个热键, 当系统中检测到按键按下, 指定的窗口会接收到WM_HOTKEY消息
//@参数:hWnd, 接收WM_HOTKEY消息的窗体，如果hWnd == NULL, 则WM_HOTKEY则送给调用AddHotkey的线程的消息队列中。
//      id, 热键的唯一标识符，为了避免冲突，使用GlobalAddAtom函数从字符串生成这个id
//      fsModifiers, 可以是下列值得组合
//                 MOD_ALT Either ALT key must be held down. 
//                 MOD_CONTROL Either CTRL key must be held down. 
//                 MOD_KEYUP Both key up events and key down events generate a WM_HOTKEY message 
//                 MOD_SHIFT Either SHIFT key must be held down. 
//                 MOD_WIN Either WINDOWS key was held down. These keys are labeled with the Microsoft Windows logo. 
//     vk,虚拟键值
extern "C"  HOTKEY_API BOOL AddHotKey(HWND hWnd, int id, UINT fsModifiers, UINT vk);

//@功能:移除一个热键,
//@参数:id, 热键的标识符
extern "C"  HOTKEY_API BOOL RemoveHotKey(int id);

//@功能:启动热键监控
extern "C" HOTKEY_API BOOL StartHotKeyMonitor();


//@功能:停止热键监控
extern "C" HOTKEY_API BOOL StopHotKeyMonitor();