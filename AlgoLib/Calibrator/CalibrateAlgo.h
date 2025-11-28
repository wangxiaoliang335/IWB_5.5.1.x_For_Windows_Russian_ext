#pragma once

#include "../Math/LinearEquation.h"
#include "../Math/Matrix.h"
#include "../Math/Svd.h"

//#include <complex>
//#define lapack_complex_float std::complex<float>
//#define lapack_complex_double std::complex<double>

#define LAPACK_COMPLEX_STRUCTURE
#define LAPACK_GLOBAL_PATTERN_UC

#include "../LAPACK/inc/lapacke_config.h"
#include "../LAPACK/inc/lapacke.h"
#include "../inc/CameraModel.h"
#include "../inc/CameraSpecs.h"
#include "../inc/MultiDimensionPoint.h"
#include "../../CameraModel/PerspectiveCameraModel.h"
//#define USE_CYLINDER_BULB_MODEL

#ifdef USE_CYLINDER_BULB_MODEL
#include "CylinderBulbModel.h"
#endif


template<int N>
struct CoefCounter;

template<int N>
struct CoefCounter
{
    static const int count = N + 1 + typename CoefCounter < N-1> ::count;

};

//explicit specializaiton
template<> struct CoefCounter<0>
{
    static const int count = 1;
};


template<int N>
class OrderCombinator
{
public:

    OrderCombinator()
    {
        int index = 0;

        for(int q = N; q>=0; q--)
        {
            for(int p=q; p>=0; p--)
            {
                allOrderCombine[index].p = p;
                allOrderCombine[index].q = q-p;

                index++;
            }
        }

    }

    struct comb
    {
        int p;
        int q;
    };

    static const int COEF_NUMBER = typename CoefCounter<N>::count;

    //所有的幂次组合
    comb allOrderCombine[COEF_NUMBER];

};


template<class T>
inline T POW(T base, int exponent)
{
    T val = T(1);
    for(int i=0; i<exponent; i++)
    {
        val*=base;
    }
    return val;
}

//@说明:校正接口
class ICalibrate
{
public:
    ICalibrate(E_CALIBRATE_MODEL eCalibrateModel)
        :
        m_eCalibrateModel(eCalibrateModel)
    {
        
    }

    //@功能:计算校正参数
    //@输入:ALL_CALIB_DATA ,输入, 所有屏幕的校正数据
    //      screenSize,输入, 屏幕尺寸
    //      imageSize ,输入, 图象尺寸
    virtual BOOL CalcParams(const TCalibData& calibData, BOOL bDebug = FALSE) = 0;



    //@功能;根据视频坐标计算得出计算机屏幕坐标
    //@参数:ptImage, 光斑的视频图像坐标
    //      ptScreen, 保存计算机屏幕坐标的地址
    //      nMonitorId, 屏幕ID,从零开始
    //      bWithoutAutoCalibCompensate, 不做自动校正补偿标志
    virtual BOOL GetScreenPt(const TPoint2D& ptImage, TPoint2D* ptScreen, int nMonitorId=0, BOOL bWithoutAutoCalibCompensate=FALSE, TCameraDebugData* pDebugOutput = NULL) = 0;


    /*
    //@功能:根据摄像头的视频坐标计算得出屏幕坐标
    //@参数:pImgPts, 指向光斑的相机坐标点数组的指针
    //      prcBounds, 指向光斑的外界矩形数组的指针
    //      nPtNumber, 光斑个数
    //      pScreenPts, 指向保存屏幕坐标数组的指针
    //      nMonitorId, 屏幕Id,在一个镜头看多个屏幕时才有效。
    virtual BOOL GetScreenCoord_Aug(const POINT* pImgPts,  const RECT* prcBounds, int nPtNumber, POINT* pScreenPts, int nMonitorId = 0) = 0;
    */

	//@功能:将世界坐标系中的3维坐标映射维相机图片中的点
	virtual BOOL MapWorldToImage(const TPoint3D* ptWorld, int nPtNumber, TPoint2D* ptImage, int nMonitorId = 0) = 0;

    /*
    //@功能:根据摄像头的视频坐标计算得出屏幕坐标
    //@参数:pCentroid, 光斑的重心
    //      pContourPoints, 指向光斑轮廓点的数组
    //      nContourPointCount, 光斑轮廓点个数
    //      pScreenPts, 指向保存屏幕坐标的内存地址的指针
    //      nMonitorId, 屏幕Id,在一个镜头看多个屏幕时才有效。
    virtual BOOL GetScreenCoord_WithLightSpotContour(const POINT& pCentroid,  const POINT* pContourPoints, int nContourPointCount, POINT* pScreenPts, int nMonitorId = 0, POINT* debugContourCross = NULL) = 0;
    */

    //@功能:返回校正方程参数
    virtual const TCalibParams* GetCalibParams()const = 0;

    //@功能:返回校正方程参数
    virtual  void SetCalibParams(const TCalibParams& params) = 0;


    //@功能:设置镜头规格数据
    virtual void SetLensSpecification(const TLensSpecification& lensSpec) = 0;

    //@功能:设置CMOS芯片规格数据
    virtual void SetCMOSChipSpecification(const TCMOSChipSpecification& cmosChipSpec) = 0;

    //@功能:设置自动校正补偿数据
    virtual void SetAutoCalibCompCoefs(const TAutoCalibCompCoefs& autoCalibCompCoefs) = 0;

    //@功能:设置相机的已知的内部参数和对称畸变参数
    virtual void SetLensInternalAndSymmetricDistortParams(const TInternalAndSymmetricDistortParams* pLensInternalParams) = 0;

    E_CALIBRATE_MODEL GetCalibrateModel() const
    {
        return m_eCalibrateModel;
    }

protected:
    E_CALIBRATE_MODEL m_eCalibrateModel;

};



//@说明:通用相机校正接口
class Calibrator_GenericCameraModel:public ICalibrate
{
public:

    Calibrator_GenericCameraModel()
        :
    ICalibrate(E_CALIBRATE_MODEL_GENERICAL_CAMERA),
    m_bInternalAndSymmetricDistortParamsIsValid(FALSE)
    {

    }

    virtual ~Calibrator_GenericCameraModel()
    {

    }


    //@功能:计算校正参数
    //@输入:ALL_CALIB_DATA ,输入, 所有屏幕的校正数据
    //      screenSize,输入, 屏幕尺寸
    //      imageSize ,输入, 图象尺寸
    virtual BOOL CalcParams(const TCalibData& calibData, BOOL bDebug = FALSE)
    {
         int nMonitorCount = calibData.allMonitorCalibData.size();
         m_calibParams.allCalibCoefs.resize(nMonitorCount);

         if(m_vecCameraModels.size() != nMonitorCount)
         {
            m_vecCameraModels.resize(nMonitorCount);
         }

        for(int nMonitorId = 0; nMonitorId < nMonitorCount; nMonitorId++)
        {
            //屏幕区域
            const RECT& rcMonitor    = calibData.allMonitorCalibData[nMonitorId].rcMonitor;
            
            int nMonitorScreenWidth  = rcMonitor.right  - rcMonitor.left;
            int nMonitorScreenHeight = rcMonitor.bottom - rcMonitor.top ;

            m_calibParams.allCalibCoefs[nMonitorId].rcMonitor = rcMonitor;

           const std::vector<TCalibCoordPair>& data = calibData.allMonitorCalibData[0].calibData;

           int N = calibData.allMonitorCalibData[0].calibData.size();

           std::vector<TPoint2D> vecImagePts;
           std::vector<TPoint2D> vecScreenPts;

           vecImagePts.resize(N);
           vecScreenPts.resize(N);


            for(int i=0; i < N ; i++)
            {
                vecImagePts[i] = data[i].pt2DImageCoord;

                if (calibData.bUsingScreenPhysicalDimensions)
                {
                    vecScreenPts[i].d[0] = double(calibData.nScreenWidthInmm  * (data[i].ptScreenCoord.x - rcMonitor.left) / nMonitorScreenWidth);
                    vecScreenPts[i].d[1] = double(calibData.nScreenHeightInmm * (data[i].ptScreenCoord.y - rcMonitor.top ) / nMonitorScreenHeight);
                }
                else
                {
                    vecScreenPts[i].d[0] = double(data[i].ptScreenCoord.x);
                    vecScreenPts[i].d[1] = double(data[i].ptScreenCoord.y);
                }


            }
            

            try
            {
                //置位调试标志
                m_vecCameraModels[nMonitorId].EnableDebug(bDebug);

                //double dbThrowRatio = 0.15;//镜头投射比
                BOOL calibrateSuccess = 
                m_vecCameraModels[nMonitorId].Calibrate(
                    E_CAMERA_PROJECTION_PERSPECTIVE,
                    m_tLensSpec.throwRatio,//投射比
                    &vecScreenPts[0],//屏幕坐标数组
                    &vecImagePts[0],//图像坐标数组
                    N,
                    calibData.allMonitorCalibData[nMonitorId].rcMonitor,
                    calibData.allMonitorCalibData[nMonitorId].radius,
                    calibData.szImage.cx,
                    calibData.szImage.cy,
                    m_bInternalAndSymmetricDistortParamsIsValid ? &m_tInternalAndSymmetricDistortParams : NULL
                    );

                if (!calibrateSuccess)
                {
                    return FALSE;
                }

                //保存校正模型参数
                std::vector<double>& calibCoefs =  m_calibParams.allCalibCoefs[nMonitorId].calibCoefs;
                int nParamCount = m_vecCameraModels[nMonitorId].GetParameterCount();
                const double* pModelParams = m_vecCameraModels[nMonitorId].GetParameters();
                calibCoefs.resize(nParamCount);
                for(int j = 0; j < nParamCount; j++)
                {
                   calibCoefs[j] = pModelParams[j];
                }

                m_calibParams.eCalibrateModel = calibData.eCalibrateModel;

            }
            catch(...)
            {
                goto CalcParams_Exit;
            }

        }



        m_calibParams.szImage    = calibData.szImage;
        m_calibParams.eCalibType = calibData.eCalibType;//校正类别:手动，自动

        m_calibParams.bUsingScreenPhysicalDimensions = calibData.bUsingScreenPhysicalDimensions;
        m_calibParams.nScreenWidthInmm  = calibData.nScreenWidthInmm;
        m_calibParams.nScreenHeightInmm = calibData.nScreenHeightInmm;
        //<<test
#ifdef USE_CYLINDER_BULB_MODEL
        if(m_calibParams.eCalibType == E_CALIBRATE_TYPE_AUTO)
        {
            
            if(m_vecCylinderBulbModel.size() != nMonitorCount)
            {
                m_vecCylinderBulbModel.resize(nMonitorCount);
            }


            for(int i=0; i < nMonitorCount; i++)
            {
                m_vecCylinderBulbModel[i].CalcProjectData(
                    &m_vecCameraModels[i],
                    calibData.allMonitorCalibData[i].rcMonitor, //(const BYTE*)calibData.lpCtx,
                    calibData.szImage.cx ,
                    calibData.szImage.cy);
            }

        }//if
#endif
        //test>>
    CalcParams_Exit:


       return TRUE;

    }


    //@功能;根据视频坐标计算得出计算机屏幕坐标
    //@参数:ptVideo, 视频坐标
    //      nMonitorId, 屏幕ID,从零开始
    virtual BOOL GetScreenPt(const TPoint2D& ptCentroid, TPoint2D* pptScreen, int nMonitorId=0, BOOL bWithoutAutoCalibCompensate=FALSE, TCameraDebugData* pDebugOutput = NULL)
    {
        if(nMonitorId < 0 || size_t(nMonitorId) >= m_vecCameraModels.size()) return FALSE;

         TPoint2D pt2DContact;

        if(m_calibParams.eCalibType == E_CALIBRATE_TYPE_MANUAL || bWithoutAutoCalibCompensate)
        {//手动校正
           pt2DContact = ptCentroid;
        }
        else
        {//自动校正
            TVector2D offsetFromRefraction  = GetRefractionOffset(m_vecCameraModels[nMonitorId], ptCentroid);
            //TVector2D offsetFromRefraction = GetRefractionOffsetEx(m_vecCameraModels[nMonitorId], ptCentroid);


#ifdef USE_CYLINDER_BULB_MODEL
          TVector2D offsetByLedBubble;
          offsetByLedBubble.d[0] = 0.0;
          offsetByLedBubble.d[1] = 0.0;
          
          if(m_vecCylinderBulbModel.size() > nMonitorId)
          {
               offsetByLedBubble = m_vecCylinderBulbModel[nMonitorId].GetRealOffset(ptCentroid);
          }
          //test>>

          pt2DContact = ptCentroid - offsetFromRefraction  - offsetByLedBubble ;
#else
         
          pt2DContact = ptCentroid - offsetFromRefraction;
#endif

        }


        if (pDebugOutput)
        {
            pDebugOutput->pt2DContactInImage = pt2DContact;
            pDebugOutput->pt2DCentroid = ptCentroid;
        }
        BOOL bRet = m_vecCameraModels[nMonitorId].FromImage2World(&pt2DContact, 1, pptScreen);


        if (m_calibParams.bUsingScreenPhysicalDimensions && m_calibParams.nScreenWidthInmm &&  m_calibParams.nScreenHeightInmm)
        {//用屏幕物理尺寸做线性调整
            const RECT & rcMonitor = m_calibParams.allCalibCoefs[nMonitorId].rcMonitor;
            int nMonitorWidth = rcMonitor.right - rcMonitor.left;
            int nMonitorHeight = rcMonitor.bottom - rcMonitor.top;

            pptScreen->d[0] = (pptScreen->d[0] - 0) * nMonitorWidth / m_calibParams.nScreenWidthInmm   + rcMonitor.left;
            pptScreen->d[1] = (pptScreen->d[1] - 0) * nMonitorHeight / m_calibParams.nScreenHeightInmm + rcMonitor.top;
        }


        return bRet;
    }


    virtual BOOL MapWorldToImage(const TPoint3D* ptWorld, int nPtNumber, TPoint2D* ptImage, int nMonitorId)
    {
        if (nMonitorId < 0 || size_t(nMonitorId) >= m_vecCameraModels.size()) return FALSE;

        std::vector<TPoint3D> vecPtNormalizedWorld;
        vecPtNormalizedWorld.resize(nPtNumber);

        if(m_calibParams.bUsingScreenPhysicalDimensions && m_calibParams.nScreenWidthInmm &&  m_calibParams.nScreenHeightInmm)
        {
            const RECT & rcMonitor = m_calibParams.allCalibCoefs[nMonitorId].rcMonitor;
            int nMonitorScreenWidth = rcMonitor.right - rcMonitor.left;
            int nMonitorScreenHeight = rcMonitor.bottom - rcMonitor.top;
            
            for (int i = 0; i < nPtNumber; i++)
            {
                vecPtNormalizedWorld[i].d[0] = m_calibParams.nScreenWidthInmm *(ptWorld[i].d[0] - rcMonitor.left) / nMonitorScreenWidth;
                vecPtNormalizedWorld[i].d[1] = m_calibParams.nScreenHeightInmm*(ptWorld[i].d[1] - rcMonitor.top) / nMonitorScreenHeight;
            }
            ptWorld = &vecPtNormalizedWorld[0];
        }


        BOOL bRet = m_vecCameraModels[nMonitorId].MapWorldToImage(ptWorld, nPtNumber, ptImage);

        return bRet;
    }

    //@功能:计算自动校正时摄像头接收可见光,无红外滤光片和正常使用时有红外滤光片,通过940nm波长时,光斑的偏移距离
    TVector2D GetRefractionOffset(const CGenericCameraModel& camera, const TPoint2D& ptObj)
    {

        double back_focal_length         = m_tLensSpec.backFocalLength    ;//镜头后焦距, 单位:mm
        double image_sensor_width        = m_tCMOSChipSepc.width_in_mm    ;//图形传感器有效区宽度, 单位:mm
        double image_sensor_Height       = m_tCMOSChipSepc.height_in_mm   ;//图形传感器有效区高度, 单位:mm
        double image_sensor_pixel_width  = m_tCMOSChipSepc.width_in_pixel ;
        double image_sensor_pixel_height = m_tCMOSChipSepc.height_in_pixel;
        double pixel_size                = m_tCMOSChipSepc.pixel_size     ;//单位:mm, 即6um

        SIZE sizeImage = camera.GetImageSize();
        TVector2D  v;

        v.d[0] = ptObj.d[0] - m_tAutoCalibCompCoefs.u0 * sizeImage.cx;
        v.d[1] = ptObj.d[1] - m_tAutoCalibCompCoefs.v0 * sizeImage.cy;

        
        
        /*
        TPoint2D ptLensCenter = camera.GetLensCenterInImage();

        v.d[0] = ptObj.d[0] - ptLensCenter.d[0];
        v.d[1] = ptObj.d[1] - ptLensCenter.d[1];
        */

        double r = norm(v);

        if(r > std::numeric_limits<double>::epsilon())
        {
            v = v/r;
        }

        double theta  = atan(r*pixel_size/back_focal_length);
        double theta2  = theta  * theta;
        double theta3  = theta2 * theta;
        double theta5  = theta3 * theta2;

        //Matalb仿真计算表明,像素偏移量drθ和光点与光轴的夹角θ,近似满足多项式关系
        //dr=K(1)*θ + K(2)*θ^3 + K(3)**θ^5 

        double K1 = m_tAutoCalibCompCoefs.k[0];
        double K2 = m_tAutoCalibCompCoefs.k[1];
        double K3 = m_tAutoCalibCompCoefs.k[2];

        double dr = K1*theta + K2*theta3 + K3*theta5;
        
       TVector2D dv = dr*v;

        return dv;
    }

 
    //@功能:计算自动校正时摄像头接收可见光,无红外滤光片和正常使用时有红外滤光片,通过940nm波长时,光斑的偏移距离
    TVector2D GetRefractionOffsetEx(const CGenericCameraModel& camera, const TPoint2D& ptObj)
    {

        double theta, phi;
        
        camera.GetThetaPhi(&ptObj, 1, &theta, &phi);

        //Matalb仿真计算表明,像素偏移量drθ和光点与光轴的夹角θ,近似满足多项式关系
        //dr=K(1)*θ + K(2)*θ^3 + K(3)**θ^5 
        double K1 = m_tAutoCalibCompCoefs.k[0];
        double K2 = m_tAutoCalibCompCoefs.k[1];
        double K3 = m_tAutoCalibCompCoefs.k[2];
        
        double theta2 = theta  * theta;
        double theta3 = theta2 * theta;
        double theta5 = theta3 * theta2;

        double dr = K1*theta + K2*theta3 + K3*theta5;

        TVector2D dv;
        dv.d[0] = dr*cos(phi);
        dv.d[1] = dr*sin(phi);

        return dv;
    }
    

    //@功能:返回校正方程参数
    virtual const TCalibParams* GetCalibParams()const
    {
        return &m_calibParams;
    }

    //@功能:设置校正方程参数
    virtual  void SetCalibParams(const TCalibParams& params)
    {
        int nMonitorCount = params.allCalibCoefs.size();
        m_vecCameraModels.resize(nMonitorCount);
        for(int i = 0; i < nMonitorCount; i++)
        {
            if(params.allCalibCoefs[i].calibCoefs.size())
            {
                m_vecCameraModels[i].SetParameters(
                    &params.allCalibCoefs[i].calibCoefs[0],
                     params.allCalibCoefs[i].calibCoefs.size());
            }
        }//for

        m_calibParams = params;

    }

    virtual void SetLensSpecification(const TLensSpecification& lensSpec)
    {
        m_tLensSpec = lensSpec;
    }

    virtual void SetLensInternalAndSymmetricDistortParams(const TInternalAndSymmetricDistortParams* pInternalAndSymmetricDistortParams)
    {
        if (pInternalAndSymmetricDistortParams)
        {
            m_tInternalAndSymmetricDistortParams = *pInternalAndSymmetricDistortParams;
            m_bInternalAndSymmetricDistortParamsIsValid = TRUE;
        }
        else
        {
            m_bInternalAndSymmetricDistortParamsIsValid = FALSE;
        }
        
    }

    
    virtual void SetCMOSChipSpecification(const TCMOSChipSpecification& cmosChipSpec)
    {
        m_tCMOSChipSepc = cmosChipSpec;
    }

    virtual void SetAutoCalibCompCoefs(const TAutoCalibCompCoefs& autoCalibCompCoefs)
    {
        m_tAutoCalibCompCoefs = autoCalibCompCoefs;
    }


protected:
    std::vector<CGenericCameraModel> m_vecCameraModels     ;//相机投影模型数组, 每一个实际的屏幕对应一个模型。

#ifdef USE_CYLINDER_BULB_MODEL
     std::vector<CylinderBulbModel>   m_vecCylinderBulbModel;//圆柱形灯泡模型。
#endif
    TCalibParams m_calibParams;
    TInternalAndSymmetricDistortParams m_tInternalAndSymmetricDistortParams;//镜头内部和非对称畸变参数
    BOOL  m_bInternalAndSymmetricDistortParamsIsValid;//镜头内部和非对称畸变参数是否有效标志
    TLensSpecification     m_tLensSpec          ;//镜头规格
    TCMOSChipSpecification m_tCMOSChipSepc      ;//CMOS芯片规格
    TAutoCalibCompCoefs    m_tAutoCalibCompCoefs;//自动校正补偿系数


};


//四点定位校正模型
class Calibrator_4PointsPerspectiveCameraModel :public ICalibrate
{
public:
    Calibrator_4PointsPerspectiveCameraModel()
        :
        ICalibrate(E_CALIBRATE_MODEL_4_POINST_PERSPECTIVE)
    {

    }


    //@功能:计算校正参数
    //@输入:ALL_CALIB_DATA ,输入, 所有屏幕的校正数据
    //      screenSize,输入, 屏幕尺寸
    //      imageSize ,输入, 图象尺寸
    virtual BOOL CalcParams(const TCalibData& calibData, BOOL bDebug = FALSE)
    {
        int N = calibData.allMonitorCalibData[0].calibData.size();

        TPoint2D* pImagePts  = new TPoint2D[N];
        TPoint2D* pScreenPts = new TPoint2D[N];

        int nMonitorCount = calibData.allMonitorCalibData.size();

        m_calibParams.allCalibCoefs.resize(nMonitorCount);

        if (m_vecCameraModels.size() != nMonitorCount)
        {
            m_vecCameraModels.resize(nMonitorCount);
        }

        for (int nMonitorId = 0; nMonitorId < nMonitorCount; nMonitorId++)
        {
            //屏幕区域
            const RECT& rcMonitor = calibData.allMonitorCalibData[nMonitorId].rcMonitor;
            m_calibParams.allCalibCoefs[nMonitorId].rcMonitor = rcMonitor;


            const std::vector<TCalibCoordPair>& data = calibData.allMonitorCalibData[0].calibData;

            int N = calibData.allMonitorCalibData[0].calibData.size();

            std::vector<TPoint2D> vecImagePts;
            std::vector<TPoint2D> vecScreenPts;

            vecImagePts.resize(N);
            vecScreenPts.resize(N);
            
            
            for (int i = 0; i < N; i++)
            {
                vecImagePts[i] = data[i].pt2DImageCoord;
                vecScreenPts[i].d[0] = double(data[i].ptScreenCoord.x);
                vecScreenPts[i].d[1] = double(data[i].ptScreenCoord.y);

            }//for-each(i)


            BOOL bRet = m_vecCameraModels[nMonitorId].CalcParams(&vecImagePts[0], &vecScreenPts[0], N, calibData.szImage);

            if (FALSE == bRet)
                return bRet;

            //保存校正参数
            std::vector<double>& calibCoefs = m_calibParams.allCalibCoefs[nMonitorId].calibCoefs;
            int nParamCount = m_vecCameraModels[nMonitorId].GetParameterCount();
            const double* pModelParams = m_vecCameraModels[nMonitorId].GetParameters();
            calibCoefs.resize(nParamCount);
            for (int j = 0; j < nParamCount; j++)
            {
                calibCoefs[j] = pModelParams[j];
            }
           

        }//for=each(nMonitorId)

        m_calibParams.eCalibrateModel = calibData.eCalibrateModel;
        m_calibParams.szImage = calibData.szImage;
        m_calibParams.eCalibType = calibData.eCalibType;//校正类别:手动，自动

        return TRUE;
    }



    //@功能;根据视频坐标计算得出计算机屏幕坐标
    //@参数:ptImage, 光斑的视频图像坐标
    //      ptScreen, 保存计算机屏幕坐标的地址
    //      nMonitorId, 屏幕ID,从零开始
    //      bWithoutAutoCalibCompensate, 不做自动校正补偿标志
    virtual BOOL GetScreenPt(const TPoint2D& ptImage, TPoint2D* pptScreen, int nMonitorId = 0, BOOL bWithoutAutoCalibCompensate = FALSE, TCameraDebugData* pDebugOutput = NULL)
    {
        if (nMonitorId < 0 || size_t(nMonitorId) >= m_vecCameraModels.size()) return FALSE;

        BOOL bRet = m_vecCameraModels[nMonitorId].GetScreenPt(&ptImage,  pptScreen, 1);

        return bRet;
        
    }


	virtual BOOL MapWorldToImage(const TPoint3D* ptWorld, int nPtNumber, TPoint2D* ptImage, int nMonitorId)
	{
		if (nMonitorId < 0 || size_t(nMonitorId) >= m_vecCameraModels.size()) return FALSE;

		BOOL bRet = m_vecCameraModels[nMonitorId].MapWorldToImage(ptWorld, nPtNumber, ptImage);
		//if (bRet)
		//{
		//    for (int i = 0; i < nPtNumber; i++)
		//    {
		//        ptImage[i].d[0] /= m_tCMOSChipSpec.pixel_size;
		//        ptImage[i].d[1] /= m_tCMOSChipSpec.pixel_size;
		//    }
		//}
		return bRet;
	}

    //@功能:返回校正方程参数
    virtual const TCalibParams* GetCalibParams()const
    {
        return &m_calibParams;
    }

    //@功能:设置校正方程参数
    virtual  void SetCalibParams(const TCalibParams& params)
    {
        int nMonitorCount = params.allCalibCoefs.size();
        m_vecCameraModels.resize(nMonitorCount);
        for (int i = 0; i < nMonitorCount; i++)
        {
            if (params.allCalibCoefs[i].calibCoefs.size())
            {
                m_vecCameraModels[i].SetParameters(
                    &params.allCalibCoefs[i].calibCoefs[0],
                    params.allCalibCoefs[i].calibCoefs.size());
            }
        }//for

        m_calibParams = params;
    }


    //@功能:设置镜头规格数据
    virtual void SetLensSpecification(const TLensSpecification& lensSpec)
    {
        //do nothing
    }

    //@功能:设置CMOS芯片规格数据
    virtual void SetCMOSChipSpecification(const TCMOSChipSpecification& cmosChipSpec)
    {
        //do nothing
    }

    //@功能:设置自动校正补偿数据
    virtual void SetAutoCalibCompCoefs(const TAutoCalibCompCoefs& autoCalibCompCoefs)
    {
        //do nothing
    }

    //@功能:设置相机的已知的内部参数和对称畸变参数
    virtual void SetLensInternalAndSymmetricDistortParams(const TInternalAndSymmetricDistortParams* pLensInternalParams)
    {
        //do nothing
    }

protected:
    std::vector<CPerspectiveCameraModel> m_vecCameraModels;//相机投影模型数组, 每一个实际的屏幕对应一个模型。
    TCalibParams m_calibParams;
};