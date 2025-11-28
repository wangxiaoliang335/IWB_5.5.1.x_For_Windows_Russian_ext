#pragma once
//@function: convert ARGB to DIB(device independent bitmap) in which bitmap is stored upside down.
//@paramteres: pARGB, pointer which points to pARGB last 4 data, is input parameter
//             widht, the width of image
//             height, the height of image
//             rgb, pointer which points to rgb destination buffer, is output parameter.
//           pFirstLineDibData, the first line  of dib data.
//
//@Note:In DIB format, the first line is stored lastly.
//SSSE3:Supplemental Streaming SIMD Extensions 3
//PSHUFB:
//
inline _declspec(naked) void ARGBTo24BitDIB_SSSE3(
    const unsigned char* pARGB,
    unsigned short nWidth,
    unsigned short nHeight,
    unsigned char* pFirstLineDibData)
{
    __declspec( align( 16) ) static const unsigned char ShuffleIndex[16] =
    {   
   //NO  0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15
   //FRM B0,  G0,  R0,  A0,  B1,  G1,  R1,  A1,  B2,  R2,  G2,  A2,  B3,  G3,  R3,  A3
   //TO  B0,  G0,  R0,  B1,  G1,  R1,  B2,  R2,  G2,  B3,  G3,  R3,  A0,  A1,  A2,  A3
      0x00, 0x01,0x02,0x04,0x05,0x06,0x08,0x09,0x0A,0x0C,0x0D,0x0E,0x03,0x07,0x0B,0x0F
    };
   
    __declspec( align( 16) )static const unsigned char Mask[16] =
    {
       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,0x00, 0x00, 0x00
    };

    _asm
    {
        //push all registers on the stack.
        pushad

        //reset the floating-point unit
        finit


        //The 32 is caused by pushad
        mov esi, [esp + 32 +  4 ];//pointer to ARGB data
        mov ecx, [esp + 32 +  8 ];//width
        mov edx, [esp + 32 +  12];//height
        mov edi, [esp + 32 + 16 ];//pointer to DIB rgb data

        xor eax, eax
        mov eax, ecx;
        imul eax,eax,6;//
        mov  ebp, eax ;//ebp = width*3*2, 每次行处理后, edi减去的偏移量。
        
        
        
        movdqa xmm0, ShuffleIndex
        movdqa xmm1, Mask
_H_LOOP:
        push ecx

_W_LOOP:
        ;//xmm4 = A3,  R3,  G3,  B3,  A2,  R2,  G2,  B2,  A1,  R1,  G1,  B1,  A0,  R0,  G0,  B0
        movdqa xmm4, [esi];//
        ;//xmm4 = A3,  A2,  A1,  A0,  R3,  G3,  B3,  R2,  G2,  B2,   R1,  G1,  B1,  R0,  G0,  B0
        pshufb xmm4, xmm0
        ;//xmm4 = 00,  00,  00,  00,  R3,  G3,  B3,  R2,  G2,  B2,   R1,  G1,  B1,  R0,  G0,  B0
        pand   xmm4, xmm1
        
        ;//------------------------------------------------------------------------------------
        ;//xmm5 = A7,  R7,  G7,  B7,  A6,  R6,  G6,  B6,  A5,  R5,  G5,  B5,  A4,  R4,  G4,  B4
        movdqa xmm5, [esi+16];

        ;//xmm5 = A7,  A6,  A5,  A4,  R7,  G7,  B7,  R6,  G6,  B6,  R5,  G5,  B5,  R4,  G4,  B4
         pshufb xmm5, xmm0

        ;//xmm5 = 00,  00,  00,  00,  R7,  G7,  B7,  R6,  G6,  B6,  R5,  G5,  B5,  R4,  G4,  B4
        pand   xmm5, xmm1

        ;//xmm6 =  00,  00,  00,  00,  R7,  G7,  B7,  R6,  G6,  B6,  R5,  G5,  B5,  R4,  G4,  B4
        movdqa  xmm6, xmm5

        ;//xmm6 =  B5,  R4,  G4,  B4,  00,  00,  00,  00,  00,  00,  00,  00,  00,  00,  00,  00
        pslldq xmm6, 12

        ;//xmm4 =  B5,  R4,  G4,  B4,  R3,  G3,  B3,  R2,  G2,  B2,   R1,  G1,  B1,  R0,  G0,  B0
        paddb xmm4, xmm6;

        //-------------------------------------------------------------------------------------
        movdqa [edi], xmm4;//写入16字节(0~15),xmm4随后可用
        //-------------------------------------------------------------------------------------

        ;//xmm5 =  00,  00,  00,  00,  00,  00,  00,  00,  R7,  G7,  B7,  R6,  G6,  B6,  R5,  G5
        psrldq xmm5, 4

        
         //-------------------------------------------------------------------------------------
        ;//xmm4 = AB,  RB,  GB,  BB,  AA,  RA,  GA,  BA,  A9,  R9,  G9,  B9,  A8,  R8,  G8,  B8
         movdqa xmm4, [esi + 32];//
        ;//xmm4 = AB,  AA,  A9,  A8,  RB,  GB,  BB,  RA,  GA,  BA,  R9,  G9,  B9,  R8,  G8,  B8
        pshufb xmm4, xmm0
        
       ;//xmm4 = 00,  00,  00,  00,  RB,  GB,  BB,  RA,  GA,  BA,  R9,  G9,  B9,  R8,  G8,  B8
        pand   xmm4, xmm1
       
       ;//xmm6 = 00,  00,  00,  00,  RB,  GB,  BB,  RA,  GA,  BA,  R9,  G9,  B9,  R8,  G8,  B8
        movdqa xmm6, xmm4

        ;//xmm6 = GA,  BA,  R9,  G9,  B9,  R8,  G8,  B8,  00,  00,  00,  00,  00,  00,  00, 00
        pslldq xmm6, 8

        ;//xmm5 = GA,  BA,  R9,  G9,  B9,  R8,  G8,  B8,  R7,  G7,  B7,  R6,  G6,  B6,  R5,  G5
        paddb xmm5, xmm6;


        ;//-------------------------------------------------------------------------------------
        movdqa [edi + 16], xmm5;//写入16字节(16~31),xmm5随后可用
        
        ;//xmm4 = 00,  00,  00,  00,  00,  00,  00,  00,  00,  00,  00,  00,  RB,  GB,  BB,  RA
         psrldq xmm4, 8

        
        ;//-------------------------------------------------------------------------------------
        ;//xmm5 = AF,  RF,  GF,  BF,  AE,  RE,  GE,  BE,  AD,  RD,  GD,  BD,  AC,  RC,  GC,  BC
        movdqa xmm5, [esi + 48];//

        ;//xmm5 = AF,  AE,  AD,  AC,  RF,  GF,  BF,  RE,  GE,  BE,  RD,  GD,  BD,  RC,  GC,  BC
         pshufb xmm5, xmm0

        ;//xmm5 = RF,  GF,  BF,  RE,  GE,  BE,  RD,  GD,  BD,  RC,  GC,  BC,  00,  00,  00,  00
         pslldq  xmm5, 4

         ;//xmm4 = RF,  GF,  BF,  RE,  GE,  BE,  RD,  GD,  BD,  RC,  GC,  BC,  RB,  GB,  BB,  RA
         paddb xmm4,xmm5

         movdqa [ edi + 32], xmm4;//写入16字节(32~48)
         
        
        add esi, 64
        add edi, 48

        sub  ecx, 16;//16 pixel each loop
        jnz _W_LOOP;

        sub edi, ebp
        pop ecx
        dec edx
        jnz _H_LOOP


        //Empty the multimedia state.
        emms

        //restore values of all register;
        popad

         //
        ret
    }//_asm


}