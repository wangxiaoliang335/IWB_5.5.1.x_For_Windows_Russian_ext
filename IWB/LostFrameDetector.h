#pragma once
//@功能:丢失帧数检测
class CLostFrameCountDetector
{
public:
	CLostFrameCountDetector()
		:
	m_lRecvFrameCount(0),
	m_lFPS(60)
	{
		QueryPerformanceFrequency(&m_liFrequency);
		m_liLastTime.QuadPart = 0;
		m_liBeginTime.QuadPart = 0;
		
	}

	
	~CLostFrameCountDetector()
	{


	}

	void Reset(LONG lFPS)
	{
	
		QueryPerformanceCounter(&m_liBeginTime);
		QueryPerformanceCounter(&m_liLastTime);
		m_lFPS = lFPS;
		m_lRecvFrameCount = 0;

	}

	void DoDetect()
	{
		LARGE_INTEGER liNow;
		QueryPerformanceCounter(&liNow);

		LONG lExpectedFrameCount = (LONG)((liNow.QuadPart - m_liBeginTime.QuadPart) * m_lFPS / m_liFrequency.QuadPart );

		LARGE_INTEGER liDelta ;
		liDelta.QuadPart = (liNow.QuadPart - m_liLastTime.QuadPart) * 1000/ m_liFrequency.QuadPart;//单位毫秒


		if(liDelta.QuadPart >=  16)
		{
			m_liLastTime.QuadPart = liNow.QuadPart;

#ifdef _LOG
		//LOG_INF("Expected Frame=%li, Actual Frame=%li, missing=%li",  lExpectedFrameCount, m_lRecvFrameCount, lExpectedFrameCount - m_lRecvFrameCount) ;
#endif
		}
		m_lRecvFrameCount++;
	}

protected:
	LARGE_INTEGER m_liFrequency    ;//
	LARGE_INTEGER m_liBeginTime    ;//开始统计时刻
	LARGE_INTEGER m_liLastTime     ;//上一次统计输出时刻
	LONG          m_lFPS           ; //正常帧率
	LONG          m_lRecvFrameCount;//帧计数器
};