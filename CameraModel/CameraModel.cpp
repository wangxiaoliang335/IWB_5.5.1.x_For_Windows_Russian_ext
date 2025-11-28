// CameraModel.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "../inc/CameraModel.h"
#include "./Routines.h"
#include "./Rodrigues.h"

/////////////////////////////////////////////////////////////////////////
//相机模型



//@功 能:根据相机的针孔模型，对相机进行校正,计算出相机的内部参数和外部参数
//@参 数:pPts2DInWorld, 在世界坐标系中的校正点3D坐标
//       pPt2DInImg, 在图像中对应的每个校正点的2D坐标 
//       nPtNumber  , 校正点个数
//@返回值:成功, TRUE
//        失败, FALSE
BOOL CPinHoleCameraModel::Calibrate(const TPoint2D* pPts2DInWorld, const TPoint2D* pPt2DInImg, int nPtNumber, int nCxImg, int nCyImg)
{

    BOOL bStatus = FALSE;
    if(NULL == pPts2DInWorld || NULL == pPt2DInImg || nPtNumber <=0 ) return FALSE;

    //when using a single image, please, no principal point estimation!!! 
    //init_intrinsic_param
    //compute_extrinsic_init
    //compute_extrinsic_refine

    //IntrinsicParameters  intrinsic;
    //DistortionParameters distortion;

    //计算相机内部的初始参数
    InitIntrinsicParam(
        pPts2DInWorld,
        pPt2DInImg,
        nPtNumber,
        nCxImg,
        nCyImg,
        m_oIntrinsicParams,
        m_oDistortionParams);

    m_oIntrinsicParams.alpha  = 0.0;
    //m_oIntrinsicParams.c.d[0] = 319.5;
    //m_oIntrinsicParams.c.d[1] = 239.5;
    //m_oIntrinsicParams.f.d[0] = 124;
    //m_oIntrinsicParams.f.d[1] = 124;
    m_oDistortionParams.kc1   = -0.0054285891395760 ;


    MatlabSimu::Matrix<double> R;

    TPoint3D* pPt3DInWorld = new TPoint3D[nPtNumber];
    for(int i=0;i < nPtNumber; i++)
    {
        pPt3DInWorld[i].d[0] = pPts2DInWorld[i].d[0];
        pPt3DInWorld[i].d[1] = pPts2DInWorld[i].d[1];
        pPt3DInWorld[i].d[2] = 1.0;
    }

    //计算相机外部的初始参数
    ComputeExtrinsincInit(
        pPt3DInWorld,
        pPt2DInImg,
        nPtNumber,
        m_oIntrinsicParams,
        m_oDistortionParams,
        m_oExtrinsicParams);

    //printf("\
    //initial  params:\n\
    //focal     length:% 21.15e,% 21.15e\n\
    //principal  point:% 21.15e,% 21.15e\n\
    //            skew:% 21.15e\n\
    //distortion params:% 21.15e,% 21.15e,% 21.15e,% 21.15e,% 21.15e\n\
    //omega:(% 21.15e,% 21.15e,% 21.15e)\n\
    //t:(% 21.15e,% 21.15e,% 21.15e)\n",
    //m_oIntrinsicParams.f[0],
    //m_oIntrinsicParams.f[1],
    //m_oIntrinsicParams.c[0],
    //m_oIntrinsicParams.c[1],
    //m_oIntrinsicParams.alpha,
    //m_oDistortionParams.kc1,
    //m_oDistortionParams.kc2,
    //m_oDistortionParams.kc3,
    //m_oDistortionParams.kc4,
    //m_oDistortionParams.kc5,
    //m_oExtrinsicParams.m_omc[0],
    //m_oExtrinsicParams.m_omc[1],
    //m_oExtrinsicParams.m_omc[2],
    //m_oExtrinsicParams.m_tc[0],
    //m_oExtrinsicParams.m_tc[1],
    //m_oExtrinsicParams.m_tc[2]);





    //保持内部参数不变，迭代优化外部参数初值
    ComputeExtrinsicRefine(
        pPt3DInWorld,
        pPt2DInImg,
        nPtNumber,
        m_oIntrinsicParams,
        m_oDistortionParams,
        m_oExtrinsicParams);


    
    printf("\
    initial  params:\n\
    focal     length:% 21.15e,% 21.15e\n\
    principal  point:% 21.15e,% 21.15e\n\
                skew:% 21.15e\n\
    distortion params:% 21.15e,% 21.15e,% 21.15e,% 21.15e,% 21.15e\n\
    omega:(% 21.15e,% 21.15e,% 21.15e)\n\
    t:(% 21.15e,% 21.15e,% 21.15e)\n",
    m_oIntrinsicParams.f[0],
    m_oIntrinsicParams.f[1],
    m_oIntrinsicParams.c[0],
    m_oIntrinsicParams.c[1],
    m_oIntrinsicParams.alpha,
    m_oDistortionParams.kc1,
    m_oDistortionParams.kc2,
    m_oDistortionParams.kc3,
    m_oDistortionParams.kc4,
    m_oDistortionParams.kc5,
    m_oExtrinsicParams.m_omc[0],
    m_oExtrinsicParams.m_omc[1],
    m_oExtrinsicParams.m_omc[2],
    m_oExtrinsicParams.m_tc[0],
    m_oExtrinsicParams.m_tc[1],
    m_oExtrinsicParams.m_tc[2]);


    //

    //外部参数和内部参数一起参与非线性迭代
    //注意c0,c1和alpha不参与迭代
    //
    //迭代求解的参数列表为
    //[f0,f1, k1,k2,k3,k4,k5, omega(0), omgea(1), omega(2), t(0),t(1),t(2)]
    //共13个参数
    const int PARAM_NUMBER = 13;





    int iter = 0;
    double change = 1.0;

    MatlabSimu::Matrix<double> dXp_dOmega;
    MatlabSimu::Matrix<double> dXp_dT    ;
    MatlabSimu::Matrix<double> dXp_df    ;
    MatlabSimu::Matrix<double> dXp_dc    ;
    MatlabSimu::Matrix<double> dXp_dk    ;
    MatlabSimu::Matrix<double> dXp_dalpha;

    MatlabSimu::Matrix<double> J (2*nPtNumber,PARAM_NUMBER);
    MatlabSimu::Matrix<double> JT(PARAM_NUMBER,2*nPtNumber);
    int LD = 2*nPtNumber;//Leading dimension of each column

    MatlabSimu::Matrix<double>      JTJ(PARAM_NUMBER, PARAM_NUMBER);
    MatlabSimu::Matrix<double>  JTJ_inv(PARAM_NUMBER,PARAM_NUMBER);
    MatlabSimu::Matrix<double>  param_innov(PARAM_NUMBER,1) ;//更新差值
    MatlabSimu::Matrix<double>  ex(2*nPtNumber,1);


    TPoint2D* pXp = new TPoint2D[nPtNumber];


    //Maximum number of  iterations in the gradient descent
    //
    const int MAX_ITER =  30;

    //set alpha_smooth = 1; for steepest gradient descent
    double smooth_coef = 0.9; //Levenberg-Marquardt算法的平滑系数


    //Conditioning threshold for view rejection
    double thresh_cond = 1e6;
    while(iter < MAX_ITER && change > 1e-9)
    {

        //映射校正点3D坐标到2D图像坐标
        ProjectPoints(
            pPt3DInWorld,
            nPtNumber,
            m_oIntrinsicParams,
            m_oExtrinsicParams,
            m_oDistortionParams,
            pXp,
            dXp_dOmega,
            dXp_dT,
            dXp_df,
            dXp_dc,
            dXp_dk,
            dXp_dalpha);

        //dXp_dOmega.Print("dXp_dOemga\n");
        //dXp_dT.Print("dXp_dT\n");
        //dXp_df.Print("dXp_df\n");
        //dXp_dc.Print("dXp_dc\n");
        //dXp_dk.Print("dXp_dk\n");
        //dXp_dalpha.Print("dXp_dalpha\n");


        //构建雅克比矩阵
        double* pdXp_dOmega = dXp_dOmega.GetDataPtr();
        double* pdXp_dT     = dXp_dT.GetDataPtr();
        double* pdXp_df     = dXp_df.GetDataPtr();
        double* pdXp_dc     = dXp_dc.GetDataPtr();
        double* pdXp_dk     = dXp_dk.GetDataPtr();
        double* pdXp_dalpha = dXp_dalpha.GetDataPtr();

        double* pJ          = J.GetDataPtr();
        double*pJT          = JT.GetDataPtr();


        double* pex        = ex.GetDataPtr();

        //[f0,f1, c0,c1, alpha_c, k1,k2,k3,k4,k5, omega(0), omgea(1), omega(2), t(0),t(1),t(2)]
        //构建雅克比矩阵
        for(int i = 0; i < nPtNumber; i++)
        {
            //偶数行
            int r = 2*i;
           
            for(int j=0; j<2;j++)
            {
                r += j;
                pJT[0  + r*PARAM_NUMBER] = pJ[r + 0 * LD ] = pdXp_df     [r + 0*LD];//dxp/df0
                pJT[1  + r*PARAM_NUMBER] = pJ[r + 1 * LD ] = pdXp_df     [r + 1*LD];//dxp/df1
                pJT[2  + r*PARAM_NUMBER] = pJ[r + 2 * LD ] = pdXp_dk     [r + 0*LD];//dxp/dk1
                pJT[3  + r*PARAM_NUMBER] = pJ[r + 3 * LD ] = pdXp_dk     [r + 1*LD];//dxp/dk2
                pJT[4  + r*PARAM_NUMBER] = pJ[r + 4 * LD ] = pdXp_dk     [r + 2*LD];//dxp/dk3
                pJT[5  + r*PARAM_NUMBER] = pJ[r + 5 * LD ] = pdXp_dk     [r + 3*LD];//dxp/dk4
                pJT[6  + r*PARAM_NUMBER] = pJ[r + 6 * LD ] = pdXp_dk     [r + 4*LD];//dxp/dk5 
                pJT[7  + r*PARAM_NUMBER] = pJ[r + 7 * LD ] = pdXp_dOmega [r + 0*LD];//dxp/domgea(0)
                pJT[8  + r*PARAM_NUMBER] = pJ[r + 8 * LD ] = pdXp_dOmega [r + 1*LD];//dxp/domgea(1)
                pJT[9  + r*PARAM_NUMBER] = pJ[r + 9 * LD ] = pdXp_dOmega [r + 2*LD];//dxp/domgea(2)
                pJT[10 + r*PARAM_NUMBER] = pJ[r +10 * LD ] = pdXp_dT     [r + 0*LD];//dxp/dt(0)
                pJT[11 + r*PARAM_NUMBER] = pJ[r +11 * LD ] = pdXp_dT     [r + 1*LD];//dxp/dt(1)
                pJT[12 + r*PARAM_NUMBER] = pJ[r +12 * LD ] = pdXp_dT     [r + 2*LD];//dxp/dt(2)
            }

            double du = pPt2DInImg[i].d[0] - pXp[i].d[0];
            double dv = pPt2DInImg[i].d[1] - pXp[i].d[1];

            pex[i*2   ] = du;
            pex[i*2 +1] = dv;

        }//for

        JTJ = JT*J;


        int nInfo = MatlabSimu::Inverse(JTJ, JTJ_inv);

        if(nInfo != 0)
        {
            if(nInfo < 0)
                throw std::exception("invalid arguments");

            if(nInfo > 0)
                throw std::exception("sigular matrix");
            break;
        }

        param_innov = JTJ_inv*JT*ex;
        param_innov *= (1 - smooth_coef);

        //param_innov.Print();

        //[f0,f1, c0,c1, alpha_c, k1,k2,k3,k4,k5, omega(0), omgea(1), omega(2), t(0),t(1),t(2)]
        m_oIntrinsicParams.f[0]  += param_innov(0,0);
        m_oIntrinsicParams.f[1]  += param_innov(1,0);

       
        m_oDistortionParams.kc1  += param_innov(2,0);
        m_oDistortionParams.kc2  += param_innov(3,0);
        m_oDistortionParams.kc3  += param_innov(4,0);
        m_oDistortionParams.kc4  += param_innov(5,0);
        m_oDistortionParams.kc5  += param_innov(6,0);

        m_oExtrinsicParams.m_omc[0] += param_innov(7,0);
        m_oExtrinsicParams.m_omc[1] += param_innov(8,0);
        m_oExtrinsicParams.m_omc[2] += param_innov(9,0);

        m_oExtrinsicParams.m_omc[0] += param_innov(10,0);
        m_oExtrinsicParams.m_omc[1] += param_innov(11,0);
        m_oExtrinsicParams.m_omc[2] += param_innov(12,0);


        //根据计算得到的内部参数，重新计算和优化外部参数
        ComputeExtrinsincInit(
            pPt3DInWorld,
            pPt2DInImg,
            nPtNumber,
            m_oIntrinsicParams,
            m_oDistortionParams,
            m_oExtrinsicParams);



        //保持内部参数不变，迭代优化外部参数初值
        ComputeExtrinsicRefine(
            pPt3DInWorld,
            pPt2DInImg,
            nPtNumber,
            m_oIntrinsicParams,
            m_oDistortionParams,
            m_oExtrinsicParams);

        smooth_coef *= smooth_coef;

        //迭代次数
        iter += 1;

    }//while

    printf("\
    calibrate  results:\n\
    focal     length:% 21.15e,% 21.15e\n\
    principal  point:% 21.15e,% 21.15e\n\
                skew:% 21.15e\n\
    distortion params:% 21.15e,% 21.15e,% 21.15e,% 21.15e,% 21.15e\n\
    omega:(% 21.15e,% 21.15e,% 21.15e)\n\
    t:(% 21.15e,% 21.15e,% 21.15e)\n",
    m_oIntrinsicParams.f[0],
    m_oIntrinsicParams.f[1],
    m_oIntrinsicParams.c[0],
    m_oIntrinsicParams.c[1],
    m_oIntrinsicParams.alpha,
    m_oDistortionParams.kc1,
    m_oDistortionParams.kc2,
    m_oDistortionParams.kc3,
    m_oDistortionParams.kc4,
    m_oDistortionParams.kc5,
    m_oExtrinsicParams.m_omc[0],
    m_oExtrinsicParams.m_omc[1],
    m_oExtrinsicParams.m_omc[2],
    m_oExtrinsicParams.m_tc[0],
    m_oExtrinsicParams.m_tc[1],
    m_oExtrinsicParams.m_tc[2]);


    //计算内部参数矩阵的逆矩阵
    //                    ┌                                      ┐
    //                    │1/f0  -α/(f0*f1) -c0/f0+α*c1/(f0*f1)│
    //inverse_intrinsic = │ 0       1/f1       -c1/f1            │
    //                    │ 0        0            1              │
    //                    └                                      ┘
    m_InvIntrinsic[0][0] = 1/m_oIntrinsicParams.f[0];
    m_InvIntrinsic[0][1] = -m_oIntrinsicParams.alpha /(m_oIntrinsicParams.f[0]*m_oIntrinsicParams.f[1]);
    m_InvIntrinsic[0][2] = -m_oIntrinsicParams.c[0]/m_oIntrinsicParams.f[0] - m_oIntrinsicParams.c[1]*m_InvIntrinsic[0][1];
    m_InvIntrinsic[1][0] = 0.0;
    m_InvIntrinsic[1][1] = 1/m_oIntrinsicParams.f[1];
    m_InvIntrinsic[1][2] = -m_oIntrinsicParams.c[1]/m_oIntrinsicParams.f[1];
    m_InvIntrinsic[2][0] = 0.0;
    m_InvIntrinsic[2][1] = 0.0;
    m_InvIntrinsic[2][2] = 1.0;



        //映射校正点3D坐标到2D图像坐标
        ProjectPoints(
            pPt3DInWorld,
            nPtNumber,
            m_oIntrinsicParams,
            m_oExtrinsicParams,
            m_oDistortionParams,
            pXp,
            dXp_dOmega,
            dXp_dT,
            dXp_df,
            dXp_dc,
            dXp_dk,
            dXp_dalpha);




    delete[] pXp;
    delete[] pPt3DInWorld;


    return bStatus;
}


//@功  能:从图像坐标影映射为屏幕坐标
//@返回值:映射后的屏幕坐标
TPoint2D CPinHoleCameraModel::Image2Screen(const TPoint2D& ptImage)
{
    double u = ptImage[0];
    double v = ptImage[1];
    
    //Step 1
    //┌  ┐   ┌                                      ┐┌  ┐
    //│xx│   │1/f0  -α/(f0*f1) -c0/f0+α*c1/(f0*f1)││u │
    //│yy│ = │ 0       1/f1       -c1/f1            ││v │
    //│1 │   │ 0        0            1              ││1 │
    //└  ┘   └                                      ┘└  ┘
    double xx = m_InvIntrinsic[0][0]*u + m_InvIntrinsic[0][1]*v + m_InvIntrinsic[0][2];
    double yy = m_InvIntrinsic[1][0]*u + m_InvIntrinsic[1][1]*v + m_InvIntrinsic[1][2];
    
    //Step 2
    //xx = a * (1 + kc(1)*r^2 + kc(2)*r^4 + kc(5)*r^6)      +      2*p1*a*b + p2*(r^2 + 2*a^2);
    //yy = b * (1 + kc(1)*r^2 + kc(2)*r^4 + kc(5)*r^6)      +      p1*(r^2 + 2*b^2) + 2*p2*a*b;
    //利用迭代法,求出a,b
    double a = xx, b=yy;

    double k1 = m_oDistortionParams.kc1;
    double k2 = m_oDistortionParams.kc2;
    double k3 = m_oDistortionParams.kc5;
    double p1 = m_oDistortionParams.kc3;
    double p2 = m_oDistortionParams.kc4;

      for(int kk=0; kk < 10; kk++)
      {
          double a2 = a*a;
          double b2 = b*b;
          double ab = a*b;
          double r2 = a2 + b2;
          double r4 = r2*r2;
          double r6 = r2*r4;
          double k_radial = 1 + k1*r2 + k2*r4 + k3*r6;
          double dx = 2*p1*ab       + p2*(r2 + 2*a2) ;
          double dy = p1*(r2+ 2*b2) + 2*p2*ab        ;

          a = (xx - dx)/k_radial;
          b = (yy - dy)/k_radial;
      }//for

      //Step 3
     //┌    ┐     ┌ ┐  ┌  ┐
     //│a*z │     │x│  │t0│
     //│b*z │ = R*│y│ +│t1│
     //│z   │     │1│  │t2│
     //└    ┘     └ ┘  └  ┘
     //
     //┌                         ┐┌ ┐  ┌                      ┐
     //│R00-a*R20    R01 - a*R21 ││x│  │-R02-t0 + a*R22 + a*t2│
     //│                         ││ │ =│                      │
     //│R10-b*R20    R11 - b*R21 ││y│  │-R12-t1 + b*R22 + b*t2│
     //└                         ┘└ ┘  └                      ┘

      const double (&R_T)[3][3] = m_oExtrinsicParams.R_T;
      const TPoint3D& t         = m_oExtrinsicParams.m_tc;

     double C[2][2], B[2];
     C[0][0] = R_T[0][0] - a*R_T[0][2];
     C[0][1] = R_T[1][0] - a*R_T[1][2];
     C[1][0] = R_T[0][1] - b*R_T[0][2];
     C[1][1] = R_T[1][1] - b*R_T[1][2];
     B[0] = -R_T[2][0] - t[0] + a*(R_T[2][2] + t[2]);
     B[1] = -R_T[2][1] - t[1] + b*(R_T[2][2] + t[2]);

     //解方程CX=B
     //
     //
     double detC =  C[0][0]*C[1][1] - C[0][1]*C[1][0];
     double x = (B[0]*C[1][1] - C[0][1]*B[1])/detC;
     double y = (C[0][0]*B[1] - B[0]*C[1][0])/detC;

      TPoint2D ptScreen;
      ptScreen[0] = x;
      ptScreen[1] = y;

    return ptScreen;

}




