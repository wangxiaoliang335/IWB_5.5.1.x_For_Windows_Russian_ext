// Test_Morphology.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "./inc/Dib/Dib.h"
#include "./inc/ImageFrameAPI.h"
#include "./inc/ImageDib.h"

#include "../inc/MorphologyAlgo.h"
#include "../inc/RLEBinaryImage.h"

#define PERF_TEST(Cmd, TryTimes)\
{\
	 LARGE_INTEGER liBegin,liEnd1,liEnd2, liFreq;\
	 QueryPerformanceFrequency(&liFreq);\
	 QueryPerformanceCounter(&liBegin);\
	for(int i=0; i<TryTimes; i++)\
	{\
	}\
	QueryPerformanceCounter(&liEnd1);\
	for(int j=0; j<TryTimes; j++)\
	{\
        Cmd;\
	}\
    QueryPerformanceCounter(&liEnd2);\
	double avgTime = ((double)(liEnd2.QuadPart - liEnd1.QuadPart - (liEnd1.QuadPart - liBegin.QuadPart)) * 1000*1000*1000)/(double)liFreq.QuadPart/(double)TryTimes;\
	std::cout<<"\""<< #Cmd <<"\" costs " << avgTime << " ns."<<std::endl;\
}
int _tmain(int argc, _TCHAR* argv[])
{

	CDib dib;




    //²âÊÔRLE±àÂëÍ¼µÄÂß¼­'»ò'ÔËËã
    dib.Open(_T("../src/src1.bmp"));
    CImageFrame src1;
    Dib2Image(dib, src1);

    dib.Open(_T("../src/src2.bmp"));
    CImageFrame src2;
    Dib2Image(dib, src2);



    //======²âÊÔ'Óë'ÔËËã=======
   CRLEBinaryImage rleSrc1(src1.Width(), src1.Height(), src1.GetData());
   CRLEBinaryImage rleSrc2(src2.Width(), src2.Height(), src2.GetData());
   CRLEBinaryImage rleAndResult(src1.Width(), src1.Height());
   //²âÊÔRLE±àÂëÍ¼µÄÂß¼­'&'ÔËËã
   rleAndResult = rleSrc1 & rleSrc2;


     CImageFrame result;
    result.SetSize(src1.Width(), src1.Height(), 1);
    rleAndResult.RLE2Gray(result.GetData(), result.Size());
    SaveToDib(result, _T("../result/andResult01.bmp"));



    rleAndResult = rleSrc1;
     //²âÊÔRLE±àÂëÍ¼µÄÂß¼­'&='ÔËËã
    rleAndResult &= rleSrc2;
    rleAndResult.RLE2Gray(result.GetData(), result.Size());
    SaveToDib(result, _T("../result/andResult02.bmp"));



    //======²âÊÔ'»ò'ÔËËã=======
     CRLEBinaryImage rleOrResult(src1.Width(), src1.Height());
   //²âÊÔRLE±àÂëÍ¼µÄÂß¼­'|'ÔËËã
   rleOrResult = rleSrc1 | rleSrc2;

    rleOrResult.RLE2Gray(result.GetData(), result.Size());
    SaveToDib(result, _T("../result/orResult01.bmp"));



    rleOrResult = rleSrc1;
     //²âÊÔRLE±àÂëÍ¼µÄÂß¼­'|='ÔËËã
    rleOrResult |= rleSrc2;
    rleOrResult.RLE2Gray(result.GetData(), result.Size());
    SaveToDib(result, _T("../result/orResult02.bmp"));




    //======²âÊÔ'È¡·´²Ù×÷'===
    rleOrResult = rleSrc1;

    rleOrResult = ~rleOrResult;

    rleOrResult.RLE2Gray(result.GetData(), result.Size());
    SaveToDib(result, _T("../result/ComplementResult01.bmp"));


    //====="ÅòÕÍ²Ù×÷"===
    //ËÄÁªÍ¨ÅòÕÍ
    Morph_Dilate4(src1.GetData(), result.GetData(), result.Width(), result.Height());
    SaveToDib(result, _T("../result/4_DilateResult01.bmp"));


    //°ËÁªÍ¨ÅòÕÍ
    Morph_Dilate8(src1.GetData(), result.GetData(), result.Width(), result.Height());
    SaveToDib(result, _T("../result/8_DilateResult01.bmp"));


   //====="¸¯Ê´²Ù×÷"===
     //rleOrResult = rleSrc1;
     //rleOrResult.Erode();
     //rleOrResult.RLE2Gray(result.GetData(), result.Size());
     //SaveToDib(result, _T("../result/ErodeResult01.bmp"));
    //ËÄÁªÍ¨¸¯Ê´
    Morph_Erode4(src1.GetData(), result.GetData(), result.Width(), result.Height());
    SaveToDib(result, _T("../result/4_ErodeResult01.bmp"));

    //°ËÁªÍ¨¸¯Ê´ÕÍ
    Morph_Erode8(src1.GetData(), result.GetData(), result.Width(), result.Height());
    SaveToDib(result, _T("../result/8_ErodeResult01.bmp"));
   

     //²âÊÔ²¹¶´Ëã·¨
    dib.Open(_T("../src/FillHoleSrc01.bmp"));
	CImageFrame srcImage;
	Dib2Image(dib, srcImage);

    CImageFrame dstImage = srcImage;
    PERF_TEST(
    Morph_FillHole(
        srcImage.GetData(),
        dstImage.GetData(),
        dstImage.Width(),
        dstImage.Height()), 10);



	SaveToDib(dstImage, _T("../result/FillHoleResult01.bmp"));


    dib.Open(_T("../src/FillHoleSrc02.bmp"));
	Dib2Image(dib, srcImage);

   dstImage = srcImage;
    PERF_TEST(
    Morph_FillHole(
        srcImage.GetData(),
        dstImage.GetData(),
        dstImage.Width(),
        dstImage.Height()), 10);



	SaveToDib(dstImage, _T("../result/FillHoleResult02.bmp"));



    dib.Open(_T("../src/FillHoleSrc03.bmp"));
	Dib2Image(dib, srcImage);

   dstImage = srcImage;
    PERF_TEST(
    Morph_FillHole(
        srcImage.GetData(),
        dstImage.GetData(),
        dstImage.Width(),
        dstImage.Height()), 10);



	SaveToDib(dstImage, _T("../result/FillHoleResult03.bmp"));



    dib.Open(_T("../src/FillHoleSrc04.bmp"));
	Dib2Image(dib, srcImage);

   dstImage = srcImage;
    PERF_TEST(
    Morph_FillHole(
        srcImage.GetData(),
        dstImage.GetData(),
        dstImage.Width(),
        dstImage.Height()), 10);



	SaveToDib(dstImage, _T("../result/FillHoleResult04.bmp"));



    dib.Open(_T("../src/FillHoleSrc05.bmp"));
	Dib2Image(dib, srcImage);

   dstImage = srcImage;
    PERF_TEST(
    Morph_FillHole(
        srcImage.GetData(),
        dstImage.GetData(),
        dstImage.Width(),
        dstImage.Height()), 10);



	SaveToDib(dstImage, _T("../result/FillHoleResult05.bmp"));



    //=====±Õ²Ù×÷²âÊÔ=====
    dib.Open(_T("../src/FillHoleSrc04.bmp"));
	Dib2Image(dib, srcImage);

   dstImage = srcImage;

    Morph_Close(
        srcImage.GetData(),
        dstImage.GetData(),
        dstImage.Width(),
        dstImage.Height());


	SaveToDib(dstImage, _T("../result/Src04Close.bmp"));

    //=====¿ª²Ù×÷²âÊÔ=====
    dib.Open(_T("../src/FillHoleSrc04.bmp"));
	Dib2Image(dib, srcImage);

   dstImage = srcImage;

    Morph_Open(
        srcImage.GetData(),
        dstImage.GetData(),
        dstImage.Width(),
        dstImage.Height());


	SaveToDib(dstImage, _T("../result/Src04Open.bmp"));

    return 0;
}

