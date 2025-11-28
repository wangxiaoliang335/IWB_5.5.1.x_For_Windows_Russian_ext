#pragma once
#include "Routines.h"


//@功能:根据旋转矩阵计算出旋转轴矢量， 旋转轴的模为旋转角度
//@参数:pRoationMatrix, 输入参数, 指向3*3旋转矩阵的首地址的指针, 旋转矩阵的元素按列依次排序
//      pOmega, 输出参数, 根据旋转矩阵计算得出的3D旋转矢量
//      dOmdR, 输出参数, 雅克比矩阵,为3×9的矩阵
//              ┌                                                  ┐   
//              │dOmega(1)/dR11, dOmega(1)/dR21, ... , Omega(1)/dR33│
//      dOmdR = │dOmega(2)/dR11, dOmega(2)/dR21, ... , Omega(2)/dR33│
//              │dOmega(3)/dR11, dOmega(3)/dR21, ... , Omega(3)/dR33│
//              └                                                  ┘ 
void Rodrigues(const double* pR, ROTATION_VECTOR& omega, double* dOmdR);



//@功能:根据旋转矩阵计算出旋转轴矢量， 旋转轴的模为旋转角度
//@参数:omega, 输入参数, 3D旋转矢量，矢量的模表示旋转角度大小
//      pRoationMatrix, 输出参数, 指向3*3旋转矩阵元素的首地址的指针, 旋转矩阵按列优先排列
//      dRdOm, 输出参数, 雅克比矩阵,为9×3的矩阵
//            ┌                                             ┐ 
//            │dR11/dOmega(1) dR11/dOmega(2)  dR11/dOmega(3)│
//            │dR12/dOmega(1) dR12/dOmega(2)  dR12/dOmega(3)│ 
//            │dR13/dOmega(1) dR13/dOmega(2)  dR13/dOmega(3)│
//            │dR21/dOmega(1) dR21/dOmega(2)  dR21/dOmega(3)│
//dRdOm =     │dR22/dOmega(1) dR22/dOmega(2)  dR22/dOmega(3)│
//            │dR23/dOmega(1) dR23/dOmega(2)  dR23/dOmega(3)│
//            │dR31/dOmega(1) dR31/dOmega(2)  dR31/dOmega(3)│
//            │dR32/dOmega(1) dR32/dOmega(2)  dR32/dOmega(3)│
//            │dR33/dOmega(1) dR33/dOmega(2)  dR33/dOmega(3)│
//            └                                             ┘ 
void Rodrigues(const ROTATION_VECTOR& omega,  double* pR, double* dRdOm);




//@功能:测试函数
void Rodrigues_Test();