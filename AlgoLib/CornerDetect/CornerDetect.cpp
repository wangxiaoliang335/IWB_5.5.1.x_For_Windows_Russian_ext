#include "CornerDetect.h"
#include <math.h>
#include <stdlib.h>
//5 px sampling radius
//
//         2    1   0
//         ↓  ↓  ↓ 
//   □□□■□■□■□□□
//3→□■□□□□□□□■□←15
//   □□□□□□□□□□□
//4→■□□□□□□□□□■←14
//   □□□□□□□□□□□
//5→■□□□□●□□□□■←13
//   □□□□□□□□□□□
//6→■□□□□□□□□□■←12
//   □□□□□□□□□□□
//7→□■□□□□□□□■□←11
//   □□□■□■□■□□□
//         ↑  ↑  ↑
//         8   9  10

/**
* Perform the cross detection with 5 pixels window size
*
* @param w image width
* @param h image height
* @param image input image
* @param response output response image
*/

extern "C" void cross_detect5(size_t w, size_t h, const uint8_t* image, uint8_t* response)
{
    size_t x, y;
    // funny bounds due to sampling ring radius (5) and border of previously applied blur (2)
    for (y = 5; y < h - 5; y++)
        for (x = 5; x < w - 5; x++) {
            const unsigned offset = x + y * w;

            uint8_t circular_sample[16];

            circular_sample[2 ] = image[offset - 2 - 5 * w];
            circular_sample[1 ] = image[offset     - 5 * w];
            circular_sample[0 ] = image[offset + 2 - 5 * w];

            circular_sample[8 ] = image[offset - 2 + 5 * w];
            circular_sample[9 ] = image[offset     + 5 * w];
            circular_sample[10] = image[offset + 2 + 5 * w];

            circular_sample[3 ] = image[offset - 4 - 4 * w];
            circular_sample[15] = image[offset + 4 - 4 * w];
            circular_sample[7 ] = image[offset - 4 + 4 * w];
            circular_sample[11] = image[offset + 4 + 4 * w];


            circular_sample[4 ] = image[offset - 5 - 2 * w];
            circular_sample[14] = image[offset + 5 - 2 * w];
            circular_sample[6 ] = image[offset - 5 + 2 * w];
            circular_sample[12] = image[offset + 5 + 2 * w];

            circular_sample[5 ] = image[offset - 5 ];
            circular_sample[13] = image[offset + 5 ];

            // purely horizontal local_mean samples
            uint16_t local_mean =
                (
                 image[offset - 1 - w] + image[offset - w] + image[offset + 1 - w] 
                             +
                 image[offset - 1] /*+ image[offset] */+ image[offset + 1]
                             +
                 image[offset - 1 + w] + image[offset + w] + image[offset + 1 + w] 
                 )  ;

            int16_t sum_response = 0;
            int16_t diff_response = 0;
            int16_t mean = 0;

            int sub_idx;
            for (sub_idx = 0; sub_idx < 4; ++sub_idx) {
                uint8_t a = circular_sample[sub_idx];
                uint8_t b = circular_sample[sub_idx + 4];
                uint8_t c = circular_sample[sub_idx + 8];
                uint8_t d = circular_sample[sub_idx + 12];

                sum_response += abs(a - b + c - d);
                diff_response += abs(a - c) + abs(b - d);
                mean += a + b + c + d;
            }
          
            int16_t v = (sum_response - diff_response) - abs((mean>>1) - local_mean);

            if(v < 0) v = 0;

            v >>= 3;
            response[offset] = (uint8_t)v;

        }
}



//5 px sampling radius
//
//         2    1   0
//         ↓  ↓  ↓ 
//   □□□■□■□■□□□
//3→□■□□□□□□□■□←15
//   □□□□□□□□□□□
//4→■□□□□□□□□□■←14
//   □□□□□□□□□□□
//5→■□□□□●□□□□■←13
//   □□□□□□□□□□□
//6→■□□□□□□□□□■←12
//   □□□□□□□□□□□
//7→□■□□□□□□□■□←11
//   □□□■□■□■□□□
//         ↑  ↑  ↑
//         8   9  10

/**
* Perform the cross detection with 5 pixels window size
*
* @param w image width
* @param h image height
* @param image input image
* @param response output response image
*/
//5*5窗口检测,增加了校正图案边长小于等于11个像素时的校正成功率
extern "C" void cross_detect5_improve(size_t w, size_t h, const uint8_t* image, uint8_t* response)
{
    size_t x, y;
    // funny bounds due to sampling ring radius (5) and border of previously applied blur (2)
    for (y = 5; y < h - 5; y++)
        for (x = 5; x < w - 5; x++) {
            const unsigned offset = x + y * w;

            uint8_t outer_circular_sample[16];

            outer_circular_sample[2 ] = image[offset - 2 - 5 * w];
            outer_circular_sample[1 ] = image[offset     - 5 * w];
            outer_circular_sample[0 ] = image[offset + 2 - 5 * w];

            outer_circular_sample[8 ] = image[offset - 2 + 5 * w];
            outer_circular_sample[9 ] = image[offset     + 5 * w];
            outer_circular_sample[10] = image[offset + 2 + 5 * w];

            outer_circular_sample[3 ] = image[offset - 4 - 4 * w];
            outer_circular_sample[15] = image[offset + 4 - 4 * w];
            outer_circular_sample[7 ] = image[offset - 4 + 4 * w];
            outer_circular_sample[11] = image[offset + 4 + 4 * w];


            outer_circular_sample[4 ] = image[offset - 5 - 2 * w];
            outer_circular_sample[14] = image[offset + 5 - 2 * w];
            outer_circular_sample[6 ] = image[offset - 5 + 2 * w];
            outer_circular_sample[12] = image[offset + 5 + 2 * w];

            outer_circular_sample[5 ] = image[offset - 5 ];
            outer_circular_sample[13] = image[offset + 5 ];

            // purely horizontal local_mean samples
            uint16_t local_mean =
                (
                 (int16_t)image[offset - 1 - w] + (int16_t)image[offset - w] + (int16_t)image[offset + 1 - w] 
                             +
                 (int16_t)image[offset - 1] /*+ image[offset] */+ (int16_t)image[offset + 1]
                             +
                 (int16_t)image[offset - 1 + w] + (int16_t)image[offset + w] + (int16_t)image[offset + 1 + w] 
                 )  ;

            //
            uint16_t outer_circle_diff_sum = abs(outer_circular_sample[0] - outer_circular_sample[15]);
            for(int i = 1; i <  _countof(outer_circular_sample); i++)
            {
                outer_circle_diff_sum += abs((int16_t)outer_circular_sample[i] - (int16_t)outer_circular_sample[i-1]);
            }




            uint8_t inner_circular_sample[16];

            inner_circular_sample[2 ] = image[offset - 1 - 3 * w];
            inner_circular_sample[1 ] = image[offset     - 3 * w];
            inner_circular_sample[0 ] = image[offset + 1 - 3 * w];

            inner_circular_sample[8 ] = image[offset - 1 + 3 * w];
            inner_circular_sample[9 ] = image[offset     + 3 * w];
            inner_circular_sample[10] = image[offset + 1 + 3 * w];

            inner_circular_sample[3 ] = image[offset - 2 - 2 * w];
            inner_circular_sample[15] = image[offset + 2 - 2 * w];
            inner_circular_sample[7 ] = image[offset - 2 + 2 * w];
            inner_circular_sample[11] = image[offset + 2 + 2 * w];


            inner_circular_sample[4 ] = image[offset - 3 - 1 * w];
            inner_circular_sample[14] = image[offset + 3 - 1 * w];
            inner_circular_sample[6 ] = image[offset - 3 + 1 * w];
            inner_circular_sample[12] = image[offset + 3 + 1 * w];

            inner_circular_sample[5 ] = image[offset - 3 ];
            inner_circular_sample[13] = image[offset + 3 ];


            uint16_t inner_circle_diff_sum = abs(inner_circular_sample[0] - inner_circular_sample[15]);
            for(int i = 1; i < _countof(outer_circular_sample); i++)
            {
                inner_circle_diff_sum += abs((int16_t)inner_circular_sample[i] - (int16_t)inner_circular_sample[i-1]);
            }


            uint16_t outer_inner_diff = abs(outer_circle_diff_sum - inner_circle_diff_sum);


            int16_t sum_response = 0;
            int16_t diff_response = 0;
            int16_t mean = 0;

            int sub_idx;
            for (sub_idx = 0; sub_idx < 4; ++sub_idx) {
                uint8_t a = outer_circular_sample[sub_idx];
                uint8_t b = outer_circular_sample[sub_idx + 4];
                uint8_t c = outer_circular_sample[sub_idx + 8];
                uint8_t d = outer_circular_sample[sub_idx + 12];

                sum_response += abs(a - b + c - d);
                diff_response += abs(a - c) + abs(b - d);
                mean += a + b + c + d;
            }
          
            //int16_t v = (sum_response - diff_response) - abs((mean>>1) - local_mean);
            int16_t v = (sum_response - diff_response)  - outer_inner_diff;

            if(v < 0) v = 0;

            v >>= 3;
            response[offset] = v;

        }
}



//3 px sampling radius
//
//        2  1  0
//        ↓↓↓
//    □□■■■□□□
//   3→■□□□■←15
// →4■□□□□□■←14
// →5■□□●□□■←13
// →6■□□□□□■←12
//   →7■□□□■←11
//    □□■■■□□
//        ↑ ↑↑
//         8 9 10 

/**
* Perform the cross detection with 3 pixels window size
*
* @param w image width
* @param h image height
* @param image input image
* @param response output response image
* @author:toxuke@gmail.com
* @date  :2013/11/13
*/
extern "C" void cross_detect3(size_t w, size_t h, const uint8_t* image, uint8_t* response)
{
    size_t x, y;
    // funny bounds due to sampling ring radius (5) and border of previously applied blur (2)
    for (y = 3; y < h - 3; y++)
        for (x = 3; x < w - 3; x++) {
            const unsigned offset = x + y * w;

            uint8_t circular_sample[16];

            circular_sample[2 ] = image[offset - 1 - 3 * w];
            circular_sample[1 ] = image[offset     - 3 * w];
            circular_sample[0 ] = image[offset + 1 - 3 * w];

            circular_sample[8 ] = image[offset - 1 + 3 * w];
            circular_sample[9 ] = image[offset     + 3 * w];
            circular_sample[10] = image[offset + 1 + 3 * w];

            circular_sample[3 ] = image[offset - 2 - 2 * w];
            circular_sample[15] = image[offset + 2 - 2 * w];
            circular_sample[7 ] = image[offset - 2 + 2 * w];
            circular_sample[11] = image[offset + 2 + 2 * w];


            circular_sample[4 ] = image[offset - 3 - 1 * w];
            circular_sample[14] = image[offset + 3 - 1 * w];
            circular_sample[6 ] = image[offset - 3 + 1 * w];
            circular_sample[12] = image[offset + 3 + 1 * w];

            circular_sample[5 ] = image[offset - 3 ];
            circular_sample[13] = image[offset + 3 ];

            // purely horizontal local_mean samples
            uint16_t local_mean =
                (
                 image[offset - 1 - w] + image[offset - w] + image[offset + 1 - w] 
                             +
                 image[offset - 1] /*+ image[offset] */+ image[offset + 1]
                             +
                 image[offset - 1 + w] + image[offset + w] + image[offset + 1 + w] 
                 )  ;

            int16_t sum_response = 0;
            int16_t diff_response = 0;
            int16_t mean = 0;

            int sub_idx;
            for (sub_idx = 0; sub_idx < 4; ++sub_idx) {
                uint8_t a = circular_sample[sub_idx];
                uint8_t b = circular_sample[sub_idx + 4];
                uint8_t c = circular_sample[sub_idx + 8];
                uint8_t d = circular_sample[sub_idx + 12];

                sum_response += abs(a - b + c - d);
                diff_response += abs(a - c) + abs(b - d);
                mean += a + b + c + d;
            }
          
            int16_t v = (sum_response - diff_response) - abs((mean>>1) - local_mean);

            if(v < 0) v = 0;

            v >>= 3;
            response[offset] = (uint8_t)v;

        }
}




//计算绝对值的两种汇编算法
//[算法1]
//cdq
//xor eax, edx
//sub eax, edx

//x86指令:
//CDQ:Sign-extends EAX into EDX, forming the quad-word EDX:EAX. Since (I)DIV uses EDX:EAX as its input,
//    CDQ must be called after setting EAX if EDX is not manually initialized (as in 64/32 division) before (I)DIV.
        
//[算法2]
//;abs(eax), with no branches.
//; intel syntax (dest, src)
//mov ebx, eax ;store eax in ebx
//neg eax
//cmovl eax, ebx ;if eax is now negative, restore its saved value*/
//x86指令:
//CMOVcc--Conditional MoveConditional Move
//CMOVL:Move if less (SF<>OF)


//5 px sampling radius
//
//         2    1   0
//         ↓  ↓  ↓ 
//   □□□■□■□■□□□
//3→□■□□□□□□□■□←15
//   □□□□□□□□□□□
//4→■□□□□□□□□□■←14
//   □□□□□□□□□□□
//5→■□□□□●□□□□■←13
//   □□□□□□□□□□□
//6→■□□□□□□□□□■←12
//   □□□□□□□□□□□
//7→□■□□□□□□□■□←11
//   □□□■□■□■□□□
//         ↑  ↑  ↑
//         8   9  10

/**
* Perform the cross detection with 5 pixels window size
*
* @param w image width
* @param h image height
* @param image input image
* @param response output response image
*/

extern "C" void _declspec(naked)  cross_detect5_sse2(size_t w, size_t h, const uint8_t* image, uint8_t* response)
{

    //所有局部变量的大小
    #define LOCAL_VARIABLES_SIZE (4*128)

    //局部变量CIRCLE_SAMPLEXX的内存起始偏移量
    #define CIRCLE_SAMPLE_OFFSET 160

    //定义一个数组,保存记录圆周上的采样点。
    #define CIRCLE_SAMPLE00 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET -15*16]  
    #define CIRCLE_SAMPLE01 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET -14*16]  
    #define CIRCLE_SAMPLE02 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET -13*16]  
    #define CIRCLE_SAMPLE03 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET -12*16]  
    #define CIRCLE_SAMPLE04 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET -11*16]  
    #define CIRCLE_SAMPLE05 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET -10*16]  
    #define CIRCLE_SAMPLE06 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 9*16]  
    #define CIRCLE_SAMPLE07 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 8*16]  
    #define CIRCLE_SAMPLE08 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 7*16]  
    #define CIRCLE_SAMPLE09 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 6*16]  
    #define CIRCLE_SAMPLE10 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 5*16]  
    #define CIRCLE_SAMPLE11 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 4*16]  
    #define CIRCLE_SAMPLE12 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 3*16]  
    #define CIRCLE_SAMPLE13 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 2*16]  
    #define CIRCLE_SAMPLE14 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 1*16]  
    #define CIRCLE_SAMPLE15 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 0*16]  


    #define CIRCLE_SAMPLE_OFFSET_START 80

    //定义一个数组,记录圆周上的采样点与11X11图像块的左上角的内存偏移量
    #define CIRCLE_SAMPLE_OFFSET00 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 15*4]
    #define CIRCLE_SAMPLE_OFFSET01 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 14*4]
    #define CIRCLE_SAMPLE_OFFSET02 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 13*4]
    #define CIRCLE_SAMPLE_OFFSET03 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 12*4]
    #define CIRCLE_SAMPLE_OFFSET04 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 11*4]
    #define CIRCLE_SAMPLE_OFFSET05 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 10*4]
    #define CIRCLE_SAMPLE_OFFSET06 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 9*4 ]
    #define CIRCLE_SAMPLE_OFFSET07 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 8*4 ]
    #define CIRCLE_SAMPLE_OFFSET08 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 7*4 ]
    #define CIRCLE_SAMPLE_OFFSET09 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 6*4 ]
    #define CIRCLE_SAMPLE_OFFSET10 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 5*4 ]
    #define CIRCLE_SAMPLE_OFFSET11 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 4*4 ]
    #define CIRCLE_SAMPLE_OFFSET12 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 3*4 ]
    #define CIRCLE_SAMPLE_OFFSET13 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 2*4 ]
    #define CIRCLE_SAMPLE_OFFSET14 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 1*4 ]
    #define CIRCLE_SAMPLE_OFFSET15 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 0*4 ]



    //定义一个数组,记录中心点的8个邻接点与11X11图像块的左上角的内存偏移量
    #define CENTER_NEIGHBOUR_OFFSET 40
    #define CENTER_NEIGHBOUR_OFFSET00 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*7]
    #define CENTER_NEIGHBOUR_OFFSET01 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*6]
    #define CENTER_NEIGHBOUR_OFFSET02 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*5]
    #define CENTER_NEIGHBOUR_OFFSET03 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*4]
    #define CENTER_NEIGHBOUR_OFFSET04 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*3]
    #define CENTER_NEIGHBOUR_OFFSET05 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*2]
    #define CENTER_NEIGHBOUR_OFFSET06 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*1]
    #define CENTER_NEIGHBOUR_OFFSET07 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*0]
    

    //2*W, 3*W,..., 10*W
    #define WX_OFFSET 4
    //定义一个数组，保存预先计算的"图象宽度的倍数"
    #define WX2  dword ptr[ebp - WX_OFFSET - 8*4]
    #define WX3  dword ptr[ebp - WX_OFFSET - 7*4]
    #define WX4  dword ptr[ebp - WX_OFFSET - 6*4]
    #define WX5  dword ptr[ebp - WX_OFFSET - 5*4]
    #define WX6  dword ptr[ebp - WX_OFFSET - 4*4]
    #define WX7  dword ptr[ebp - WX_OFFSET - 3*4]
    #define WX8  dword ptr[ebp - WX_OFFSET - 2*4]
    #define WX9  dword ptr[ebp - WX_OFFSET - 1*4]
    #define WX10 dword ptr[ebp - WX_OFFSET - 0*4]



    //#define worker_register0  xmm0
    //#define worker_register1  xmm1

    #define _8pt_sum_response    xmm4
    #define _8pt_diff_response   xmm5
    #define _8pt_mean_response   xmm6
    #define _8pt_local_mean      xmm7


    #define _4pt_sum_response    mm4
    #define _4pt_diff_response   mm5
    #define _4pt_mean_response   mm6
    #define _4pt_local_mean      mm7


    //输入参数
    #define W   [ebx + 8]
    #define H   [ebx + 12]
    #define SRC [ebx + 16]
    #define DST [ebx + 20]

    #define PABSW(xmmr1, xmmr2) _asm\
    {\
    _asm pxor   xmmr1, xmmr1 \
    _asm psubw  xmmr1, xmmr2 \
    _asm pmaxsw xmmr1, xmmr2 \
    }

    

    _asm
    {
        //局部变量需要16字节对齐
                                  ;|   ...   |
        push ebx                  ;| old_ebx |←esp
                                  ;|   ret   |
                                  
        mov ebx, esp              ;|   ...   |
                                  ;| old_ebx |←ebx
                                  ;|   ret.  |
                                  
                                  ;|   ...   |←esp
        sub esp, 8                ;|   ...   |
                                  ;| old_ebx |←ebx
                                  ;|   ret   |
                                  
                                  
                                  ;|   ...   |←esp (esp is 16 bytes aligned )
                                  ;|   ...   |
                                  ;|   ...   |
                                  ;|   ...   |
       and esp, 0FFFFFFF0h        ;| old_ebx |←ebx
                                  ;|   ret   |
                                  
                                  
                                  
                                  
                                  ;|   ...   |←16 bytes aligned
                                  ;|   ...   |←esp
                                  ;|   ...   |
                                  ;|   ...   |
       add esp, 4                 ;| old_ebx |←ebx
                                  ;|   ret   |
                                  
                                  
                                  ;|   ebp   |←esp,16 bytes aligned
                                  ;|   ...   |
                                  ;|   ...   |
                                  ;|   ...   |
       push ebp                   ;| old_ebx |←ebx
                                  ;|   ret   |


                                  ;|   ebp   |←esp 16 bytes aligned
                                  ;|   ret   |
       mov ebp, dword ptr [ebx+4] ;|         |
                                  ;|   ...   |
       mov dword ptr [esp+4], ebp ;| old_ebx |←ebx
                                  ;|   ret   |


        mov ebp, esp              ;|   ebp   |←ebp(esp 16 bytes aligned)
                                  ;|   ret   |
                                  ;|         |
                                  ;|   ...   |
                                  ;| old_ebx |←ebx
                                  ;|   ret   |




       sub esp, LOCAL_VARIABLES_SIZE;



        pushad ; Temp  (ESP);
               ; Push(EAX);
               ; Push(ECX);
               ; Push(EDX);
               ; Push(EBX);
               ; Push(Temp);
               ; Push(EBP);
               ; Push(ESI);
               ; Push(EDI);

        mov ecx, 2
        lea edx, WX2
       
        //计算2*W, 3*W, 4*W, ..., 10*W
CALC_WXN:
        mov eax, ecx
        imul eax, W
        mov [edx], eax //[edx] = ecx*W
        add edx, 4
        inc ecx;
        cmp ecx, 10;
        jbe CALC_WXN;


        //计算圈层上的目标点的相对于11X11左上角的偏离量，点的标号见注释
        //point 0: 7      
         mov CIRCLE_SAMPLE_OFFSET00, 7;

        //point 1: 5      
         mov CIRCLE_SAMPLE_OFFSET01, 5
        
        //point 2: 3      
        mov CIRCLE_SAMPLE_OFFSET02, 3
        
        //point 3: W  + 1 
        mov eax, W
        add eax, 1
        mov CIRCLE_SAMPLE_OFFSET03, eax

        //point 4: 3W
        mov eax, WX3
        mov CIRCLE_SAMPLE_OFFSET04, eax

        //point 5: 5W     
        mov eax, WX5
        mov CIRCLE_SAMPLE_OFFSET05, eax

        //point 6: 7W     
        mov eax, WX7
        mov CIRCLE_SAMPLE_OFFSET06, eax

        //point 7: 9W  + 1
        mov eax, WX9
        add eax, 1
        mov CIRCLE_SAMPLE_OFFSET07, eax

        //point 8: 10W + 3
        mov eax, WX10
        add eax, 3
        mov CIRCLE_SAMPLE_OFFSET08, eax

        //point 9: 10W + 5
        mov eax, WX10
        add eax, 5
        mov CIRCLE_SAMPLE_OFFSET09, eax

        //point 10:10W + 7
        mov eax, WX10
        add eax, 7
        mov CIRCLE_SAMPLE_OFFSET10, eax

        //point 11: 9W + 9
        mov eax, WX9
        add eax, 9
        mov CIRCLE_SAMPLE_OFFSET11, eax

        //point 12: 7W + 10
        mov eax, WX7
        add eax, 10
        mov CIRCLE_SAMPLE_OFFSET12, eax

        //point 13: 5W + 10
        mov eax, WX5
        add eax, 10
        mov CIRCLE_SAMPLE_OFFSET13, eax

        //point 14: 3W + 10
        mov eax, WX3
        add eax, 10
        mov CIRCLE_SAMPLE_OFFSET14, eax

        //point 15: W + 9
        mov eax, W
        add eax, 9
        mov CIRCLE_SAMPLE_OFFSET15, eax


        //=====计算中心点的8个邻接点与11X11图块的左上角的偏移量=====
        //  A  B  C
       //    □□□
       //   H□●□D
       //    □□□
       //   G  F   E
       //A点
       mov eax, WX4
       add eax, 4
       mov CENTER_NEIGHBOUR_OFFSET00, eax

       //B点
       mov eax, WX4
       add eax, 5
       mov CENTER_NEIGHBOUR_OFFSET01, eax

       //C点
       mov eax, WX4
       add eax, 6
       mov CENTER_NEIGHBOUR_OFFSET02, eax

       //D点
       mov eax, WX5
       add eax, 6
       mov CENTER_NEIGHBOUR_OFFSET03, eax

       //E点
       mov eax, WX6
       add eax, 6
       mov CENTER_NEIGHBOUR_OFFSET04, eax


       //F点
       mov eax, WX6
       add eax, 5
       mov CENTER_NEIGHBOUR_OFFSET05, eax


       //G点
       mov eax, WX6
       add eax, 4
       mov CENTER_NEIGHBOUR_OFFSET06, eax


       //H点
       mov eax, WX5
       add eax, 4
       mov CENTER_NEIGHBOUR_OFFSET07, eax


        finit  ;//reset the floating-point unit



        //水平循环次数
        mov eax, W
        sub eax, 10  //上下各去掉5行
        mov ecx, eax

        //垂直循环次数
        mov eax, H
        sub eax, 10 //左右各去掉5列
        mov edx, eax
        

        mov esi, SRC ;//pointer to 8 bit image source
        mov edi, DST ;//pointer to 16 bit response data

        //输出内存指针
        add edi, WX5;//WX5
        add edi, 5 ;

HLOOP:
       push esi
       push edi
       push ecx


WLOOP:
        cmp ecx, 8
        
        jb _4pt_PROCESS;//待处理点数 少于8个, 转4点处理

        //caculate local mean

       pxor xmm0, xmm0
       pxor _8pt_local_mean, _8pt_local_mean


       //A点
        mov eax, CENTER_NEIGHBOUR_OFFSET00
        movsd _8pt_local_mean, [esi + eax];//need not be 16-byte aligned
        punpcklbw _8pt_local_mean, xmm0;//the low eight btyes extend to eight word

       //B点
         mov eax, CENTER_NEIGHBOUR_OFFSET01
        movsd xmm1, [esi + eax];//need not be 16-byte aligned        
        punpcklbw xmm1, xmm0;
        paddw _8pt_local_mean, xmm1

        //C点
        mov eax, CENTER_NEIGHBOUR_OFFSET02
        movsd xmm1, [esi + eax];//need not be 16-byte aligned        
        punpcklbw xmm1, xmm0;
        paddw _8pt_local_mean, xmm1

        //D点
        mov eax, CENTER_NEIGHBOUR_OFFSET03
        movsd xmm1, [esi + eax]
        punpcklbw xmm1, xmm0
        paddw _8pt_local_mean, xmm1

        //E点
        mov eax, CENTER_NEIGHBOUR_OFFSET04
        movsd xmm1, [esi + eax]
        punpcklbw xmm1, xmm0
        paddw _8pt_local_mean, xmm1


        //F点
        mov eax, CENTER_NEIGHBOUR_OFFSET05
        movsd xmm1, [esi + eax]
        punpcklbw xmm1, xmm0
        paddw _8pt_local_mean, xmm1

        //G点
        mov eax, CENTER_NEIGHBOUR_OFFSET06
        movsd xmm1, [esi + eax]
        punpcklbw xmm1, xmm0
        paddw _8pt_local_mean, xmm1

        //H点
        mov eax, CENTER_NEIGHBOUR_OFFSET07
        movsd xmm1, [esi + eax]
        punpcklbw xmm1, xmm0
        paddw _8pt_local_mean, xmm1



        //读取圈层上的点
        //0 点
        push  ebx
        push  ecx
        push  edx


        lea ebx, CIRCLE_SAMPLE_OFFSET00
        lea edx, CIRCLE_SAMPLE00
        mov ecx, 16


_8pt_READ_CIRCLE_SAMPLE:
        mov eax, [ebx]             ;//eax=pixle byte offset relative to left-top of 11*11 bloack
        movsd xmm1, [esi + eax]    ;//read 8 bytes each time
        punpcklbw xmm1, xmm0       ;//convert each 8 bytes to 8 words
        movdqa [edx], xmm1         ;//store data to cache
        
        add ebx, 4                 ;//next offset position
        add edx, 16                ;//next CIRCLE_SAMPLE position
        dec ecx                    ;//loop
        jnz  _8pt_READ_CIRCLE_SAMPLE



#define _8_PT_A xmmword ptr [eax         ]
#define _8_PT_B xmmword ptr [eax + 4 *16 ]
#define _8_PT_C xmmword ptr [eax + 8 *16 ]
#define _8_PT_D xmmword ptr [eax + 12*16 ]

//                A
//               ↓
//               ■
//               □
//               □
//               □
//               □
// B→ ■□□□□●□□□□■←D
//               □
//               □
//               □
//               □
//               ■
//               ↑
//               C

        //====Response Part=======
        pxor _8pt_sum_response    , _8pt_sum_response ;//_8pt_sum_response
        pxor _8pt_diff_response   , _8pt_diff_response ;//_8pt_diff_response
        pxor _8pt_mean_response   , _8pt_mean_response ;//mean

        mov ecx, 0
        lea eax, CIRCLE_SAMPLE00

_8pt_CALC_EACH_RESPONSE://计算各响应


        //abs(A - B + C -D)
        movdqa xmm0, _8_PT_A
        psubw  xmm0, _8_PT_B
        paddw  xmm0, _8_PT_C
        psubw  xmm0, _8_PT_D

       
        PABSW(xmm1, xmm0);//xmm1 = abs(xmm0)
        paddw _8pt_sum_response, xmm1;

        //abs(A - C) + abs(B - D);
        movdqa xmm0, _8_PT_A
        psubw  xmm0, _8_PT_C
        PABSW (xmm1, xmm0);//xmm1 = abs(xmm0)
        paddw _8pt_diff_response, xmm1
        
        movdqa xmm0, _8_PT_B
        psubw  xmm0, _8_PT_D
        PABSW (xmm1, xmm0);//xmm1 = abs(xmm0)
        paddw  _8pt_diff_response, xmm1

        //;mean = A + B + C +D
        paddw  _8pt_mean_response, _8_PT_A;
        paddw  _8pt_mean_response, _8_PT_B;
        paddw  _8pt_mean_response, _8_PT_C;
        paddw  _8pt_mean_response, _8_PT_D;


        add eax, 16
        inc ecx
        cmp ecx, 4
        jnz _8pt_CALC_EACH_RESPONSE


        //计算结果
        //response = (_8pt_sum_response - _8pt_diff_response)*2 - abs(_8pt_mean_response - _8pt_local_mean)

        psrlw _8pt_mean_response, 1 ;//_8pt_mean_response除2, 以使取值范围一致

        psubw _8pt_mean_response, _8pt_local_mean
        PABSW(xmm0, _8pt_mean_response)      ;//xmm0 = abs(_8pt_mean_response/2 - _8pt_local_mean)

        movdqa   xmm1, _8pt_sum_response     ;//
        psubusw  xmm1, _8pt_diff_response    ;//xmm1  = _8pt_sum_response - _8pt_diff_response
        psubusw  xmm1, xmm0                  ;//xmm1 = (_8pt_sum_response - _8pt_diff_response) - abs(mean/2 - _8pt_local_mean)

        psrlw  xmm1, 3                       ;//divide by 8
        packuswb xmm1, xmm1                  ;//8个字压缩为8个字节
        
        
        //输出8个像素的检测结果
        movsd qword ptr[edi], xmm1


        pop edx
        pop ecx
        pop ebx


        add esi, 8  ;//一次处理8个像素点
        add edi, 8  ;//
        
        sub ecx, 8
        ja WLOOP

        jmp HLOOP_CONTINUE



_4pt_PROCESS://一次处理4点
       cmp ecx, 4
       jb _SINGLE_PT_PROCESS

       pxor mm0, mm0
       pxor _4pt_local_mean, _4pt_local_mean


       //A点
        mov eax, CENTER_NEIGHBOUR_OFFSET00
        movd _4pt_local_mean, [esi + eax];//need not be 16-byte aligned
        punpcklbw _4pt_local_mean, mm0;//the low eight btyes extend to eight word

       //B点
        mov eax, CENTER_NEIGHBOUR_OFFSET01
        movd mm1, [esi + eax];//need not be 16-byte aligned        
        punpcklbw mm1, mm0;
        paddw _4pt_local_mean, mm1

        //C点
        mov eax, CENTER_NEIGHBOUR_OFFSET02
        movd mm1, [esi + eax];//need not be 16-byte aligned        
        punpcklbw mm1, mm0;
        paddw _4pt_local_mean, xmm1

        //D点
        mov eax, CENTER_NEIGHBOUR_OFFSET03
        movd mm1, [esi + eax]
        punpcklbw mm1, mm0
        paddw _4pt_local_mean, mm1

        //E点
        mov eax, CENTER_NEIGHBOUR_OFFSET04
        movd mm1, [esi + eax]
        punpcklbw mm1, mm0
        paddw _4pt_local_mean, mm1


        //F点
        mov eax, CENTER_NEIGHBOUR_OFFSET05
        movd mm1, [esi + eax]
        punpcklbw mm1, mm0
        paddw _4pt_local_mean, mm1

        //G点
        mov eax, CENTER_NEIGHBOUR_OFFSET06
        movd mm1, [esi + eax]
        punpcklbw mm1, mm0
        paddw _4pt_local_mean, mm1

        //H点
        mov eax, CENTER_NEIGHBOUR_OFFSET07
        movd mm1, [esi + eax]
        punpcklbw mm1, mm0
        paddw _4pt_local_mean, mm1



        //读取圈层上的点
        //0 点
        push  ebx
        push  ecx
        push  edx


        lea ebx, CIRCLE_SAMPLE_OFFSET00
        lea edx, CIRCLE_SAMPLE00
        mov ecx, 16


_4pt_READ_CIRCLE_SAMPLE:
        mov eax, [ebx]            ;//eax=pixle byte offset relative to left-top of 11*11 bloack
        movd mm1, [esi + eax]     ;//read 8 bytes each time
        punpcklbw mm1, mm0        ;//convert each 8 bytes to 8 words
        movq  [edx], mm1          ;//store data to cache
        
        add ebx, 4                 ;//next offset position
        add edx, 16                ;//next CIRCLE_SAMPLE position
        dec ecx                    ;//loop
        jnz  _4pt_READ_CIRCLE_SAMPLE


        //====Response Part=======
        pxor _4pt_sum_response    , _4pt_sum_response  ;//
        pxor _4pt_diff_response   , _4pt_diff_response ;//
        pxor _4pt_mean_response   , _4pt_mean_response ;//

        mov ecx, 0
        lea eax, CIRCLE_SAMPLE00

#define _4_PT_A mmword ptr [eax         ]
#define _4_PT_B mmword ptr [eax + 4 *16 ]
#define _4_PT_C mmword ptr [eax + 8 *16 ]
#define _4_PT_D mmword ptr [eax + 12*16 ]

_4pt_CALC_EACH_RESPONSE://计算各响应


        //abs(A - B + C -D)
        movq  mm0, _4_PT_A
        psubw mm0, _4_PT_B
        paddw mm0, _4_PT_C
        psubw mm0, _4_PT_D

       
        PABSW(mm1, mm0);//mm1 = abs(xmm0)
        paddw _4pt_sum_response, mm1;

        //abs(A - C) + abs(B - D);
        movq   mm0, _4_PT_A
        psubw  mm0, _4_PT_C
        PABSW (mm1, mm0);//mm1 = abs(mm0)
        paddw _4pt_diff_response,mm1
        
        movq   mm0, _4_PT_B
        psubw  mm0, _4_PT_D
        PABSW (mm1, mm0);//mm1 = abs(mm0)
        paddw  _4pt_diff_response, mm1

        //;mean = A + B + C +D
        paddw  _4pt_mean_response, _4_PT_A;
        paddw  _4pt_mean_response, _4_PT_B;
        paddw  _4pt_mean_response, _4_PT_C;
        paddw  _4pt_mean_response, _4_PT_D;


        add eax, 16
        inc ecx
        cmp ecx, 4
        jnz _4pt_CALC_EACH_RESPONSE


        //计算结果
        //response = (_4pt_sum_response - _4pt_diff_response)*2 - abs(_4pt_mean_response - _4pt_local_mean)

        psrlw _4pt_mean_response, 1 ;//_4pt_mean_response/2
        psubw _4pt_mean_response, _4pt_local_mean
        PABSW(mm0, _4pt_mean_response)      ;//mm0 = abs(_4pt_mean_response - _4pt_local_mean)

        movq     mm1, _4pt_sum_response     ;//
        psubusw  mm1, _4pt_diff_response    ;//mm1  = _4pt_sum_response - _4pt_diff_response       
        psubusw  mm1, mm0                   ;//mm1 = (_4pt_sum_response - _4pt_diff_response) - abs(_4pt_mean_response/2 - _4pt_local_mean)
        

       psrlw    mm1, 3                      ;//divide by 8
       packuswb mm1, mm1                    ;//8个字压缩为8个字节

        //输出4个像素的检测结果
        movd [edi], mm1;

        pop edx
        pop ecx
        pop ebx

        add esi, 4  ;//一次处理4个像素点
        add edi, 4  ;//

         sub ecx, 4


_SINGLE_PT_PROCESS://一次处理1点,不做任何处理,只是保证循环计数变量值的正确
        cmp ecx,0
        je HLOOP_CONTINUE





        add esi, 1   ;//一次处理1个像素点
        add edi, 1   ;//

        dec ecx
        jmp _SINGLE_PT_PROCESS



HLOOP_CONTINUE:

        pop ecx
        pop edi
        pop esi

        add edi, W  ;//输出数据每个像素有1个字节
        add esi, W  ;//元数据每个像素1个字节

        dec edx
        jnz HLOOP;


        emms  ;Empty the multimedia state.


        popad ;Registers are popped in the following order: 
              ;EDI
              ;ESI
              ;EBP 
              ;ESP,increment ESP by 4 (* skip next 4 bytes of stack *)
              ;EBX
              ;EDX
              ;ECX
              ;EAX.  
              ;The (E)SP value popped from the stack is actually discarded.


        mov  esp,ebp 
        pop  ebp

        mov esp, ebx;
        pop ebx;
        ret;

    }
}




//3 px sampling radius
//
//        2  1  0
//        ↓↓↓
//    □□■■■□□
//   3→■□□□■←15
// →4■□□□□□■←14
// →5■□□●□□■←13
// →6■□□□□□■←12
//   →7■□□□■←11
//    □□■■■□□
//        ↑↑↑
//         8 9 10 

/**
* Perform the cross detection with 3 pixels window size
*
* @param w image width
* @param h image height
* @param image input image
* @param response output response image
* @author:toxuke@gmail.com
* @date  :2013/11/13
*/

extern "C" void _declspec(naked)  cross_detect3_sse2(size_t w, size_t h, const uint8_t* image, uint8_t* response)
{

    //所有局部变量的大小
    #define LOCAL_VARIABLES_SIZE (4*128)

    //局部变量CIRCLE_SAMPLEXX的内存起始偏移量
    #define CIRCLE_SAMPLE_OFFSET 160

    //定义一个数组,记录圆周上的采样点的值
    #define CIRCLE_SAMPLE00 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET -15*16]  
    #define CIRCLE_SAMPLE01 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET -14*16]  
    #define CIRCLE_SAMPLE02 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET -13*16]  
    #define CIRCLE_SAMPLE03 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET -12*16]  
    #define CIRCLE_SAMPLE04 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET -11*16]  
    #define CIRCLE_SAMPLE05 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET -10*16]  
    #define CIRCLE_SAMPLE06 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 9*16]  
    #define CIRCLE_SAMPLE07 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 8*16]  
    #define CIRCLE_SAMPLE08 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 7*16]  
    #define CIRCLE_SAMPLE09 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 6*16]  
    #define CIRCLE_SAMPLE10 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 5*16]  
    #define CIRCLE_SAMPLE11 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 4*16]  
    #define CIRCLE_SAMPLE12 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 3*16]  
    #define CIRCLE_SAMPLE13 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 2*16]  
    #define CIRCLE_SAMPLE14 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 1*16]  
    #define CIRCLE_SAMPLE15 xmmword ptr[ebp - CIRCLE_SAMPLE_OFFSET - 0*16]  


    #define CIRCLE_SAMPLE_OFFSET_START 80

    //定义一个数组,记录圆周上的采样点与7X7图像块的左上角的内存偏移量
    #define CIRCLE_SAMPLE_OFFSET00 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 15*4]
    #define CIRCLE_SAMPLE_OFFSET01 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 14*4]
    #define CIRCLE_SAMPLE_OFFSET02 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 13*4]
    #define CIRCLE_SAMPLE_OFFSET03 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 12*4]
    #define CIRCLE_SAMPLE_OFFSET04 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 11*4]
    #define CIRCLE_SAMPLE_OFFSET05 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 10*4]
    #define CIRCLE_SAMPLE_OFFSET06 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 9*4 ]
    #define CIRCLE_SAMPLE_OFFSET07 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 8*4 ]
    #define CIRCLE_SAMPLE_OFFSET08 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 7*4 ]
    #define CIRCLE_SAMPLE_OFFSET09 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 6*4 ]
    #define CIRCLE_SAMPLE_OFFSET10 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 5*4 ]
    #define CIRCLE_SAMPLE_OFFSET11 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 4*4 ]
    #define CIRCLE_SAMPLE_OFFSET12 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 3*4 ]
    #define CIRCLE_SAMPLE_OFFSET13 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 2*4 ]
    #define CIRCLE_SAMPLE_OFFSET14 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 1*4 ]
    #define CIRCLE_SAMPLE_OFFSET15 dword ptr[ebp - CIRCLE_SAMPLE_OFFSET_START - 0*4 ]



    //定义一个数组,记录中心点的8个邻接点与7X7图像块的左上角的内存偏移量
    #define CENTER_NEIGHBOUR_OFFSET 40
    #define CENTER_NEIGHBOUR_OFFSET00 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*7]
    #define CENTER_NEIGHBOUR_OFFSET01 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*6]
    #define CENTER_NEIGHBOUR_OFFSET02 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*5]
    #define CENTER_NEIGHBOUR_OFFSET03 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*4]
    #define CENTER_NEIGHBOUR_OFFSET04 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*3]
    #define CENTER_NEIGHBOUR_OFFSET05 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*2]
    #define CENTER_NEIGHBOUR_OFFSET06 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*1]
    #define CENTER_NEIGHBOUR_OFFSET07 dword ptr[ebp - CENTER_NEIGHBOUR_OFFSET -4*0]
    

    //2*W, 3*W,..., 6*W
    #define WX_OFFSET 4
    //定义一个数组，保存预先计算的"图象宽度的倍数"
    #define WX2  dword ptr[ebp - WX_OFFSET - 8*4]
    #define WX3  dword ptr[ebp - WX_OFFSET - 7*4]
    #define WX4  dword ptr[ebp - WX_OFFSET - 6*4]
    #define WX5  dword ptr[ebp - WX_OFFSET - 5*4]
    #define WX6  dword ptr[ebp - WX_OFFSET - 4*4]
    //#define WX7  dword ptr[ebp - WX_OFFSET - 3*4]
    //#define WX8  dword ptr[ebp - WX_OFFSET - 2*4]
    //#define WX9  dword ptr[ebp - WX_OFFSET - 1*4]
    //#define WX10 dword ptr[ebp - WX_OFFSET - 0*4]



    //#define worker_register0  xmm0
    //#define worker_register1  xmm1

    #define _8pt_sum_response    xmm4
    #define _8pt_diff_response   xmm5
    #define _8pt_mean_response   xmm6
    #define _8pt_local_mean      xmm7


    #define _4pt_sum_response    mm4
    #define _4pt_diff_response   mm5
    #define _4pt_mean_response   mm6
    #define _4pt_local_mean      mm7


    //输入参数
    #define W   [ebx + 8]
    #define H   [ebx + 12]
    #define SRC [ebx + 16]
    #define DST [ebx + 20]

    #define PABSW(xmmr1, xmmr2) _asm\
    {\
    _asm pxor   xmmr1, xmmr1 \
    _asm psubw  xmmr1, xmmr2 \
    _asm pmaxsw xmmr1, xmmr2 \
    }

    

    _asm
    {
        //局部变量需要16字节对齐
                                  ;|   ...   |
        push ebx                  ;| old_ebx |←esp
                                  ;|   ret   |
                                  
        mov ebx, esp              ;|   ...   |
                                  ;| old_ebx |←ebx
                                  ;|   ret.  |
                                  
                                  ;|   ...   |←esp
        sub esp, 8                ;|   ...   |
                                  ;| old_ebx |←ebx
                                  ;|   ret   |
                                  
                                  
                                  ;|   ...   |←esp (esp is 16 bytes aligned )
                                  ;|   ...   |
                                  ;|   ...   |
                                  ;|   ...   |
       and esp, 0FFFFFFF0h        ;| old_ebx |←ebx
                                  ;|   ret   |
                                  
                                  
                                  
                                  
                                  ;|   ...   |←16 bytes aligned
                                  ;|   ...   |←esp
                                  ;|   ...   |
                                  ;|   ...   |
       add esp, 4                 ;| old_ebx |←ebx
                                  ;|   ret   |
                                  
                                  
                                  ;|   ebp   |←esp,16 bytes aligned
                                  ;|   ...   |
                                  ;|   ...   |
                                  ;|   ...   |
       push ebp                   ;| old_ebx |←ebx
                                  ;|   ret   |


                                  ;|   ebp   |←esp 16 bytes aligned
                                  ;|   ret   |
       mov ebp, dword ptr [ebx+4] ;|         |
                                  ;|   ...   |
       mov dword ptr [esp+4], ebp ;| old_ebx |←ebx
                                  ;|   ret   |


        mov ebp, esp              ;|   ebp   |←ebp(esp 16 bytes aligned)
                                  ;|   ret   |
                                  ;|         |
                                  ;|   ...   |
                                  ;| old_ebx |←ebx
                                  ;|   ret   |




       sub esp, LOCAL_VARIABLES_SIZE;



        pushad ; Temp  (ESP);
               ; Push(EAX);
               ; Push(ECX);
               ; Push(EDX);
               ; Push(EBX);
               ; Push(Temp);
               ; Push(EBP);
               ; Push(ESI);
               ; Push(EDI);

        mov ecx, 2
        lea edx, WX2
       
        //计算2*W, 3*W, 4*W, ..., 6*W
CALC_WXN:
        mov eax, ecx
        imul eax, W
        mov [edx], eax //[edx] = ecx*W
        add edx, 4
        inc ecx;
        cmp ecx, 6;
        jbe CALC_WXN;


        //计算圈层上的目标点的相对于7X7左上角的偏离量，点的标号见注释
        //point 0: 4      
         mov CIRCLE_SAMPLE_OFFSET00, 4

        //point 1: 3      
         mov CIRCLE_SAMPLE_OFFSET01, 3
        
        //point 2: 2      
        mov CIRCLE_SAMPLE_OFFSET02,  2
        
        //point 3: W  + 1 
        mov eax, W
        add eax, 1
        mov CIRCLE_SAMPLE_OFFSET03, eax

        //point 4: 2W
        mov eax, WX2
        mov CIRCLE_SAMPLE_OFFSET04, eax

        //point 5: 3W 
        mov eax, WX3
        mov CIRCLE_SAMPLE_OFFSET05, eax

        //point 6: 4W     
        mov eax, WX4
        mov CIRCLE_SAMPLE_OFFSET06, eax

        //point 7: 5W  + 1
        mov eax, WX5
        add eax, 1
        mov CIRCLE_SAMPLE_OFFSET07, eax

        //point 8: 6W + 2
        mov eax, WX6
        add eax, 2
        mov CIRCLE_SAMPLE_OFFSET08, eax

        //point 9: 6W + 3
        mov eax, WX6
        add eax, 3
        mov CIRCLE_SAMPLE_OFFSET09, eax

        //point 10:6W + 4
        mov eax, WX6
        add eax, 4
        mov CIRCLE_SAMPLE_OFFSET10, eax

        //point 11: 5W + 5
        mov eax, WX5
        add eax, 5
        mov CIRCLE_SAMPLE_OFFSET11, eax

        //point 12: 4W + 6
        mov eax, WX4
        add eax, 6
        mov CIRCLE_SAMPLE_OFFSET12, eax

        //point 13: 3W + 6
        mov eax, WX3
        add eax, 6
        mov CIRCLE_SAMPLE_OFFSET13, eax

        //point 14: 2W + 6
        mov eax, WX2
        add eax, 6
        mov CIRCLE_SAMPLE_OFFSET14, eax

        //point 15: W + 5
        mov eax, W
        add eax, 5
        mov CIRCLE_SAMPLE_OFFSET15, eax


        //=====计算中心点的8个邻接点与7X7图块的左上角的偏移量=====
        //  A  B  C
       //    □□□
       //   H□●□D
       //    □□□
       //   G  F   E
       //A点
       mov eax, WX2
       add eax, 2
       mov CENTER_NEIGHBOUR_OFFSET00, eax

       //B点
       mov eax, WX2
       add eax, 3
       mov CENTER_NEIGHBOUR_OFFSET01, eax

       //C点
       mov eax, WX2
       add eax, 4
       mov CENTER_NEIGHBOUR_OFFSET02, eax

       //D点
       mov eax, WX3
       add eax, 4
       mov CENTER_NEIGHBOUR_OFFSET03, eax

       //E点
       mov eax, WX4
       add eax, 4
       mov CENTER_NEIGHBOUR_OFFSET04, eax


       //F点
       mov eax, WX4
       add eax, 3
       mov CENTER_NEIGHBOUR_OFFSET05, eax


       //G点
       mov eax, WX4
       add eax, 2
       mov CENTER_NEIGHBOUR_OFFSET06, eax


       //H点
       mov eax, WX3
       add eax, 2
       mov CENTER_NEIGHBOUR_OFFSET07, eax


        finit  ;//reset the floating-point unit



        //水平循环次数
        mov eax, W
        sub eax, 6  //上下各去掉3行
        mov ecx, eax

        //垂直循环次数
        mov eax, H
        sub eax, 6 //左右各去掉3列
        mov edx, eax
        

        mov esi, SRC ;//pointer to 8 bit image source
        mov edi, DST ;//pointer to 16 bit response data

        //输出内存指针
        add edi, WX3;//WX3
        add edi, 3 ;

HLOOP:
       push esi
       push edi
       push ecx


WLOOP:
        cmp ecx, 8
        
        jb _4pt_PROCESS;//待处理点数 少于8个, 转4点处理

        //caculate local mean

       pxor xmm0, xmm0
       pxor _8pt_local_mean, _8pt_local_mean


       //A点
        mov eax, CENTER_NEIGHBOUR_OFFSET00
        movsd _8pt_local_mean, [esi + eax];//need not be 16-byte aligned
        punpcklbw _8pt_local_mean, xmm0;//the low eight btyes extend to eight word

       //B点
         mov eax, CENTER_NEIGHBOUR_OFFSET01
        movsd xmm1, [esi + eax];//need not be 16-byte aligned        
        punpcklbw xmm1, xmm0;
        paddw _8pt_local_mean, xmm1

        //C点
        mov eax, CENTER_NEIGHBOUR_OFFSET02
        movsd xmm1, [esi + eax];//need not be 16-byte aligned        
        punpcklbw xmm1, xmm0;
        paddw _8pt_local_mean, xmm1

        //D点
        mov eax, CENTER_NEIGHBOUR_OFFSET03
        movsd xmm1, [esi + eax]
        punpcklbw xmm1, xmm0
        paddw _8pt_local_mean, xmm1

        //E点
        mov eax, CENTER_NEIGHBOUR_OFFSET04
        movsd xmm1, [esi + eax]
        punpcklbw xmm1, xmm0
        paddw _8pt_local_mean, xmm1


        //F点
        mov eax, CENTER_NEIGHBOUR_OFFSET05
        movsd xmm1, [esi + eax]
        punpcklbw xmm1, xmm0
        paddw _8pt_local_mean, xmm1

        //G点
        mov eax, CENTER_NEIGHBOUR_OFFSET06
        movsd xmm1, [esi + eax]
        punpcklbw xmm1, xmm0
        paddw _8pt_local_mean, xmm1

        //H点
        mov eax, CENTER_NEIGHBOUR_OFFSET07
        movsd xmm1, [esi + eax]
        punpcklbw xmm1, xmm0
        paddw _8pt_local_mean, xmm1



        //读取圈层上的点
        //0 点
        push  ebx
        push  ecx
        push  edx


        lea ebx, CIRCLE_SAMPLE_OFFSET00
        lea edx, CIRCLE_SAMPLE00
        mov ecx, 16


_8pt_READ_CIRCLE_SAMPLE:
        mov eax, [ebx]             ;//[ebx]=eax=pixle byte offset relative to left-top of 11*11 block
        movsd xmm1, [esi + eax]    ;//read 8 bytes each time,
                                   ;//注意内存可能未16字节对齐
        punpcklbw xmm1, xmm0       ;//convert each 8 bytes to 8 words
        movdqa [edx], xmm1         ;//store data to cache， 内存变量CIRCLE_SAMPLEXX已经16字节对齐
        
        add ebx, 4                 ;//next offset position
        add edx, 16                ;//next CIRCLE_SAMPLE position
        dec ecx                    ;//loop
        jnz  _8pt_READ_CIRCLE_SAMPLE


//16个圈层上的点，组成4组十字
#define _8_PT_A xmmword ptr [eax         ]
#define _8_PT_B xmmword ptr [eax + 4 *16 ]
#define _8_PT_C xmmword ptr [eax + 8 *16 ]
#define _8_PT_D xmmword ptr [eax + 12*16 ]

//                A
//               ↓
//               ■
//               □
//               □
//               □
//               □
// B→ ■□□□□●□□□□■←D
//               □
//               □
//               □
//               □
//               ■
//               ↑
//               C

        //====Response Part=======
        pxor _8pt_sum_response    , _8pt_sum_response  ;//_8pt_sum_response
        pxor _8pt_diff_response   , _8pt_diff_response ;//_8pt_diff_response
        pxor _8pt_mean_response   , _8pt_mean_response ;//mean

        mov ecx, 0
        lea eax, CIRCLE_SAMPLE00

_8pt_CALC_EACH_RESPONSE://计算各响应


        //abs(A - B + C -D)
        movdqa xmm0, _8_PT_A
        psubw  xmm0, _8_PT_B
        paddw  xmm0, _8_PT_C
        psubw  xmm0, _8_PT_D

       
        PABSW(xmm1, xmm0);//xmm1 = abs(xmm0)
        paddw _8pt_sum_response, xmm1;

        //abs(A - C) + abs(B - D);
        movdqa xmm0, _8_PT_A
        psubw  xmm0, _8_PT_C
        PABSW (xmm1, xmm0);//xmm1 = abs(xmm0)
        paddw _8pt_diff_response, xmm1
        
        movdqa xmm0, _8_PT_B
        psubw  xmm0, _8_PT_D
        PABSW (xmm1, xmm0);//xmm1 = abs(xmm0)
        paddw  _8pt_diff_response, xmm1

        //;mean = A + B + C +D
        paddw  _8pt_mean_response, _8_PT_A;
        paddw  _8pt_mean_response, _8_PT_B;
        paddw  _8pt_mean_response, _8_PT_C;
        paddw  _8pt_mean_response, _8_PT_D;


        add eax, 16
        inc ecx
        cmp ecx, 4
        jnz _8pt_CALC_EACH_RESPONSE


        //计算结果
        //response = (_8pt_sum_response - _8pt_diff_response)*2 - abs(_8pt_mean_response - _8pt_local_mean)

        psrlw _8pt_mean_response, 1 ;//_8pt_mean_response除2, 以使取值范围一致

        psubw _8pt_mean_response, _8pt_local_mean
        PABSW(xmm0, _8pt_mean_response)      ;//xmm0 = abs(_8pt_mean_response/2 - _8pt_local_mean)

        movdqa   xmm1, _8pt_sum_response     ;//
        psubusw  xmm1, _8pt_diff_response    ;//xmm1  = _8pt_sum_response - _8pt_diff_response
        psubusw  xmm1, xmm0                  ;//xmm1 = (_8pt_sum_response - _8pt_diff_response) - abs(mean/2 - _8pt_local_mean)

        psrlw  xmm1, 3                       ;//divide by 8
        packuswb xmm1, xmm1                  ;//8个字压缩为8个字节
        
        
        //输出8个像素的检测结果
        movsd qword ptr[edi], xmm1


        pop edx
        pop ecx
        pop ebx


        add esi, 8  ;//一次处理8个像素点
        add edi, 8  ;//
        
        sub ecx, 8
        ja WLOOP

        jmp HLOOP_CONTINUE



_4pt_PROCESS://一次处理4点
       cmp ecx, 4
       jb _SINGLE_PT_PROCESS

       pxor mm0, mm0
       pxor _4pt_local_mean, _4pt_local_mean


       //A点
        mov eax, CENTER_NEIGHBOUR_OFFSET00
        movd _4pt_local_mean, [esi + eax];//need not be 16-byte aligned
        punpcklbw _4pt_local_mean, mm0;//the low eight btyes extend to eight word

       //B点
        mov eax, CENTER_NEIGHBOUR_OFFSET01
        movd mm1, [esi + eax];//need not be 16-byte aligned        
        punpcklbw mm1, mm0;
        paddw _4pt_local_mean, mm1

        //C点
        mov eax, CENTER_NEIGHBOUR_OFFSET02
        movd mm1, [esi + eax];//need not be 16-byte aligned        
        punpcklbw mm1, mm0;
        paddw _4pt_local_mean, xmm1

        //D点
        mov eax, CENTER_NEIGHBOUR_OFFSET03
        movd mm1, [esi + eax]
        punpcklbw mm1, mm0
        paddw _4pt_local_mean, mm1

        //E点
        mov eax, CENTER_NEIGHBOUR_OFFSET04
        movd mm1, [esi + eax]
        punpcklbw mm1, mm0
        paddw _4pt_local_mean, mm1


        //F点
        mov eax, CENTER_NEIGHBOUR_OFFSET05
        movd mm1, [esi + eax]
        punpcklbw mm1, mm0
        paddw _4pt_local_mean, mm1

        //G点
        mov eax, CENTER_NEIGHBOUR_OFFSET06
        movd mm1, [esi + eax]
        punpcklbw mm1, mm0
        paddw _4pt_local_mean, mm1

        //H点
        mov eax, CENTER_NEIGHBOUR_OFFSET07
        movd mm1, [esi + eax]
        punpcklbw mm1, mm0
        paddw _4pt_local_mean, mm1



        //读取圈层上的点
        //0 点
        push  ebx
        push  ecx
        push  edx


        lea ebx, CIRCLE_SAMPLE_OFFSET00
        lea edx, CIRCLE_SAMPLE00
        mov ecx, 16


_4pt_READ_CIRCLE_SAMPLE:
        mov eax, [ebx]            ;//eax=pixle byte offset relative to left-top of 11*11 bloack
        movd mm1, [esi + eax]     ;//read 8 bytes each time
        punpcklbw mm1, mm0        ;//convert each 8 bytes to 8 words
        movq  [edx], mm1          ;//store data to cache
        
        add ebx, 4                 ;//next offset position
        add edx, 16                ;//next CIRCLE_SAMPLE position
        dec ecx                    ;//loop
        jnz  _4pt_READ_CIRCLE_SAMPLE


        //====Response Part=======
        pxor _4pt_sum_response    , _4pt_sum_response  ;//
        pxor _4pt_diff_response   , _4pt_diff_response ;//
        pxor _4pt_mean_response   , _4pt_mean_response ;//

        mov ecx, 0
        lea eax, CIRCLE_SAMPLE00

#define _4_PT_A mmword ptr [eax         ]
#define _4_PT_B mmword ptr [eax + 4 *16 ]
#define _4_PT_C mmword ptr [eax + 8 *16 ]
#define _4_PT_D mmword ptr [eax + 12*16 ]

_4pt_CALC_EACH_RESPONSE://计算各响应


        //abs(A - B + C -D)
        movq  mm0, _4_PT_A
        psubw mm0, _4_PT_B
        paddw mm0, _4_PT_C
        psubw mm0, _4_PT_D

       
        PABSW(mm1, mm0);//mm1 = abs(xmm0)
        paddw _4pt_sum_response, mm1;

        //abs(A - C) + abs(B - D);
        movq   mm0, _4_PT_A
        psubw  mm0, _4_PT_C
        PABSW (mm1, mm0);//mm1 = abs(mm0)
        paddw _4pt_diff_response,mm1
        
        movq   mm0, _4_PT_B
        psubw  mm0, _4_PT_D
        PABSW (mm1, mm0);//mm1 = abs(mm0)
        paddw  _4pt_diff_response, mm1

        //;mean = A + B + C +D
        paddw  _4pt_mean_response, _4_PT_A;
        paddw  _4pt_mean_response, _4_PT_B;
        paddw  _4pt_mean_response, _4_PT_C;
        paddw  _4pt_mean_response, _4_PT_D;


        add eax, 16
        inc ecx
        cmp ecx, 4
        jnz _4pt_CALC_EACH_RESPONSE


        //计算结果
        //response = (_4pt_sum_response - _4pt_diff_response)*2 - abs(_4pt_mean_response - _4pt_local_mean)

        psrlw _4pt_mean_response, 1 ;//_4pt_mean_response/2
        psubw _4pt_mean_response, _4pt_local_mean
        PABSW(mm0, _4pt_mean_response)      ;//mm0 = abs(_4pt_mean_response - _4pt_local_mean)

        movq     mm1, _4pt_sum_response     ;//
        psubusw  mm1, _4pt_diff_response    ;//mm1  = _4pt_sum_response - _4pt_diff_response       
        psubusw  mm1, mm0                   ;//mm1 = (_4pt_sum_response - _4pt_diff_response) - abs(_4pt_mean_response/2 - _4pt_local_mean)
        

       psrlw    mm1, 3                      ;//divide by 8
       packuswb mm1, mm1                    ;//8个字压缩为8个字节

        //输出4个像素的检测结果
        movd [edi], mm1;

        pop edx
        pop ecx
        pop ebx

        add esi, 4  ;//一次处理4个像素点
        add edi, 4  ;//

        sub ecx, 4


_SINGLE_PT_PROCESS://一次处理1点,不做任何处理,只是保证循环计数变量值的正确
        cmp ecx,0
        je HLOOP_CONTINUE





        add esi, 1   ;//一次处理1个像素点
        add edi, 1   ;//

        dec ecx
        jmp _SINGLE_PT_PROCESS



HLOOP_CONTINUE:

        pop ecx
        pop edi
        pop esi

        add edi, W  ;//输出数据每个像素有1个字节
        add esi, W  ;//元数据每个像素1个字节

        dec edx
        jnz HLOOP;


        emms  ;Empty the multimedia state.


        popad ;Registers are popped in the following order: 
              ;EDI
              ;ESI
              ;EBP 
              ;ESP,increment ESP by 4 (* skip next 4 bytes of stack *)
              ;EBX
              ;EDX
              ;ECX
              ;EAX.  
              ;The (E)SP value popped from the stack is actually discarded.


        mov  esp,ebp 
        pop  ebp

        mov esp, ebx;
        pop ebx;
        ret;

    }
}