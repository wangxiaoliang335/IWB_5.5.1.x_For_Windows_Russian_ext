// MJPG_Encoder.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <atlstr.h>
#include "../../inc/MyApI.h"
#include "../../inc/MJPG/MJPG_Encoder.h"

//避免在"jmorecfg.h"中的"typedef long INT32" 导致的INT32重定义
#define XMD_H 

//仿照libjpeg-turbo-1.2.90\cjpeg.c
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"		/* get library error codes too */
#include "config.h"

typedef struct cjpeg_source_struct * cjpeg_source_ptr;
/* Create the add-on message string table. */
#define JMESSAGE(code,string)	string ,
static const char * const cdjpeg_message_table[] = {
#include "cderror.h"
  NULL
};

//仿照libjpeg-turbo-1.2.90\rdbmp.c
/* Private version of data source object */
#include "cdjpeg.h"		/* Common decls for cjpeg/djpeg applications */
typedef struct _dib_source_struct * dib_source_ptr;

typedef struct _dib_source_struct {
  struct cjpeg_source_struct pub; /* public fields */

  j_compress_ptr cinfo;		/* back link saves passing separate parm */

  JSAMPARRAY colormap;		/* BMP colormap (converted to my format) */

  jvirt_sarray_ptr whole_image;	/* Needed to reverse row order */
  JDIMENSION source_row;	    /* Current source row number */
  JDIMENSION row_width;	     	/* Physical width of scanlines in file */

  int bits_per_pixel;		   /* remembers 8- or 24-bit format */

  //added by toxuke@gmail.com, 2014/12/04
  //<<
   BITMAPINFOHEADER* dib    ;//POINTER to dib header
   //>>
} dib_source_struct;


//LOCAL(int)
//read_byte (bmp_source_ptr sinfo)
///* Read next byte from BMP file */
//{
//  register FILE *infile = sinfo->pub.input_file;
//  register int c;
//
//  if ((c = getc(infile)) == EOF)
//    ERREXIT(sinfo->cinfo, JERR_INPUT_EOF);
//  return c;
//}


//LOCAL(void)
//read_colormap (bmp_source_ptr sinfo, int cmaplen, int mapentrysize)
///* Read the colormap from a BMP file */
//{
//  int i;
//
//  switch (mapentrysize) {
//  case 3:
//    /* BGR format (occurs in OS/2 files) */
//    for (i = 0; i < cmaplen; i++) {
//      sinfo->colormap[2][i] = (JSAMPLE) read_byte(sinfo);
//      sinfo->colormap[1][i] = (JSAMPLE) read_byte(sinfo);
//      sinfo->colormap[0][i] = (JSAMPLE) read_byte(sinfo);
//    }
//    break;
//  case 4:
//    /* BGR0 format (occurs in MS Windows files) */
//    for (i = 0; i < cmaplen; i++) {
//      sinfo->colormap[2][i] = (JSAMPLE) read_byte(sinfo);
//      sinfo->colormap[1][i] = (JSAMPLE) read_byte(sinfo);
//      sinfo->colormap[0][i] = (JSAMPLE) read_byte(sinfo);
//      (void) read_byte(sinfo);
//    }
//    break;
//  default:
//    ERREXIT(sinfo->cinfo, JERR_BMP_BADCMAP);
//    break;
//  }
//}


/*
 * Read one row of pixels.
 * The image has been read into the whole_image array, but is otherwise
 * unprocessed.  We must read it out in top-to-bottom row order, and if
 * it is an 8-bit image, we must expand colormapped pixels to 24bit format.
 */

METHODDEF(JDIMENSION)
get_8bit_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading 8-bit colormap indexes */
{
  //bmp_source_ptr source = (bmp_source_ptr) sinfo;  
  dib_source_ptr source = (dib_source_ptr) sinfo;
  register JSAMPARRAY colormap = source->colormap;
  JSAMPARRAY image_ptr;
  register int t;
  register JSAMPROW inptr, outptr;
  register JDIMENSION col;

  /* Fetch next row from virtual array */
  source->source_row--;
  image_ptr = (*cinfo->mem->access_virt_sarray)
    ((j_common_ptr) cinfo, source->whole_image,
     source->source_row, (JDIMENSION) 1, FALSE);

  /* Expand the colormap indexes to real data */
  inptr = image_ptr[0];
  outptr = source->pub.buffer[0];
  for (col = cinfo->image_width; col > 0; col--) {
    t = GETJSAMPLE(*inptr++);
    *outptr++ = colormap[0][t];	/* can omit GETJSAMPLE() safely */
    *outptr++ = colormap[1][t];
    *outptr++ = colormap[2][t];
  }

  return 1;
}


METHODDEF(JDIMENSION)
get_24bit_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading 24-bit pixels */
{
  //bmp_source_ptr source = (bmp_source_ptr) sinfo;
  dib_source_ptr source = (dib_source_ptr) sinfo;
  JSAMPARRAY image_ptr;
  register JSAMPROW inptr, outptr;
  register JDIMENSION col;

  /* Fetch next row from virtual array */
  source->source_row--;
  image_ptr = (*cinfo->mem->access_virt_sarray)
    ((j_common_ptr) cinfo, source->whole_image,
     source->source_row, (JDIMENSION) 1, FALSE);

  /* Transfer data.  Note source values are in BGR order
   * (even though Microsoft's own documents say the opposite).
   */
  inptr = image_ptr[0];
  outptr = source->pub.buffer[0];
  for (col = cinfo->image_width; col > 0; col--) {
    outptr[2] = *inptr++;	/* can omit GETJSAMPLE() safely */
    outptr[1] = *inptr++;
    outptr[0] = *inptr++;
    outptr += 3;
  }

  return 1;
}


METHODDEF(JDIMENSION)
get_32bit_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading 32-bit pixels */
{
  //bmp_source_ptr source = (bmp_source_ptr) sinfo;
  dib_source_ptr source = (dib_source_ptr) sinfo;
  JSAMPARRAY image_ptr;
  register JSAMPROW inptr, outptr;
  register JDIMENSION col;

  /* Fetch next row from virtual array */
  source->source_row--;
  image_ptr = (*cinfo->mem->access_virt_sarray)
    ((j_common_ptr) cinfo, source->whole_image,
     source->source_row, (JDIMENSION) 1, FALSE);
  /* Transfer data.  Note source values are in BGR order
   * (even though Microsoft's own documents say the opposite).
   */
  inptr = image_ptr[0];
  outptr = source->pub.buffer[0];
  for (col = cinfo->image_width; col > 0; col--) {
    outptr[2] = *inptr++;	/* can omit GETJSAMPLE() safely */
    outptr[1] = *inptr++;
    outptr[0] = *inptr++;
    inptr++;			/* skip the 4th byte (Alpha channel) */
    outptr += 3;
  }

  return 1;
}


/*
 * This method loads the image into whole_image during the first call on
 * get_pixel_rows.  The get_pixel_rows pointer is then adjusted to call
 * get_8bit_row, get_24bit_row, or get_32bit_row on subsequent calls.
 */

METHODDEF(JDIMENSION)
preload_image (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
{
  //bmp_source_ptr source = (bmp_source_ptr) sinfo;
  dib_source_ptr source = (dib_source_ptr) sinfo;
  //register FILE *infile = source->pub.input_file;
  //<<added by toxuke@gmail.com, 2014/12/05
  JSAMPLE* pBmpBits = 0;
  DWORD dwNumColors,dwColorTableSize;

  dwNumColors = source->dib->biClrUsed;
  
  if(dwNumColors == 0)
  	if(source->dib->biBitCount <= 8)    
           dwNumColors = 1L << source->dib->biBitCount;  

	dwColorTableSize = dwNumColors*sizeof(RGBQUAD);
  
  pBmpBits = (JSAMPLE*)source->dib + source->dib->biSize + dwColorTableSize;
  
  register JSAMPROW out_ptr;
  JSAMPARRAY image_ptr;
  JDIMENSION row;
  cd_progress_ptr progress = (cd_progress_ptr) cinfo->progress;

  /* Read the data into a virtual array in input-file row order. */
 //将位图数据读入内存
 //每次读入一行像素数据
  for (row = 0; row < cinfo->image_height; row++) {
    if (progress != NULL) {
      progress->pub.pass_counter = (long) row;
      progress->pub.pass_limit = (long) cinfo->image_height;
      (*progress->pub.progress_monitor) ((j_common_ptr) cinfo);
    }

    image_ptr = (*cinfo->mem->access_virt_sarray)
      ((j_common_ptr) cinfo, source->whole_image,
       row, (JDIMENSION) 1, TRUE);

    out_ptr = image_ptr[0];
    
     memcpy(out_ptr,  pBmpBits ,source->row_width);
    //if (fread(out_ptr, 1, source->row_width, infile) != source->row_width) {
    //  if (feof(infile))
    //    ERREXIT(cinfo, JERR_INPUT_EOF);
    //  else
    //    ERREXIT(cinfo, JERR_FILE_READ);

    pBmpBits += source->row_width;
    
  }
  if (progress != NULL)
    progress->completed_extra_passes++;

  /* Set up to read from the virtual array in top-to-bottom order */
  switch (source->bits_per_pixel) {
  case 8:
    source->pub.get_pixel_rows = get_8bit_row;
    break;
  case 24:
    source->pub.get_pixel_rows = get_24bit_row;
    break;
  case 32:
    source->pub.get_pixel_rows = get_32bit_row;
    break;
  default:
    ERREXIT(cinfo, JERR_BMP_BADDEPTH);
  }
  source->source_row = cinfo->image_height;//从最后一行开始读取

  /* And read the first row */
  return (*source->pub.get_pixel_rows) (cinfo, sinfo);
}



/*
 * Read the file header; return image size and component count.
 */
METHODDEF(void)
start_input_dib(j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
{

  dib_source_ptr source = (dib_source_ptr) sinfo;
  BITMAPINFOHEADER* pBIH = source->dib;

  //U_CHAR bmpfileheader[14];
  //U_CHAR bmpinfoheader[64];

//#define GET_2B(array,offset)  ((unsigned int) UCH(array[offset]) + \
//			       (((unsigned int) UCH(array[offset+1])) << 8))
//#define GET_4B(array,offset)  ((INT32) UCH(array[offset]) + \
//			       (((INT32) UCH(array[offset+1])) << 8) + \
//			       (((INT32) UCH(array[offset+2])) << 16) + \
//			       (((INT32) UCH(array[offset+3])) << 24))
  //INT32 bfOffBits;
  //INT32 headerSize;
  //INT32 biWidth;
  //INT32 biHeight;
  //unsigned int biPlanes;
  //INT32 biCompression;
  //INT32 biXPelsPerMeter,biYPelsPerMeter;
  //INT32 biClrUsed = 0;
  //int mapentrysize = 0;		/* 0 indicates no colormap */
  //INT32 bPad;
  JDIMENSION row_width;

  //dib数据不包含BITMAPFILEHEADER
  //而且已经知道是dib数据, 无需判断判断是否是Bitmap格式
  //
  /* Read and verify the bitmap file header */
  //if (! ReadOK(source->pub.input_file, bmpfileheader, 14))
  //  ERREXIT(cinfo, JERR_INPUT_EOF);
  //if (GET_2B(bmpfileheader,0) != 0x4D42) /* 'BM' */
  //  ERREXIT(cinfo, JERR_BMP_NOT);
  //bfOffBits = (INT32) GET_4B(bmpfileheader,10);
  /* We ignore the remaining fileheader fields */

  /* The infoheader might be 12 bytes (OS/2 1.x), 40 bytes (Windows),
   * or 64 bytes (OS/2 2.x).  Check the first 4 bytes to find out which.
   */
  
  //if (! ReadOK(source->pub.input_file, bmpinfoheader, 4))
  //  ERREXIT(cinfo, JERR_INPUT_EOF);
  //headerSize = (INT32) GET_4B(bmpinfoheader,0);
  //if (headerSize < 12 || headerSize > 64)
  //  ERREXIT(cinfo, JERR_BMP_BADHEADER);
  //if (! ReadOK(source->pub.input_file, bmpinfoheader+4, headerSize-4))
  //  ERREXIT(cinfo, JERR_INPUT_EOF);

   //headerSize = sizeof(BITMAPINFOHEADER);
   
  //switch ((int) headerSize) {
  //case 12:
  //  /* Decode OS/2 1.x header (Microsoft calls this a BITMAPCOREHEADER) */
  //  biWidth = (INT32) GET_2B(bmpinfoheader,4);
  //  biHeight = (INT32) GET_2B(bmpinfoheader,6);
  //  biPlanes = GET_2B(bmpinfoheader,8);
  //  source->bits_per_pixel = (int) GET_2B(bmpinfoheader,10);

  //  switch (source->bits_per_pixel) {
  //  case 8:			/* colormapped image */
  //    mapentrysize = 3;		/* OS/2 uses RGBTRIPLE colormap */
  //    TRACEMS2(cinfo, 1, JTRC_BMP_OS2_MAPPED, (int) biWidth, (int) biHeight);
  //    break;
  //  case 24:			/* RGB image */
  //    TRACEMS2(cinfo, 1, JTRC_BMP_OS2, (int) biWidth, (int) biHeight);
  //    break;
  //  default:
  //    ERREXIT(cinfo, JERR_BMP_BADDEPTH);
  //    break;
  //  }
  //  break;
  //case 40:
  //case 64:
    /* Decode Windows 3.x header (Microsoft calls this a BITMAPINFOHEADER) */
    /* or OS/2 2.x header, which has additional fields that we ignore */
    //biWidth = GET_4B(bmpinfoheader,4);
    //biHeight = GET_4B(bmpinfoheader,8);
    //biPlanes = GET_2B(bmpinfoheader,12);
    
    //source->bits_per_pixel = (int) GET_2B(bmpinfoheader,14);
    source->bits_per_pixel = pBIH->biBitCount;

    //biCompression = GET_4B(bmpinfoheader,16);
    //biXPelsPerMeter = GET_4B(bmpinfoheader,24);
    //biYPelsPerMeter = GET_4B(bmpinfoheader,28);
    //biClrUsed = GET_4B(bmpinfoheader,32);
    /* biSizeImage, biClrImportant fields are ignored */

    //switch (source->bits_per_pixel) {
    //case 8:			/* colormapped image */
    //  mapentrysize = 4;		/* Windows uses RGBQUAD colormap */
    //  TRACEMS2(cinfo, 1, JTRC_BMP_MAPPED, (int) biWidth, (int) biHeight);
    //  break;
    //case 24:			/* RGB image */
    //  TRACEMS2(cinfo, 1, JTRC_BMP, (int) biWidth, (int) biHeight);
    //  break;
    //case 32:			/* RGB image + Alpha channel */
    //  TRACEMS2(cinfo, 1, JTRC_BMP, (int) biWidth, (int) biHeight);
    //  break;
    //default:
    //  ERREXIT(cinfo, JERR_BMP_BADDEPTH);
    //  break;
    //}
    //if (biCompression != 0)
    if (pBIH->biCompression != 0)
      ERREXIT(cinfo, JERR_BMP_COMPRESSED);

    //if (biXPelsPerMeter > 0 && biYPelsPerMeter > 0) {
    if (pBIH->biXPelsPerMeter > 0 && pBIH->biYPelsPerMeter > 0) {
      /* Set JFIF density parameters from the BMP data */
      //cinfo->X_density = (UINT16) (biXPelsPerMeter/100); /* 100 cm per meter */
        cinfo->X_density = (UINT16) (pBIH->biXPelsPerMeter/100);/* 100 cm per meter */

      //cinfo->Y_density = (UINT16) (biYPelsPerMeter/100);
        cinfo->Y_density = (UINT16) (pBIH->biYPelsPerMeter/100);/* 100 cm per meter */
        cinfo->density_unit = 2;	/* dots/cm */
    }
  //break;
  //default:
  //  ERREXIT(cinfo, JERR_BMP_BADHEADER);
  //  return;
  //}

  //if (biWidth <= 0 || biHeight <= 0)
  if(pBIH->biWidth <= 0 || pBIH->biHeight <= 0 )
    ERREXIT(cinfo, JERR_BMP_EMPTY);
  //if (biPlanes != 1)
  if(pBIH->biPlanes != 1)
    ERREXIT(cinfo, JERR_BMP_BADPLANES);

  /* Compute distance to bitmap data --- will adjust for colormap below */
  //bPad = bfOffBits - (headerSize + 14);//sizeof(BITMAPFILEHEADER) equals 14, commented by toxuke@gmail.com, 2012/12/14

  /* Read the colormap, if any */
  //if (mapentrysize > 0) {
  //  if (biClrUsed <= 0)
  //    biClrUsed = 256;		/* assume it's 256 */
  //  else if (biClrUsed > 256)
  //    ERREXIT(cinfo, JERR_BMP_BADCMAP);
  //  /* Allocate space to store the colormap */
  //  source->colormap = (*cinfo->mem->alloc_sarray)
  //    ((j_common_ptr) cinfo, JPOOL_IMAGE,
  //     (JDIMENSION) biClrUsed, (JDIMENSION) 3);
  //  /* and read it from the file */
  //  read_colormap(source, (int) biClrUsed, mapentrysize);
  //  /* account for size of colormap */
  //  bPad -= biClrUsed * mapentrysize;
  //}

  ///* Skip any remaining pad bytes */
  //if (bPad < 0)			/* incorrect bfOffBits value? */
  //  ERREXIT(cinfo, JERR_BMP_BADHEADER);
  //while (--bPad >= 0) {
  //  (void) read_byte(source);
  //}

  /* Compute row width in file, including padding to 4-byte boundary */
  if (source->bits_per_pixel == 24)
    //row_width = (JDIMENSION) (biWidth * 3);
    row_width = (JDIMENSION) (pBIH->biWidth * 3);
  else if (source->bits_per_pixel == 32)
    //row_width = (JDIMENSION) (biWidth * 4);
    row_width = (JDIMENSION)(pBIH->biWidth * 4);
  else
    //row_width = (JDIMENSION) biWidth;
    row_width = (JDIMENSION)pBIH->biWidth;

  //每行Bitmap数据的字节宽度是4的整数倍。
  while ((row_width & 3) != 0) row_width++;
  source->row_width = row_width;

  /* Allocate space for inversion array, prepare for preload pass */
  source->whole_image = (*cinfo->mem->request_virt_sarray)
    ((j_common_ptr) cinfo, JPOOL_IMAGE, FALSE,
     //row_width, (JDIMENSION) biHeight, (JDIMENSION) 1);
     row_width, (JDIMENSION) pBIH->biHeight, (JDIMENSION) 1);
  
  source->pub.get_pixel_rows = preload_image;

  if (cinfo->progress != NULL) {
    cd_progress_ptr progress = (cd_progress_ptr) cinfo->progress;
    progress->total_extra_passes++; /* count file input as separate pass */
  }

  /* Allocate one-row buffer for returned data */
  //并不实际分配,实际分配由realize_virt_arrays实现
  //在早jpeg_start_compress
  source->pub.buffer = (*cinfo->mem->alloc_sarray)
    ((j_common_ptr) cinfo, JPOOL_IMAGE,
     //(JDIMENSION) (biWidth * 3), (JDIMENSION) 1);
     (JDIMENSION) (pBIH->biWidth * 3), (JDIMENSION) 1);
  source->pub.buffer_height = 1;

  cinfo->in_color_space = JCS_RGB;
  cinfo->input_components = 3;
  cinfo->data_precision = 8;
  //cinfo->image_width = (JDIMENSION) biWidth;
  cinfo->image_width = (JDIMENSION) pBIH->biWidth;

  //cinfo->image_height = (JDIMENSION) biHeight;
  cinfo->image_height = (JDIMENSION) pBIH->biHeight;

}


/*
 * Finish up at the end of the file.
 */

METHODDEF(void)
finish_input_dib (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
{
  /* no work */
}




/*
 * The module selection routine for DIB format input.
 */

GLOBAL(cjpeg_source_ptr)
jinit_read_dib (j_compress_ptr cinfo)
{
  dib_source_ptr source;

  /* Create module interface object */
  source = (dib_source_ptr)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  SIZEOF(dib_source_struct));
  source->cinfo = cinfo;	/* make back link for subroutines */
  /* Fill in method ptrs, except get_pixel_rows which start_input sets */
  //get_pixel_rows was set in function preload_image
  source->pub.start_input  = start_input_dib;
  source->pub.finish_input = finish_input_dib;

  return (cjpeg_source_ptr) source;
}



//@功  能:将DIB保存为,JPEG文件
//@参  数:pBIH, 指向DIB头的数据,后接调色板和像素数据
//        szJPEGFileName, JPEG文件路径名称。
//@返回值:成功, TRUE
//        失败, FALSE
MJPG_ENCODER_API BOOL Dib2JPEG(const BITMAPINFOHEADER* pBIH, LPCTSTR lpszJPEGFileName)
{

    //JPEG压缩信息
    struct jpeg_compress_struct cinfo;
    
    //JPEG错误管理器
    struct jpeg_error_mgr jerr;

    //数据源管理器
    cjpeg_source_ptr src_mgr;

    //输出文件句柄
    FILE * output_file = NULL;

    //每次位图中读取的图像数据的行数
    JDIMENSION num_scanlines; 

    CAtlString absPath = CheckAndCreatePath(lpszJPEGFileName);

    //打开输出文件
    //注意:一定要明指定义二进制形式打开文件, 否则缺省以文本格式打开，后果是API会自动将0x0A变为0x0D 0x0A插入文件，
    //导致JPEG格式不正确
    //此处Bug调试花费了两天宝贵的时间，教训深刻，切记!
    errno_t err = _wfopen_s(&output_file,CT2W(absPath), L"wb");

    if(0 != err)
    {
        return FALSE;
    }

   /* Initialize the JPEG compression object with default error handling. */
   cinfo.err = jpeg_std_error(&jerr);

  jpeg_create_compress(&cinfo);
  
  /* Add some application-specific error messages (from cderror.h) */
  jerr.addon_message_table = cdjpeg_message_table;
  jerr.first_addon_message = JMSG_FIRSTADDONCODE;
  jerr.last_addon_message  = JMSG_LASTADDONCODE;

  
  /* Initialize JPEG parameters.
   * Much of this may be overridden later.
   * In particular, we don't yet know the input file's color space,
   * but we need to provide some value for jpeg_set_defaults() to work.
   */
  cinfo.in_color_space = JCS_RGB; /* arbitrary guess */
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, 100, TRUE);


  /* Figure out the input file format, and set up to read it. */
  src_mgr = jinit_read_dib(&cinfo);

  ((dib_source_ptr)src_mgr)->dib = const_cast<BITMAPINFOHEADER*>(pBIH);



  /* Read the input file header to obtain file size & colorspace. */
  (*src_mgr->start_input) (&cinfo, src_mgr);

  /* Now that we know input colorspace, fix colorspace-dependent defaults */
  jpeg_default_colorspace(&cinfo);



  jpeg_stdio_dest(&cinfo, output_file);



  /* Start compressor */
  jpeg_start_compress(&cinfo, TRUE);

 /* Process data */
  while (cinfo.next_scanline < cinfo.image_height) {
    num_scanlines = (*src_mgr->get_pixel_rows) (&cinfo, src_mgr);
    (void) jpeg_write_scanlines(&cinfo, src_mgr->buffer, num_scanlines);
  }


  /* Finish compression and release memory */
  jpeg_finish_compress(&cinfo);


  jpeg_destroy_compress(&cinfo);

  
   
   fclose(output_file);

   BOOL bRet = jerr.num_warnings ? FALSE : TRUE;

   return bRet;
}

