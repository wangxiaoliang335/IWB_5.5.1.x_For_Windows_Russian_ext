#pragma once
//@Comments:
// USB摄像头设备列表类，负责检索系统中的符合候选标准的视频设备
//
//
class CUsbCameraDeviceList
{
public:
	CUsbCameraDeviceList();
	~CUsbCameraDeviceList();


	//@功能:根据视频格式名称获取指定设备的视频格式数据结构
	const VideoMediaType* CUsbCameraDeviceList::GetVideoFormat(LPCTSTR lpszDevicePath, LPCTSTR lpszVideoFormatName);


	//@功能:更新设备列表
	void UpdateDeviceList();


	//@功能:返回设备实例的个数
	UINT GetDeviceInstanceCount()const;

	
	//@功能:从设备实例数组中, 返回指定下标的设备实例
	//@参数:nIndex, 设备实例的索引号,从0开始
	//@返回:指向设备实例结构的指针
	const TCaptureDeviceInstance* GetCaptureDeviceInstance(UINT nIndex)const;


	//@功能:判断指定的设备路径是否在列表中存在
	//@参数:lpszDevicePath, 要匹配的设备路径字符串
	//@返回:TRUE, 在列表中存在指定的设备路径。
	//      FALSE, 在列表中未找到匹配的条目。
	BOOL IsDevicePathExists(LPCTSTR lpszDevicePath);


    //@功能:根据设备路径返回设备实例数据
	//@参数:
	//@返回:指向设备实例结构的指针
	const TCaptureDeviceInstance* GetCaptureDeviceInstance(LPCTSTR lpszDevicePath)const;

    

    //@功能:根据设备路径判断是否是候选设备
    //@参数:lpszDevicePath, 设备路径
    BOOL IsCandidateDevice(LPCTSTR lpszDevicePath);
	

protected:
	//@功能:枚举所有系统中已按装的候选设备
	BOOL EnumAllCandidateDevice();

	//@功能:更新候选设备信息
	BOOL UpdateCandidateDeviceInfo();

	std::vector<CAtlString>                m_aryCandidateDeviceInfo  ;//候选设备信息
	std::vector<TCaptureDeviceInstance>    m_aryCaptureDeviceInstance;//视频捕获设备实例

};