#include "stdafx.h"
BOOL LoadTime(const char* lpszTime, TActionTime* pTime)
{
	enum
	{
		E_PARSE_HOUR,
		E_PARSE_MININUTE,
		E_PARSE_SECOND
	}eParseStage;

	eParseStage = E_PARSE_HOUR;

	BOOL bIsNumber = FALSE;
	BOOL bIsSeparator = FALSE;
	BOOL bIsWhiteSpace = FALSE;
	char hour[3], minute[3], second[3];
	int  nCharPos = 0;

	memset(hour, 0, sizeof(hour));
	memset(minute, 0, sizeof(minute));
	memset(second, 0, sizeof(second));

	while (*lpszTime)
	{
		bIsNumber = FALSE;
		bIsSeparator = FALSE;
		bIsWhiteSpace = FALSE;

		char c = *lpszTime;
		if ('0' <= c && c <= '9')
		{
			bIsNumber = TRUE;
		}
		else if (c == ':')
		{
			bIsSeparator = TRUE;
		}
		else if (c == ' ' || c == '\t')
		{
			bIsWhiteSpace = TRUE;
		}


		if (!bIsNumber && !bIsSeparator && !bIsWhiteSpace)
		{
			return FALSE;
		}

		switch (eParseStage)
		{
		case E_PARSE_HOUR:
			if (bIsNumber && nCharPos < sizeof(hour) - 1)
			{
				hour[nCharPos] = c;
				nCharPos++;
			}

			if (bIsSeparator)
			{
				nCharPos = 0;
				eParseStage = E_PARSE_MININUTE;
			}

			break;

		case E_PARSE_MININUTE:
			if (bIsNumber && nCharPos < sizeof(minute) - 1)
			{
				minute[nCharPos] = c;
				nCharPos++;
			}

			if (bIsSeparator)
			{
				nCharPos = 0;
				eParseStage = E_PARSE_SECOND;
			}

			break;

		case E_PARSE_SECOND:
			if (bIsNumber && nCharPos < sizeof(second) - 1)
			{
				second[nCharPos] = c;
				nCharPos++;
			}

			break;
		}

		lpszTime++;
	}


	if (pTime)
	{
		pTime->wHour = atoi(hour);
		pTime->wMinute = atoi(minute);
		pTime->wSecond = atoi(second);

	}

	return TRUE;
}


int ActionTimeInSeconds(const TActionTime& actionTime)
{
	int nSecond = int(actionTime.wHour) * 3600 +
		int(actionTime.wMinute) * 60 +
		int(actionTime.wSecond);

	return nSecond;
}

BOOL InActionTimeRange(const TActionTime& actionTime, const TActionTime& beginTime, const TActionTime& endTime)
{
	int actionTimeInSeconds = ActionTimeInSeconds(actionTime);
	int beginTimeInSeconds  = ActionTimeInSeconds(beginTime);
	int endTimeInSeconds    = ActionTimeInSeconds(endTime);

	if (endTimeInSeconds < beginTimeInSeconds)
	{//结束时刻小于开始时刻，意味着结束时刻在第二天
		endTimeInSeconds += 24 * 3600;
	}

	if (actionTimeInSeconds < beginTimeInSeconds)
	{
		actionTimeInSeconds += 24 * 3600;
	}


	if (actionTimeInSeconds < beginTimeInSeconds)
	{
		return FALSE;
	}

	if (actionTimeInSeconds > endTimeInSeconds)
	{
		return FALSE;
	}

	if (actionTimeInSeconds < beginTimeInSeconds) return FALSE;
	

	return TRUE;
}
CSensorTimerActionExecuter::CSensorTimerActionExecuter(CIWBSensor& sensor)
	:
	m_sensor(sensor),
	m_bRunning(FALSE)
{

}

void CSensorTimerActionExecuter::Run()
{
	if (!m_bRunning)
	{
		if (LoadTimerActionFile())
		{
			m_bRunning = TRUE;
		}
	}

	if (m_bRunning)
	{
		DoAction();
	}
}

//@功能:载入定时动作设置文件
BOOL CSensorTimerActionExecuter::LoadTimerActionFile()
{
	CString strActionFileName;
	strActionFileName.Format(_T(".\\Sensor%02d\\SensorTimerActions.xml"), m_sensor.GetID());

	m_timerActionEntries.clear();
	TiXmlDocument oXMLDoc;
	if (!oXMLDoc.LoadFile(CT2A(strActionFileName), TIXML_ENCODING_UTF8))
	{
		return FALSE;
	}

	TiXmlElement *pRootElement = oXMLDoc.RootElement();
	if (pRootElement == NULL)
	{
		return FALSE;
	}

	TiXmlNode *pNode = NULL;
	do
	{
		pNode = pRootElement->IterateChildren(pNode);
		if (NULL == pNode)
		{
			break;
		}
		const char* lpszElementName = pNode->Value();

		if (_stricmp(lpszElementName, "Action") == 0)
		{  //读取Action

			const char* time_start = ((TiXmlElement*)pNode)->Attribute("time_start");
			const char* time_end = ((TiXmlElement*)pNode)->Attribute("time_end");
			const char* brightness = ((TiXmlElement*)pNode)->Attribute("brightness");
			const char* gamma = ((TiXmlElement*)pNode)->Attribute("gamma");


			TTimerActionEntry entry;
			memset(&entry, 0, sizeof(entry));
			if (time_start)
			{

				TActionTime actionTime;
				if (LoadTime(time_start, &actionTime))
				{
					entry.beginTime = actionTime;
					entry.dwFlags |= TAE_FLAG_BEGIN_TIME;
				}
			}

			if (time_end)
			{
				TActionTime actionTime;
				if (LoadTime(time_end, &actionTime))
				{
					entry.endTime = actionTime;
					entry.dwFlags |= TAE_FLAG_END_TIME;
				}
			}

			if (brightness)
			{
				entry.brightness = atoi(brightness);
				entry.dwFlags |= TAE_FLAG_BRIGHTNESS;
			}

			if (gamma)
			{
				entry.gamma = atoi(gamma);
				entry.dwFlags |= TAE_FLAG_GAMMA;
			}

			m_timerActionEntries.push_back(entry);
		}


	} while (pNode);



	return TRUE;
}


BOOL CSensorTimerActionExecuter::DoAction()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	TActionTime current;
	current.wHour   = st.wHour;
	current.wMinute = st.wMinute;
	current.wSecond = st.wSecond;

	for (size_t i = 0; i < m_timerActionEntries.size(); i++)
	{

		if (!(m_timerActionEntries[i].dwFlags & TAE_FLAG_BEGIN_TIME)) continue;
		if (!(m_timerActionEntries[i].dwFlags & TAE_FLAG_END_TIME)) continue;

		const TActionTime& beginTime = m_timerActionEntries[i].beginTime;
		const TActionTime& endTime = m_timerActionEntries[i].endTime;

		if (InActionTimeRange(current, beginTime, endTime))
		{
			if (!m_timerActionEntries[i].bFininsed)
			{

				if (m_timerActionEntries[i].dwFlags & TAE_FLAG_BRIGHTNESS)
				{
					m_sensor.GetVideoPlayer()->SetCameraParams(m_timerActionEntries[i].brightness, VideoProcAmp_Brightness);
					LOG_INF("Timer Action to Set Brightness of Sensor%02d to %d", m_sensor.GetID(), m_timerActionEntries[i].brightness);
				}

				if (m_timerActionEntries[i].dwFlags & TAE_FLAG_GAMMA)
				{
					m_sensor.GetVideoPlayer()->SetCameraParams(m_timerActionEntries[i].gamma, VideoProcAmp_Gamma);
					LOG_INF("Timer Action Set Gamma of Sensor%02d to %d", m_sensor.GetID(), m_timerActionEntries[i].gamma);
				}

				m_timerActionEntries[i].bFininsed = TRUE;
			}
		}
		else
		{
			m_timerActionEntries[i].bFininsed = FALSE;
		}
	}


	return TRUE;
}
