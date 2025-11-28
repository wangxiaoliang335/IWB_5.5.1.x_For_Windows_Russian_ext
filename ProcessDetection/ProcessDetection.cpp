#include "StdAfx.h"

#include <assert.h>
#define _WIN32_DCOM  
#include <comdef.h>   
#include <atlbase.h>  
#include <WtsApi32.h>
using namespace std;

#include "ProcessDetection.h"

#pragma comment(lib, "WtsApi32.lib")
#pragma comment(lib, "wbemuuid.lib")  

const int KMININTERVAL_BETWEEN_EMUMS = 5000; //毫秒

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//class CProcessDetection;

class EventSink : public IWbemObjectSink
{
public:
    EventSink(CProcessDetection* pProcessDetection) { m_pProcessDetection = pProcessDetection; m_lRef = 0; }
    ~EventSink() { bDone = true; }

    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();        
    virtual HRESULT 
        STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);

    virtual HRESULT STDMETHODCALLTYPE Indicate( 
        LONG lObjectCount,
        IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
        );

    virtual HRESULT STDMETHODCALLTYPE SetStatus( 
        /* [in] */ LONG lFlags,
        /* [in] */ HRESULT hResult,
        /* [in] */ BSTR strParam,
        /* [in] */ IWbemClassObject __RPC_FAR *pObjParam
        );
protected:    
    LONG m_lRef;
    bool bDone;

    CProcessDetection* m_pProcessDetection;
};


ULONG EventSink::AddRef()
{
    return InterlockedIncrement(&m_lRef);
}

ULONG EventSink::Release()
{
    LONG lRef = InterlockedDecrement(&m_lRef);
    if(lRef == 0)
        delete this;
    return lRef;
}

HRESULT EventSink::QueryInterface(REFIID riid, void** ppv)
{
    if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
    {
        *ppv = (IWbemObjectSink *) this;
        AddRef();
        return WBEM_S_NO_ERROR;
    }
    else return E_NOINTERFACE;
}


HRESULT EventSink::Indicate(long lObjectCount,
                            IWbemClassObject **apObjArray)
{
    if (m_pProcessDetection)
    {
        return m_pProcessDetection->OnReponseProcessModification(lObjectCount, apObjArray);
    }

    return WBEM_S_NO_ERROR;    
}

HRESULT EventSink::SetStatus(
                             /* [in] */ LONG lFlags,
                             /* [in] */ HRESULT hResult,
                             /* [in] */ BSTR strParam,
                             /* [in] */ IWbemClassObject __RPC_FAR *pObjParam
                             )
{
    if(lFlags == WBEM_STATUS_COMPLETE)
    {
        printf("Call complete. hResult = 0x%X\n", hResult);
    }
    else if(lFlags == WBEM_STATUS_PROGRESS)
    {
        printf("Call in progress.\n");
    }

    return WBEM_S_NO_ERROR;
}    // end of EventSink.cpp
//////////////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain( HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//@功能:线程处理函数
ULONG _stdcall CProcessDetection::ProcessThreadProc(LPVOID lpCtx)
{
	////提高线程的优先级别
	//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	CProcessDetection* lpThis = reinterpret_cast<CProcessDetection*>(lpCtx);

	HRESULT hres;

    //hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    hres = CoInitialize(NULL);

    if (FAILED(hres))
    {
        return 1;                      // Program has failed.
    }

	hres =  CoInitializeSecurity(
		NULL, 
		-1,                          // COM negotiates service
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
		);


	if (FAILED(hres))
	{
		cout << "Failed to initialize security. Error code = 0x" 
			<< hex << hres << endl;	
        CoUninitialize();
		return 1;                      // Program has failed.
	}

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------

	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,             
		0, 
		CLSCTX_INPROC_SERVER, 
		IID_IWbemLocator, (LPVOID *) &pLoc);

	if (FAILED(hres))
	{
		cout << "Failed to create IWbemLocator object. "
			<< "Err code = 0x"
			<< hex << hres << endl;	
        CoUninitialize();
		return 1;                 // Program has failed.
	}

	// Step 4: ---------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	//IWbemServices *pSvc = NULL;
    if (lpThis->m_pSvc)
    {
        lpThis->m_pSvc->Release();
        lpThis->m_pSvc = NULL;
    }

	// Connect to the local root\cimv2 namespace
	// and obtain pointer pSvc to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), 
		NULL,
		NULL, 
		0, 
		NULL, 
		0, 
		0, 
		&(lpThis->m_pSvc)
		);

	if (FAILED(hres))
	{
		cout << "Could not connect. Error code = 0x" 
			<< hex << hres << endl;
		pLoc->Release();  
        CoUninitialize();
		return 1;                // Program has failed.
	}

	//cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		lpThis->m_pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx 
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx 
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
		);

	if (FAILED(hres))
	{
		cout << "Could not set proxy blanket. Error code = 0x" 
			<< hex << hres << endl;
		lpThis->m_pSvc->Release();
        lpThis->m_pSvc = NULL;
		pLoc->Release();  
        CoUninitialize();
		return 1;               // Program has failed.
	}

	// Step 6: -------------------------------------------------
	// Receive event notifications -----------------------------

	// Use an unsecured apartment for security
	IUnsecuredApartment* pUnsecApp = NULL;

	hres = CoCreateInstance(CLSID_UnsecuredApartment, NULL, 
		CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment, 
		(void**)&pUnsecApp);

	EventSink* pSink = new EventSink(lpThis);
	pSink->AddRef();

	IUnknown* pStubUnkCreate = NULL; 
	pUnsecApp->CreateObjectStub(pSink, &pStubUnkCreate);

	IWbemObjectSink* pStubSinkCreate = NULL;
	pStubUnkCreate->QueryInterface(IID_IWbemObjectSink,
		(void **) &pStubSinkCreate);

	// The ExecNotificationQueryAsync method will call
	// The EventQuery::Indicate method when an event occurs
	hres = lpThis->m_pSvc->ExecNotificationQueryAsync(
		_bstr_t("WQL"), 
		_bstr_t("SELECT * " 
		"FROM __InstanceOperationEvent WITHIN 10 "
		"WHERE TargetInstance ISA 'Win32_Process'"), 
		WBEM_FLAG_SEND_STATUS, 
		NULL, 
		pStubSinkCreate);
	//WBEM_E_FAILED
	// Check for errors.
	if (FAILED(hres))
	{
		printf("ExecNotificationQueryAsync failed "
			"with = 0x%X\n", hres);
		lpThis->m_pSvc->Release();
        lpThis->m_pSvc = NULL;
		pLoc->Release();
		pUnsecApp->Release();
		pStubUnkCreate->Release();
		pSink->Release();
		pStubSinkCreate->Release();	
        CoUninitialize();
		return 1;
	}

	//pSink->AddRef();

	//IUnknown* pStubUnkDelete = NULL; 
	//pUnsecApp->CreateObjectStub(pSink, &pStubUnkDelete);

	//IWbemObjectSink* pStubSinkDelete = NULL;
	//pStubUnkDelete->QueryInterface(IID_IWbemObjectSink,
	//	(void **) &pStubSinkDelete);

	//// The ExecNotificationQueryAsync method will call
	//// The EventQuery::Indicate method when an event occurs
	//hres = pSvc->ExecNotificationQueryAsync(
	//	_bstr_t("WQL"), 
	//	_bstr_t("SELECT * " 
	//	"FROM __InstanceDeletionEvent WITHIN 5 "
	//	"WHERE TargetInstance ISA 'Win32_Process'"), 
	//	WBEM_FLAG_SEND_STATUS, 
	//	NULL, 
	//	pStubSinkDelete);
	////WBEM_E_FAILED
	//// Check for errors.
	//if (FAILED(hres))
	//{
	//	printf("ExecNotificationQueryAsync failed "
	//		"with = 0x%X\n", hres);
	//	pSvc->Release();
	//	pLoc->Release();
	//	pUnsecApp->Release();
	//	pStubUnkDelete->Release();
	//	pSink->Release();
	//	pStubSinkDelete->Release();	
 //       CoUninitialize();
	//	return 1;
	//}

	lpThis->m_bIsWMISucceed = true;


    MSG msg;
    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	//while(true)
	//{
	//	DWORD dwWaitResult = WaitForSingleObject(lpThis->m_hEventExit, INFINITE);

	//	break;
	//}   

	hres = lpThis->m_pSvc->CancelAsyncCall(pStubSinkCreate);
	//hres = pSvc->CancelAsyncCall(pStubSinkDelete);

	// Cleanup
	// ========

	lpThis->m_pSvc->Release();
    lpThis->m_pSvc = NULL;
	pLoc->Release();
	pUnsecApp->Release();
	pStubUnkCreate->Release();
	pSink->Release();
	pStubSinkCreate->Release();
	//pStubUnkDelete->Release();
	//pSink->Release();
	//pStubSinkDelete->Release();

    CoUninitialize();

	return 0UL;
}

//////////////////////////////////////////////////////////////////////////

CProcessDetection::CProcessDetection(void)
	: m_hEventExit(NULL),
	  m_hProcessThread(NULL),
	  m_bIsWMISucceed(false),
	  m_dwLastEnumTime(0),
      m_pSvc(NULL),
      m_dwThreadId(0)
{    
    m_currentuser = "";
	m_processInfoMap.clear();
    m_specProcInfo[0].procName = "JOSS.EXE";
    m_specProcInfo[0].procCreatDate = "";
}

CProcessDetection::~CProcessDetection(void)
{
	StopDetection();

	m_processInfoMap.clear(); 

    m_pSvc = NULL;
}

void CProcessDetection::EnumateProcess()
{
	if(m_bIsWMISucceed) return;

	DWORD dwCurTime = GetTickCount();

	if(dwCurTime - m_dwLastEnumTime < KMININTERVAL_BETWEEN_EMUMS) return;

	WTS_PROCESS_INFO *pProcessInfo = NULL;
	DWORD            dwCount = 0;

	if (FALSE == WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pProcessInfo, &dwCount)) return;

	if (0 == dwCount || NULL == pProcessInfo) return;    

    //PSID  currentUserSid = NULL;
    //{
    //    //首先获取本进程的SID,根据该SID，剔除其它用户的进程
    //    DWORD currentProcessId = GetCurrentProcessId();        

    //    TCHAR userName[256];
    //    DWORD dwBufferSize = 256;
    //    memset(userName, 0, sizeof(TCHAR) * 256);
    //    if (GetUserName(userName, &dwBufferSize))
    //    {
    //        m_currentuser = CT2A(userName);
    //    }

    //    for(int i = 0; i < dwCount; i++)
    //    {
    //        WTS_PROCESS_INFO &refInfo = pProcessInfo[i];
    //        if (refInfo.ProcessId == currentProcessId)
    //        {
    //            currentUserSid = refInfo.pUserSid;

    //            break;
    //        }
    //    }
    //}

    bool isFindSpecProc = false;
    m_specProcInfo[0].isProcExist = false;

    {
        //CComCritSecLock<CComAutoCriticalSection> cs(m_csForAccessData);

        for (unsigned int i = 0; i < dwCount; i++)
        {
            WTS_PROCESS_INFO &refInfo = pProcessInfo[i];
            
            if (refInfo.pUserSid)
            {
               /* if (!EqualSid(currentUserSid, refInfo.pUserSid))
                {
                    continue;
                }*/                

                CW2A strProcName(refInfo.pProcessName);

                m_processInfoMap[refInfo.ProcessId] = strProcName;

                if (!isFindSpecProc)
                {
                    if (_stricmp(strProcName, m_specProcInfo[0].procName.c_str()) == 0)
                    {
                        isFindSpecProc = true;
                        /*m_specProcInfo[0].isProcExist = true;
                        m_specProcInfo[0].procID = refInfo.ProcessId;*/
                    }
                }        
            }                                                      
        }        
    }	

	WTSFreeMemory(pProcessInfo);
	pProcessInfo = NULL;

	m_dwLastEnumTime = dwCurTime;
}

//const map<DWORD, string> &CProcessDetection::GetProcessInfoMap()
//{
//	EnumateProcess();
//
//	return m_processInfoMap;
//}

//int CProcessDetection::GetCurrentProcessInfo(std::string *procNameArry, const int &arrySize)
//{
//    if (procNameArry == NULL || arrySize <= 0) return 0;
//
//    int i = 0;
//    {
//        CComCritSecLock<CComAutoCriticalSection> cs(m_csForAccessData);
//
//        EnumateProcess();
//
//        std::map<DWORD, std::string>::iterator it = m_processInfoMap.begin();        
//        for(; it != m_processInfoMap.end(); it++)
//        {
//            procNameArry[i] = it->second;
//
//            i++;
//            if (i >= arrySize) break;
//        }
//
//    }
//    return i;
//}

int CProcessDetection::GetCurrentProcessInfo(char (*ppProcNameArry)[256], const int &nMaxArrySize)
{
    if (ppProcNameArry == NULL || nMaxArrySize <= 0) return 0;

    int i = 0;
    {
        CComCritSecLock<CComAutoCriticalSection> cs(m_csForAccessData);

        EnumateProcess();

        std::map<DWORD, std::string>::iterator it = m_processInfoMap.begin();        
        for(; it != m_processInfoMap.end(); it++)
        {
            int nCpyLen = it->second.length();
            if (nCpyLen >= 255)
            {
                   nCpyLen = 255;
            }     

            memcpy_s(ppProcNameArry[i], 255, it->second.c_str(), nCpyLen);
            i++;

            if (i >= nMaxArrySize) break;
        }
    }

    return i;
}

bool CProcessDetection::IsTheSpecialWatchProcessExsit()
{    
    {
        CComCritSecLock<CComAutoCriticalSection> cs(m_csForAccessData);

        EnumateProcess();
        return m_specProcInfo[0].isProcExist;
    }    
}

//bool CProcessDetection::GetProcessNameWithPID(DWORD dwProcId, const std::string &procName, bool isFindSpecProc)
//{
//    {
//        CComCritSecLock<CComAutoCriticalSection> cs(m_csForAccessData);
//
//        EnumateProcess();
//
//        std::map<DWORD, std::string>::iterator it = m_processInfoMap.begin();
//
//        if (isFindSpecProc)
//        {
//            if (m_specProcInfo[0].isProcExist && m_specProcInfo[0].procID == dwProcId)
//            {
//                procName = m_specProcInfo[0].procName;
//
//                return true;
//            }       
//
//            procName = "";
//            return false;
//        }    
//
//        it = m_processInfoMap.find(dwProcId);
//
//        if (it != m_processInfoMap.end())
//        {
//            procName = it->second;
//
//            return true;
//        }
//    }
//
//    procName = "";
//
//    return false;
//}

bool CProcessDetection::IsTheSpecProcessWithPID(const DWORD &dwProcId)
{
    if (m_specProcInfo[0].isProcExist && m_specProcInfo[0].procID == dwProcId)
    {
        return true;
    }

    return false;
}

bool CProcessDetection::GetProcessNameWithPID(DWORD dwProcId, char procName[], const int &arrySize, bool isFindSpecProc)
{
    if (procName == NULL || arrySize <= 0) return false;

    {
        CComCritSecLock<CComAutoCriticalSection> cs(m_csForAccessData);

        EnumateProcess();

        std::map<DWORD, std::string>::iterator it = m_processInfoMap.begin();

        if (isFindSpecProc)
        {
            if (m_specProcInfo[0].isProcExist && m_specProcInfo[0].procID == dwProcId)
            {
                int nCpyLen = m_specProcInfo[0].procName.length();
                if (nCpyLen > arrySize - 1) nCpyLen = arrySize - 1;
                memcpy_s(procName, arrySize, m_specProcInfo[0].procName.c_str(), nCpyLen);
                return true;
            }       

            return false;
        }    

        it = m_processInfoMap.find(dwProcId);

        if (it != m_processInfoMap.end())
        {
            int nCpyLen = it->second.length();
            if (nCpyLen > arrySize - 1) nCpyLen = arrySize - 1;
            memcpy_s(procName, arrySize, it->second.c_str(), nCpyLen);

            return true;
        }
    }

    return false;
}

void  CProcessDetection::AddSpecialWatchProcName(const std::string &procName)
{
    m_specProcInfo[0].isProcExist = false;
    m_specProcInfo[0].procName = procName;
}

//const std::string &CProcessDetection::IsTheSpecWatchProcessWithID(const DWORD &dwProcId)
//{
//    ST_SPEC_WATCH_PROCSS &refElem = m_specProcInfo[0];
//    if (refElem.isProcExist && dwProcId == refElem.procID && dwProcId != 0)
//    {
//        return m_specProcInfo[0].procName;
//    }
//
//    return "";
//}

void CProcessDetection::StartDetection()
{
	assert(NULL == m_hProcessThread);

	EnumateProcess();

	m_hEventExit            = ::CreateEvent(NULL, FALSE, FALSE, NULL);	

	//DWORD dwThreadId = 0;//线程Id
	m_hProcessThread = CreateThread(
		NULL,//lpThreadAttributes
		0,   //dwStackSize
		ProcessThreadProc,//lpStartAddress,
		(LPVOID)this,     //lpParameter,
		0,                //dwCreationFlags
		&m_dwThreadId       //lpThreadId
		);
}

void CProcessDetection::StopDetection()
{
	if (NULL == m_hEventExit) return;

	//触发线程退出事件
	//SetEvent(m_hEventExit);
    //PostThreadMessage(GetThreadId(m_hProcessThread), WM_QUIT, 0, 0);
    PostThreadMessage(m_dwThreadId, WM_QUIT, 0, 0);
	WaitForSingleObject(m_hProcessThread, INFINITE);
	m_hProcessThread = NULL;  

	CloseHandle(m_hEventExit);
	m_hEventExit = NULL;
}

HRESULT CProcessDetection::OnReponseProcessModification(long lObjectCount, IWbemClassObject **apObjArray)
{
    HRESULT hres = S_OK;
    
    for (int i = 0; i < lObjectCount; i++)
    {
        /*printf("Event occurred\n");*/
        do 
        {
            CComVariant var_Class;
            hres = (apObjArray[i])->Get( L"__Class", 0, &var_Class, NULL, NULL );

            if (!SUCCEEDED(hres)) break;

            if (var_Class != CComVariant("__InstanceCreationEvent") && var_Class != CComVariant("__InstanceDeletionEvent"))
            {
                break;
            }

           /* IsTheProcessOfCurrentUser(apObjArray[i]);*/

            CComVariant var_TarInst;
            hres = (apObjArray[i])->Get( L"TargetInstance", 0, &var_TarInst, NULL, NULL );

            if (!SUCCEEDED(hres)) break;            

            IUnknown* punkVal = var_TarInst.punkVal;
            CComPtr< IWbemClassObject > tarInstObj;

            if (punkVal == NULL) break;

            hres = punkVal->QueryInterface( IID_IWbemClassObject, reinterpret_cast< void** >( &tarInstObj ) );

            if (!SUCCEEDED(hres)) break;                                                  

            if (CComVariant("__InstanceCreationEvent") == var_Class)
            {
                //pEventType = "Creation Event";
                //bIsCreateEvent = true;	
                if(!IsTheProcessOfCurrentUser(tarInstObj.p)) break;

                DWORD dwProcessId = 0;			            

                CComVariant var_processid;
                hres = tarInstObj->Get(L"ProcessId", 0, &var_processid, NULL, NULL);	

                if (SUCCEEDED(hres)) dwProcessId = var_processid.lVal;

                CComVariant var_processname;
                hres = tarInstObj->Get(L"Name", 0, &var_processname, NULL, NULL);

                if (SUCCEEDED(hres)) 
                {
                    CComCritSecLock<CComAutoCriticalSection> cs(m_csForAccessData);

                    char *pProcessName = NULL;

                    pProcessName =_com_util::ConvertBSTRToString(var_processname.bstrVal);
                    {                        
                        m_processInfoMap[dwProcessId] = pProcessName;
                        
                        ////if (m_specProcInfo[0].isProcExist == false)
                        //{
                        //    if (_stricmp(pProcessName, m_specProcInfo[0].procName.c_str()) == 0)
                        //    {
                        //        CComVariant var_createDate;
                        //        hres = tarInstObj->Get(L"CreationDate", 0, &var_createDate, NULL, NULL);

                        //        if (SUCCEEDED(hres))
                        //        {   
                        //            char *pProcCreateDate = NULL;
                        //            pProcCreateDate =_com_util::ConvertBSTRToString(var_createDate.bstrVal);
                        //            //OutputDebugStringA("$$$$$$$$$$$$Special Process Creation!!!!\r\n");
                        //            if (m_specProcInfo[0].isProcExist)
                        //            {                                                                        
                        //                if (strcmp(pProcCreateDate, m_specProcInfo[0].procCreatDate.c_str()) < 0)
                        //                {//当前同名进程开启的时间较早，则选择较早的进程作为特殊进程
                        //                    m_specProcInfo[0].procID = dwProcessId;
                        //                    m_specProcInfo[0].procCreatDate = pProcCreateDate;
                        //                }
                        //            }
                        //            else
                        //            {
                        //                m_specProcInfo[0].isProcExist = true;
                        //                m_specProcInfo[0].procID = dwProcessId;
                        //                m_specProcInfo[0].procCreatDate = pProcCreateDate;
                        //            }

                        //            delete []pProcCreateDate;
                        //            pProcCreateDate = NULL;
                        //        }                                
                        //        else
                        //        {
                        //            m_specProcInfo[0].isProcExist = true;
                        //            m_specProcInfo[0].procID = dwProcessId;
                        //        }                                
                        //    }
                        //}
                    }

                    delete []pProcessName;
                    pProcessName = NULL;
                }		
            }
            else if (CComVariant("__InstanceDeletionEvent") == var_Class)
            {
                //pEventType = "Deletion Event";
                //bIsCreateEvent = false;
                {
                    //CComCritSecLock<CComAutoCriticalSection> cs(m_csForAccessData);

                    DWORD dwProcessId = 0;			            

                    CComVariant var_processid;
                    hres = tarInstObj->Get(L"ProcessId", 0, &var_processid, NULL, NULL);	

                    if (SUCCEEDED(hres)) dwProcessId = var_processid.lVal;

                    CComCritSecLock<CComAutoCriticalSection> cs(m_csForAccessData);

                    //if (m_specProcInfo[0].isProcExist)
                    //{
                    //    if (m_specProcInfo[0].procID == dwProcessId)
                    //    {
                    //        m_specProcInfo[0].isProcExist = false;

                    //        //OutputDebugStringA("$$$$$$$$$$$$Special Process Deletion!!!!\r\n");
                    //    }
                    //}

                    map<DWORD, string>::iterator it = m_processInfoMap.find(dwProcessId);
                    if (it != m_processInfoMap.end()) m_processInfoMap.erase(it);
                }
            }								

            //printf("Event=%s occurred!!! ProcessId=%d ProcessName=%s\n", pEventType, dwProcessId, pProcessName);				

        } while (false);

    }

    return WBEM_S_NO_ERROR;
}

bool CProcessDetection::IsTheProcessOfCurrentUser(IWbemClassObject *&pClsObj)
{
    HRESULT hres = S_FALSE;

    do 
    {
        if(m_pSvc == NULL) break;

        //IWbemClassObject *pClass = NULL;

        //CComBSTR className = L"Win32_Process";
        //hres = m_pSvc->GetObject(className, 0, NULL, &pClass, NULL);

        //if (!SUCCEEDED(hres)) break;
        //
        //IWbemClassObject *pOutParamsDef = NULL;
        //CComBSTR methodName = L"GetOwner";
        //hres = pClass->GetMethod(methodName, 0, NULL, &pOutParamsDef);

        //if (!SUCCEEDED(hres)) break;
        //
        //IWbemClassObject *pOutparamClassInst = NULL;
        //hres = pOutParamsDef->SpawnInstance(0, &pOutparamClassInst);

        ///*CComVariant varCommand;

        //hres = pOutparamClassInst->Put()*/

        CIMTYPE pType;
        LONG pFlavor;
        CComVariant varPath;
        hres = pClsObj->Get(L"__PATH", 0, &varPath, &pType, &pFlavor);

        if (!SUCCEEDED(hres)) break;

        CComBSTR methodName = L"GetOwnerSid";
        IWbemClassObject *pOutParams = NULL;        
        hres = m_pSvc->ExecMethod(varPath.bstrVal, methodName, 0, NULL, NULL, &pOutParams, NULL);

        if (!SUCCEEDED(hres)) break;

        CComVariant varUserSid;
        hres = pOutParams->Get(L"Sid", 0, &varUserSid, &pType, &pFlavor);

        if (!SUCCEEDED(hres)) break;   

        if (varUserSid.vt != VT_NULL) 
        {
            if (varUserSid.bstrVal != NULL)
            {
                hres = S_OK;
            }
        }
    } while (false);

    return (hres == S_OK);
}
