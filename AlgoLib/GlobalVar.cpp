#include "stdafx.h"
#include "headers.h"

//中间文件根目录
CAtlString g_strIntermediatRootDirName = _T(".\\Intermediate");


void ChangeImageDebugSubDir(const TCHAR* szSubDirName)
{

    g_strImageDebugRoot = g_strIntermediatRootDirName + _T("\\");
    g_strImageDebugRoot += szSubDirName;
    
    //生成完整目录目录
	CreateFullDirectory(g_strImageDebugRoot);

}


void SetImageDebugDir(const TCHAR* szDirName)
{
    g_strImageDebugRoot = szDirName;
    //生成完整目录目录
	CreateFullDirectory(g_strImageDebugRoot);

}