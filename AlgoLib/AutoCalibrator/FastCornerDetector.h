#pragma once
#include <vector>
#include <Algolib.h>
//@说明:快速角点检测
class CFastCornerDetector
{
public:

    //@功能:快速检测角点;
    //@参数:pSrcImage, 指向灰度图片数据的指针
    //      nWidth, 图片宽度
    //      nHeight, 图片高度
    bool operator()(
    const unsigned char* pSrcImage, 
    int nWidth, 
    int nHeight, 
    UINT uExpectedCornerNumber, 
    //int nDetectRadius, 
    ECalibrationDebugLevel eDebugLevel=E_CALIB_DEBUG_LEVEL_CONCISE, 
    int nDebugTimes =1);



	const std::vector<POINT>& GetResults()const
	{
		return m_vecCorners;
	}


protected:
	//检测结果
	std::vector<POINT> m_vecCorners;
};
