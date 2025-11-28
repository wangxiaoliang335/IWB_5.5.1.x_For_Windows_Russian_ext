#pragma once

#include "WGRPermissionPublic.h"

class CWGRPermissionParse
{
private:
    CWGRPermissionParse(void);
    ~CWGRPermissionParse(void);

public:
    //////////////////////////////////////////////////////////////////////////
    //由许可值集获特定功能的许可性
    static bool IsTheEventPermited(E_TRIGGEREVENT evt, const DWORD &permissionSet);

    //////////////////////////////////////////////////////////////////////////
    //由特定功能的许可性转化为许可值集
    //参数：permissionSet[in, out]，[in]当前的permissionSet值，[out]更改evt所对应的许可性后的permissionSet值
    static void ConvertToPermissionSet(E_TRIGGEREVENT evt, bool isPermited, DWORD &permissionSet);
};
