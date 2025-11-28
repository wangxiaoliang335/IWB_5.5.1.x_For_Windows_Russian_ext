// testDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../inc/MJPG/MJPG_Decoder.h"
#include "../../inc/MJPG/MJPG_Encoder.h"
#include "../../inc/DIB/DIB.h"

#include <fstream>
#include <iostream>
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
#ifdef _DEBUG
	#pragma comment(lib, "../lib/DEBUG/MJPG_Decoder.lib")
#else
	#pragma comment(lib, "../lib/Release/MJPG_Decoder.lib")
#endif
int _tmain(int argc, _TCHAR* argv[])
{
    /*
	CMJPG_Decoder decoder;


	std::ifstream file;
    std::string  strFileName = "test.jpg";
    file.open(strFileName.c_str(), std::ios_base::in | std::ios_base::binary);

    if(!file.is_open())
    {
        std::cout<<"open file " << strFileName.c_str() << std::endl;
        return 0;
    }

    const std::streamsize size = 640*480;
    char buf[size];
    file.read(buf, size);

    int nRead = file.gcount();

     //PERF_TEST(decoder.ProcessData((const BYTE*)&buf[0], nRead), 10);

	 file.close();
     */



     CDib dib;
     dib.Open(_T("test.bmp"));

     BYTE * pBits = dib.GetBits();
     Dib2JPEG(dib, _T("test.jpg"));


	 std::cout<<"Press Q to quit!"<<std::endl;
	 while(tolower(getchar()) != 'q');
	return 0;
}

