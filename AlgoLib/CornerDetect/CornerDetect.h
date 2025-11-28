#pragma once
/* Type declarations */
#ifdef _MSC_VER
#include <basetsd.h>
typedef UINT8  uint8_t;
typedef UINT16 uint16_t;
typedef INT16  int16_t;
typedef UINT32 uint32_t;
#pragma warning( disable: 4799 )
#else
#include <stdint.h>
#endif

//十字检测的CPP实现
//3*3窗口检测
extern "C" void cross_detect3(size_t w, size_t h, const uint8_t* image, uint8_t* response );

//5*5窗口检测
extern "C" void cross_detect5(size_t w, size_t h, const uint8_t* image, uint8_t* response );

//5*5窗口检测,增加了校正图案边长小于等于11个像素时的校正成功率
extern "C" void cross_detect5_improve(size_t w, size_t h, const uint8_t* image, uint8_t* response);

//十字检测的SIMD实现
//3*3窗口检测
extern "C" void cross_detect3_sse2(size_t w, size_t h, const uint8_t* image, uint8_t* response);

//5*5窗口检测
extern "C" void cross_detect5_sse2(size_t w, size_t h, const uint8_t* image, uint8_t* response);
