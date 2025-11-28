#pragma once 
#include <map>
#include <utility>
#include <atlbase.h>
//@说明:MF2GF模板函数将类的成员函数转换为_stdcall调用类型的全局函数
//
//
//成员函数的thunk映射类
class CMemberFunctionThunkMap
{

protected:	

#pragma pack(push, 1)
    template<class MemberFunc, class T>
    class thunk_key
    {
    public:
        //typedef classType::(*f)(P1 p1);
        thunk_key(MemberFunc memFunc,  const T* pThis)
        {
            m_tMemFunc = memFunc;
            m_pThis     = pThis ;
        }

    protected:
        MemberFunc  m_tMemFunc;//成员函数
        const T* m_pThis   ;//对象指针
        
        
    };

    typedef struct _map_key
    {

        bool operator < (const _map_key &right)const
        {
            if(lpObj > right.lpObj) return false;

			if(lpObj == right.lpObj)
			{
				if(lpMemFunc >= right.lpMemFunc) return false;
			}
            return true;
        }
        ULONG_PTR lpMemFunc;//
        ULONG_PTR lpObj    ;//
    }map_key;

    //
    typedef struct 
    {
        BYTE   movecx;//
        PVOID  lpThis;//
        BYTE   jmp   ;//
        ULONG  rel   ;//
    }thunk_data;


#pragma pack(pop)
    CMemberFunctionThunkMap()
    {

    }

    ~CMemberFunctionThunkMap()
    {
        CComCritSecLock<CComAutoCriticalSection> lock(m_csForThunkMap);
        for(std::map<map_key, LPVOID>::iterator it = m_mapThunk.begin (); it != m_mapThunk.end(); it++)
        {
            VirtualFree(it->second, 0, MEM_RELEASE);
        }

    }
public:

    //@功  能:
    //@参  数:memFunc, 成员函数地址
    //        pObj,对象地址
    //@返回值:
    //
    //
    //


        template<class memberFuncType, class classType> 
    ULONG_PTR GetThunk(memberFuncType memFunc , classType* pObj)
    {

			CComCritSecLock<CComAutoCriticalSection> lock(m_csForThunkMap);
            thunk_data* pThunkData = NULL;

            thunk_key<memberFuncType, classType> tk(memFunc, pObj);
        
            map_key mk = *reinterpret_cast<map_key*>(&tk);

             if( m_mapThunk.find(mk) == m_mapThunk.end())
             {

                    pThunkData  = (thunk_data*)VirtualAlloc(NULL,
                                                      sizeof(thunk_data),
                                                      MEM_COMMIT,
                                                     PAGE_EXECUTE_READWRITE);

                    pThunkData->movecx = 0xb9;
                    pThunkData->lpThis = (PVOID)mk.lpObj;
                    pThunkData->jmp    = 0xe9;
                    pThunkData->rel    = (ULONG)((INT_PTR)mk.lpMemFunc - ((INT_PTR)pThunkData  + sizeof(thunk_data)));

                    FlushInstructionCache(GetCurrentProcess(), pThunkData, sizeof(thunk_data));
                    m_mapThunk[mk] = pThunkData;
             }
			 else
			 {
				pThunkData = (thunk_data*) m_mapThunk[mk]; 
			 }

            return (ULONG_PTR)pThunkData;
    }


template<class memberFuncType, class classType> 
    void FreeThunk(memberFuncType memFunc , classType* pObj)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csForThunkMap);
		thunk_key<memberFuncType, classType> tk(memFunc, pObj);
        map_key mk = *reinterpret_cast<map_key*>(&tk);
		std::map<map_key, LPVOID>::iterator it = m_mapThunk.find(mk);

		if(it != m_mapThunk.end())
		{
			m_mapThunk.erase(it);
		}

	}


    
    static CMemberFunctionThunkMap& GetSingleInstance()
    {
        static CMemberFunctionThunkMap m_oThunkMap;

        return m_oThunkMap;
    }
private:
    std::map<map_key, LPVOID> m_mapThunk;
	CComAutoCriticalSection   m_csForThunkMap;
     
};


template<class GFType, class classType,  class S>
GFType	MF2GF(S   (classType::*f)()/*, classType* pObj*/)
{
    GFType p = NULL;


    p = (GFTYPE)CMemberFunctionThunkMap((ULONG_PTR)p1, (ULONG_PTR)pObj);

    return p;
}


template<class GFType, class classType,  class S, class P1>
GFType	MF2GF<>  (S   (classType::*f)(P1 p1), classType* pObj)
{
    GFType p = NULL;

    ULONG_PTR pThunk = CMemberFunctionThunkMap::GetSingleInstance().GetThunk(f, pObj);

    p = (GFType)pThunk;
    return p;
}


template<class GFType, class classType,  class S, class P1, class P2>
GFType	MF2GF<>  (S   (classType::*f)(P1 p1, P2 p2), classType* pObj)
{
    GFType p = NULL;

    ULONG_PTR pThunk = CMemberFunctionThunkMap::GetSingleInstance().GetThunk(f, pObj);

    p = (GFType)pThunk;
    return p;
}


template<class GFType, class classType,  class S, class P1, class P2, class P3>
GFType	MF2GF<>  (S   (classType::*f)(P1 p1, P2 p2, P3 p3), classType* pObj)
{
    GFType p = NULL;

    ULONG_PTR pThunk = CMemberFunctionThunkMap::GetSingleInstance().GetThunk(f, pObj);

    p = (GFType)pThunk;
    return p;
}


template<class GFType, class classType,  class S, class P1, class P2, class P3, class P4>
GFType	MF2GF<>  (S   (classType::*f)(P1 p1, P2 p2, P3 p3, P4 p4), classType* pObj)
{
    GFType p = NULL;

    ULONG_PTR pThunk = CMemberFunctionThunkMap::GetSingleInstance().GetThunk(f, pObj);

    p = (GFType)pThunk;
    return p;
}




template<class classType,  class S>
void FreeThunk(S   (classType::*f)()/*, classType* pObj*/)
{
    GFType p = NULL;


    p = (GFTYPE)CMemberFunctionThunkMap((ULONG_PTR)p1, (ULONG_PTR)pObj);

    return p;
}


template<class classType,  class S, class P1>
void	FreeThunk<>  (S   (classType::*f)(P1 p1), classType* pObj)
{
    CMemberFunctionThunkMap::GetSingleInstance().FreeThunk(f, pObj);



}


template<class classType,  class S, class P1, class P2>
void    FreeThunk<>  (S   (classType::*f)(P1 p1, P2 p2), classType* pObj)
{
    CMemberFunctionThunkMap::GetSingleInstance().FreeThunk(f, pObj);
}


template<class classType,  class S, class P1, class P2, class P3>
void  FreeThunk<>  (S   (classType::*f)(P1 p1, P2 p2, P3 p3), classType* pObj)
{
    CMemberFunctionThunkMap::GetSingleInstance().FreeThunk(f, pObj);
}


template<class classType,  class S, class P1, class P2, class P3, class P4>
void	FreeThunk<>  (S   (classType::*f)(P1 p1, P2 p2, P3 p3, P4 p4), classType* pObj)
{
    CMemberFunctionThunkMap::GetSingleInstance().FreeThunk(f, pObj);
}