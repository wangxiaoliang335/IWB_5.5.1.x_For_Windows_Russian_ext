#include "stdafx.h"
//#include "headers.h"
CInterceptInputPin::CInterceptInputPin(
    TCHAR *pObjectName,
    CTransformFilter *pTransformFilter,
    HRESULT * phr,
    LPCWSTR pName)
    : CTransformInputPin(pObjectName, pTransformFilter, phr, pName)
{

}

CInterceptInputPin::~CInterceptInputPin()
{


}

/* Tell the input pin which allocator the output pin is actually going to use
   Override this if you care - NOTE the locking we do both here and also in
   GetAllocator is unnecessary but derived classes that do something useful
   will undoubtedly have to lock the object so this might help remind people */
STDMETHODIMP
CInterceptInputPin::NotifyAllocator(
    IMemAllocator * pAllocator,
    BOOL bReadOnly)
{
    CheckPointer(pAllocator,E_POINTER);
    ValidateReadPtr(pAllocator,sizeof(IMemAllocator));
    CAutoLock cObjectLock(m_pLock);

    IMemAllocator *pOldAllocator = m_pAllocator;
    pAllocator->AddRef();
    m_pAllocator = pAllocator;

    if (pOldAllocator != NULL) {
        pOldAllocator->Release();
    }

    // the readonly flag indicates whether samples from this allocator should
    // be regarded as readonly - if true, then inplace transforms will not be
    // allowed.

    // <inserted_code>
	ALLOCATOR_PROPERTIES Props, Actual;
	// get the current settings
	HRESULT hr = m_pAllocator->GetProperties(&Props);

    if(hr == S_OK)
    {
	    LOG_INF("CInterceptInputPin::NotifyAllocator{Original Buffer Number=0x%x, BufferSize=0x%x, Align=%d}",Props.cBuffers, Props.cbBuffer, Props.cbAlign);
    }


	Props.cBuffers = 3;
	Props.cbAlign = 64;

	hr = m_pAllocator->SetProperties(&Props, &Actual);

    if(hr == S_OK)
    {
	    LOG_INF("CInterceptInputPin::NotifyAllocator{New Buffer Number=0x%x, BufferSize=0x%x, Align=%d}",Actual.cBuffers, Actual.cbBuffer, Actual.cbAlign);
    }


    m_bReadOnly = (BYTE)bReadOnly;
    return NOERROR;
}

