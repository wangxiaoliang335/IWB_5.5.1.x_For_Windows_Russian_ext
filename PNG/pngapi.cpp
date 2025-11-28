#include "./png/png.h"
#include "pngapi.h"
#include  <stdlib.h>

#define ERROR 0

void user_error_fn(png_structp, png_const_charp)
{

}

void user_waring_fn(png_structp, png_const_charp)
{

}

//@function:Load png file as gray bit map
//@paraters:
//          fileName, png file name
//          pGrayData, the buffer to store the new allocated gray bits data, the data should be freed outside by user
//          pWidth,  buffer which stores the image width
//          pHeigth, buffer which stores the image height
bool  LoadPng(const char* fileName, unsigned char ** ppGrayData, int* pWidth, int* pHeight)
{
	//open the file to read
	FILE* fp = fopen(fileName, "rb");
	if(!fp)
		return false;

	//To check whether the file is a png file
	int number = 8;
	unsigned char header[8];
	if(fread(header, 1, number, fp) != number)
	{
		return false;
	}
	bool bIsPNG = !png_sig_cmp(header, 0, number);
	if(!bIsPNG)
	{
		return false;
	}

	//allocate and initialize png_struct and png_info
	png_structp png_ptr = png_create_read_struct(
		PNG_LIBPNG_VER_STRING,
		(png_voidp)NULL,//user_error_ptr
		NULL,//user_error_fn
		NULL //user_warning_fn
		);

	if(!png_ptr)
		return false;

	png_infop info_ptr = png_create_info_struct(png_ptr);

	if(!info_ptr)
	{
		png_destroy_read_struct(
			&png_ptr,        //png_ptr_ptr
			(png_infopp)NULL,//info_ptr_ptr
			(png_infopp)NULL //end_info_ptr_ptr
			);

		return false;

	}

	//setjmp
	if(setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(
			&png_ptr,
			&info_ptr,
			(png_infopp)NULL);
		fclose(fp);
		return false;

	}

	//set up the input
	png_init_io(png_ptr, fp);

	//let libpng know that there are some bytes missing for the start of the file
	png_set_sig_bytes(png_ptr, number);


	//process all chunks up to but not including the image data
	png_read_info(png_ptr, info_ptr);

	//Querying the info structure
	unsigned int width, height;
	int bit_depth, color_type, interlace_type, compression_type, filter_method;
	png_get_IHDR(
		png_ptr,
		info_ptr,
		&width,
		&height,
		&bit_depth,
		&color_type,
		&interlace_type,
		&compression_type,
		&filter_method);


	if(color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		png_set_rgb_to_gray(
		png_ptr,
		1,//1: silently do the conversion
		//2: issue a warning if the original image has any pixel where read!=green or red !=blue
		//3: issue an error and abort the conversion if the original image has any pixle where red!=green or red !=blue
		0.212671,//weight of red component
		0.715160 //weight of green component
		);

	//allocate memory to store
	*ppGrayData = (unsigned char*)malloc((width*height*bit_depth)>>3 );


	unsigned char* pRow = *ppGrayData;
	for(unsigned int row=0; row < height; row++)
	{
		png_read_row(png_ptr, pRow, NULL);
		pRow += width;
	}

	//Finishing a sequential read
	png_read_end(png_ptr, (png_infop)NULL);

	png_destroy_read_struct(
		&png_ptr,
		&info_ptr,
		(png_infopp)NULL);


	if(pWidth) *pWidth = width;
	if(pHeight) *pHeight = height;

	fclose(fp);

	return true;
}

//@function  :save gray bitmap as png file
//@parameters:
//           fileName, png file name
//           pGrayData, the gray data of 8 bit bitmap
//           nWidth, the width of the bitmap
//           nHeight, the height of the bitmap
//@author: 15077726@qq.com
//         2015/08/04
bool  SaveGrayAsPng(const char* fileName, const unsigned char * pGrayData, int width, int height)
{
	int bit_depth = 8;
	//Open the file
	FILE* fp = fopen(fileName, "w+b");
	if(!fp)
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
		PNG_COLOR_TYPE_GRAY      ,//�Ҷ�ͼƬ
		PNG_INTERLACE_NONE       ,
		PNG_COMPRESSION_TYPE_BASE,
		PNG_FILTER_TYPE_BASE      );



	//Write the file header information
	png_write_info(png_ptr, info_ptr);


	/* Write the bits */
	for(int row=0; row < height; row ++)
	{
		//png_write_image(png_ptr, info_ptr->row_pointers);
		png_write_row(png_ptr, pGrayData);
		pGrayData += width;
	}

	/* It is REQUIRED to call this to finish writing the rest of the file */
	png_write_end(png_ptr, info_ptr);

	//Free all memory used by libpng
	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(fp);

	return true;
}

//@function  :save ARGB bitmap as png file
//@parameters:
//           fileName, png file name
//           pGrayData, the gray data of 8 bit bitmap
//           nWidth, the width of the bitmap
//           nHeight, the height of the bitmap
//@author: 15077726@qq.com
//         2015/08/04
bool  SaveRGBAAsPng(const char* fileName, const unsigned long * pRGBAData, int width, int height)
{
	int bit_depth = 8;
	//Open the file
	FILE* fp = fopen(fileName, "wb");
	if(!fp)
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
		PNG_COLOR_TYPE_RGB_ALPHA ,//RGBA
		PNG_INTERLACE_NONE       ,
		PNG_COMPRESSION_TYPE_BASE,
		PNG_FILTER_TYPE_BASE      );



	//Write the file header information
	png_write_info(png_ptr, info_ptr);


	/* Write the bits */
	for(int row=0; row < height; row ++)
	{
		//png_write_image(png_ptr, info_ptr->row_pointers);
		png_write_row(png_ptr, (png_const_bytep)pRGBAData);
		pRGBAData += width;
	}

	/* It is REQUIRED to call this to finish writing the rest of the file */
	png_write_end(png_ptr, info_ptr);

	//Free all memory used by libpng
	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(fp);

	return true;
}



bool  SaveBGRAAsPng(const char* fileName, const unsigned long* pBGRAData, int width, int height)
{
	int bit_depth = 8;
	//Open the file
	FILE* fp = fopen(fileName, "wb");
	if(!fp)
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
        PNG_COLOR_TYPE_RGB_ALPHA ,//RGBA
        PNG_INTERLACE_NONE       ,
        PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE      );



    //Write the file header information
    png_write_info(png_ptr, info_ptr);


    /* Write the bits */
  
    unsigned long* pRGBAData = new  unsigned long[width];
    for(int row=0; row < height; row ++)
    {
        //dib像素格式(从低到高) B G R A
        //png像素格式(从低到高) R G B A
        //将BGR数据转为RGB数据
        for(int i = 0; i < width; i++)
        {
            unsigned char*       rgba =  reinterpret_cast<unsigned char*>(&pRGBAData[i]);
            const unsigned char* bgra =  reinterpret_cast<const unsigned char*>(&pBGRAData[i]);

            rgba[0] = bgra[2];
            rgba[1] = bgra[1];
            rgba[2] = bgra[0];
            rgba[3] = bgra[3];
        }

        //png_write_image(png_ptr, info_ptr->row_pointers);
        png_write_row(png_ptr, (png_const_bytep)pRGBAData);

        pBGRAData += width; 
    }

    delete[] pRGBAData;

    /* It is REQUIRED to call this to finish writing the rest of the file */
    png_write_end(png_ptr, info_ptr);

    //Free all memory used by libpng
    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose(fp);

    return true;
}