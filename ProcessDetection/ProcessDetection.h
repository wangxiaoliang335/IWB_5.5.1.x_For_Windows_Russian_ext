#pragma once

#ifdef EASIPROCESSDETECTION_EXPORTS
#define EASI_PROCDETECTION_CLASS __declspec(dllexport)
#else
#define EASI_PROCDETECTION_CLASS __declspec(dllimport)
#endif

#include <Wbemidl.h> 
#include <iostream>  
#include <map>
#include <string>

//////////////////////////////////////////////////////////////////////////
const int KMAXSPECWATCHPROCS    = 1;
typedef struct _ST_SPEC_WATCH_PROCSS
{
    string procName;
    DWORD  procID;
    bool   isProcExist;
    string procCreatDate;
}ST_SPEC_WATCH_PROCSS;
//////////////////////////////////////////////////////////////////////////

class EASI_PROCDETECTION_CLASS CProcessDetection
{
public:
	CProcessDetection(void);
	~CProcessDetection(void);

public:
	void StartDetection();
	void StopDetection();

	/*const map<DWORD, string> &GetProcessInfoMap();*/

    bool IsTheSpecProcessWithPID(const DWORD &dwProcId);

    bool GetProcessNameWithPID(DWORD dwProcId, const std::string &procName, bool isFindSpecProc = false);

    bool GetProcessNameWithPID(DWORD dwProcId, char procName[], const int &arrySize, bool isFindSpecProc = false);

    bool IsTheSpecialWatchProcessExsit();

    void AddSpecialWatchProcName(const std::string &procName);

    //int GetCurrentProcessInfo(std::string *procNameArry, const int &arrySize);
    int GetCurrentProcessInfo(char (*ppProcNameArry)[256], const int &nMaxArrySize);

    /*const std::string &IsTheSpecWatchProcessWithID(const DWORD &dwProcId);*/
public:
    HRESULT OnReponseProcessModification(long lObjectCount, IWbemClassObject **apObjArray);

private:
    bool IsTheProcessOfCurrentUser(IWbemClassObject *&clsObj);

private:
	static ULONG _stdcall ProcessThreadProc(LPVOID lpCtx);
	void EnumateProcess();

private:
	HANDLE			m_hEventExit;
    DWORD           m_dwThreadId;
	HANDLE			m_hProcessThread;

	bool			m_bIsWMISucceed;
	DWORD			m_dwLastEnumTime; //如果WMI初始化失败，则每隔一定时间段枚举进程

    map<DWORD, string> m_processInfoMap;
    ST_SPEC_WATCH_PROCSS m_specProcInfo[KMAXSPECWATCHPROCS]; //特别监视进程，现在只有一个GLBOARD程序

    CComAutoCriticalSection m_csForAccessData;

    IWbemServices   *m_pSvc;

    string          m_currentuser;

    //bool            m_hasThreadStarted;
};

