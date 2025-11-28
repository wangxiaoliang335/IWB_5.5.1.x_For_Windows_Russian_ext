#pragma once
#ifdef CAMERAMODEL_EXPORTS
#define PERSPECTIVE_MODEL_API _declspec(dllexport)
#else
#define PERSPECTIVE_MODEL_API _declspec(dllimport)
#endif

#include "../inc/MultiDimensionPoint.h"

class PERSPECTIVE_MODEL_API CPerspectiveCameraModel
{
public:
    CPerspectiveCameraModel();

    BOOL CalcParams(const TPoint2D* pPointsInImage, const TPoint2D* pPointsOnPlane, int nPtNumber, const SIZE& sImage);

    //@功能;根据视频坐标计算得出计算机屏幕坐标
    //@参数:ptImage, 光斑的视频图像坐标
    //      ptScreen, 保存计算机屏幕坐标的地址
    BOOL GetScreenPt(const TPoint2D* ptImage, TPoint2D* ptScreen, int nPtNumber);

    BOOL IsValidParameters();

    void SetParameters(const double* pParameters, int nParametersCount);

	//@功能:映射3D世界坐标系中的点到相机图片上
	//@参数:ptWorld, 3D世界坐标数组
	//      nPtNumber, 3D世界坐标个数
	//      ptImage, 输出的2D图像坐标
	BOOL MapWorldToImage(const TPoint3D* ptWorld, int nPtNumber, TPoint2D* ptImage) const; 



    int GetParameterCount()const;
    const double* GetParameters()const;
protected:
    double m_Parameters[11];//H[3][3],
    double(&m_H)[3][3];

    //相机画面宽度
    double& m_cxImage;

    //相机画面高度
    double& m_cyImage;

    //SIZE   m_szImage;
};