#pragma once


//圆柱形灯泡相机成像模型
class CGenericCameraModel;
class CylinderBulbModel
{
public:
    CylinderBulbModel();

    //Copy Constructor
    //必须要定义拷贝构造函数, 因为类成员变量有很多是引用。
     CylinderBulbModel(const CylinderBulbModel& clone);

    //@功能:对于相机中每个像素点对应的世界坐标位置Pi处,直与z=0平面放置一个半径为R,长度为L的
    //      圆柱形灯泡, 利用相机成像，计算灯泡在相片中的重心坐标Gi, Δ=Pi-Gi的值用来修正计算光笔触点的相机坐标。
    //@参数:pCameraModel, 相机模型指针
    //      pMaskImage, 屏蔽图指针
    //      nImageWidth, 相机图片宽度
    //      nImageHeight, 相机图片高度
    //void CalcProjectData(const CGenericCameraModel* pCameraModel, const BYTE* pMaskImage, int nImageWidth, int nImageHeight);
     void CalcProjectData(const CGenericCameraModel* pCameraModel, const RECT& rcMonitor, int nImageWidth, int nImageHeight);


    //@功能:根据激光笔光斑的重心和轮廓，求触点与重心的偏移量。
    //@参数:pCentroid,光斑重心的坐标。
    //      pContourPts,光斑轮廓点数组。
    //      nContourPointsCount, 轮廓点个数。
    //TVector2D  GetOffset(const TPoint2D& ptCentroid, const TPoint2D* pContourPts, int nContourPointsCount, TPoint2D* pContactPoint);
    TVector2D  GetOffset(const TPoint2D& ptCentroid);


	TVector2D  GetRealOffset(const TPoint2D& ptCentroid);
protected:
    //@功能:采样圆柱表面上的点
    //@参数:ptPlacePosition，在世界坐标系Z=0平面上的摆放位置
    //      nRadius, 圆柱体半径
    //      nLenght, 圆柱体长度
    //void SampleCylinderSurface(const TPoint3D& ptPlacePosition, double Radius, double Length);

    const CGenericCameraModel* m_pGenericCameraModel  ;//通用相机模型
    struct TContactPointOffsetInformation
    {
        TPoint2D  vOffset;
        TPoint2D  pt2dCentroidPos;
        
        //int       nLightSpotMass;//模型模拟生成的光斑质量
        //RECT      rcBound;
        BOOL      bValid;
    };
    std::vector<TContactPointOffsetInformation>    m_vecContatcPointOffset;//触点相对于重心的偏移矢量数组。
    //std::vector<TPoint3D>      m_vecSurfacePoints     ;//圆柱体表面的点的坐标。
    int                        m_nImageWidth;
    int                        m_nImageHeight;
};