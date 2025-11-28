#pragma once

//光亮斑位置信息
struct TLightSpot
{
    int mass                  ;//质量
	//int mx                    ;//对x轴的力矩, 合并光斑时需要用到
    //int my                    ;//对y轴的力矩,,合并光斑时需要用到

	//POINT ptPosInVideo         ;//光笔触板坐标(视频坐标),放大2^INT_SCALE_SHIFT_SIZ倍
	//RECT rcPosInVideo          ;//区域位置(视频坐标), 原始数据未放大
  
	//LONG AreaInVideo         ;//光斑的实际面积(视频坐标)
	LONG lStdSpotAreaInVideo   ;//光斑所处位置的标准面积(视频坐标)
    POINT ptPosInScreen        ;//重心坐标(屏幕坐标)


    
	bool operator < (TLightSpot& right)
	{
		return (mass < right.mass);
	}

	bool operator <= (TLightSpot& right)
	{
		return (mass <= right.mass);
	}


	bool operator > (TLightSpot& right)
	{
		return (mass > right.mass);
	}

	bool operator >= (TLightSpot& right)
	{
		return (mass >= right.mass);
	}

};