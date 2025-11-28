#pragma once
#include <vector>
#include <Algolib.h>
//@说明:快速角点检测
class CCrossDetector
{
public:

    //@功能:快速检测角点;
    //@参数:pSrcImage, 指向灰度图片数据的指针
    //      nWidth, 图片宽度
    //      nHeight, 图片高度    
    //      uExpectedCornerNumber, 期望的校正点个数
    //      nGridSize, 棋盘网格平均大小
    //      pMaskImage, 指向屏蔽图的指针，屏蔽图的指针和图片的高度和宽度一致
    //      ECalibDebugLevel, 调试级别
    //      nDebugTimes,调试参数
    bool operator()(
    const unsigned char* pSrcImage, 
    int nWidth, 
    int nHeight, 
    UINT uExpectedCrossNumber, 
    int nGridSize, 
    const unsigned char* pMaskImage,
    ECalibDebugLevel eDebugLevel=E_CALIB_DEBUG_LEVEL_CONCISE, 
    int nDebugTimes =1);



    

    //@功能:返回校正点坐标数组
	const POINT* GetResults()const
	{
		return &m_vecCrosses[0];
	}
    



    //@功能:返回高精度的校正点坐标数组, 坐标被放大2^INT_SCALE_SHIFT_SIZE倍
  	const POINT* GetHDResults()const
	{
		return &m_vecHDCrosses[0];
	}



protected:
	//视频坐标中的,十字校正点坐标的数组, 
	std::vector<POINT> m_vecCrosses;

    //扩大2^INT_SCALE_SHIFT_SIZE倍的校正点坐标数组，该数组可以消除四舍五入带来的误差
    std::vector<POINT> m_vecHDCrosses;
};
