//added by toxuke@gmail.com, 2016/11/04
#include <windows.h>
#include <tchar.h>
#include "../inc/CameraModel.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <vector>
#include <Matrix.h>
#include <levenbergMarquardt.h>
#include "nr3.h"
#include "eigen_unsym.h"
#include <atlbase.h>
#include <atlstr.h>
#include "../inc/PerfDetector.h"


#define GENERIC_CAMERA__MODEL_DEBUG 1

void _cdecl GCM_Debug(const TCHAR* szFormat, ...)
{

    TCHAR text[1024];
    va_list vlist;
    va_start(vlist,szFormat);

    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);
        
    _vstprintf_s(text, _countof(text), szFormat,vlist);


    //在Debug Output中输出
    OutputDebugString(text);
    
    //在控制台中输出
    _tprintf(text);

    va_end(vlist);
}



//@功能:旋转矩阵转为旋转矢量, 矢量的模等于绕矢量旋转的角度
//@参数:R, 输入参数, 旋转矩阵
//      k, 输出参数, 旋转矢量
void R2V(const double R[3][3], double k[3])
{
    double traceR = R[0][0] + R[1][1] + R[2][2];


    //θ= arccos[(trace(R) - 1)/2]
    double cosTheta = 0.5 * (traceR - 1);
    double theta    = acos(cosTheta);
    double two_sinTheta =  sin(theta) * 2.0;
    if(abs(theta) < std::numeric_limits<double>::epsilon())
    {//角度太小, 则旋转矢量为零矢量。
        k[0] = k[1] = k[2] = 0.0;
    }
    else
    {
        //   |R32 - R23|
        //k =|R13 - R31| /(2sinθ)
        //   |R21 - R12|
        //
        k[0] = (R[2][1] - R[1][2]) / two_sinTheta * theta;
        k[1] = (R[0][2] - R[2][0]) / two_sinTheta * theta;
        k[2] = (R[1][0] - R[0][1]) / two_sinTheta * theta;
    }
}


//@功能:将旋转矢量转换为旋转矩阵
//@参数:k, 输入参数，旋转矢量
//      R, 输出参数，旋转矩阵
void V2R(const double (&k)[3], double (&R)[3][3])
{
    double theta = norm(&k[0], 3);

    if(abs(theta) < std::numeric_limits<double>::epsilon())
    {
        for(int r = 0; r < 3 ; r++)
        {
            for (int c = 0; c < 3; c++)
            {
                if(r == c)
                {
                    R[r][c] = 1.0;
                }
                else
                {
                    R[r][c] = 0.0;
                }
            }

        }

        return;
    }



    double k1 = k[0] / theta;
    double k2 = k[1] / theta;
    double k3 = k[2] / theta;

    double cos_theta = cos(theta);
    double sin_theta = sin(theta);
    double one_minus_cos_theta = 1 - cos_theta;
    //R00
    R[0][0] = cos_theta + one_minus_cos_theta * k1 * k1;

    //R10
    R[1][0] = k3 * sin_theta + one_minus_cos_theta * k2 * k1;

    //R20
    R[2][0] = -k2 * sin_theta + one_minus_cos_theta * k3 * k1;

    //R01
    R[0][1] = -k3 * sin_theta + one_minus_cos_theta * k1 *k2;

    //R11
    R[1][1] = cos_theta + one_minus_cos_theta * k2 * k2;

    //R21
    R[2][1] = k1 * sin_theta + one_minus_cos_theta * k3 * k2;

    //R02
    R[0][2] = k2 * sin_theta + one_minus_cos_theta * k1 * k3;

    //R12
    R[1][2] = -k1 *sin_theta + one_minus_cos_theta * k2 * k3;

    //R22
    R[2][2] = cos_theta + one_minus_cos_theta * k3 * k3;

}



void InvR(const double (&R)[3][3], double (&inv_R)[3][3])
{

    //  | R00 R01 R02 |
    //R=| R10 R11 R12 |
    //  | R20 R21 R22 |
    double detR = R[0][0] * (R[1][1] * R[2][2] - R[1][2] * R[2][1]) 
        + R[0][1] * (R[1][2] * R[2][0] - R[1][0] * R[2][2])
        + R[0][2] * (R[1][0] * R[2][1] - R[1][1] * R[2][0]);

    if(fabs(detR) < std::numeric_limits<double>::epsilon())
    {//det(R)-->0
        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                inv_R[i][j] = 0.0;
            }
        }
    }



    inv_R[0][0] = (R[1][1] * R[2][2] - R[1][2] * R[2][1])/detR;
    inv_R[0][1] = (R[0][2] * R[2][1] - R[0][1] * R[2][2])/detR;
    inv_R[0][2] = (R[0][1] * R[1][2] - R[0][2] * R[1][1])/detR;


    inv_R[1][0] = (R[1][2] * R[2][0] - R[1][0] * R[2][2])/detR;
    inv_R[1][1] = (R[0][0] * R[2][2] - R[0][2] * R[2][0])/detR;
    inv_R[1][2] = (R[0][2] * R[1][0] - R[0][0] * R[1][2])/detR;

    inv_R[2][0] = (R[1][0] * R[2][1] - R[1][1] * R[2][0])/detR;
    inv_R[2][1] = (R[0][1] * R[2][0] - R[0][0] * R[2][1])/detR;
    inv_R[2][2] = (R[0][0] * R[1][1] - R[0][1] * R[1][0])/detR;

}


//@功能:设函数r(θ)可以用θ的多项式表示, 
//r = k0*θ + k1*θ^3 + k2*θ^5 + k3*θ^7 + k4*θ^9
//利用多项式的伴随矩阵的特征根是
//    r(θ)=0
//的根的性质
//@参数:
//    radius, 目标距离光轴的距离。
//    K, 多项式系数。
//thetaMax, 最大可能的θ值。
//@返回值;
//       返回计算得到的θ值
BOOL CalcTheta(double radius, double (&K)[5], double thetaMax, double &result)
{
    if(radius < std::numeric_limits<double>::epsilon())
	{
		result  = 0.0;
		return TRUE;
	}

    int  highest_index = 4;
    while(highest_index >= 0 )
    {
        if(fabs(K[highest_index]) > std::numeric_limits<double>::epsilon())
        {
            break;
        }

        highest_index --;
    }

    if(highest_index < 0 )
	{
		result  = 0.0;
		return TRUE;
	}

    int dimension = highest_index*2 + 1;

    MatDoub A(dimension, dimension);


    //构造多项式的伴随矩阵(Companion Matrix)
    //     -                                                --    -
    //     |0     1                                         ||1   |
    //     |0     0       1                                 ||θ  |
    //     |0     0       0   1                             ||θ^2|
    //     |0     0       0   0      1                      ||θ^3|
    //  C =|0     0       0   0      0   1                  ||θ^4|
    //     |0     0       0   0      0   0      1           ||θ^5|
    //     |0     0       0   0      0   0      0   1       ||θ^6|
    //     |0     0       0   0      0   0      0   0      1||θ^7|
    //     |r/k5  -k1/k5  0  -k2/k5  0  -k3/k5  0  -k4/k5  0||θ^8|
    //     -                                                --    -
    //
    for(int r = 0; r < dimension - 1; r ++)
    {
        for(int c = 0; c < dimension; c++)
        {

            if(c == (r+1)) 
            {
                A[r][c] = 1.0;
            }
            else
            {
                A[r][c] = 0.0;
            }

        }//for-each(c)

    }//for-each(r)

    //最后一行
    A[dimension - 1][0] = radius / K[highest_index];

    for(int c = 1; c < dimension ; c ++)
    {
        if(c % 2 == 1)
        {
            A[dimension - 1][c] = - K[(c >> 1)] / K[highest_index];
        }
        else
        {
            A[dimension - 1][c] = 0.0;
        }
    }
    /*
    #ifdef GENERIC_CAMERA__MODEL_DEBUG
    for(int r = 0; r < dimension; r++)
    {
    for(int c = 0; c < dimension; c++)
    {
    printf("%20g\t",A[r][c]);
    }
    printf("\n");
    }
    #endif
    */

	try
	{
		//求解伴随矩阵的特征根
		Unsymmeig unsymmeig(A, false);//false:不计算特征向量

		result = thetaMax;

		double tol = 10e-10;

		//选择虚部趋于0, 且实部小于maxTheta的最小实根
		for(int i=0; i < unsymmeig.wri.size(); i++)
		{
			double image = unsymmeig.wri[i].imag();
			double real  = unsymmeig.wri[i].real();
			if(fabs(image) < tol)
			{
				if(0 < real &&  real <  thetaMax)
				{
					result = unsymmeig.wri[i].real();
				}
			}
			/*
			#ifdef GENERIC_CAMERA__MODEL_DEBUG
			printf("%20g + i%-20g \t", unsymmeig.wri[i].real(), unsymmeig.wri[i].imag());
			#endif
			*/
		}//for

	}
	catch(const char* szError)
	{
		szError = szError;

		return FALSE;
	}

	return TRUE;
}


struct TScreen2Sphere3DPoints
{
    std::vector<TPoint3D>  vecScreenPts;
    std::vector<TPoint3D>  vecSpherePts ;
};



//@功能:校正平面上的点Pi <Xi,Yi,Zi> (n=1,..,N) 经过单应矩阵H的变化后得到以相机中心为原点的坐标Pi' <xi',yi',zi'>, 
//      同时相机照片上的校正点的像逆映射到单位半径球面上, 得到坐标点Qi <xi,yi,zi>。
//      设矢量OPi与OQi之间的夹角为αi, 计算夹角αi
//@参数:H, H[3][3]的按行序排列的数组。
void homangleerr(const double* pParams, int nParamCount,  void* lpCtx, std::vector<double>& vecValues)
{
    const double* H = pParams;

    TScreen2Sphere3DPoints* pScreen2Sphere3DPoints = (TScreen2Sphere3DPoints*)lpCtx;
    int nPointNumber = pScreen2Sphere3DPoints->vecScreenPts.size();
    vecValues.resize(nPointNumber);


    for(int i = 0; i< nPointNumber; i++)
    {
        TPoint3D p, P,Q;
        p = pScreen2Sphere3DPoints->vecScreenPts[i];

        for(int j=0; j < 3; j++)
        {
            //H按列排列
            P.d[j] = H[0*3 + j] * p.d[0] + H[1*3 + j] * p.d[1] + H[2*3 + j] * p.d[2];
        }



        double norm_P = sqrt(P.d[0] * P.d[0] + P.d[1] * P.d[1] + P.d[2] * P.d[2]);


        //归一化H转换后的矢量。
        for(int j = 0; j < 3; j++)
        {
            P.d[j] /= norm_P;
        }


        Q = pScreen2Sphere3DPoints->vecSpherePts[i];



        //||A × B|| = ||A||*||B||*sinα, α为矢量A,B之间的夹角
        //
        //        |  i  j   k |
        //A × B = | a0 a1  a2 |
        //        | b0 b1  b2 |
        //
        double crossProduct[3];
        crossProduct[0] = P.d[1] * Q.d[2] - P.d[2] * Q.d[1];
        crossProduct[1] = P.d[2] * Q.d[0] - P.d[0] * Q.d[2];
        crossProduct[2] = P.d[0] * Q.d[1] - P.d[1] * Q.d[0];

        double sinAlpha = sqrt(crossProduct[0] * crossProduct[0] + crossProduct[1] * crossProduct[1] + crossProduct[2] * crossProduct[2]);

        vecValues[i] = sinAlpha;

    }//for

}

//@功能:计算从校正平面到单位球面的中心投影的单应矩阵H
void CompHs_RadialModel(const TPoint2D* pPtsInWorld, const TPoint2D* pPt2DInImg, int nPtNumber, const double* pParams, int nParamCount, double(&Hs)[3][3])
{
	assert(nParamCount == 9);
	double mu = pParams[0];
	double mv = pParams[1];
	double u0 = pParams[2];
	double v0 = pParams[3];
	double K[5];
	K[0] = pParams[4];
	K[1] = pParams[5];
	K[2] = pParams[6];
	K[3] = pParams[7];
	K[4] = pParams[8];
	

	//1.将相机图片上的校正点反投影到以相机中心为原点的单位半径的圆上
	//┌   ┐ ┌                 ┐┌  ┐
	//│xd │ │1/mu  0    -u0/mu││u │
	//│yd │=│0    1/mv  -v0/mv││v │
	//│1  │ │0     0    1     ││1 │
	//└   ┘ └                 ┘└  ┘
	std::vector<TPoint2D>  xy_d;
	xy_d.resize(nPtNumber);

	//xd = (u-u0)/mu
	//yd = (u-v0)/mv

	for (int i = 0; i < nPtNumber; i++)
	{
		xy_d[i].d[0] = (pPt2DInImg[i].d[0] - u0) / mu;
		xy_d[i].d[1] = (pPt2DInImg[i].d[1] - v0) / mv;
	}

	//计算在uv平面上的方向角
	std::vector<double> cosphi, sinphi;
	cosphi.resize(nPtNumber);
	sinphi.resize(nPtNumber);

	for (int i = 0; i < nPtNumber; i++)
	{
		double r_d = sqrt(xy_d[i].d[0] * xy_d[i].d[0] + xy_d[i].d[1] * xy_d[i].d[1]);

		if (r_d != 0.0)
		{
			cosphi[i] = xy_d[i].d[0] / r_d;
			sinphi[i] = xy_d[i].d[1] / r_d;
		}
		else
		{
			cosphi[i] = 0.0;
			sinphi[i] = 0.0;
		}

	}//for



	 //r = x_d^2 + y_d^2
	std::vector<double> r;
	r.resize(nPtNumber);

	for (int i = 0; i < nPtNumber; i++)
	{
		r[i] = sqrt(xy_d[i].d[0] * xy_d[i].d[0] + xy_d[i].d[1] * xy_d[i].d[1]);
	}


	std::vector<double> theta;
	theta.resize(nPtNumber);
	for (int i = 0; i < nPtNumber; i++)
	{
		double result = 0.0;
		if (CalcTheta(r[i], K, M_PI_2, result))
		{
			theta[i] = result;
		}
		else
		{
			assert(false);
		}
		
	}

		 //计算逆映射到单元球面上的点的3D坐标xyz_sphere
	std::vector<TPoint3D> xyz_sphere;
	xyz_sphere.resize(nPtNumber);

	for (int i = 0; i < nPtNumber; i++)
	{
		double sin_theta = sin(theta[i]);
		double cos_theta = cos(theta[i]);

		xyz_sphere[i].d[0] = sin_theta * cosphi[i];
		xyz_sphere[i].d[1] = sin_theta * sinphi[i];
		xyz_sphere[i].d[2] = cos_theta;

	}//


	 //2.利用DLT(Direct Linear Transform)方法计算单应矩阵的初始值。
	 //2.1将校正平面z=0上的点,看作在世界坐标系中的向量, 将这些向量归一化为模为1的单位向量。
	 //  校正平面上的齐次坐标<Xi,Yi,0,1>, 对齐次坐标归一化。
	 //  对齐次坐标归一化不影响其实际的几何意义， 归一化前的坐标和归一化后指的是同一个点。
	 //
	 //
	std::vector<TPoint3D> xyz_world_normalized;
	xyz_world_normalized.resize(nPtNumber);

	for (int i = 0; i< nPtNumber; i++)
	{
		double norm = sqrt(pPtsInWorld[i].d[0] * pPtsInWorld[i].d[0] + pPtsInWorld[i].d[1] * pPtsInWorld[i].d[1] + 1);
		xyz_world_normalized[i].d[0] = pPtsInWorld[i].d[0] / norm;
		xyz_world_normalized[i].d[1] = pPtsInWorld[i].d[1] / norm;
		xyz_world_normalized[i].d[2] = 1 / norm;
	}


	//求解A*h = 0
	//h为9*1的列, hi为单应矩阵的元素。
	//    ┌                                    ┐
	//    │0     0   0 -zX -zY -zZ  yX   yY  yZ│
	//A = │zX   zY  zZ   0   0   0 -xX  -xY -xZ│
	//    │-yX -yY -yZ  xX  xY  xZ   0    0   0│
	//    └                                    ┘

	MatlabSimu::Matrix<double> A(3 * nPtNumber, 9);
	for (int i = 0; i < nPtNumber; i++)
	{
		double X = xyz_world_normalized[i].d[0];
		double Y = xyz_world_normalized[i].d[1];
		double Z = xyz_world_normalized[i].d[2];
		double x = xyz_sphere[i].d[0];
		double y = xyz_sphere[i].d[1];
		double z = xyz_sphere[i].d[2];

		A[i * 3 + 0][0] = 0;
		A[i * 3 + 0][1] = 0;
		A[i * 3 + 0][2] = 0;
		A[i * 3 + 0][3] = -z * X;
		A[i * 3 + 0][4] = -z * Y;
		A[i * 3 + 0][5] = -z * Z;
		A[i * 3 + 0][6] = y * X;
		A[i * 3 + 0][7] = y * Y;
		A[i * 3 + 0][8] = y * Z;


		A[i * 3 + 1][0] = z * X;
		A[i * 3 + 1][1] = z * Y;
		A[i * 3 + 1][2] = z * Z;
		A[i * 3 + 1][3] = 0;
		A[i * 3 + 1][4] = 0;
		A[i * 3 + 1][5] = 0;
		A[i * 3 + 1][6] = -x * X;
		A[i * 3 + 1][7] = -x * Y;
		A[i * 3 + 1][8] = -x * Z;

		A[i * 3 + 2][0] = -y * X;
		A[i * 3 + 2][1] = -y * Y;
		A[i * 3 + 2][2] = -y * Z;
		A[i * 3 + 2][3] = x * X;
		A[i * 3 + 2][4] = x * Y;
		A[i * 3 + 2][5] = x * Z;
		A[i * 3 + 2][6] = 0;
		A[i * 3 + 2][7] = 0;
		A[i * 3 + 2][8] = 0;

	}

	MatlabSimu::Matrix<double> U, S, V;
	MatlabSimu::SVD(A, &S, &U, &V);

	//V的最后一列即为H的解
	std::vector<double> H;
	H.resize(9);

	for (int i = 0; i < 9; i++)
	{
		//V[:][8]是H按行序排列
		//i%3=列
		//i/3=行
		H[(i % 3) * 3 + i / 3] = V[i][8];
	}



	//using non-linear maximization algorithm "Levenberg-Marqudart" to polish value of H.
	TScreen2Sphere3DPoints screen2Sphere3DPoints;
	screen2Sphere3DPoints.vecScreenPts.resize(nPtNumber);
	for (int i = 0; i < nPtNumber; i++)
	{
		screen2Sphere3DPoints.vecScreenPts[i].d[0] = pPtsInWorld[i].d[0];
		screen2Sphere3DPoints.vecScreenPts[i].d[1] = pPtsInWorld[i].d[1];
		screen2Sphere3DPoints.vecScreenPts[i].d[2] = 1;
	}

	screen2Sphere3DPoints.vecSpherePts = xyz_sphere;



	levenbergMarquardt(&H[0], 9, homangleerr, (void*)&screen2Sphere3DPoints);


	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			Hs[r][c] = H[c * 3 + r];
		}
	}

}



//@功能:计算从校正平面到单位球面的中心投影的单应矩阵H
void CompHs_BasicModel(const TPoint2D* pPtsInWorld, const TPoint2D* pPt2DInImg, int nPtNumber, const double* pParams, int nParamCount, double (&Hs)[3][3])
{
    assert(nParamCount == 6);
    double mu  = pParams[0];
    double mv  = pParams[1];
    double u0  = pParams[2];
    double v0  = pParams[3];
    double K1  = pParams[4];
    double K2  = pParams[5];

    //1.将相机图片上的校正点反投影到以相机中心为原点的单位半径的圆上
    //┌   ┐ ┌                 ┐┌  ┐
    //│xd │ │1/mu  0    -u0/mu││u │
    //│yd │=│0    1/mv  -v0/mv││v │
    //│1  │ │0     0    1     ││1 │
    //└   ┘ └                 ┘└  ┘
    std::vector<TPoint2D>  xy_d;
    xy_d.resize(nPtNumber);

    //xd = (u-u0)/mu
    //yd = (u-v0)/mv

    for(int i = 0; i < nPtNumber; i++)
    {
        xy_d[i].d[0]  = (pPt2DInImg[i].d[0] - u0)/mu;
        xy_d[i].d[1]  = (pPt2DInImg[i].d[1] - v0)/mv;
    }

    //计算在uv平面上的方向角
    std::vector<double> cosphi, sinphi;
    cosphi.resize(nPtNumber);
    sinphi.resize(nPtNumber);

    for(int i = 0; i < nPtNumber; i++)
    {
        double r_d = sqrt(xy_d[i].d[0] * xy_d[i].d[0] + xy_d[i].d[1] * xy_d[i].d[1]);

        if(r_d != 0.0)
        {
            cosphi[i] = xy_d[i].d[0] / r_d;
            sinphi[i] = xy_d[i].d[1] / r_d;
        }
        else
        {
            cosphi[i] = 0.0;
            sinphi[i] = 0.0;
        }

    }//for



    //r = x_d^2 + y_d^2
    std::vector<double> r;
    r.resize(nPtNumber);

    for(int i = 0; i < nPtNumber; i++)
    {
        r[i] = sqrt(xy_d[i].d[0] * xy_d[i].d[0] + xy_d[i].d[1] * xy_d[i].d[1]);
    }

    std::vector<double> theta;
    theta.resize(nPtNumber);
    //r = k1*θ + k2 *θ^3
    // k2 *θ^3 + k1*θ - r = 0
    //r,k1,k2已知，求解一元3次方程组的实数解
    //求解方程
    //a*x^3 + b*x^2 + c*x + d = 0
    //这里 b ≡ 0.0;
    double a, c ,d, root = 0.0;
    a = K2;
    c = K1;
    for(int i=0; i < nPtNumber; i++)
    {
        if(fabs(K2) <= std::numeric_limits<double>::epsilon())// K2 == 0.0
        {
            if(fabs(K1) > std::numeric_limits<double>::epsilon())//K1 != 0.0;
            {
                r[i] = r[i] / K1;
            }
        }
        else 
        {
            d = - r[i];

            double delta_0, delta_1, delta;

            delta_0 = - 3 * a * c;
            delta_1 =  27 * a * a * d;
            //可以看到
            //delta = 27*a^4*d^4 + 108*a^3*c^3
            //a,c都大于0,所以delta恒大于0
            delta   = delta_1 * delta_1 - 4 * delta_0 * delta_0 * delta_0;
            double B = ( delta_1 - sqrt(delta) ) / 2.0;

            double C;
            //pow,计算base^exp，但base必须大于0
            if(B < 0)
            {
                C = pow(-B, 1.0 / 3.0 );
                C = -C;
            }
            else
            {
                C = pow (B, 1.0 / 3.0);
            }
            if( fabs(C) > std::numeric_limits<double>::epsilon()) //C !=0
            {
                root =  -( C + delta_0 / C ) / ( 3 * a );
            }
            else
            {
                root = 0.0;
            }

            theta[i] = root;

        }//else
    }//for-each points in image


    //计算逆映射到单元球面上的点的3D坐标xyz_sphere
    std::vector<TPoint3D> xyz_sphere;
    xyz_sphere.resize(nPtNumber);

    for(int i= 0; i < nPtNumber; i++)
    {
        double sin_theta = sin(theta[i]);
        double cos_theta = cos(theta[i]);

        xyz_sphere[i].d[0] = sin_theta * cosphi[i];
        xyz_sphere[i].d[1] = sin_theta * sinphi[i];
        xyz_sphere[i].d[2] = cos_theta;

    }//


    //2.利用DLT(Direct Linear Transform)方法计算单应矩阵的初始值。
    //2.1将校正平面z=0上的点,看作在世界坐标系中的向量, 将这些向量归一化为模为1的单位向量。
    //  校正平面上的齐次坐标<Xi,Yi,0,1>, 对齐次坐标归一化。
    //  对齐次坐标归一化不影响其实际的几何意义， 归一化前的坐标和归一化后指的是同一个点。
    //
    //
    std::vector<TPoint3D> xyz_world_normalized;
    xyz_world_normalized.resize(nPtNumber);

    for(int i=0; i< nPtNumber; i++)
    {
        double norm = sqrt(pPtsInWorld[i].d[0] * pPtsInWorld[i].d[0] + pPtsInWorld[i].d[1] * pPtsInWorld[i].d[1] + 1);
        xyz_world_normalized[i].d[0] = pPtsInWorld[i].d[0] / norm;
        xyz_world_normalized[i].d[1] = pPtsInWorld[i].d[1] / norm;
        xyz_world_normalized[i].d[2] = 1 / norm;
    }


    //求解A*h = 0
    //h为9*1的列, hi为单应矩阵的元素。
    //    ┌                                    ┐
    //    │0     0   0 -zX -zY -zZ  yX   yY  yZ│
    //A = │zX   zY  zZ   0   0   0 -xX  -xY -xZ│
    //    │-yX -yY -yZ  xX  xY  xZ   0    0   0│
    //    └                                    ┘

    MatlabSimu::Matrix<double> A(3*nPtNumber, 9);
    for(int i = 0; i < nPtNumber; i++)
    {
        double X  = xyz_world_normalized[i].d[0];
        double Y  = xyz_world_normalized[i].d[1];
        double Z  = xyz_world_normalized[i].d[2];
        double x = xyz_sphere[i].d[0];
        double y = xyz_sphere[i].d[1];
        double z = xyz_sphere[i].d[2];

        A[i*3 + 0][0] = 0;
        A[i*3 + 0][1] = 0;
        A[i*3 + 0][2] = 0;
        A[i*3 + 0][3] = -z * X;
        A[i*3 + 0][4] = -z * Y;
        A[i*3 + 0][5] = -z * Z;
        A[i*3 + 0][6] = y * X;
        A[i*3 + 0][7] = y * Y;
        A[i*3 + 0][8] = y * Z;


        A[i*3 + 1][0] = z * X;
        A[i*3 + 1][1] = z * Y;
        A[i*3 + 1][2] = z * Z;
        A[i*3 + 1][3] = 0;
        A[i*3 + 1][4] = 0;
        A[i*3 + 1][5] = 0;
        A[i*3 + 1][6] = -x * X;
        A[i*3 + 1][7] = -x * Y;
        A[i*3 + 1][8] = -x * Z;

        A[i*3 + 2][0] = -y * X;
        A[i*3 + 2][1] = -y * Y;
        A[i*3 + 2][2] = -y * Z;
        A[i*3 + 2][3] = x * X;
        A[i*3 + 2][4] = x * Y;
        A[i*3 + 2][5] = x * Z;
        A[i*3 + 2][6] = 0;
        A[i*3 + 2][7] = 0;
        A[i*3 + 2][8] = 0;

    }

    MatlabSimu::Matrix<double> U, S, V;
    MatlabSimu::SVD(A,&S, &U, &V);

    //V的最后一列即为H的解
    std::vector<double> H;
    H.resize(9);

    for(int i = 0; i < 9; i++)
    {
        //V[:][8]是H按行序排列
        //i%3=列
        //i/3=行
        H[(i%3)*3 + i/3] = V[i][8];
    }



    //using non-linear maximization algorithm "Levenberg-Marqudart" to polish value of H.
    TScreen2Sphere3DPoints screen2Sphere3DPoints;
    screen2Sphere3DPoints.vecScreenPts.resize(nPtNumber);
    for(int i=0; i < nPtNumber; i++)
    {
        screen2Sphere3DPoints.vecScreenPts[i].d[0]  = pPtsInWorld[i].d[0];
        screen2Sphere3DPoints.vecScreenPts[i].d[1]  = pPtsInWorld[i].d[1];
        screen2Sphere3DPoints.vecScreenPts[i].d[2]  = 1;
    }

    screen2Sphere3DPoints.vecSpherePts = xyz_sphere;



    levenbergMarquardt(&H[0], 9,  homangleerr, (void*)&screen2Sphere3DPoints );


    for(int r = 0; r < 3; r++)
    {
        for(int c = 0; c < 3; c++)
        {
            Hs[r][c] =  H[c*3 + r];
        }
    }

}


//@功能:从单应矩阵m_hs计算出相机的外部参数
BOOL InitExternalParameter(const double (&Hs)[3][3], double (&Rs)[3][3], double (&Ts)[3] )
{

    //
    double norm_1st_col = 0.0, norm_2nd_col = 0.0;

    //计算m_Hs第一列和第二列的模
    for(int r = 0; r < 3; r++)
    {
        norm_1st_col += Hs[r][0] * Hs[r][0];
        norm_2nd_col += Hs[r][1] * Hs[r][1];
    }

    norm_1st_col = sqrt(norm_1st_col);
    norm_2nd_col = sqrt(norm_2nd_col);

    double Rt[3][3];

    //Rt = Hs.* 1/2(norm_1st_col + norm_2nd_col);

    double divisor = 0.5 * (norm_1st_col + norm_2nd_col);
    if( abs(divisor) < std::numeric_limits<double>::epsilon())
    {
        return false;
    }


    for(int r = 0; r < 3; r++)
    {
        for(int c = 0; c < 3; c++)
        {
            Rt[r][c] =  Hs[r][c] / divisor;

        }
    }

    //偏移量
    for(int r = 0; r < 3; r++)
    {
        Ts[r] = Rt[r][2];
    }


    double Rotation[3][3];
    for(int r = 0; r < 3; r++)
    {
        Rotation[r][0] = Rt[r][0];
        Rotation[r][1] = Rt[r][1];
    }

    //r3 = cross(r1, r2)
    Rotation[0][2] = Rotation[1][0] * Rotation [2][1] - Rotation[2][0] * Rotation [1][1];
    Rotation[1][2] = Rotation[2][0] * Rotation [0][1] - Rotation[0][0] * Rotation [2][1];
    Rotation[2][2] = Rotation[0][0] * Rotation [1][1] - Rotation[1][0] * Rotation [0][1];



    //┌ ┐ ┌              ┐┌ ┐
    //│x│ │R11 R12 R13 t1││X│
    //│y│=│R21 R22 R23 t2││Y│
    //│z│ │R31 R32 R33 t3││Z│
    //│1│ │0   0   0   1 ││1│
    //└ ┘ └              ┘└ ┘
    //<t1,t2,t3,1>对应世界坐标系原点<0,0,0,1>在以相机为原点的坐标系下的坐标。

 	//在校正平面所在的世界坐标系O-XYZ中，相机的位置应该处于世界坐标系中的负Z轴上
	//
    //┌ ┐ ┌              ┐┌ ┐
    //│X│ │InvR  -InvR*Ts││x│
    //│Y│=│              ││y│
    //│Z│ │              ││z│
    //│1│ │0           1 ││1│
    //└ ┘ └              ┘└ ┘
	//
	//相机中心在世界坐标系中的坐标为-InvR*Ts, 其中Z分量等于 -[R13 R23 R33]*Ts
	//即,
	//    -[R13 R23 R33]*Ts < 0
	//     [R13 R23 R33]*Ts > 0
	//


    double ZZ = Rotation[0][2] * Ts[0] + Rotation[1][2]*Ts[1] + Rotation[2][2]*Ts[2];

    if(ZZ < 0 )
    {
        //R(:,0) = -R(:,0)
        Rotation[0][0] = -Rotation[0][0];Rotation[1][0] = -Rotation[1][0];Rotation[2][0] = -Rotation[2][0];
        //R(:,1) = -R(:,1)
        Rotation[0][1] = -Rotation[0][1];Rotation[1][1] = -Rotation[1][1];Rotation[2][1] = -Rotation[2][1];

        Ts[0] = -Ts[0];Ts[1] = -Ts[1];Ts[2] = -Ts[2];
    }



    //正交化化Rotation
    MatlabSimu::Matrix<double> A, U, S, V;

    A.SetDimension(3,3);

    for(int r = 0; r < 3; r++)
    {
        for(int c = 0; c < 3; c++)
        {
            A[r][c] = Rotation[r][c];

        }
    }



    MatlabSimu::SVD(A,&S, &U, &V);

    A = U * Transpose(V);

    //A.Print("R=\n");

    for(int r = 0; r < 3; r++)
    {
        for(int c = 0; c < 3; c++)
        {
            Rs[r][c] = A[r][c];

        }
    }

    return TRUE;

}


//x = (r + dr)*cosφ - dt*sinφ
//y = (r + dr)*sinφ + dt*cosφ
//where
//r = (k1*theta + k2*theta.^3 + k3*theta.^5 + k4*theta.^7 + k5*theta.^9);
//Δr = (g(1)*θ + g(2)*θ^3 + g(3)*θ^5) * (i1*cosφ + i2*sinφ + i3*cos2φ + i4*sin2φ)
//Δt = (h(1)*θ + h(2)*θ^3 + h(3)*θ^5) * (j1*cosφ + j2*sinφ + j3*cos2φ + j4*sin2φ)
//@功能:根据入射光线的角度(θ,φ ), 和畸变参数,k, g, h, i, j来计算雅可比行列式d(x,y)/d(θ, φ);
//
void Jacobian_ExtendedMode(
                           const double& theta, 
                           const double& phi, 
                           const double (&k)[5], 
                           const double (&g)[3], 
                           const double (&i)[4], 
                           const double (&h)[3], 
                           const double (&j)[4],
                           double (&jac)[2][2])
{
    double cosphi = cos(phi);
    double sinphi = sin(phi);
    double cos2phi = cosphi*cosphi - sinphi*sinphi;
    double sin2phi = 2*sinphi*cosphi;

    double theta_2 = theta   * theta;
    double theta_3 = theta_2 * theta;
    double theta_4 = theta_3 * theta;
    double theta_5 = theta_4 * theta;
    double theta_6 = theta_5 * theta;
    double theta_7 = theta_6 * theta;
    double theta_8 = theta_7 * theta;
    double theta_9 = theta_8 * theta;

    double r = k[0]*theta + k[1]*theta_3 + k[2]*theta_5 + k[3]*theta_7 + k[4]*theta_9;
    //dr/dθ
    double dr_dTheta = k[0] + 3*k[1]*theta_2 + 5*k[2]*theta_4 + 7*k[3]*theta_6 + 9*k[4]*theta_8;
    
    double dr = (g[0]*theta + g[1]*theta_3 + g[2]*theta_5) * (i[0]*cosphi + i[1]*sinphi + i[2]*cos2phi + i[3]*sin2phi);
    
    double dt = (h[0]*theta + h[1]*theta_3 + h[2]*theta_5) * (j[0]*cosphi + j[1]*sinphi + j[2]*cos2phi + j[3]*sin2phi);
    

    //Δr = (g(1)*θ + g(2)*θ^3 + g(3)*θ^5) * (i1*cosφ + i2*sinφ + i3*cos2φ + i4*sin2φ)
    //dΔr/dθ
    double d_dr_dTheta = (g[0] + g[1]*3*theta_2 + g[2]*5*theta_4) * (i[0]*cosphi + i[1]*sinphi + i[2]*cos2phi + i[3]*sin2phi);
    //dΔr/dφ
    double d_dr_dPhi   = (g[0]*theta + g[1]*theta_3 + g[2]*theta_5) *(i[0]*(-sinphi) + i[1]*cosphi + i[2]*(-sin2phi)*2 + i[3]*cos2phi*2);

    //Δt = (h(1)*θ + h(2)*θ^3 + h(3)*θ^5) * (j1*cosφ + j2*sinφ + j3*cos2φ + j4*sin2φ)
    //dΔt/dθ
    double d_dt_dTheta = (h[0] + h[1]*3*theta_2 + h[2]*5*theta_4) * (j[0]*cosphi + j[1]*sinphi + j[2]*cos2phi + j[3]*sin2phi);
    //dΔt/dφ
    double d_dt_dPhi= (h[0]*theta + h[1]*theta_3 + h[2]*theta_5) * (j[0]*(-sinphi) + j[1]*cosphi + j[2]*(-sin2phi)*2 + j[3]*cos2phi*2);

    ///x = (r + dr)*cosφ - dt*sinφ
    //dx/dθ
    double dx_dTheta = (dr_dTheta + d_dr_dTheta)*cosphi - d_dt_dTheta*sinphi;
    //dx/dφ
    double dx_dPhi   = d_dr_dPhi*cosphi + (r+dr)*(-sinphi) - d_dt_dPhi*sinphi - dt*cosphi;

    //y = (r + dr)*sinφ + dt*cosφ
    double dy_dTheta = (dr_dTheta + d_dr_dTheta)*sinphi + d_dt_dTheta*cosphi;
    double dy_dPhi   = d_dr_dPhi*sinphi + (r + dr)*cosphi + d_dt_dPhi*cosphi + dt*(-sinphi);

    
    jac[0][0] = dx_dTheta;
    jac[0][1] = dx_dPhi;

    jac[1][0] = dy_dTheta;
    jac[1][1] = dy_dPhi;
    

}

//@功能:根据目标通过光心的入射光线与光轴的夹角和方位角以及相机畸变参数
//      计算其在成像面上带畸变的坐标
void xdyd(
          const double& theta, 
          const double& phi, 
          const double (&k)[5], 
          const double (&g)[3], 
          const double (&i)[4], 
          const double (&h)[3], 
          const double (&j)[4],
          double & xd,
          double & yd)
{
    double sinphi  = sin(phi);
    double cosphi  = cos(phi);
    
    double sin2phi = 2*sinphi*cosphi;
    double cos2phi = cosphi*cosphi - sinphi*sinphi;


    double theta_2 = theta   * theta;
    double theta_3 = theta_2 * theta;
    double theta_5 = theta_3 * theta_2;
    double theta_7 = theta_5 * theta_2;
    double theta_9 = theta_7 * theta_2;


    double dr = (g[0]*theta + g[1]*theta_3 + g[2]*theta_5) * (i[0]*cosphi + i[1]*sinphi + i[2]*cos2phi + i[3]*sin2phi);
    double dt = (h[0]*theta + h[1]*theta_3 + h[2]*theta_5) * (j[0]*cosphi + j[1]*sinphi + j[2]*cos2phi + j[3]*sin2phi);

    double r = k[0]*theta + k[1]*theta_3 + k[2]*theta_5 + k[3]*theta_7 + k[4]*theta_9;

    xd = (r + dr)*cosphi + dt*(-sinphi);
    yd = (r + dr)*sinphi + dt*(cosphi );

}


//@功能:利用Levenberg-Marquardt算法迭代计算θ,φ参数的最优值
void GetThetaPhi_LM(
                    const double& xd,
                    const double& yd,
                    const double& init_theta, 
                    const double& init_phi, 
                    const double (&k)[5], 
                    const double (&g)[3], 
                    const double (&i)[4], 
                    const double (&h)[3], 
                    const double (&j)[4],
                    double &theta,
                    double &phi)
{

    double theta_model = init_theta;
    double phi_model   = init_phi;

    double xd_model, yd_model;

    xdyd(
        theta_model,
        phi_model,
        k,
        g,
        i,
        h,
        j,
        xd_model,
        yd_model);


    double sum_error_square_model = (xd - xd_model)*(xd - xd_model) + (yd - yd_model)*(yd - yd_model);


    double deltaFuncs[2];
    deltaFuncs[0] = xd - xd_model;
    deltaFuncs[1] = yd - yd_model;


    double Jac[2][2];
    Jacobian_ExtendedMode(
        theta_model,
        phi_model,
        k,
        g,
        i,
        h,
        j,
        Jac);


 

    double JTJ[2][2];

    JTJ[0][0] = Jac[0][0]*Jac[0][0] + Jac[1][0]*Jac[1][0];
    JTJ[0][1] = Jac[0][0]*Jac[0][1] + Jac[1][0]*Jac[1][1];
    JTJ[1][0] = Jac[0][1]*Jac[0][0] + Jac[1][1]*Jac[1][0];
    JTJ[1][1] = Jac[0][1]*Jac[0][1] + Jac[1][1]*Jac[1][1];


    int max_iter = 10000;
    int iter = 0;
    double tolX   = 1e-6;
    double tolFun = 1e-6;
    double lambda = 1e-2;
    double eps2 = std::numeric_limits<double>::epsilon()*std::numeric_limits<double>::epsilon();

    
    double JTJ_Aug[2][2];
    //Levenberg-Marquardt算法迭代计算θ,φ参数
    bool bDone = false;
    bool  bSuccessfulStep = true;

    while(!bDone)
    {
        //J_Aug = λI + JTJ
        if(bSuccessfulStep)
        {
            JTJ_Aug[0][0] = JTJ[0][0];
            JTJ_Aug[0][1] = JTJ[0][1];
            JTJ_Aug[1][0] = JTJ[1][0];
            JTJ_Aug[1][1] = JTJ[1][1];
        }

        JTJ_Aug[0][0] += lambda;
        JTJ_Aug[1][1] += lambda;

        double inv_Jac_Aug[2][2];

        double det_Jac_Aug = JTJ_Aug[0][0]*JTJ_Aug[1][1] - JTJ_Aug[0][1]*JTJ_Aug[1][0];

        inv_Jac_Aug[0][0] =  JTJ_Aug[1][1] / det_Jac_Aug;
        inv_Jac_Aug[0][1] = -JTJ_Aug[0][1] / det_Jac_Aug;
        inv_Jac_Aug[1][0] = -JTJ_Aug[1][0] / det_Jac_Aug;
        inv_Jac_Aug[1][1] =  JTJ_Aug[0][0] / det_Jac_Aug;;


        double innovation[2];
        //innovation = inv(λI + JTJ)*JT*deltaFuncs
        innovation[0] = (inv_Jac_Aug[0][0]*Jac[0][0] + inv_Jac_Aug[0][1]*Jac[0][1])*deltaFuncs[0] + \
            (inv_Jac_Aug[0][0]*Jac[1][0] + inv_Jac_Aug[0][1]*Jac[1][1])*deltaFuncs[1];

        innovation[1] = (inv_Jac_Aug[1][0]*Jac[0][0] + inv_Jac_Aug[1][1]*Jac[0][1])*deltaFuncs[0] + \
            (inv_Jac_Aug[1][0]*Jac[1][0] + inv_Jac_Aug[1][1]*Jac[1][1])*deltaFuncs[1];

        double normsq_innovation = (innovation[0]*innovation[0] + innovation[1]*innovation[1]);
        double theta_try, phi_try;

        theta_try = theta_model + innovation[0];
        phi_try   = phi_model   + innovation[1];

        double xd_try,yd_try;

        xdyd(
            theta_try,
            phi_try,
            k,
            g,
            i,
            h,
            j,
            xd_try,
            yd_try);

        double sum_error_square_try = (xd - xd_try)*(xd - xd_try) + (yd - yd_try)*(yd - yd_try);

        if(sum_error_square_try < sum_error_square_model)
        {
            if(bSuccessfulStep) lambda = lambda * 0.1;

            theta_model = theta_try;
            phi_model   =  phi_try;

            //更新雅克比行列式
            Jacobian_ExtendedMode(
                theta_model,
                phi_model,
                k,
                g,
                i,
                h,
                j,
                Jac);

            //JTJ = transpose(J)*J;

            JTJ[0][0] = Jac[0][0]*Jac[0][0] + Jac[1][0]*Jac[1][0];
            JTJ[0][1] = Jac[0][0]*Jac[0][1] + Jac[1][0]*Jac[1][1];
            JTJ[1][0] = Jac[0][1]*Jac[0][0] + Jac[1][1]*Jac[1][0];
            JTJ[1][1] = Jac[0][1]*Jac[0][1] + Jac[1][1]*Jac[1][1];


            double normsq_param = theta_model*theta_model + phi_model*phi_model;
            if(normsq_innovation < tolX * (eps2 + normsq_param))
            {
                bDone  = true;
            }

            if(sum_error_square_try < tolFun *(eps2 + sum_error_square_model))
            {
                bDone = true;
            }



            deltaFuncs[0] = xd - xd_try;
            deltaFuncs[1] = yd - yd_try;

            sum_error_square_model = sum_error_square_try;

            bSuccessfulStep = true;
        }
        else
        {
            lambda = lambda*10;

            double normsq_param = theta_try*theta_try + phi_try*phi_try;

            if( normsq_innovation < tolX*(eps2 + normsq_param) )
            {
                bDone = true;
            }

            bSuccessfulStep = false;

        }

		iter = iter + 1;
        if(iter > max_iter) bDone = true;

    }//while

    theta = theta_model;
    phi   = phi_model;
}





//@功能:根据校正得到的内部参数和外部参数，将坐标从相机平面投影到校正平面上。
//@参数:pPt2DInImag, 输出参数, 输入的相机坐标
//      nPtNumber, 点的个数
//      vecInternalParams, 内部参数
//      R, 旋转矩阵
//      T, 偏移矢量
//      pPt2DInWorld, 输出参数, 保存世界坐标的数组
BOOL BackwardProject(const TPoint2D* pPt2DInImag, int nPtNumber, const double* pInternalParams, int nParamsCount, double thetaMax, const double (&InvR)[3][3], const double (&T)[3],  TPoint2D* pPt2DInWorld)
{

    double k[5], g[3], i[4], h[3], j[4] ;
    double mu,mv, u0,v0;
    if(nParamsCount == CGenericCameraModel::BASIC_MODEL_INTERNAL_PARAMS_COUNT)
    {
        mu   = pInternalParams[0];
        mv   = pInternalParams[1];
        u0   = pInternalParams[2];
        v0   = pInternalParams[3];
        k[0] = pInternalParams[4];
        k[1] = pInternalParams[5];
        k[2] = 0.0;
        k[3] = 0.0;
        k[4] = 0.0;
        g[0] = 0.0;
        g[1] = 0.0;
        g[2] = 0.0;
        i[0] = 0.0;
        i[1] = 0.0;
        i[2] = 0.0;
        i[3] = 0.0;
        h[0] = 0.0;
        h[1] = 0.0;
        h[2] = 0.0;
        j[0] = 0.0;
        j[1] = 0.0;
        j[2] = 0.0;
        j[3] = 0.0;

    }
    else if(nParamsCount == CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT)
    {
        mu   = pInternalParams[0];
        mv   = pInternalParams[1];
        u0   = pInternalParams[2];
        v0   = pInternalParams[3];
        k[0] = pInternalParams[4];
        k[1] = pInternalParams[5];
        k[2] = pInternalParams[6];
        k[3] = pInternalParams[7];
        k[4] = pInternalParams[8];
        g[0] = 0.0;
        g[1] = 0.0;
        g[2] = 0.0;
        i[0] = 0.0;
        i[1] = 0.0;
        i[2] = 0.0;
        i[3] = 0.0;
        h[0] = 0.0;
        h[1] = 0.0;
        h[2] = 0.0;
        j[0] = 0.0;
        j[1] = 0.0;
        j[2] = 0.0;
        j[3] = 0.0;
    }
    else if(nParamsCount == CGenericCameraModel::EXTENDED_MODEL_INTERNAL_PARAMS_COUNT)
    {
        mu   = pInternalParams[0 ];
        mv   = pInternalParams[1 ];
        u0   = pInternalParams[2 ];
        v0   = pInternalParams[3 ];
        k[0] = pInternalParams[4 ];
        k[1] = pInternalParams[5 ];
        k[2] = pInternalParams[6 ];
        k[3] = pInternalParams[7 ];
        k[4] = pInternalParams[8 ];
        g[0] = pInternalParams[9 ];
        g[1] = pInternalParams[10];
        g[2] = pInternalParams[11];
        i[0] = pInternalParams[12];
        i[1] = pInternalParams[13];
        i[2] = pInternalParams[14];
        i[3] = pInternalParams[15];
        h[0] = pInternalParams[16];
        h[1] = pInternalParams[17];
        h[2] = pInternalParams[18];
        j[0] = pInternalParams[19];
        j[1] = pInternalParams[20];
        j[2] = pInternalParams[21];
        j[3] = pInternalParams[22];

    }
    else
    {
        return FALSE;
    }

    if(fabs(mu) < std::numeric_limits<double>::epsilon())
    {//mu-->0;
        return FALSE;
    }

    if(fabs(mv) < std::numeric_limits<double>::epsilon())
    {//mv-->0
        return FALSE;
    }


    double offset_X = InvR[0][0] * T[0] + InvR[0][1] * T[1] + InvR[0][2] * T[2];
    double offset_Y = InvR[1][0] * T[0] + InvR[1][1] * T[1] + InvR[1][2] * T[2];

    for(int nPtIndex = 0; nPtIndex < nPtNumber; nPtIndex++)
    {
        double u = pPt2DInImag[nPtIndex].d[0];
        double v = pPt2DInImag[nPtIndex].d[1];
        double xd = (u - u0) / mu;
        double yd = (v - v0) / mv;
        double rd = sqrt(xd*xd + yd*yd);

        double x_sphere = 0.0, y_sphere = 0.0, z_sphere=0.0;//映射到单位球面上的坐标
        if( fabs(rd) < std::numeric_limits<double>::epsilon())
        {//rd -->0
            x_sphere = 0.0; 
            y_sphere = 0.0;
            z_sphere = 1.0;
        }
        else
        {
            double phi_d   = atan2(yd, xd);


            double theta_d;
            {
				//性能测试
                //CPerfDetector pf1(_T("CalcTheta"));


				//rd^2=(r+dr)^2 + dt^2
				//一般dt很小,所以 rd≈(r+dr)
				//这里
				//dr = (g(1)*theta + g(2)*theta.^3 + g(3)*theta.^5 ).*( i(1)*cosphi + i(2)*sinphi + i(3)*cos2phi + i(4)*sin2phi );
				//r = k1*theta + k2*theta.^3 + k3*theta.^5 + k4*theta.^7 + k5*theta.^9;
				
				 double cosphi_d = cos(phi_d);
				 double sinphi_d = sin(phi_d);
				 double cos2phi_d = cosphi_d*cosphi_d - sinphi_d*sinphi_d;
				 double sin2phi_d = 2*cosphi_d*sinphi_d;
   				 double dr_coef = i[0]*cosphi_d + i[1]*sinphi_d + i[2]*cos2phi_d + i[3]*sin2phi_d;

				 double kk[5];
				 kk[0] = k[0] +  dr_coef*g[0];
				 kk[1] = k[1] +  dr_coef*g[1];
				 kk[2] = k[2] +  dr_coef*g[2];
				 kk[3] = k[3];
				 kk[4] = k[4];

                BOOL bRet =  CalcTheta(rd, kk, thetaMax, theta_d);//测试表明CalcTheta耗费800us
				
				if(!bRet) return FALSE;
				if(theta_d < 0.0 || theta_d  > M_PI/2)
				{
					return FALSE;
				}
            }

            double theta,phi;

            {
                //CPerfDetector pf1(_T("GetThetaPhi_LM"));//测试表明GetThetaPhi_LM耗费4us
                GetThetaPhi_LM(
                    xd,
                    yd,
                    theta_d,
                    phi_d,
                    k,
                    g,
                    i,
                    h,
                    j,
                    theta,
                    phi);
            }



            double sinphi = sin(phi);
            double cosphi = cos(phi);

            x_sphere = sin(theta) * cosphi;
            y_sphere = sin(theta) * sinphi;
            z_sphere = cos(theta);
        }

        double ratio = (InvR[2][0] * x_sphere + InvR[2][1] * y_sphere + InvR[2][2]* z_sphere);
        double scale_factor = (InvR[2][0] * T[0] + InvR[2][1] * T[1] + InvR[2][2] * T[2])/ratio;

        double X = (InvR[0][0] * x_sphere + InvR[0][1] * y_sphere + InvR[0][2] * z_sphere) * scale_factor - offset_X;
        double Y = (InvR[1][0] * x_sphere + InvR[1][1] * y_sphere + InvR[1][2] * z_sphere) * scale_factor - offset_Y;

        pPt2DInWorld[nPtIndex].d[0] = X;
        pPt2DInWorld[nPtIndex].d[1] = Y;

    }//for

    return TRUE;
}

//@功能:根据校正得到的内部参数和外部参数，从校正平面正向投影到相机平面上
//@参数:pPt2DInWorld, 输出参数, 输入的世界坐标
//      nPtNumber, 点的个数
//      vecInternalParams, 内部参数
//      R, 旋转矩阵
//      T, 偏移矢量
//      pPt2DInImg, 输出参数, 保存图像坐标的数组
BOOL ForwardProject2D(const TPoint2D* pPt2DInWorld, int nPtNumber, const double* pInternalParams, int nParamsCount, const double (&R)[3][3], const double (&T)[3],  TPoint2D* pPt2DInImg )
{

    double k[5], g[3], i[4], h[3], j[4];
    double mu,mv, u0,v0;
    if(nParamsCount == CGenericCameraModel::BASIC_MODEL_INTERNAL_PARAMS_COUNT)
    {
        mu   = pInternalParams[0];
        mv   = pInternalParams[1];
        u0   = pInternalParams[2];
        v0   = pInternalParams[3];
        k[0] = pInternalParams[4];
        k[1] = pInternalParams[5];
        k[2] = 0.0;
        k[3] = 0.0;
        k[4] = 0.0;
        g[0] = 0.0;
        g[1] = 0.0;
        g[2] = 0.0;
        i[0] = 0.0;
        i[1] = 0.0;
        i[2] = 0.0;
        i[3] = 0.0;
        h[0] = 0.0;
        h[1] = 0.0;
        h[2] = 0.0;
        j[0] = 0.0;
        j[1] = 0.0;
        j[2] = 0.0;
        j[3] = 0.0;

    }
    else if(nParamsCount == CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT)
    {
        mu   = pInternalParams[0];
        mv   = pInternalParams[1];
        u0   = pInternalParams[2];
        v0   = pInternalParams[3];
        k[0] = pInternalParams[4];
        k[1] = pInternalParams[5];
        k[2] = pInternalParams[6];
        k[3] = pInternalParams[7];
        k[4] = pInternalParams[8];
        g[0] = 0.0;
        g[1] = 0.0;
        g[2] = 0.0;
        i[0] = 0.0;
        i[1] = 0.0;
        i[2] = 0.0;
        i[3] = 0.0;
        h[0] = 0.0;
        h[1] = 0.0;
        h[2] = 0.0;
        j[0] = 0.0;
        j[1] = 0.0;
        j[2] = 0.0;
        j[3] = 0.0;
    }
    else if(nParamsCount == CGenericCameraModel::EXTENDED_MODEL_INTERNAL_PARAMS_COUNT)
    {
        mu   = pInternalParams[0 ];
        mv   = pInternalParams[1 ];
        u0   = pInternalParams[2 ];
        v0   = pInternalParams[3 ];
        k[0] = pInternalParams[4 ];
        k[1] = pInternalParams[5 ];
        k[2] = pInternalParams[6 ];
        k[3] = pInternalParams[7 ];
        k[4] = pInternalParams[8 ];
        g[0] = pInternalParams[9 ];
        g[1] = pInternalParams[10];
        g[2] = pInternalParams[11];
        i[0] = pInternalParams[12];
        i[1] = pInternalParams[13];
        i[2] = pInternalParams[14];
        i[3] = pInternalParams[15];
        h[0] = pInternalParams[16];
        h[1] = pInternalParams[17];
        h[2] = pInternalParams[18];
        j[0] = pInternalParams[19];
        j[1] = pInternalParams[20];
        j[2] = pInternalParams[21];
        j[3] = pInternalParams[22];

    }
    else
    {
        return FALSE;
    }


    for(int nPtIndex = 0; nPtIndex < nPtNumber; nPtIndex++)
    {
        double X = pPt2DInWorld[nPtIndex].d[0];
        double Y = pPt2DInWorld[nPtIndex].d[1];
        //double Z = 0

        double Xc, Yc, Zc;
        Xc = R[0][0] * X + R[0][1] * Y + T[0];
        Yc = R[1][0] * X + R[1][1] * Y + T[1];
        Zc = R[2][0] * X + R[2][1] * Y + T[2];

        double Rc = sqrt(Xc * Xc + Yc * Yc + Zc * Zc);
        double theta  = 0.0;

        if(Rc < std::numeric_limits<double>::epsilon())
        {
            theta = 0.0;
        }
        else
        {
            theta = acos(Zc / Rc);
        }



        double  xy_Rc = sqrt(Xc * Xc + Yc * Yc);

        double cosphi = 0.0;
        double sinphi = 0.0;
        if(xy_Rc  < std::numeric_limits<double>::epsilon())
        {
            cosphi = 1.0;
            sinphi = 0.0;
        }
        else
        {
            cosphi = Xc / xy_Rc;
            sinphi = Yc / xy_Rc;
        }

        double theta_2 = theta * theta;
        double theta_3 = theta_2 * theta;
        double theta_5 = theta_2 * theta_3;
        double theta_7 = theta_2 * theta_5;
        double theta_9 = theta_2 * theta_7;

        //对称畸变项
        double r  = k[0] * theta + k[1] * theta_3  + k[2] * theta_5 + k[3] * theta_7 + k[4] * theta_9;


        double cos2phi = cosphi*cosphi - sinphi*sinphi;
        double sin2phi = 2*sinphi*cosphi;

        //非对称畸变项
        double dr = (g[0]*theta + g[1]*theta_3 + g[2]*theta_5) * (i[0]*cosphi + i[1]*sinphi + i[2]*cos2phi + i[3]*sin2phi);
        double dt = (h[0]*theta + h[1]*theta_3 + h[2]*theta_5) * (j[0]*cosphi + j[1]*sinphi + j[2]*cos2phi + j[3]*sin2phi); 

        double x = (r + dr)*cosphi - dt*sinphi;
        double y = (r + dr)*sinphi + dt*cosphi;

        double u = mu * x + u0;
        double v = mv * y + v0;

        pPt2DInImg[nPtIndex].d[0] = u;
        pPt2DInImg[nPtIndex].d[1] = v;

    }//for

    return TRUE;
}



//@功能:根据校正得到的内部参数和外部参数，从校正平面正向投影到相机平面上
//@参数:pPt3DInWorld, 输出参数, 输入的世界坐标
//      nPtNumber, 点的个数
//      vecInternalParams, 内部参数
//      R, 旋转矩阵
//      T, 偏移矢量
//      pPt2DInImg, 输出参数, 保存图像坐标的数组
BOOL ForwardProject3D(const TPoint3D* pPt3DInWorld, int nPtNumber, const double* pInternalParams, int nParamsCount, const double (&R)[3][3], const double (&T)[3],  TPoint2D* pPt2DInImg )
{

    double k[5], g[3], i[4], h[3], j[4] ;
    double mu,mv, u0,v0;
    if(nParamsCount == CGenericCameraModel::BASIC_MODEL_INTERNAL_PARAMS_COUNT)
    {
        mu   = pInternalParams[0];
        mv   = pInternalParams[1];
        u0   = pInternalParams[2];
        v0   = pInternalParams[3];
        k[0] = pInternalParams[4];
        k[1] = pInternalParams[5];
        k[2] = 0.0;
        k[3] = 0.0;
        k[4] = 0.0;
        g[0] = 0.0;
        g[1] = 0.0;
        g[2] = 0.0;
        i[0] = 0.0;
        i[1] = 0.0;
        i[2] = 0.0;
        i[3] = 0.0;
        h[0] = 0.0;
        h[1] = 0.0;
        h[2] = 0.0;
        j[0] = 0.0;
        j[1] = 0.0;
        j[2] = 0.0;
        j[3] = 0.0;

    }
    else if(nParamsCount == CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT)
    {
        mu   = pInternalParams[0];
        mv   = pInternalParams[1];
        u0   = pInternalParams[2];
        v0   = pInternalParams[3];
        k[0] = pInternalParams[4];
        k[1] = pInternalParams[5];
        k[2] = pInternalParams[6];
        k[3] = pInternalParams[7];
        k[4] = pInternalParams[8];
        g[0] = 0.0;
        g[1] = 0.0;
        g[2] = 0.0;
        i[0] = 0.0;
        i[1] = 0.0;
        i[2] = 0.0;
        i[3] = 0.0;
        h[0] = 0.0;
        h[1] = 0.0;
        h[2] = 0.0;
        j[0] = 0.0;
        j[1] = 0.0;
        j[2] = 0.0;
        j[3] = 0.0;
    }
    else if(nParamsCount == CGenericCameraModel::EXTENDED_MODEL_INTERNAL_PARAMS_COUNT)
    {
        mu   = pInternalParams[0 ];
        mv   = pInternalParams[1 ];
        u0   = pInternalParams[2 ];
        v0   = pInternalParams[3 ];
        k[0] = pInternalParams[4 ];
        k[1] = pInternalParams[5 ];
        k[2] = pInternalParams[6 ];
        k[3] = pInternalParams[7 ];
        k[4] = pInternalParams[8 ];
        g[0] = pInternalParams[9 ];
        g[1] = pInternalParams[10];
        g[2] = pInternalParams[11];
        i[0] = pInternalParams[12];
        i[1] = pInternalParams[13];
        i[2] = pInternalParams[14];
        i[3] = pInternalParams[15];
        h[0] = pInternalParams[16];
        h[1] = pInternalParams[17];
        h[2] = pInternalParams[18];
        j[0] = pInternalParams[19];
        j[1] = pInternalParams[20];
        j[2] = pInternalParams[21];
        j[3] = pInternalParams[22];

    }
    else
    {
        return FALSE;
    }


    for(int nPtIndex = 0; nPtIndex < nPtNumber; nPtIndex++)
    {
        double X = pPt3DInWorld[nPtIndex].d[0];
        double Y = pPt3DInWorld[nPtIndex].d[1];
        double Z = pPt3DInWorld[nPtIndex].d[2];

        double Xc, Yc, Zc;
        Xc = R[0][0] * X + R[0][1] * Y + R[0][2] * Z + T[0];
        Yc = R[1][0] * X + R[1][1] * Y + R[1][2] * Z + T[1];
        Zc = R[2][0] * X + R[2][1] * Y + R[2][2] * Z + T[2];

        double Rc = sqrt(Xc * Xc + Yc * Yc + Zc * Zc);
        double theta  = 0.0;

        if(Rc < std::numeric_limits<double>::epsilon())
        {
            theta = 0.0;
        }
        else
        {
            theta = acos(Zc / Rc);
        }



        double  xy_Rc = sqrt(Xc * Xc + Yc * Yc);

        double cosphi = 0.0;
        double sinphi = 0.0;
        if(xy_Rc  < std::numeric_limits<double>::epsilon())
        {
            cosphi = 1.0;
            sinphi = 0.0;
        }
        else
        {
            cosphi = Xc / xy_Rc;
            sinphi = Yc / xy_Rc;
        }

        double theta_2 = theta * theta;
        double theta_3 = theta_2 * theta;
        double theta_5 = theta_2 * theta_3;
        double theta_7 = theta_2 * theta_5;
        double theta_9 = theta_2 * theta_7;

        //对称畸变项
        double r  = k[0] * theta + k[1] * theta_3  + k[2] * theta_5 + k[3] * theta_7 + k[4] * theta_9;


        double cos2phi = cosphi*cosphi - sinphi*sinphi;
        double sin2phi = 2*sinphi*cosphi;

        //非对称畸变项
        double dr = (g[0]*theta + g[1]*theta_3 + g[2]*theta_5) * (i[0]*cosphi + i[1]*sinphi + i[2]*cos2phi + i[3]*sin2phi);
        double dt = (h[0]*theta + h[1]*theta_3 + h[2]*theta_5) * (j[0]*cosphi + j[1]*sinphi + j[2]*cos2phi + j[3]*sin2phi); 

        double x = (r + dr)*cosphi - dt*sinphi;
        double y = (r + dr)*sinphi + dt*cosphi;

        double u = mu * x + u0;
        double v = mv * y + v0;

        pPt2DInImg[nPtIndex].d[0] = u;
        pPt2DInImg[nPtIndex].d[1] = v;

    }//for

    return TRUE;
}





enum EModelType
{
    E_MODEL_TYPE_BASIC ,//'Basic Mode'有6个内部参数(mu, mv, u0, v0, k1, k2)
    E_MODEL_TYPE_RADIAL,//'Raidal Mode'有9个内部参数(mu, mv, u0, v0, k1, k2, k3, k4, k5)
};


template <int N>
struct TSamplesOnCircle
{
	TPoint2D samples[N];
	static const int SAMPLE_COUNT = N;
};

typedef TSamplesOnCircle<16> SamplesOnCircle;

struct TProjErrCtx
{
	
	//测量数据
	std::vector<TPoint2D>             vecCircleCentersInWorld   ;//校正圆圆心的世界坐标
	std::vector<TPoint2D>             vecCircleCentroidInImage  ;//校正圆所成像的重心
	int                               radius                    ;//校正圆半径
	//模型计算数据
	std::vector<SamplesOnCircle>       vecSamplesOnCircleInWorld;//校正圆圆周上的采样点的坐标
	std::vector<SamplesOnCircle>       vecSamplesOnCircleInImage;//校正圆圆周上的采样点在相机图片上的坐标
	//避免在projerr中动态分配内存
	std::vector<TPoint2D>              vecCircleCentersInImage  ;//校正圆圆心在相机画面上所成像的坐标

	const TInternalAndSymmetricDistortParams*         pLensInternalParams       ;//指向镜头内部参数数据结构的参数
	
};


//@说明:lm迭代优化时会到的函数vecErrors[i]=Func(params,context[i])
//      该函数由于被多次调用，特别要注意在其中不要进行内存的分配和释放操作，否则严重影响执行效率。
void ProjErr(const double* pParams, int nParamsCount, void* lpCtx, std::vector<double>& vecErrs)
{

    TProjErrCtx* pProjErrCtx = reinterpret_cast< TProjErrCtx*>(lpCtx);

    int nPtNumber =  pProjErrCtx->vecCircleCentersInWorld.size();


    double R[3][3], T[3];
    double Rv[3];

    int nInternalParamsCount = 6;

    if(nParamsCount ==  CGenericCameraModel::BASIC_MODEL_INTERNAL_PARAMS_COUNT + 6)//basic model
    {

        Rv[0] = pParams[CGenericCameraModel::BASIC_MODEL_INTERNAL_PARAMS_COUNT + 0];
        Rv[1] = pParams[CGenericCameraModel::BASIC_MODEL_INTERNAL_PARAMS_COUNT + 1];
        Rv[2] = pParams[CGenericCameraModel::BASIC_MODEL_INTERNAL_PARAMS_COUNT + 2];
        T[0]  = pParams[CGenericCameraModel::BASIC_MODEL_INTERNAL_PARAMS_COUNT + 3];
        T[1]  = pParams[CGenericCameraModel::BASIC_MODEL_INTERNAL_PARAMS_COUNT + 4];
        T[2]  = pParams[CGenericCameraModel::BASIC_MODEL_INTERNAL_PARAMS_COUNT + 5];
        
        nInternalParamsCount = CGenericCameraModel::BASIC_MODEL_INTERNAL_PARAMS_COUNT;
    }
    else if(nParamsCount == CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT + 6)//radial mode
    {
        Rv[0] = pParams[CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT + 0];
        Rv[1] = pParams[CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT + 1];
        Rv[2] = pParams[CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT + 2];

        T[0] = pParams[CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT + 3];
        T[1] = pParams[CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT + 4];
        T[2] = pParams[CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT + 5];


        nInternalParamsCount = CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT;

    }
    else if(nParamsCount == CGenericCameraModel::EXTENDED_MODEL_INTERNAL_PARAMS_COUNT + 6)//extended model
    {


        Rv[0] = pParams[CGenericCameraModel::EXTENDED_MODEL_INTERNAL_PARAMS_COUNT + 0];
        Rv[1] = pParams[CGenericCameraModel::EXTENDED_MODEL_INTERNAL_PARAMS_COUNT + 1];
        Rv[2] = pParams[CGenericCameraModel::EXTENDED_MODEL_INTERNAL_PARAMS_COUNT + 2];

        T[0] = pParams[CGenericCameraModel::EXTENDED_MODEL_INTERNAL_PARAMS_COUNT + 3];
        T[1] = pParams[CGenericCameraModel::EXTENDED_MODEL_INTERNAL_PARAMS_COUNT + 4];
        T[2] = pParams[CGenericCameraModel::EXTENDED_MODEL_INTERNAL_PARAMS_COUNT + 5];


        nInternalParamsCount = CGenericCameraModel::EXTENDED_MODEL_INTERNAL_PARAMS_COUNT;
    }
    else
    {
        assert(false);
    }


    V2R(Rv, R);

	vecErrs.resize(nPtNumber * 2);
	if (pProjErrCtx->radius == 0.0)
	{
		//TPoint2D imagePt;
		ForwardProject2D(
			&pProjErrCtx->vecCircleCentersInWorld[0],
			nPtNumber,
			pParams,
			nInternalParamsCount,
			R,
			T,
			&pProjErrCtx->vecCircleCentersInImage[0]);


		for (int i = 0; i < nPtNumber; i++)
		{
			vecErrs[i * 2 + 0] = pProjErrCtx->vecCircleCentroidInImage[i].d[0] - pProjErrCtx->vecCircleCentersInImage[i].d[0];
			vecErrs[i * 2 + 1] = pProjErrCtx->vecCircleCentroidInImage[i].d[1] - pProjErrCtx->vecCircleCentersInImage[i].d[1];

		}
	}
	else
	{

		int N = _countof(pProjErrCtx->vecSamplesOnCircleInWorld[0].samples);
		for (int i = 0; i < nPtNumber; i++)
		{
			 SamplesOnCircle& samplesOnCircleInWorld = pProjErrCtx->vecSamplesOnCircleInWorld[i];
			 SamplesOnCircle& samplesOnCircleInImage = pProjErrCtx->vecSamplesOnCircleInImage[i];
			

			ForwardProject2D(
				&samplesOnCircleInWorld.samples[0],
				N,
				pParams,
				nInternalParamsCount,
				R,
				T,
				&samplesOnCircleInImage.samples[0]);

			//计算根据投影模型得出的校正圆图像的重心
			TPoint2D calculatedCentroid;
			double mx = 0.0;
			double my = 0.0;

			
			for (int j = 0; j < N; j++)
			{
				mx += samplesOnCircleInImage.samples[j].d[0];
				my += samplesOnCircleInImage.samples[j].d[1];
			}

			calculatedCentroid.d[0] = mx / double(N);
			calculatedCentroid.d[1] = my / double(N);

			vecErrs[i * 2 + 0] = pProjErrCtx->vecCircleCentroidInImage[i].d[0] - calculatedCentroid.d[0];
			vecErrs[i * 2 + 1] = pProjErrCtx->vecCircleCentroidInImage[i].d[1] - calculatedCentroid.d[1];

		}//for-each(i)

	}//else
	
}



//@功能:通过最小化"投影误差"，来跌代优化相机的内部参数和外部参数
BOOL MinimizeProjErr(const TPoint2D* pPtsInWorld, const TPoint2D* pPtsInImg, int nPtNumber, int radius, double* pParams, int nParamCount)
{

    TProjErrCtx projErrCtx;

    projErrCtx.vecCircleCentersInWorld.resize(nPtNumber);
    projErrCtx.vecCircleCentroidInImage.resize(nPtNumber);

    for(int i = 0; i < nPtNumber; i++)
    {
		projErrCtx.vecCircleCentersInWorld[i] = pPtsInWorld[i];
        projErrCtx.vecCircleCentroidInImage[i] = pPtsInImg  [i];
        
    }

	projErrCtx.radius = radius;

	int N = SamplesOnCircle::SAMPLE_COUNT;

	std::vector<TPoint2D> vecPointSamplesOnCircle;
	vecPointSamplesOnCircle.resize(N);

	double theta = 0.0;
	double theta_step = M_PI * 2.0 / (double)N;

	//计算在圆周上采样的N个点的坐标
	for (int j = 0; j < N; j++)
	{
		vecPointSamplesOnCircle[j].d[0] = cos(theta) * projErrCtx.radius;
		vecPointSamplesOnCircle[j].d[1] = sin(theta) * projErrCtx.radius;
		theta += theta_step;
	}//for-each



	projErrCtx.vecSamplesOnCircleInWorld.resize(nPtNumber);

	for (int i = 0; i < nPtNumber; i++)
	{
		for (int j = 0; j < N; j++)
		{
			projErrCtx.vecSamplesOnCircleInWorld[i].samples[j].d[0] = vecPointSamplesOnCircle[j].d[0] + projErrCtx.vecCircleCentersInWorld[i].d[0];
			projErrCtx.vecSamplesOnCircleInWorld[i].samples[j].d[1] = vecPointSamplesOnCircle[j].d[1] + projErrCtx.vecCircleCentersInWorld[i].d[1];
		}//for-each(j)

	}
	projErrCtx.vecSamplesOnCircleInImage.resize(nPtNumber);
	projErrCtx.vecCircleCentersInImage.resize(nPtNumber);

    int maxIter    = 1000;
    double tolX    = 1e-6;//tolerance of variables
    double tolFun  = 1e-6;//tolerance of function
    levenbergMarquardt(pParams, nParamCount, ProjErr, (void*)&projErrCtx, maxIter, tolFun, tolX);


    return TRUE;
}


//@说明:lm迭代优化时会到的函数vecErrors[i]=Func(params,context[i])
//      该函数由于被多次调用，特别要注意在其中不要进行内存的分配和释放操作，否则严重影响执行效率。
//      在已知相机内部参数的情形下，计算正向投影误差
void ProjErrWithKnownInternalParams(
	const double* pParams, 
	int nParamsCount, 
	void* lpCtx, 
	std::vector<double>& vecErrs)
{

	TProjErrCtx* pProjErrCtx = reinterpret_cast< TProjErrCtx*>(lpCtx);

	int nPtNumber = pProjErrCtx->vecCircleCentersInWorld.size();


	double R[3][3], T[3];
	double Rv[3];
	double localParams[23];

	int nInternalParamsCount = 9;
	localParams[0] = pProjErrCtx->pLensInternalParams->mu;
	localParams[1] = pProjErrCtx->pLensInternalParams->mv;
	localParams[2] = pProjErrCtx->pLensInternalParams->u0;
	localParams[3] = pProjErrCtx->pLensInternalParams->v0;
	localParams[4] = pProjErrCtx->pLensInternalParams->radialDistrt[0];
	localParams[5] = pProjErrCtx->pLensInternalParams->radialDistrt[1];
	localParams[6] = pProjErrCtx->pLensInternalParams->radialDistrt[2];
	localParams[7] = pProjErrCtx->pLensInternalParams->radialDistrt[3];
	localParams[8] = pProjErrCtx->pLensInternalParams->radialDistrt[4];



	if (nParamsCount == 6)//radial mode
	{//pParams指向的数组只包含6个外部参数
		Rv[0] = pParams[0];
		Rv[1] = pParams[1];
		Rv[2] = pParams[2];

		T[0] = pParams[3];
		T[1] = pParams[4];
		T[2] = pParams[5];

		nInternalParamsCount = CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT;

	}
	else if (nParamsCount == 8)
	{   //优化相机镜头光心和镜头外部参数
		localParams[2] = pParams[0];
		localParams[3] = pParams[1];

		Rv[0] = pParams[2];
		Rv[1] = pParams[3];
		Rv[2] = pParams[4];

		T[0] = pParams[5];
		T[1] = pParams[6];
		T[2] = pParams[7];

		nInternalParamsCount = CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT;
	}
	else if (nParamsCount == 14 + 6)//extended model
	{//pParams指向的数组包含6个外部参数和14个非对称畸变参数


		Rv[0] = pParams[0];
		Rv[1] = pParams[1];
		Rv[2] = pParams[2];

		T[0] = pParams[3];
		T[1] = pParams[4];
		T[2] = pParams[5];

		for (int i = 0; i < 14; i++)
		{
			localParams[9 + i] = pParams[6 + i];			
		}

		nInternalParamsCount = CGenericCameraModel::EXTENDED_MODEL_INTERNAL_PARAMS_COUNT;
	}
	//else if (nParamsCount == 2 + 14 + 6)
	//{
	//	localParams[2] = pParams[0];
	//	localParams[3] = pParams[1];

	//	Rv[0] = pParams[2];
	//	Rv[1] = pParams[3];
	//	Rv[2] = pParams[4];

	//	T[0] = pParams[5];
	//	T[1] = pParams[6];
	//	T[2] = pParams[7];

	//	for (int i = 0; i < 14; i++)
	//	{
	//		localParams[9 + i] = pParams[6 + i];
	//	}

	//	nInternalParamsCount = CGenericCameraModel::EXTENDED_MODEL_INTERNAL_PARAMS_COUNT;


	//}
	else
	{
		assert(false);
	}


	V2R(Rv, R);

	vecErrs.resize(nPtNumber * 2);
	if (pProjErrCtx->radius == 0.0)
	{
		//TPoint2D imagePt;
		ForwardProject2D(
			&pProjErrCtx->vecCircleCentersInWorld[0],
			nPtNumber,
			localParams,
			nInternalParamsCount,
			R,
			T,
			&pProjErrCtx->vecCircleCentersInImage[0]);


		for (int i = 0; i < nPtNumber; i++)
		{
			vecErrs[i * 2 + 0] = pProjErrCtx->vecCircleCentroidInImage[i].d[0] - pProjErrCtx->vecCircleCentersInImage[i].d[0];
			vecErrs[i * 2 + 1] = pProjErrCtx->vecCircleCentroidInImage[i].d[1] - pProjErrCtx->vecCircleCentersInImage[i].d[1];

		}
	}
	else
	{

		int N = _countof(pProjErrCtx->vecSamplesOnCircleInWorld[0].samples);
		for (int i = 0; i < nPtNumber; i++)
		{
			SamplesOnCircle& samplesOnCircleInWorld = pProjErrCtx->vecSamplesOnCircleInWorld[i];
			SamplesOnCircle& samplesOnCircleInImage = pProjErrCtx->vecSamplesOnCircleInImage[i];


			ForwardProject2D(
				&samplesOnCircleInWorld.samples[0],
				N,
				localParams,
				nInternalParamsCount,
				R,
				T,
				&samplesOnCircleInImage.samples[0]);

			//计算根据投影模型得出的校正圆图像的重心
			TPoint2D calculatedCentroid;
			double mx = 0.0;
			double my = 0.0;


			for (int j = 0; j < N; j++)
			{
				mx += samplesOnCircleInImage.samples[j].d[0];
				my += samplesOnCircleInImage.samples[j].d[1];
			}

			calculatedCentroid.d[0] = mx / double(N);
			calculatedCentroid.d[1] = my / double(N);

			vecErrs[i * 2 + 0] = pProjErrCtx->vecCircleCentroidInImage[i].d[0] - calculatedCentroid.d[0];
			vecErrs[i * 2 + 1] = pProjErrCtx->vecCircleCentroidInImage[i].d[1] - calculatedCentroid.d[1];

		}//for-each(i)

	}//else

}



//@说明:lm迭代优化相机光心位置
void ProjErrForOptimizeOpticalCenter(
	const double* pParams, 
	int nParamsCount, 
	void* lpCtx, 
	std::vector<double>& vecErrs)
{

	TProjErrCtx* pProjErrCtx = reinterpret_cast< TProjErrCtx*>(lpCtx);

	int nPtNumber = pProjErrCtx->vecCircleCentersInWorld.size();


	double R[3][3], T[3];
	double Rv[3];
	double localParams[23];

	int nInternalParamsCount = 9;
	localParams[0] = pProjErrCtx->pLensInternalParams->mu;
	localParams[1] = pProjErrCtx->pLensInternalParams->mv;
	localParams[2] = pProjErrCtx->pLensInternalParams->u0;
	localParams[3] = pProjErrCtx->pLensInternalParams->v0;
	localParams[4] = pProjErrCtx->pLensInternalParams->radialDistrt[0];
	localParams[5] = pProjErrCtx->pLensInternalParams->radialDistrt[1];
	localParams[6] = pProjErrCtx->pLensInternalParams->radialDistrt[2];
	localParams[7] = pProjErrCtx->pLensInternalParams->radialDistrt[3];
	localParams[8] = pProjErrCtx->pLensInternalParams->radialDistrt[4];

	if (nParamsCount == 6)//radial mode
	{//pParams指向的数组只包含6个外部参数
		Rv[0] = pParams[0];
		Rv[1] = pParams[1];
		Rv[2] = pParams[2];

		T[0] = pParams[3];
		T[1] = pParams[4];
		T[2] = pParams[5];

		nInternalParamsCount = CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT;

	}
	else if (nParamsCount == 8)
	{   //优化相机镜头光心和镜头外部参数
		localParams[2] = pParams[0];//u0
		localParams[3] = pParams[1];//v0

		Rv[0] = pParams[2];
		Rv[1] = pParams[3];
		Rv[2] = pParams[4];

		T[0] = pParams[5];
		T[1] = pParams[6];
		T[2] = pParams[7];

		nInternalParamsCount = CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT;
	}
	else if (nParamsCount == 10)
	{   //优化相机镜头尺度因子, 光心坐标u0,v0,和镜头外部参数
		localParams[0] = pParams[0];//mu
		localParams[1] = pParams[1];//mv
		localParams[2] = pParams[2];//u0
		localParams[3] = pParams[3];//v0

		Rv[0] = pParams[4];
		Rv[1] = pParams[5];
		Rv[2] = pParams[6];

		T[0] = pParams[7];
		T[1] = pParams[8];
		T[2] = pParams[9];

		nInternalParamsCount = CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT;
	}
	else if (nParamsCount == 14 + 6)//extended model
	{//pParams指向的数组包含6个外部参数和14个非对称畸变参数


		Rv[0] = pParams[0];
		Rv[1] = pParams[1];
		Rv[2] = pParams[2];

		T[0] = pParams[3];
		T[1] = pParams[4];
		T[2] = pParams[5];

		for (int i = 0; i < 14; i++)
		{
			localParams[9 + i] = pParams[6 + i];			
		}

		nInternalParamsCount = CGenericCameraModel::EXTENDED_MODEL_INTERNAL_PARAMS_COUNT;
	}
	else
	{
		assert(false);
	}


	V2R(Rv, R);

	vecErrs.resize(nPtNumber * 2);
	if (pProjErrCtx->radius == 0.0)
	{
		//TPoint2D imagePt;
		ForwardProject2D(
			&pProjErrCtx->vecCircleCentersInWorld[0],
			nPtNumber,
			localParams,
			nInternalParamsCount,
			R,
			T,
			&pProjErrCtx->vecCircleCentersInImage[0]);


		for (int i = 0; i < nPtNumber; i++)
		{
			vecErrs[i * 2 + 0] = pProjErrCtx->vecCircleCentroidInImage[i].d[0] - pProjErrCtx->vecCircleCentersInImage[i].d[0];
			vecErrs[i * 2 + 1] = pProjErrCtx->vecCircleCentroidInImage[i].d[1] - pProjErrCtx->vecCircleCentersInImage[i].d[1];

		}
	}
	else
	{

		int N = _countof(pProjErrCtx->vecSamplesOnCircleInWorld[0].samples);
		for (int i = 0; i < nPtNumber; i++)
		{
			SamplesOnCircle& samplesOnCircleInWorld = pProjErrCtx->vecSamplesOnCircleInWorld[i];
			SamplesOnCircle& samplesOnCircleInImage = pProjErrCtx->vecSamplesOnCircleInImage[i];


			ForwardProject2D(
				&samplesOnCircleInWorld.samples[0],
				N,
				localParams,
				nInternalParamsCount,
				R,
				T,
				&samplesOnCircleInImage.samples[0]);

			//计算根据投影模型得出的校正圆图像的重心
			TPoint2D calculatedCentroid;
			double mx = 0.0;
			double my = 0.0;


			for (int j = 0; j < N; j++)
			{
				mx += samplesOnCircleInImage.samples[j].d[0];
				my += samplesOnCircleInImage.samples[j].d[1];
			}

			calculatedCentroid.d[0] = mx / double(N);
			calculatedCentroid.d[1] = my / double(N);

			vecErrs[i * 2 + 0] = pProjErrCtx->vecCircleCentroidInImage[i].d[0] - calculatedCentroid.d[0];
			vecErrs[i * 2 + 1] = pProjErrCtx->vecCircleCentroidInImage[i].d[1] - calculatedCentroid.d[1];

		}//for-each(i)

	}//else

}

//@功能:在已知内部参数和对称畸变参数的情况下,最优化迭代求出相机的光心坐标
BOOL MinimizeProjErrForOpticalCenter(
	const TPoint2D* pPtsInWorld,
	const TPoint2D* pPtsInImg,
	int nPtNumber,
	int radius,
	double* pParams,
	int nParamCount,
	const TInternalAndSymmetricDistortParams* pLensInternalParams)
{

	TProjErrCtx projErrCtx;


	projErrCtx.vecCircleCentersInWorld.resize(nPtNumber);
	projErrCtx.vecCircleCentroidInImage.resize(nPtNumber);

	for (int i = 0; i < nPtNumber; i++)
	{
		projErrCtx.vecCircleCentersInWorld[i] = pPtsInWorld[i];
		projErrCtx.vecCircleCentroidInImage[i] = pPtsInImg[i];

	}

	projErrCtx.radius = radius;
	projErrCtx.pLensInternalParams = pLensInternalParams;

	int N = SamplesOnCircle::SAMPLE_COUNT;

	std::vector<TPoint2D> vecPointSamplesOnCircle;
	vecPointSamplesOnCircle.resize(N);

	double theta = 0.0;
	double theta_step = M_PI * 2.0 / (double)N;

	//计算在圆周上采样的N个点的坐标
	for (int j = 0; j < N; j++)
	{
		vecPointSamplesOnCircle[j].d[0] = cos(theta) * projErrCtx.radius;
		vecPointSamplesOnCircle[j].d[1] = sin(theta) * projErrCtx.radius;
		theta += theta_step;
	}//for-each

	projErrCtx.vecSamplesOnCircleInWorld.resize(nPtNumber);

	for (int i = 0; i < nPtNumber; i++)
	{
		for (int j = 0; j < N; j++)
		{
			projErrCtx.vecSamplesOnCircleInWorld[i].samples[j].d[0] = vecPointSamplesOnCircle[j].d[0] + projErrCtx.vecCircleCentersInWorld[i].d[0];
			projErrCtx.vecSamplesOnCircleInWorld[i].samples[j].d[1] = vecPointSamplesOnCircle[j].d[1] + projErrCtx.vecCircleCentersInWorld[i].d[1];
		}//for-each(j)

	}

	projErrCtx.vecSamplesOnCircleInImage.resize(nPtNumber);
	projErrCtx.vecCircleCentersInImage.resize(nPtNumber);

	int maxIter = 1000;
	double tolX = 1e-6;//tolerance of variables
	double tolFun = 1e-6;//tolerance of function

	//在对称畸变模型下估计相机光心坐标和外部参数
	levenbergMarquardt(pParams, nParamCount, ProjErrForOptimizeOpticalCenter, (void*)&projErrCtx, maxIter, tolFun, tolX);


	return TRUE;
}


//@功能:在已知内部参数和对称畸变参数的情况下,通过最小化"投影误差"，来跌代优化相机的非对称畸变参数和外部参数
BOOL MinimizeProjErrWithKnownInternalParams(
	const TPoint2D* pPtsInWorld, 
	const TPoint2D* pPtsInImg,
	int nPtNumber, 
	int radius, 
	double* pParams, 
	int nParamCount, 
	const TInternalAndSymmetricDistortParams* pLensInternalParams)
{

	TProjErrCtx projErrCtx;

	TInternalAndSymmetricDistortParams lensInternalParams = *pLensInternalParams;
	projErrCtx.vecCircleCentersInWorld.resize(nPtNumber);
	projErrCtx.vecCircleCentroidInImage.resize(nPtNumber);

	for (int i = 0; i < nPtNumber; i++)
	{
		projErrCtx.vecCircleCentersInWorld[i] = pPtsInWorld[i];
		projErrCtx.vecCircleCentroidInImage[i] = pPtsInImg[i];

	}

	projErrCtx.radius = radius;
	projErrCtx.pLensInternalParams = &lensInternalParams;

	int N = SamplesOnCircle::SAMPLE_COUNT;

	std::vector<TPoint2D> vecPointSamplesOnCircle;
	vecPointSamplesOnCircle.resize(N);

	double theta = 0.0;
	double theta_step = M_PI * 2.0 / (double)N;

	//计算在圆周上采样的N个点的坐标
	for (int j = 0; j < N; j++)
	{
		vecPointSamplesOnCircle[j].d[0] = cos(theta) * projErrCtx.radius;
		vecPointSamplesOnCircle[j].d[1] = sin(theta) * projErrCtx.radius;
		theta += theta_step;
	}//for-each

	projErrCtx.vecSamplesOnCircleInWorld.resize(nPtNumber);

	for (int i = 0; i < nPtNumber; i++)
	{
		for (int j = 0; j < N; j++)
		{
			projErrCtx.vecSamplesOnCircleInWorld[i].samples[j].d[0] = vecPointSamplesOnCircle[j].d[0] + projErrCtx.vecCircleCentersInWorld[i].d[0];
			projErrCtx.vecSamplesOnCircleInWorld[i].samples[j].d[1] = vecPointSamplesOnCircle[j].d[1] + projErrCtx.vecCircleCentersInWorld[i].d[1];
		}//for-each(j)

	}

	projErrCtx.vecSamplesOnCircleInImage.resize(nPtNumber);
	projErrCtx.vecCircleCentersInImage.resize(nPtNumber);

	int maxIter = 1000;
	double tolX = 1e-6;//tolerance of variables
	double tolFun = 1e-6;//tolerance of function
		
	//优化非对称畸变参数和外部参数	
	levenbergMarquardt(pParams, nParamCount, ProjErrWithKnownInternalParams, (void*)&projErrCtx, maxIter, tolFun, tolX);
	
	return TRUE;
}


//检查正向映射的精度
void CheckForwardProjectPrecision(const TPoint2D* pPtsInImg, const TPoint2D* pPtsInWorld, int nPtNumber, const double* pParams, int nParamCount, const double (&R)[3][3], const double (&T)[3])
{

    std::vector<TPoint2D> vecPtForwardProject;
    vecPtForwardProject.resize(nPtNumber);

    ForwardProject2D(
        pPtsInWorld,
        nPtNumber,
        pParams,
        nParamCount,
        R,
        T,
        &vecPtForwardProject[0]);


    std::vector <TPoint2D> vecError;
    double max_dx = (std::numeric_limits<double>::min)();
    double max_dy = (std::numeric_limits<double>::min)();
    double sum_dx = 0.0;
    double sum_dy = 0.0;
    for(int i = 0; i < nPtNumber; i++)
    {
        double dx = vecPtForwardProject[i].d[0] -  pPtsInImg[i].d[0];
        double dy = vecPtForwardProject[i].d[1] -  pPtsInImg[i].d[1];

        if(fabs(dx) > fabs(max_dx))
        {
            max_dx = dx;
        }

        if(fabs(dy) > fabs(max_dy))
        {
            max_dy = dy;
        }

        sum_dx += dx*dx;
        sum_dy += dy*dy;
    }

    double std_dx = sqrt(sum_dx/nPtNumber);
    double std_dy = sqrt(sum_dy/nPtNumber);


    GCM_Debug(_T("======\nForward Project Result:\nmax dx=%g\nmax dy=%g\nstd_dx=%g\nstd_dy=%g\n"), max_dx, max_dy, std_dx, std_dy);


}

//检查逆向映射的精度
void CheckBackwardProjectPrecision(const TPoint2D* pPtsInImg, const TPoint2D* pPtsInWorld, int nPtNumber, double* pParams, int nParamCount, double thetaMax, const double (&InvR)[3][3], const double (&T)[3])
{

    std::vector<TPoint2D> vecPtBackwardProject;
    vecPtBackwardProject.resize(nPtNumber);


    BackwardProject(
        pPtsInImg,
        nPtNumber,
        pParams,
        nParamCount,
        thetaMax,
        InvR,
        T,
        &vecPtBackwardProject[0]);


    std::vector <TPoint2D> vecError;
    double max_dx = (std::numeric_limits<double>::min)();
    double max_dy = (std::numeric_limits<double>::min)();
    double sum_dx = 0.0;
    double sum_dy = 0.0;
    for(int i = 0; i < nPtNumber; i++)
    {
        double dx = vecPtBackwardProject[i].d[0] -  pPtsInWorld[i].d[0];
        double dy = vecPtBackwardProject[i].d[1] -  pPtsInWorld[i].d[1];

        if(fabs(dx) > fabs(max_dx))
        {
            max_dx = dx;
        }

        if(fabs(dy) > fabs(max_dy))
        {
            max_dy = dy;
        }

        sum_dx += dx*dx;
        sum_dy += dy*dy;
    }

    double std_dx = sqrt(sum_dx/nPtNumber);
    double std_dy = sqrt(sum_dy/nPtNumber);


    GCM_Debug(_T("======\nBackward Project Result:\nmax dx=%g\nmax dy=%g\nstd_dx=%g\nstd_dy=%g\n"), max_dx, max_dy, std_dx, std_dy);

}

void SaveCameraModelParams(const CGenericCameraModel& model, const char* lpszFileName)
{
	char text[1024];
	FILE* file;

	if (NULL == lpszFileName)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		sprintf_s(
			text,
			_countof(text),
			"CalibrateParam(%04d%02d%02d-%02d%02d%02d-%03d).txt",
			sysTime.wYear,
			sysTime.wMonth,
			sysTime.wDay,
			sysTime.wHour,
			sysTime.wMinute,
			sysTime.wSecond,
			sysTime.wMilliseconds);
	}
	else
	{
		strcpy_s(text, _countof(text), lpszFileName);
	}

	errno_t err = fopen_s(&file, text, "w");
	if (err == 0)
	{
		int nLen = 0;

		//输出镜头内部参数
		//注释
		strcpy_s(text, _countof(text), "#Internal Parameters:\n");
		fwrite(text, 1, strlen(text), file);

		nLen = sprintf_s(
			text,
			_countof(text), 
			"mu = %g\nmv = %g\nu0 = %g\nv0 = %g\nk1 = %g\nk2 = %g\nk3 = %g\nk4 = %g\nk5 = %g\n",
			model.m_mu,
			model.m_mv,
			model.m_u0,
			model.m_v0,
			model.m_k1,
			model.m_k2,
			model.m_k3,
			model.m_k4,
			model.m_k5);
		fwrite(text, 1, nLen, file);


		nLen = 	sprintf_s(
			text,
			_countof(text),
			"g1 = %g\ng2 = %g\ng3 = %g\ni1 = %g\ni2 = %g\ni3 = %g\ni4 = %g\n",
			model.m_g1,
			model.m_g2,
			model.m_g3,
			model.m_i1,
			model.m_i2,
			model.m_i3,
			model.m_i4);
		fwrite(text, 1, nLen, file);


		nLen =	sprintf_s(
				text,
				_countof(text),
				"h1 = %g\nh2 = %g\nh3 = %g\nj1 = %g\nj2 = %g\nj3 = %g\nj4 = %g\n",
				model.m_g1,
				model.m_g2,
				model.m_g3,
				model.m_i1,
				model.m_i2,
				model.m_i3,
				model.m_i4);
		fwrite(text, 1, nLen, file);

		
		//输出外部参数
		strcpy_s(text, _countof(text), "#Rotation Matrix:\n");
		fwrite(text, 1, strlen(text), file);

		nLen = sprintf_s(
			text,
			_countof(text),
			"=====\nRotation Matrix:\n%12.6g\t%12.6g\t%12.6g\n%12.6g\t%12.6g\t%12.6g\n%12.6g\t%12.6g\t%12.6g\n",
			model.m_Rs[0][0],
			model.m_Rs[0][1],
			model.m_Rs[0][2],
			model.m_Rs[1][0],
			model.m_Rs[1][1],
			model.m_Rs[1][2],
			model.m_Rs[2][0],
			model.m_Rs[2][1],
			model.m_Rs[2][2]);
		fwrite(text, 1, strlen(text), file);
		
		
		strcpy_s(text, _countof(text), "#Translation:\n");
		fwrite(text, 1, strlen(text), file);

		nLen = sprintf_s(
			text,
			_countof(text),
			"%12.6g\n%12.6g\n%12.6g\n",
			model.m_ts[0],
			model.m_ts[1],
			model.m_ts[2]);

		fwrite(text, 1, strlen(text), file);

		fclose(file);

	}//if(err == 0)
}

void SaveCalibrateData(
	EGenericCameraProjectionType eProjectType,
	double dbThrowRatio,
	const TPoint2D* pPtsInWorld,
	const TPoint2D* pPt2DInImg,
	int nPtNumber,
	const RECT& rcMonitor,
	int radius,
	int nCxImg,
	int nCyImg)
{

	char text[1024];
	FILE* file;

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	sprintf_s(
		text,
		_countof(text),
		"CalibrateData(%04d%02d%02d-%02d%02d%02d-%03d).txt",
		sysTime.wYear,
		sysTime.wMonth,
		sysTime.wDay,
		sysTime.wHour,
		sysTime.wMinute,
		sysTime.wSecond,
		sysTime.wMilliseconds);


	errno_t err = fopen_s(&file, text, "w");
	if (err == 0)
	{

		int nLen = 0;

		//镜头透射比		
		//注释
		strcpy_s(text, _countof(text), "#lens throw ratio\n");
		fwrite(text, 1, strlen(text), file);
		//
		nLen = sprintf_s(
			text,
			_countof(text),
			"%.2f\n",
			dbThrowRatio
			);
		fwrite(text, 1, nLen, file);


		//图像的宽度和高度
		//注释
		strcpy_s(text, _countof(text), "#image width and height\n");
		fwrite(text, 1, strlen(text), file);
		//
		nLen = sprintf_s(
			text,
			_countof(text),
			"%d,%d\n",
			nCxImg,
			nCyImg);
		fwrite(text, 1, nLen, file);


		//屏幕矩形区域
		//注释
		strcpy_s(text, _countof(text), "#screen dimension in pixel\n");
		fwrite(text, 1, strlen(text), file);

		nLen = sprintf_s(
			text,
			_countof(text),
			"%d,%d, %d, %d \n",
			rcMonitor.left,
			rcMonitor.top,
			rcMonitor.right,
			rcMonitor.bottom);

		fwrite(text, 1, nLen, file);

		//校正圆半径
		strcpy_s(text, _countof(text), "#circle radius in pixel\n");
		fwrite(text, 1, strlen(text), file);

		nLen = sprintf_s(
			text,
			_countof(text),
			"%d\n",
			radius);

		fwrite(text, 1, nLen, file);

		//校正点数据
		strcpy_s(text, _countof(text), "#calibrate points(u,v,x,y)\n");
		fwrite(text, 1, strlen(text), file);
		for (int i = 0; i < nPtNumber; i++)
		{
			

			nLen =
				sprintf_s(
					text,
					_countof(text),
					"%lf,%lf,%d,%d\n",
					pPt2DInImg[i].d[0],
					pPt2DInImg[i].d[1],
					int(pPtsInWorld[i].d[0]),
					int(pPtsInWorld[i].d[1]));
			fwrite(text, 1, nLen, file);
		};

		fclose(file);
	}

	
}
//debug>>

CGenericCameraModel::CGenericCameraModel()
:
m_mu(m_Parameters[INTERNAL_PARAMS_OFFSET + 0]),
m_mv(m_Parameters[INTERNAL_PARAMS_OFFSET + 1]),
m_u0(m_Parameters[INTERNAL_PARAMS_OFFSET + 2]),
m_v0(m_Parameters[INTERNAL_PARAMS_OFFSET + 3]),
m_k1(m_Parameters[SYMMETRIC_DISTORTION_PARAMS_OFFFSET + 0]),
m_k2(m_Parameters[SYMMETRIC_DISTORTION_PARAMS_OFFFSET + 1]),
m_k3(m_Parameters[SYMMETRIC_DISTORTION_PARAMS_OFFFSET + 2]),
m_k4(m_Parameters[SYMMETRIC_DISTORTION_PARAMS_OFFFSET + 3]),
m_k5(m_Parameters[SYMMETRIC_DISTORTION_PARAMS_OFFFSET + 4]),
m_k (*((double (*)[5])&m_Parameters[SYMMETRIC_DISTORTION_PARAMS_OFFFSET])),
m_g1(m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_G_OFFSET + 0]),
m_g2(m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_G_OFFSET + 1]),
m_g3(m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_G_OFFSET + 2]),
m_g (*((double (*)[3])&m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_G_OFFSET])),
m_i1(m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_I_OFFSET + 0]),
m_i2(m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_I_OFFSET + 1]),
m_i3(m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_I_OFFSET + 2]),
m_i4(m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_I_OFFSET + 3]),
m_i (*((double (*)[4])&m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_I_OFFSET])),
m_h1(m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_H_OFFSET + 0]),
m_h2(m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_H_OFFSET + 1]),
m_h3(m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_H_OFFSET + 2]),
m_h (*((double (*)[3])&m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_H_OFFSET])),
m_j1(m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_J_OFFSET + 0]),
m_j2(m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_J_OFFSET + 1]),
m_j3(m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_J_OFFSET + 2]),
m_j4(m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_J_OFFSET + 3]),
m_j (*((double (*)[4])&m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_J_OFFSET])),
m_Rs(*((double(*)[3][3])&m_Parameters[EXTERNAL_PARAMS_ROTATION_MAXTRIX_OFFSET])),
m_ts(*((double(*)[3])&m_Parameters[EXTERNAL_PARAMS_TRANSLATION_OFFSET])),
m_thetaMax(m_Parameters[AUX_PARAM_THETA_MAX_OFFSET]),
m_throwRatio(m_Parameters[THROW_RATIO_OFFSET]),
m_cxImage(m_Parameters[IMAGE_CX_OFFSET]),
m_cyImage(m_Parameters[IMAGE_CY_OFFSET]),
m_Monitor_Left  (m_Parameters[MONITOR_LEFT_OFFSET ]),
m_Monitor_Right (m_Parameters[MONITOR_RIGHT_OFFSET]),
m_Monitor_Top   (m_Parameters[MONITOR_TOP_OFFSET  ]),
m_Monitor_Bottom(m_Parameters[MONITOR_BOTTOM_OFFSET]),
m_bDebug(FALSE)
{
    //memset(m_InvRs, 0, sizeof(m_InvRs));
    memset(m_Parameters, 0, sizeof(m_Parameters));

	memset(m_RPhi2ThetaTables, 0, sizeof(m_RPhi2ThetaTables));
}

//拷贝构造函数
//std::vector<CGenericCameraModel>中要用到
CGenericCameraModel::CGenericCameraModel(const CGenericCameraModel& clone)
:
m_mu(m_Parameters[INTERNAL_PARAMS_OFFSET + 0]),
m_mv(m_Parameters[INTERNAL_PARAMS_OFFSET + 1]),
m_u0(m_Parameters[INTERNAL_PARAMS_OFFSET + 2]),
m_v0(m_Parameters[INTERNAL_PARAMS_OFFSET + 3]),
m_k1(m_Parameters[SYMMETRIC_DISTORTION_PARAMS_OFFFSET + 0]),
m_k2(m_Parameters[SYMMETRIC_DISTORTION_PARAMS_OFFFSET + 1]),
m_k3(m_Parameters[SYMMETRIC_DISTORTION_PARAMS_OFFFSET + 2]),
m_k4(m_Parameters[SYMMETRIC_DISTORTION_PARAMS_OFFFSET + 3]),
m_k5(m_Parameters[SYMMETRIC_DISTORTION_PARAMS_OFFFSET + 4]),
m_k (*((double (*)[5])&m_Parameters[SYMMETRIC_DISTORTION_PARAMS_OFFFSET])),
m_g1(m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_G_OFFSET + 0]),
m_g2(m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_G_OFFSET + 1]),
m_g3(m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_G_OFFSET + 2]),
m_g (*((double (*)[3])&m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_G_OFFSET])),
m_i1(m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_I_OFFSET + 0]),
m_i2(m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_I_OFFSET + 1]),
m_i3(m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_I_OFFSET + 2]),
m_i4(m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_I_OFFSET + 3]),
m_i (*((double (*)[4])&m_Parameters[UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_I_OFFSET])),
m_h1(m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_H_OFFSET + 0]),
m_h2(m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_H_OFFSET + 1]),
m_h3(m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_H_OFFSET + 2]),
m_h (*((double (*)[3])&m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_H_OFFSET])),
m_j1(m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_J_OFFSET + 0]),
m_j2(m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_J_OFFSET + 1]),
m_j3(m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_J_OFFSET + 2]),
m_j4(m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_J_OFFSET + 3]),
m_j (*((double (*)[4])&m_Parameters[UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_J_OFFSET])),
m_Rs(*((double(*)[3][3])&m_Parameters[EXTERNAL_PARAMS_ROTATION_MAXTRIX_OFFSET])),
m_ts(*((double(*)[3])&m_Parameters[EXTERNAL_PARAMS_TRANSLATION_OFFSET])),
m_thetaMax(m_Parameters[AUX_PARAM_THETA_MAX_OFFSET]),
m_throwRatio(m_Parameters[THROW_RATIO_OFFSET]),
m_cxImage(m_Parameters[IMAGE_CX_OFFSET]),
m_cyImage(m_Parameters[IMAGE_CY_OFFSET]),
m_Monitor_Left  (m_Parameters[MONITOR_LEFT_OFFSET ]),
m_Monitor_Right (m_Parameters[MONITOR_RIGHT_OFFSET]),
m_Monitor_Top   (m_Parameters[MONITOR_TOP_OFFSET  ]),
m_Monitor_Bottom(m_Parameters[MONITOR_BOTTOM_OFFSET]),
m_bDebug(FALSE)
{

    memcpy(m_Parameters, clone.m_Parameters, sizeof(m_Parameters));

	memset(m_RPhi2ThetaTables, 0, sizeof(m_RPhi2ThetaTables));


    memcpy(m_InvRs, clone.m_InvRs, sizeof(m_InvRs));
    m_ptCameraLocation = clone.m_ptCameraLocation;
    m_ptOZFootImage    = clone.m_ptOZFootImage;


}


CGenericCameraModel::~CGenericCameraModel()
{
	for(int i = 0; i < PHI_DIVISION; i++)
	{
		if(m_RPhi2ThetaTables[i].r2ThetaLUT)
		{
			delete [] m_RPhi2ThetaTables[i].r2ThetaLUT;
			 m_RPhi2ThetaTables[i].r2ThetaLUT = NULL;
			 m_RPhi2ThetaTables[i].nLUTSize   =  0;
		}//if		
	}//for

}


//@功 能:根据相机的针孔模型，对相机进行校正,计算出相机的内部参数和外部参数
//@参 数:eProjectType, 投影类型
//       dbThrowRatio, 投射比,对透视相机有效, 对鱼眼镜头,该参数无效,则设为为0.0.
//       pPtsInWorld, 在世界坐标系中的校正点坐标
//       pPt2DInImg, 在图像中对应的每个校正点的坐标 
//       nPtNumber, 校正点个数
//      rcMonitor, 屏幕区域
//      radius, 校正圆半径
//      nCxImg, 图片宽度
//      nCyImg, 图片高度
//     pInternalParams,相机内部参数
//@返回值:成功, TRUE
//        失败, FALSE
BOOL CGenericCameraModel::Calibrate(
	EGenericCameraProjectionType eProjectType,
	double dbThrowRatio,
	const TPoint2D* pPtsInWorld,
	const TPoint2D* pPt2DInImg,
	int nPtNumber,
	const RECT& rcMonitor,
	int radius,
	int nCxImg,
	int nCyImg,
	const TInternalAndSymmetricDistortParams* pInternalAndSymmetricDistortionParams)
{
	BOOL bInternalParamsIsKnown = (NULL != pInternalAndSymmetricDistortionParams) ? TRUE : FALSE;
	double Hs[3][3];
	TInternalAndSymmetricDistortParams internalAndSymmetricDistortParams;
	if (bInternalParamsIsKnown)
	{//相机内部参数已知
		internalAndSymmetricDistortParams = *pInternalAndSymmetricDistortionParams;
		m_mu = internalAndSymmetricDistortParams.mu;
		m_mv = internalAndSymmetricDistortParams.mv;
		m_u0 = internalAndSymmetricDistortParams.u0;
		m_v0 = internalAndSymmetricDistortParams.v0;

		m_k1 = internalAndSymmetricDistortParams.radialDistrt[0];
		m_k2 = internalAndSymmetricDistortParams.radialDistrt[1];
		m_k3 = internalAndSymmetricDistortParams.radialDistrt[2];
		m_k4 = internalAndSymmetricDistortParams.radialDistrt[3];
		m_k5 = internalAndSymmetricDistortParams.radialDistrt[4];

		m_g[0] = 1e-7;
		m_g[1] = 1e-7;
		m_g[2] = 1e-7;

		m_i[0] = 1e-7;
		m_i[1] = 1e-7;
		m_i[2] = 1e-7;
		m_i[3] = 1e-7;

		m_h[0] = 1e-7;
		m_h[1] = 1e-7;
		m_h[2] = 1e-7;


		m_j[0] = 1e-7;
		m_j[1] = 1e-7;
		m_j[2] = 1e-7;
		m_j[3] = 1e-7;

		//2.计算从校正平面到单位球面的中心投影的单应矩阵H
		CompHs_RadialModel(pPtsInWorld, pPt2DInImg, nPtNumber, m_Parameters, RADIAL_MODEL_INTERNAL_PARAMS_COUNT, Hs);
	}
	else
	{

		//1.初始化内部参数
		if (!InitInteralParameters(eProjectType, dbThrowRatio, nCxImg, nCyImg))
		{
			return FALSE;
		}

		//2.计算从校正平面到单位球面的中心投影的单应矩阵H
		CompHs_BasicModel(pPtsInWorld, pPt2DInImg, nPtNumber, m_Parameters, BASIC_MODEL_INTERNAL_PARAMS_COUNT, Hs);
	}


   // std::vector<double> vecParams;
   // vecParams.resize(nInteralParamsNumber );

   //<<debug
    //m_K1 =  2.0589;
    //m_K2 = -0.1016;
    //m_K3 = -0.2256;
    //m_K4 = 0.3147;
    //m_K5 = -0.1493;
    //debug>>

    //相机内部参数读入
    //vecParams[0 ] = m_k1;//  = 0.999999992982425;
    //vecParams[1 ] = m_k2;// = 0.333403069525502;
    //vecParams[2 ] = m_mu;// = 202.280238462546;
    //vecParams[3 ] = m_mv;// = 202.280238462546;
    //vecParams[4 ] = m_u0;
    //vecParams[5 ] = m_v0;






    //3.从单应矩阵H初始化为外部参数(即从校正平面所在的世界坐标系转换到以相机为原点的相机坐标系的旋转矩阵Rs和偏移量ts)
    if(!InitExternalParameter(Hs, m_Rs, m_ts))
    {
        return FALSE;
    }

    //将旋转矩阵R转换为旋转向量
    double Rv[3];
    R2V(m_Rs, Rv);

	const int nExternalParamsNumber = 6;//外部参数个数
	double params[9+14+6];//4个内部参数，5个对称畸变参数，14个非对称畸变参数，6个外部参数
	if (bInternalParamsIsKnown)
	{

		//相机镜头光心位置和焦距也需要优化更新
		//因为:不同的电路板上，镜头光心在相机画面位置上的坐标是不一样的
		//     不同的摄像头上的镜头实际焦距会有微小差别
		params[0] = internalAndSymmetricDistortParams.mu;
		params[1] = internalAndSymmetricDistortParams.mv;
		params[2] = internalAndSymmetricDistortParams.u0;
		params[3] = internalAndSymmetricDistortParams.v0;

		//相机外部参数
		params[4] = Rv[0];
		params[5] = Rv[1];
		params[6] = Rv[2];
		params[7] = m_ts[0];
		params[8] = m_ts[1];
		params[9] = m_ts[2];

		//迭代优化外部参数和相机光心位置
		if (!MinimizeProjErrForOpticalCenter(
			pPtsInWorld,
			pPt2DInImg,
			nPtNumber,
			radius,
			&params[0],
			10,
			&internalAndSymmetricDistortParams))
		{
			return FALSE;
		}

		m_mu = internalAndSymmetricDistortParams.mu = params[0];
		m_mv = internalAndSymmetricDistortParams.mv = params[1];
		m_u0 = internalAndSymmetricDistortParams.u0 = params[2];
		m_v0 = internalAndSymmetricDistortParams.v0 = params[3];


		//相机外部参数
		params[0] = Rv[0];
		params[1] = Rv[1];
		params[2] = Rv[2];
		params[3] = m_ts[0];
		params[4] = m_ts[1];
		params[5] = m_ts[2];

		//相机镜头的非对称畸变参数
		params[6 ] = m_g1;
		params[7 ] = m_g2;
		params[8 ] = m_g3;
		params[9 ] = m_i1;
		params[10] = m_i2;
		params[11] = m_i3;
		params[12] = m_i4;
		params[13] = m_h1;
		params[14] = m_h2;
		params[15] = m_h3;
		params[16] = m_j1;
		params[17] = m_j2;
		params[18] = m_j3;
		params[19] = m_j4;



		//迭代优化外部参数和非对称畸变参数
		if (!MinimizeProjErrWithKnownInternalParams(
			pPtsInWorld,
			pPt2DInImg,
			nPtNumber,
			radius,
			&params[0],
			20,
			&internalAndSymmetricDistortParams))
		{
			return FALSE;
		}

		//更新相机外部参数
		Rv[0]   = params[0 ];
		Rv[1]   = params[1 ];
		Rv[2]   = params[2 ];
		m_ts[0] = params[3 ];
		m_ts[1] = params[4 ];
		m_ts[2] = params[5 ];

		//相机镜头的非对称畸变参数
		m_g1    = params[6 ];
		m_g2    = params[7 ];
		m_g3    = params[8 ];
		m_i1    = params[9 ];
		m_i2    = params[10];
		m_i3    = params[11];
		m_i4    = params[12];
		m_h1    = params[13];
		m_h2    = params[14];
		m_h3    = params[15];
		m_j1    = params[16];
		m_j2    = params[17];
		m_j3    = params[18];
		m_j4    = params[19];
	}
	else
	{//相机内部参数全部未知的情形
		int nInteralParamsNumber = BASIC_MODEL_INTERNAL_PARAMS_COUNT;//内部参数个数		
		//vecParams.resize(nInteralParamsNumber + nExternalParamsNumber);

		//相机内部参数读入
		for (int i = 0; i < nInteralParamsNumber; i++)
		{
			params[i] = m_Parameters[i];
		}

		//相机外部参数读入
		params[6]  = Rv[0];
		params[7]  = Rv[1];
		params[8]  = Rv[2];
		params[9]  = m_ts[0];
		params[10] = m_ts[1];
		params[11] = m_ts[2];


		//4.利用Levenverg-Marquardt算法求解'Basic Model'的内部参数和外部参数的最优值。
		//  'Basic Mode'有6个内部参数(mu, mv, u0, v0, k1, k2)
		if (!MinimizeProjErr(pPtsInWorld, pPt2DInImg, nPtNumber, 0, &params[0], 6+6))
		{
			return FALSE;
		}

		//更新迭代优化计算得到的参数
		m_mu = params[0];
		m_mv = params[1];
		m_u0 = params[2];
		m_v0 = params[3];
		m_k1 = params[4];
		m_k2 = params[5];

		//相机外部参数更新
		Rv[0]   = params[6];
		Rv[1]   = params[7];
		Rv[2]   = params[8];
		m_ts[0] = params[9];
		m_ts[1] = params[10];
		m_ts[2] = params[11];

		//相机内部参数读入
		params[0]  = m_mu;
		params[1]  = m_mv;
		params[2]  = m_u0;
		params[3]  = m_v0;
		params[4]  = m_k1;
		params[5]  = m_k2;
		params[6]  = m_k3;
		params[7]  = m_k4;
		params[8]  = m_k5;
		params[9]  = m_g1;
		params[10] = m_g2;
		params[11] = m_g3;
		params[12] = m_i1;
		params[13] = m_i2;
		params[14] = m_i3;
		params[15] = m_i4;
		params[16] = m_h1;
		params[17] = m_h2;
		params[18] = m_h3;
		params[19] = m_j1;
		params[20] = m_j2;
		params[21] = m_j3;
		params[22] = m_j4;


		//相机外部参数读入
		params[23] = Rv[0];
		params[24] = Rv[1];
		params[25] = Rv[2];
		params[26] = m_ts[0];
		params[27] = m_ts[1];
		params[28] = m_ts[2];

		//5.利用Levenverg-Marquardt算法求解'Radial Mode'的内部参数和外部参数的最优值。
		// 'Raidal Mode'有9个内部参数(mu, mv, u0, v0, k1, k2, k3, k4, k5)
		if (!MinimizeProjErr(pPtsInWorld, pPt2DInImg, nPtNumber, radius, &params[0], 9+14+6))
		{
			return FALSE;
		}

		//更新迭代计算得到的参数
		m_mu = params[0];
		m_mv = params[1];
		m_u0 = params[2];
		m_v0 = params[3];
		m_k1 = params[4];
		m_k2 = params[5];
		m_k3 = params[6];
		m_k4 = params[7];
		m_k5 = params[8];
		m_g1 = params[9];
		m_g2 = params[10];
		m_g3 = params[11];
		m_i1 = params[12];
		m_i2 = params[13];
		m_i3 = params[14];
		m_i4 = params[15];
		m_h1 = params[16];
		m_h2 = params[17];
		m_h3 = params[18];
		m_j1 = params[19];
		m_j2 = params[20];
		m_j3 = params[21];
		m_j4 = params[22];


		//相机外部参数读入
		Rv[0]   = params[23];
		Rv[1]   = params[24];
		Rv[2]   = params[25];
		m_ts[0] = params[26];
		m_ts[1] = params[27];
		m_ts[2] = params[28];
	}

    V2R(Rv, m_Rs);


    //计算旋转矩阵的逆矩阵
    InvR(m_Rs, m_InvRs);


    //相机在世界坐标系中的坐标
    m_ptCameraLocation[0] = -(m_InvRs[0][0] * m_ts[0] + m_InvRs[0][1] * m_ts[1] + m_InvRs[0][2] * m_ts[2]);
    m_ptCameraLocation[1] = -(m_InvRs[1][0] * m_ts[0] + m_InvRs[1][1] * m_ts[1] + m_InvRs[1][2] * m_ts[2]);
    m_ptCameraLocation[2] = -(m_InvRs[2][0] * m_ts[0] + m_InvRs[2][1] * m_ts[1] + m_InvRs[2][2] * m_ts[2]);
	 
    //相机中心在在校正平面上的垂足
    TPoint2D ptCameraFootOnCalibratePlane;
    ptCameraFootOnCalibratePlane[0] =  m_ptCameraLocation[0];
    ptCameraFootOnCalibratePlane[1] =  m_ptCameraLocation[1];
	
    //相机中心在世界坐标系z=0平面上的垂足F,在相机中所成的像F'
    ForwardProject2D(
        &ptCameraFootOnCalibratePlane,
        1,
        m_Parameters,
        EXTENDED_MODEL_INTERNAL_PARAMS_COUNT,
        m_Rs,
        m_ts,
        &m_ptOZFootImage);


    
    GCM_Debug(
        _T("=====\nInternal Parameters:\nmu = %g\nmv = %g\nu0 = %g\nv0 = %g\nk1 = %g\nk2 = %g\nk3 = %g\nk4 = %g\nk5 = %g\n"),
        this->m_mu,
        this->m_mv,
        this->m_u0,
        this->m_v0,
        this->m_k1,
        this->m_k2,
        this->m_k3,
        this->m_k4,
        this->m_k5);

    GCM_Debug(
        _T("g1 = %g\ng2 = %g\ng3 = %g\ni1 = %g\ni2 = %g\ni3 = %g\ni4 = %g\n"),
        this->m_g1,
        this->m_g2,
        this->m_g3,
        this->m_i1,
        this->m_i2,
        this->m_i3,
        this->m_i4);

    GCM_Debug(
        _T("h1 = %g\nh2 = %g\nh3 = %g\nj1 = %g\nj2 = %g\nj3 = %g\nj4 = %g\n"),
        this->m_h1,
        this->m_h2,
        this->m_h3,
        this->m_j1,
        this->m_j2,
        this->m_j3,
        this->m_j4);


    GCM_Debug(
        _T("=====\nRotation Matrix:\n%12.6g\t%12.6g\t%12.6g\n%12.6g\t%12.6g\t%12.6g\n%12.6g\t%12.6g\t%12.6g\n"),
        this->m_Rs[0][0],
        this->m_Rs[0][1],
        this->m_Rs[0][2],
        this->m_Rs[1][0],
        this->m_Rs[1][1],
        this->m_Rs[1][2],
        this->m_Rs[2][0],
        this->m_Rs[2][1],
        this->m_Rs[2][2]);


    GCM_Debug(
        _T("=====\nTranslation:\n%12.6g\n%12.6g\n%12.6g\n"),
        this->m_ts[0],
        this->m_ts[1],
        this->m_ts[2]);

	//如果允许调试输出则输出校正结果
	if (this->m_bDebug)
	{
		SaveCameraModelParams(*this, NULL);

		SaveCalibrateData(
			eProjectType,
			dbThrowRatio,
			pPtsInWorld,
			pPt2DInImg,
			nPtNumber,
			rcMonitor,
			radius,
			nCxImg,
			nCyImg);
	}


    CheckForwardProjectPrecision(
        pPt2DInImg,
        pPtsInWorld,
        nPtNumber,
        m_Parameters,
        EXTENDED_MODEL_INTERNAL_PARAMS_COUNT,
        m_Rs,
        m_ts);

    CheckBackwardProjectPrecision(
        pPt2DInImg,
        pPtsInWorld,
        nPtNumber,
        m_Parameters,
        EXTENDED_MODEL_INTERNAL_PARAMS_COUNT,
        M_PI/2.0,//m_thetaMax,
        m_InvRs,
        m_ts);

    //图像尺寸
    m_cxImage = nCxImg;
    m_cyImage = nCyImg;

	//校正圆半径
	m_nPatternCircleRadius = radius;

    ConstructRToThetaLUT(m_cxImage, m_cyImage);


    //屏幕尺寸
    m_Monitor_Left   = (double)rcMonitor.left ;
    m_Monitor_Right  = (double)rcMonitor.right;
    m_Monitor_Top    = (double)rcMonitor.top  ;
    m_Monitor_Bottom = (double)rcMonitor.bottom;

    return TRUE;
}

/*
//@功能:建立从r查找theta值(光线入射角)的查找表
//     r,θ满足关系
//      r=k(0)*θ+ k(1)*θ^3 + k(2)*θ^5 + k(3)*θ^7 + k(4)*θ^9
void CGenericCameraModel::ConstructRToThetaLUT(const double& cxImage, const double& cyImage)

{

    //m_k[0]为
    if(m_k[0] < std::numeric_limits<double>::epsilon())
    {
        return;
    }

 //计算最大归一化半径
    TPoint2D corners[4];

    corners[0].d[0] = 0;
    corners[0].d[1] = 0;

    corners[1].d[0] = cxImage;
    corners[1].d[1] = 0;

    corners[2].d[0] = 0;
    corners[2].d[1] = cyImage;

    corners[3].d[0] = cxImage;
    corners[3].d[1] = cyImage;

    //距离相机中心最远的像素距离
    double pixleDistMax = (std::numeric_limits<double>::min)();
    for(int i=0; i < _countof(corners); i++)
    {
        double dx = (corners[i].d[0] - m_u0);
        double dy = (corners[i].d[1] - m_v0);
        double r = sqrt(dx * dx + dy * dy);
        if(r > (double)pixleDistMax)
        {
            pixleDistMax = r;
        }

    }//for


    //归一化最大半径(在距离光心单位距离的成像面上)
    double radiusMax = (std::numeric_limits<double>::min)();

    for(int i=0; i < _countof(corners); i++)
    {
        double dx = (corners[i].d[0] - m_u0)/m_mu;
        double dy = (corners[i].d[1] - m_v0)/m_mv;
        double r = sqrt(dx * dx + dy * dy);
        if(r > radiusMax)
        {
            radiusMax = r;
        }

    }//for

    double radiusMin = 0.0;

    //距离半径的微分增量
    double radius_inc_step = (radiusMax - radiusMin)/(pixleDistMax*16);//像素数值精度=1/16像素

    int  LUT_SIZE =  int((radiusMax - radiusMin)/radius_inc_step);//查找表的长度

    if(m_R2ThetaLUT != NULL)
    {
        delete [] m_R2ThetaLUT;
        m_R2ThetaLUT = NULL;
        m_nLUTSize = 0;
    }


    m_R2ThetaLUT =  new TRToThetaLUTEntry[PHI_DIVISION*LUT_SIZE];
   


    int nStep = 0;
    double theta = 0.0;
    double r = radiusMin;
    
    while(nStep < LUT_SIZE)
    {
        m_R2ThetaLUT[nStep].radius = r;
        m_R2ThetaLUT[nStep].theta  = theta;

        double theta2 = theta  * theta;
        double theta3 = theta  * theta2;
        double theta4 = theta2 * theta2;
        double theta5 = theta  * theta4;
        double theta6 = theta4 * theta2;
        double theta7 = theta6 * theta;
        double theta8 = theta4 * theta4;
        double theta9 = theta8 * theta;

        //dr=(dr/dθ)*dθ+ O(θ)
        double dr_dtheta = m_k[0] + m_k[1]*3*theta2 + m_k[2]*5*theta4 + m_k[3]*7*theta6 + m_k[4]*9*theta8;

        //计算r的微分增量对应的θ增量
        double delta_theta = radius_inc_step/dr_dtheta;
        
        //加上微分增量的角度
        theta = theta + delta_theta;

        //
        r = m_k[0]*theta + m_k[1]*theta3 + m_k[2]*theta5 + m_k[3]*theta7 + m_k[4]*theta9;

        //
        nStep = nStep + 1; 
    }//while

	 m_nLUTSize = nStep;

}

*/


//@功能:建立从r查找theta值(光线入射角)的查找表
//     r,θ满足关系
//      r=k(0)*θ+ k(1)*θ^3 + k(2)*θ^5 + k(3)*θ^7 + k(4)*θ^9
void CGenericCameraModel::ConstructRToThetaLUT(const double& cxImage, const double& cyImage)
{

    //m_k[0]为
    if(m_k[0] < std::numeric_limits<double>::epsilon())
    {
        return;
    }

 //计算最大归一化半径
    TPoint2D corners[4];

    corners[0].d[0] = 0;
    corners[0].d[1] = 0;

    corners[1].d[0] = cxImage;
    corners[1].d[1] = 0;

    corners[2].d[0] = 0;
    corners[2].d[1] = cyImage;

    corners[3].d[0] = cxImage;
    corners[3].d[1] = cyImage;

    //距离相机中心最远的像素距离
    double pixleDistMax = (std::numeric_limits<double>::min)();
    for(int i=0; i < _countof(corners); i++)
    {
        double dx = (corners[i].d[0] - m_u0);
        double dy = (corners[i].d[1] - m_v0);
        double r = sqrt(dx * dx + dy * dy);
        if(r > (double)pixleDistMax)
        {
            pixleDistMax = r;
        }

    }//for


    //归一化最大半径(在距离光心单位距离的成像面上)
    double radiusMax = (std::numeric_limits<double>::min)();

    for(int i=0; i < _countof(corners); i++)
    {
        double dx = (corners[i].d[0] - m_u0)/m_mu;
        double dy = (corners[i].d[1] - m_v0)/m_mv;
        double r = sqrt(dx * dx + dy * dy);
        if(r > radiusMax)
        {
            radiusMax = r;
        }

    }//for

    double radiusMin = 0.0;

    //距离半径的微分增量
    double radius_inc_step = (radiusMax - radiusMin)/(pixleDistMax*16);//像素数值精度=1/16像素

    int  LUT_SIZE =  int((radiusMax - radiusMin)/radius_inc_step);//查找表的长度


	
	for(int i = 0; i < PHI_DIVISION; i++)
	{

		if(m_RPhi2ThetaTables[i].nLUTSize < LUT_SIZE && m_RPhi2ThetaTables[i].r2ThetaLUT)
		{
			delete [] m_RPhi2ThetaTables[i].r2ThetaLUT;

			 m_RPhi2ThetaTables[i].r2ThetaLUT = NULL;
			 m_RPhi2ThetaTables[i].nLUTSize   =  0;
		}

		if(m_RPhi2ThetaTables[i].r2ThetaLUT == NULL)
		{
			m_RPhi2ThetaTables[i].nLUTSize   = LUT_SIZE;
			m_RPhi2ThetaTables[i].phi        = 2*M_PI*i/PHI_DIVISION;
			m_RPhi2ThetaTables[i].r2ThetaLUT = new TRToThetaLUTEntry[LUT_SIZE];
		}

		if(m_RPhi2ThetaTables[i].r2ThetaLUT == NULL) break;


		int nStep = 0;
		double theta = 0.0;
		double r = radiusMin;
    
		

		double phi = i * 2*M_PI/PHI_DIVISION;

		double cosphi  = cos(phi);
		double sinphi  = sin(phi);
		double cos2phi = cos(phi)*cos(phi) - sin(phi)*sin(phi);
		double sin2phi = 2*sin(phi)*cos(phi);



		//dr = (g(1)*theta + g(2)*theta.^3 + g(3)*theta.^5 ).*( i(1)*cosphi + i(2)*sinphi + i(3)*cos2phi + i(4)*sin2phi );
		//rd = r + dr = kk(1)*theta + kk(2)*theta.^3 + kk(3)*theta.^5 + kk(4)*theta.^7 + kk(5)*theta.^9;
		//这里
		//dr = (g(1)*theta + g(2)*theta.^3 + g(3)*theta.^5 ).*( i(1)*cosphi + i(2)*sinphi + i(3)*cos2phi + i(4)*sin2phi );
		//r = k1*theta + k2*theta.^3 + k3*theta.^5 + k4*theta.^7 + k5*theta.^9;
		double  dr_coef =  m_i[0]*cosphi + m_i[1]*sinphi + m_i[2]*cos2phi + m_i[3]*sin2phi;
		
		double kk[5];
		kk[0] = m_k[0] + m_g[0]*dr_coef;
		kk[1] = m_k[1] + m_g[1]*dr_coef;
		kk[2] = m_k[2] + m_g[2]*dr_coef;
		kk[3] = m_k[3];
		kk[4] = m_k[4];


		double r_last = 0.0;
		while(nStep < LUT_SIZE)
		{
			m_RPhi2ThetaTables[i].r2ThetaLUT[nStep].radius = r;
			m_RPhi2ThetaTables[i].r2ThetaLUT[nStep].theta  = theta;

			double theta2 = theta  * theta;
			double theta3 = theta  * theta2;
			double theta4 = theta2 * theta2;
			double theta5 = theta  * theta4;
			double theta6 = theta4 * theta2;
			double theta7 = theta6 * theta;
			double theta8 = theta4 * theta4;
			double theta9 = theta8 * theta;

			//用一阶近似逼近
			//dr=(dr/dθ)*dθ+ O(θ)
			double dr_dtheta = kk[0] + kk[1]*3*theta2 + kk[2]*5*theta4 + kk[3]*7*theta6 + kk[4]*9*theta8;

			//计算r的微分增量对应的θ增量
			double delta_theta = radius_inc_step/dr_dtheta;

			if(delta_theta < 0.0)
			{//二分查找必须保证键值单调递增(或者递减)
				break;
			}
	        
			//加上微分增量的角度
			theta = theta + delta_theta;

			//
			r = kk[0]*theta + kk[1]*theta3 + kk[2]*theta5 + kk[3]*theta7 + kk[4]*theta9;


			if(theta > M_PI/2 || r < r_last || r >  radiusMax)
			{
					
				break;
			}

			r_last = r;

			nStep = nStep + 1; 
		}//while

		m_RPhi2ThetaTables[i].nLUTSize = nStep;
	}

        

}

//@功能:从查找表中查找r值对应的θ值。
//      因为查找表按照大小有序,因此采用二分查找内插值区间
BOOL CGenericCameraModel::GetThetaFrmLUT(const double& r, const double& phi, double& theta)const
{

	//根据phi值,二分法确定搜索的表格
	double phi_adjust =  fmod(phi, M_PI*2);
	if(phi_adjust < 0.0)
	{
		phi_adjust = phi_adjust + M_PI*2;
	}


	int m;
	int l = 0;
	int u = PHI_DIVISION - 1;
	

	BOOL found = FALSE;

	
	int tableIndex  = 0;
	while(l <= u)
    {
         m = (l + u) / 2;

        if(phi_adjust <= m_RPhi2ThetaTables[m].phi)
        {
              if( m > 0 && m_RPhi2ThetaTables[m - 1].phi <= phi_adjust)
              {
				  double d1 =  phi_adjust - m_RPhi2ThetaTables[m - 1].phi;
				  double d2 =  m_RPhi2ThetaTables[m].phi - phi_adjust;
				  if(d1 < d2)
				  {
					tableIndex = m - 1;
				  }
				  else
				  {
					tableIndex = m;
				  }

				 found = 1;
				 break;
              }
			  else if(m == 0)
			  {
				  tableIndex = 0;
				  found  = 0;
				  break;
			  }
              else
              {
                 u = m - 1;
              }
        }
        else // m_RPhi2ThetaTables[m].phi < phi_adjust
        {
            if(m < (PHI_DIVISION - 1) && phi_adjust <= m_RPhi2ThetaTables[m + 1].phi)
            {
				  double d1 =  phi_adjust - m_RPhi2ThetaTables[m].phi;
				  double d2 =  m_RPhi2ThetaTables[m + 1].phi - phi_adjust;
				  if(d1 < d2)
				  {
					tableIndex = m ;
				  }
				  else
				  {
					tableIndex = m + 1;
				  }

                found = 1;
                break;
            }
			else if(m == PHI_DIVISION - 1)
			{
				found  = 1;

				tableIndex = PHI_DIVISION - 1;
				break;
			}
            else
            {
                l = m + 1;
            }
        }

    }//while


	if(!found) return FALSE;


	const TRPhiToThetaTable& lut = m_RPhi2ThetaTables[tableIndex];


	//根据r值, ,二分法查找

     l = 0;
	 u = lut.nLUTSize - 1;    
    double r0,r1,theta1, theta0;
    found = FALSE;
    while(l < u)
    {
         m = (l + u) / 2;

		 if(r <= lut.r2ThetaLUT[m].radius)
        {
              if(m > 0 && lut.r2ThetaLUT[m-1].radius <= r )
              {
               //找到了内插值区间
               r0     = lut.r2ThetaLUT[m - 1].radius;
               theta0 = lut.r2ThetaLUT[m - 1].theta;
               r1     = lut.r2ThetaLUT[m    ].radius; 
               theta1 = lut.r2ThetaLUT[m    ].theta;
               
               found = 1;
               break;
              }
              else
              {
                u = m - 1;
              }
        }
        else 
        {
            if(m < (lut.nLUTSize - 1 - 1) && r <= lut.r2ThetaLUT[m + 1].radius)
            {
                //找到了内插值区间
                r0     = lut.r2ThetaLUT[m].radius;
                theta0 = lut.r2ThetaLUT[m].theta;
             
                r1     = lut.r2ThetaLUT[m+1].radius;
                theta1 = lut.r2ThetaLUT[m+1].theta;
                
                found = 1;
                break;
            }
            else
            {
                l = m + 1;
            }
        }

    }//while

	//找到了
    if (found)
    {
		double cosphi  = cos(phi_adjust);
		double sinphi  = sin(phi_adjust);
		double cos2phi = cos(phi_adjust)*cos(phi_adjust) - sin(phi_adjust)*sin(phi_adjust);
		double sin2phi = 2*sin(phi_adjust)*cos(phi_adjust);

		//dr = (g(1)*theta + g(2)*theta.^3 + g(3)*theta.^5 ).*( i(1)*cosphi + i(2)*sinphi + i(3)*cos2phi + i(4)*sin2phi );
		//rd = r + dr = kk(1)*theta + kk(2)*theta.^3 + kk(3)*theta.^5 + kk(4)*theta.^7 + kk(5)*theta.^9;
		//这里
		//dr = (g(1)*theta + g(2)*theta.^3 + g(3)*theta.^5 ).*( i(1)*cosphi + i(2)*sinphi + i(3)*cos2phi + i(4)*sin2phi );
		//r = k1*theta + k2*theta.^3 + k3*theta.^5 + k4*theta.^7 + k5*theta.^9;
		double  dr_coef =  m_i[0]*cosphi + m_i[1]*sinphi + m_i[2]*cos2phi + m_i[3]*sin2phi;
		
		double kk[5];
		kk[0] = m_k[0] + m_g[0]*dr_coef;
		kk[1] = m_k[1] + m_g[1]*dr_coef;
		kk[2] = m_k[2] + m_g[2]*dr_coef;
		kk[3] = m_k[3];
		kk[4] = m_k[4];


        double theta0_2 = theta0  * theta0;
      // double theta0_3 = theta0  * theta02;
        double theta0_4 = theta0_2 * theta0_2;
      //double theta0_5 = theta0  * theta04;
        double theta0_6 = theta0_4 * theta0_2;
     // double theta0_7 = theta06 * theta0 ;
        double theta0_8 = theta0_4 * theta0_4;
     // double theta0_9 = theta08 * theta0 ;
        double dr_dtheta0 = kk[0] + kk[1]*3*theta0_2 + kk[2]*5*theta0_4 + kk[3]*7*theta0_6 + kk[4]*9*theta0_8;
        
        double theta1_2 = theta1  * theta1;
        //double theta1_3 = theta1  * theta12;
        double theta1_4 = theta1_2 * theta1_2;
        //double theta1_5 = theta1  * theta14;
        double theta1_6 = theta1_4 * theta1_2;
        //double theta1_7 = theta16 * theta1 ;
        double theta1_8 = theta1_4 * theta1_4;
        //double theta1_9 = theta18 * theta1 ;
        double dr_dtheta1 = kk[0] + kk[1]*3*theta1_2 + kk[2]*5*theta1_4 + kk[3]*7*theta1_6 + kk[4]*9*theta1_8;

        //计算r的微分对应的theta微分
        double d_theta = 0.0;
		
		if((dr_dtheta0 + dr_dtheta1)/2 > std::numeric_limits<double>::epsilon())
		{
			d_theta = (r - r0)/((dr_dtheta0 + dr_dtheta1)/2);
		}

        theta = theta0 + d_theta;
    }

    
    return found;
}


//@功能:根据校正得到的内部参数和外部参数，将坐标从相机平面投影到校正平面上。
//@参数:pPt2DInImag, 输出参数, 输入的相机坐标
//      nPtNumber, 点的个数
//      pPt2DInWorld, 输出参数, 保存世界坐标的数组
BOOL CGenericCameraModel::BackwardProject(const TPoint2D* pPt2DInImag, int nPtNumber, TPoint2D* pPt2DInWorld)const
{


    //double k[5], g[3], i[4], h[3], j[4] ;
    //double mu,mv, u0,v0;
    //if(nParamsCount == CGenericCameraModel::BASIC_MODEL_INTERNAL_PARAMS_COUNT)
    //{
    //    mu   = pInternalParams[0];
    //    mv   = pInternalParams[1];
    //    u0   = pInternalParams[2];
    //    v0   = pInternalParams[3];
    //    k[0] = pInternalParams[4];
    //    k[1] = pInternalParams[5];
    //    k[2] = 0.0;
    //    k[3] = 0.0;
    //    k[4] = 0.0;
    //    g[0] = 0.0;
    //    g[1] = 0.0;
    //    g[2] = 0.0;
    //    i[0] = 0.0;
    //    i[1] = 0.0;
    //    i[2] = 0.0;
    //    i[3] = 0.0;
    //    h[0] = 0.0;
    //    h[1] = 0.0;
    //    h[2] = 0.0;
    //    j[0] = 0.0;
    //    j[1] = 0.0;
    //    j[2] = 0.0;
    //    j[3] = 0.0;

    //}
    //else if(nParamsCount == CGenericCameraModel::RADIAL_MODEL_INTERNAL_PARAMS_COUNT)
    //{
    //    mu   = pInternalParams[0];
    //    mv   = pInternalParams[1];
    //    u0   = pInternalParams[2];
    //    v0   = pInternalParams[3];
    //    k[0] = pInternalParams[4];
    //    k[1] = pInternalParams[5];
    //    k[2] = pInternalParams[6];
    //    k[3] = pInternalParams[7];
    //    k[4] = pInternalParams[8];
    //    g[0] = 0.0;
    //    g[1] = 0.0;
    //    g[2] = 0.0;
    //    i[0] = 0.0;
    //    i[1] = 0.0;
    //    i[2] = 0.0;
    //    i[3] = 0.0;
    //    h[0] = 0.0;
    //    h[1] = 0.0;
    //    h[2] = 0.0;
    //    j[0] = 0.0;
    //    j[1] = 0.0;
    //    j[2] = 0.0;
    //    j[3] = 0.0;
    //}
    //else if(nParamsCount == CGenericCameraModel::EXTENDED_MODEL_INTERNAL_PARAMS_COUNT)
    //{
    //    mu   = pInternalParams[0 ];
    //    mv   = pInternalParams[1 ];
    //    u0   = pInternalParams[2 ];
    //    v0   = pInternalParams[3 ];
    //    k[0] = pInternalParams[4 ];
    //    k[1] = pInternalParams[5 ];
    //    k[2] = pInternalParams[6 ];
    //    k[3] = pInternalParams[7 ];
    //    k[4] = pInternalParams[8 ];
    //    g[0] = pInternalParams[9 ];
    //    g[1] = pInternalParams[10];
    //    g[2] = pInternalParams[11];
    //    i[0] = pInternalParams[12];
    //    i[1] = pInternalParams[13];
    //    i[2] = pInternalParams[14];
    //    i[3] = pInternalParams[15];
    //    h[0] = pInternalParams[16];
    //    h[1] = pInternalParams[17];
    //    h[2] = pInternalParams[18];
    //    j[0] = pInternalParams[19];
    //    j[1] = pInternalParams[20];
    //    j[2] = pInternalParams[21];
    //    j[3] = pInternalParams[22];

    //}
    //else
    //{
    //    return FALSE;
    //}

    if(fabs(m_mu) < std::numeric_limits<double>::epsilon())
    {//mu-->0;
        return FALSE;
    }

    if(fabs(m_mv) < std::numeric_limits<double>::epsilon())
    {//mv-->0
        return FALSE;
    }


    //double offset_X = m_InvRs[0][0] * m_ts[0] + m_InvRs[0][1] * m_ts[1] + m_InvRs[0][2] * m_ts[2];
    //double offset_Y = m_InvRs[1][0] * m_ts[0] + m_InvRs[1][1] * m_ts[1] + m_InvRs[1][2] * m_ts[2];



    for(int nPtIndex = 0; nPtIndex < nPtNumber; nPtIndex++)
    {
        double u = pPt2DInImag[nPtIndex].d[0];
        double v = pPt2DInImag[nPtIndex].d[1];
        double xd = (u - m_u0) / m_mu;
        double yd = (v - m_v0) / m_mv;
        double rd = sqrt(xd*xd + yd*yd);

        double x_sphere = 0.0, y_sphere = 0.0, z_sphere=0.0;//映射到单位球面上的坐标
        if( fabs(rd) < std::numeric_limits<double>::epsilon())
        {//rd -->0
            x_sphere = 0.0; 
            y_sphere = 0.0;
            z_sphere = 1.0;
        }
        else
        {
            double phi_d   = atan2(yd, xd);


			double theta_d;
			{
				//CPerfDetector pf1(_T("CalcTheta"));

				if(!GetThetaFrmLUT(rd, phi_d, theta_d))//使用查表法计算theta值,消耗1~2us, 而CalcTheta耗费太多CPU资源
				{

					//rd^2=(r+dr)^2 + dt^2
					//一般dt很小,所以 rd≈(r+dr)
					//这里
					//dr = (g(1)*theta + g(2)*theta.^3 + g(3)*theta.^5 ).*( i(1)*cosphi + i(2)*sinphi + i(3)*cos2phi + i(4)*sin2phi );
					//r = k1*theta + k2*theta.^3 + k3*theta.^5 + k4*theta.^7 + k5*theta.^9;

					double cosphi_d = cos(phi_d);
					double sinphi_d = sin(phi_d);
					double cos2phi_d = cosphi_d*cosphi_d - sinphi_d*sinphi_d;
					double sin2phi_d = 2*cosphi_d*sinphi_d;
					double dr_coef = m_i[0]*cosphi_d + m_i[1]*sinphi_d + m_i[2]*cos2phi_d + m_i[3]*sin2phi_d;

					double kk[5];
					kk[0] = m_k[0] +  dr_coef * m_g[0];
					kk[1] = m_k[1] +  dr_coef * m_g[1];
					kk[2] = m_k[2] +  dr_coef * m_g[2];
					kk[3] = m_k[3];
					kk[4] = m_k[4];
					BOOL bRet = CalcTheta(rd, kk, m_thetaMax, theta_d);//测试表明CalcTheta耗费800us
					if(!bRet) return FALSE;

				}
			}

            double theta,phi;

            {
                //CPerfDetector pf1(_T("GetThetaPhi_LM"));//测试表明GetThetaPhi_LM耗费4us
                GetThetaPhi_LM(
                    xd,
                    yd,
                    theta_d,
                    phi_d,
                    m_k,
                    m_g,
                    m_i,
                    m_h,
                    m_j,
                    theta,
                    phi);
            }



            double sinphi = sin(phi);
            double cosphi = cos(phi);

            x_sphere = sin(theta) * cosphi;
            y_sphere = sin(theta) * sinphi;
            z_sphere = cos(theta);
        }

        double ratio = (m_InvRs[2][0] * x_sphere + m_InvRs[2][1] * y_sphere + m_InvRs[2][2]* z_sphere);
        double scale_factor = (m_InvRs[2][0] * m_ts[0] + m_InvRs[2][1] * m_ts[1] + m_InvRs[2][2] * m_ts[2])/ratio;

        double X = (m_InvRs[0][0] * x_sphere + m_InvRs[0][1] * y_sphere + m_InvRs[0][2] * z_sphere) * scale_factor + m_ptCameraLocation[0];
        double Y = (m_InvRs[1][0] * x_sphere + m_InvRs[1][1] * y_sphere + m_InvRs[1][2] * z_sphere) * scale_factor + m_ptCameraLocation[1];

        pPt2DInWorld[nPtIndex].d[0] = X;
        pPt2DInWorld[nPtIndex].d[1] = Y;

    }//for



    return TRUE;
}



//@功能:计算光点与光轴的夹角θ和在图像平面的方位角φ
BOOL CGenericCameraModel::GetThetaPhi(const TPoint2D* pPt2DInImag, int nPtNumber, double* pThetas, double* pPhis)const 
{

	for (int nPtIndex = 0; nPtIndex < nPtNumber; nPtIndex++)
	{
		double u = pPt2DInImag[nPtIndex].d[0];
		double v = pPt2DInImag[nPtIndex].d[1];
		double xd = (u - m_u0) / m_mu;
		double yd = (v - m_v0) / m_mv;
		double rd = sqrt(xd*xd + yd*yd);

		double theta =0.0, phi = 0.0;
		if (fabs(rd) < std::numeric_limits<double>::epsilon())
		{//rd -->0
			theta = 0.0;
			phi   = 0.0;
		}
		else
		{
			double phi_d = atan2(yd, xd);


			double theta_d;
			{
				//CPerfDetector pf1(_T("CalcTheta"));

				if (!GetThetaFrmLUT(rd, phi_d, theta_d))//使用查表法计算theta值,消耗1~2us, 而CalcTheta耗费太多CPU资源
				{

					//rd^2=(r+dr)^2 + dt^2
					//一般dt很小,所以 rd≈(r+dr)
					//这里
					//dr = (g(1)*theta + g(2)*theta.^3 + g(3)*theta.^5 ).*( i(1)*cosphi + i(2)*sinphi + i(3)*cos2phi + i(4)*sin2phi );
					//r = k1*theta + k2*theta.^3 + k3*theta.^5 + k4*theta.^7 + k5*theta.^9;

					double cosphi_d = cos(phi_d);
					double sinphi_d = sin(phi_d);
					double cos2phi_d = cosphi_d*cosphi_d - sinphi_d*sinphi_d;
					double sin2phi_d = 2 * cosphi_d*sinphi_d;
					double dr_coef = m_i[0] * cosphi_d + m_i[1] * sinphi_d + m_i[2] * cos2phi_d + m_i[3] * sin2phi_d;

					double kk[5];
					kk[0] = m_k[0] + dr_coef * m_g[0];
					kk[1] = m_k[1] + dr_coef * m_g[1];
					kk[2] = m_k[2] + dr_coef * m_g[2];
					kk[3] = m_k[3];
					kk[4] = m_k[4];
					BOOL bRet = CalcTheta(rd, kk, m_thetaMax, theta_d);//测试表明CalcTheta耗费800us
					if (!bRet) return FALSE;

				}
			}



			{
				//CPerfDetector pf1(_T("GetThetaPhi_LM"));//测试表明GetThetaPhi_LM耗费4us
				GetThetaPhi_LM(
					xd,
					yd,
					theta_d,
					phi_d,
					m_k,
					m_g,
					m_i,
					m_h,
					m_j,
					theta,
					phi);
			}

		}//else

		if (pThetas)
		{
			pThetas[nPtIndex] = theta;
		}

		if (pPhis)
		{
			pPhis[nPtIndex] = phi;
		}
	}//for


	return TRUE;
}
//@功  能:从图像坐标影映射为屏幕坐标
//@参  数:ptImage, 输入参数, 相机画面中的坐标数组
//        nPtNumber, 数组个坐标个数
//        ptWorld, 输出参数，保存从相机平面坐标转化成校正平面坐标的数组。
//@返回值:成功, TRUE
//        失败, FALSE
BOOL CGenericCameraModel::FromImage2World(const TPoint2D* ptsInImg, int nPtNumber, TPoint2D* ptsInWorld) const
{

    BOOL bRet =  BackwardProject(
        ptsInImg,
        nPtNumber,
        ptsInWorld);

    return bRet;
}

/*
//@功  能:从图像坐标影映射为屏幕坐标
//@参  数:pImgPts, 输入参数, 相机画面中的坐标数组.
//        prcBounds, 输入参数, 指向光斑外界矩形数组的指针.
//        nPtNumber, 数组个坐标个数.
//        ptWorldPts, 输出参数，保存从相机平面坐标转化成校正平面坐标的数组。
//@返回值:成功, TRUE
//        失败, FALSE
//BOOL CGenericCameraModel::FromImage2World_Aug(const TPoint2D* pImgPts, const RECT* prcBounds, int nPtNumber, TPoint2D* ptWorldPts)
BOOL CGenericCameraModel::FromImage2World_Aug(const POINT* pImgPts, const RECT* prcBounds, int nPtNumber, POINT* ptWorldPts)const
{

    if(fabs(m_mu) < std::numeric_limits<double>::epsilon())
    {//m_mu-->0;
        return FALSE;
    }

    if(fabs(m_mv) < std::numeric_limits<double>::epsilon())
    {//m_mv-->0
        return FALSE;
    }


    for(int i = 0; i < nPtNumber; i++)
    {
        TPoint2D pt2DsInImage[5];

        pt2DsInImage[0].d[0] = double(pImgPts[i].x) / 16.0;
        pt2DsInImage[0].d[1] = double(pImgPts[i].y) / 16.0;

        int nWidth  = prcBounds[i].right  - prcBounds[i].left;
        int nHeight = prcBounds[i].bottom - prcBounds[i].top;

        pt2DsInImage[1].d[0] =  prcBounds[i].left + (double)nWidth / 2.0;
        pt2DsInImage[1].d[1] =  prcBounds[i].top;

        pt2DsInImage[2].d[0] =  prcBounds[i].left; 
        pt2DsInImage[2].d[1] =  prcBounds[i].top + (double)nHeight / 2.0;

        pt2DsInImage[3].d[0] =  prcBounds[i].right; 
        pt2DsInImage[3].d[1] =  prcBounds[i].bottom -  (double)nHeight / 2.0;

        pt2DsInImage[4].d[0] = prcBounds[i].right - (double)nWidth / 2.0;
        pt2DsInImage[4].d[1] = prcBounds[i].bottom;


        TPoint3D pt3DsInWorld[5];//以相机为中心的世界坐标系
        for(int j = 0; j < _countof(pt2DsInImage); j++)
        {
            double u  = pt2DsInImage[j].d[0];
            double v  = pt2DsInImage[j].d[1];
            double xd = (u - m_u0) / m_mu;
            double yd = (v - m_v0) / m_mv;
            double rd = sqrt(xd*xd + yd*yd);

            double x_sphere = 0.0, y_sphere = 0.0, z_sphere=0.0;//映射到单位球面上的坐标
            if( fabs(rd) < std::numeric_limits<double>::epsilon())
            {//rd -->0
                x_sphere = 0.0; 
                y_sphere = 0.0;
                z_sphere = 1.0;
            }
            else
            {
                double cosphi = xd/rd;
                double sinphi = yd/rd;
                double theta  = CalcTheta(rd, m_k, m_thetaMax);
                x_sphere = sin(theta) * cosphi;
                y_sphere = sin(theta) * sinphi;
                z_sphere = cos(theta);
            }

            double ratio = (m_InvRs[2][0] * x_sphere + m_InvRs[2][1] * y_sphere + m_InvRs[2][2]* z_sphere);
            double scale_factor = (m_InvRs[2][0] * m_ts[0] + m_InvRs[2][1] * m_ts[1] + m_InvRs[2][2] * m_ts[2])/ratio;

            double X = (m_InvRs[0][0] * x_sphere + m_InvRs[0][1] * y_sphere + m_InvRs[0][2] * z_sphere) * scale_factor;
            double Y = (m_InvRs[1][0] * x_sphere + m_InvRs[1][1] * y_sphere + m_InvRs[1][2] * z_sphere) * scale_factor;
            double Z = (m_InvRs[2][0] * x_sphere + m_InvRs[2][1] * y_sphere + m_InvRs[2][2] * z_sphere) * scale_factor;

            pt3DsInWorld[j].d[0] = X + m_ptCameraLocation[0];
            pt3DsInWorld[j].d[1] = Y + m_ptCameraLocation[1];
            pt3DsInWorld[j].d[2] = Z + m_ptCameraLocation[2];

        }//for-each(j)

        //计算光球的半径
        double R[4];
        for(int k = 0; k < 4; k++)
        {
            double dX = pt3DsInWorld[k + 1].d[0] - pt3DsInWorld[0].d[0];
            double dY = pt3DsInWorld[k + 1].d[1] - pt3DsInWorld[0].d[1];
            double dZ = pt3DsInWorld[k + 1].d[2] - pt3DsInWorld[0].d[2];
            double distance = sqrt(dX*dX + dY*dY + dZ*dZ);
            R[k] = distance;
        }


        double R_mean = (R[0] + R[1] + R[2] + R[3]) / 4;


        double R_min = (std::numeric_limits<double>::max)();

        for(int k=0; k < 4; k ++)
        {
            if(R_min > R[k])
            {
                R_min = R[k];
            }
        }


        //在世界坐标系中, 计算重心与相机光心的的连线矢量与Z = 0 平面所成的夹角。
        TPoint3D V3D;
        V3D[0] = pt3DsInWorld[0][0] - m_ptCameraLocation[0]; //重心的世界坐标
        V3D[1] = pt3DsInWorld[0][1] - m_ptCameraLocation[1]; //重心的世界坐标
        V3D[2] = pt3DsInWorld[0][2] - m_ptCameraLocation[2];


        double norm_V3D = sqrt(V3D[0] * V3D[0] + V3D[1] * V3D[1] + V3D[2]*V3D[2]);

        if(fabs(norm_V3D) < std::numeric_limits<double>::epsilon())
        {//norm_V-->0
            return FALSE;
        }


        V3D /= norm_V3D;

        //平面 Z = 0 的法向量为(0,0,1)
        TPoint3D N;

        N[0] = 0;
        N[1] = 0;
        N[2] = 1;

        //计算V与N的夹角
        //dot(V,N) = |V||N|cos(α)
        double cosAlpha = V3D[0]*N[0] + V3D[1]*N[1] + V3D[2]*N[2];

        double tgAlpha = sqrt(1 - cosAlpha * cosAlpha) / cosAlpha;
        //double modify_length = R_mean * tgAlpha;
        double modify_length = R_min * tgAlpha;

        TPoint2D V2D;
        V2D[0] = V3D[0];
        V2D[1] = V3D[1];

        double norm_V2D = sqrt(V2D[0] * V2D[0] + V2D[1] * V2D[1]);

        V2D /= norm_V2D;


        double X_modify = V2D[0] * modify_length;
        double Y_modify = V2D[1] * modify_length;


        ptWorldPts[i].x = long(pt3DsInWorld[0].d[0] - X_modify);
        ptWorldPts[i].y = long(pt3DsInWorld[0].d[1] - Y_modify); 

    }//for-each(i)

    return TRUE;
}

*/

/*
//@功能:根据摄像头的视频坐标计算得出屏幕坐标
//@参数:pCentroid, 光斑的重心
//      pContourPoints, 指向光斑轮廓点的数组
//      nContourPointCount, 光斑轮廓点个数
//      ptWorld, 指向保存屏幕坐标的内存地址的指针
BOOL CGenericCameraModel::FromImage2World_WithLighgSpotContour(const POINT& ptCentroid,  const POINT* pContourPoints, int nContourPointCount, POINT* pPtWorld, POINT* debugPtCross) const
{

    //1.计算光斑重心反向映射到的世界坐标
    TPoint2D pt2DImageCentroid;
    pt2DImageCentroid.d[0] = double(ptCentroid.x)/16.0;
    pt2DImageCentroid.d[1] = double(ptCentroid.y)/16.0;



    TPoint2D pt2DWorldCentroid;

    if(!FromImage2World(&pt2DImageCentroid, 1, &pt2DWorldCentroid))
    {
        return FALSE;
    }



    //2.设在光斑重心映射的世界坐标处,垂直z=0平面放置一个长度为L的直径趋于0的光笔, 计算光笔轴向矢量在相机中所成的像。
    //像中的矢量, 表示光笔触点(与板面接触点)的像的方位。
    //static const double L = 30;
    static const double L = 0.0;

    TPoint3D pt3DWorldPenEnd;

    pt3DWorldPenEnd.d[0] = pt2DWorldCentroid[0];
    pt3DWorldPenEnd.d[1] = pt2DWorldCentroid[1];
    pt3DWorldPenEnd.d[2] = -L;

    TPoint2D pt2DImagePenEnd;


    ForwardProject3D(
        &pt3DWorldPenEnd,
        1, 
        m_Parameters,
        EXTENDED_MODEL_INTERNAL_PARAMS_COUNT,
        m_Rs,
        m_ts,
        &pt2DImagePenEnd);

    //计算光笔轴向矢量在相机照片中的矢量
    TPoint2D vPen;
    vPen.d[0] = pt2DImagePenEnd.d[0] - pt2DImageCentroid.d[0];
    vPen.d[1] = pt2DImagePenEnd.d[1] - pt2DImageCentroid.d[1];

    double norm_vPen = sqrt(vPen.d[0]*vPen.d[0] + vPen.d[1]*vPen.d[1]);




    //vPen.d[0] /= norm_vPen;
    //vPen.d[1] /= norm_vPen;

    TPoint2D pt2DContact;
    pt2DContact.d[0] = pt2DImageCentroid.d[0] - vPen.d[0]/2;
    pt2DContact.d[1] = pt2DImageCentroid.d[1] - vPen.d[1]/2;

    TPoint2D pt2DWorld;

    BOOL bRet = FromImage2World(&pt2DContact, 1, &pt2DWorld);

    if(bRet)
    {
        pPtWorld->x = LONG(pt2DWorld[0]);
        pPtWorld->y = LONG(pt2DWorld[1]);
    }


    
    
    //if(nContourPointCount < 8)
    //{
    //    //return FromImage2World_Aug(&ptCentroid,
    //    TPoint2D ptImage;
    //    ptImage[0] = (double)ptCentroid.x / 16.0;
    //    ptImage[1] = (double)ptCentroid.y / 16.0;

    //    TPoint2D ptWorld;

    //    BOOL bRet = FromImage2World(&ptImage, 1, &ptWorld);

    //    if(bRet)
    //    {
    //        pPtWorld->x = LONG(ptWorld[0]);
    //        pPtWorld->y = LONG(ptWorld[1]);
    //    }


    //    if(debugPtCross)
    //    {
    //        debugPtCross[0].x = long(ptImage.d[0]);
    //        debugPtCross[0].y = long(ptImage.d[1]);

    //        debugPtCross[1].x = long(ptImage.d[0]);
    //        debugPtCross[1].y = long(ptImage.d[1]);

    //    }


    //    return bRet;

    //}
    ////设相机中心在世界坐标系z=0平面上的垂足为F,其在相机中所成的像F'
    ////设光斑中心为G
    ////在光斑轮廓点中，查找在GF矢量上的两个点。



    //double Gx = double(ptCentroid.x) / 16.0;
    //double Gy = double(ptCentroid.y) / 16.0;

    //double Fx = m_ptOZFootImage[0];
    //double Fy = m_ptOZFootImage[1];

    //double L[3];
    ////直线FG的方程:
    ////L[0]*X + L[1]*Y + L[2] = 0;
    ////                ┌          ┐
    ////                │ I   J   K│
    ////L = cross(G,F) =│ Gx  Gy  1│
    ////                │ Fx  Fy  1│
    ////                └          ┘
    //L[0] = Gy - Fy;
    //L[1] = Fx - Gx;
    //L[2] = Gx*Fy - Gy*Fx;

    ////二维平面上，点P<Px, Py, 1>到直线的距离为
    ////
    ////d = abs(L[1]*Px + L[2]*Py + L[3]) /sqrt(L[1]^2 + L[2]^2);
    ////注意: sqrt(L[1]^2 + L[2]^2)是常量

    //struct TCrossContourElement
    //{
    //    //int nIndex;
    //    double distance;
    //    TPoint2D pt;
    //};

    //TCrossContourElement aryContourCrossed[4];//按距离从下到大排列

    //for(int i = 0; i < _countof(aryContourCrossed); i++)
    //{
    //    aryContourCrossed[i].distance = (std::numeric_limits<double>::max)();
    //}

    //for(int i= 0; i < nContourPointCount; i++)
    //{
    //    double contour_X = (double)(pContourPoints[i].x );
    //    double contour_Y = (double)(pContourPoints[i].y );

    //    double newDist = fabs(L[0] * contour_X + L[1] * contour_Y + L[2]);
    //    
    //    
    //    int nInsertPos = -1;
    //    for(int j = _countof(aryContourCrossed) - 1; j >= 0; j --)
    //    {//从后往前搜索插入位置

    //        double cmpDist  = aryContourCrossed[j].distance;
    //        if(newDist < cmpDist)
    //        {
    //             if(j == 0 ) nInsertPos = 0;
    //        }
    //        else //newDist >= cmpDist
    //        {
    //            if( j == _countof(aryContourCrossed) - 1) break;//大于最大的距离
    //            else       nInsertPos = j + 1;
    //            break;
    //        }


    //    }//for-each(j)

    //    if(-1 != nInsertPos)
    //    {
    //                //j+1是插入位置,j+1及以后的元素向后移动。
    //            for(int k = _countof(aryContourCrossed) - 1; k > nInsertPos; k --)
    //            {

    //                aryContourCrossed[k] =  aryContourCrossed[k -1];
    //            }

    //            //填充新的数据
    //            //aryContourIndex[j + 1].nIndex   = i;
    //            aryContourCrossed[nInsertPos].distance = newDist;
    //            aryContourCrossed[nInsertPos].pt.d[0] = contour_X;
    //            aryContourCrossed[nInsertPos].pt.d[1] = contour_Y;
    //    }





    //}//for-each(i);

    //TPoint2D img_ptA,img_ptB;

    //img_ptA  = aryContourCrossed[0].pt;


    //for(int i = 1;  i < _countof(aryContourCrossed); i++)
    //{
    //    img_ptB = aryContourCrossed[i].pt;

    //    double dx = img_ptB[0] - img_ptA[0];
    //    double dy = img_ptB[1] - img_ptA[1];

    //    double dist = sqrt(dx*dx + dy*dy);
    //    if(dist > 1.0)
    //    {
    //        break;
    //    }

    //}//for


    ////将A,B两点从图象坐标映射到校正平面
    //TPoint2D world_pt2DA, world_pt2DB;

    //if(!FromImage2World(&img_ptA, 1, &world_pt2DA))
    //{
    //    return FALSE;
    //}

    //if(!FromImage2World(&img_ptB, 1, &world_pt2DB))
    //{
    //    return FALSE;
    //}



    //TPoint2D img_ptCentroid, world_pt2DCentroid;
    //img_ptCentroid.d[0] = Gx ;
    //img_ptCentroid.d[1] = Gy;

    ////光斑重心映射到屏幕坐标
    //if(!FromImage2World(&img_ptCentroid, 1, &world_pt2DCentroid))
    //{
    //    return FALSE;
    //}


    ////计算光球的半径
    ////S△=1/2*c*H = 1/2(a+b+t)*r, r为三角形ABC的内切圆。
    ////

    ////AB = norm(A - B);
    ////AC = norm(A - C);
    ////BC = norm(B - C);        
    ////h = abs(camera_location(3));
    ////radius_calc = (AB*h)/(AB+AC+BC);%S△=1/2*c*H = 1/2(a+b+t)*r, r为三角形ABC的内切圆。

    //TPoint3D world_pt3DA, world_pt3DB,world_pt3DC;
    //world_pt3DA[0] = world_pt2DA[0];
    //world_pt3DA[1] = world_pt2DA[1];
    //world_pt3DA[2] = 0;

    //world_pt3DB[0] = world_pt2DB[0];
    //world_pt3DB[1] = world_pt2DB[1];
    //world_pt3DB[2] = 0;

    //world_pt3DC   = m_ptCameraLocation;



    //double AB = norm(world_pt3DA  - world_pt3DB);
    //double AC = norm(world_pt3DA  - world_pt3DC);
    //double BC = norm(world_pt3DB  - world_pt3DC);

    //double h = abs(world_pt3DC[2]);
    ////
    //double radius =  (AB * h)/(AB + AC + BC);
    ////radius = 8.5;
    //radius = 12;



    ////(_T("Radius = %g\n"), radius);

    ////在世界坐标系中, 计算重心与相机光心的的连线矢量与Z = 0 平面所成的夹角。
    //TPoint3D V3D;
    //V3D[0] = world_pt2DCentroid[0] - m_ptCameraLocation[0]; //重心的世界坐标
    //V3D[1] = world_pt2DCentroid[1] - m_ptCameraLocation[1]; //重心的世界坐标
    //V3D[2] = 0                     - m_ptCameraLocation[2];


    //double norm_V3D = sqrt(V3D[0] * V3D[0] + V3D[1] * V3D[1] + V3D[2]*V3D[2]);

    //if(fabs(norm_V3D) < std::numeric_limits<double>::epsilon())
    //{//norm_V-->0
    //    return FALSE;
    //}


    //V3D /= norm_V3D;

    ////平面 Z = 0 的法向量为(0,0,1)
    //TPoint3D N;

    //N[0] = 0;
    //N[1] = 0;
    //N[2] = 1;

    ////计算V与N的夹角
    ////dot(V,N) = |V||N|cos(α)
    //double cosAlpha = V3D[0]*N[0] + V3D[1]*N[1] + V3D[2]*N[2];

    //double tgAlpha = sqrt(1 - cosAlpha * cosAlpha) / cosAlpha;
    ////double modify_length = R_mean * tgAlpha;
    //double modify_length = radius * tgAlpha;

    //TPoint2D V2D;
    //V2D[0] = V3D[0];
    //V2D[1] = V3D[1];

    //double norm_V2D = sqrt(V2D[0] * V2D[0] + V2D[1] * V2D[1]);

    //V2D /= norm_V2D;



    //double X_modify = V2D[0] * modify_length;
    //double Y_modify = V2D[1] * modify_length;


    //if(pPtWorld)
    //{
    //    pPtWorld->x = long(world_pt2DCentroid[0] - X_modify);
    //    pPtWorld->y = long(world_pt2DCentroid[1] - Y_modify); 
    //}

    //if(debugPtCross)
    //{
    //    debugPtCross[0].x = long(img_ptA.d[0] + .5);
    //    debugPtCross[0].y = long(img_ptA.d[1] + .5);

    //    debugPtCross[1].x = long(img_ptB.d[0] + .5);
    //    debugPtCross[1].y = long(img_ptB.d[1] + .5);

    //    debugPtCross[2].x = long(img_ptCentroid.d[0] + .5);
    //    debugPtCross[2].y = long(img_ptCentroid.d[1] + .5);

    //    
    //    debugPtCross[3].x = long(m_ptOZFootImage.d[0] + .5);
    //    debugPtCross[3].y = long(m_ptOZFootImage.d[1] + .5) ;


    //}

     //TCHAR szDebugText[1024];;
    //_stprintf_s(szDebugText, _countof(szDebugText), _T("vpen_x=%f, vpen_y=%f, pen vector length=%f\n"), vPen.d[0], vPen.d[1], norm_vPen);

    //OutputDebugString(szDebugText);
    return TRUE;
}
*/


//@功能:映射3D世界坐标系中的点到相机图片上
//@参数:ptWorld, 3D世界坐标数组
//      nPtNumber, 3D世界坐标个数
//      ptImage, 输出的2D图像坐标
BOOL CGenericCameraModel::MapWorldToImage(const TPoint3D* ptWorld, int nPtNumber, TPoint2D* ptImage) const
{
    BOOL bRet = ForwardProject3D(
        ptWorld,
        nPtNumber, 
        m_Parameters,
        EXTENDED_MODEL_INTERNAL_PARAMS_COUNT,
        m_Rs,
        m_ts,
        ptImage);

    return bRet;
}


//@功能:初始镜头内部参数
BOOL CGenericCameraModel::InitInteralParameters(EGenericCameraProjectionType eProjectType, double dbThrowRatio, int nImageWidth, int nImageHeight)
{
    //按照相机数学模型的级数展开确定k1,k2的初始值。
    //r(θ) = k1*θ + k2*θ^3 + k3*θ^5 + k4*θ^7 + k7*θ^9.
    switch(eProjectType)
    {
    case E_CAMERA_PROJECTION_PERSPECTIVE:
        //透射投影类型, r = tan(θ) for all |θ| < π/2
        //tan(θ) = θ + 1/3*θ^3 + 2/15*θ^3 + ...
        //m_K1 = 1;
        //m_K2 = (double)1/(double)3;
        //拟合的参数和级数展开的不一致, 因为模型只有θ和θ^3项, 为了
        //拟合误差最小, 必然不等于原始级数的系数。
        //m_k1 =  0.759655386615938;
        //m_k2  = 0.893857725455975;

        //m_K1 =  0.84335623516814;
        //m_K2 =  0.76106812037112;
        //m_K1 =  0.999999992982425;
        //m_K2 =  0.333403069525502;

		m_k1 = 0.578085003266279;
		m_k2 = 1.146184423648731;
        break;


    case E_CAMERA_PROJECTION_STEREOGRAPHIC:
        //立体投影      r = 2tan(θ/2)
        //tan(θ/2) = θ/2 + 1/3*(θ/2)^3 + 2/15*(θ/2)^3 + ...for all |θ/2| < π/2
        m_k1 = 1;
        m_k2 = (double)1/(double)12;
        break;

    case E_CAMERA_PROJECTION_EQUIDISTANCE:
        //等距投影      r = θ;
        //
        m_k1 = 1;
        m_k2 = 0;
        break;


    case E_CAMERA_PROJECTION_EQUISOLD_ANGLE:
        // r = 2sin(θ/2)
        //sin(θ/2) = (θ/2) - 1/(3!)*(θ/2)^3 + 1/(5!)*(θ/2)^5 - ...for all  θ
        m_k1 = 1;
        m_k2 = double(-1)/(double)(3*8);
        break;

    case E_CAMERA_PROJECTION_ORTHOGONAL:
        //正交投影      r = sin(θ)
        //sin(θ) = θ - 1/(3!)*θ^3 + 1/(5!)*θ^5 - ...for all  θ
        m_k1 = 1;
        m_k2 =(double)(-1)/(double)(2*3);
        break;

    default:
        return FALSE;
    }

    m_k3 = m_k4 = m_k5 = 0.0;//radial模型的其他参数初始化为0

    if(dbThrowRatio != 0.0)
    {
        //根据投射比计算θmax
        //投影距离 = 屏幕宽度×投射比
        //640:480 = 4:3
        //S为投射距离，W,H为画面宽度, D为对角线长度
        //S/W = tr, throw ratio
        //  W = S/tr;
        //W:H = 4:3
        //对角线长度 D = sqrt(W^2 + H^2) = 5/4*W
        //tr = S/W, S为相机到投影画面的距离
        //θ=2arctg[D/(2*S)] = 2atan[sqrt(W^2 + H^2) /(2*tr*W)] = 2atan(0.5/(tr) * sqrt(1+(H/W)^2);
        double H_W_Ratio = 3.0 / 4.0;
        m_thetaMax = atan2(0.5*sqrt(H_W_Ratio * H_W_Ratio + 1)/dbThrowRatio, 1);

    }
    else
    {//
        m_thetaMax = M_PI/2.0;
    }

    m_throwRatio = dbThrowRatio;
    if(m_throwRatio == 0.0)
    {
        m_throwRatio = 0.21;
    }

    double r_max = m_k1 * m_thetaMax + m_k2 * m_thetaMax * m_thetaMax * m_thetaMax;
    double image_diagnoal_length = sqrt(double(nImageWidth * nImageWidth + nImageHeight * nImageHeight));
    m_mu = (image_diagnoal_length / 2.0) / r_max;
    m_mv = m_mu;

    m_u0 = (double)nImageWidth  / (double)2;
    m_v0 = (double)nImageHeight / (double)2;

    //非对称畸变计算公式如下    
    //Δr = (g(1)*θ + g(2)*θ^3 + g(3)*θ^5) * (i1*cosφ + i2*sinφ + i3*cos2φ + i4*sin2φ)
    //Δt = (h(1)*θ + h(2)*θ^3 + h(3)*θ^5) * (j1*cosφ + j2*sinφ + j3*cos2φ + j4*sin2φ)
    //如果g,h,i,j均为0,计算出的导数恒等于0, 无法用Levenber-Marquardt算法迭代求解
    //因此,初始化为一个溦小量。
    m_g[0] = 1e-7;
    m_g[1] = 1e-7;
    m_g[2] = 1e-7;

    m_i[0] = 1e-7;
    m_i[1] = 1e-7;
    m_i[2] = 1e-7;
    m_i[3] = 1e-7;

    m_h[0] = 1e-7;
    m_h[1] = 1e-7;
    m_h[2] = 1e-7;


    m_j[0] = 1e-7;
    m_j[1] = 1e-7;
    m_j[2] = 1e-7;
    m_j[3] = 1e-7;

    return TRUE;
}


//@功能:返回相机模型参数个数
int CGenericCameraModel::GetParameterCount()const
{
    return _countof(m_Parameters);
}

//@功能:返回相机模型参数数组
const double* CGenericCameraModel::GetParameters() const
{
    return &m_Parameters[0];
}

//@功能:设置模型参数
void CGenericCameraModel::SetParameters(const double* pParameters, int nParametersCount)
{
    if(nParametersCount > _countof(m_Parameters))
    {
        nParametersCount = _countof(m_Parameters);
    }

    memcpy(m_Parameters, pParameters, sizeof(double)*nParametersCount);



    //根据旋转矩阵计算其逆矩阵
    InvR(m_Rs, m_InvRs);


    //相机在世界坐标系中的坐标
    m_ptCameraLocation[0] = -(m_InvRs[0][0] * m_ts[0] + m_InvRs[0][1] * m_ts[1] + m_InvRs[0][2] * m_ts[2]);
    m_ptCameraLocation[1] = -(m_InvRs[1][0] * m_ts[0] + m_InvRs[1][1] * m_ts[1] + m_InvRs[1][2] * m_ts[2]);
    m_ptCameraLocation[2] = -(m_InvRs[2][0] * m_ts[0] + m_InvRs[2][1] * m_ts[1] + m_InvRs[2][2] * m_ts[2]);



    //相机中心在在校正平面上的垂足
    TPoint2D ptCameraFootOnCalibratePlane;
    ptCameraFootOnCalibratePlane[0] =  m_ptCameraLocation[0];
    ptCameraFootOnCalibratePlane[1] =  m_ptCameraLocation[1];



    //相机中心在世界坐标系z=0平面上的垂足F,在相机中所成的像F'
    ForwardProject2D(
        &ptCameraFootOnCalibratePlane,
        1,
        m_Parameters,
        EXTENDED_MODEL_INTERNAL_PARAMS_COUNT,
        m_Rs,
        m_ts,
        &m_ptOZFootImage);


    ConstructRToThetaLUT(this->m_cxImage, this->m_cyImage);

}


CGenericCameraModel& CGenericCameraModel::operator= (const CGenericCameraModel& right)
{
    memcpy(m_Parameters, right.m_Parameters, sizeof(m_Parameters));
    return *this;
}



const TPoint3D& CGenericCameraModel::GetCameraWorldPosition()const
{
    return m_ptCameraLocation;
}


//@功能:返回相机的投射比
double CGenericCameraModel::GetThrowRatio() const 
{  
    return  m_throwRatio;
}

//@功能:返回相机镜头的中心在画面中的坐标
TPoint2D CGenericCameraModel::GetLensCenterInImage()const
{
    TPoint2D c;
    c.d[0] = m_u0;
    c.d[1] = m_v0;
    return c;

}


//@功能:获取校正时的屏幕尺寸信息
 RECT CGenericCameraModel::GetMonitorRect()const
{
    RECT rcMonitor;
    rcMonitor.left   = long(m_Monitor_Left);
    rcMonitor.right  = long(m_Monitor_Right);
    rcMonitor.top    = long(m_Monitor_Top);
    rcMonitor.bottom = long(m_Monitor_Bottom);

    return rcMonitor;
}



//@功能:获取视频图片的尺寸
 SIZE CGenericCameraModel::GetImageSize()const
{
    SIZE sizeOfImage;
    sizeOfImage.cx = long(m_cxImage);
    sizeOfImage.cy = long(m_cyImage);
    return sizeOfImage;
}


