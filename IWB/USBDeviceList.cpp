#include "stdafx.h"
//#include "headers.h"
CUsbCameraDeviceList::CUsbCameraDeviceList()
{
    CoInitialize(NULL);
    UpdateCandidateDeviceInfo();

}

CUsbCameraDeviceList::~CUsbCameraDeviceList()
{
    CoUninitialize();
}


//@功能:更新候选设备信息
BOOL CUsbCameraDeviceList::UpdateCandidateDeviceInfo()
{
	m_aryCandidateDeviceInfo.clear();
	
	//缺省候选设备信息
		
	//从CameraDevice.xml中载入附加的候选设备信息

	const TCHAR* lpszXMLFileName = _T("CameraDevice.xml");

	TiXmlDocument oXMLDoc;

	if(!oXMLDoc.LoadFile(CT2CA(lpszXMLFileName), TIXML_ENCODING_UTF8))
	{
		return FALSE;
	}

	TiXmlElement* pRootElement = oXMLDoc.RootElement();

	if(pRootElement == NULL) return FALSE;

	TiXmlNode* pChild = NULL;
	
	while(pChild = pRootElement->IterateChildren("USBCamera", pChild))
	{	
		const char* NodeName = pChild->Value();//节点名称
		const char* lpszVID  = ((TiXmlElement*)pChild)->Attribute("vid");
		const char* lpszPID  = ((TiXmlElement*)pChild)->Attribute("pid");

		if(lpszVID == NULL || strlen(lpszVID) == 0 || lpszPID == NULL || strlen(lpszPID) == 0)
		{
			break;
		}

		CAtlString strDevicePathInfo;

		strDevicePathInfo.Format(_T("\\\\?\\usb#vid_%s&pid_%s"), (LPCTSTR)(CA2T(lpszVID)), (LPCTSTR)(CA2T(lpszPID)));
		m_aryCandidateDeviceInfo.push_back(strDevicePathInfo);
	}

	return TRUE;
}


//@功能:更新设备列表
void CUsbCameraDeviceList::UpdateDeviceList()
{
	//清空已有设备列表
	m_aryCaptureDeviceInstance.clear();

	//查找所有视频输入设备
	CaptuerDeviceInstanceSet instanceSet;
	
	//枚举视频捕获设备
	EnumVideoCaptureDevice(instanceSet);

	
	//根据候选信息筛选
	for(UINT i=0; i < instanceSet.size(); i++)
	{
		const TCaptureDeviceInstance& instance = instanceSet[i];
#ifdef _LOG
        LOG_INF("(%s):Find Capture Device %s", __FUNCTION__,(const char*)CT2CA(instance.m_strDevPath));
#endif 

		for(UINT j=0; j < m_aryCandidateDeviceInfo.size(); j++)		
		{
			const CAtlString& strInfo = m_aryCandidateDeviceInfo[j];

			if(_tcsnicmp(instance.m_strDevPath, strInfo, strInfo.GetLength()) == 0)
			{
				m_aryCaptureDeviceInstance.push_back(instance);
				//break;
			}
		}

	}

}

//@功能:返回设备实例的个数
UINT CUsbCameraDeviceList::GetDeviceInstanceCount()const
{
	return m_aryCaptureDeviceInstance.size();
}


//@功能:枚举所有候选设备
BOOL CUsbCameraDeviceList::EnumAllCandidateDevice()
{
	return TRUE;
}


//@功能:从设备实例数组中, 返回指定下标的设备实例
//@参数:nIndex, 设备实例的索引号
//@返回:指向设备实例结构的指针
const TCaptureDeviceInstance* CUsbCameraDeviceList::GetCaptureDeviceInstance(UINT nIndex)const
{
	const TCaptureDeviceInstance* pDevInst = NULL;

	if(nIndex < m_aryCaptureDeviceInstance.size()) 
	{
		pDevInst = &m_aryCaptureDeviceInstance[nIndex];
	}

	return pDevInst;
}

//@功能:判断指定的设备路径是否在列表中存在
//@参数:lpszDevicePath, 要匹配的设备路径字符串
//@返回:TRUE, 在列表中存在指定的设备路径。
//      FALSE, 在列表中未找到匹配的条目。
BOOL CUsbCameraDeviceList::IsDevicePathExists(LPCTSTR lpszDevicePath)
{
	BOOL bFound = FALSE;

	for(UINT i=0; i < m_aryCaptureDeviceInstance.size(); i++)
	{
		const TCaptureDeviceInstance& instance = m_aryCaptureDeviceInstance[i];

			if(_tcsicmp(instance.m_strDevPath, lpszDevicePath) == 0)
			{
				bFound = TRUE;
				break;
			}			
	}
	return bFound;
}

//@功能:根据设备路径返回设备实例数据
//@参数:
//@返回:指向设备实例结构的指针
const TCaptureDeviceInstance* CUsbCameraDeviceList::GetCaptureDeviceInstance(LPCTSTR lpszDevicePath)const
{
	BOOL bFound = FALSE;
	for(UINT i=0; i < m_aryCaptureDeviceInstance.size(); i++)
	{
		const TCaptureDeviceInstance& instance = m_aryCaptureDeviceInstance[i];
		if(_tcsicmp(instance.m_strDevPath, lpszDevicePath) == 0)
		{
			return &instance;			
		}
	}
    return NULL;
}

//@功能:根据视频格式名称获取指定设备的视频格式数据结构
const VideoMediaType* CUsbCameraDeviceList::GetVideoFormat(LPCTSTR lpszDevicePath, LPCTSTR lpszVideoFormatName)
{
	const VideoMediaType* pVMT = 0;
	for(UINT i=0; i < m_aryCaptureDeviceInstance.size(); i++)
	{
		const TCaptureDeviceInstance& instance = m_aryCaptureDeviceInstance[i];
		if(_tcsicmp(instance.m_strDevPath, lpszDevicePath) == 0)
		{
			UINT  fmtCount = instance.m_vecVideoFmt.size();
			for(UINT j=0; j<fmtCount; j++)
			{
				CString strFmtName= GetVideoFormatName(instance.m_vecVideoFmt[j]);
				if(strFmtName == lpszVideoFormatName)
				{
					pVMT = &instance.m_vecVideoFmt[j];			
					break;
				}
			}//for j;		
			break;
		 }			
	 }
	 return pVMT;

}

//@功能:根据设备路径判断是否是候选设备
//@参数:lpszDevicePath, 设备路径
BOOL CUsbCameraDeviceList::IsCandidateDevice(LPCTSTR lpszDevicePath)
{
    for(UINT j=0; j < m_aryCandidateDeviceInfo.size(); j++)
    {
        const CAtlString& strInfo = m_aryCandidateDeviceInfo[j];

        if(_tcsnicmp(lpszDevicePath, strInfo, strInfo.GetLength()) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}





