// MJPG_Decoder.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "../../inc/MJPG/MJPG_Decoder.h"
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
//#define JPEG_INTERNALS
//#include "jpeg/jpeglib.h"
//#include "jpeg/jinclude.h"
//#include "libjpeg-turbo-1.2.90/jconfig.h"

//避免在"jmorecfg.h"中的"typedef long INT32" 导致的INT32重定义
#define XMD_H 

#include "../libjpeg-turbo-1.2.90/jpeglib.h"
#include "../libjpeg-turbo-1.2.90/jinclude.h"

//#pragma comment(lib, "libjpeg-turbo/lib/jpeg-static.lib")
//#include "./libjpeg-turbo/include/jpeglib.h"
#include <tchar.h>
#include "../../inc/PerfDetector.h"

#include <assert.h>
//全局变量
/* This struct contains the JPEG decompression parameters and pointers to
* working space (which is allocated as needed by the JPEG library).
*/
//struct jpeg_decompress_struct g_cinfo;

//const int RAW_LINES = 8;
//unsigned char** g_raw_ata[3];/* Output row buffer */
//unsigned char*  g_YRowPtr[RAW_LINES];
//unsigned char*  g_URowPtr[RAW_LINES];
//unsigned char*  g_VRowPtr[RAW_LINES];
//
//
////
//int    g_nImageWidth  = 640;
//int    g_nImageHeight = 480;
//
//unsigned char* g_YBuf = NULL;
//unsigned char* g_UBuf = NULL;
//unsigned char* g_VBuf = NULL;
//
//
//unsigned char*  g_YUY2Buf      = NULL;


//@功能:利用MMX指令和SSE2指令将YUV420的Y, U, V平面数据组合成YUY2
//
void _declspec(naked) YUV420ToYUY2_MMX(
					const unsigned char* Y,
					const unsigned char* U,
					const unsigned char* V,
					unsigned char* YUY2,
					unsigned int pixelCount)
{

	_asm{

		pushad
		//reset the floating-point unit
		finit

		mov esi, [esp + 32 + 4 ] ;pointer to Y plane data
		mov edi, [esp + 32 + 16] ;pointer to YUY2 destination data
		mov ebp, [esp + 32 + 8 ] ;pointer to U plane data
		mov edx, [esp + 32 + 12] ;pointer to V plane data
		mov ecx, [esp + 32 + 20] ;pixel count


next_8_pixel:
		//First four pixels
		//mm0 = 00 00 00 00 Y3 Y2 Y1 Y0
		movd mm0,[esi];//MMX
		add esi, 4;//


		//mm1=00 00 00 00 U3 U2 U1 U0
		movd mm1, [ebp]
		add ebp,4
		//pinsrw mm1, eax, 0

		//mm2=00 00 00 00 V3 V2 V1 V0
		movd mm2, [edx];//MMX
		add edx,4


		//mm1=V3 U3 V2 U2 V1 U1 V0 U0
		punpcklbw mm1,mm2;//MMX

		//mm0=V1 Y3 U1 Y2 V0 Y1 U0 Y0
		punpcklbw  mm0,mm1

		movq [edi],mm0

		add edi,4*2;//four pixels, each pixel is two bytes


		//Next four pixels
		//mm0 = 00 00 00 00 Y7 Y6 Y5 Y4
		movd mm0, [esi]
		add esi, 4;//

		//mm0 =Y7 Y6 Y5 Y4 00 00 00 00
		psllq mm0, 32;//MMX

		//mm1 = V3  Y7 U3 Y6  V2 Y5 U2 Y4
		punpckhbw mm0,mm1

		movq[edi], mm0
		add edi,4*2;//four pixels;


		sub ecx,8;//eigth pixels are ok

		jnz next_8_pixel;

		//Empty the multimedia state.
		emms

		popad

		ret

	}
}

//@功能:利用SSE2指令将YUV420的Y, U, V平面数据组合成YUY2图片帧
//@参数:pY, 指向Y平面数据的指针
//      pU, 指向U平面数据的指针
//      pV, 指向V平面数据的指针
//      pYUY2，输出数据, 指向YUY2图片帧的数据指针
//      pixelCount, 像素个数
void _declspec(naked) YUV420ToYUY2_SSE2(
					const unsigned char* pY,
					const unsigned char* pU,
					const unsigned char* pV,
					unsigned char* pYUY2,
					unsigned int pixelCount)
{

	_asm{

		pushad
		//reset the floating-point unit
		finit

		mov esi, [esp + 32 + 4 ] ;pointer to Y plane data
		mov ebp, [esp + 32 + 8 ] ;pointer to U plane data
		mov edx, [esp + 32 + 12] ;pointer to V plane data
		mov edi, [esp + 32 + 16] ;pointer to YUY2 destination data
		mov ecx, [esp + 32 + 20] ;pixel count

next_16_pixel:
		//First eight pixels
		//xmm0 = 00 00 00 00 00 00 00 00 Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
		//movapd xmm0,[esi];//SSE2
		movsd xmm0,mmword ptr [esi];//SSE2
		add esi, 8;//

		//xmm1= 00 00 00 00 00 00 00 00 U7 U6 U5 U4 U3 U2 U1 U0
		movsd xmm1, [ebp]
		add ebp,8
		

		//xmm2= 00 00 00 00 00 00 00 00 V7 V6 V5 V4 V3 V2 V1 V0
		movsd xmm2, [edx];//SSE2
		add edx,8


		//xmm1=V7 U7 V6 U6 V5 U5 V4 U4 V3 U3 V2 U2 V1 U1 V0 U0
		punpcklbw xmm1, xmm2;//SSE2

		//xmm0=V3 Y7 U3 Y6 V2 Y5 U2 Y4 V1 Y3 U1 Y2 V0 Y1 U0 Y0
		punpcklbw  xmm0, xmm1

		movapd [edi], xmm0

		add edi,8*2;//eight pixels, each pixel is two bytes


		//Next eight pixels
		//xmm0 = 00 00 00 00 00 00 00 00 Y15 Y14 Y13 Y12 Y11 Y10 Y9 Y8
		//movapd xmm0, [esi]
		movsd xmm0, mmword ptr [esi]
		add esi, 8;//

		//xmm0 = Y15 Y14 Y13 Y12 Y11 Y10 Y9 Y8 00 00 00 00 00 00 00 00
		pslldq xmm0, 8;//SSE2

		//xmm1 = V7 Y15 U7 Y14 V6 Y13 U6 Y12 V5 Y11 U5 Y10 V4 Y9 U4 Y8
		punpckhbw xmm0,xmm1

		movapd [edi], xmm0
		add edi,8*2;//eight pixels;


		sub ecx,16;//sixteen pixels are ok

		jnz next_16_pixel;

		//Empty the multimedia state.
		emms

		popad

		ret

	}
}

/*
* ERROR HANDLING:
*
* The JPEG library's standard error handler (jerror.c) is divided into
* several "methods" which you can override individually.  This lets you
* adjust the behavior without duplicating a lot of code, which you might
* have to update with each future release.
*
* Our example here shows how to override the "error_exit" method so that
* control is returned to the library's caller when a fatal error occurs,
* rather than calling exit() as the standard error_exit method does.
*
* We use C's setjmp/longjmp facility to return control.  This means that the
* routine which calls the JPEG library must first execute a setjmp() call to
* establish the return point.  We want the replacement error_exit to do a
* longjmp().  But we need to make the setjmp buffer accessible to the
* error_exit routine.  To do this, we make a private extension of the
* standard JPEG error handler object.  (If we were using C++, we'd say we
* were making a subclass of the regular error handler.)
*
* Here's the extended error handler struct:
*/

struct my_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */

	jmp_buf setjmp_buffer;	/* for return to caller */
};


/* We use our private extension JPEG error handler.
* Note that this struct must live as long as the main JPEG parameter
* struct, to avoid dangling-pointer problems.
*/
//struct my_error_mgr g_jerr;


typedef struct my_error_mgr * my_error_ptr;

/*
* Here's the routine that will replace the standard error_exit method:
*/

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	/* Always display the message. */
	/* We could postpone this until after returning, if we chose. */
	(*cinfo->err->output_message) (cinfo);

	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}



//class CMJPG_Decoder

void CMJPG_Decoder::Init()
{
	/* Step 1: allocate and initialize JPEG decompression object */

	/* We set up the normal JPEG error routines, then override error_exit. */
    m_cinfo = (jpeg_decompress_struct*)malloc(sizeof(jpeg_decompress_struct));
    m_jerr  = (my_error_mgr*)malloc(sizeof(my_error_mgr));

	m_cinfo->err = jpeg_std_error(&m_jerr->pub);

	m_cinfo->err->trace_level = 3;

	/* Now we can initialize the JPEG decompression object. */
	//jpeg_create_decompress(&g_cinfo);

    jpeg_CreateDecompress(m_cinfo, JPEG_LIB_VERSION, (size_t) sizeof(struct jpeg_decompress_struct));

	m_jerr->pub.error_exit = my_error_exit;


	//m_nRowSize = m_nImageWidth * 2;
    //m_YUY2Buf = (unsigned char*)malloc(m_nRowSize * m_nImageHeight );
	//memset(m_YUY2Buf, 0, m_nRowSize * m_nImageHeight);
    m_nRowSize = m_nImageWidth;
    m_GrayBuf = (unsigned char*)malloc(m_nRowSize * m_nImageHeight );
    memset(m_GrayBuf, 0, m_nRowSize * m_nImageHeight);


	m_YBuf = (unsigned char*)malloc(m_nImageWidth  * RAW_LINES );
	m_UBuf = (unsigned char*)malloc(m_nImageWidth * RAW_LINES );
	m_VBuf = (unsigned char*)malloc(m_nImageWidth * RAW_LINES );

	m_raw_data[0]  = &m_YRowPtr[0];
	m_raw_data[1]  = &m_URowPtr[0];
	m_raw_data[2]  = &m_VRowPtr[0];

	unsigned char* pYBuf = m_YBuf;
	for(int i=0; i<RAW_LINES; i++)
	{
		m_YRowPtr[i] = pYBuf;
		pYBuf += m_nImageWidth;
	}

	unsigned char* pUBuf = m_UBuf;
	for(int i=0; i<RAW_LINES; i++)
	{
		m_URowPtr[i] = pUBuf;
		pUBuf += m_nImageWidth/2;
	}

	unsigned char* pVBuf = m_VBuf;
	for(int i=0; i<RAW_LINES; i++)
	{
		m_VRowPtr[i] = pVBuf;
		pVBuf += m_nImageWidth/2;
	}


}

#define FREEBUF(buf)\
	if(buf){free(buf); buf=NULL;}


void CMJPG_Decoder::Uninit()
{
	/*Release JPEG decompression object */
	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(m_cinfo);


	FREEBUF(m_GrayBuf);
	FREEBUF(m_YBuf);
	FREEBUF(m_UBuf);
	FREEBUF(m_VBuf);
    FREEBUF(m_jerr);
    FREEBUF(m_cinfo);

}

//@功能:输入MJPG数据,并进行解码处理
//参数 :lpMJPGData,指向MJPG数据的指针。
//      nLength, MJPG数据长度。
//      pYUY2Data, 输出参数, 保存YUY2数据的缓冲区。
//      pEffectiveDataLength, 输出参数, 保存实际有效数据长度的变量地址。
//@说明:lpMJPGData,应该指向一个完整的MJPG帧
int CMJPG_Decoder::ProcessData(const BYTE* lpMJPGData,  int nLength, BYTE* pGrayData, int* pEffectiveDataLength)
{
	bool bRedo = false;
	do
	{
		bRedo = false;
		m_cinfo->global_state = 200;
		jpeg_mem_src(m_cinfo, const_cast<unsigned char*>(lpMJPGData), nLength);

		/* Start decompressor */
		static bool s_bFirstTime = true;
		if (setjmp(m_jerr->setjmp_buffer))
		{
			//出错了
			return FALSE;
		}

		(void)jpeg_read_header(m_cinfo, TRUE);

		m_cinfo->raw_data_out = true;

		(void)jpeg_start_decompress(m_cinfo);

		if (m_cinfo->image_height > (unsigned int)m_nImageHeight
			||
			m_cinfo->image_width > (unsigned int)m_nImageWidth
			)
		{
			m_nImageHeight = m_cinfo->image_height;
			m_nImageWidth = m_cinfo->image_width;
			Uninit();
			Init();
			bRedo = true;
		}
	} while (bRedo);


    //unsigned char* pYUY2 = pGrayData;
    //unsigned char* pGrayData = pGrayData;

    int nPixelCountEachScan= RAW_LINES *  m_cinfo->output_width;

    while (m_cinfo->output_scanline < m_cinfo->output_height)
    {
        JDIMENSION rows = jpeg_read_raw_data(m_cinfo, &m_raw_data[0], RAW_LINES);
        assert(rows == RAW_LINES);

//        unsigned char* pY = m_YBuf;
//        unsigned char* pU = m_UBuf;
//        unsigned char* pV = m_VBuf;

//        YUV420ToYUY2_SSE2(pY, pU, pV, pGrayData, nPixelCountEachScan);
        //memcpy(pGrayData, pY, nPixelCountEachScan);

//        pYUY2 += nPixelCountEachScan<<1;
       // pGrayData += nPixelCountEachScan;

		int  length = m_cinfo->output_width;
		for (unsigned int r = 0; r < rows; r++)
		{ 
			memcpy(pGrayData, m_raw_data[0][r], length);
			pGrayData += length;
		}

        //for(unsigned int row=0; row < RAW_LINES; row++)
        //{
        //	for(unsigned int col=0; col< g_cinfo.output_width; col += 2)
        //	{
        //		unsigned char Y1 = *pY++;
        //		unsigned char Y2 = *pY++;
        //		unsigned char U  = *pU++;
        //		unsigned char V  = *pV++;
        //		*pYUY2 =  V<< 24 | Y2<<16  | U<<8 |Y1;
        //		pYUY2 += ;
        //	}
        //}

    }//while

    if(pEffectiveDataLength)
    {
        *pEffectiveDataLength = m_cinfo->src->next_input_byte - lpMJPGData;
    }
    jpeg_finish_decompress(m_cinfo);

    return 0;

}

//const BYTE* CMJPG_Decoder::GetYUVData() const
//{
//	return m_YUY2Buf;
//}


const BYTE* CMJPG_Decoder::GetGrayData() const
{
    return m_GrayBuf;
}

//@功 能:将MJPG流转化为YUY2视频流
//@参 数:pMJGPStream,输入参数， 指向MJPG视频帧的内存指针
//       nStreamSize,输入参数， 视频流尺寸
//       pYUY2Buf, 输出参数，指向保存YUY2数据的缓冲区的尺寸
//       nBufSize, YUY2视频缓冲区地址。
//@返回值:0，无错误
//        其他, 错误代码
int MJPG2YUV2(const BYTE* pMJGPStream, int nStreamSize, BYTE* pYUY2Buf, int nBufSize)
{

	return 0;
}


