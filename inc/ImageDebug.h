#pragma once
_declspec(selectany) CAtlString g_strImageDebugRoot = _T(".\\Intermediate");//自动校正调试输出根目录
_declspec(selectany) int g_nImageIndex = 1;


template<class T>
BOOL Debug_SaveImageFrame(const CImageFrameT<T>& frame, LPCTSTR lpszFileName)
{

    CAtlString strFullPath;
    CAtlString strImageFileName;

    strImageFileName.Format(_T("%03d_%s"), g_nImageIndex, lpszFileName);

    strFullPath =  g_strImageDebugRoot;
    strFullPath += _T("\\");
    strFullPath += strImageFileName;

    g_nImageIndex++;


    //return  SaveToBitmap(frame, strFullPath);
    return SaveToJPEG(frame, strFullPath);
};


inline BOOL Debug_SaveDib(CDib& dib, LPCTSTR lpszFileName)
{
    CAtlString strFullPath;
    CAtlString strImageFileName;

    strImageFileName.Format(_T("%03d_%s"), g_nImageIndex, lpszFileName);

    strFullPath =  g_strImageDebugRoot;
    strFullPath += _T("\\");
    strFullPath += strImageFileName;
    g_nImageIndex++;


    return dib.Save(strFullPath);
}

inline BOOL Debug_SaveDibToJPEG(CDib& dib, LPCTSTR lpszFileName)
{
    CAtlString strFullPath;
    CAtlString strImageFileName;

    strImageFileName.Format(_T("%03d_%s"), g_nImageIndex, lpszFileName);


    strFullPath =  g_strImageDebugRoot;
    strFullPath += _T("\\");
    strFullPath += strImageFileName;

    g_nImageIndex++;


    return Dib2JPEG(dib, strFullPath);
}



#include <png.h>
#include "../png/pngapi.h"
//@function  :save ARGB bitmap as png file
//@parameters:
//           fileName, png file name
//           pGrayData, the gray data of 8 bit bitmap
//           nWidth, the width of the bitmap
//           nHeight, the height of the bitmap
//@author: 15077726@qq.com
//         2015/08/04
inline bool  Debug_SaveDibToPNG(const CDib& dib, LPCTSTR lpszFileName)
{
    CAtlString strFullPath;
    CAtlString strImageFileName;

    strImageFileName.Format(_T("%03d_%s"), g_nImageIndex, lpszFileName);

    strFullPath =  g_strImageDebugRoot;
    strFullPath += _T("\\");
    strFullPath += strImageFileName;
    g_nImageIndex++;

    int width = dib.GetWidth();
    int height = dib.GetHeight();

    int bit_depth = 8;
    //Open the file
    FILE* fp;
    errno_t e = fopen_s(&fp, CT2CA(strFullPath), "wb");
    if(e)
        return false;


    //allocate and initialize png_struct and png_info
    png_structp png_ptr = png_create_write_struct
        (PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
        NULL, NULL);
    if(!png_ptr)
        return false;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr)
    {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        return false;
    }

    /* Set error handling.  REQUIRED if you aren't supplying your own
    * error handling functions in the png_create_write_struct() call.
    */
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        /* If we get here, we had a problem writing the file */
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return  false;
    }

    /* Set up the output control if you are using standard C streams */
    png_init_io(png_ptr, fp);


    /* Set the image information here.  Width and height are up to 2^31,
    * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
    * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
    * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
    * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
    * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
    * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
    */
    png_set_IHDR(
        png_ptr,
        info_ptr,
        width,
        height,
        bit_depth,
        PNG_COLOR_TYPE_RGB,//RGB //PNG_COLOR_TYPE_RGB_ALPHA ,//ARGB
        PNG_INTERLACE_NONE       ,
        PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE      );



    //Write the file header information
    png_write_info(png_ptr, info_ptr);


    /* Write the bits */
    png_const_bytep pBGRData = (png_const_bytep)dib.GetBits() + (height - 1) * dib.GetLineStride();
    BYTE* pRGBData = new BYTE[dib.GetLineStride()];
    for(int row=0; row < height; row ++)
    {
        //dib像素格式(从低到高) B G R
        //png像素格式(从低到高) R G B
        //将BGR数据转为RGB数据
        for(int i = 0; i < width; i++)
        {
            int nOffset = i*3;
            pRGBData[nOffset    ] = pBGRData[nOffset + 2];
            pRGBData[nOffset + 1] = pBGRData[nOffset + 1];
            pRGBData[nOffset + 2] = pBGRData[nOffset    ];
        }

        //png_write_image(png_ptr, info_ptr->row_pointers);
        png_write_row(png_ptr, (png_const_bytep)pRGBData);

        pBGRData -= dib.GetLineStride();
    }

    delete[] pRGBData;

    /* It is REQUIRED to call this to finish writing the rest of the file */
    png_write_end(png_ptr, info_ptr);

    //Free all memory used by libpng
    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose(fp);

    return true;
}




inline bool  Debug_SaveRGBAToPNG(const unsigned long * pRGBAData, int width, int height,  LPCTSTR lpszFileName)
{
    CAtlString strFullPath;
    CAtlString strImageFileName;

    strImageFileName.Format(_T("%03d_%s"), g_nImageIndex, lpszFileName);

    strFullPath =  g_strImageDebugRoot;
    strFullPath += _T("\\");
    strFullPath += strImageFileName;
    g_nImageIndex++;

    return SaveRGBAAsPng(CT2A(strFullPath), pRGBAData,  width, height);
}



inline bool  Debug_SaveGrayToPNG(const unsigned char* pGrayData, int width, int height,  LPCTSTR lpszFileName)
{
    CAtlString strFullPath;
    CAtlString strImageFileName;

    strImageFileName.Format(_T("%03d_%s"), g_nImageIndex, lpszFileName);

    strFullPath =  g_strImageDebugRoot;
    strFullPath += _T("\\");
    strFullPath += strImageFileName;
    g_nImageIndex++;

    return SaveGrayAsPng(CT2A(strFullPath), pGrayData,  width, height);
}