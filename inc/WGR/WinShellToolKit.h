#pragma once

/*
  使用者使用前需要通过CoInitialize来初始化com环境，
  如果需要，使用后需要通过CoUninitialize来反初始化com环境
*/

//////////////////////////////////////////////////////////////////////////
//宏定义
typedef enum
{
    ESCRLT_UP   , //竖直向上
    ESCRLT_DOWN , //竖直向下
    ESCRLT_LEFT , //水平向左
    ESCRLT_RIGHT, //水平向右
}E_ScroolType;

class CWinShellInstances;

class CWinShellUtility
{
public:
    CWinShellUtility();
    ~CWinShellUtility();

public:
    //并列显示窗口
    bool ShowWindowsSidebySide();
    //最小化全部窗口
    bool MinimizeAllWindows();
    bool UndoMinizeAllWindows();
    //窗口切换
    bool SwitchWindows(char *shortCut);

    //水平滚动最前端窗口
    bool HScroolForegroundWindow(E_ScroolType scollDir);

    //竖直滚动最前端窗口
    bool VScroolForegroundWindow(E_ScroolType scollDir);

    //最小化当前窗口
    bool MiniCurrentWindow(const POINT &ptCur);

    //最大化当前窗口
    bool MaxiCurrentWindow(const POINT &ptCur);

    //显示桌面
    bool DisplayDesktop(const POINT &ptCur);
   
private:
    //获取最顶层的祖先窗口
    HWND GetTopParent(const POINT &ptCur);

private:
    CWinShellInstances *m_shell_inst;
};