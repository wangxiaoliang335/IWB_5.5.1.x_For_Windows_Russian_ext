#pragma once

//中间文件根目录
extern "C" CAtlString g_strIntermediatRootDirName;

//功能:设置图片调试输出的子目录
void ChangeImageDebugSubDir(const TCHAR* szSubDirName);

void SetImageDebugDir(const TCHAR* szDirName);