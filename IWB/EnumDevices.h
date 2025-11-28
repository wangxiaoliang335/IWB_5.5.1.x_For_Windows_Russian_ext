#pragma once
#include <list>
typedef struct
{
    GUID majortype;
    GUID subtype;
    GUID formattype;
    union
    {
        VIDEOINFOHEADER  videoInfoHeader;
        VIDEOINFOHEADER2 videoInfoHeader2;
    };

}VideoMediaType;


inline CAtlString GetVideoFormatName(const VideoMediaType& vmt)
{
    CAtlString strFmtName;
    strFmtName.Format(
                _T("%d X %d %c%c%c%c"), 
                vmt.videoInfoHeader.bmiHeader.biWidth,
                vmt.videoInfoHeader.bmiHeader.biHeight,
                vmt.videoInfoHeader.bmiHeader.biCompression & 0xFF,
                (vmt.videoInfoHeader.bmiHeader.biCompression >> 8) & 0xFF,
                (vmt.videoInfoHeader.bmiHeader.biCompression >> 16) & 0xFF,
                (vmt.videoInfoHeader.bmiHeader.biCompression >> 24) & 0xFF);

    return strFmtName;
}



//视频捕获设备实例
typedef struct
{
    CAtlString m_strName;
    CAtlString m_strDevPath;
    //int        m_nInstNo;//保留不再使用
    DWORD        m_dwDevInst;//设备实例句柄(device instance handle),调用CM_XXX函数时使用
    std::vector<VideoMediaType> m_vecVideoFmt;

    //std::vector<VIDEOINFOHEADER> m_vecVideoFmt;
	int m_nVID;
	int m_nPID;	
}TCaptureDeviceInstance;


//inline CAtlString GetCaptureDeviceFullName(const TCaptureDeviceInstance& capDevInst)
//{
//	CAtlString strCapDevName;
//	strCapDevName = capDevInst.m_strName;
//
//	if(capDevInst.m_nInstNo != 1)
//	{
//		CString strNo;
//		strNo.Format(_T("[%d]"), capDevInst.m_nInstNo);
//		strCapDevName += strNo;
//	}
//
//
//	return strCapDevName;
//}

inline const TCHAR* GetMajorTypeDescription(const GUID& majorType)
{

    const TCHAR* pString = NULL;
    //reference Dshow.h
    if(MEDIATYPE_AnalogAudio == majorType)
    {
        pString = _T("MEDIATYPE_AnalogAudio, Analog audio");
    }
    else if(MEDIATYPE_AnalogVideo == majorType)
    {
        pString = _T("MEDIATYPE_AnalogVideo, Analog video");
    }
    else if(MEDIATYPE_Audio == majorType)
    {
        pString = _T("MEDIATYPE_Audio, Audio");
    }
    if(MEDIATYPE_AUXLine21Data == majorType)
    {
        pString = _T("MEDIATYPE_AUXLine21Data, Line 21 data. Used by closed captions");
    }
    if(MEDIATYPE_Interleaved == majorType)
    {
        pString = _T("MEDIATYPE_Interleaved, Interleaved audio and video. Used for Digital Video (DV).");
    }
   else if(MEDIATYPE_Midi == majorType)
    {
        pString = _T("MEDIATYPE_Midi");
    }
    else if(MEDIATYPE_MPEG2_PES == majorType)
    {
        pString = _T("MEDIATYPE_MPEG2_PES, MPEG-2 PES packets.");
    }
    else if(MEDIATYPE_MPEG2_SECTIONS == majorType)
    {
        pString = _T("MEDIATYPE_MPEG2_SECTIONS, MPEG-2 section data");
    }
    else if(MEDIATYPE_ScriptCommand == majorType)
    {
        pString = _T("MEDIATYPE_ScriptCommand, Data is a script command, used by closed captions");
    }
    else if(MEDIATYPE_Stream == majorType)
    {
        pString = _T("MEDIATYPE_Stream, Byte stream with no time stamps");
    }
    else if(MEDIATYPE_Text == majorType)
    {
        pString = _T("MEDIATYPE_Text");
    }
    else if(MEDIATYPE_Timecode == majorType)
    {
        pString = _T("MEDIATYPE_Timecode, Timecode data");
    }
     else if(MEDIATYPE_VBI == majorType)
    {
        pString = _T("MEDIATYPE_VBI, Vertical blanking interval (VBI) data (for television). Same as KSDATAFORMAT_TYPE_VBI.");
    }
    else if(MEDIATYPE_Video == majorType)
    {
       pString = _T("MEDIATYPE_Video");
    }

    else if(MEDIATYPE_Text == majorType)
    {
        pString = _T("MEDIATYPE_Text");
    }
    else
    {
        static CAtlString str;

        str.Format(
            _T("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x"),
            majorType.Data1,
            majorType.Data2,
            majorType.Data3,
            majorType.Data4[0],
            majorType.Data4[1],
            majorType.Data4[2],
            majorType.Data4[3],
            majorType.Data4[4],
            majorType.Data4[5],
            majorType.Data4[6],
            majorType.Data4[7]);
        pString = str;
                    
    }
    return pString;

}

inline const TCHAR* GetSubTypeDescription(const GUID& subtype)
{
     const TCHAR* pString = NULL;
    if(MEDIASUBTYPE_YUY2 == subtype)
    {
        pString = _T("MEDIASUBTYPE_YUY2");
    }
    else if(MEDIASUBTYPE_MJPG == subtype)
    {
       pString = _T("MEDIASUBTYPE_MJPG, Motion JPEG (MJPG) compressed video"); 
    }
    else if(MEDIASUBTYPE_RGB24 == subtype)
    {
        pString = _T("MEDIASUBTYPE_RGB24");
    }
    else
    {
        static CAtlString str;

        str.Format(
            _T("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x"),
            subtype.Data1,
            subtype.Data2,
            subtype.Data3,
            subtype.Data4[0],
            subtype.Data4[1],
            subtype.Data4[2],
            subtype.Data4[3],
            subtype.Data4[4],
            subtype.Data4[5],
            subtype.Data4[6],
            subtype.Data4[7]);
        pString = str;
                    
    }
    return pString;
}

inline const TCHAR* GetCompressionDescription(DWORD iCompression)
{
    static CAtlString str;
    str.Format(
        _T("%c%c%c%c"),
         iCompression & 0xFF,
         (iCompression>> 8) & 0xFF,
         (iCompression>> 16)& 0xFF,
        (iCompression>> 24)& 0xFF
);

    return str;
}
//@功能:打印视频格式
inline void PrintVideoFormat(const AM_MEDIA_TYPE& amt)
{
     const BITMAPINFOHEADER* pbmiHeader;
    if(FORMAT_VideoInfo == amt.formattype)
    {
        pbmiHeader = &reinterpret_cast<VIDEOINFOHEADER*>(amt.pbFormat)->bmiHeader;
    }
    else if(FORMAT_VideoInfo2 == amt.formattype)
    {
        pbmiHeader = &reinterpret_cast<VIDEOINFOHEADER2*>(amt.pbFormat)->bmiHeader;
    }
       
    if(pbmiHeader)
    {
            AtlTrace(_T("\
        ====================================\n\
        MajorType:%s\n \
        SubType:%s\n \
        Width:%d\n \
        Height:%d\n \
        Compression:%s\n \
        BitCount:%d\n \
        SizeImage:%d\n"),
        GetMajorTypeDescription(amt.majortype),
        GetSubTypeDescription(amt.subtype),
        pbmiHeader->biWidth,
        pbmiHeader->biHeight,
        GetCompressionDescription(pbmiHeader->biCompression),
        pbmiHeader->biBitCount,
        pbmiHeader->biSizeImage);
    }
}

typedef std::vector<TCaptureDeviceInstance> CaptuerDeviceInstanceSet;

inline HRESULT RetrieveCaptureDeviceNames(CaptuerDeviceInstanceSet& instanceSet, const GUID& clsidDeviceCategory)
{
    HRESULT hr = S_OK;
    CComPtr<ICreateDevEnum> ptrSysDevEnum = NULL;

    hr = ptrSysDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum);

    if(hr != S_OK) return hr;

    CComPtr<IEnumMoniker> ptrEnumMoniker;

    hr = ptrSysDevEnum->CreateClassEnumerator(
        clsidDeviceCategory, 
        &ptrEnumMoniker, 
        0);

    if(hr != S_OK) return hr;


    CComPtr<ICaptureGraphBuilder2> ptrCaptureGraphBuilder;
    hr = ptrCaptureGraphBuilder.CoCreateInstance(CLSID_CaptureGraphBuilder2);
    if(hr != S_OK) return hr;

    //CAtlMap<CAtlString, LONG> DeviceInstanceMap;
    //Iterate through each moniker
    CComPtr<IMoniker> ptrMoniker = NULL;
    while(S_OK == ptrEnumMoniker->Next(
        1,//The number of monikers to return in rgelt
        &ptrMoniker,//When this method returns, contains a reference to the enumerated monikers. This parameter is passed uninitialized.
        NULL))//When this method returns, contains a reference to the actual number of monikers enumerated in rgelt. 
    {

        CComPtr<IPropertyBag> ptrPropBag;

        hr = ptrMoniker->BindToStorage(
            0,//bind context
            0,//
            IID_IPropertyBag,
            (VOID**)&ptrPropBag);

        VARIANT varCLSID;
        VariantInit(&varCLSID);
        hr = ptrPropBag->Read(L"CLSID", &varCLSID, 0);
        VariantClear(&varCLSID);


        VARIANT varDevPath;

        VariantInit(&varDevPath);
        BOOL bDevPathIsValid = FALSE;
        hr = ptrPropBag->Read(L"DevicePath", &varDevPath, 0 );
        bDevPathIsValid = (hr == S_OK)?TRUE:FALSE;

        //VARIANT varDescription;
        //VariantInit(&varDescription);
        //hr = ptrPropBag->Read(L"Description", &varDescription, NULL);
        //VariantClear(&varDescription);


        VARIANT varName;
        VariantInit(&varName);
        hr = ptrPropBag->Read(L"FriendlyName", &varName, 0);

        if(hr == S_OK)
        {
            CAtlString strName = varName.bstrVal;
            VariantClear(&varName);

            TCaptureDeviceInstance deviceInstance;
            deviceInstance.m_strName   = strName;
            
            if(bDevPathIsValid)
            {
                deviceInstance.m_strDevPath = varDevPath.bstrVal;
                VariantClear(&varDevPath);
            }

            //CAtlMap<CAtlString, LONG>::CPair* pPair = DeviceInstanceMap.Lookup(strName);
            //if(pPair == NULL)
            //{
            //	DeviceInstanceMap[strName] = 1;
            //	deviceInstance.m_nInstNo = 1;
            //}
            //else
            //{
            //	DeviceInstanceMap[strName] ++;
            //	deviceInstance.m_nInstNo = DeviceInstanceMap[strName];
            //}

            CComPtr<IBaseFilter> ptrFilter;
        
            hr  = ptrMoniker->BindToObject(
                0,//bind context
                0,//
                IID_IBaseFilter,
                (VOID**)&ptrFilter);

            if(SUCCEEDED(hr))
            {
                CComPtr<IAMStreamConfig > ptrConfig;

                hr = ptrCaptureGraphBuilder->FindInterface(
                            &PIN_CATEGORY_CAPTURE,
                            &MEDIATYPE_Video,
                            ptrFilter,
                            IID_IAMStreamConfig,
                            (void **)&ptrConfig);

                if(SUCCEEDED(hr))
                {
                    int iCount = 0, iSize = 0;
                    hr = ptrConfig->GetNumberOfCapabilities(&iCount, &iSize);
                    if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
                    {
                        for(int iFormat = 0; iFormat < iCount; iFormat++)
                        {
                            VIDEO_STREAM_CONFIG_CAPS vscc;
                            AM_MEDIA_TYPE *pmtConfig;
                            hr = ptrConfig->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&vscc);
                            if(SUCCEEDED(hr))
                            {
                                if(FORMAT_VideoInfo == pmtConfig->formattype)
                                {
                                    if(pmtConfig->cbFormat >= sizeof(VIDEOINFOHEADER) && pmtConfig->pbFormat != NULL)
                                    {
                                        VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)pmtConfig->pbFormat;
                                        BITMAPINFOHEADER *bmiHeader = &pVIH->bmiHeader;

                                        VideoMediaType vmt;
                                        vmt.majortype       = pmtConfig->majortype;
                                        vmt.subtype         = pmtConfig->subtype;
                                        vmt.formattype      = FORMAT_VideoInfo;
                                        vmt.videoInfoHeader = *pVIH ;

                                        //<<debug
                                        PrintVideoFormat(*pmtConfig);
                                        //debug>>
                                        //VIDEOINFOHEADER vih = *pVIH;
                                        deviceInstance.m_vecVideoFmt.push_back(vmt);


                                    }
                                }
                                else if(FORMAT_VideoInfo2 == pmtConfig->formattype)
                                {
                                    
                                    if(pmtConfig->cbFormat >= sizeof(VIDEOINFOHEADER) && pmtConfig->pbFormat != NULL)
                                    {
                                        VIDEOINFOHEADER2 *pVIH = (VIDEOINFOHEADER2*)pmtConfig->pbFormat;
                                        BITMAPINFOHEADER *bmiHeader = &pVIH->bmiHeader;

                                        VideoMediaType vmt;
                                        vmt.majortype       = pmtConfig->majortype;
                                        vmt.subtype         = pmtConfig->subtype;
                                        vmt.formattype      = FORMAT_VideoInfo2;
                                        vmt.videoInfoHeader2 = *pVIH ;
                                        
                                        //<<debug
                                        PrintVideoFormat(*pmtConfig);
                                        //debug>>

                                        deviceInstance.m_vecVideoFmt.push_back(vmt);

                                    }
                                }
                                
                            }//if

                            DeleteMediaType(pmtConfig);

                        }//for each image format
                    }
                }


            }//if

            instanceSet.push_back(deviceInstance);
        }

        //
        VariantClear(&varName);

        ptrMoniker =NULL;
    }

    return S_OK;
}



//@功能:判断设备的VID和PID相同
//
inline BOOL IsUSBDevVidPidEqual(LPCTSTR szLeft, LPCTSTR szRight)
{
        
    //Interface Name:\\?\USB#Vid_0ac8&Pid_3450#5&23688d58&0&3#{a5dcbf10-6530-11d2-901f-00c04fb951e
    //Dev Path      :\\?\usb#vid_0ac8&pid_3450&mi_00#6&2826585d&0&0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\global
    //GUID_DEVINTERFACE_USB_DEVICE Class GUID = {A5DCBF10-6530-11D2-901F-00C04FB951ED} defined in Usbiodef.h.
    //
    //

 
    TCHAR left[26], right[26];//\\?\USB#Vid_0ac8&Pid_3450
    memset(left,  0, sizeof(left));
    memset(right, 0, sizeof(right));
    if(_tcslen(szLeft) < sizeof(left) -1 || _tcslen(szRight) < sizeof(right)-1)
    {
        return FALSE;
    }
    _tcsncpy_s(left, _countof(left), szLeft, _countof(left) -1);
    _tcsncpy_s(right, _countof(right),szRight, _countof(left) -1);

    if( _tcsnicmp(left, _T("\\\\?\\USB#Vid_"), 12) !=0  ||  _tcsnicmp(right, _T("\\\\?\\USB#Vid_"), 12) !=0 )
    {
        return FALSE;
    }


    if(_tcsnicmp(&left[12], &right[12], 4) != 0 )
    {
        return FALSE;
    }

    
    if( _tcsnicmp(&left[16], _T("&Pid_"), 5) !=0  ||  _tcsnicmp(&right[16], _T("&Pid_"), 5) !=0 )
    {
        return FALSE;
    }

    if(_tcsnicmp(&left[21], &right[21], 4) != 0 )
    {
        return FALSE;
    }

    return TRUE;
}







//@功能:使用SetupDixxx API函数枚举视频捕获设备
//@参数:
//@说明:步骤1:
//      使用SetupDixxx API函数枚举出所有视频捕获设备和它的设备路径, 相关API函数有
//      SetupDiGetClassDevs
//      SetupDiEnumDeviceInfo
//      SetupDiEnumDeviceInterfaces
//
//      步骤2:通过设备路径打开设备, 使用到的API函数有
//      CreateBindCtx
//      MkParseDisplayName  
//     IMoniker::BindToObject
// moniker prefixes used in DirectShow
//@Author:toxuke@gmail.com, 2011/10/09
//

inline HRESULT EnumVideoCaptureDeviceVideoFormat(std::vector<VideoMediaType>& vecVideoMediaType, const OLECHAR* lpszDevPath);

inline HRESULT EnumVideoCaptureDevice(CaptuerDeviceInstanceSet& instanceSet)
{
    HRESULT hr = S_OK;

    instanceSet.clear();

    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;


    hDevInfo = 
        SetupDiGetClassDevs(
            &KSCATEGORY_CAPTURE,	
            NULL,
            NULL,
            DIGCF_PRESENT | DIGCF_DEVICEINTERFACE  

          );


    /*
    hDevInfo =
        SetupDiGetClassDevs(
            &CLSID_VideoInputDeviceCategory,
            NULL,
            NULL,
            DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
        );
        */


    if(hDevInfo == INVALID_HANDLE_VALUE)
    {
        return E_FAIL;
    }


    DWORD nMemberIndex = 0;
    SP_DEVINFO_DATA  DeviceInfoData;
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    DWORD dwError = 0;
    BOOL bMoreDevice = FALSE;


    do
    {
        bMoreDevice = SetupDiEnumDeviceInfo(
            hDevInfo,
            nMemberIndex++,
            &DeviceInfoData);


        if(!bMoreDevice) break;
        

        WCHAR szFriendlyName[MAX_DEVICE_ID_LEN];
        memset(szFriendlyName, 0, sizeof(szFriendlyName));

             
        SetupDiGetDeviceRegistryProperty(
                hDevInfo,
                &DeviceInfoData,
                SPDRP_FRIENDLYNAME,
                NULL,
                (PBYTE)szFriendlyName,
                sizeof(szFriendlyName),
                NULL);



        DWORD interfaceMemberIndex = 0;
        SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
        DeviceInterfaceData.cbSize = sizeof(DeviceInterfaceData);

        BOOL bMoreIterface = FALSE;

        do
        {
            bMoreIterface = 
            SetupDiEnumDeviceInterfaces(
                hDevInfo,
                &DeviceInfoData,
                &KSCATEGORY_CAPTURE,
                interfaceMemberIndex,
                &DeviceInterfaceData);

            if(!bMoreIterface) break;

            interfaceMemberIndex++;


            DWORD dwRequiredSize = 0;
            SetupDiGetDeviceInterfaceDetail(
                hDevInfo,
                &DeviceInterfaceData,
                (PSP_DEVICE_INTERFACE_DETAIL_DATA)NULL,
                0,
                &dwRequiredSize,	
                &DeviceInfoData);


            PSP_DEVICE_INTERFACE_DETAIL_DATA  pDeviceInterfaceDetailData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)malloc(dwRequiredSize);
                
            pDeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

            SetupDiGetDeviceInterfaceDetail(
                hDevInfo,
                &DeviceInterfaceData,
                pDeviceInterfaceDetailData,
                dwRequiredSize,
                &dwRequiredSize,	
                &DeviceInfoData);


            TCaptureDeviceInstance deviceInstance;

            hr = EnumVideoCaptureDeviceVideoFormat(deviceInstance.m_vecVideoFmt, pDeviceInterfaceDetailData->DevicePath);


            if(SUCCEEDED(hr))
            {	
                errno_t err = _tcslwr_s(pDeviceInterfaceDetailData->DevicePath, _tcslen(pDeviceInterfaceDetailData->DevicePath) + 1);
                deviceInstance.m_strDevPath = pDeviceInterfaceDetailData->DevicePath;
                deviceInstance.m_dwDevInst  = DeviceInfoData.DevInst;
                deviceInstance.m_strName    = szFriendlyName;

				int nPID = 0, nVID = 0;
				int ret = _stscanf_s(pDeviceInterfaceDetailData->DevicePath, _T("\\\\?\\usb#vid_%04x&pid_%04x"), &nVID, &nPID);

				deviceInstance.m_nPID = nPID;
				deviceInstance.m_nVID = nVID;
                instanceSet.push_back(deviceInstance);
            }

            free(pDeviceInterfaceDetailData);
                
            dwError = GetLastError();
            
        }while(bMoreIterface);

    }while(bMoreDevice);


    
    SetupDiDestroyDeviceInfoList(hDevInfo);

    return S_OK;
}


//@功能;根据视频设备的设备路径枚举视频格式
//@参数:videoMediaType, 保存视频格式的数组
//       lpszDevPath, 设备路径字符串,
//@Author:toxuke@gmail.com, 2011/10/09
#define VC_MONIKER_PREFIX L"@device:pnp:"
#define AC_MONIKER_PREFIX L"@device:cm:{33D9A762-90C8-11D0-BD43-00A0C911CE86}\\"
inline HRESULT EnumVideoCaptureDeviceVideoFormat(std::vector<VideoMediaType>& vecVideoMediaType, const OLECHAR* lpszDevPath)
{
    HRESULT hr = S_OK;

    CComPtr<ICaptureGraphBuilder2> ptrCaptureGraphBuilder;
    hr = ptrCaptureGraphBuilder.CoCreateInstance(CLSID_CaptureGraphBuilder2);
    if(hr != S_OK) return hr;

    vecVideoMediaType.clear();
    OLECHAR  wszMonikerDevPath[MAX_PATH];
    wcscpy_s(wszMonikerDevPath,_countof(wszMonikerDevPath), VC_MONIKER_PREFIX);
    wcscat_s(wszMonikerDevPath,_countof(wszMonikerDevPath), lpszDevPath);

    do
    {

        CComPtr<IBindCtx> ptrBindCtx;
        hr = CreateBindCtx(0, &ptrBindCtx);
        if(FAILED(hr))
        {
            break;
        }
        

        CComPtr<IMoniker> ptrMoniker;
        DWORD wEaten;

        hr =
        MkParseDisplayName(
            ptrBindCtx,//pointer to IBindCtx interface
            wszMonikerDevPath,
            &wEaten,
            &ptrMoniker);

        if(FAILED(hr))
        {
            break;
        }

        CComPtr<IBaseFilter> ptrFilter;
        
        hr  = ptrMoniker->BindToObject(
                ptrBindCtx,//bind context
                0,//
                IID_IBaseFilter,
                (VOID**)&ptrFilter);

        if(SUCCEEDED(hr))
        {
            //枚举所有视频格式
            CComPtr<IAMStreamConfig > ptrConfig;
            hr = ptrCaptureGraphBuilder->FindInterface(
                            &PIN_CATEGORY_CAPTURE,
                            &MEDIATYPE_Video,
                            ptrFilter,
                            IID_IAMStreamConfig,
                            (void **)&ptrConfig);

            if(SUCCEEDED(hr))
            {
                    int iCount = 0, iSize = 0;
                    hr = ptrConfig->GetNumberOfCapabilities(&iCount, &iSize);
                    if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
                    {
                        for(int iFormat = 0; iFormat < iCount; iFormat++)
                        {
                            VIDEO_STREAM_CONFIG_CAPS vscc;
                            AM_MEDIA_TYPE *pmtConfig;
                            hr = ptrConfig->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&vscc);
                            if(SUCCEEDED(hr))
                            {
                                if(FORMAT_VideoInfo == pmtConfig->formattype)
                                {
                                    if(pmtConfig->cbFormat >= sizeof(VIDEOINFOHEADER) && pmtConfig->pbFormat != NULL)
                                    {
                                        VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)pmtConfig->pbFormat;
                                        BITMAPINFOHEADER *bmiHeader = &pVIH->bmiHeader;

                                        VideoMediaType vmt;
                                        vmt.majortype       = pmtConfig->majortype;
                                        vmt.subtype         = pmtConfig->subtype;
                                        vmt.formattype      = FORMAT_VideoInfo;
                                        vmt.videoInfoHeader = *pVIH ;

                                        //<<debug
                                        PrintVideoFormat(*pmtConfig);
                                        //debug>>

                                       //VIDEOINFOHEADER vih = *pVIH;
                                        vecVideoMediaType.push_back(vmt);


                                    }
                                }
                                else if(FORMAT_VideoInfo2 == pmtConfig->formattype)
                                {
                                    
                                    if(pmtConfig->cbFormat >= sizeof(VIDEOINFOHEADER) && pmtConfig->pbFormat != NULL)
                                    {
                                        VIDEOINFOHEADER2 *pVIH = (VIDEOINFOHEADER2*)pmtConfig->pbFormat;
                                        BITMAPINFOHEADER *bmiHeader = &pVIH->bmiHeader;

                                        VideoMediaType vmt;
                                        vmt.majortype       = pmtConfig->majortype;
                                        vmt.subtype         = pmtConfig->subtype;
                                        vmt.formattype      = FORMAT_VideoInfo2;
                                        vmt.videoInfoHeader2 = *pVIH ;
                                        //<<debug
                                        PrintVideoFormat(*pmtConfig);
                                        //debug>>

                                        vecVideoMediaType.push_back(vmt);

                                    }
                                }
                                
                            }//if

                            DeleteMediaType(pmtConfig);

                        }//for each image format
                    }//if
            }
        }
        else
        {
            break;
        }

    }while(0);
    
    return hr;

}