//	RtlGetVersion APIを使用してWindowsのバージョン番号を取得する
//	RtlGetVersion APIをサポートしていない場合は、GetVersionEx APIを使用する
//	Visual C++ 2005/2008/2013 Unicode/マルチバイト

#ifndef RtlGetVersion_Header
#define RtlGetVersion_Header

//	DLL内の関数へのポインタ型を定義
typedef void (WINAPI *RtlGetVersion_FUNC)(OSVERSIONINFOEXW*);

//BOOL GetVersion2(OSVERSIONINFOEX* os);

/*
int WINAPI _tWinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPTSTR lpsCmdLine, int nCmdShow){
	TCHAR buf[256];
	OSVERSIONINFOEX os;
	if (GetVersion2(&os) == TRUE){

		_stprintf_s(buf, sizeof(buf) / sizeof(TCHAR), _TEXT("MajorVersion %d\nMinorVersion %d\nBuild %d\n"),
			os.dwMajorVersion, os.dwMinorVersion, os.dwBuildNumber);

		MessageBox(0, buf, _TEXT("Windows Version"), MB_OK);
	}
	else{
		MessageBox(0, _TEXT("RtlGetVersion APIがサポートされていません"), _TEXT("Error"), MB_OK);
	}
	return (int)0;
}
*/
inline BOOL RtlGetVersionWrapper(OSVERSIONINFOEX* os){
	HMODULE hMod;
	RtlGetVersion_FUNC func;
#ifdef UNICODE
	OSVERSIONINFOEXW* osw = os;
#else
	OSVERSIONINFOEXW o;
	OSVERSIONINFOEXW* osw = &o;
#endif

	hMod = LoadLibrary(TEXT("ntdll.dll"));
	if (hMod){
		func = (RtlGetVersion_FUNC)GetProcAddress(hMod, "RtlGetVersion");
		if (func == 0){
			FreeLibrary(hMod);
			return FALSE;
		}
		ZeroMemory(osw, sizeof(*osw));
		osw->dwOSVersionInfoSize = sizeof(*osw);
		func(osw);
#ifndef	UNICODE
		os->dwBuildNumber = osw->dwBuildNumber;
		os->dwMajorVersion = osw->dwMajorVersion;
		os->dwMinorVersion = osw->dwMinorVersion;
		os->dwPlatformId = osw->dwPlatformId;
		os->dwOSVersionInfoSize = sizeof(*os);
		DWORD sz = sizeof(os->szCSDVersion);
		WCHAR* src = osw->szCSDVersion;
		unsigned char* dtc = (unsigned char*)os->szCSDVersion;
		while (*src)
			*dtc++ = (unsigned char)*src++;
		*dtc = '\0';
#endif

	}
	else
		return FALSE;
	FreeLibrary(hMod);
	return TRUE;
}

#endif