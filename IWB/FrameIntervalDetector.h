#pragma once
class CFrameIntervalDetector
{

public:

	CFrameIntervalDetector()
		:
	m_hThread(NULL),
	m_dwThreadId(0),
	m_bIsFirstFrame(TRUE)
	{

		QueryPerformanceFrequency(&m_liFrequency);
	}
	~CFrameIntervalDetector()
	{
		CloseDetector();


	}

	void StartDetector()
	{
		if(!m_hThread)
		{
			m_hThread = ::CreateThread(
				NULL,
				0,
				LogProc,
				(LPVOID)this,
				0,
				&m_dwThreadId);

			m_bIsFirstFrame = TRUE;


		}

		QueryPerformanceCounter(&m_liLastFrameTime);
	}

	void CloseDetector()
	{
		if(m_hThread)
		{


			PostThreadMessage(m_dwThreadId, WM_QUIT, 0, 0);
			WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}

		
	}

	void Trigger()
	{
		if(m_bIsFirstFrame)
		{
			QueryPerformanceCounter(&m_liLastFrameTime);
			m_bIsFirstFrame = FALSE;
		}
		else
		{
			LARGE_INTEGER  liNow;
			QueryPerformanceCounter(&liNow);
			LONG lInterval =  (LONG)((liNow.QuadPart - m_liLastFrameTime.QuadPart)*1000*1000/m_liFrequency.QuadPart);

			m_liLastFrameTime.QuadPart = liNow.QuadPart;
			PostThreadMessage(m_dwThreadId, WM_LOG_NOTIFY_MESSAGE, 0, (LPARAM)lInterval);
		}
	}

	static ULONG __stdcall LogProc(LPVOID lpCtx)
	{
		MSG msg;
		const char* szLogFileName = "FrameInterval.csv";
		FILE* logFile = NULL;
		fopen_s(&logFile,szLogFileName, "w+");
		char szBuf[MAX_PATH];
		memset(szBuf, 0 ,sizeof(szBuf));
		while(::GetMessage(&msg, NULL, 0, 0 ))
		{
			switch(msg.message)
			{

			case WM_LOG_NOTIFY_MESSAGE:
				sprintf_s(szBuf, _countof(szBuf), "%d,", msg.lParam);
				fwrite(szBuf, 1, strlen(szBuf), logFile);
				fflush(logFile);
				
				break;

			default:
				break;
			}//switch

		}//while

		fclose(logFile);

		return 0;
	}
protected:
	HANDLE m_hThread;
	DWORD m_dwThreadId;

	LARGE_INTEGER m_liFrequency         ;//
	LARGE_INTEGER m_liLastFrameTime     ;//上一次统计输出时刻
	BOOL          m_bIsFirstFrame       ;//第一帧
	static const int WM_LOG_NOTIFY_MESSAGE = WM_USER + 1;



};