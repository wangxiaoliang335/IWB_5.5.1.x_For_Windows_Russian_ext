#pragma once
//速度补偿
//根据检测到的光点位置,实时动态补偿光点位置
//
class CVelocityCompensator
{
public:
	CVelocityCompensator()
	{
		Reset();
	}

	//@功能:运动补偿复位
	void Reset()
	{
		m_Vx = 0;
		m_Vy = 0;

		m_nRunTimes = 0;
	}

	POINT Compensate(POINT& ptNow)
	{
		POINT ptResult;

		if(0 == m_nRunTimes)
		{

			ptResult =  ptNow;
		}
		else
		{	
			long Vx = (ptNow.x - m_ptLastPos.x) << SCALE_SHIFT_SIZE;
            long Vy = (ptNow.y - m_ptLastPos.y) << SCALE_SHIFT_SIZE;

            

			long SmoothFactor1 = 20;
			long SmoothFactor2 = 80;
			m_Vx = (Vx * SmoothFactor1  + m_Vx*SmoothFactor2)/100;
			m_Vy = (Vy * SmoothFactor1  + m_Vy*SmoothFactor2)/100;

            ptResult =  ptNow;


			const int N_FRAME = 3;

            //如果速度过小，则不作补偿
            if(Vx >= (5<<SCALE_SHIFT_SIZE) || Vy >= (5<<SCALE_SHIFT_SIZE))
            {
			    ptResult.x = (m_Vx >> SCALE_SHIFT_SIZE)*N_FRAME + ptNow.x;//提前补偿N帧的移动距离
                ptResult.y = (m_Vy >> SCALE_SHIFT_SIZE)*N_FRAME + ptNow.y;//提前补偿N帧的移动距离
            }
			

			AtlTrace(_T("Vx = %d, Vy= %d\n"), m_Vx >> SCALE_SHIFT_SIZE , m_Vy>>SCALE_SHIFT_SIZE);

		}
		
		m_ptLastPos = ptNow;

		m_nRunTimes ++;

		return ptResult;
	}

protected:
	POINT m_ptLastPos;
	long  m_Vx;//水平速度,扩大64倍
	long  m_Vy;//垂直速度,,扩大64倍
	static const int SCALE_SHIFT_SIZE = 6;
	int  m_nRunTimes;//运行次数


};