/*************************************************************
Author		: David A. Jones
File Name	: MemLeakDetect.h
Date		: July 30, 2004
Synopsis	:		 
A trace memory feature for source code to trace and
find memory related bugs. 

Future		:
1) Memory corruption
2) Freeing memory without allocating
3) Freeing memory twice
4) Not Freeing memory at all
5) over running memory boundardies


****************************************************************/
#if !defined(MEMLEAKDETECT_H)
#define MEMLEAKDETECT_H

#define _CRTDBG_MAP_ALLOC

#include <map>
#define _CRTBLD
#include <windows.h>
#include "dbgint.h"
#include <ImageHlp.h>
#include <crtdbg.h>
#include <atlbase.h>

//#define  MLD_CUSTOMSTACKWALK

#pragma comment( lib, "imagehlp.lib" )

using namespace std;

// if you want to use the custom stackwalker otherwise
// comment this line out
//#define MLD_CUSTOMSTACKWALK			1
//
#define MLD_MAX_NAME_LENGTH			256
#define MLD_MAX_TRACEINFO			256
#define MLD_TRACEINFO_EMPTY			_T("")
#define MLD_TRACEINFO_NOSYMBOL		_T("?(?)")

#ifdef  MLD_CUSTOMSTACKWALK
#define MLD_STACKWALKER				symStackTrace2
#else
#define MLD_STACKWALKER				symStackTrace
#endif


inline void MemLeakTrace(LPCTSTR lpszFormat, ...)
{
	USES_CONVERSION;
	TCHAR buffer[1024*32];
 	va_list args;
	va_start( args, lpszFormat);
	_vstprintf_s( buffer, _countof(buffer), lpszFormat, args );

	OutputDebugString(buffer);
}


typedef DWORD ADDR;

class CMemLeakDetect
{
public:

	typedef struct 	{
		ADDRESS				addrPC;
		ADDRESS				addrFrame;

	} STACKFRAMEENTRY;

	typedef struct { 
		void*				address;
		DWORD				size;
		TCHAR				fileName[MLD_MAX_NAME_LENGTH];
		DWORD				lineNumber;
		DWORD				occurance;
		STACKFRAMEENTRY		traceinfo[MLD_MAX_TRACEINFO];

	} AllocBlockInfo;

	//typedef int POSITION;
	typedef map<LPVOID, AllocBlockInfo>				KEYMAP;
	typedef map<LPVOID, AllocBlockInfo>::iterator	POSITION;
	typedef pair<LPVOID, AllocBlockInfo>			KEYVALUE;

	class CMapMem
	{
	public:

		KEYMAP			m_Map;
		POSITION		m_Pos;

		inline BOOL Lookup(LPVOID pAddr,  AllocBlockInfo& aInfo) { 

			m_Pos = m_Map.find(pAddr);
			//
			if (m_Pos == m_Map.end())
			{
				return FALSE;
			}
			//
			pAddr = m_Pos->first;
			aInfo = m_Pos->second;

			return TRUE;
		};

		inline POSITION end() { 

			return m_Map.end(); 
		};

		inline void RemoveKey(LPVOID pAddr) { 

			m_Map.erase(pAddr);
		};

		inline void RemoveAll() {
			m_Map.clear();
		};

		void SetAt(LPVOID pAddr, AllocBlockInfo& aInfo) {

			m_Map[pAddr] = aInfo;
		};

		inline POSITION GetStartPosition() { 
			POSITION pos = m_Map.begin(); 
			return pos;
		};

		inline void GetNextAssoc(POSITION& pos, LPVOID& rAddr, AllocBlockInfo& aInfo) {

			rAddr = pos->first;
			aInfo = pos->second;
			pos++;
		};

		void InitHashTable(int preAllocEntries, BOOL flag) 	{
			preAllocEntries	= NULL;
			flag				= NULL;
		};

	};

	CMemLeakDetect();
	~CMemLeakDetect();
	void Init();
	void End();
	void addMemoryTrace(void* addr,  DWORD asize,  const TCHAR *fname, DWORD lnum);
	void redoMemoryTrace(void* addr,  void* oldaddr, DWORD asize,  const TCHAR *fname, DWORD lnum);
	void removeMemoryTrace(void* addr, void* realdataptr);
	void cleanupMemoryTrace();
	void dumpMemoryTrace();
	//

	//CMap<LPVOID, LPVOID, AllocBlockInfo, AllocBlockInfo> m_AllocatedMemoryList;
	CMapMem			 m_AllocatedMemoryList;
	DWORD m_dwMemOccurrence;
	bool  isLocked;
	//
private:

	BOOL initSymInfo(const TCHAR* lpUserPath);
	BOOL cleanupSymInfo();

	//@function:
	//@params  :
	//
	//
	//@return  :
	void symbolPaths(TCHAR* lpszSymbolPaths, size_t NumberOfElements);


	void symStackTrace(STACKFRAMEENTRY* pStacktrace);
	void symStackTrace2(STACKFRAMEENTRY* pStacktrace);
	BOOL symFunctionInfoFromAddresses(ULONG fnAddress, ULONG stackAddress, LPTSTR lpszSymbol, size_t ElementBufferNumber);
	BOOL symSourceInfoFromAddress(UINT address, LPTSTR lpszSourceInfo, size_t ElementBufferNumber);
	BOOL symModuleNameFromAddress(UINT address, LPTSTR lpszModule, size_t ElementBufferNumber);

	HANDLE				m_hProcess;
	PIMAGEHLP_SYMBOL	m_pSymbol;
	DWORD				m_dwsymBufSize;

	static CMemLeakDetect*	m_pMemTrace			    ;
	static _CRT_ALLOC_HOOK	m_pfnOldCrtAllocHook	;

	static int CatchMemoryAllocHook(
		int	allocType, 
		void	*userData, 
		size_t size, 
		int	blockType, 
		long	requestNumber, 
		const unsigned char	*filename, 
		int	lineNumber);

};

_declspec(selectany) CMemLeakDetect* CMemLeakDetect::m_pMemTrace          = NULL;
_declspec(selectany) _CRT_ALLOC_HOOK CMemLeakDetect::m_pfnOldCrtAllocHook = NULL;




inline void CMemLeakDetect::Init()
{
	m_dwMemOccurrence   = 0;
	m_dwsymBufSize		= (MLD_MAX_NAME_LENGTH + sizeof(PIMAGEHLP_SYMBOL));
	m_hProcess			= GetCurrentProcess();
	m_pSymbol		    = (PIMAGEHLP_SYMBOL)GlobalAlloc( GMEM_FIXED, m_dwsymBufSize);

	m_AllocatedMemoryList.InitHashTable(10211, TRUE);

	initSymInfo( NULL );

	isLocked				= false;
	m_pMemTrace 			= this;
	m_pfnOldCrtAllocHook	= _CrtSetAllocHook( CatchMemoryAllocHook ); 
}
inline void CMemLeakDetect::End()
{
	isLocked				= true;
	_CrtSetAllocHook(m_pfnOldCrtAllocHook);
	dumpMemoryTrace();
	cleanupMemoryTrace();
	//<<temp>> cleanupSymInfo();
	GlobalFree(m_pSymbol);
	m_pMemTrace				= NULL;
}


inline CMemLeakDetect::CMemLeakDetect()
{
	Init();
}

inline CMemLeakDetect::~CMemLeakDetect()
{
	End();
}

//static function 
inline int CMemLeakDetect::CatchMemoryAllocHook(int	allocType, 
												void	*userData, 
												size_t size, 
												int	blockType, 
												long	requestNumber, 
												const unsigned char	* filename, 
												int	lineNumber)
{

	USES_CONVERSION;

	 const TCHAR* tcsFileName = A2CT((const char*)filename);
	_CrtMemBlockHeader *pCrtHead;
	long prevRequestNumber;

	// internal C library internal allocations
	if ( blockType == _CRT_BLOCK )
	{
		return( TRUE );
	}

	// check if someone has turned off mem tracing
	if  ((( _CRTDBG_ALLOC_MEM_DF & _crtDbgFlag) == 0) && 
		(( allocType		== _HOOK_ALLOC)		|| 
		( allocType			== _HOOK_REALLOC)))
	{
		if (m_pfnOldCrtAllocHook)
		{
			m_pfnOldCrtAllocHook(allocType, userData, size, blockType, requestNumber, filename, lineNumber);
		}
		return TRUE;
	}


	// protect if mem trace is not initialized
	if (m_pMemTrace == NULL)
	{
		if (m_pfnOldCrtAllocHook)
		{
			m_pfnOldCrtAllocHook(allocType, userData, size, blockType, requestNumber, filename, lineNumber);
		}
		return TRUE;
	}

	// protect internal mem trace allocs
	if (m_pMemTrace->isLocked)
	{
		if (m_pfnOldCrtAllocHook)
		{
			m_pfnOldCrtAllocHook(allocType, userData, size, blockType, requestNumber, filename, lineNumber);
		}
		return( TRUE);
	}


	// lock the function
	m_pMemTrace->isLocked = true;



	switch(allocType)
	{

	case _HOOK_ALLOC://Allocate A Block Of Memory

		m_pMemTrace->addMemoryTrace((void *) requestNumber, size, tcsFileName, lineNumber);
		break;

	case _HOOK_REALLOC://Reallocate A Block of Memory
		if (_CrtIsValidHeapPointer(userData))
		{
			pCrtHead = pHdr(userData);
			prevRequestNumber = pCrtHead->lRequest;

			if (pCrtHead->nBlockUse == _IGNORE_BLOCK)
			{
				if (m_pfnOldCrtAllocHook)
				{
					m_pfnOldCrtAllocHook(allocType, userData, size, blockType, requestNumber, filename, lineNumber);
				}
				goto END;
			}
			m_pMemTrace->redoMemoryTrace((void *) requestNumber, (void *) prevRequestNumber, size, tcsFileName, lineNumber);
		}//if
		break;

	case _HOOK_FREE://Free A Block Of Memory
		if (_CrtIsValidHeapPointer(userData))
		{
			pCrtHead = pHdr(userData);
			requestNumber = pCrtHead->lRequest;

			if (pCrtHead->nBlockUse == _IGNORE_BLOCK)
			{
				if (m_pfnOldCrtAllocHook)
				{
					m_pfnOldCrtAllocHook(allocType, userData, size, blockType, requestNumber, filename, lineNumber);
				}
				goto END;
			}
			m_pMemTrace->removeMemoryTrace((void *) requestNumber, userData);
		}//if

		break;



	}//switch


END:
	// unlock the function
	m_pMemTrace->isLocked = false;

	return TRUE;
}

//@function:
//@params  :
//
//
//@return  :
inline void CMemLeakDetect::addMemoryTrace(void* addr,  DWORD asize,  const TCHAR *fname, DWORD lnum)
{

	AllocBlockInfo ainfo;
	
	if (m_AllocatedMemoryList.Lookup(addr, ainfo))
	{
		// already allocated
		MemLeakTrace(_T("ERROR!CMemLeakDetect::addMemoryTrace() Address(0x%08X) already allocated\n"), addr);
		return;
	}
	//
	ainfo.address		= addr;
	ainfo.lineNumber	= lnum;
	ainfo.size			= asize;
	ainfo.occurance		= m_dwMemOccurrence++;
	MLD_STACKWALKER(&ainfo.traceinfo[0]);
	//
	if (fname)
	{
		_tcsncpy_s(&ainfo.fileName[0], _countof(ainfo.fileName), fname, MLD_MAX_NAME_LENGTH);
	}
	else
	{
		ainfo.fileName[0] = 0;
	}
	
	m_AllocatedMemoryList.SetAt(addr, ainfo);

}

//@function:
//@params  :
//
//
//@return  :
inline void CMemLeakDetect::redoMemoryTrace(void* addr,  void* oldaddr, DWORD asize,  const TCHAR *fname, DWORD lnum)
{

	AllocBlockInfo ainfo;

	if (m_AllocatedMemoryList.Lookup(oldaddr,(AllocBlockInfo &) ainfo))
	{
		m_AllocatedMemoryList.RemoveKey(oldaddr);
	}
	else
	{
		MemLeakTrace(_T("ERROR!CMemLeakDetect::redoMemoryTrace() didnt find Address(0x%08X) to free\n"), oldaddr);
	}
	//
	ainfo.address		= addr;
	ainfo.lineNumber	= lnum;
	ainfo.size			= asize;
	ainfo.occurance		= m_dwMemOccurrence++;
	MLD_STACKWALKER(&ainfo.traceinfo[0]);

	if (fname)
	{
		_tcsncpy_s(&ainfo.fileName[0], _countof(ainfo.fileName), fname, MLD_MAX_NAME_LENGTH);
	}
	else
	ainfo.fileName[0] = 0;

	m_AllocatedMemoryList.SetAt(addr, ainfo);

}

//@function:
//@params  :
//
//
//@return  :
inline void CMemLeakDetect::removeMemoryTrace(void* addr, void* realdataptr)
{

	AllocBlockInfo ainfo;
	//
	if (m_AllocatedMemoryList.Lookup(addr,(AllocBlockInfo &) ainfo))
	{
		m_AllocatedMemoryList.RemoveKey(addr);
	}
	else
	{
		//freeing unallocated memory
		MemLeakTrace(_T("ERROR!CMemLeakDetect::removeMemoryTrace() didnt find Address(0x%08X) to free\n"), realdataptr);
	}

};

//@function:
//@params  :
//
//
//@return  :
inline void CMemLeakDetect::cleanupMemoryTrace()
{
	m_AllocatedMemoryList.RemoveAll();
};



//@function  : Initialize Symbol Information.
//@params    :
//            lpszUserSymbolPath, the additional symbol path specified by the user.
//
//@return    :
//@API Called:SymInitialize

inline BOOL CMemLeakDetect::initSymInfo(const TCHAR* lpszUserSymbolPath )
{
	USES_CONVERSION;
	TCHAR   lpszSymbolPath[MLD_MAX_NAME_LENGTH];
	DWORD   symOptions = SymGetOptions();

	symOptions |= SYMOPT_LOAD_LINES; 
	symOptions &= ~SYMOPT_UNDNAME;
	SymSetOptions( symOptions );

	// Get the search path for the symbol files
	symbolPaths( lpszSymbolPath, _countof(lpszSymbolPath));
	//
	if (lpszUserSymbolPath)
	{
		_tcscat_s(lpszSymbolPath,_countof(lpszSymbolPath),  _T(";"));
		_tcscat_s(lpszSymbolPath, _countof(lpszSymbolPath), lpszUserSymbolPath);
	}
	return SymInitialize( GetCurrentProcess(), T2A(lpszSymbolPath), TRUE);
}


//@function:prepare the symbol path
//@params  :
//
//
//@return  :

inline void CMemLeakDetect::symbolPaths(TCHAR* lpszSymbolPath, size_t NumberOfElements)
{
	TCHAR lpszPath[MLD_MAX_NAME_LENGTH];

	// Creating the default path where the dgbhelp.dll is located
	// ".;%_NT_SYMBOL_PATH%;%_NT_ALTERNATE_SYMBOL_PATH%;%SYSTEMROOT%;%SYSTEMROOT%\System32;"
	_tcscpy_s( lpszSymbolPath, NumberOfElements, _T(".;..\\;..\\..\\"));

	// environment variable _NT_SYMBOL_PATH
	if ( GetEnvironmentVariable(_T("_NT_SYMBOL_PATH"), lpszPath, MLD_MAX_NAME_LENGTH ))
	{
		_tcscat_s( lpszSymbolPath, NumberOfElements, _T(";"));
		_tcscat_s( lpszSymbolPath, NumberOfElements, lpszPath );
	}

	// environment variable _NT_ALTERNATE_SYMBOL_PATH
	if ( GetEnvironmentVariable( _T("_NT_ALTERNATE_SYMBOL_PATH"), lpszPath, MLD_MAX_NAME_LENGTH ))
	{
		_tcscat_s( lpszSymbolPath, NumberOfElements, _T(";")  );
		_tcscat_s( lpszSymbolPath, NumberOfElements, lpszPath );
	}

	// environment variable SYSTEMROOT
	if ( GetEnvironmentVariable( _T("SYSTEMROOT"), lpszPath, MLD_MAX_NAME_LENGTH ) )
	{
		_tcscat_s( lpszSymbolPath, NumberOfElements, _T(";"));
		_tcscat_s( lpszSymbolPath, NumberOfElements, lpszPath);
		_tcscat_s( lpszSymbolPath, NumberOfElements, _T(";"));

		// SYSTEMROOT\System32
		_tcscat_s( lpszSymbolPath, NumberOfElements, lpszPath );
		_tcscat_s( lpszSymbolPath, NumberOfElements, _T("\\System32"));
	}
}



inline void CMemLeakDetect::symStackTrace(STACKFRAMEENTRY* pStacktrace )
{
	STACKFRAME     callStack;
	BOOL           bResult;
	CONTEXT        context;
	HANDLE		   hThread  = GetCurrentThread();

	// get the context
	memset( &context, NULL, sizeof(context) );
	context.ContextFlags = CONTEXT_FULL;

	//MSDN:If you call GetThreadContext for the current thread, the function returns successfully; 
	//however, the context returned is not valid.
	//
	if ( !GetThreadContext( hThread, &context ) )
	{
       MemLeakTrace(_T("Call stack info(thread=0x%X) failed.\n"), hThread );
	   return;
	}
	//initialize the call stack
	memset( &callStack, NULL, sizeof(callStack) );
	callStack.AddrPC.Offset    = context.Eip;
	callStack.AddrStack.Offset = context.Esp;
	callStack.AddrFrame.Offset = context.Ebp;
	callStack.AddrPC.Mode      = AddrModeFlat;
	callStack.AddrStack.Mode   = AddrModeFlat;
	callStack.AddrFrame.Mode   = AddrModeFlat;
	//
	for( DWORD index = 0; index < MLD_MAX_TRACEINFO; index++ ) 
	{
		bResult = StackWalk(IMAGE_FILE_MACHINE_I386,
							m_hProcess,
							hThread,
							&callStack,
							NULL, 
							NULL,
							SymFunctionTableAccess,
							SymGetModuleBase,
							NULL);

		//if ( index == 0 )
		 //  continue;

		if( !bResult || callStack.AddrFrame.Offset == 0 ) 
		{
			if(bResult == FALSE)
			{
				//MemLeakTrace(_T("StackWalk Return FALSE.\n"));
			}

			break;
		}
		//
		pStacktrace[0].addrPC	 = callStack.AddrPC;
		pStacktrace[0].addrFrame = callStack.AddrFrame;
		pStacktrace++;
	}
	//clear the last entry
	memset(pStacktrace, NULL, sizeof(STACKFRAMEENTRY));
}

inline void CMemLeakDetect::dumpMemoryTrace()
{

	POSITION			pos;
	LPVOID				addr;
	AllocBlockInfo		ainfo;
	TCHAR				buf[MLD_MAX_NAME_LENGTH];
	TCHAR				symInfo[MLD_MAX_NAME_LENGTH];
	TCHAR				srcInfo[MLD_MAX_NAME_LENGTH];
	int					totalSize						= 0;
	int					numLeaks						= 0;
	STACKFRAMEENTRY*	p								= 0;

	//
	_tcscpy_s(symInfo, _countof(symInfo), MLD_TRACEINFO_NOSYMBOL);
	_tcscpy_s(srcInfo, _countof(symInfo), MLD_TRACEINFO_NOSYMBOL);
	//
	pos = m_AllocatedMemoryList.GetStartPosition();
	//
	while(pos != m_AllocatedMemoryList.end())
	{
		numLeaks++;
		_stprintf_s(buf,_countof(buf), _T("[MLD]Memory Leak(%d)------------------->\n"), numLeaks);
		
		MemLeakTrace(buf);


		m_AllocatedMemoryList.GetNextAssoc(pos, (LPVOID &) addr, (AllocBlockInfo&) ainfo);

		if (ainfo.fileName[0] != NULL)
		{
			_stprintf_s(buf, _countof(buf),_T("[MLD]Memory Leak <0x%X> bytes(%d) occurance(%d) %s(%d)\n"), 
					ainfo.address, ainfo.size, ainfo.occurance, ainfo.fileName, ainfo.lineNumber);
		}
		else
		{
			_stprintf_s(buf, _countof(buf), _T("[MLD]Memory Leak <0x%X> bytes(%d) occurance(%d)\n"), 
					ainfo.address, ainfo.size, ainfo.occurance);
		}

		MemLeakTrace(buf);

		p = &ainfo.traceinfo[0];
		while(p[0].addrPC.Offset)
		{
			//dump the stack information
			symFunctionInfoFromAddresses( p[0].addrPC.Offset, p[0].addrFrame.Offset, symInfo, _countof(symInfo) );
			symSourceInfoFromAddress(     p[0].addrPC.Offset, srcInfo, _countof(srcInfo) );
			//MemLeakTrace(_T("%s->%s()\n"), srcInfo, symInfo);
			p++;
		}
		totalSize += ainfo.size;
	}
	_stprintf_s(buf,_countof(buf),_T("\n[MLD]-----------------------------------------------------------\n"));
	MemLeakTrace(buf);
	if(!totalSize) 
	{
		_stprintf_s(buf,_countof(buf),_T("[MLD]No Memory Leaks Detected for %d Allocations\n\n"), m_dwMemOccurrence);
		MemLeakTrace(buf);
	}
	else
	{
		_stprintf_s(buf,_countof(buf),_T("[MLD]Total %d Memory Leaks: %d bytes Total Alocations %d\n\n"), numLeaks, totalSize, m_dwMemOccurrence);
		MemLeakTrace(buf);
	}
	
}



inline BOOL CMemLeakDetect::symFunctionInfoFromAddresses( ULONG fnAddress, ULONG stackAddress, LPTSTR lpszSymbol, size_t ElementBufferNumber)
{
	USES_CONVERSION;
	DWORD             dwDisp	= 0;

	::ZeroMemory(m_pSymbol, m_dwsymBufSize );
	m_pSymbol->SizeOfStruct		= m_dwsymBufSize;
	m_pSymbol->MaxNameLength	= m_dwsymBufSize - sizeof(IMAGEHLP_SYMBOL);

    // Set the default to unknown
	_tcscpy_s( lpszSymbol, ElementBufferNumber, MLD_TRACEINFO_NOSYMBOL);

	// Get symbol info for IP
	if ( SymGetSymFromAddr( m_hProcess, (ULONG)fnAddress, &dwDisp, m_pSymbol ) )
	{
		_tcscpy_s(lpszSymbol, ElementBufferNumber, A2CT(m_pSymbol->Name));
		return TRUE;
	}
	//create the symbol using the address because we have no symbol
	_stprintf_s(lpszSymbol, ElementBufferNumber, _T("0x%08X"), fnAddress);
	return FALSE;
}

inline  BOOL CMemLeakDetect::symSourceInfoFromAddress(UINT address, TCHAR* lpszSourceInfo, size_t ElementBufferNumber)
{
	USES_CONVERSION;
	BOOL           ret = FALSE;
	IMAGEHLP_LINE  lineInfo;
	DWORD          dwDisp;
	TCHAR          lpModuleInfo[MLD_MAX_NAME_LENGTH] = MLD_TRACEINFO_EMPTY;

	_tcscpy_s( lpszSourceInfo, ElementBufferNumber, MLD_TRACEINFO_NOSYMBOL);

	memset( &lineInfo, NULL, sizeof( IMAGEHLP_LINE ) );
	lineInfo.SizeOfStruct = sizeof( IMAGEHLP_LINE );

	if ( SymGetLineFromAddr( m_hProcess, address, &dwDisp, &lineInfo ) )
	{
	   // Using the "sourcefile(linenumber)" format
		_stprintf_s( lpszSourceInfo, ElementBufferNumber, _T("%s(%d): 0x%08X"), A2CT(lineInfo.FileName), lineInfo.LineNumber, address );
		ret = TRUE;
	}
	else
	{
        // Using the "modulename!address" format
	  	symModuleNameFromAddress( address, lpModuleInfo, _countof(lpModuleInfo));

		if ( lpModuleInfo[0] == _T('?') || lpModuleInfo[0] == _T('\0'))
		{
			// Using the "address" format
			_stprintf_s(lpszSourceInfo, ElementBufferNumber, _T("0x%08X"), lpModuleInfo, address );
		}
		else
		{
			_stprintf_s(lpszSourceInfo, ElementBufferNumber, _T("%sdll! 0x%08X"), lpModuleInfo, address );
		}
		ret = FALSE;
	}
	//
	return ret;
}

inline  BOOL CMemLeakDetect::symModuleNameFromAddress( UINT address, TCHAR* lpszModule, size_t ElementBufferNumber)
{
	USES_CONVERSION;
	BOOL              ret = FALSE;
	IMAGEHLP_MODULE   moduleInfo;

	::ZeroMemory( &moduleInfo, sizeof(IMAGEHLP_MODULE) );
	moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);

	if ( SymGetModuleInfo( m_hProcess, (DWORD)address, &moduleInfo ) )
	{
		//_tcscpy(moduleInfo.ModuleName, lpszModule);
		_tcscpy_s(lpszModule, ElementBufferNumber, A2T(moduleInfo.ModuleName));
		ret = TRUE;
	}
	else
	{
		_tcscpy_s( lpszModule, ElementBufferNumber, MLD_TRACEINFO_NOSYMBOL);
	}
	
	return ret;
}

// This code is still under investigation
// I have to test this code and make sure it is compatible
// with the other stack walker!
//
void CMemLeakDetect::symStackTrace2(STACKFRAMEENTRY* pStacktrace )
{
	ADDR			FramePtr				= NULL;
	ADDR			InstructionPtr			= NULL;
	ADDR			OriFramePtr				= NULL;
	ADDR			PrevFramePtr			= NULL;
	long			StackIndex				= NULL;

	// Get frame pointer
	_asm mov DWORD PTR [OriFramePtr], ebp

	FramePtr = OriFramePtr;

	//
	while (FramePtr)
	{
		InstructionPtr = ((ADDR *)FramePtr)[1];

		pStacktrace[StackIndex].addrPC.Offset	= InstructionPtr;
		pStacktrace[StackIndex].addrPC.Segment	= NULL;
		pStacktrace[StackIndex].addrPC.Mode		= AddrModeFlat;
		//
		StackIndex++;
		PrevFramePtr			= FramePtr;
		FramePtr				= ((ADDR *)FramePtr)[0];
	}
}


class CMemLeakDetect_Init
{
public:
	static CMemLeakDetect s_oMemLeakDetect;
};
_declspec(selectany) CMemLeakDetect CMemLeakDetect_Init::s_oMemLeakDetect;

static CMemLeakDetect_Init s_MemLeakDetect_Init;



#endif