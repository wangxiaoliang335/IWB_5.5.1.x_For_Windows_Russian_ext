#include "stdafx.h"
#define _USE_MATH_DEFINES
#include "Math.h"
#include <Matrix.h>
#include "Rodrigues.h"
#include <float.h>

//Robust sign function
#define SGN(x) ((x > 1e-4)?1:0 - (x < -1e-4)?-1:0)

//@功能:根据旋转矩阵计算出旋转轴矢量， 旋转轴的模为旋转角度
//@参数:pRoationMatrix, 输入参数, 指向3*3旋转矩阵的首地址的指针, 旋转矩阵的元素按列依次排序
//      pOmega, 输出参数, 根据旋转矩阵计算得出的3D旋转矢量
//      dOmdR, 输出参数, 雅克比矩阵,为3×9的矩阵
//              ┌                                                  ┐
//              │dOmega(1)/dR11, Omega(1)/dR12, .. , Omega(1)/dR33│
//      dOmdR = │dOmega(2)/dR11, Omega(2)/dR12, .. , Omega(2)/dR33│
//              │dOmega(3)/dR11, Omega(3)/dR12, .. , Omega(3)/dR33│
//              └                                                  ┘
void Rodrigues(const double* pR, ROTATION_VECTOR& omega, double* dOmega_dR)
{

    MatlabSimu::Matrix<double>  R(pR,3,3);

    MatlabSimu::Matrix<double> S, U,V;

    MatlabSimu::SVD(R,&S, &U, &V);

    //project the rotation matrix to SO(3)， SO(3):special orthogonal group. 旋转群
    R = U * MatlabSimu::Transpose(V);

    //R.Print("R=U*V':\n");


    //计算公式
    //ω=vθ
    //其中
    //θ=arccos[(trace(R)-1)/2]
    //                 ┌               ┐
    //                 │R(3,2) - R(2,3)│
    //v = 1/(2*sinθ) *│R(1,3) - R(3,1)│
    //                 │R(2,1) - R(1,2)│
    //                 └               ┘
    //
    const double& R11 = R(0, 0);
    const double& R21 = R(1, 0);
    const double& R31 = R(2, 0);

    const double& R12 = R(0, 1);
    const double& R22 = R(1, 1);
    const double& R32 = R(2, 1);

    const double& R13 = R(0, 2);
    const double& R23 = R(1, 2);
    const double& R33 = R(2, 2);

    double trace_R   = R11  + R22 + R33;
    double tr        = (trace_R - 1)/2;
    double theta     = acos(tr);
    double cos_theta = tr;
    double sin_theta = sin(theta);


    //特殊情形θ
    //
    if(sin_theta < 1e-4)//θ→0 或者θ→π
    {

        //1.θ→0
        //tr > 0, 意味着cosθ> 0, θ在0附近
        if(tr > 0)
        {
            //此时旋转轴为0矢量
            omega.d[0] = 0.0;
            omega.d[1] = 0.0;
            omega.d[2] = 0.0;


            //                          ┌               ┐
            //                          │R(3,2) - R(2,3)│
            //ω = v*θ = θ/(2*sinθ) *│R(1,3) - R(3,1)│
            //                          │R(2,1) - R(1,2)│
            //                          └               ┘
            //θ→0时,θ/sinθ→1
            //          ┌               ┐
            //          │R(3,2) - R(2,3)│
            //ω ≈ 1/2*│R(1,3) - R(3,1)│
            //          │R(2,1) - R(1,2)│
            //          └               ┘
            //
            //
            //
            //∂ω/∂(R11, R21,..,R33)=
            // ┌                               ┐
            // │0  0    0   0  0 1/2  0  -1/2 0│
            // │0  0  -1/2  0  0  0  1/2   0  0│
            // │0 1/2   0 -1/2 0  0   0    0  0│
            // └                               ┘
            //
            //
            //
            if(dOmega_dR)
            {
                double* pJocobi =  dOmega_dR;

                pJocobi[0 ] = 0   ;//1列
                pJocobi[1 ] = 0   ;
                pJocobi[2 ] = 0   ;
                          
                pJocobi[3 ] = 0   ;//2列
                pJocobi[4 ] = 0   ;
                pJocobi[5 ] = 1/2 ;
                          
                pJocobi[6 ] = 0   ;//3列
                pJocobi[7 ] = -1/2;
                pJocobi[8 ] = 0   ;

                pJocobi[9 ] = 0   ;//4列
                pJocobi[10] = 0   ;
                pJocobi[11] = -1/2;

                pJocobi[12] = 0   ;//5列
                pJocobi[13] = 0   ;
                pJocobi[14] = 0   ;
                          
                pJocobi[15] = 1/2 ;//6列
                pJocobi[16] = 0   ;
                pJocobi[17] = 0   ;
                          
                pJocobi[18] = 0   ;//7列 
                pJocobi[19] = 1/2 ;
                pJocobi[20] = 0   ;
                          
                pJocobi[21] = -1/2;//8列
                pJocobi[22] = 0   ;
                pJocobi[23] = 0   ;
                          
                pJocobi[24] = 0   ;//9列
                pJocobi[25] = 0   ;
                pJocobi[26] = 0   ;

             }//if

            return;

        }//if(tr > 0)
        else
        {
            //2.θ→π情形
            //θ=norm(ω)
            //R = cos(θ)*I + sin(θ)/θ*[ω]x + (1-cosθ)/θ^2*[ω]x*[ω]x^T
            //θ→π时，cos(θ)=-1，sin(θ)=0
            //令v = ω/θ, tranpose(T) = v^^T
            //R = -I + 2*v*v^^T
            //v*v^^T = (R+I)/2
            //令R' = (R+I)/2
            //┌                   ┐  ┌                         ┐
            //│v1^2   v2*v1  v3*v1│  │R'(1,1)  R'(1,2)  R'(1,3)│
            //│v1*v2  v2^2   v3*v2│= │R'(1,2)  R'(2,2)  R'(2,3)│
            //│v1*v3  v3*v3  v3^2 │  │R'(1,3)  R'(2,3)  R'(3,3)│
            //└                   ┘  └                         ┘
            //观察等式两边得到
            //v1^2 = R'(1,1)
            //v2^2 = R'(2,2)
            //v3^2 = R'(3,3)
            //但是好需要由对称矩阵R'中的元素R'(1,2), R'(2,3), R'(1,3)的符号共同决定
            //
            //符号函数定义为
            //         -1, when x<0
            //sgn(x) = 0,  when x=0
            //         1,  whe  x>0
            //
            //Hash function =  sgn(R'(1,2)) * 9 + sgn(R'(2,3))*3 + sgn(R'(1,3))
            //┌──────┬──────┬──────┬──┬────┬────┬────┐
            //│sgn(R'(1,2))│sgn(R'(2,3))│sgn(R'(1,3))│Hash│sgn(v1) │ sgn(v2)│ sgn(v3)│
            //├──────┼──────┼──────┼──┼────┼────┼────┤
            //│   -1       │   -1       │     1      │ -11│  1     │   -1   │  1     │
            //├──────┼──────┼──────┼──┼────┼────┼────┤ 
            //│   1        │   0        │     0      │  9 │  1     │   1    │  0     │
            //├──────┼──────┼──────┼──┼────┼────┼────┤
            //│  -1        │   0        │     0      │ -9 │  1     │  -1    │  0     │
            //├──────┼──────┼──────┼──┼────┼────┼────┤
            //│  0         │   1        │     0      │  3 │  0     │  1     │  1     │
            //├──────┼──────┼──────┼──┼────┼────┼────┤
            //│  0         │  -1        │     0      │ -3 │  0     │  1     │ -1     │
            //├──────┼──────┼──────┼──┼────┼────┼────┤
            //│  0         │   0        │     1      │ 1  │  1     │  0     │  1     │
            //├──────┼──────┼──────┼──┼────┼────┼────┤
            //│  0         │   0        │    -1      │-1  │  1     │  0     │ -1     │
            //├──────┼──────┼──────┼──┼────┼────┼────┤
            //│  1         │   1        │    1       │13  │  1     │  1     │ 1      │
            //├──────┼──────┼──────┼──┼────┼────┼────┤
            //│  1         │   -1       │   -1       │ 5  │  1     │  1     │ -1     │
            //├──────┼──────┼──────┼──┼────┼────┼────┤
            //│ -1         │   1        │   -1       │-7  │  1     │ -1     │ -1     │
            //├──────┼──────┼──────┼──┼────┼────┼────┤
            //│ 0          │   0        │   0        │ 0  │  1     │  1     │  1     │
            //├──────┼──────┼──────┼──┼────┼────┼────┤
            //
            //得到
            //v1 = sgn(v1)*sqrt((R(1,1) + 1.0)/2)
            //v2 = sgn(v2)*sqrt((R(2,2) + 1.0)/2)
            //v3 = sgn(v3)*sqrt((R(3,3) + 1.0)/2)
            static int map[11][4]  =
            {   
                //hash   sgn(v1)  sgn(v2) sgn(v3)
                {-11,    1,         -1,     1},
                {  9,    1,          1,     0},
                { -9,    1,         -1,     0},
                {  3,    0,          1,     1},
                { -3,    0,          1,    -1},
                {  1,    1,          0,     1},
                { -1,    1,          0,    -1},
                { 13,    1,          1,     1},
                {  5,    1,          1,    -1},
                { -7,    1,         -1,    -1},
                {  0,    1,          1,     1}
            };

            //v11
            omega.d[0] = theta * sqrt((R11 + 1.0)/2.0);
            omega.d[1] = theta * sqrt((R22 + 1.0)/2.0);
            omega.d[2] = theta * sqrt((R33 + 1.0)/2.0);

            int hash = SGN(R12)*9 + SGN(R23)*3 + SGN(R13);

            for(int i=0; i < 11; i++)
            {
                if(map[i][0] == hash)
                {
                    omega.d[0] *= double(map[i][1]);
                    omega.d[1] *= double(map[i][2]);
                    omega.d[2] *= double(map[i][3]);
                    break;
                }
            }            

            if(dOmega_dR != NULL)//θ→π，偏导数不存在, 雅克比行列式不存在
                throw std::exception("theta is close to Pi, Jacobian Matrix not defined");
            return;
        }
    }



    //3.正常情形
    omega.d[0] = theta * 0.5/sin_theta * (R32 - R23);
    omega.d[1] = theta * 0.5/sin_theta * (R13 - R31);
    omega.d[2] = theta * 0.5/sin_theta * (R21 - R12);

    if(NULL == dOmega_dR) return;//无须计算雅克比矩阵

    MatlabSimu::Matrix<double>  dTheta_dR(1,9);
    //θ=arccos[(trace(R)-1)/2]
    //tr =(trace(R)-1)/2
    //-->θ=arccos(tr)
    //-->dθ/dtr=-0.5/sqrt(1-tr^2);
    double dTheta_dtr = -0.5/sqrt(1 - tr*tr);
    //dTheta/dR = dTheta_dtr * (1, 0, 0, 0, 1, 0, 0, 0, 1)
    dTheta_dR(0, 0) = dTheta_dtr;
    dTheta_dR(0, 1) =  0;
    dTheta_dR(0, 2) =  0;
    dTheta_dR(0, 3) =  0;
    dTheta_dR(0, 4) =  dTheta_dtr;
    dTheta_dR(0, 5) =  0;
    dTheta_dR(0, 6) =  0;
    dTheta_dR(0, 7) =  0;
    dTheta_dR(0, 8) =  dTheta_dtr;


    //<<debug
    dTheta_dR.Print("dTheta_dR=\n");
    //debug>>

    double coef_2 = 1.0/(2.0*sin_theta);
    MatlabSimu::Matrix<double> v(3,1);
    v(0,0) = coef_2 * (R32 - R23);
    v(1,0) = coef_2 * (R13 - R31);
    v(2,0) = coef_2 * (R21 - R12);


    //令
    //     ┌               ┐
    //     │R(3,2) - R(2,3)│
    //om1 =│R(1,3) - R(3,1)│
    //     │R(2,1) - R(1,2)│
    //     └               ┘
    //则 v = 1/(2*sinθ)* om1
    //dv/dR = 1/(2*sinθ)*dom1/dR + v * cosθ/sinθ* dθ/dR
    //
    //
    MatlabSimu::Matrix<double>  dom1_dR(3,9);

    //           ┌                    ┐
    //           │0 0  0  0 0 1 0 -1 0│
    // dom1_dR = │0 0 -1  0 0 0 1  0 0│
    //           │0 1  0 -1 0 0 0  0 0│
    //           └                    ┘
     dom1_dR(0,0) = 0; dom1_dR(0,1) = 0; dom1_dR(0,2) = 0; dom1_dR(0,3) = 0; dom1_dR(0,4) = 0; dom1_dR(0,5) = 1; dom1_dR(0,6) = 0; dom1_dR(0,7) = -1; dom1_dR(0,8) = 0;
     dom1_dR(1,0) = 0; dom1_dR(1,1) = 0; dom1_dR(1,2) =-1; dom1_dR(1,3) = 0; dom1_dR(1,4) = 0; dom1_dR(1,5) = 0; dom1_dR(1,6) = 1; dom1_dR(1,7) =  0; dom1_dR(1,8) = 0;
     dom1_dR(2,0) = 0; dom1_dR(2,1) = 1; dom1_dR(2,2) = 0; dom1_dR(2,3) =-1; dom1_dR(2,4) = 0; dom1_dR(2,5) = 0; dom1_dR(2,6) = 0; dom1_dR(2,7) =  0; dom1_dR(2,8) = 0;


    MatlabSimu::Matrix<double>  dv_dR(3,9);

    dv_dR = dom1_dR * coef_2 - v * cos_theta/sin_theta * dTheta_dR;

    //<<debug
    dv_dR.Print("dv_dR=\n");
    //debug>>



    //ω=vθ
    //dOmega/dR = dv/dR *θ  + v * dθ/dR
    //这里
    //               ┌               ┐
    //               │R(3,2) - R(2,3)│
    //v = 1/2*sinθ *│R(1,3) - R(3,1)│
    //               │R(2,1) - R(1,2)│
    //               └               ┘
    //
    //dv/dR     = 1/(2*sinθ)*dom1/dR + v * cosθ/sinθ* dθ/dR
    //dTheta/dR = coef * (1, 0, 0, 0, 1, 0, 0, 0, 1)


    //求解雅克比行列式
    double* pJocobi =  dOmega_dR;

    for(int c=0; c < 9; c++)
    {
        for(int r=0; r < 3; r++)
        {
            *pJocobi = theta * dv_dR(r,c) + v(r,0) * dTheta_dR(0, c);

            pJocobi ++;
        }
    }

}





//@功能:根据旋转矩阵计算出旋转轴矢量， 旋转轴的模为旋转角度
//@参数:omega, 输入参数, 3D旋转矢量，矢量的模表示旋转角度大小
//      pRoationMatrix, 输出参数, 指向3*3旋转矩阵元素的首地址的指针, 旋转矩阵按列优先排列
//      dR_dOm, 输出参数, 雅克比矩阵,为9×3的矩阵
//            ┌                                             ┐
//            │dR11/dOmega(1) dR11/dOmega(2)  dR11/dOmega(3)│
//            │dR12/dOmega(1) dR12/dOmega(2)  dR12/dOmega(3)│
//            │dR13/dOmega(1) dR13/dOmega(2)  dR13/dOmega(3)│
//            │dR21/dOmega(1) dR21/dOmega(2)  dR21/dOmega(3)│
//dR_dOm =    │dR22/dOmega(1) dR22/dOmega(2)  dR22/dOmega(3)│
//            │dR23/dOmega(1) dR23/dOmega(2)  dR23/dOmega(3)│
//            │dR31/dOmega(1) dR31/dOmega(2)  dR31/dOmega(3)│
//            │dR32/dOmega(1) dR32/dOmega(2)  dR32/dOmega(3)│
//            │dR33/dOmega(1) dR33/dOmega(2)  dR33/dOmega(3)│
//            └                                             ┘
void Rodrigues(const ROTATION_VECTOR& omega,  double* pR, double* dR_dOm)
{
    //设ω为3维旋转矢量
    //[ω]x为反对称矩阵
    ///       ┌                 ┐
    //        │0     -ω3   ω2 │
    //[ω]x = │ω3   0      -ω1│
    //        │-ω2  ω1    0   │
    //        └                 ┘
    //定义
    //e^[ω]x = I + [ω]x + [ω]x^2/2! + .. + [ω]x^n/n! +  ..
    //e^[ω]x为绕ω轴转动|ω|的旋转矩阵
    //注意到
    //[ω]x^3 = -|ω|^2*[ω]x，
    //[ω]x^4 = -|ω|^2*[ω]x^2
    //[ω]x^5 = -|ω|^2*[ω]x^3 = -|ω|^2 * (-|ω|^2*[ω]x) = |ω|^4*[ω]x
    //
    //代入级数表达式中得到
    //e^[ω]x = I + [ω]x + [ω]x^2/2! - |ω|^2*[ω]x/3! + - |ω|^2*[ω]x^2/4! + |ω|^4*[ω]x/5! + |ω|^4*[ω]x^2/6! + .. + [ω]x^n/n! + ..
    //        = I + (1 - |ω|^2/3! + |ω|^4/5! +..)*[ω]x + (1/2! - |ω|^2/4! + |ω|^4/6! +..)*[ω]x^2
    //        = I + (|ω| - |ω|^3/3! + |ω|^5/5! +..)*[ω']x + (|ω|^2/2! - |ω|^4/4! + |ω|^6/6! +..)*[ω']x^2
    //        这里ω' = ω/|ω|
    //用sin(|ω|)和cos(|ω|)将上式中的级数展开式替换掉，得到
    //e^[ω]x = I + sin(|ω|)*[ω']x + cos([ω]x)*[ω']x^2
    //        = I + sin(|ω|)/|ω|*[ω]x + + cos([ω]x)/|ω|^2*[ω]x^2
    //
    //注意到[ω]x^2=ω*ω^T - |ω|^2*I
    //带入上式得到
    //e^[ω]x = cos(|ω|)*I + sin(|ω|)/|ω|*|ω|x + (1-cos|ω|)/|ω|^2*[ω]x*[ω]x^T
    //注意！
    //特殊情形
    //|ω|=0时，不旋转,因此旋转矩阵就是单位矩阵I
    //|ω|=0时,雅克比矩阵的推导参考《Rodrigues公式推导.docx》
    //
    //|ω|=0时的
    //  ┌       ┐
    //  │0  0  0│
    //  │0  0  1│
    //  │0 -1  0│
    //  │0  0 -1│
    //J=│0  0  0│
    //  │1  0  0│
    //  │0  1  0│
    //  │-1 0  0│
    //  │0  0  0│
    //  └       ┘

    //旋转角度
    double theta = Norm((double*)&omega, 3);

    if(theta < DBL_EPSILON)
    {
      //旋转角度-->0
     //
     //    ┌      ┐
     //    │1 0 0 │
     //R = │0 1 0 │
     //    │0 0 1 │
     //    └      ┘
     //
     pR[0] = 1.0;
     pR[1] = 0.0;
     pR[2] = 0.0;
     pR[3] = 0.0;
     pR[4] = 1.0;
     pR[5] = 0.0;
     pR[6] = 0.0;
     pR[7] = 0.0;
     pR[8] = 1.0;

    //  ┌        ┐
    //  │ 0  0  0│
    //  │ 0  0  1│
    //  │ 0 -1  0│
    //  │ 0  0 -1│
    //J=│ 0  0  0│
    //  │ 1  0  0│
    //  │ 0  1  0│
    //  │-1  0  0│
    //  │ 0  0  0│
    //  └        ┘
    dR_dOm[0] =  0.0; dR_dOm[9 ] =  0.0; dR_dOm[18] =  0.0;
    dR_dOm[1] =  0.0; dR_dOm[10] =  0.0; dR_dOm[19] =  1.0;
    dR_dOm[2] =  0.0; dR_dOm[11] = -1.0; dR_dOm[20] =  0.0;
    dR_dOm[3] =  0.0; dR_dOm[12] =  0.0; dR_dOm[21] = -1.0;
    dR_dOm[4] =  0.0; dR_dOm[13] =  0.0; dR_dOm[22] =  0.0;
    dR_dOm[5] =  1.0; dR_dOm[14] =  0.0; dR_dOm[23] =  0.0;
    dR_dOm[6] =  0.0; dR_dOm[15] =  1.0; dR_dOm[24] =  0.0;
    dR_dOm[7] = -1.0; dR_dOm[16] =  0.0; dR_dOm[25] =  0.0;
    dR_dOm[8] =  8.0; dR_dOm[17] =  0.0; dR_dOm[26] =  0.0;

     return;
    }


    //Let k=ω/|ω|=ω/θ
    //Let V1=<θ, k1, k2, k3>
    double k1 = omega.d[0]/theta;
    double k2 = omega.d[1]/theta;
    double k3 = omega.d[2]/theta;

    double sin_theta  = sin(theta);
    double cos_theta  = cos(theta);
    double _1_minus_cos_theta = 1 - cos_theta;

    //R =  cos(θ)*I + sin(θ)/|k|x + (1-cosθ)/[k]x*[k]x^T
    MatlabSimu::Matrix<double>  dR_dV1;
    dR_dV1.SetDimension(9, 4);

    //dR11/d(θ,k1,k2,k3)
    //dR11/dθ
    dR_dV1(0, 0) = -sin_theta + k1*k1*sin_theta;

    //dR11/dk1
    dR_dV1(0, 1) = 2*k1*_1_minus_cos_theta;

    //dR11/dk2
    dR_dV1(0, 2) = 0;

    //dR11/dk3
    dR_dV1(0, 3) = 0;


    //dR21/d(θ,k1,k2,k3)
    //dR21/dθ
    dR_dV1(1, 0) = k3*cos_theta + k1*k2*sin_theta;

    //dR21/dk1
    dR_dV1(1, 1) = _1_minus_cos_theta * k2;

    //dR21/dk2
    dR_dV1(1, 2) = _1_minus_cos_theta * k1;

    //dR21/dk3
    dR_dV1(1, 3) = sin_theta;


    //dR31/d(θ,k1,k2,k3)
    //dR31/dθ
    dR_dV1(2, 0) = -k2*cos_theta + k1*k3*sin_theta;

    //dR31/dk1
    dR_dV1(2, 1) = _1_minus_cos_theta * k3;

    //dR31/dk2
    dR_dV1(2, 2) = -sin_theta;

    //dR31/dk3
    dR_dV1(2, 3) = _1_minus_cos_theta * k1;



    //dR12/d(θ,k1,k2,k3)
    //dR12/dθ
    dR_dV1(3, 0) = -k3*cos_theta + k1*k2*sin_theta;

    //dR12/dk1
    dR_dV1(3, 1) = _1_minus_cos_theta * k2;

    //dR12/dk2
    dR_dV1(3, 2) =_1_minus_cos_theta * k1;

    //dR12/dk3
    dR_dV1(3, 3) = -sin_theta;


    //dR22/d(θ,k1,k2,k3)
    //dR22/dθ
    dR_dV1(4, 0) = -sin_theta + k2*k2*sin_theta;

    //dR22/dk1
    dR_dV1(4, 1) = 0;

    //dR22/dk2
    dR_dV1(4, 2) = 2 * _1_minus_cos_theta * k2;

    //dR22/dk3
    dR_dV1(4, 3) = 0;


    //dR32/d(θ,k1,k2,k3)
    //dR32/dθ
    dR_dV1(5, 0) = k1 * cos_theta + k2 * k3 * sin_theta;

    //dR32/dk1
    dR_dV1(5, 1) = sin_theta;

    //dR32/dk2
    dR_dV1(5, 2) = _1_minus_cos_theta * k3;

    //dR32/dk3
    dR_dV1(5, 3) = _1_minus_cos_theta * k2;


    //dR13/d(θ,k1,k2,k3)
    //dR13/dθ
    dR_dV1(6, 0) = k2 * cos_theta + k1 * k3 * sin_theta;

    //dR13/dk1
    dR_dV1(6, 1) = _1_minus_cos_theta * k3;

    //dR13/dk2
    dR_dV1(6, 2) = sin_theta;

    //dR13/dk3
    dR_dV1(6, 3) = _1_minus_cos_theta * k1;


    //dR23/d(θ,k1,k2,k3)
    //dR23/dθ
    dR_dV1(7, 0) = -k1 *cos_theta + k2 * k3 * sin_theta;

    //dR23/dk1
    dR_dV1(7, 1) = -sin_theta ;

    //dR23/dk2
    dR_dV1(7, 2) = _1_minus_cos_theta * k3;

    //dR23/dk3
    dR_dV1(7, 3) = _1_minus_cos_theta * k2;


    //dR33/d(θ,k1,k2,k3)
    //dR33/dθ
    dR_dV1(8, 0) = -sin_theta + k3 * k3 * sin_theta;

    //dR33/dk1
    dR_dV1(8, 1) = 0;

    //dR33/dk2
    dR_dV1(8, 2) = 0;

    //dR33/dk3
    dR_dV1(8, 3) = 2 * k3 * _1_minus_cos_theta;


    //dR_dV1.Print("dR_dV1=\n");


    MatlabSimu::Matrix<double>  dV1_dOmega;
    dV1_dOmega.SetDimension(4, 3);
    double norm2 = omega.d[0] * omega.d[0] + omega.d[1] * omega.d[1] + omega.d[2] * omega.d[2];

    //Let V1=<θ, k1, k2, k3>
    //calc dV1/d(dω1,dω2, dω3)

    //dθ/(dω1,dω2, dω3)
    dV1_dOmega(0, 0 ) = omega.d[0]/theta;
    dV1_dOmega(0, 1 ) = omega.d[1]/theta;
    dV1_dOmega(0, 2 ) = omega.d[2]/theta;

    //dk1/d(dω1,dω2, dω3)
    dV1_dOmega(1, 0 ) = (omega.d[1] * omega.d[1] + omega.d[2] * omega.d[2]) /(norm2*theta);
    dV1_dOmega(1, 1 ) = -omega.d[0]* omega.d[1]  /(norm2*theta);
    dV1_dOmega(1, 2 ) = -omega.d[0]* omega.d[2]  /(norm2*theta);


    //dk2/d(dω1,dω2, dω3)
    dV1_dOmega(2, 0 ) = -omega.d[0]* omega.d[1]  /(norm2*theta);
    dV1_dOmega(2, 1 ) = (omega.d[0]* omega.d[0]+ omega.d[2] * omega.d[2]) /(norm2*theta);
    dV1_dOmega(2, 2 ) = -omega.d[1] * omega.d[2]  /(norm2*theta);


    //dk3/d(dω1,dω2, dω3)
    dV1_dOmega(3, 0 ) = -omega.d[0]* omega.d[2] /(norm2*theta);
    dV1_dOmega(3, 1 ) = -omega.d[1] * omega.d[2] /(norm2*theta);
    dV1_dOmega(3, 2 ) = (omega.d[0]* omega.d[0]+ omega.d[1] * omega.d[1]) /(norm2*theta);

    //dV1_dOmega.Print("dV1_dOmega=\n");


    //计算雅克比矩阵
     if(dR_dOm)
     {
         double *pJocobi = dR_dOm;


         for( int c = 0; c < 3; c ++)
         {
             for( int r = 0; r < 9; r++)
             {
                 *pJocobi = 0.0;

                 for(int i=0; i < 4; i++)
                 {
                     *pJocobi  += dR_dV1(r, i) * dV1_dOmega(i,c) ;
                 }

                 pJocobi ++;
             }//for-each(r)
         }//for-each(c)
     }//if

    if(pR)
    {
        //R11
        pR[0] = cos_theta + _1_minus_cos_theta * k1 * k1;

       //R21
        pR[1] = k3 * sin_theta + _1_minus_cos_theta * k2 * k1;

        //R31
        pR[2] = -k2 * sin_theta + _1_minus_cos_theta * k3 * k1;

        //R12
        pR[3] = -k3 * sin_theta + _1_minus_cos_theta * k1 *k2;

        //R22
        pR[4] = cos_theta + _1_minus_cos_theta * k2 * k2;

        //R32
        pR[5] = k1 * sin_theta + _1_minus_cos_theta * k3 * k2;

        //R13
        pR[6] = k2 * sin_theta + _1_minus_cos_theta * k1 * k3;

        //R23
        pR[7] = -k1 *sin_theta + _1_minus_cos_theta * k2 * k3;

        //R33
        pR[8] = cos_theta + _1_minus_cos_theta * k3 * k3;

    }
}

//@功能:测试函数
void Rodrigues_Test()
{
    ROTATION_VECTOR omega;
    omega.d[0]= 1.0;
    omega.d[1] = 2.0;
    omega.d[2] = 3.0;


    //omega = (1,2,3)时的
    //Matlab运行结果
    //R:
   //-6.949205576413117e-001    7.135209905277877e-001    8.929285886191218e-002
   //-1.920069727919994e-001   -3.037850443394704e-001    9.331923538236470e-001
   // 6.929781677417702e-001    6.313496993837179e-001    3.481074778302649e-001

   //dR_dOm:
   // 3.822727406530116e-001    2.430314635702349e-001    3.645471953553524e-001
   // 5.743630338635131e-002   -2.762629065291387e-001   -7.608834436645932e-001
   // 3.992584034051992e-001    1.671671074266806e-001    1.547471747440212e-001
   // 3.464553326100489e-001    3.017751519182565e-001    4.080162984464286e-001
   // 3.329894548057631e-002    5.881119086969410e-001    9.989683644172892e-002
   //-3.755247380329964e-001   -5.807130832422258e-002   -4.130532235712014e-001
   // 2.065790505894009e-001   -5.200342526448452e-001   -4.232908837033739e-001
   // 2.265759281483185e-002    1.346080444915758e-001   -1.240341943475039e-001
   //-1.137290316936589e-001   -2.274580633873178e-001    4.410839315227056e-001

     MatlabSimu::Matrix<double> R(3,3);
     MatlabSimu::Matrix<double> dR_dOm(9,3);

     Rodrigues(omega, R.GetDataPtr(), dR_dOm.GetDataPtr());

     R.Print("R=\n");
     dR_dOm.Print("partial(R)/partial(Omega)=\n");

     //om = (-0.4326, -1.6656, 0.1253);
     //对应的R
     //   ┌                           ┐
     //   │-0.0815    0.2075   -0.9748│
     //R =│0.3511    0.9214    0.1668 │
     //   │0.9328   -0.3286   -0.1479 │
     //   └                           ┘
     //
     const char* szMatrix ="\
   -8.145350692605430e-002    2.075068839526711e-001   -9.748365090217435e-001;\
    3.510536263412363e-001    9.213797786437021e-001    1.667952485508803e-001;\
    9.328058091811580e-001   -3.286338336488423e-001   -1.478956582836365e-001";

     R.LoadFromString(szMatrix);
     R.Print("R=\n");

    MatlabSimu::Matrix<double> dOm_dR(3,9);
    Rodrigues(R.GetDataPtr(), omega, dOm_dR.GetDataPtr());

    dOm_dR.Print("dOm_dR=\n");


    std::cout << "omega=<" << omega.d[0]<< ","<<omega.d[1] << "," << omega.d[2] << ">" <<std::endl;

    //特殊情形测试
    //|ω| = π
    //Test when the norm of omega is π
    double norm_omega = Norm((double*)&omega, 3);
    omega.d[0] /= norm_omega;
    omega.d[1]  /= norm_omega;
    omega.d[2]  /= norm_omega;

    omega.d[0]*= M_PI;
    omega.d[1] *= M_PI;
    omega.d[2] *= M_PI;

    std::cout << "norm pi omega=<" << omega.d[0]<< ","<<omega.d[1] << "," << omega.d[2] << ">" <<std::endl;

    Rodrigues(omega, R.GetDataPtr(), dR_dOm.GetDataPtr());

    R.Print("R=\n");
    dR_dOm.Print("dR_dOm=\n");

    Rodrigues(R.GetDataPtr(), omega, NULL);
    std::cout << "omega from R=<" << omega.d[0]<< ","<<omega.d[1] << "," << omega.d[2] << ">" <<std::endl;

}