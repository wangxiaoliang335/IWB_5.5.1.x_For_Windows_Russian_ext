#pragma once
//CComPtr<>
#include <atlbase.h>

//IAMVideoProcAmp is define <strmif.h>
//<strmif.h> is include in <dshow.h>
#include <dshow.h>

//Chip ARK3389 write and read EPROM  by UVC Camera Unit VC_PROCESSING_UNIT with control selector PU_GAIN_CONTROL.
class CUVCToEEPROM
{
public:
	 CUVCToEEPROM();	
	~CUVCToEEPROM();


	//@功能:写入数据到EEPROM
	//@参数:offset, 写入EEPROM的偏移地址
	//      buf, 写入数据的缓冲区地址
	//      nLength, 写入数据的字节长度，必须是2的整数倍
	//      instance, 设备实例编号
	//@返回值:成功, TRUE
	//        失败, FALSE
	BOOL Write(USHORT offset, const BYTE *buf, int nLength, int instance = 0);
		

	//BOOL Write2(USHORT addr, const BYTE *buf, int nLength);

	//@功能:从EEPROM读数据
	//@参数:offset, 读取的EEPROM内容的偏移地址
	//      buf, 保存读取数据的缓冲区地址
	//      nLength, 读取数据的字节个数，必须是2的整数倍
	//      instance, 设备实例编号
	//@返回值:成功, TRUE
	//        失败, FALSE
	BOOL Read(USHORT offset, BYTE *buf, int nLength, int instance = 0);


	//@功能:设备实例数目
	static int GetInstanceCount() ;
		

	//@功能:打开指定的设备实例
	//@参数:instance, 设备实例id(0~MAX_INSTANCE)
	HRESULT OpenDevice(int instance);

	

protected:
	//@功    能:查询并生成设备实例
	//@输出参数:
	//          nInstId, 指定从零开始的实例编号
	//			ppInteface, COM 对象interface 
	//          pInstCount，实例个数
	static HRESULT QueryDevices(int nInstId, IAMVideoProcAmp** ppInteface, int* pInstCount);

	

	//@功能:关闭指定的设备实例
	//@参数:instance,, 设备实例id(0~MAX_INSTANCE)	
	void CloseDevices();

	//最大设备实例数目
	static const int MAX_INSTANCE = 10;

	//struct DeviceInstanceInfo
	//{
	//	TCHAR  devicePath[MAX_PATH];
	//	CComPtr<IBaseFilter>  ptrFilter;
	//};


	//同一台计算机上可能有n个设备实例
	//CComPtr<IAMVideoProcAmp> m_aryPtrAMVideoProcAmp[MAX_INSTANCE];
	CComPtr<IAMVideoProcAmp> m_ptrAMVideoProcAmp;

	//DeviceInstanceInfo m_aryDeviceInstances[MAX_INSTANCE]; 
	class CComInitialize
	{
	public:
		CComInitialize()
		{
			CoInitialize(NULL);
		}
		~CComInitialize()
		{
			CoUninitialize();
		}

	};

	//static CComInitialize s_oComInitialize;
};