#pragma once
//为了减少整型运算带来的误差,将整型扩大2^ALOG_SCALE_SHIFT_SIZE后运算
//例如整型数a,b
//加 c = (a<<ALOG_SCALE_SHIFT_SIZE) + (b<<ALOG_SCALE_SHIFT_SIZE)
//减 c = (a<<ALOG_SCALE_SHIFT_SIZE) - (b<<ALOG_SCALE_SHIFT_SIZE)
//乘 c = (a<<ALOG_SCALE_SHIFT_SIZE) *(b<<ALOG_SCALE_SHIFT_SIZE),
//       注意扩到2^ALOG_SCALE_SHIFT_SIZE*2^ALOG_SCALE_SHIFT_SIZE  可能导致数据溢出
//除 c = (a<<ALOG_SCALE_SHIFT_SIZE)/b


//提高视频坐标点精度
#define INT_SCALE_SHIFT_SIZE  4



//0x33AA = 13226
#define IRCUT_OFF 0x33AA

//0x44AA = 17578
#define IRCUT_ON  0x44AA


