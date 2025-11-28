#pragma once
#define TAE_FLAG_BEGIN_TIME 0x1
#define TAE_FLAG_END_TIME   0x2
#define TAE_FLAG_BRIGHTNESS 0x4
#define TAE_FLAG_GAMMA      0x8

struct TActionTime
{
	WORD wHour  ;//时
	WORD wMinute;//分
	WORD wSecond;//秒
};

struct TTimerActionEntry
{
	TActionTime beginTime;
	TActionTime endTime;
	int brightness;
	int gamma;
	BOOL bFininsed;//动作执行标志，保证在一个时段内动作只执行一次。
	DWORD dwFlags;//标志begin,end,brightness,gamma数据有效的标志为	
};

//@功能:传感器定时动作执行器
class CSensorTimerActionExecuter
{
public:
	CSensorTimerActionExecuter(CIWBSensor& sensor);
	
	void Run();
	
protected:
	//@功能:载入定时动作设置文件
	BOOL LoadTimerActionFile();
	
	BOOL DoAction();

	CIWBSensor& m_sensor;//关联的传感器
	BOOL       m_bRunning;//执行器运行标志
	std::vector<TTimerActionEntry> m_timerActionEntries;
};