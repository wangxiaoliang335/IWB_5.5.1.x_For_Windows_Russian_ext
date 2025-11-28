
#pragma once
struct MonitorDescritpor
{


};

struct TDetailedTimesDescription
{


};

struct TMonitorDescriptor
{
	enum E_MONITOR_DESCRIPTOR_DATA_TYPE
	{
		E_DESCRIPTOR_DATA_TYPE_SERIAL_NUMBER                = 0xFF,//FFh: Monitor Serial Number - Stored as ASCII, code page # 437, ≤ 13 bytes.
		E_DESCRIPTOR_DATA_TYPE_ASCII_STRING                 = 0xFE,//FEh: ASCII String - Stored as ASCII, code page # 437, ≤ 13 bytes.
		E_DESCRIPTOR_DATA_TYPE_RANGE_LIMITS                 = 0xFD,//FDh: Monitor range limits, binary coded
		E_DESCRIPTOR_DATA_TYPE_MONITOR_NAME                 = 0xFC,//FCh : Monitor name, stored as ASCII, code page # 437
		E_DESCRIPTOR_DATA_TYPE_COLOR_POINT_DATA             = 0xFB,//FBh: Descriptor contains additional color point data
		E_DESCRIPTOR_DATA_TYPE_STANDARD_TIME_IDENTIFICATION = 0xFA,//FAh : Descriptor contains additional Standard Timing Identifications
		E_DESCRIPTOR_DATA_TYPE_UNDIFINED                    = 0xF9,//F9h - 11h : Currently undefined
		E_DESCRIPTOR_DATA_TYPE_DUMMY_DESCRITPOR             = 0x10,//10h : Dummy descriptor, used to indicate that the descriptor space is unused
		E_DESCRIPTOR_DATA_TYPE_DESCRITPOR_BY_MANUFACTURER   = 0x0F //0Fh - 00h : Descriptor defined by manufacturer.
	}eMonitorDescritporDataType;

	union
	{
		CHAR szSerialNumber[13 + 1];
		CHAR szASCIIString [13 + 1];
		CHAR szMonitorName[13 + 1];
	};
	
};

struct TBlockData
{
	enum E_BLOCK_TYPE
	{
		E_BLOCK_TYPE_DETAILED_TIMING_DESCRIPTION = 1,
		E_BLOCK_TYPE_MONITOR_DESCRIPTOR = 2
	}eBlockType;

	union
	{
		TDetailedTimesDescription detailedTimeDescription;
		TMonitorDescriptor monitorDescriptor;
	};

};


struct EDIDInfo
{
	BYTE      header[8];
	CHAR      szvendor[3 + 1];
	WORD      wProductId;
	DWORD     dwSerialId;
	BYTE      cWeek;//Week number of Model Year Flag
	WORD      wYear;//
	BYTE      checkSum;
	TDetailedTimesDescription detailedTimesDescription0;
	TBlockData                blockDatas[3];

	const char* GetSerialNumber()
	{
		for (int i = 0; i < _countof(blockDatas); i++)
		{
			if (blockDatas[i].eBlockType == TBlockData::E_BLOCK_TYPE::E_BLOCK_TYPE_MONITOR_DESCRIPTOR)
			{
				if (blockDatas[i].monitorDescriptor.eMonitorDescritporDataType
					==
					TMonitorDescriptor::E_MONITOR_DESCRIPTOR_DATA_TYPE::E_DESCRIPTOR_DATA_TYPE_SERIAL_NUMBER)
				{
					return blockDatas[i].monitorDescriptor.szSerialNumber;

				}

			}

		}
	
		return "";
	}



};

//EDID: Extended Display Identification Data
class CEDID
{
public:

	CEDID()
	{
		memset(m_data, 0, sizeof(m_data));
	}
	int Set(const BYTE* pNewData, DWORD dwSize)
	{
		if (dwSize > _countof(m_data))
		{
			dwSize = _countof(m_data);
		}

		memcpy(m_data, pNewData, dwSize);

		return dwSize;
	}

	BYTE* GetData()
	{
		return m_data;
	}

	DWORD GetSize()
	{
		return _countof(m_data);
	}


	BOOL GetPhysicalDimension(WORD* pwWidthInMm, WORD* pwHeightInMm)
	{
		//EDID Detailed Timing Descriptor
		//Horizontoal Display Size in mm.
		if (pwWidthInMm) *pwWidthInMm = ((WORD)(m_data[68] & 0xF0) << 4) + m_data[66];

		//Vertical Display Size in mm.
		if (pwHeightInMm) *pwHeightInMm = ((WORD)(m_data[68] & 0x0F) << 8) + m_data[67];

		return TRUE;
	}

	BOOL GetOptimalImageSize(WORD* pwHorizontalActivePixel, WORD* pwVerticalActivePixel)
	{
		//The first Detailed Timing shall only be used to indicate the mode that the monitor vendor has determined will give
		//an optimal image.
		const int OFFSET = 54;
		if (pwHorizontalActivePixel)
		{
			*pwHorizontalActivePixel = ((WORD)(m_data[OFFSET + 4] & 0xF0) << 4) + m_data[OFFSET + 2];
		}

		if (pwVerticalActivePixel)
		{
			*pwVerticalActivePixel = ((WORD)(m_data[OFFSET + 7] & 0xF0) << 4) + m_data[OFFSET + 5];
		}


		return TRUE;
	}

	void GetProductWeekAndYear(BYTE& week, BYTE& year)const
	{


	}

	void ParseBlockData(int nOffset, TBlockData& block)const
	{
		if (
			m_data[nOffset + 0] == 0x00
			&&
			m_data[nOffset + 1] == 0x00
			&&
			m_data[nOffset + 2] == 0x00
			&&
			m_data[nOffset + 4] == 0x00)
		{
			//There is an 18-Byte Monitor Descriptor
			//FFh: Monitor Serial Number - Stored as ASCII, code page # 437, ≤ 13 bytes.
			//FEh: ASCII String - Stored as ASCII, code page # 437, ≤ 13 bytes.
			//FDh: Monitor range limits, binary coded
			//FCh : Monitor name, stored as ASCII, code page # 437
			//FBh: Descriptor contains additional color point data
			//FAh : Descriptor contains additional Standard Timing Identifications
			//F9h - 11h : Currently undefined
			//10h : Dummy descriptor, used to indicate that the descriptor space is unused
			//0Fh - 00h : Descriptor defined by manufacturer.

			block.eBlockType = TBlockData::E_BLOCK_TYPE_MONITOR_DESCRIPTOR;
			block.monitorDescriptor.eMonitorDescritporDataType = (TMonitorDescriptor::E_MONITOR_DESCRIPTOR_DATA_TYPE)m_data[nOffset + 3];
			switch (block.monitorDescriptor.eMonitorDescritporDataType)
			{
			case TMonitorDescriptor::E_MONITOR_DESCRIPTOR_DATA_TYPE::E_DESCRIPTOR_DATA_TYPE_SERIAL_NUMBER:
				//序列号
				memcpy(block.monitorDescriptor.szSerialNumber, &m_data[nOffset + 5], 13);
				block.monitorDescriptor.szSerialNumber[13] = '\0';
				{//剔除结尾的空格和换行付
					int i = 12;
					while (i)
					{
						if(block.monitorDescriptor.szSerialNumber[i] != '\x0a'
							&&
							block.monitorDescriptor.szSerialNumber[i] != '\x20'
							&&
							block.monitorDescriptor.szSerialNumber[i] != '\x0d')
						{
							break;
						}
						else
						{
							block.monitorDescriptor.szSerialNumber[i] = '\x00';
						}

						i--;
					}
				}

				break;

			case TMonitorDescriptor::E_MONITOR_DESCRIPTOR_DATA_TYPE::E_DESCRIPTOR_DATA_TYPE_RANGE_LIMITS:
				//显示器范围限制


				break;

			}//switch

		}
		else
		{//There is an 18-Byte Detailed Timing Descriptor Block



		}



	}
 
	BOOL Parse(EDIDInfo& result)const
	{
		memset(&result, 0, sizeof(result));
		//1.header
		memcpy(result.header, &m_data[0], 8);

		//2.id_vendor
		result.szvendor[0] = ((m_data[8] >> 2) & 0x1F) + 0x40;
		result.szvendor[1] = (((m_data[8] << 3) & 0x18) | (m_data[9] >> 5 )) + 0x40;
		result.szvendor[2] = (m_data[9] & 0x1F) + 0x40;

		//3.id_product
		result.wProductId = (m_data[0x0B] << 8) | m_data[0x0A];

		//4.id_serial
		result.dwSerialId = (m_data[0x0F] << 24) | (m_data[0x0E] << 16) | (m_data[0x0D] << 8) | m_data[0x0C];

		//5. Product Week
		result.cWeek = m_data[0x10];

		//6.Proudct Year
		result.wYear = m_data[0x11] + 1990u;


		//
		result.checkSum = m_data[0x7F];

		//ParseDetailedTimeDescription(0x36, result.[0]);
		ParseBlockData(0x48, result.blockDatas[0]);
		ParseBlockData(0x5A, result.blockDatas[1]);
		ParseBlockData(0x6C, result.blockDatas[2]);

		return TRUE;
	}

protected:

	BYTE m_data[128];
};