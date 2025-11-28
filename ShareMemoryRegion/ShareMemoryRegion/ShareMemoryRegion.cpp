// ShareMemoryRegion.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "../inc/ShareMemoryRegion.h"
#include <assert.h>
#include <memory>
#include <atlstr.h>


#define  SHARED_MEMORY_NAME  _T("ControlTboardButtonState_A85967F8-DE36-4a58-9964-51FB0A690C01")

HANDLE g_hShareMemory = NULL;
LPVOID g_lpShareMemoryStartAddress = NULL;

HANDLE g_hSharedMemoryMutex = NULL;
#define SHARED_MEMORY_MUTEX_NAME _T("ControlTboardButtonStateMutex_CC5562DF-DC4D-46c0-AC02-9CC11CD3E37B")

HANDLE g_hShareMemoryEvent = NULL;
#define  HARED_MEMROY_NEW_TBOARDBUTTON_EVENT  _T("ControlTboardButtonStateEvent_B2FB504E-26FC-4207-A080-CA63C5570558")


static const int SHARE_MEMORY_SIZE = 1024 - 50;    //定义共享内存的大小
static const int SHARE_MEMORY_PROGRAME_BLOCK_NUM_SIZE = 4;        //定义数据块的个数的内存大小

static const int SHARE_MEMORY_PROGRAME_NAME_SIZE = 26;            //定义共享内存中存储程序名字的内存大小
static const int SHARE_MEMORY_PROGRAME_VERSION_SIZE  =10;         //定义共享内存中存储程序版本的内存大小
static const int SHARE_MEMORY_PROGRAME_LEN_SIZE     =10;          //定义共享内存中存储写入数据长度的内存大小

static const int SHARE_MEMORY_PROGRAME_BLOCK_SIZE    = SHARE_MEMORY_PROGRAME_NAME_SIZE +SHARE_MEMORY_PROGRAME_VERSION_SIZE+ SHARE_MEMORY_PROGRAME_LEN_SIZE;

extern "C"
{

    //@功  能: 打开共享内存
	//@参  数:
	//
	//
	//@返回值: TURE为打开共享内存成功，FALSE为打开共享内存失败。

	BOOL OpenShareMemory()      
	{
		g_hShareMemory = OpenFileMapping(FILE_MAP_WRITE | FILE_MAP_READ,TRUE, SHARED_MEMORY_NAME);

		if (g_hShareMemory == NULL)
		{
			return FALSE;
		}

		g_lpShareMemoryStartAddress =
			MapViewOfFile(
			g_hShareMemory,
			FILE_MAP_WRITE | FILE_MAP_READ,
			0,
			0,
			0);

		if (g_lpShareMemoryStartAddress == NULL)
		{
			CloseHandle(g_hShareMemory);
			g_hShareMemory =NULL;

			return FALSE;
		}

		g_hSharedMemoryMutex = OpenMutex(MUTEX_ALL_ACCESS, TRUE, SHARED_MEMORY_MUTEX_NAME);
		g_hShareMemoryEvent = OpenEvent(EVENT_ALL_ACCESS, TRUE,HARED_MEMROY_NEW_TBOARDBUTTON_EVENT);

		return TRUE;

	}

	//@功  能:程序开始时初始化各个参数。
	//@参  数:lReserved为保留值，0是不做任何处理
	//
	//
	//@返回值: 返回布尔类型，TURE是初始化成功。FALSE为初始化失败

    BOOL SMR_API SMR_Init(LONG lReserved)
    {

        if(NULL == g_hShareMemory)
        {
            g_hShareMemory =
                CreateFileMapping(
                NULL,
                NULL,
                PAGE_READWRITE,
                0,
                SHARE_MEMORY_SIZE,
                SHARED_MEMORY_NAME);

            if(g_hShareMemory == NULL)
            {	 
                return FALSE; 
            }
        }

        if(NULL == g_lpShareMemoryStartAddress)
        {
            g_lpShareMemoryStartAddress =
                MapViewOfFile(
                g_hShareMemory,
                FILE_MAP_WRITE | FILE_MAP_READ,
                0,
                0,
                0);
            if (g_lpShareMemoryStartAddress == NULL)
            {
                CloseHandle(g_hShareMemory);
                return FALSE;
            }
        }

        if(NULL == g_hSharedMemoryMutex)
        {
            g_hSharedMemoryMutex = CreateMutex(NULL,FALSE,SHARED_MEMORY_MUTEX_NAME);
            if (g_hSharedMemoryMutex == NULL)
            {
                return FALSE;
            }

        }
        if(NULL == g_hShareMemoryEvent)
        {
            g_hShareMemoryEvent = CreateEvent(NULL,FALSE,FALSE,HARED_MEMROY_NEW_TBOARDBUTTON_EVENT);
            if (g_hShareMemoryEvent == NULL)
            {
                return FALSE;
            }
        }

        return TRUE;
    }

	 //@功  能: 程序结束时需要释放各个参数
	 //@参  数: lReserved为保留值，0是不做任何处理
	 //
	 //
	 //@返回值: 返回布尔类型，TURE是初始化成功。FALSE为初始化失败

	 BOOL SMR_API SMR_Uninit(LONG lReserved)
	 {
		 DWORD dwWaitResult = WaitForSingleObject(g_hSharedMemoryMutex,10*1000);
		 if (g_lpShareMemoryStartAddress !=NULL)
		 {
			 UnmapViewOfFile(g_lpShareMemoryStartAddress);

			 g_lpShareMemoryStartAddress =NULL;
		 }

		 if (g_hShareMemory)
		 {
			 CloseHandle(g_hShareMemory);
			 g_hShareMemory =NULL;
		 }
		 ReleaseMutex(g_hSharedMemoryMutex);

		 if (g_hShareMemoryEvent != NULL)
		 {
			 CloseHandle(g_hShareMemoryEvent);
			 g_hShareMemoryEvent =NULL;
		 }

		 CloseHandle(g_hSharedMemoryMutex);

		 g_hSharedMemoryMutex =NULL;

		 return TRUE;
	 }

	 //@功  能: 函数是向共享内存写入数据
	 //@参  数: pData是需要写入的数据，dataLength写入数据的长度，lReserved为保留值，OffSet 是写的偏移量，0是不做任何处理
	 //
	 //
	 //@返回值: 返回是写入数据的长度

	 UINT SMR_API SMR_Write(const char * pProName, const char * pVerName,const unsigned char* pData, UINT dataLength,UINT OffSet ,LONG lReserved)
	 {
		 if (OpenShareMemory())
		 {
			 DWORD dwWaitResult = WaitForSingleObject( 
				 g_hSharedMemoryMutex,// handle to mutex
				 10);

			 if(dwWaitResult == WAIT_OBJECT_0)
			 {
				 char m_BlockNum[SHARE_MEMORY_PROGRAME_BLOCK_NUM_SIZE];               //写之前先把块的个数得到
				 memset(m_BlockNum,0,sizeof(m_BlockNum));

				 memcpy(m_BlockNum,g_lpShareMemoryStartAddress,SHARE_MEMORY_PROGRAME_BLOCK_NUM_SIZE);

				 int m_nBlockNum = atoi(m_BlockNum);   

				 if (m_nBlockNum == 0)         //开始这里是0.说明 没有一个数据块，那么就增加数据块
				 {
					 SMR_AddDataBlock(pProName,pVerName,(char *)g_lpShareMemoryStartAddress+SHARE_MEMORY_PROGRAME_BLOCK_NUM_SIZE,128);

					 m_nBlockNum++;
					 _itoa_s(m_nBlockNum,m_BlockNum,10);

					 memcpy(g_lpShareMemoryStartAddress,m_BlockNum,SHARE_MEMORY_PROGRAME_BLOCK_NUM_SIZE);

					 if(dataLength >  SHARE_MEMORY_SIZE) 
					 {
						 dataLength = SHARE_MEMORY_SIZE; 
					 }

					 memcpy((char *)g_lpShareMemoryStartAddress + OffSet+ 50 , pData , dataLength);

					 SetEvent(g_hShareMemoryEvent);
				 }
				 else
				 {
					 unsigned char* THead = (unsigned char*)g_lpShareMemoryStartAddress+SHARE_MEMORY_PROGRAME_BLOCK_NUM_SIZE;
// 					 BOOL FoundBolck = FALSE;
// 					 int  nDatalen =0;

// 					 for(int i = 0;i< m_nBlockNum;i++)
// 					 {
						 char  m_ProName[SHARE_MEMORY_PROGRAME_NAME_SIZE];

						 memset(m_ProName,0,sizeof(m_ProName));
						 memcpy(m_ProName,THead,SHARE_MEMORY_PROGRAME_NAME_SIZE);


						 if (strcmp(m_ProName,pProName) == 0)
						 {
							 if(dataLength >  SHARE_MEMORY_SIZE) 
							 {
								 dataLength = SHARE_MEMORY_SIZE; 
							 }

							 memset(THead + OffSet + 46 , 0, 128 );
							 memcpy(THead + OffSet + 46, pData , dataLength);

							 SetEvent(g_hShareMemoryEvent);
	//						 FoundBolck  = TRUE;                 //发现有相同的块
	//						 break;
						 }
// 					 }
// 					 if(!FoundBolck)               //没有相同的名称，说明是新的一个数据块。需要分配数据
// 					 {
// 
// 					 }

				 }

				 ReleaseMutex(g_hSharedMemoryMutex);
				 return dataLength;

			 }		
		 }		
		 return 0U;
	 }

	 //@功  能: 函数是从共享内存中读出数据
	 //@参  数: pBuf读出数据的存储内存。BufSize读出数据的长度。lReserved为保留值，OffSet 是读的偏移量，0是不做任何处理
	 //
	 //
	 //@返回值: 返回是写入数据的长度
	 UINT SMR_API SMR_Read(const char * pProName, const char * pVerName ,unsigned char* pBuf, UINT BufSize,UINT OffSet , LONG lReserved)
	 {
		 DWORD dwWaitResult = WaitForSingleObject(g_hSharedMemoryMutex,10);

		 if (dwWaitResult == WAIT_OBJECT_0)
		 {
			 if(OpenShareMemory())
			 {
				 if(BufSize > SHARE_MEMORY_SIZE) 
				 {
					 BufSize = SHARE_MEMORY_SIZE;
				 }

				 memcpy(pBuf,(unsigned char *)(g_lpShareMemoryStartAddress)+ 50 + OffSet ,BufSize);
				 ReleaseMutex(g_hSharedMemoryMutex);

				 return BufSize;
			 }
		 }		 
		 return 0U;
	 }


	 //@功  能: 函数是等待写的事件
	 //@参  数: dwWaitTime为等待的时间，单位是毫秒
	 //
	 //
	 //@返回值: 返回事件的类型

	 DWORD SMR_API SMR_WaitForWriteEvent(DWORD dwWaitTime)
	 {
		
		DWORD dwWaitResult = WaitForSingleObject(g_hShareMemoryEvent,dwWaitTime);
		return dwWaitResult;
	 }

     //需要增加数据块的开始地址也需要传输进去
	 UINT  SMR_API SMR_AddDataBlock(const char * ProgramName, const char * VersionNumber,LPVOID StartAddress, UINT nDataLen)
	 {

		 char m_WriteDataLen[SHARE_MEMORY_PROGRAME_LEN_SIZE];
		 memset(m_WriteDataLen,0,sizeof(m_WriteDataLen));
		 _itoa_s(nDataLen,m_WriteDataLen,10);


		 int nProgramNameLen = strlen(ProgramName);
		 if (nProgramNameLen > SHARE_MEMORY_PROGRAME_NAME_SIZE)
		 {
			 nProgramNameLen = SHARE_MEMORY_PROGRAME_NAME_SIZE;
		 }
		 memcpy((char *)StartAddress,ProgramName,nProgramNameLen);


		 int nVersionNumberLen = strlen(VersionNumber);
		 if (nVersionNumberLen > SHARE_MEMORY_PROGRAME_VERSION_SIZE)
		 {
			 nVersionNumberLen =SHARE_MEMORY_PROGRAME_VERSION_SIZE;
		 }
		 memcpy((char *)StartAddress + SHARE_MEMORY_PROGRAME_NAME_SIZE, VersionNumber,nVersionNumberLen);



		 int nWriteDataLen = strlen(m_WriteDataLen);
		 if (nWriteDataLen > SHARE_MEMORY_PROGRAME_LEN_SIZE)
		 {
			 nWriteDataLen = SHARE_MEMORY_PROGRAME_LEN_SIZE;
		 }
		 memcpy((char * )StartAddress +( SHARE_MEMORY_PROGRAME_NAME_SIZE +SHARE_MEMORY_PROGRAME_VERSION_SIZE),m_WriteDataLen,nWriteDataLen);
		 return nDataLen;

	 }

	 UINT SMR_API SMR_ReadDataBlockInfo(char* ProgramName,UINT ProgramNameLen, char * VersionNumber,UINT VersionNumberLen)
	 {
		 DWORD dwWaitResult = WaitForSingleObject(g_hSharedMemoryMutex,10);
		 if (dwWaitResult == WAIT_OBJECT_0)
		 {
			 if(OpenShareMemory())
			 {
				 if (ProgramNameLen > SHARE_MEMORY_PROGRAME_NAME_SIZE )
				 {
					 ProgramNameLen = SHARE_MEMORY_PROGRAME_NAME_SIZE;
				 }
				 memcpy(ProgramName,(char *)g_lpShareMemoryStartAddress+4,ProgramNameLen);


				 if (VersionNumberLen > SHARE_MEMORY_PROGRAME_VERSION_SIZE)
				 {
					 VersionNumberLen = SHARE_MEMORY_PROGRAME_VERSION_SIZE;
				 }
				 memcpy(VersionNumber,(char *)g_lpShareMemoryStartAddress + SHARE_MEMORY_PROGRAME_NAME_SIZE+4,VersionNumberLen);


				 char m_WriteDatalen[SHARE_MEMORY_PROGRAME_LEN_SIZE];
				 memcpy(m_WriteDatalen,(char *)g_lpShareMemoryStartAddress +SHARE_MEMORY_PROGRAME_NAME_SIZE+SHARE_MEMORY_PROGRAME_VERSION_SIZE+4,SHARE_MEMORY_PROGRAME_LEN_SIZE);

				 ReleaseMutex(g_hSharedMemoryMutex);
				 UINT nDataLen = atoi(m_WriteDatalen);

				 return nDataLen;

			 }
		 }
		 return 0U;
	 }


}//end extern "C"


