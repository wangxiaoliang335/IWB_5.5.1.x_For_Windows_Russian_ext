//@功能:使用MMX指令快速屏蔽YUY2图片帧
//@参数:pYUY2Src, 指向YUY2源数据的指针
//      pYUY2Mask， 指向YUY2屏蔽图数据的指针
//      nPixelCount，像素总数
extern "C"
void  _declspec(naked) YUY2FrameMask_MMX(
    unsigned short* pYUY2Src,
    const unsigned short* pYUY2Mask,
    unsigned int nPixelCount)
{
    _asm
    {
        pushad
        finit
        mov edi, [esp + 4*8 + 4  ] ;//pYUY2,
        mov esi, [esp + 4*8 + 8  ] ;//pYUY2Mask;
        mov ecx, [esp + 4*8 + 12 ] ;//nPixelCount

_loop_Next4Pixels:
        movq mm0, [esi];//原图数据
        add esi, 8

        movq mm1, [edi];//屏蔽图数据
        pand mm0, mm1  ;//

        movq [edi], mm0;//保存屏蔽后的数据
        add edi, 8


        sub ecx, 4
        jnz _loop_Next4Pixels
        emms;
        popad
        ret
    }

}


extern "C"
void  _declspec(naked) GrayFrameMask_MMX(
    unsigned short* pGraySrc,
    const unsigned short* pGrayMask,
    unsigned int nPixelCount)
{
    _asm
    {
        pushad
        finit
        mov edi, [esp + 4*8 + 4  ] ;//pGray,
        mov esi, [esp + 4*8 + 8  ] ;//pGrayMask;
        mov ecx, [esp + 4*8 + 12 ] ;//nPixelCount

_loop_Next8Pixels:
        movq mm0, [esi];//原图数据
        add esi, 8

        movq mm1, [edi];//屏蔽图数据
        pand mm0, mm1  ;//

        movq [edi], mm0;//保存屏蔽后的数据
        add edi, 8


        sub ecx, 8
        jnz _loop_Next8Pixels
        emms;
        popad
        ret
    }

}


//@功能:使用SSE指令将YUY2图片帧转化为1位位图
extern "C"
void _declspec(naked) YUY2ToBitFrame_SSE(
    const unsigned short* pYUY2,
    unsigned char* pBits,
    unsigned char cThreshold,
    unsigned int nPixelCount)
{
    static const unsigned char mask[8]={0xFF, 0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00};
    _asm
    {
        pushad
        finit
        mov esi, [esp + 4*8 + 4  ] ;//pYUY2,
        mov edi, [esp + 4*8 + 8  ] ;//pBits;
        mov eax, [esp + 4*8 + 12 ] ;//cThreshold
        mov ecx, [esp + 4*8 + 16 ] ;//nPixelCount


        mov  ah,  al;
        mov  bx,  ax;
        shl  eax, 16;
        mov  ax,  bx;

        //mm3 = 00 00 00 00 T T T T
        movd mm3, eax;

        //mm4 = 00 00 00 00 T T T T
        movd mm4, eax;

        //mm3 = T T T T T T T T
        //punpckldq a, b
        //Interleave the 32-bit value from the low half of a with the 32-bit value from the low half of b and take the least significant element from a.
        punpckldq mm3, mm4;



        movq mm0, mask;


_loop_Next32Pixels:
        //[Step 1].0~7 Bytes
        //mm1 = V1 Y3 U1 Y2  V0 Y1 U0 Y0
        movq mm1, [esi];

        //mm2 = V3 Y7 U3 Y6  V2 Y5 U2 Y4
        movq mm2, [esi+8]
        add esi, 16

        //mm1 = 00 Y3 00 Y2  00 Y1 00 Y0
        pand mm1, mm0;

        //mm2 = 00 Y7 00 Y6  00 Y5 00 Y4
        pand mm2, mm0;

        //mm1= Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
        packuswb mm1, mm2;

        //无符号字节比较
        //与门限比较
        //min(val,T)
        pminub  mm1, mm3
        
        //min(val, T) == T
        pcmpeqb mm1, mm3


        pmovmskb eax, mm1
        mov ebx,eax;




        //[Step 2].8~15 Bytes
        //mm1 = V1 Y3 U1 Y2  V0 Y1 U0 Y0
        movq mm1, [esi];

        //mm2 = V3 Y7 U3 Y6  V2 Y5 U2 Y4
        movq mm2, [esi+8]
        add esi, 16

        //mm1 = 00 Y3 00 Y2  00 Y1 00 Y0
        pand mm1, mm0;

        //mm2 = 00 Y7 00 Y6  00 Y5 00 Y4
        pand mm2, mm0;

        //mm1= Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
        packuswb mm1, mm2;

        //无符号字节比较
        //与门限比较
        //min(val,T)
        pminub  mm1, mm3
        //min(val, T) == T
        pcmpeqb mm1, mm3

        pmovmskb eax, mm1
        shl eax, 8;
        or ebx, eax



        //[Step 3] 16~23 Bytes
        //mm1 = V1 Y3 U1 Y2  V0 Y1 U0 Y0
        movq mm1, [esi];

        //mm2 = V3 Y7 U3 Y6  V2 Y5 U2 Y4
        movq mm2, [esi+8]
        add esi, 16

        //mm1 = 00 Y3 00 Y2  00 Y1 00 Y0
        pand mm1, mm0;

        //mm2 = 00 Y7 00 Y6  00 Y5 00 Y4
        pand mm2, mm0;


        //mm1= Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
        packuswb mm1, mm2;


        //无符号字节比较
        //与门限比较
        //min(val,T)
        pminub  mm1, mm3
        //min(val, T) == T
        pcmpeqb mm1, mm3

        pmovmskb eax, mm1
        shl eax, 16
        or ebx, eax


        //[Step 4] 16~23 Bytes
        //mm1 = V1 Y3 U1 Y2  V0 Y1 U0 Y0
        movq  mm1, [esi];

        //mm2 = V3 Y7 U3 Y6  V2 Y5 U2 Y4
        movq mm2, [esi+8]
        add esi, 16


        //mm1 = 00 Y3 00 Y2  00 Y1 00 Y0
        pand mm1, mm0;

        //mm2 = 00 Y7 00 Y6  00 Y5 00 Y4
        pand mm2, mm0;

        //mm1= Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
        packuswb mm1, mm2;

        //无符号字节比较
        //与门限比较
        //min(val,T)
        pminub  mm1, mm3
        //min(val, T) == T
        pcmpeqb mm1, mm3
        pmovmskb eax, mm1

        shl eax, 24
        or ebx, eax


        //输出结果
        mov [edi], ebx;

        add edi, 4

        sub ecx, 32;

        jnz _loop_Next32Pixels

        emms;
        popad
        ret

    }
}




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
extern "C"
void _declspec(naked) DynamicMaskStatistic_YUY2_MMX(
                                        const unsigned short* pImageData,//YUY2格式的位图
                                        unsigned char* pStatistic       ,//统计位图
                                        unsigned char cCmpTheshold      ,//比较门限
                                        int nPixelCount                  //像素总数
                                        )
{

    static const unsigned char mask[8]={0xFF, 0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00};
    _asm
    {

        pushad

       //reset the floating-point unit
       finit

        mov esi, [esp + 4*8 + 4*1];//YUY2格式的位图
        mov edi, [esp + 4*8 + 4*2];//每个像素的计数器，一个像素对应一个16位的计数器
        mov eax, [esp + 4*8 + 4*3];//门限
        mov ecx, [esp + 4*8 + 4*4];//像素个数


        //mm7中存放比较门限
        //mm7 = T T T T T T T T
        movd mm7, eax;
        punpcklbw mm7, mm7;
        punpcklbw mm7, mm7;
        punpcklbw mm7, mm7;


        //mm6中存放计数器增量
        //01 01 01 01 01 01 01 01
        mov eax,0x01;
        ;//mm6 = XX XX XX XX XX XX XX 01
        movd mm6, eax;

        ;//mm6 = XX XX XX XX XX XX 01 01
        punpcklbw mm6, mm6;

        ;//mm6 = XX XX XX XX 01 01 01 01
        punpcklbw mm6, mm6;

        ;//mm6 =  01 01 01 01 01 01 01 01
        punpcklbw mm6, mm6;


        movq mm5, mask 

_loop_next_8_pixels:

        //1.提取8个像素的亮度值
        //mm0 =  V1 Y3 U1 Y2 V0 Y1 U0 Y0
        movq mm0, [esi]

        //mm1 =  V3 Y7 U3 Y6 V2 Y5 U2 Y4
        movq mm1, [esi+8];

        //
        add esi, 16

        //mm0 = 00 Y3 00 Y2 00 Y1 00 Y0
        pand mm0, mm5;

        //mm1 = 00 Y7 00 Y6 00 Y5 00 Y4
        pand mm1, mm5;

        //mm1 = Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
        packuswb mm0, mm1


        //2.计算每个字节的屏蔽字
       //无符号比较,注意不能够使用pcmpgtb
       //mm0 >= mm7判断
       //mm0存放min(mm0,mm7)
       pminub  mm0, mm7
       //min(mm0[i], mm7[i]) == mm7[i],意味mm0[i]着大于门限T
       pcmpeqb mm0, mm7

       //比较结果用作屏蔽字, mm0[i]若大于T, 则mm1[i] == 0xFF
       movq mm1, mm0

       //依照屏蔽字设置+1还是+0。
       //mm0= 01 00 01 01 01 00  00 10
       pand mm1, mm6

       //统计屏蔽图
       //mm1 = Counter[Y7] Counter[Y6] Counter[Y5] Counter[Y4] Counter[Y3] Counter[Y2] Counter[Y1] Counter[Y0]
       movq mm2, [edi];


      //Subtract packed unsigned 8-bit integers in b from a, and store unsigned satured results in dst
      //向素值大于门限T的像素在统计屏蔽图中的计数+1
      paddusb mm2, mm1


      //3.
      //mm1 = FF FF FF FF FF FF FF FF
      //置mm18字节全为0xFF
      pcmpeqb mm1,mm1

      //屏蔽字取反，即<门限T, 则对应的屏蔽字为FF
      pxor mm0, mm1

      //设置每个像素是-1还是-0。
      pand mm0, mm6

      //Subtract packed unsigned 8-bit integers in b from a, and store unsigned satured results in dst
      //向素值小于门限T的像素在统计屏蔽图中的计数-1，>=T, 则计数不变。
      psubusb mm2, mm0;

      movq [edi], mm2;

      add edi, 8

      sub ecx, 8

      jnz _loop_next_8_pixels;//处理未结束则继续处理下8个像素。


      emms
      popad
      ret 

    }

}


extern "C"
void _declspec(naked) DynamicMaskStatistic_Gray_MMX(
                                        const unsigned short* pImageData,//8Bit灰度格式的位图
                                        unsigned char* pStatistic       ,//统计位图
                                        unsigned char cCmpTheshold      ,//比较门限
                                        int nPixelCount                  //像素总数
                                        )
{

    _asm
    {

        pushad

       //reset the floating-point unit
       finit

        mov esi, [esp + 4*8 + 4*1];//8Bit灰度格式的位图
        mov edi, [esp + 4*8 + 4*2];//每个像素的计数器，一个像素对应一个16位的计数器
        mov eax, [esp + 4*8 + 4*3];//门限
        mov ecx, [esp + 4*8 + 4*4];//像素个数


        //mm7中存放比较门限
        //mm7 = T T T T T T T T
        movd mm7, eax;
        punpcklbw mm7, mm7;
        punpcklbw mm7, mm7;
        punpcklbw mm7, mm7;


        //mm6中存放计数器增量
        //01 01 01 01 01 01 01 01
        mov eax,0x01;
        ;//mm6 = XX XX XX XX XX XX XX 01
        movd mm6, eax;

        ;//mm6 = XX XX XX XX XX XX 01 01
        punpcklbw mm6, mm6;

        ;//mm6 = XX XX XX XX 01 01 01 01
        punpcklbw mm6, mm6;

        ;//mm6 =  01 01 01 01 01 01 01 01
        punpcklbw mm6, mm6;


_loop_next_8_pixels:

        //1.提取8个像素的亮度值
        //mm0 =  Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
        movq mm0, [esi]



        //2.计算每个字节的屏蔽字
       //无符号比较,注意不能够使用pcmpgtb
       //mm0 >= mm7判断
       //mm0存放min(mm0,mm7)
       pminub  mm0, mm7
       //min(mm0[i], mm7[i]) == mm7[i],意味mm0[i]着大于门限T
       pcmpeqb mm0, mm7

       //比较结果用作屏蔽字, mm0[i]若大于T, 则mm1[i] == 0xFF
       movq mm1, mm0

       //依照屏蔽字设置+1还是+0。
       //mm0= 01 00 01 01 01 00  00 10
       pand mm1, mm6

       //统计屏蔽图
       //mm1 = Counter[Y7] Counter[Y6] Counter[Y5] Counter[Y4] Counter[Y3] Counter[Y2] Counter[Y1] Counter[Y0]
       movq mm2, [edi];


      //Subtract packed unsigned 8-bit integers in b from a, and store unsigned satured results in dst
      //向素值大于门限T的像素在统计屏蔽图中的计数+1
      paddusb mm2, mm1


      //3.
      //mm1 = FF FF FF FF FF FF FF FF
      //置mm18字节全为0xFF
      pcmpeqb mm1,mm1

      //屏蔽字取反，即<门限T, 则对应的屏蔽字为FF
      pxor mm0, mm1

      //设置每个像素是-1还是-0。
      pand mm0, mm6

      //Subtract packed unsigned 8-bit integers in b from a, and store unsigned satured results in dst
      //向素值小于门限T的像素在统计屏蔽图中的计数-1，>=T, 则计数不变。
      psubusb mm2, mm0;

      movq [edi], mm2;

      add edi, 8
      add esi, 8

      sub ecx, 8

      jnz _loop_next_8_pixels;//处理未结束则继续处理下8个像素。


      emms
      popad
      ret 

    }

}

//@功能:使用SSE2指令将8位灰度图片帧转化为1位位图
//@参数:pGray, 输入参数,指向8位灰度图片的首地址
//      pBits, 输出参数,
extern "C"
void _declspec(naked) GrayToBitFrame_SSE2(
    const unsigned char* pGray,
    unsigned char* pBits,
    unsigned char cThreshold,
    unsigned int nPixelCount)
{
    _asm
    {
        pushad
        finit
        mov esi, [esp + 4*8 + 4  ] ;//pGray,
        mov edi, [esp + 4*8 + 8  ] ;//pBits;
        mov eax, [esp + 4*8 + 12 ] ;//cThreshold
        mov ecx, [esp + 4*8 + 16 ] ;//nPixelCount


		//set threshold in xmm0
        mov  ah,  al;
        mov  bx,  ax;
        shl  eax, 16;
        mov  ax,  bx;

        //xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 T T T T
        //Here, T is the threhold
        movd xmm0, eax;

        //xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 T T T T
        movdqa xmm1, xmm0

        //xmm0 = 00 00 00 00 00 00 00 00 T T T T T T T T
        punpckldq xmm0, xmm1;

        //xmm1 = 00 00 00 00 00 00 00 00 T T T T T T T T
        movdqa xmm1, xmm0

        //xmm0 = T T T T T T T T T T T T T T T T T T T
        punpckldq xmm0, xmm1;


_loop_Next32Pixels:
        movdqa xmm1, [esi]


         //判断是否">="门限T,
        //注意不能使使用有符号的比较指令pcmpgtb
        //无符号字节比较
        //与门限比较
        //xmm1 = min(val,T)
        pminub  xmm1, xmm0
        
        //判断min(val, T) == T
        //xmm1中"==T"的字节,被置为FF
        pcmpeqb xmm1, xmm0

        //eax的低16位由xmm1的每个字节的第8位组成。
        pmovmskb eax, xmm1

        mov ebx,eax;


        add esi, 16
        movdqa xmm1, [esi]


        //无符号字节比较
        //与门限比较
        //注意不能使使用有符号的比较指令pcmpgtb
        //xmm1 = min(val,T)
        pminub  xmm1, xmm0
        
        //判断min(val, T) == T
        //xmm1中"==T"的字节,被置为FF
        pcmpeqb xmm1, xmm0

        //eax的低16位由xmm1的每个字节的第8位组成。
        pmovmskb eax, xmm1

        //移入eax高16位
        shl eax, 16

        //高16位和低16位合并。
        or ebx,eax;

        add esi, 16


        //输出结果
        mov [edi], ebx;

        add edi, 4

        sub ecx, 32;

        jnz _loop_Next32Pixels

        emms;
        popad
        ret

    }
}


//@功能:使用SSE2指令将8位灰度图片帧二值化,目标像素值为0xFF, 其他像素值为0x00, 转化为1位位图
//@参数:pGray, 输入/输出参数,输入指向8位灰度图片的首地址,输出保存二值划后的结果
//      pBits, 输出参数,1位位图
//     cThreshold, 二值化门限
//    nPixelCount, 像素总数
extern "C"
void _declspec(naked) BinarizeGrayToBitFrame_SSE2(
    unsigned char* pGray,
    unsigned char* pBits,
    unsigned char cThreshold,
    unsigned int nPixelCount)
{
    _asm
    {
        pushad
        finit
        mov esi, [esp + 4*8 + 4  ] ;//pGray,
        mov edi, [esp + 4*8 + 8  ] ;//pBits;
        mov eax, [esp + 4*8 + 12 ] ;//cThreshold
        mov ecx, [esp + 4*8 + 16 ] ;//nPixelCount


        //set threshold in xmm0
        mov  ah,  al;
        mov  bx,  ax;
        shl  eax, 16;
        mov  ax,  bx;

        //xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 T T T T
        //Here, T is the threhold
        movd xmm0, eax;

        //xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 T T T T
        movdqa xmm1, xmm0

        //xmm0 = 00 00 00 00 00 00 00 00 T T T T T T T T
        punpckldq xmm0, xmm1;

        //xmm1 = 00 00 00 00 00 00 00 00 T T T T T T T T
        movdqa xmm1, xmm0

        //xmm0 = T T T T T T T T T T T T T T T T T T T
        punpckldq xmm0, xmm1;


_loop_Next32Pixels:
        movdqa xmm1, [esi]


        //判断是否">="门限T,
        //注意不能使使用有符号的比较指令pcmpgtb
        //无符号字节比较
        //与门限比较
        //xmm1 = min(val,T)
        pminub  xmm1, xmm0
        
        //判断min(val, T) == T
        //xmm1中"== T"的字节,被置为FF
        pcmpeqb xmm1, xmm0

        //二值化数据回写灰度帧
        //2016/11/17
        movdqa [esi], xmm1;

        //eax的低16位由xmm1的每个字节的第8位组成。
        pmovmskb eax, xmm1

        mov ebx,eax;


        add esi, 16

        movdqa xmm1, [esi]


        //无符号字节比较
        //与门限比较
        //注意不能使使用有符号的比较指令pcmpgtb
        //xmm1 = min(val,T)
        pminub  xmm1, xmm0
        
        //判断min(val, T) == T
        //xmm1中"==T"的字节,被置为FF
        pcmpeqb xmm1, xmm0

        //二值化数据回写灰度帧
        //2016/11/17
        movdqa [esi], xmm1;

        //eax的低16位由xmm1的每个字节的第8位组成。
        pmovmskb eax, xmm1

        //移入eax高16位
        shl eax, 16

        //高16位和低16位合并。
        or ebx,eax;

        add esi, 16


        //输出结果
        mov [edi], ebx;

        add edi, 4

        sub ecx, 32;

        jnz _loop_Next32Pixels

        emms;
        popad
        ret

    }
}





//@功能:使用SSE2指令将1位位图帧转化为8位灰度图片
//@参数:pGray, 输入参数,指向8位灰度图片的首地址
//      pBits, 输出参数,
extern "C"
void _declspec(naked) BitToGrayFrame_MMX(
    const unsigned char* pBits,
    unsigned char* pGray,
    unsigned int nPixelCount)
{

    _asm
    {
        pushad
        finit
        mov esi, [esp + 4*8 + 4 ] ;//pBits
        mov edi, [esp + 4*8 + 8 ] ;//pGray
        mov ecx, [esp + 4*8 + 12] ;//nPixelCount

        ;//prepare dqword  mask = 0x80 0x40 0x20 0x10 0x08 0x04 0x02 0x01
        mov eax, 08040201h
        movd mm1, eax
       

        ;//eax=80402010h
        mov eax, 80402010h
        movd mm0, eax
        psllq mm0,32 ;//mm0 = 0x80 0x40 0x20 0x10 00 00 00 00 00 00 00 00
        ;//mm1= 0x80 0x40 0x20 0x10 0x08 0x04 0x02 0x01
        por mm1, mm0;

        
_loop_next_8_bits:
        xor eax, eax ;
        mov al, [esi]      ;//eax = 00 00 00 DD
        movd mm0, eax      ;//mm0 = 00 00 00 00 00 00 00 DD
        punpcklbw mm0, mm0 ;//mm0 = 00 00 00 00 00 00 DD DD
        pshufw mm0, mm0, 0 ;//mm0 = DD DD DD DD DD DD DD DD
        
        //DD DD DD DD DD DD DD DD & 8040201008040201
        pand mm0,mm1      ;

        //
        pcmpeqb mm0, mm1  ;//
        movq [edi], mm0  ;//store the result in memory
        add edi, 8;
        inc esi   ;
        sub ecx, 8;
        jnz _loop_next_8_bits;//next bytes

        emms
        popad;
        ret
    }

}

//@功能:将8位灰度图片转换为ARGB格式
//@参数:
//     p8BitGrayData, 灰度图片数据
//      pARGBData, ARGB数据
//      pixelCount, 像素个数
//      
extern "C"
void _declspec(naked) GrayToARGB_SSE2(
        const unsigned char* p8BitGrayData, 
        unsigned long* pARGBData, 
        unsigned long pixelCount)
{

 __declspec( align( 16) ) static const unsigned char AlphaCode[16] =
 {
    0x00,0x00,0x00,0xFF,
    0x00,0x00,0x00,0xFF,
    0x00,0x00,0x00,0xFF,
    0x00,0x00,0x00,0xFF
 };

    _asm
    {
        pushad
        finit

        mov esi, [esp + 4*8 + 4 ] ;//p8BitGrayData
        mov edi, [esp + 4*8 + 8 ] ;//pARGBData
        mov ecx, [esp + 4*8 + 12] ;//nPixelCount

        ;//xxm7 = FF 00 00 00 FF 00 00 00 FF 00 00 00 FF 00 00 00
        movdqa xmm7, AlphaCode ;//常数


_next_16_pixles:

        ;//读取16个像素的灰度值
        ;//xmm0 = PF PE PD PC PB PA P9 P8 P7 P6 P5 P4 P3 P2 P1 P0
        movdqa xmm0,[esi];

        //------0~7像素-------
        ;//灰度数据
        ;//xmm1 = PF PE PD PC PB PA P9 P8 P7 P6 P5 P4 P3 P2 P1 P0
        movdqa xmm1, xmm0


        ;//扩展0~7像素为字
        ;//xmm1 = P7P7 P6P6 P5P5 P4P4 P3P3 P2P2 P1P1 P0P0
        punpcklbw xmm1, xmm1

        ;//灰度数据
        movdqa xmm2, xmm1

        ;//0~3像素扩展为双字
        ;//xmm2 = P3P3P3P3 P2P2P2P2 P1P1P1P1 P0P0P0P0
        punpcklbw xmm2, xmm2
       
        ;//设置0~3像素的Alpha值
        por xmm2, xmm7

        ;//保存0~3像素的ARGB数据
        movdqa [edi], xmm2;


        ;//4~7像素扩展为双字
        ;//xmm1 = P7P7P7P7 P6P6P6P6 P5P5P5P5 P4P4P4P4
        punpckhbw xmm1,xmm1

        ;//设置4~7像素的Alpha值
        por xmm1, xmm7

        ;//保存4~7像素的ARGB数据
        movdqa [edi + 16], xmm1;


        ;//----8~11像素----
        ;//8~11像素扩展为字
        ;//xmm0 = PFPF PEPE PDPD PCPC PBPB PAPA P9P9 P8P8
        punpckhbw xmm0, xmm0

        ;//xmm1 = PFPF PEPE PDPD PCPC PBPB PAPA P9P9 P8P8
        movdqa xmm1, xmm0;


        ;//8~11像素扩展为双字
        ;//xmm1 = PBPBPBPB PAPAPAPA P9P9P9P9 P8P8P8P8
        punpcklbw xmm1,xmm1


        ;//设置8~11像素的Alpha值
        por xmm1, xmm7

        ;//保存8~11像素的ARGB数据
        movdqa [edi + 32], xmm1;//
        
        ;//12~15像素
        ;//12~15像素扩展为双字
        ;//xmm0 = PFPFPFPF PEPEPEPE PDPDPDPD PCPCPCPC
        punpckhbw xmm0,xmm0


        ;//设置12~15像素的Alpha值
        por xmm0, xmm7
        ;//保存12~15像素的ARGB数据
        movdqa [edi + 48], xmm0;//保存ARGB数据

        add esi, 16
        add edi,64
        sub ecx,16
        jnz _next_16_pixles;
        emms
        popad
        ret

    }

}