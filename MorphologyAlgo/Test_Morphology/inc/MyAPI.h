#pragma once
#include <atlstr.H>
inline BOOL CreateFullDirectory(LPCTSTR lpszPath)
{

	TCHAR path[MAX_PATH];
	memset(path, 0 ,sizeof(path));

	const TCHAR* pData = lpszPath;
	int l = 0;


	do
	{
		if(*pData == _T('\\') || *pData ==_T('/') || *pData == _T('\0'))
		{
			if(l!=0)
			{
				BOOL bRet = PathIsRoot(path);
				if(!bRet)
				{
					bRet = PathFileExists(path);

					if(!bRet)
					{
						bRet = CreateDirectory(path, NULL);
						if(!bRet)
						{
							DWORD dwError = GetLastError();

							if(dwError != ERROR_ALREADY_EXISTS)
							{
								return FALSE;
							}
						
						}
					}
				}


			}//if(l)
	   }//if found path sepearator symbol

		if(*pData == _T('\0'))
		{
			break;
		}
		path[l++] = *pData++;
	

	}while(1);

	return TRUE;
}





inline CAtlString CheckAndCreatePath(LPCTSTR szPath)
{	
	CAtlString strAbsPath;

	TCHAR absPathBuf[MAX_PATH];

	if(PathIsRelative(szPath))
	{
		GetCurrentDirectory(_countof(absPathBuf), absPathBuf);

		_tcscat_s(absPathBuf, _countof(absPathBuf), _T("\\"));
		_tcscat_s(absPathBuf, _countof(absPathBuf), szPath);

		strAbsPath = absPathBuf;
	}
	else
	{

		strAbsPath = szPath;
	}

	PathCanonicalize(absPathBuf, strAbsPath);

	strAbsPath = absPathBuf;


	PathRemoveFileSpec(absPathBuf);

	CreateFullDirectory(absPathBuf);

	return strAbsPath;
}


//