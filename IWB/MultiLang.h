#pragma once

//@功能:根据Locale ID获取不同的资源库
//inline HINSTANCE LoadResDll(HINSTANCE hAppInst, LCID lcid)
//@参数:hAppInst, 模块实例句柄
//      strExpectedLanguageCode, 输入/输出参数, 期望的语言
inline HINSTANCE LoadResDll(HINSTANCE hAppInst, CString& strExpectedLanguageCode)
{
    //=================Prepare the dll path format string ==========
    TCHAR szFormat[MAX_PATH];
    LPTSTR pszFileName;

    int ret = ::GetModuleFileName(hAppInst, szFormat, MAX_PATH);
    if(ret == 0 || ret == MAX_PATH)
    {
        ASSERT(FALSE);
        return NULL;
    }
    pszFileName = :: PathFindFileName(szFormat);


    TCHAR szExt[] = _T("%s_Res.dll");
    if((pszFileName - szFormat + sizeof(szExt)) > MAX_PATH)
    {
        ASSERT(FALSE);
        return NULL;
    }

    lstrcpy(pszFileName, szExt);


    //===========载入资源库===============================================================
    TCHAR szLangDLL[_MAX_PATH+14];
    TCHAR szLangCode[4];

    //if (lcid == LOCALE_SYSTEM_DEFAULT)
    //{
    //    lstrcpy(szLangCode, _T("LOC"));
    //}
    //else
    if(strExpectedLanguageCode.IsEmpty())//语言代码为空，
    {
        int nResult;

        //LCID lcid = GetSystemDefaultLCID();

        //LANGID langId = GetSystemDefaultUILanguage();
        LANGID langId = GetUserDefaultUILanguage();
        WORD srtId    = SORT_DEFAULT;
        LCID   lcid   = MAKELCID(langId, srtId);

        nResult = ::GetLocaleInfo(lcid, LOCALE_SABBREVLANGNAME, szLangCode, _countof(szLangCode));
        if (nResult == 0)
        {
            DWORD dwError = GetLastError();
            switch(dwError)
            {
            case ERROR_INSUFFICIENT_BUFFER:
                AtlTrace(_T("GetLocaleInfo ERROR_INSUFFICIENT_BUFFER\r\n"));
                break;

            case ERROR_INVALID_FLAGS :
                AtlTrace(_T("GetLocaleInfo ERROR_INVALID_FLAGS\r\n"));
                break;

            case ERROR_INVALID_PARAMETER:
                AtlTrace(_T("GetLocaleInfo ERROR_INVALID_PARAMETER\r\n"));
                break;
            }//switch
            return NULL;
        }

        strExpectedLanguageCode =szLangCode;

        ASSERT( nResult == 4 );
    }
     ret = _sntprintf_s(szLangDLL, _countof(szLangDLL), _countof(szLangDLL) - 1, szFormat,(LPCTSTR)strExpectedLanguageCode);
    if(ret == -1 || ret >= sizeof(szLangDLL))
    {
        ASSERT(FALSE);
        return NULL;
    }

    HINSTANCE hInstance = ::LoadLibrary(szLangDLL);

    return hInstance;
}

