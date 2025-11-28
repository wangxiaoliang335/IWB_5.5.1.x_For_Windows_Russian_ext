#pragma once
#include <stdio.h>
#include <fstream>
#include <vector>
#include <deque>
#include <atlbase.h>
#include <assert.h>

#ifndef LOG_FILE_NAME
    #define LOG_FILE_NAME   _T("IWBProxy.log")
#endif


//   PURPOSE: Log a message to the Application event log.
//
//   PARAMETERS:
//   * pszMessage - string message to be logged.
//   * wType - the type of event to be logged. The parameter can be one of 
//     the following values.
//
//     EVENTLOG_SUCCESS
//     EVENTLOG_AUDIT_FAILURE
//     EVENTLOG_AUDIT_SUCCESS
//     EVENTLOG_ERROR_TYPE
//     EVENTLOG_INFORMATION_TYPE
//     EVENTLOG_WARNING_TYPE

inline void _cdecl WriteEventLogEntry(LPCTSTR lpszEventSource, WORD wType, LPCTSTR lpszMessageFormat, ...)
{
	HANDLE hEventSource = NULL;
	LPCWSTR lpszStrings[2] = { NULL, NULL };

	hEventSource = RegisterEventSource(NULL, lpszEventSource);
	if (hEventSource)
	{
		TCHAR buf[1024 * 4];
		va_list vlist;
		va_start(vlist, lpszMessageFormat);

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		_vstprintf_s(buf, _countof(buf), lpszMessageFormat, vlist);

		va_end(vlist);


		lpszStrings[0] = lpszEventSource;

		lpszStrings[1] = buf;

		ReportEvent(hEventSource,  // Event log handle
			wType,                 // Event type
			0,                     // Event category
			0,                     // Event identifier
			NULL,                  // No security identifier
			2,                     // Size of lpszStrings array
			0,                     // No binary data
			lpszStrings,           // Array of strings
			NULL                   // No binary data
		);

		DeregisterEventSource(hEventSource);
	}
}

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



     int GetFreeLogBufIndex()
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

     void RecycleFreeLogBuf(size_t iLogBufIndex)
     {
            CComCritSecLock<CComAutoCriticalSection> lock(m_csForFreeBuf);
            m_queueFreeBuf.push_back(iLogBufIndex);
     }

     //@功能:将新写入的日志缓冲区索引号放入“已写入日志内存块索引"队列中
     //@参数:iLogBufIndex, 日志缓冲区索引号
     void QueueNewLog(size_t iLogBufIndex)
     {
         CComCritSecLock<CComAutoCriticalSection> lock(m_csForWrittenBuf);
         m_queueWrittenBuf.push_back(iLogBufIndex);

         //触发"新日志事件"
         SetEvent(m_hEventNewLog);
     }

     //@功能:提取一个已写入的新日志的内存块索引号
    int FetchWrittenLogBufIndex()
    {
        CComCritSecLock<CComAutoCriticalSection> lock(m_csForWrittenBuf);
        if(m_queueWrittenBuf.empty())
        {
            return -1;
        }
        int nIndex = m_queueWrittenBuf.front();
        m_queueWrittenBuf.pop_front();
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
         SetEvent(m_hThreadExitEvent);
         WaitForSingleObject(m_hLogThread,INFINITE);
         CloseHandle(m_hThreadExitEvent);
         CloseHandle(m_hLogThread);
         CloseHandle(m_hEventNewLog);
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

             m_hEventNewLog   = CreateEvent(NULL, FALSE, FALSE, NULL);
                
             m_hThreadExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

             m_hLogThread = CreateThread(NULL,0,LogThread,this,0,&m_dwThreadId);

             return TRUE;
     }

     static unsigned long _stdcall LogThread(LPVOID lParam)
     {

         CAsyncLog* pLog = (CAsyncLog*)lParam;
         
        HANDLE events[2];
        events[0] = pLog->m_hThreadExitEvent;
        events[1] = pLog->m_hEventNewLog;

         std::string strLogSeparator="=================LOG Start=======================\r\n";
         pLog->m_oLogFile.write(strLogSeparator.c_str(), strLogSeparator.length());

         for(;;)
         {

            DWORD dwWaitResult = WaitForMultipleObjects(_countof(events), &events[0], FALSE, INFINITE);

            if(WAIT_OBJECT_0 == dwWaitResult)
            {//线程退出事件
                 break;
            }
            else if(WAIT_OBJECT_0 + 1 == dwWaitResult)
            {
                int nIndex = -1;
                while((nIndex = pLog->FetchWrittenLogBufIndex()) != -1)
                {

                    TLogBuf& refLogBuf = pLog->GetLogBuf(nIndex);

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

                      //回收Log内存块
                      pLog-> RecycleFreeLogBuf(nIndex);

                }//while
                
                

            }//else if


         }//for

         strLogSeparator="=================LOG End=======================\r\n";
         pLog->m_oLogFile.write(strLogSeparator.c_str(), strLogSeparator.length());

         return 0;
     }

     

protected:
     //日志缓冲区
     std::vector<TLogBuf> m_vecLogBufs;

     //记录空闲空闲日志缓冲区的队列
     std::deque<int> m_queueFreeBuf;


     CComAutoCriticalSection m_csForFreeBuf;
  
     //记录已写入日志的队列
     std::deque<int> m_queueWrittenBuf;

     CComAutoCriticalSection m_csForWrittenBuf;

     //新日志事件
     HANDLE m_hEventNewLog;

     //日志文件
     std::ofstream m_oLogFile;

     //日志线程句柄
     HANDLE m_hLogThread;

     //日志线程ID
     DWORD  m_dwThreadId;

     //
     HANDLE m_hThreadExitEvent;//线程退出通知事件
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
    CAsyncLog_Init()
    {

    }

    ~CAsyncLog_Init()
    {

    }
    static CAsyncLog s_oAsyncLog;
};

_declspec(selectany) CAsyncLog CAsyncLog_Init::s_oAsyncLog;

//技巧:确保任何一个包含Log.h的cpp文件都能初始化 CAsyncLog_Init::s_oAsyncLog.
static CAsyncLog_Init  s_AsyncLog_Init;

 inline void _cdecl AsyncLog(const char* szFormat,...)
 {

     //if(g_pAsyncLog == NULL) return;
     int nBufIndex = s_AsyncLog_Init.s_oAsyncLog.GetFreeLogBufIndex();

     BOOL bRet   = FALSE;
     DWORD dwErr = 0;
     if(nBufIndex >=0 )
     {
         CAsyncLog::TLogBuf& refLogBuf = s_AsyncLog_Init.s_oAsyncLog.GetLogBuf(nBufIndex);

         va_list args;
        va_start(args, szFormat);
        vsnprintf_s( refLogBuf.szText, _countof(refLogBuf.szText) - 1,sizeof(refLogBuf.szText) - 1, szFormat, args); 

        s_AsyncLog_Init.s_oAsyncLog.QueueNewLog(nBufIndex);

        //问题:日志记录如果用消息触发存在的问题是线程还未运行, 消息队列未生成，造成日志丢失
        /*
         bRet = PostThreadMessage(s_AsyncLog_Init.s_oAsyncLog.m_dwThreadId,WM_LOGMSG,(WPARAM)nBufIndex,0);

         if(!bRet)
         {

              dwErr = GetLastError();

         }
         */
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
    int nBufIndex = s_AsyncLog_Init.s_oAsyncLog.GetFreeLogBufIndex();
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



   s_AsyncLog_Init.s_oAsyncLog.QueueNewLog(nBufIndex);

   /*
    bRet = PostThreadMessage(s_AsyncLog_Init.s_oAsyncLog.m_dwThreadId,WM_LOGMSG,(WPARAM)nBufIndex,0);
    if(!bRet)
    {
        dwErr = GetLastError();

    }
    */

    return bRet;
}

 
#ifdef LOG_ENABLED

//#define LOG_INIT(FileName)  AsyncLogInit(FileName)
//#define LOG_UNINIT          AsyncLogUninit

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
#define LOG_DBG(format,...)\
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