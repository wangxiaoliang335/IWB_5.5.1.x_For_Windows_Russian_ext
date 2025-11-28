#include "UVCToEEPROM.h"
#include <ks.h>

//where KSPROPERTY_VIDEOPROCAMP_GAIN is defined
#include <ksmedia.h>
#pragma comment(lib, "strmiids.lib")

//CUVCToEEPROM::CComInitialize  CUVCToEEPROM::s_oComInitialize;
CUVCToEEPROM::CUVCToEEPROM()
{	
}

CUVCToEEPROM::~CUVCToEEPROM()
{
	CloseDevices();

}



//@功    能:查询并生成设备实例
//@输出参数:
//          nInstId, 指定从零开始的实例编号
//			ppInteface, COM 对象interface 
//          nInstCount，实例个数
              
HRESULT CUVCToEEPROM::QueryDevices(int nInstId, IAMVideoProcAmp** ppInteface, int* pInstCount)
{
	CComPtr <ICreateDevEnum> ptrDevEnum;
	CComPtr <IMoniker>       ptrMoniker;    
	CComPtr<IBaseFilter>     ptrFilter;
	CComPtr <IEnumMoniker>   ptrClassEnum;
	HRESULT hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
		IID_ICreateDevEnum, (void **) &ptrDevEnum);
	if (FAILED(hr))
	{    	 
		return hr;
	}

	// Create an enumerator for the video capture devices
	hr = ptrDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &ptrClassEnum, 0);
	if (FAILED(hr))
	{   	 
		return hr;
	}

	// If there are no enumerators for the requested type, then 
	// CreateClassEnumerator will succeed, but pClassEnum will be NULL.
	if (ptrClassEnum == NULL)
	{   	  
		return E_FAIL;
	}

	int nInstCount = 0;
	ULONG celtFeteched;

	BOOL bLoop = TRUE;

	while(bLoop && S_OK == (ptrClassEnum->Next(1, &ptrMoniker, &celtFeteched)))
	{
		CComPtr<IPropertyBag> ptrPropertyBag=0;

		hr = ptrMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&ptrPropertyBag);

		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = ptrPropertyBag->Read(L"DevicePath", &var, NULL);

			if(wcsstr(var.bstrVal,L"vid_18ec"))
			{
				hr = ptrMoniker->BindToObject(0,0,IID_IBaseFilter, (void**)&ptrFilter);

				if(FAILED(hr))
				{
					continue;
				}

				
				if(nInstId != -1 && nInstId == nInstCount && ppInteface)
				{
					hr = ptrFilter->QueryInterface(IID_IAMVideoProcAmp,(void **)ppInteface);

					if(SUCCEEDED(hr))
					{
						bLoop = FALSE;
						hr = S_OK;
					}

				}
				nInstCount ++;

				ptrFilter = NULL;
			}

		}

		ptrMoniker = NULL;

	}//while


	if(pInstCount) *pInstCount = nInstCount;
	return hr;

}

HRESULT CUVCToEEPROM::OpenDevice(int nInstNo)
{

	m_ptrAMVideoProcAmp = NULL;
	
	return QueryDevices(nInstNo, &m_ptrAMVideoProcAmp, NULL);

	
}


void CUVCToEEPROM::CloseDevices()
{
	//for(int i = 0; i < MAX_INSTANCE; i++) 
	//{
	//	m_aryPtrAMVideoProcAmp[i] = NULL;
	//}
	m_ptrAMVideoProcAmp = NULL;

}



//@功能:设备实例数目
int CUVCToEEPROM::GetInstanceCount() 
{

	int nCount = 0;
	QueryDevices(-1, NULL, &nCount);
	return nCount;
}



//@功能:写入数据到EEPROM
//@参数:offset, 写入EEPROM的偏移地址
//      buf, 写入数据的缓冲区地址
//      nLength, 写入数据的字节长度，必须是2的整数倍
//@返回值:成功, TRUE
//        失败, FALSE
BOOL CUVCToEEPROM::Write(USHORT offset, const BYTE *buf, int nLength, int instance)
{
	HRESULT hr = S_FALSE;
	if(m_ptrAMVideoProcAmp == NULL)
	{
		hr = OpenDevice(instance);
		if(FAILED(hr))
		{
			return FALSE;
		}
	}

	int nTryCount = 0;
	do
	{
		//写数据发送的报文格式
		//55aa, 握手标志
		//88aa, 写E2ROM标志
		//数据长度, 2字节, Big Endian, Low-Byte=MSB(Length) High-Byte = LSB(Length)
		//延时,2字节, Big Endian, Low-Byte=MSB(Length) High-Byte = LSB(Length)
		//地址偏移量, 2字节，Big Endian, Low-Byte=MSB(Length) High-Byte = LSB(Length)		
		hr = m_ptrAMVideoProcAmp->Set(KSPROPERTY_VIDEOPROCAMP_GAIN, 0x55aa, VideoProcAmp_Flags_Manual);
		if (hr != S_OK)
		{
			//调试发现,写入的时候有时会出现设备未就绪的错误，
			//通过延时等待解决
			if(hr == HRESULT_FROM_WIN32(ERROR_NOT_READY))
			{
				const int MAX_WAIT_COUNT = 10;
				int nWaitCount  = 0;
				while(nWaitCount < MAX_WAIT_COUNT)
				{
					Sleep(10);
					
					hr = m_ptrAMVideoProcAmp->Set(KSPROPERTY_VIDEOPROCAMP_GAIN, 0x55aa, VideoProcAmp_Flags_Manual);
					if(hr != HRESULT_FROM_WIN32(ERROR_NOT_READY))
					{
						break;
					}
					nWaitCount++;
				}


			}
			else
			{
				goto _handle_write_error_;
			}
		}



		hr = m_ptrAMVideoProcAmp->Set(KSPROPERTY_VIDEOPROCAMP_GAIN, 0x88aa, VideoProcAmp_Flags_Manual);
		if (hr != S_OK)
		{
			goto _handle_write_error_;
		}


		WORD wLength = (nLength << 8) | (nLength >> 8);
		hr = m_ptrAMVideoProcAmp->Set(KSPROPERTY_VIDEOPROCAMP_GAIN, wLength, VideoProcAmp_Flags_Manual);
		if (hr != S_OK)
		{
			goto _handle_write_error_;
		}


		hr = m_ptrAMVideoProcAmp->Set(KSPROPERTY_VIDEOPROCAMP_GAIN, 0x0001, VideoProcAmp_Flags_Manual);
		if (hr != S_OK)
		{
			goto _handle_write_error_;
		}


		//I2C先送EEPROM的高地址。
		WORD wAddr = (offset << 8) | (offset >> 8);
		hr = m_ptrAMVideoProcAmp->Set(KSPROPERTY_VIDEOPROCAMP_GAIN, wAddr, VideoProcAmp_Flags_Manual);
		if (hr != S_OK)
		{
			goto _handle_write_error_;
		}



		int nByteLengthLeft = nLength;


		const USHORT* pData = (const USHORT*)buf;
		while (nByteLengthLeft >= 2)
		{

			long value = long(*pData);
			pData++;
			long flags = 0;
			hr = m_ptrAMVideoProcAmp->Set(KSPROPERTY_VIDEOPROCAMP_GAIN, value, VideoProcAmp_Flags_Manual);
			if (hr != S_OK)
			{
				goto _handle_write_error_;
			}
			nByteLengthLeft -= 2;
		}



		if (nByteLengthLeft == 1)
		{
			long value = long(buf[nLength - 1]);
			long flags = 0;
			hr = m_ptrAMVideoProcAmp->Set(KSPROPERTY_VIDEOPROCAMP_GAIN, value, VideoProcAmp_Flags_Manual);
			if (hr != S_OK)
			{
				goto _handle_write_error_;
			}
		}

		long value = 0, flag = 0;

		//读取发送结果
		//成功返回0x0001, 失败返回0x0000.
		hr = m_ptrAMVideoProcAmp->Get(KSPROPERTY_VIDEOPROCAMP_GAIN, (long *)&value, &flag);
		if (hr == S_OK && value != 0x01)
		{
			hr = E_FAIL;
			
		}
			
//0x80070015
	_handle_write_error_:
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{//处理设备丢失的情形，再重新打开一次设备
			nTryCount++;
			hr = OpenDevice(instance);
			if (FAILED(hr))
			{
				break;
			}
		}
		else
		{
			break;
		}


	} while (nTryCount < 2);

	return hr == S_OK;
}





//@功能:从EEPROM读数据
//@参数:offset, 读取的EEPROM内容的偏移地址
//      buf, 保存读取数据的缓冲区地址
//      nLength, 读取数据的字节个数，必须是2的整数倍
//@返回值:成功, TRUE
//        失败, FALSE
BOOL CUVCToEEPROM::Read(USHORT offset, BYTE *buf, int nLength, int instance)
{
	HRESULT hr = S_FALSE;
	if(m_ptrAMVideoProcAmp == NULL)
	{
		hr = OpenDevice(instance);
		if(FAILED(hr))
		{
			return FALSE;
		}
	}

	if(m_ptrAMVideoProcAmp == NULL)
	{
		return FALSE;
	}

	int nTryCount = 0;
	do
	{
		//读数据发送的报文格式
		//55aa, 握手标志
		//89aa, 写E2ROM标志
		//数据长度, 2字节, Big Endian, Low-Byte=MSB(Length) High-Byte = LSB(Length)
		//延时,2字节, Big Endian, Low-Byte=MSB(Length) High-Byte = LSB(Length)
		//地址偏移量, 2字节，Big Endian, Low-Byte=MSB(Length) High-Byte = LSB(Length)

		hr = m_ptrAMVideoProcAmp->Set(KSPROPERTY_VIDEOPROCAMP_GAIN, 0x55aa, VideoProcAmp_Flags_Manual);
		if (hr != S_OK)
		{
			goto _handle_read_error_;
		}

		hr = m_ptrAMVideoProcAmp->Set(KSPROPERTY_VIDEOPROCAMP_GAIN, 0x89aa, VideoProcAmp_Flags_Manual);
		if (hr != S_OK)
		{
			goto _handle_read_error_;
		}



		WORD wLength = (nLength << 8) | (nLength >> 8);
		hr = m_ptrAMVideoProcAmp->Set(KSPROPERTY_VIDEOPROCAMP_GAIN, wLength, VideoProcAmp_Flags_Manual);
		if (hr != S_OK)
		{
			goto _handle_read_error_;
		}


		hr = m_ptrAMVideoProcAmp->Set(KSPROPERTY_VIDEOPROCAMP_GAIN, 0x0001, VideoProcAmp_Flags_Manual);
		if (hr != S_OK)
		{
			goto _handle_read_error_;
		}


		//I2C先送EEPROM的高地址。
		WORD wAddr = (offset << 8) | (offset >> 8);
		hr = m_ptrAMVideoProcAmp->Set(KSPROPERTY_VIDEOPROCAMP_GAIN, wAddr, VideoProcAmp_Flags_Manual);
		if (hr != S_OK)
		{
			goto _handle_read_error_;
		}

		long value = 0;
		long flags = 0;

		//Read two bytes each time
		int nByteLength = nLength;

		USHORT* pData = (USHORT*)buf;
		while (nByteLength >= 2)
		{
			hr = m_ptrAMVideoProcAmp->Get(KSPROPERTY_VIDEOPROCAMP_GAIN, (long *)&value, &flags);
			if (hr != S_OK)
			{
				goto _handle_read_error_;
			}
			*pData++ = (USHORT)value;
			nByteLength -= 2;

		}


		BYTE c = 0;
		if (nByteLength == 1)
		{
			hr = m_ptrAMVideoProcAmp->Get(KSPROPERTY_VIDEOPROCAMP_GAIN, (long *)&value, &flags);
			if (hr != S_OK)
			{
				goto _handle_read_error_;
			}
			buf[nLength - 1] = value & 0xFF;//最后一个读取数据
			c = (value >> 8) & 0xFF;
		}




		//read checksum
		hr = m_ptrAMVideoProcAmp->Get(KSPROPERTY_VIDEOPROCAMP_GAIN, (long *)&value, &flags);
		if (hr != S_OK)
		{
			goto _handle_read_error_;
		}

		WORD wReadCheckSum = WORD(value << 8 | value >> 8);

		//calculate checksum
		WORD wCalcCheckSum = 0;
		for (int i = 0; i < nLength; i++)
		{
			wCalcCheckSum += (WORD)buf[i];
		}

		wCalcCheckSum += c;

		//读取的校验码和计算的不一致则失败
		if (wReadCheckSum != wCalcCheckSum)
		{

			hr = E_FAIL;
		}

		
	_handle_read_error_:
		if(hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{//处理设备丢失的情形，再重新打开一次设备
			nTryCount++;
			hr = OpenDevice(instance);
			if (FAILED(hr))
			{
				break;
			}
		}
		else
		{
			break;
		}


	}while (nTryCount < 2);

	return hr == S_OK;
}

