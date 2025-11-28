#include "stdafx.h"
//#include <windows.h>
#include "../inc/CameraModel.h"
#include <iostream>
#include <vector>

extern BOOL CalcTheta(double radius, double (&K)[5], double thetaMax, double& result);

int main(int argc, char** args)
{
    //const char* fileName = "CalibrateData.txt";
	//const char* fileName = "CalibrateData(20170427-161517-698).txt";
	//const char* fileName = "CalibrateData(20170512-170527-846).txt";

	//const char* fileName = "CalibrateData(20170512-172736-109).txt";
	//const char* fileName = "CalibrateData(20170626-111325-438).txt";
	const char* fileName = "CalibrateData(2017-09-08-17-34-22).txt";

    FILE* file;
    errno_t err = fopen_s(&file, fileName, "r");
    if(err)
    {
        std::cout << "open file \""<< fileName << "\" failed with error 0x" << std::hex << err << "." << std::endl;
        return -1;
    }

    char buf[1024];
    int nStep = 0;
    std::vector<TPoint2D> vecPtImages;
    std::vector<TPoint2D> vecPtScreens;

    int nImageWidth=0, nImageHeight=0;
    RECT rcScreenBound = {0};
    bool bReadError = false;
	int radius;
    while(fgets(buf, _countof(buf), file))
    {
        const char* pData = buf;
        
        while(*pData == ' ' || *pData == '\t') pData ++;

        if(*pData == '#') continue;
        
        if(nStep == 0)
        {
            int nRet = sscanf_s(buf, "%d,%d", &nImageWidth, &nImageHeight);
            if(nRet != 2)
            {
                bReadError = true;
                break;
            }

            
            nStep ++;
        }
        else if(nStep == 1)
        {

            int nRet = sscanf_s(buf, "%d,%d,%d,%d", &rcScreenBound.left, &rcScreenBound.top, &rcScreenBound.right, &rcScreenBound.bottom);
            if(nRet != 4)
            {
                bReadError = true;
                break;
            }
            nStep ++;
        }
		else if (nStep == 2)
		{
			int nRet = sscanf_s(buf, "%d", &radius);
			if (nRet != 1)
			{
				bReadError = true;
				break;
			}
			nStep++;

		}
        else if(nStep >= 3)
        {
            double u,v;
			int x,y;
            int nRet = sscanf_s(buf, "%lf,%lf,%d,%d", &u, &v, &x, &y);
            if(nRet != 4)
            {
                bReadError = true;
                break;
            }
            
            TPoint2D ptUV;
            ptUV.d[0]  = u ;
            ptUV.d[1]  = v ;

            TPoint2D ptXY;
            ptXY.d[0] = double(x);
            ptXY.d[1] = double(y);


            vecPtImages.push_back(ptUV);
            vecPtScreens.push_back(ptXY);
            
            nStep ++;
        }

    }//while

    if(bReadError)
    {
        std::cout << "read data from file \""<< fileName << "\" failed." << std::endl;
	    fclose(file);
        return -1;
    }
	
	fclose(file);

    //执行校正操作
    CGenericCameraModel camera;

    //double throwRatio = 0.15;
	double throwRatio = 0.86;

    //RECT rcMonitor={0,0, 1920, 1080};

    camera.Calibrate(
        E_CAMERA_PROJECTION_PERSPECTIVE,
        throwRatio,
        & vecPtScreens[0],
        & vecPtImages[0],
        vecPtImages.size(),
        rcScreenBound,
		radius,                 //校正圆半径, 单位:计算机屏幕像素
        nImageWidth,            //相机图像宽度
        nImageHeight);          //相机图像高度


     double r = 1.87366630956431;
     double K[5];
      
     K[0] = 2.05210796214767;
     K[1] = -0.0897249417017254;
     K[2] = -0.234579239333706;
     K[3] = 0.312648415617559;
     K[4] =-0.14485534206051;

     double thetaMax = 1.24664769546042;

	 double theta = 0.0;
     BOOL bRet = CalcTheta(r, K, thetaMax, theta);


    std::cout << "Press q  to exit" << std::endl;
    while(tolower(getchar()) != 'q')
    {

    }
    return 0;
}