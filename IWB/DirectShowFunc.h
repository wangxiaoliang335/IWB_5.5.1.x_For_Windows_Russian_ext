#pragma once
HRESULT FindPin(IBaseFilter* pFilter, PIN_DIRECTION pindir, LPCWSTR pPinName, IPin** ppPin);

//@功  能:根据指定的设备路径查找视频捕获设备
//@参  数:ppSrcFilter, 指向保存"捕获滤器(capture filter)"对象指针的内存地址。
//       lpszDevPath, 设备路径。
//@返回值:S_OK, 成功。
//        其他, 失败
//@说明:如果设备路径为空, 则返回第一个找到的视频捕获设备滤器
// 
HRESULT FindVideoCaptureDevice(IBaseFilter **ppSrcFilter, LPCWSTR lpszDevPath=NULL);


HRESULT  PrintFilterGraph(IFilterGraph* pFG);
HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
void RemoveFromRot(DWORD pdwRegister);

 void PrintObjRef(IUnknown* pUnk);

HRESULT GetNextFilter(
    IBaseFilter *pFilter, // Pointer to the starting filter
    PIN_DIRECTION Dir,    // Direction to search (upstream or downstream)
    IBaseFilter **ppNext); // Receives a pointer to the next filter.

// Define a typedef for a list of filters.
typedef CGenericList<IBaseFilter> CFilterList;
//Find all the immediate upstream or downstream peers of a filter.	
HRESULT GetPeerFilters(
    IBaseFilter *pFilter,     // Pointer to the starting filter
    PIN_DIRECTION Dir,        // Direction to search (upstream or downstream)
    CFilterList &FilterList); // Collect the results in this list.


//@功能: 读取VideoProcAmp的属性值
//@参数:pCaptureFilter, 指向Video Capture Filter对象的指针 
//      pVideoProcAmpProp, 指向保存VideoProcAmp属性的数据缓冲区的指针
BOOL ReadVideoProcAmpProperty(IBaseFilter *pCaptureFilter, TVideoProcAmpProperty* pVideoProcAmpProp);


//@功能: 设置VideoProcAmp的属性值
//@参数:pCaptureFilter, 指向Video Capture Filter对象的指针 
//      pVideoProcAmpProp, 指向设置VideoProcAmp属性的数据缓冲区的指针
BOOL SetVideoProcAmpProperty(IBaseFilter *pCaptureFilter, const TVideoProcAmpProperty* pVideoProcAmpProp);
BOOL IRCUTSwtich(IBaseFilter *pCaptureFilter,BOOL bOn,int PID ,int VID);

//@功能:GPIO控制
BOOL GPIOControl(IBaseFilter *pCaptureFilter, BOOL bOn);