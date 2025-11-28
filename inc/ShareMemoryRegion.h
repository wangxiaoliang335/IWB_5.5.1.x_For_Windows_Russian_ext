#ifndef _H_SHRARE_MEMROY_REGION
#define _H_SHRARE_MEMROY_REGION

#ifdef SHAREMEMORYREGION_EXPORTS

#define SMR_API _declspec(dllexport)
#else
#define SMR_API _declspec(dllimport)
#endif

extern "C"
{
	//@功  能:程序开始时初始化各个参数。
	//@参  数:lReserved为保留值，0是不做任何处理
	//
	//
	//@返回值: 返回布尔类型，TURE是初始化成功。FALSE为初始化失败

	BOOL SMR_API SMR_Init(LONG lReserved=0);

	//@功  能: 程序结束时需要释放各个参数
	//@参  数: lReserved为保留值，0是不做任何处理
	//
	//
	//@返回值: 返回布尔类型，TURE是初始化成功。FALSE为初始化失败

	BOOL SMR_API SMR_Uninit(LONG lReserved=0);

	//@功  能: 函数是向共享内存写入数据
	//@参  数: pData是需要写入的数据，dataLength写入数据的长度，OffSet 是写的偏移量，lReserved为保留值，0是不做任何处理
	//
	//
	//@返回值: 返回是写入数据的长度

	UINT SMR_API SMR_Write(const char * pProName, const char * pVerName,const unsigned char* pData, UINT dataLength,UINT OffSet ,LONG lReserved = 0);


	//@功  能: 函数是从共享内存中读出数据
	//@参  数: pBuf读出数据的存储内存。BufSize读出数据的长度。OffSet 是读的偏移量lReserved为保留值，0是不做任何处理
	//
	//
	//@返回值: 返回是写入数据的长度
	UINT SMR_API SMR_Read(const char * pProName, const char * pVerName, unsigned char* pBuf, UINT BufSize,UINT OffSet ,LONG lReserved = 0);


	//@功  能: 函数是等待写的事件
	//@参  数: dwWaitTime为等待的时间，单位是毫秒
	//
	//
	//@返回值: 返回事件的类型
	DWORD SMR_API SMR_WaitForWriteEvent(DWORD dwWaitTime);

	//@功  能: 函数是写入程序的相关信息
	//@参  数: ProgramName程序的名称。VersionNumber 程序的版本号，DataLen需要写入的字符的长度
	//
	//
	//@返回值: 返回内存块的长度
	
	UINT SMR_API SMR_AddDataBlock(const char * ProgramName, const char * VersionNumber, LPVOID StartAddress ,UINT nDataLen);


	//@功  能: 函数是读出程序的相关信息
	//@参  数: ProgramName程序的名称。ProgramNameLen程序名称的长度，VersionNumber 程序的版本号，VersionNumberLen程序版本号的长度
	//
	//
	//@返回值: 返回读出数据的长度

	UINT SMR_API SMR_ReadDataBlockInfo(char* ProgramName,UINT ProgramNameLen, char * VersionNumber,UINT VersionNumberLen);
}
#endif