#pragma once
#include "../inc/CameraModel.h"
#include <Matrix.h>
//template<class T, int M = 3, int N = 3>
//class CHomography
//{
//public:
//
//    CHomography()
//    {
//        for(int i = 0; i < M; i++)
//        {
//            for(int j = 0; j < N; j++)
//            {
//                m_data[i][j] = 0.0;
//            }
//        }
//    }
//protected:
//    T m_data[M][N];
//};
//typedef CHomography<double> HOMOGRAPHY3D;
typedef  MatlabSimu::Matrix<double> HOMOGRAPHY3D;
typedef  TPoint3D ROTATION_VECTOR;


inline double Norm(const double* pVector, int nDimension)
{

    double sum = 0.0;
    for(int i=0; i < nDimension; i++)
    {
        sum += pVector[i]*pVector[i];
    }   
    return sqrt(sum);
}

//@功  能:根据校正点的世界坐标和图像坐标的映射关系, 来计算单应矩阵。
//@参  数:pPtsInWorld, 在世界坐标系中校正平面中的校正点坐标
//        pPt2DInImg, 在图像中对应的每个校正点的坐标 
//        nPtNumber  , 校正点个数
//@返回值:返回3*3的Homography矩阵对象
HOMOGRAPHY3D ComputeHomography(const TPoint2D* pPtsInWorld, const TPoint2D* pPt2DInImg, int nPtNumber);


//@功 能:根据校正点坐标和相机的外部参数和内部参数,以及畸变参数计算出图像坐标
//@参 数:pPtsInWorld，校正平面的校正点的世界坐标
//       nPtNumber, 校正平面的校正点个数
//       intrinsic, 相机内部参数
//       extrinsic, 相机外部参数
//       pPtsImage, 输出参数, 图像坐标
//      dXp_dOmega, 输出参数, 图像坐标Xp对旋转轴omega的偏导数组成的雅可比矩阵
//      dXp_dT,     输出参数, 图像坐标Xp对偏移量T的偏导数组成的雅可比矩阵
//      dXp_df,     输出参数, 图像坐标Xp对焦距的偏导数组成的雅可比矩阵 
//      dXp_dc,     输出参数, 图像坐标Xp对相机光心的偏导数组成的雅可比矩阵   
//      dXp_dk,     输出参数, 图像坐标Xp对畸变系数的偏导数组成的雅可比矩阵   
//      dXp_alpha,  输出参数，图像坐标Xp对斜度系数的偏导数组成的雅克比矩阵
void ProjectPoints(
                   const TPoint3D* pPt3DInWorld,  
                   int nPtNumber, 
                   const IntrinsicParameters& intrinsic,
                   const ExtrinsicParameters& extrinsic,
                   const DistortionParameters& distortion,
                   TPoint2D* pPtsImage,
                   MatlabSimu::Matrix<double>& dXp_dOmega,
                   MatlabSimu::Matrix<double>& dXp_dT,
                   MatlabSimu::Matrix<double>& dXp_df,
                   MatlabSimu::Matrix<double>& dXp_dc,
                   MatlabSimu::Matrix<double>& dXp_dk,
                   MatlabSimu::Matrix<double>& dXp_dalpha);






//@功 能:相机校正计算相机内部参数的初始值
//@参 数:pPtsInWorld, 校正平面上校正点的世界坐标
//       pPt2DInImg, 每个校正点对应的图像坐标
//       nPtNumber  , 校正点个数
//       nImageWidth, 图像宽度
//       nImageHeight,图像高度
//       intrinsic,  输出参数, 相机的内部参数
//       distortion, 输出参数, 相机镜头的畸变参数
void InitIntrinsicParam(
                        const TPoint2D* pPtsInWorld, 
                        const TPoint2D* pPt2DInImg, 
                        int nPtNumber, 
                        int nImageWidth, 
                        int nImageHeight, 
                        IntrinsicParameters& intrinsic, 
                        DistortionParameters& distortion);


//@功 能:计算相机的外部参数(旋转矢量和平移矢量
//@参 数:pPtsInWorld, 校正平面上校正点的世界坐标
//       pPt2DInImg, 每个校正点对应的图像坐标
//       nPtNumber  , 校正点个数
//       intrinsic  , 相机的内部参数
//      distortion  , 相机的畸变参数
//          omega   , 输出参数, 相机外部参数的旋转矢量，矢量的大小表示旋转角度。
//              t   , 输出参数, 相机外部参数的平移矢量
//             R    , 输出参数, 与旋转矢量对应的相机的外部旋转矩阵,
void ComputeExtrinsincInit(
                           const TPoint3D* pPtsInWorld, 
                           const TPoint2D* pPt2DInImg, 
                           int nPtNumber,
                           const IntrinsicParameters& intrinsic,
                           const DistortionParameters& distortion,
                           ExtrinsicParameters& extrinsic);

 

//@功能:采用迭代非线性迭代, 精细求解3D旋转矢量, 3D偏移矢量，并返回旋转矩阵
//@参 数:pPtsInWorld, 校正平面上校正点的世界坐标
//       pPt2DInImg, 每个校正点对应的图像坐标
//       nPtNumber  , 校正点个数
//       intrinsic  , 相机的内部参数
//      distortion  , 相机的畸变参数
//          omega   , 输出参数, 相机外部参数的旋转矢量，矢量的大小表示旋转角度。
//              t   , 输出参数, 相机外部参数的平移矢量
//             R    , 输出参数, 与旋转矢量对应的相机的外部旋转矩阵,
//          nMaxIter, 最大迭代次数
void ComputeExtrinsicRefine(
                           const TPoint3D* pPt3DInWorld, 
                           const TPoint2D* pPt2DInImg, 
                           int nPtNumber,
                           const IntrinsicParameters& intrinsic,
                           const DistortionParameters& distortion,
                           ExtrinsicParameters& extrinsic,
                           int nMaxIter=20);


//@功 能:归一化图像坐标
//@参 数:pPt2DInImg, 校正点的图像坐标
//       nPtNumbe, 校正点个数
//       intrinsic, 相机的内部参数
//       distortion, 相机的外部参数
//       pPtsNormalized,输出参数，归一化后的图像坐标
void NormalizePixel(
                    const TPoint2D* pPt2DInImg, 
                    int nPtNumber,
                    const IntrinsicParameters& intrinsic,
                    const DistortionParameters& distortion,
                    TPoint2D* pPtsNormalized
                    );

//compute_extrinsic_refine








//@功能:测试函数ProjectPoints
void Test_ProjectPoints();





                   

                    



