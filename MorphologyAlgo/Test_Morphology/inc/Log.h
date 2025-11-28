#pragma once
#include <stdio.h>
#include <fstream>
#include <vector>
#include <deque>
#include <atlbase.h>
#include <assert.h>

 inline void _cdecl MyDebug(const char* szFormat,...)
{

	char buf[1024];
	va_list vlist;
	va_start(vlist,szFormat);

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	
	
	vsprintf_s(buf, _countof(buf), szFormat,vlist);
	
	OutputDebugStringA(buf);
	

	va_end(vlist);
}


#define WM_LOGMSG (WM_USER+119)

 class CAsyncLog
 {

 public:
	 struct TLogBuf
	 {
		char szText[1024];//		
	 };
	 std::deque<int> m_queueFreeBuf;
	 CComAutoCriticalSection m_csForFreeBuf;

	 std::vector<TLogBuf> m_vecLogBufs;
	 


	 int GetLogBufIndex()
	 {
		 CComCritSecLock<CComAutoCriticalSection> lock(m_csForFreeBuf);
		 if(m_queueFreeBuf.empty())
		 {
			 return -1;
		 }
		int nIndex =  m_queueFreeBuf.front();		
		m_queueFreeBuf.pop_front();
		return nIndex;



	 }

	 TLogBuf& GetLogBuf(size_t iIndex)
	 {
		assert(iIndex < m_vecLogBufs.size());

		return m_vecLogBufs[iIndex];
	 }



	 CAsyncLog(const TCHAR* szLogFileName)
	 {
		Init(szLogFileName);

	 }

	 CAsyncLog()
	 {
#ifdef LOG_FILE_NAME
		Init(LOG_FILE_NAME);
#else
		Init(_T("RunTime.log"));
#endif
	 }




	 ~CAsyncLog()
	 {
		 PostThreadMessage(m_dwThreadId,WM_QUIT,NULL,NULL);
		 WaitForSingleObject(m_hLogThread,INFINITE);
		 CloseHandle(m_hThreadRunEvent);
		 CloseHandle(m_hLogThread);		 
		 m_oLogFile.close();

	 }

 protected:

	 BOOL Init(const TCHAR* szLogFileName)
	 {
			 m_oLogFile.open(szLogFileName,std::ios_base::app | std::ios_base::out,_SH_DENYNO );
			 if(m_oLogFile.fail())
			 {

				 return FALSE;
			 }
			 

			 int nLogBufNum = 100;
			 m_vecLogBufs.resize(nLogBufNum);

			 for(int i=0; i < nLogBufNum; i++)
			 {
				m_queueFreeBuf.push_back(i);
			 }
				
			 m_hThreadRunEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	 		 m_hLogThread = CreateThread(NULL,0,LogThread,this,0,&m_dwThreadId);

			 DWORD dwWaitResult = WAIT_OBJECT_0;

			 if(m_hLogThread)
			 {
				 //确保线程运行
				 dwWaitResult = WaitForSingleObject(m_hThreadRunEvent, INFINITE);
				 
			 }

			 return TRUE;
	 }

	 static unsigned long _stdcall LogThread(LPVOID lParam)
	 {

		 CAsyncLog* pLog = (CAsyncLog*)lParam;
		 
		 SetEvent(pLog->m_hThreadRunEvent);

		 std::string strLogSeparator="=================LOG Start=======================\r\n";
		 pLog->m_oLogFile.write(strLogSeparator.c_str(), strLogSeparator.length());

		 MSG msg;

		 while(GetMessage(&msg,NULL,0,0))
		 {
			 

			 if(msg.message == WM_LOGMSG)
			 {
				 TLogBuf& refLogBuf = pLog->GetLogBuf((int)msg.wParam);

				  SYSTEMTIME sysTime;
				  GetLocalTime(&sysTime);

				  char szTime[64];
				  _snprintf_s(szTime,sizeof(szTime),
					  "[%04d/%02d/%02d %02d:%02d:%02d:%03d]",
					  sysTime.wYear,
					  sysTime.wMonth,
					  sysTime.wDay,
					  sysTime.wHour,
					  sysTime.wMinute,
					  sysTime.wSecond,
					  sysTime.wMilliseconds);


				  pLog-> m_oLogFile.write(szTime,strlen(szTime));
				  pLog-> m_oLogFile.write(refLogBuf.szText,strlen(refLogBuf.szText) );
				  pLog-> m_oLogFile.flush();


				  {
					  CComCritSecLock<CComAutoCriticalSection> lock(pLog->m_csForFreeBuf);
					  pLog->m_queueFreeBuf.push_back((int)msg.wParam);			 
				  }


			 }

		 };//while
		 return 0;
	 }

	 
 public:
	 
	 std::ofstream m_oLogFile;
	 HANDLE m_hLogThread;
	 DWORD  m_dwThreadId;
	 HANDLE m_hThreadRunEvent;//线程正在运行事件
 };


//_declspec(selectany)
//CAsyncLog*  g_pAsyncLog = NULL;
//
//inline void _cdecl  AsyncLogInit(const TCHAR * szLogFileName)
//{
//    assert(g_pAsyncLog == NULL);
//    g_pAsyncLog = new CAsyncLog(szLogFileName);
//    
//}
//
//inline void _cdecl  AsyncLogUninit()
//{
//    if(g_pAsyncLog != NULL)
//    {
//        delete g_pAsyncLog;
//        g_pAsyncLog = NULL;
//    }
//
//    
//}

class CAsyncLog_Init
{
public:
	static CAsyncLog s_oAsyncLog;
};

_declspec(selectany) CAsyncLog CAsyncLog_Init::s_oAsyncLog;

//技巧:确保任何一个包含Log.h的cpp文件都能初始化 CAsyncLog_Init::s_oAsyncLog.
//
static CAsyncLog_Init  s_AsyncLog_Init;




 inline void _cdecl AsyncLog(const char* szFormat,...)
 {

     //if(g_pAsyncLog == NULL) return;
	 int nBufIndex = s_AsyncLog_Init.s_oAsyncLog.GetLogBufIndex();

	 BOOL bRet   = FALSE;
	 DWORD dwErr = 0;
	 if(nBufIndex >=0 )
	 {
		 CAsyncLog::TLogBuf& refLogBuf = s_AsyncLog_Init.s_oAsyncLog.GetLogBuf(nBufIndex);

		 va_list args;
		va_start(args, szFormat);
		vsnprintf_s( refLogBuf.szText, _countof(refLogBuf.szText) - 1,sizeof(refLogBuf.szText) - 1, szFormat, args); 	


		 bRet = PostThreadMessage(s_AsyncLog_Init.s_oAsyncLog.m_dwThreadId,WM_LOGMSG,(WPARAM)nBufIndex,0);

		 if(!bRet)
		 {

			  dwErr = GetLastError();

		 }

	 }

 };


#define LOG_TYPE_INF  0
#define LOG_TYPE_WRN  1
#define LOG_TYPE_ERR  2
#define LOG_TYPE_DBG  3
inline BOOL _cdecl AsyncTypedLog(int nType, const char* szFormat,...)
{
	BOOL   bRet  = FALSE;
	 DWORD dwErr = 0;
	// if(g_pAsyncLog == NULL) return bRet;
	int nBufIndex = s_AsyncLog_Init.s_oAsyncLog.GetLogBufIndex();
	if(nBufIndex < 0 ) return bRet;

	CAsyncLog::TLogBuf& refLogBuf = s_AsyncLog_Init.s_oAsyncLog.GetLogBuf(nBufIndex);
	
	switch(nType)
	{
	case LOG_TYPE_INF://正常信息
		strcpy_s(refLogBuf.szText, _countof(refLogBuf.szText), "[INF]");
		break;

	case LOG_TYPE_WRN://警告信息
		strcpy_s(refLogBuf.szText,_countof(refLogBuf.szText), "[WRN]");
		break;

	case LOG_TYPE_ERR://错误信息
		strcpy_s(refLogBuf.szText,_countof(refLogBuf.szText), "[ERR]");
		break;

	case LOG_TYPE_DBG:
		strcpy_s(refLogBuf.szText, _countof(refLogBuf.szText),"[DBG]");
		break;

	default:
		strcpy_s(refLogBuf.szText, _countof(refLogBuf.szText),"[UNK]");
		break;
	}
	va_list args;
	va_start(args, szFormat);
	
	int nOffset = strlen(refLogBuf.szText);
	vsnprintf_s( &refLogBuf.szText[nOffset], _countof(refLogBuf.szText) - nOffset - 1 ,sizeof(refLogBuf.szText) - nOffset - 1, szFormat, args);

	strcat_s(refLogBuf.szText, _countof(refLogBuf.szText), "\r\n");
	
	bRet = PostThreadMessage(s_AsyncLog_Init.s_oAsyncLog.m_dwThreadId,WM_LOGMSG,(WPARAM)nBufIndex,0);
	if(!bRet)
	{
		dwErr = GetLastError();

	}

	return bRet;
}

 
#ifdef LOG_ENABLED

#define LOG_INIT(FileName)  AsyncLogInit(FileName)
#define LOG_UNINIT          AsyncLogUninit

//#define LOG(Text)           AsyncLog Text

//正常信息
#define LOG_INF(format, ... )\
		AsyncTypedLog(LOG_TYPE_INF, format, __VA_ARGS__ )

//警告信息
#define LOG_WRN(format, ... )\
		AsyncTypedLog(LOG_TYPE_WRN, format, __VA_ARGS__ )

//错误信息
#define LOG_ERR(format, ... )\
		AsyncTypedLog(LOG_TYPE_ERR, format, __VA_ARGS__ )

//调试信息
#define LOG(format,...)\
		AsyncTypedLog(LOG_TYPE_DBG, format, __VA_ARGS__)

#else if

#define LOG_INIT(FileName) 
#define LOG_UNINIT 

//#define LOG(Text)

//正常信息
#define LOG_INF(format, ... ) 

//警告信息
#define LOG_WRN(format, ... ) 

//错误信息
#define LOG_ERR(format, ... ) 



//调试信息
#define LOG(format,...) 


#endif