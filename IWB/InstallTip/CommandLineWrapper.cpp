#include "CommandLineWrapper.h"

//////////////////////////////////////////////////////////////////////////
//命令行参数格式相关
const TCHAR *ITEMPREFIX = _T("-@");
const TCHAR *ITEMSUFFIX = _T("@-");

const TCHAR *LANGCODEITEM_TAG       = _T("LG:");
const TCHAR *SERVERWNDITTEM_TAG     = _T("HW:");
const TCHAR *SCREENMODEITEM_TAG     = _T("SM:");
//
//////////////////////////////////////////////////////////////////////////

CCommandLineWrapper::CCommandLineWrapper(void)
:m_hadUsed(0)
{
    memset(m_commandLine, 0, sizeof(m_commandLine));
}

CCommandLineWrapper::~CCommandLineWrapper(void)
{
}

bool CCommandLineWrapper::WrapLangCode(const TCHAR *langCode)
{
    WrapString(langCode, LANGCODEITEM_TAG);
    return true;
}

bool CCommandLineWrapper::WrapServerWnd(const DWORD &serverWnd)
{
    if (serverWnd == 0) return false;
    
    TCHAR strWord[16];
    memset(strWord, 0, sizeof(strWord));

    _stprintf_s(strWord, _countof(strWord), _T("%d"), serverWnd);

    WrapString(strWord, SERVERWNDITTEM_TAG);
    return true;
}

bool CCommandLineWrapper::WrapScreenMode(const byte &screenMode)
{
    TCHAR strScreenMode[4];
    memset(strScreenMode, 0, sizeof(strScreenMode));

    _stprintf_s(strScreenMode, _countof(strScreenMode), _T("%d"), screenMode);

    WrapString(strScreenMode, SCREENMODEITEM_TAG);
    return true;
}

bool CCommandLineWrapper::WrapString(const TCHAR *str, const TCHAR *tag)
{
    if(str == NULL || tag == NULL) return false;

    unsigned uLangCodeLen = _tcslen(str);
    unsigned uNeedSize = uLangCodeLen + _tcslen(ITEMPREFIX) + _tcslen(ITEMSUFFIX) + _tcslen(tag);
    unsigned uBuffRemained = KMAXBUFFSIZE - m_hadUsed;

    if (uNeedSize >= uBuffRemained) return false; //空间不足 

    _stprintf_s(m_commandLine, KMAXBUFFSIZE, _T("%s%s%s%s%s"), m_commandLine, ITEMPREFIX, tag, str, ITEMSUFFIX);

    m_hadUsed += uNeedSize;
    return true;
}
