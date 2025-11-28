#pragma once
class CInterceptInputPin:public CTransformInputPin
{
	friend class CTransformFilter;
public:
	CInterceptInputPin(
    TCHAR *pObjectName,
    CTransformFilter *pTransformFilter,
    HRESULT * phr,
    LPCWSTR pName);


	virtual ~CInterceptInputPin();

	STDMETHODIMP NotifyAllocator(
    IMemAllocator * pAllocator,
    BOOL bReadOnly);

	 CBasePin * CInterceptInputPin::GetPin(int n);

};