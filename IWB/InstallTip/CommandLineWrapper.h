#pragma once

#include <tchar.h>
#include <Windows.h>

class CCommandLineWrapper
{
public:
    CCommandLineWrapper(void);
    ~CCommandLineWrapper(void);

public:
    TCHAR *GetWrappedCommandLine() {return m_commandLine;};

public:
    bool WrapLangCode(const TCHAR *langCode);
    bool WrapServerWnd(const DWORD &serverWnd);
    /*
    @功能：封装值为screenMode的屏幕值到CommandLine
    @参数：screenMode，屏幕类型——0代表单屏，1代表双屏
    */
    bool WrapScreenMode(const byte &screenMode);

protected:
    bool WrapString(const TCHAR *str, const TCHAR *tag);

protected:
    static const unsigned  KMAXBUFFSIZE = 256;
    TCHAR           m_commandLine[KMAXBUFFSIZE];

    unsigned        m_hadUsed;  //为CommandLine提供的Buff，已经被占用的大小
};
