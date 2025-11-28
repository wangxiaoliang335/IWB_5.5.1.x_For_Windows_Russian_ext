#pragma once
extern "C"
{
//@功能:使用MMX指令快速屏蔽YUY2图片帧
//@参数:pYUY2Src, 指向YUY2源数据的指针
//      pYUY2Mask， 指向YUY2屏蔽图数据的指针
//      nPixelCount，像素总数

void  YUY2FrameMask_MMX(
    unsigned short* pYUY2Src,
    const unsigned short* pYUY2Mask,
    unsigned int nPixelCount);



//@功能:使用SSE指令将YUY2图片帧转化为1位位图

void  YUY2ToBitFrame_SSE(
    const unsigned short* pYUY2,
    unsigned char* pBits,
    unsigned char cThreshold,
    unsigned int nPixelCount);


//@函数名:DynamicMaskStatistic_MMX
//@功  能:统计每个像素大于某一门限的次数。如果像素大于门限则像素对应的计数器+1, 如果小于门限，则计数器-1。计数器的范围是0~255
//
//实现的逻辑是
//if(*pImageData >= cCmpThreshold）
//{
//  if(* pStatistic<0xFF) *pStatistic + = 0x01;
//}
//else
//{
//	 if(* pStatistic>0) *pStatistic -= 0x01;
//}

void  DynamicMaskStatistic_YUY2_MMX(
    const unsigned short* pImageData,//YUY2格式的位图
    unsigned char* pStatistic       ,//统计位图
    unsigned char cCmpTheshold      ,//比较门限
    int nPixelCount                  //像素总数
    );



void  DynamicMaskStatistic_Gray_MMX(
    const unsigned char* pImageData,//8Bit灰度格式的位图
    unsigned char* pStatistic       ,//统计位图
    unsigned char cCmpTheshold      ,//比较门限
    int nPixelCount                  //像素总数
    );



void  GrayFrameMask_MMX(
    unsigned char* pGraySrc,
    const unsigned char* pGrayMask,
    unsigned int nPixelCount);


//@功能:使用SSE2指令将8位灰度图片帧转化为1位位图
extern "C"
void  GrayToBitFrame_SSE2(
    const unsigned char* pGray,
    unsigned char* pBits,
    unsigned char cThreshold,
    unsigned int nPixelCount);

//@功能:使用SSE2指令将8位灰度图片帧二值化,目标像素值为0xFF, 其他像素值为0x00, 转化为1位位图
//@参数:pGray, 输入/输出参数,输入指向8位灰度图片的首地址,输出保存二值划后的结果
//      pBits, 输出参数,1位位图
//     cThreshold, 二值化门限
//    nPixelCount, 像素总数
extern "C"
void BinarizeGrayToBitFrame_SSE2(
    unsigned char* pGray,
    unsigned char* pBits,
    unsigned char cThreshold,
    unsigned int nPixelCount);



//@功能:使用SSE2指令将1位位图帧转化为8位灰度图片
//@参数:pGray, 输入参数,指向8位灰度图片的首地址
//      pBits, 输出参数,
//      nPixelCount, 像素总数
extern "C"
void  BitToGrayFrame_MMX(
    const unsigned char* pBits,
    unsigned char* pGray,
    unsigned int nPixelCount);
}

//@功能:将8位灰度图片转换为ARGB格式
//@参数:
//     p8BitGrayData, 灰度图片数据
//      pARGBData, ARGB数据
//      pixelCount, 像素个数
//      
extern "C"
void  GrayToARGB_SSE2(
        const unsigned char* p8BitGrayData, 
        unsigned long* pARGBData, 
        unsigned long pixelCount);