#include "stdafx.h"
//#include "headers.h"
//IKsTopologyInfo, ISelector and IKsNodeControl are defined in Vidcap.h
#include <vidcap.h>
#include <Ksmedia.h>

//IKsControl
#include <Ksproxy.h>
#pragma comment(lib,"Ksproxy.lib")

#include <processthreadsapi.h>

static const int SONIX_PID = 0x9230;
static const int SONIX_VID = 0x05a3;
HRESULT FindPin(IBaseFilter* pFilter, PIN_DIRECTION pindir, LPCWSTR pPinName, IPin** ppPin)
{
    HRESULT hr = S_FALSE;
    IPin* pPin = NULL;

    if (pFilter == NULL) return E_INVALIDARG;

    BOOL bFound = FALSE;

    CComPtr<IEnumPins> ptrEnumPins;
    ULONG cFetched = 1;

    hr = pFilter->EnumPins(&ptrEnumPins);

    if (FAILED(hr))
    {
        return hr;
    }

    //Find source output Pin
    while (!bFound)
    {

        HRESULT  hrEnum = ptrEnumPins->Next(1, &pPin, &cFetched);
        if (FAILED(hrEnum))
        {
            if (S_FALSE != hrEnum)
            {
                hr = hrEnum;
            }

            break;
        }

        PIN_INFO pinInfo;

        LPWSTR pwId;
        pPin->QueryId(&pwId);
        CoTaskMemFree(pwId);

        if (S_OK == pPin->QueryPinInfo(&pinInfo))
        {

            if (pinInfo.pFilter) pinInfo.pFilter->Release();

            if (pinInfo.dir == pindir)
            {
                if (pPinName)
                {

                    if (wcsncmp(pinInfo.achName, pPinName, wcslen(pPinName)) == 0)
                    {
                        bFound = TRUE;
                    }
                }
                else
                {
                    bFound = TRUE;
                }
            }
        }

        if (!bFound)
        {
            pPin->Release();
            pPin = NULL;
        }
        else
        {
            if (ppPin)
            {
                *ppPin = pPin;
            }
            hr = S_OK;
        }

    }//while


    return hr;

}


//@功  能:根据指定的设备路径查找视频捕获设备
//@参  数:ppSrcFilter, 指向保存"捕获滤器(capture filter)"对象指针的内存地址。
//       lpszDevPath, 设备路径。
//@返回值:S_OK, 成功。
//        其他, 失败
//@说明:如果设备路径为空, 则返回第一个找到的视频捕获设备滤器
HRESULT FindVideoCaptureDevice(IBaseFilter **ppSrcFilter, LPCWSTR lpszDevPath)
{
    HRESULT hr = S_OK;
    IBaseFilter * pSrc = NULL;
    CComPtr <IMoniker> pMoniker = NULL;
    ULONG cFetched;

    if (!ppSrcFilter)
        return E_POINTER;

    // Create the system device enumerator  
    CComPtr <ICreateDevEnum> pDevEnum = NULL;

    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
        IID_ICreateDevEnum, (void **)&pDevEnum);
    if (FAILED(hr))
    {
        return hr;
    }

    // Create an enumerator for the video capture devices
    CComPtr <IEnumMoniker> pClassEnum = NULL;

    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
    if (FAILED(hr))
    {
        return hr;
    }

    // If there are no enumerators for the requested type, then 
    // CreateClassEnumerator will succeed, but pClassEnum will be NULL.
    if (pClassEnum == NULL)
    {
        //       MessageBox(m_hApp,TEXT("No video capture device was detected.\r\n\r\n")
        //                TEXT("This sample requires a video capture device, such as a USB WebCam,\r\n")
        //                TEXT("to be installed and working properly.  The sample will now close."),
        //                TEXT("No Video Capture Hardware"), MB_OK | MB_ICONINFORMATION);
        return E_FAIL;
    }

    // Use the first video capture device on the device list.
    // Note that if the Next() call succeeds but there are no monikers,
    // it will return S_FALSE (which is not a failure).  Therefore, we
    // check that the return code is S_OK instead of using SUCCEEDED() macro.

    //To Find the Analog Capture Devices....

    BOOL         Found;
    IPin        *pP = 0;
    IEnumPins   *pins = 0;
    //ULONG        n;
    //PIN_INFO     pinInfo;
    Found = FALSE;
    IKsPropertySet *pKs = 0;
    //GUID guid;
    //DWORD dw;
    BOOL fMatch = FALSE;
    CComPtr<IPropertyBag> ptrPropBag;

    while (S_OK == (pClassEnum->Next(1, &pMoniker, &cFetched)))
    {

        CComPtr<IPropertyBag> ptrPropBag;

        hr = pMoniker->BindToStorage(
            0,//bind context
            0,//
            IID_IPropertyBag,
            (VOID**)&ptrPropBag);


        VARIANT varDevPath;
        VariantInit(&varDevPath);
        hr = ptrPropBag->Read(L"DevicePath", &varDevPath, 0);

        if (lpszDevPath != NULL)
        {
            if (_wcsicmp(varDevPath.bstrVal, lpszDevPath) != 0)
            {
                pMoniker = NULL;
                continue;
            }

        }

        VARIANT varName;
        VariantInit(&varName);
        hr = ptrPropBag->Read(L"FriendlyName", &varName, 0);


        // Bind Moniker to a filter object
        hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pSrc);
        if (FAILED(hr))
        {
            return hr;
        }

        break;//结束循环退出

    }//while

    // Copy the found filter pointer to the output parameter.
    // Do NOT Release() the reference, since it will still be used
    // by the calling function.
    if (ppSrcFilter)
    {
        *ppSrcFilter = pSrc;
    }

    return hr;
}



HRESULT  PrintFilterGraph(IFilterGraph* pFG)
{
    if (pFG == NULL) return E_POINTER;
    CComPtr<IEnumFilters> ptrEnumFilters;
    HRESULT hr = S_OK;
    hr = pFG->EnumFilters(&ptrEnumFilters);
    if (hr != S_OK)
    {
        return hr;
    }

    //CComPtr<IBaseFilter> ptrFilter;

    IBaseFilter* ptrFilter = NULL;

    ULONG cFetched = 0;

    while (ptrEnumFilters->Next(1, &ptrFilter, &cFetched) == S_OK)
    {

        CLSID clsId;
        ptrFilter->GetClassID(&clsId);
        FILTER_INFO info;
        ptrFilter->QueryFilterInfo(&info);

        AtlTrace(_T("0x%x\t"), (IBaseFilter*)ptrFilter);
#ifdef _DEBUG

        _CrtDbgReportW(
            _CRT_WARN, //Report Type
            NULL,      //File name
            0,         //Line number
            NULL,      //Module name
            L"Name:%s, CLSID:",
            info.achName);
#endif
        //AtlTrace(_T("Name:%s CLSID:"), (LPCTSTR)CW2TEX<>(info.achName, CP_UTF8));

        // {BAB309FD-24AF-4abb-AC8D-B3D6E5BCEBFF}
        if (clsId == CLSID_VideoRendererDefault)
        {
            AtlTrace("CLSID_VideoRendererDefault\r\n");
        }
        else if (clsId == CLSID_VideoRenderer)
        {
            AtlTrace("CLSID_VideoRenderer");
        }
        else if (clsId == CLSID_SmartTee)
        {
            AtlTrace("CLSID_SmartTee");
        }
        else if (clsId == CLSID_AVIDec)
        {
            AtlTrace("CLSID_AVIDec");
        }
        else
        {
            AtlTrace(
                _T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
                clsId.Data1,
                clsId.Data2,
                clsId.Data3,
                clsId.Data4[0],
                clsId.Data4[1],
                clsId.Data4[2],
                clsId.Data4[3],
                clsId.Data4[4],
                clsId.Data4[5],
                clsId.Data4[6],
                clsId.Data4[7]);
        }

        if (info.pGraph) info.pGraph->Release();

        ULONG ref = ptrFilter->Release();
        AtlTrace(_T("\t ref=%d\r\n"), ref);


        ptrFilter = NULL;
    }//while

    return hr;
}



HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister)
{

    IMoniker * pMoniker = NULL;
    IRunningObjectTable *pROT = NULL;

    if (FAILED(GetRunningObjectTable(0, &pROT)))
    {
        return E_FAIL;
    }

    const size_t STRING_LENGTH = 256;

    WCHAR wsz[STRING_LENGTH];


    StringCchPrintfW(
        wsz, STRING_LENGTH,
        L"FilterGraph %08x pid %08x",
        (DWORD_PTR)pUnkGraph,
        GetCurrentProcessId()
    );

    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr))
    {
        hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,
            pMoniker, pdwRegister);
        pMoniker->Release();
    }
    pROT->Release();

    return hr;
}

void RemoveFromRot(DWORD pdwRegister)
{
    IRunningObjectTable *pROT;
    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}


//Get the first upstream or downstream filter
HRESULT GetNextFilter(
    IBaseFilter *pFilter, // Pointer to the starting filter
    PIN_DIRECTION Dir,    // Direction to search (upstream or downstream)
    IBaseFilter **ppNext) // Receives a pointer to the next filter.
{
    if (!pFilter || !ppNext) return E_POINTER;

    IEnumPins *pEnum = 0;
    IPin *pPin = 0;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr)) return hr;
    while (S_OK == pEnum->Next(1, &pPin, 0))
    {
        // See if this pin matches the specified direction.
        PIN_DIRECTION ThisPinDir;
        hr = pPin->QueryDirection(&ThisPinDir);
        if (FAILED(hr))
        {
            // Something strange happened.
            hr = E_UNEXPECTED;
            pPin->Release();
            break;
        }
        if (ThisPinDir == Dir)
        {
            // Check if the pin is connected to another pin.
            IPin *pPinNext = 0;
            hr = pPin->ConnectedTo(&pPinNext);
            if (SUCCEEDED(hr))
            {
                // Get the filter that owns that pin.
                PIN_INFO PinInfo;
                hr = pPinNext->QueryPinInfo(&PinInfo);
                pPinNext->Release();
                pPin->Release();
                pEnum->Release();
                if (FAILED(hr) || (PinInfo.pFilter == NULL))
                {
                    // Something strange happened.
                    return E_UNEXPECTED;
                }
                // This is the filter we're looking for.
                *ppNext = PinInfo.pFilter; // Client must release.
                return S_OK;
            }
        }
        pPin->Release();
    }
    pEnum->Release();
    // Did not find a matching filter.
    return E_FAIL;
}



// Forward declaration. Adds a filter to the list unless it's a duplicate.
void AddFilterUnique(CFilterList &FilterList, IBaseFilter *pNew);

// Find all the immediate upstream or downstream peers of a filter.
HRESULT GetPeerFilters(
    IBaseFilter *pFilter, // Pointer to the starting filter
    PIN_DIRECTION Dir,    // Direction to search (upstream or downstream)
    CFilterList &FilterList)  // Collect the results in this list.
{
    if (!pFilter) return E_POINTER;

    IEnumPins *pEnum = 0;
    IPin *pPin = 0;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr)) return hr;
    while (S_OK == pEnum->Next(1, &pPin, 0))
    {
        // See if this pin matches the specified direction.
        PIN_DIRECTION ThisPinDir;
        hr = pPin->QueryDirection(&ThisPinDir);
        if (FAILED(hr))
        {
            // Something strange happened.
            hr = E_UNEXPECTED;
            pPin->Release();
            break;
        }
        if (ThisPinDir == Dir)
        {
            // Check if the pin is connected to another pin.
            IPin *pPinNext = 0;
            hr = pPin->ConnectedTo(&pPinNext);
            if (SUCCEEDED(hr))
            {
                // Get the filter that owns that pin.
                PIN_INFO PinInfo;
                hr = pPinNext->QueryPinInfo(&PinInfo);
                pPinNext->Release();
                if (FAILED(hr) || (PinInfo.pFilter == NULL))
                {
                    // Something strange happened.
                    pPin->Release();
                    pEnum->Release();
                    return E_UNEXPECTED;
                }
                // Insert the filter into the list.
                AddFilterUnique(FilterList, PinInfo.pFilter);
                PinInfo.pFilter->Release();
            }
        }
        pPin->Release();
    }
    pEnum->Release();
    return S_OK;
}

void AddFilterUnique(CFilterList &FilterList, IBaseFilter *pNew)
{
    if (pNew == NULL) return;

    POSITION pos = FilterList.GetHeadPosition();
    while (pos)
    {
        IBaseFilter *pF = FilterList.GetNext(pos);
        if (IsEqualObject(pF, pNew))
        {
            return;
        }
    }
    pNew->AddRef();  // The caller must release everything in the list.
    FilterList.AddTail(pNew);
}


void PrintObjRef(IUnknown* pUnk)
{
    if (pUnk == NULL) return;

    pUnk->AddRef();

    ULONG ref = pUnk->Release();

    AtlTrace(_T("0x%x reference count=%d\r\n"), pUnk, ref);

}

//@功能: 读取VideoProcAmp的属性值
//@参数:pCaptureFilter, 指向Video Capture Filter对象的指针 
//      pVideoProcAmpProp, 指向保存VideoProcAmp属性的数据缓冲区的指针
BOOL ReadVideoProcAmpProperty(IBaseFilter *pCaptureFilter, TVideoProcAmpProperty* pVideoProcAmpProp)
{
    BOOL bRet = TRUE;

    if (pCaptureFilter == NULL || pVideoProcAmpProp == NULL) return FALSE;

    CComPtr<IAMVideoProcAmp> ptrAMVideoProcAmp;

    HRESULT hr = pCaptureFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&ptrAMVideoProcAmp);

    if (hr == S_OK)
    {
        do
        {
            LONG lVal;
            VideoProcAmpFlags flags;
            //1.Brightness
            hr = ptrAMVideoProcAmp->Get(VideoProcAmp_Brightness, &lVal, (long*)&flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }
            else
            {
                pVideoProcAmpProp->Prop_VideoProcAmp_Brightness = lVal;
            }

            //2.Contrast
            hr = ptrAMVideoProcAmp->Get(VideoProcAmp_Contrast, &lVal, (long*)&flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }
            else
            {
                pVideoProcAmpProp->Prop_VideoProcAmp_Contrast = lVal;
            }

            //3.Hue
            hr = ptrAMVideoProcAmp->Get(VideoProcAmp_Hue, &lVal, (long*)&flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }
            else
            {
                pVideoProcAmpProp->Prop_VideoProcAmp_Hue = lVal;
            }

            //4.Satuation
            hr = ptrAMVideoProcAmp->Get(VideoProcAmp_Saturation, &lVal, (long*)&flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }
            else
            {
                pVideoProcAmpProp->Prop_VideoProcAmp_Satuation = lVal;
            }

            //5.Sharpness
            hr = ptrAMVideoProcAmp->Get(VideoProcAmp_Sharpness, &lVal, (long*)&flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }
            else
            {
                pVideoProcAmpProp->Prop_VideoProcAmp_Sharpness = lVal;
            }

            //6.Gamma
            hr = ptrAMVideoProcAmp->Get(VideoProcAmp_Gamma, &lVal, (long*)&flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }
            else
            {
                pVideoProcAmpProp->Prop_VideoProcAmp_Gamma = lVal;
            }

            //7.ColorEnable.
            //hr = ptrAMVideoProcAmp->Get(VideoProcAmp_ColorEnable,&lVal, (long*)&flags);
            //if(FAILED(hr))
            //{	
            //	bRet = FALSE;
            //	//break;
            //}
            //else
            //{
            //	pVideoProcAmpProp->Prop_VideoProcAmp_ColorEnable = lVal;
            //}

            //8.White Balance
            hr = ptrAMVideoProcAmp->Get(VideoProcAmp_WhiteBalance, &lVal, (long*)&flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }
            else
            {
                pVideoProcAmpProp->Prop_VideoProcAmp_WhiteBalance = lVal;
            }


            //9.Backlight Compensation
            hr = ptrAMVideoProcAmp->Get(VideoProcAmp_BacklightCompensation, &lVal, (long*)&flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }
            else
            {
                pVideoProcAmpProp->Prop_VideoProcMap_BacklightCompensation = lVal;
            }

            //10.Gain
            //hr = ptrAMVideoProcAmp->Get(VideoProcAmp_Gain, &lVal, (long*)&flags);
            //if(FAILED(hr))
            //{	
            //	bRet = FALSE;
            //	 
            //	//返回E_PROP_ID_UNSUPPORTED
            //	//E_PROP_ID_UNSUPPORTED定义在vfwmsgs.h中
            //	//break;
            //}
            //else
            //{
            //	pVideoProcAmpProp->Prop_VideoProcMap_Gain = lVal;
            //}


        } while (0);
    }
    else
    {
        bRet = FALSE;
    }



    return bRet;
}


//@功能: 设置VideoProcAmp的属性值
//@参数:pCaptureFilter, 指向Video Capture Filter对象的指针 
//      pVideoProcAmpProp, 指向设置VideoProcAmp属性的数据缓冲区的指针
BOOL SetVideoProcAmpProperty(IBaseFilter *pCaptureFilter, const TVideoProcAmpProperty* pVideoProcAmpProp)
{
    BOOL bRet = TRUE;

    if (pCaptureFilter == NULL || pVideoProcAmpProp == NULL) return FALSE;

    CComPtr<IAMVideoProcAmp> ptrAMVideoProcAmp;

    HRESULT hr = pCaptureFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&ptrAMVideoProcAmp);

    if (hr == S_OK)
    {
        do
        {
            //LONG lVal;
            VideoProcAmpFlags flags = VideoProcAmp_Flags_Manual;
            //1.Brightness
            hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Brightness, pVideoProcAmpProp->Prop_VideoProcAmp_Brightness, (long)flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }

            //2.Contrast
            hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Contrast, pVideoProcAmpProp->Prop_VideoProcAmp_Contrast, (long)flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }


            //3.Hue
            hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Hue, pVideoProcAmpProp->Prop_VideoProcAmp_Hue, (long)flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }

            //4.Satuation
            hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Saturation, pVideoProcAmpProp->Prop_VideoProcAmp_Satuation, (long)flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }

            //5.Sharpness
            hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Sharpness, pVideoProcAmpProp->Prop_VideoProcAmp_Sharpness, (long)flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }


            //6.Gamma
            hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Gamma, pVideoProcAmpProp->Prop_VideoProcAmp_Gamma, (long)flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }


            //7.ColorEnable.
            //hr = ptrAMVideoProcAmp->Set(VideoProcAmp_ColorEnable, pVideoProcAmpProp->Prop_VideoProcAmp_ColorEnable, (long)flags);
            //if(FAILED(hr))
            //{	
            //	bRet = FALSE;
            //	//break;
            //}

            //8.White Balance
            hr = ptrAMVideoProcAmp->Set(VideoProcAmp_WhiteBalance, pVideoProcAmpProp->Prop_VideoProcAmp_WhiteBalance, (long)flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }

            //9.Backlight Compensation
            hr = ptrAMVideoProcAmp->Set(VideoProcAmp_BacklightCompensation, pVideoProcAmpProp->Prop_VideoProcMap_BacklightCompensation, (long)flags);
            if (FAILED(hr))
            {
                bRet = FALSE;
                //break;
            }

            //10.Gain
            //hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Gain, pVideoProcAmpProp->Prop_VideoProcMap_Gain, (long)flags);
            //if(FAILED(hr))
            //{	
            //	bRet = FALSE;
            //	 
            //	//返回E_PROP_ID_UNSUPPORTED
            //	//E_PROP_ID_UNSUPPORTED定义在vfwmsgs.h中
            //	//break;
            //}


        } while (0);

    }
    else
    {
        bRet = FALSE;
    }
    return bRet;
}

//创业达镜头控制
BOOL IRCUTSwitch_CYD(IBaseFilter *pCaptureFilter, BOOL bOn);

//SONIX HD USB2.0 
BOOL IRCUTSwitch_SONIX(IBaseFilter *pCaptureFilter, BOOL bOn);

//IRCut控制函数
BOOL IRCUTSwtich(IBaseFilter *pCaptureFilter, BOOL bOn, int PID, int VID)
{
    //return IRCUTSwitch_CYD(pCaptureFilter, bOn);
    if (PID == SONIX_PID && VID == SONIX_VID)
    {
        return IRCUTSwitch_SONIX(pCaptureFilter, bOn);
    }
    else
    {
        BOOL bRet = TRUE;
        if (pCaptureFilter == NULL)
        {
            return FALSE;
        }
        CComPtr <IAMVideoProcAmp> ptrAMVideoProcAmp;

        HRESULT hr = pCaptureFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&ptrAMVideoProcAmp);
		if (hr == S_OK)
		{
			if (bOn)
			{
				hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Gain, 0x44aa, VideoProcAmp_Flags_Manual); //////*0x44aa*//* VideoProcAmp_Gain
				if (FAILED(hr))
				{
					bRet = FALSE;
				}
			}
			else
			{
				hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Gain, 0x33aa, VideoProcAmp_Flags_Manual); /////// /*0x33aa*//*VideoProcAmp_Gain
				if (FAILED(hr))
				{
					bRet = FALSE;
				}
			}
        }
        return bRet;
    }
}


//@功能:通过UVC增益控制来间接控制GPIO的输出
//      0x2288,
BOOL GPIOControl(IBaseFilter *pCaptureFilter, BOOL bOn)
{
    //return IRCUTSwitch_CYD(pCaptureFilter, bOn);
    BOOL bRet = TRUE;
    if (pCaptureFilter == NULL)
    {
        return FALSE;
    }
    CComPtr <IAMVideoProcAmp> ptrAMVideoProcAmp;

    HRESULT hr = pCaptureFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&ptrAMVideoProcAmp);

    if (hr == S_OK)
    {
        do
        {
            if (bOn)
            {
                //GPIO On
                hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Gain, 0x2288, VideoProcAmp_Flags_Manual);
                if (FAILED(hr))
                {
                    bRet = FALSE;
                }
            }
            else
            {
                //GPIO Off
                hr = ptrAMVideoProcAmp->Set(VideoProcAmp_Gain, 0x2299, VideoProcAmp_Flags_Manual);
                if (FAILED(hr))
                {
                    bRet = FALSE;
                }
            }

        } while (0);

    }
    return bRet;

}





//HRESULT FindExtensionNode(IKsTopologyInfo *pKsTopologyInfo, GUID guid, DWORD *node);
HRESULT FindExtensionNode(IKsTopologyInfo *pKsTopologyInfo, DWORD *node);


//Extension-Unit Control function
HRESULT  XU_Get_InfoSize(
    IKsControl* pKsControl,
    const GUID& set,
    DWORD dwNodeId,
    ULONG *pulSize
);

HRESULT
XU_Put_Property(
    IKsControl* pKsControl,
    const GUID& set,
    DWORD dwNodeId,
    ULONG PropertyId,
    ULONG ulSize,
    BYTE pValue[]);


HRESULT
XU_Get_Property(
    IKsControl* pKsControl,
    const GUID& set,
    DWORD dwNodeId,
    ULONG PropertyId,
    ULONG ulSize,
    BYTE pValue[]);

//46394292-0cd1-4ae3-87833133f9eaaa3b
//VIDCAP: Video Capture
//GUID of Extension UNIT
EXTERN_GUID(PROPSETID_VIDCAP_EXTENSION_UNIT, 0x46394292, 0x0cd1, 0x4ae3, 0x87, 0x83, 0x31, 0x33, 0xf9, 0xea, 0xaa, 0x3b);


//
//
//创业达镜头控制
BOOL IRCUTSwitch_CYD(IBaseFilter *pCaptureFilter, BOOL bOn)
{
    BOOL bRet = TRUE;
    if (pCaptureFilter == NULL)
    {
        return FALSE;
    }

    CComPtr<IKsTopologyInfo> ptrKsTopologInfo;

    HRESULT hr = pCaptureFilter->QueryInterface(__uuidof(IKsTopologyInfo), (void**)& ptrKsTopologInfo);

    CComPtr<IKsControl> ptrKsControl;

    hr = pCaptureFilter->QueryInterface(__uuidof(IKsControl), (void**)& ptrKsControl);




    DWORD dwNode = (UINT)(-1);
    //hr = FindExtensionNode(ptrKsTopologInfo, IID_VENDOR_EXTENSION_UINT, &dwNode);
    hr = FindExtensionNode(ptrKsTopologInfo, &dwNode);

    if (hr != S_OK)
        return false;

    ULONG lInfoSize = 0;
    hr = XU_Get_InfoSize(ptrKsControl, PROPSETID_VIDCAP_EXTENSION_UNIT, dwNode, &lInfoSize);


    BYTE aryData[2];


    //read 
    aryData[0] = 0x60;
    aryData[1] = 0xd1;
    hr = XU_Put_Property(ptrKsControl, PROPSETID_VIDCAP_EXTENSION_UNIT, dwNode, 1, _countof(aryData), aryData);

    aryData[0] = 0x02;
    aryData[1] = 0x00;
    hr = XU_Put_Property(ptrKsControl, PROPSETID_VIDCAP_EXTENSION_UNIT, dwNode, 1, _countof(aryData), aryData);

    aryData[0] = 0x64;
    aryData[1] = 0xd1;
    hr = XU_Put_Property(ptrKsControl, PROPSETID_VIDCAP_EXTENSION_UNIT, dwNode, 1, _countof(aryData), aryData);

    aryData[0] = 0x01;
    aryData[1] = 0x00;
    hr = XU_Put_Property(ptrKsControl, PROPSETID_VIDCAP_EXTENSION_UNIT, dwNode, 1, _countof(aryData), aryData);

    aryData[0] = 0x0a;
    aryData[1] = 0x02;
    hr = XU_Put_Property(ptrKsControl, PROPSETID_VIDCAP_EXTENSION_UNIT, dwNode, 1, _countof(aryData), aryData);

    hr = XU_Get_Property(ptrKsControl, PROPSETID_VIDCAP_EXTENSION_UNIT, dwNode, 1, _countof(aryData), aryData);

    BYTE GPIOValue = aryData[0];


    //write value:
    aryData[0] = 0x60;
    aryData[1] = 0xd1;
    hr = XU_Put_Property(ptrKsControl, PROPSETID_VIDCAP_EXTENSION_UNIT, dwNode, 1, _countof(aryData), aryData);

    aryData[0] = 0x02;
    aryData[1] = 0x00;
    hr = XU_Put_Property(ptrKsControl, PROPSETID_VIDCAP_EXTENSION_UNIT, dwNode, 1, _countof(aryData), aryData);


    aryData[0] = 0x64;
    aryData[1] = 0xd1;
    hr = XU_Put_Property(ptrKsControl, PROPSETID_VIDCAP_EXTENSION_UNIT, dwNode, 1, _countof(aryData), aryData);


    aryData[0] = 0x01;
    aryData[1] = 0x00;
    hr = XU_Put_Property(ptrKsControl, PROPSETID_VIDCAP_EXTENSION_UNIT, dwNode, 1, _countof(aryData), aryData);


    aryData[0] = 0x0a;
    aryData[1] = 0x02;
    hr = XU_Put_Property(ptrKsControl, PROPSETID_VIDCAP_EXTENSION_UNIT, dwNode, 1, _countof(aryData), aryData);

    aryData[0] = bOn ? GPIOValue & 0xfb : GPIOValue | 0x04;
    aryData[1] = 0x00;
    hr = XU_Put_Property(ptrKsControl, PROPSETID_VIDCAP_EXTENSION_UNIT, dwNode, 1, _countof(aryData), aryData);

    return bRet;
}


//HRESULT FindExtensionNode(IKsTopologyInfo *pKsTopologyInfo, GUID guid, DWORD *node)
HRESULT FindExtensionNode(IKsTopologyInfo *pKsTopologyInfo, DWORD *node)
{
    HRESULT hr = E_FAIL;
    DWORD dwNumNodes = 0;
    GUID guidNodeType;

    //IKsControl *pKsControl = NULL;
    ULONG ulBytesReturned = 0;
    //KSP_NODE ExtensionProp;


    if (!pKsTopologyInfo || !node)
        return E_POINTER;



    // Retrieve the number of nodes in the filter
    hr = pKsTopologyInfo->get_NumNodes(&dwNumNodes);
    if (!SUCCEEDED(hr))
        return hr;

    if (dwNumNodes == 0)
        return E_FAIL;


    // Find the extension unit node that corresponds to the given GUID
    for (unsigned int i = 0; i < dwNumNodes; i++)
    {
        hr = E_FAIL;

        //"DFF229E6-F70F-11D0-B917-00A0C9223196", KSNODETYPE_VIDEO_CAMERA_TERMINAL
        //"941C7AC0-C559-11D0-8A2B-00A0C9255AC1", KSNODETYPE_DEV_SPECIFIC
        //"DFF229E5-F70F-11D0-B917-00A0C9223196", KSNODETYPE_VIDEO_PROCESSING
        //"DFF229E1-F70F-11D0-B917-00A0C9223196", KSNODETYPE_VIDEO_STREAMING
        hr = pKsTopologyInfo->get_NodeType(i, &guidNodeType);

        if (hr == S_OK && IsEqualGUID(guidNodeType, KSNODETYPE_DEV_SPECIFIC))
        {

            WCHAR nodeName[MAX_PATH];
            DWORD dwNameLen = 0;
            //返回字符串:"设备特殊"
            hr = pKsTopologyInfo->get_NodeName(i, nodeName, _countof(nodeName), &dwNameLen);

            *node = i;
            return S_OK;
        }

    }

    return hr;

}

HRESULT  XU_Get_InfoSize(
    IKsControl* pKsControl,
    const GUID& set,
    DWORD dwNodeId,
    ULONG *pulSize
)
{
    HRESULT hr = S_OK;
    ULONG ulBytesReturned;
    KSP_NODE ExtensionProp;

    if (!pKsControl) return E_POINTER;
    if (!pulSize) return E_POINTER;

    ExtensionProp.Property.Set = set;
    ExtensionProp.Property.Id = KSPROPERTY_EXTENSION_UNIT_INFO;
    ExtensionProp.Property.Flags = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_TOPOLOGY;
    ExtensionProp.NodeId = dwNodeId;

    hr = pKsControl->KsProperty(
        (PKSPROPERTY)&ExtensionProp,
        sizeof(ExtensionProp),
        NULL,
        0,
        &ulBytesReturned);

    if (hr == HRESULT_FROM_WIN32(ERROR_MORE_DATA))
    {
        *pulSize = ulBytesReturned;
        hr = S_OK;
    }

    return hr;
}


HRESULT  Get_Info(
    IKsControl* pKsControl,
    const GUID& set,
    DWORD dwNodeId,
    ULONG ulSize,
    BYTE pInfo[])
{
    if (!pKsControl) return E_FAIL;
    HRESULT hr = S_OK;
    KSP_NODE ExtensionProp;
    ULONG ulBytesReturned;

    ExtensionProp.Property.Set = set;
    ExtensionProp.Property.Id = KSPROPERTY_EXTENSION_UNIT_INFO;
    ExtensionProp.Property.Flags = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_TOPOLOGY;
    ExtensionProp.NodeId = dwNodeId;

    hr = pKsControl->KsProperty(
        (PKSPROPERTY)&ExtensionProp,
        sizeof(ExtensionProp),
        (PVOID)pInfo,
        ulSize,
        &ulBytesReturned);

    return hr;
}

/*
HRESULT
Get_PropertySize(
    ULONG PropertyId,
    ULONG *pulSize)
{
    HRESULT hr = S_OK;
    ULONG ulBytesReturned;
    KSP_NODE ExtensionProp;

         if (!pulSize) return E_POINTER;

    ExtensionProp.Property.Set = PROPSETID_VIDCAP_EXTENSION_UNIT;
    ExtensionProp.Property.Id = PropertyId;
    ExtensionProp.Property.Flags = KSPROPERTY_TYPE_GET |
                                   KSPROPERTY_TYPE_TOPOLOGY;
    ExtensionProp.NodeId = m_dwNodeId;

    hr = m_pKsControl->KsProperty(
        (PKSPROPERTY) &ExtensionProp,
                 sizeof(ExtensionProp),
        NULL,
        0,
        &ulBytesReturned);

         if (hr == HRESULT_FROM_WIN32(ERROR_MORE_DATA))
    {
        *pulSize = ulBytesReturned;
        hr = S_OK;
    }

         return hr;
}
*/

HRESULT
XU_Get_Property(
    IKsControl* pKsControl,
    const GUID& set,
    DWORD dwNodeId,
    ULONG PropertyId,
    ULONG ulSize,
    BYTE pValue[])
{
    if (!pKsControl) return E_FAIL;
    HRESULT hr = S_OK;
    KSP_NODE ExtensionProp;
    ULONG ulBytesReturned;

    ExtensionProp.Property.Set = set;
    ExtensionProp.Property.Id = PropertyId;
    ExtensionProp.Property.Flags = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_TOPOLOGY;
    ExtensionProp.NodeId = dwNodeId;

    hr = pKsControl->KsProperty(
        (PKSPROPERTY)&ExtensionProp,
        sizeof(ExtensionProp),
        (PVOID)pValue,
        ulSize,
        &ulBytesReturned);

    return hr;
}

HRESULT
XU_Put_Property(
    IKsControl* pKsControl,
    const GUID& set,
    DWORD dwNodeId,
    ULONG PropertyId,
    ULONG ulSize,
    BYTE pValue[])
{
    if (!pKsControl) return E_FAIL;

    HRESULT hr = S_OK;
    KSP_NODE ExtensionProp;
    ULONG ulBytesReturned = 0;

    ExtensionProp.Property.Set = set;
    ExtensionProp.Property.Id = PropertyId;
    ExtensionProp.Property.Flags = KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_TOPOLOGY;
    ExtensionProp.NodeId = dwNodeId;

    hr = pKsControl->KsProperty(
        (PKSPROPERTY)&ExtensionProp,
        sizeof(ExtensionProp),
        (PVOID)pValue,
        ulSize,
        &ulBytesReturned);

    return hr;
}

/////add by zhaown 2019.07.09
//Sonix Techonology CO., LTD
//LGA54-SN9C291B
//28f03370-6311-4a2e-ba2c6890eb334016
EXTERN_GUID(UVC_GUID_SONIX_USER_HW_CONTROL, 0x28f03370, 0x6311, 0x4a2e, 0xba, 0x2c, 0x68, 0x90, 0xeb, 0x33, 0x40, 0x16);

// ----------------------------- XU Control Selector ------------------------------------
#define XU_SONIX_ASIC_CTRL 				0x01   // 与control 1 相对应
#define XU_SONIX_I2C_CTRL				0x02
#define XU_SONIX_SF_READ 				0x03

//USB2.0 H.264 Video
//Encoding Camera Controller SN9C291B

struct TSONIXControl
{
    IBaseFilter *pCaptureFilter;
    BOOL bOn;
};

DWORD _stdcall IRCUTSwitch_SONIX_ThreadProc(LPVOID lpCtx);
BOOL IRCUTSwitch_SONIX(IBaseFilter *pCaptureFilter, BOOL bOn)
{
    BOOL bRet = FALSE;

    TSONIXControl sonixControl;

    sonixControl.pCaptureFilter = pCaptureFilter;
    sonixControl.bOn = bOn;

    HANDLE hThread = CreateThread(NULL, 0, IRCUTSwitch_SONIX_ThreadProc, &sonixControl, 0, NULL);

    DWORD dwResult = WaitForSingleObject(hThread, 1000);

    DWORD dwExitCode = 0;
    switch (dwResult)
    {
    case WAIT_OBJECT_0:
        GetExitCodeThread(hThread, &dwExitCode);
        bRet = (dwExitCode == 0) ? TRUE : FALSE;
        break;

    case WAIT_TIMEOUT:
        TerminateThread(hThread, dwExitCode);
        break;

    }//switch

    CloseHandle(hThread);

    return bRet;
}

DWORD _stdcall IRCUTSwitch_SONIX_ThreadProc(LPVOID lpCtx)
{
    TSONIXControl* pSonixControl = reinterpret_cast<TSONIXControl*>(lpCtx);
    IBaseFilter *pCaptureFilter = pSonixControl->pCaptureFilter;
    BOOL bOn = pSonixControl->bOn;

    DWORD ret = -1;
    do
    {
        if (pCaptureFilter == NULL)    break;

        CComPtr<IKsTopologyInfo> ptrKsTopologInfo;

        HRESULT hr = pCaptureFilter->QueryInterface(__uuidof(IKsTopologyInfo), (void**)& ptrKsTopologInfo);
        if (FAILED(hr))               break;

        CComPtr<IKsControl> ptrKsControl;

        hr = pCaptureFilter->QueryInterface(__uuidof(IKsControl), (void**)& ptrKsControl);
        if (FAILED(hr))              break;


        DWORD dwNode = (UINT)(-1);
        //hr = FindExtensionNode(ptrKsTopologInfo, IID_VENDOR_EXTENSION_UINT, &dwNode);
        hr = FindExtensionNode(ptrKsTopologInfo, &dwNode);

        if (hr != S_OK)     		 break;

        //ULONG lInfoSize = 0;
        //hr = XU_Get_InfoSize(ptrKsControl, UVC_GUID_SONIX_USER_HW_CONTROL, dwNode, &lInfoSize);

        BYTE aryData[4];

        //1.设置GPIIO_0为输入/输出使能
        //0x1007八位寄存器，
        //  bit0 控制GPIO_0口的输入输出使能 （in:0    out:1  //输入输出控制 0：输入 1：输出）
        //  bit1~7 控制GPIO_1~7口的输入输出使能
        USHORT address = 0x1007;


        aryData[0] = address & 0xFF;       // Tag
        aryData[1] = (address & 0xFF00) >> 8;
        aryData[2] = 0x01;
        aryData[3] = 0x00;                  // Dummy   
        hr = XU_Put_Property(ptrKsControl, UVC_GUID_SONIX_USER_HW_CONTROL, dwNode, XU_SONIX_ASIC_CTRL, _countof(aryData), aryData);
        if (FAILED(hr))               break;


        //aryData[0] = address & 0xFF;       // Tag
        //aryData[1] = (address & 0xFF00)>>8;
        //aryData[2] = 0x00;
        //aryData[3] = 0x00;                  // Dummy   
        //hr = XU_Get_Property(ptrKsControl, UVC_GUID_SONIX_USER_HW_CONTROL, dwNode, XU_SONIX_ASIC_CTRL, _countof(aryData), aryData);

        Sleep(100);

        //2.设置GPIO_0的输出
        //0x1006 八位寄存器
        //bit0   控制GPIO_0口的输出状态 ；输出高：1；输出低：0；
        //bit1~7 控制GPIO_1~7口的输入状态
        address = 0x1006;
        aryData[0] = address & 0xFF;       // Tag
        aryData[1] = (address & 0xFF00) >> 8;
        aryData[2] = bOn ? 0x01 : 0x00;
        aryData[3] = 0x0;                  // Dummy  

        hr = XU_Put_Property(ptrKsControl, UVC_GUID_SONIX_USER_HW_CONTROL, dwNode, XU_SONIX_ASIC_CTRL, _countof(aryData), aryData);
        if (FAILED(hr))             break;

        //hr = XU_Get_Property(ptrKsControl, UVC_GUID_SONIX_USER_HW_CONTROL, dwNode, XU_SONIX_ASIC_CTRL, _countof(aryData), aryData);

        //0x1005 八位寄存器 
        //  bit0 控制GPIO_0口的输入状态 ；输入高：1；输入低：0；
        //  bit1~7 控制GPIO_1~7口的输入状态
        ret = 0;

    } while (0);

    return ret;
}


//HRESULT hr;
//HANDLE hndEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
//KSEVENT Event = {0};
//Event.Set = KSEVENTSETID_VIDCAPNotify;
//Event.Id = KSEVENT_VIDCAPTOSTI_EXT_TRIGGER;
//Event.Flags = KSEVENT_TYPE_ENABLE;
//KSEVENTDATA EventData = {0};
//EventData.NotificationType = KSEVENTF_EVENT_HANDLE;
//EventData.EventHandle.Event = hndEvent;
//EventData.EventHandle.Reserved[0] = 0;
//EventData.EventHandle.Reserved[1] = 0;
//// nodeID is a number form 0 to 3 (video source topology has 4 nodes)
//KSE_NODE EventNode = { 0 };
//EventNode.Event = Event;
//EventNode.NodeId = (ULONG)nodeID;
//// This time pKsControl refers to a video source output pin (video capture or still capture)
//// I always get an ERROR_SET_NOT_FOUND
//hr = pKsControl‐>KsEvent;
//hr = pKsControl‐>KsEvent(
//                         (KSEVENT)&EventNode,
//                         sizeof(EventNode),
//                         &EventData,
//                         sizeof(EventData),
//                         &ulBytesReturned);

/*
BOOL bTest = FALSE;
    if(bTest)
    {
        DWORD dwNodeId = 2;
        CComPtr<IKsControl> ptrNodeKsControl;


        hr = ptrKsTopologInfo->CreateNodeInstance(
            dwNodeId,
            __uuidof(IKsControl),
            (void**)&ptrNodeKsControl);


        CComPtr<IPin> ptrCaptureOutputPin;
        hr = FindPin(pCaptureFilter, PINDIR_OUTPUT, _T("静态") , &ptrCaptureOutputPin);
        //hr = FindPin(pCaptureFilter, PINDIR_INPUT, NULL, &ptrCaptureOutputPin);

        CComPtr<IKsControl> ptrPinKsControl;
        hr = ptrCaptureOutputPin->QueryInterface(__uuidof(IKsControl), (void**)& ptrPinKsControl);

        HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        KSEVENT Event = {0};

        Event.Set = KSEVENTSETID_VIDCAPNotify;
        Event.Id = KSEVENT_VIDCAPTOSTI_EXT_TRIGGER;
        Event.Flags = KSEVENT_TYPE_ENABLE;
        //Event.Flags = KSEVENT_TYPE_ENABLE | KSEVENT_TYPE_TOPOLOGY;

        KSEVENTDATA EventData;
        EventData.NotificationType = KSEVENTF_EVENT_HANDLE;
        EventData.EventHandle.Event = hEvent;
        EventData.EventHandle.Reserved[0] = 0;
        EventData.EventHandle.Reserved[1] = 0;

        ULONG ulBytesReturned = 0;

        // nodeID is a number form 0 to 3 (video source topology has 4 nodes)
        KSE_NODE EventNode = { 0 };
        EventNode.Event = Event;
        EventNode.NodeId = (ULONG)2;

        hr = ptrNodeKsControl->KsEvent(
                    &Event,
                    sizeof(KSEVENT),
                    &EventData,
                    sizeof(KSEVENTDATA),
                    &ulBytesReturned);


         hr = ptrPinKsControl->KsEvent(
        //hr =  ptrNodeKsControl->KsEvent(
                    (KSEVENT*)&EventNode,
                    sizeof(KSE_NODE),
                    &EventData,
                    sizeof(KSEVENTDATA),
                    &ulBytesReturned);

        // Wait for event
        BOOL bEventTigger = FALSE;
        DWORD dwError = WaitForSingleObject(hEvent, 60000);
        if ((dwError == WAIT_FAILED) ||
           (dwError == WAIT_ABANDONED) ||
           (dwError == WAIT_TIMEOUT))
        {
            bEventTigger = FALSE;
        }
        else
        {
            bEventTigger = TRUE;
        }
    }
*/
//VideoControlFlag_Trigger
//VideoControlFlag_Trigger