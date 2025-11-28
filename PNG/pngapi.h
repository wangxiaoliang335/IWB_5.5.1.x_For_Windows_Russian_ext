#ifndef _H_INCLUDE_PNGAPI
#define _H_INCLUDE_PNGAPI
struct ARGBColor
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};


#ifdef __cplusplus
extern "C"
{
#endif


#ifndef __cplusplus
	typedef int bool;
#define true  1
#define false 0
#endif

	//@function:
	//
	//bool  LoadPng(const char* szFileName, ARGBColor** ppARGBData, int* pWidth, int* pHeight);
	//bool  SavePng(const char* szFileName, cosnt ARGBColor* ppARGBData, int nWidth, int nHeight);

	bool   LoadPng(const char* fileName,      unsigned char ** pGrayData, int* pWidth,  int* pHeight);
	bool  SaveGrayAsPng (const char* fileName, const unsigned char * pGrayData, int  width,  int  height);
	
    bool  SaveRGBAAsPng (const char* fileName, const unsigned long * pRGBAData, int width, int height);
    bool  SaveBGRAAsPng (const char* fileName, const unsigned long*  pBGRAData, int width, int height);

#ifdef __cplusplus
}
#endif



#endif //ifndef _H_INCLUDE_PNGAPI
