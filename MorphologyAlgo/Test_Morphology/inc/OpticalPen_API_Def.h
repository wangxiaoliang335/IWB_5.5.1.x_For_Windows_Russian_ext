#pragma once
typedef struct _TOpticalPenNotifySink
{
	HWND  hNotifySinkWindow;
	DWORD dwThreadId;
}TOpticalPenNotifySink;

#define MAX_NOTIFY_SINK_NUMBER 1024

#define OPTICALPEN_SHARED_MEMORY_NAME	_T("OpticalVirtualPen_3F3311C0-0ABF-495a-8390-2B6A4E34AAB7")
#define OPTICALPEN_SHARED_MUTEXT_NAME	_T("OpticalVirtualPen_3F3311C0-0ABF-495a-8390-2B6A4E34AAB7_Mutex")

typedef struct _TOpticalPenSharedMemoryHeader
{
	DWORD dwTotalSize;
	int   nSinkCount;
	TOpticalPenNotifySink aryPenNotifySinks[1];
}TOpticalPenSharedMemoryHeader;

#define MAX_WATI_TIME (10*1000)


class COpticalPenSharedMemory
{
public:
		COpticalPenSharedMemory()
			:
		m_hShareMem(NULL),
		m_lpMapAddress(NULL)

		{

			m_hMutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, OPTICALPEN_SHARED_MUTEXT_NAME);

			if(m_hMutex == NULL)
			{

				m_hMutex = CreateMutex(NULL, FALSE, OPTICALPEN_SHARED_MUTEXT_NAME);
			}



			m_hShareMem = OpenSharedMemory(&m_lpMapAddress);
			if(NULL == m_hShareMem)
			{
				m_hShareMem = CreateSharedMemory(&m_lpMapAddress);
			}

		}



		~COpticalPenSharedMemory()
		{
			DWORD dwWaitResult = WaitForSingleObject(m_hMutex,MAX_WATI_TIME);

			CloseSharedMemory(m_hShareMem, m_lpMapAddress);

			ReleaseMutex(m_hMutex);
			CloseHandle(m_hMutex);
		}


		DWORD CopyNotifySink(LPVOID lpBuffer, DWORD dwSize)
		{
			DWORD dwCpySize  = 0;
			if(m_hShareMem == NULL || m_lpMapAddress == NULL || m_hMutex == NULL)  return dwCpySize;
			 WaitForSingleObject(m_hMutex, MAX_WATI_TIME);
			int nCount = dwSize / sizeof(TOpticalPenNotifySink) ;

			TOpticalPenSharedMemoryHeader* pHeader = reinterpret_cast<TOpticalPenSharedMemoryHeader*>(m_lpMapAddress);

			nCount = nCount > pHeader->nSinkCount ? pHeader->nSinkCount:nCount;
			dwCpySize = nCount * sizeof(TOpticalPenNotifySink);
			memcpy(lpBuffer, &pHeader->aryPenNotifySinks, dwCpySize);
			ReleaseMutex(m_hMutex);

			return dwCpySize;
		}


		BOOL InsertNotifySink(HWND hWnd)
		{
			if(m_hShareMem == NULL || m_lpMapAddress == NULL || m_hMutex == NULL)  return FALSE;
			 WaitForSingleObject(m_hMutex, MAX_WATI_TIME);
			TOpticalPenSharedMemoryHeader* pHeader = reinterpret_cast<TOpticalPenSharedMemoryHeader*>(m_lpMapAddress);

			if(pHeader->nSinkCount == MAX_NOTIFY_SINK_NUMBER)
			{
				ReleaseMutex(m_hMutex);
				return FALSE;
			}

			DWORD dwThreadId = GetCurrentThreadId();

			pHeader->aryPenNotifySinks[pHeader->nSinkCount].dwThreadId = dwThreadId;
			pHeader->aryPenNotifySinks[pHeader->nSinkCount].hNotifySinkWindow = hWnd;

			pHeader->nSinkCount++;
			 ReleaseMutex(m_hMutex);

			 return TRUE;
		}


		BOOL RemoveNotifySink(HWND hWnd)
		{
		
			if(m_hShareMem == NULL || m_lpMapAddress == NULL || m_hMutex == NULL)  return FALSE;
			 WaitForSingleObject(m_hMutex, MAX_WATI_TIME);
			TOpticalPenSharedMemoryHeader* pHeader = reinterpret_cast<TOpticalPenSharedMemoryHeader*>(m_lpMapAddress);


			DWORD dwThreadId = GetCurrentThreadId();

			int sinkIndex  = 0;
			for(sinkIndex = 0; sinkIndex < pHeader->nSinkCount; sinkIndex++)
			{
				TOpticalPenNotifySink sink = pHeader->aryPenNotifySinks[sinkIndex];
				if(hWnd != NULL)
				{
					if(sink.hNotifySinkWindow == hWnd)
					{
						break;
					}
				}

				else if(sink.dwThreadId == dwThreadId)
				{

					break;

				}
			}//for(i)

			if(sinkIndex != pHeader->nSinkCount)
			{
				for(int j=sinkIndex; j< pHeader->nSinkCount - 1; j++)
				{
					pHeader->aryPenNotifySinks[j] =  pHeader->aryPenNotifySinks[j + 1];
				}
			}


			pHeader->nSinkCount--;
			 ReleaseMutex(m_hMutex);

			 return TRUE;

		}
private:
	HANDLE m_hShareMem;
	LPVOID m_lpMapAddress;
	HANDLE m_hMutex;
	inline HANDLE OpenSharedMemory(LPVOID* lppMapAddress)
	{

		HANDLE hFileMapping = OpenFileMapping(FILE_MAP_WRITE | FILE_MAP_READ,TRUE, OPTICALPEN_SHARED_MEMORY_NAME);

		if(hFileMapping == NULL)
		{
			return hFileMapping;
		}

		LPVOID  lpSharedMemoryStartAddress = 
			MapViewOfFile(
			hFileMapping,
			FILE_MAP_WRITE | FILE_MAP_READ,
			0,
			0,
			0);

		if(lppMapAddress)
		{
			*lppMapAddress = lpSharedMemoryStartAddress;
		}

		return hFileMapping;
	}

	inline HANDLE  CreateSharedMemory(LPVOID* lppMapAddress)
	{
		DWORD dwError = 0;
		HANDLE hFileMapping = NULL;
		hFileMapping = 
			CreateFileMapping(
			NULL,
			NULL,
			PAGE_READWRITE,
			0,//Maximum Size High
			sizeof(TOpticalPenSharedMemoryHeader) + MAX_NOTIFY_SINK_NUMBER * sizeof(TOpticalPenNotifySink),//Maximum Size, Low
			OPTICALPEN_SHARED_MEMORY_NAME);

		if(hFileMapping == NULL)
		{
			return hFileMapping;
		}

		LPVOID lpSharedMemoryStartAddress = 
			MapViewOfFile(
			hFileMapping,
			FILE_MAP_WRITE | FILE_MAP_READ,
			0,
			0,
			0);


		TOpticalPenSharedMemoryHeader* pHeader = (TOpticalPenSharedMemoryHeader*)lpSharedMemoryStartAddress;
		pHeader->dwTotalSize = sizeof(TOpticalPenSharedMemoryHeader) + MAX_NOTIFY_SINK_NUMBER * sizeof(TOpticalPenNotifySink);
		pHeader->nSinkCount  = 0;

		if(lppMapAddress)
		{
			*lppMapAddress = lpSharedMemoryStartAddress;
		}
		return hFileMapping;
	}

	inline  BOOL  CloseSharedMemory(HANDLE hFileMapping, LPVOID fileMappingAddress)
	{
		if(fileMappingAddress)
		{
			UnmapViewOfFile(fileMappingAddress);
		}
		
		CloseHandle(hFileMapping);

		return TRUE;
	}

};
