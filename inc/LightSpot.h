#pragma once
#include "../inc/MultiDimensionPoint.h"
//光亮斑位置信息
struct TLightSpot
{
    int mass                    ;//质量
	int mx                    ;//对x轴的力矩, 合并光斑时需要用到
    int my                    ;//对y轴的力矩,,合并光斑时需要用到

	TPoint2D pt2dPosInVideo   ;//光斑在视频中的坐标
	LONG lAreaInVideo          ;//光斑的外接矩形面积(视频坐标)
	LONG lStdSpotAreaInVideo   ;//光斑所处位置的标准面积(视频坐标)
    POINT ptPosInScreen        ;//重心坐标(屏幕坐标)

    int  nMonitorId;

    //辅助数据结构
    struct  TAux
    {
		TAux()
		{
			uMergeAreaIndex  = -1;
			//bBeyondMergeArea = false;
            bOutsideOwnedArea = false;
		}

        UINT uMergeAreaIndex      ;//光斑所在的融合区索引号。
                                    //-1, 不在任何融合区
                                    //>=0, 融合区索引号
        //bool bBeyondMergeArea      ;//位置超越融合区标志

        bool bOutsideOwnedArea     ;//在相机管辖的屏幕区域以外标志
    }aux;

    //<<debug
    DWORD dwCameraId;//所属相机ID
    //debug>>

    
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