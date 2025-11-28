#pragma once

#ifdef CAMERAMODEL_EXPORTS
#define CAMERA_MODEL_API _declspec(dllexport)
#else
#define CAMERA_MODEL_API _declspec(dllimport)
#endif


#include <assert.h>
#include "MultiDimensionPoint.h"
#include "CameraSpecs.h"

#pragma warning(disable:4251)
//通用相机模型
enum CAMERA_MODEL_API EGenericCameraProjectionType
{
    E_CAMERA_PROJECTION_PERSPECTIVE    ,//透射投影类型, r = tan(θ)
    E_CAMERA_PROJECTION_STEREOGRAPHIC  ,//立体投影      r = 2tan(θ/2)
    E_CAMERA_PROJECTION_EQUIDISTANCE   ,//等距投影      r = θ;
    E_CAMERA_PROJECTION_EQUISOLD_ANGLE ,//              r = 2sin(θ/2)
    E_CAMERA_PROJECTION_ORTHOGONAL      //正交投影      r = sin(θ)
};

class CGenericCameraModel;
void SaveCameraModelParams(const CGenericCameraModel& model, const char* lpszFilename = NULL);
//通用相机模型
class CAMERA_MODEL_API CGenericCameraModel
{
public:
    CGenericCameraModel();
    //Copy Constructor
    //必须要定义拷贝构造函数, 因为类成员变量有很多是引用。
     CGenericCameraModel(const CGenericCameraModel& clone);


     ~CGenericCameraModel();

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
    //@返回值:成功, TRUE
    //        失败, FALSE
	 BOOL Calibrate(
		 EGenericCameraProjectionType eProjectType,
		 double dbThrowRatio,
		 const TPoint2D* pPtsInWorld,
		 const TPoint2D* pPt2DInImg,
		 int nPtNumber,		 
		 const RECT& rcMonitor,
		int radius = 0,
		int nCxImg=640, 
		int nCyImg=480,
		const TInternalAndSymmetricDistortParams* pInternalParams = NULL);


    //@功  能:从图像坐标影映射为屏幕坐标
    //@参  数:ptImage, 输入参数, 相机画面中的坐标数组
    //        nPtNumber, 数组个坐标个数
    //        ptWorld, 输出参数，保存从相机平面坐标转化成校正平面坐标的数组。
    //@返回值:成功, TRUE
    //        失败, FALSE
    BOOL FromImage2World(const TPoint2D* ptImage, int nPtNumber, TPoint2D* ptWorld ) const;


    /*
    //@功  能:从图像坐标影映射为屏幕坐标
    //@参  数:ptImage, 输入参数, 相机画面中的坐标数组.
    //        prcBound,输入参数, 指向光斑外界矩形数组的指针.
    //        nPtNumber, 数组个坐标个数.
    //        ptWorld, 输出参数，保存从相机平面坐标转化成校正平面坐标的数组。
    //@返回值:成功, TRUE
    //        失败, FALSE
    //BOOL FromImage2World_Aug(const TPoint2D* ptImage, const RECT* prcBound, int nPtNumber, TPoint2D* ptWorld);
    BOOL FromImage2World_Aug(const POINT* ptImage, const RECT* prcBound, int nPtNumber, POINT* ptWorld) const ;
    */




    /*
    //@功能:根据摄像头的视频坐标计算得出屏幕坐标
    //@参数:pCentroid, 光斑的重心
    //      pContourPoints, 指向光斑轮廓点的数组
    //      nContourPointCount, 光斑轮廓点个数
    //      ptWorld, 指向保存屏幕坐标的内存地址的指针
    //      debugCrossPts, 
    BOOL FromImage2World_WithLighgSpotContour(const POINT& pCentroid,  const POINT* pContourPoints, int nContourPointCount, POINT* pPtWorld, POINT* debugCrossPts = NULL) const;
    */



    //@功能:映射3D世界坐标系中的点到相机图片上
    //@参数:ptWorld, 3D世界坐标数组
    //      nPtNumber, 3D世界坐标个数
    //      ptImage, 输出的2D图像坐标
    BOOL MapWorldToImage(const TPoint3D* ptWorld, int nPtNumber, TPoint2D* ptImage) const;


    //@功能:返回相机模型参数个数
    int GetParameterCount()const;

    //@功能:返回相机模型参数数组
    const double* GetParameters() const;

    //@功能:设置模型参数
    void SetParameters(const double* pParameters, int nParametersCount);


    CGenericCameraModel& operator=( const CGenericCameraModel& right);


    const TPoint3D& GetCameraWorldPosition()const;


    //@功能:返回相机的投射比
    double GetThrowRatio()const;

    //@功能:返回相机镜头中心在画面中的坐标
    TPoint2D GetLensCenterInImage()const; 

    //@功能:获取校正时的屏幕尺寸信息
    RECT GetMonitorRect()const;

    //@功能:获取视频图片的尺寸
    SIZE GetImageSize()const;


	//@功能:使能调试输出
	void EnableDebug(BOOL bEnable) { m_bDebug = bEnable; }

	friend void SaveCameraModelParams(const CGenericCameraModel& model, const char* lpszFilename);


	//@功能:计算光点与光轴的夹角θ和在图像平面的方位角φ
	BOOL GetThetaPhi(const TPoint2D* ptImage, int nPtNumber, double* pThetas, double* pPhis)const ;

	//
public:
    static const int INTERNAL_PARAMS_OFFSET                           = 0;
    static const int SYMMETRIC_DISTORTION_PARAMS_OFFFSET              = 4;
    static const int UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_G_OFFSET    = 9;
    static const int UNSYMMMETRIC_RADIAL_DISTROTION_PARAMS_I_OFFSET    = 12;
    static const int UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_H_OFFSET    = 16;
    static const int UNSYMMMETRIC_TAGENT_DISTROTION_PARAMS_J_OFFSET    = 19;
    static const int EXTERNAL_PARAMS_ROTATION_MAXTRIX_OFFSET          = 23;
    static const int EXTERNAL_PARAMS_TRANSLATION_OFFSET               = 32;
    static const int AUX_PARAM_THETA_MAX_OFFSET                       = 35;
    static const int THROW_RATIO_OFFSET                               = 36;
    static const int IMAGE_CX_OFFSET                                  = 37;
    static const int IMAGE_CY_OFFSET                                  = 38;
    static const int MONITOR_LEFT_OFFSET                              = 39;
    static const int MONITOR_RIGHT_OFFSET                             = 40;
    static const int MONITOR_TOP_OFFSET                               = 41;
    static const int MONITOR_BOTTOM_OFFSET                            = 42;
    static const int TOTAL_PARAMETERS                                 = 43;

    static const int BASIC_MODEL_INTERNAL_PARAMS_COUNT     = 6;
    static const int RADIAL_MODEL_INTERNAL_PARAMS_COUNT    = 9;
    static const int EXTENDED_MODEL_INTERNAL_PARAMS_COUNT  = 9 + 14;
                     


protected:

    //@功能:初始镜头内部参数
    BOOL InitInteralParameters(EGenericCameraProjectionType eProjectType, double dbThrowRatio, int ImageWidth, int nImageHeight);

    //@功能:计算从校正平面到单位球面的中心投影的单应矩阵H
    //void CompHs(const TPoint2D* pPtsInWorld, const TPoint2D* pPt2DInImg, int nPtNumber);

    //@功能:从单应矩阵m_hs计算出相机的外部参数
    //BOOL InitExternalParameter();


    //@功能:通过最小化"投影误差"，来迭代优化相机的内部参数和外部参数
    //BOOL MinimizeProjErr(const TPoint2D* pPtsInWorld, const TPoint2D* pPt2DInImg, int nPtNumber, EModelType eType);


    //@功能:将校正点在相机图片中的位置，逆投影到单位球面上。
    //@参数:pPt2DInImg,
    void BackProjectToUnitSphere(const TPoint2D* pPt2DInImg, int nPtNumber, double* pTheta, double* pPhi);


    //@功能:根据校正得到的内部参数和外部参数，将坐标从相机平面投影到校正平面上。
    //@参数:pPt2DInImag, 输出参数, 输入的相机坐标
    //      nPtNumber, 点的个数
    //      pPt2DInWorld, 输出参数, 保存世界坐标的数组
    BOOL BackwardProject(const TPoint2D* pPt2DInImag, int nPtNumber, TPoint2D* pPt2DInWorld) const;


    //@功能:建立从R查找theta值(光线入射角)的查找表
    void ConstructRToThetaLUT(const double& cxImage, const double& nCyImage);

    //@功能:从查找表中查找r值对应的θ值。
    BOOL GetThetaFrmLUT(const double& r, const double& phi, double& theta)const;

    //@功能:相机参数数组。
    double m_Parameters[TOTAL_PARAMETERS];


    
    //    
    //@功能:中心映射Homography Matrix的旋转矩阵。
    double (&m_Rs)[3][3];

    //@功能:中心映射的水平平移量。
    double (&m_ts)[3];

    //内部参数变量, 引用类型指向m_Parameters中的元素
    double& m_mu ;
    double& m_mv ;
    double& m_u0 ;//镜头中心在画面中的水平坐标
    double& m_v0 ;//镜头中心在画面中的垂直坐标

    //径向对称畸变参数
    double& m_k1;
    double& m_k2;
    double& m_k3;
    double& m_k4;
    double& m_k5;
    double (&m_k)[5];


    //径向非对称畸变参数
    double& m_g1;
    double& m_g2;
    double& m_g3;
    double (&m_g)[3];

    double& m_i1;
    double& m_i2;
    double& m_i3;
    double& m_i4;
    double (&m_i)[4];


    //切向非对称畸变参数
    double& m_h1;
    double& m_h2;
    double& m_h3;
    double (&m_h)[3];
    
    double&  m_j1;
    double&  m_j2;
    double&  m_j3;
    double&  m_j4;
    double (&m_j)[4];

    double& m_thetaMax;//入射光线与镜头光轴的最大夹角。
    
    double& m_throwRatio;//投射比

    double& m_cxImage;//画面的宽度
    double& m_cyImage;//画面的高度

    double& m_Monitor_Left  ;//屏幕区域左边界
    double& m_Monitor_Right ;//屏幕区域上边界
    double& m_Monitor_Top   ;//屏幕区域上边界
    double& m_Monitor_Bottom;//屏幕区域下边界


    double   m_InvRs [3][3]    ;//m_Rs的逆矩阵
    TPoint3D m_ptCameraLocation;//相机在世界坐标系中的坐标
    TPoint2D m_ptOZFootImage   ;//相机中心在世界坐标系z=0平面上的垂足,在相机中所成的像


    //double m_radiusMax       ;//相片上的像点到光心的像(m_u0,m_v0)的最大归一化距离,
	int m_nPatternCircleRadius ;//校正圆的半径
    struct TRToThetaLUTEntry
    {
        double radius;
        double theta;
    };

	//φ —>table（R， θ）查找表（LUT）的映射表
	struct TRPhiToThetaTable
	{
		double phi;
		TRToThetaLUTEntry*  r2ThetaLUT;
		int nLUTSize;
	};


	static const int PHI_DIVISION = 60               ;//方位角分割成60份。
    TRPhiToThetaTable m_RPhi2ThetaTables[PHI_DIVISION];//2π分成PHI_DIVISION份作为φ值采样，每个φ值都关联一个(R,θ）查找表。

    
	BOOL m_bDebug;
};

#pragma warning(default:4251)