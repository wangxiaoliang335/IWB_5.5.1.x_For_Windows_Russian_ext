#pragma once
#ifdef _DEBUG

//inline void* _cdecl operator new(size_t nSize, LPCSTR lpszFileName, int nLine)
//{
//		void* pResult = _malloc_dbg(nSize, _NORMAL_BLOCK, lpszFileName, nLine);
//		return pResult;
//}

#define MLD_NEW new(__FILE__, __LINE__)

#define new MLD_NEW

#endif 