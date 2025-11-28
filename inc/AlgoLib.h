#pragma once
//@Date:2011/04/24
//@Author: toxuke@gmail.com

#ifdef ALGOLIB_EXPORTS
    #define ALGOLIB_API _declspec(dllexport)
#else
    #define ALGOLIB_API _declspec(dllimport)
#endif

#include <vector>
//Part 3. typedef std::vector<POINT> Points;
#include "CommonDefine.h"
#include "ImageFrame.h"
#include "BitFrame.h"
#include <atlstr.H>
#include "CameraSpecs.h"


#define ERR_SOFTKEY_VERSION  (-8)

#define ERR_SOFTKEY_APP_TYPE (-9)

#define ERR_START_CALIBRATING_FAILED (-10)

#define ERR_INVALID_PARAMETER (-11)



//Used In Part 1 and Part 2.
//
//================================================
//Part 1. 自动校正图像模式
enum E_AutoCalibratePattern
{

    E_CALIBRATE_PATTERN_5_COLS =  0,
    E_CALIBRATE_PATTERN_6_COLS  = 1,
    E_CALIBRATE_PATTERN_7_COLS  = 2,
    E_CALIBRATE_PATTERN_8_COLS  = 3,
    E_CALIBRATE_PATTERN_9_COLS  = 4,
    E_CALIBRATE_PATTERN_10_COLS = 5,
    E_CALIBRATE_PATTERN_11_COLS = 6,
    E_CALIBRATE_PATTERN_12_COLS = 7,
    E_CALIBRATE_PATTERN_13_COLS = 8,
    E_CALIBRATE_PATTERN_14_COLS = 9,
    E_CALIBRATE_PATTERN_15_COLS = 10,
    E_CALIBRATE_PATTERN_16_COLS = 11,
    E_CALIBRATE_PATTERN_17_COLS = 12,
    //E_CALIBRATE_SEARCH_SCREEN_AREA_ONLY = 9//只搜索屏幕区域模式
};

inline int GetPatternCols(E_AutoCalibratePattern pattern)
{
    return  9 + int(pattern);
}

inline E_AutoCalibratePattern GetAutoCalibratePattern(int cols)
{

    return E_AutoCalibratePattern(cols - 5);

}

//自动校正错误代码
enum EAutoCalibrateError
{
    E_AUTO_CALIBRATOR_OK,
    E_AUTO_CALIBRATOR_ERROR_DETECT_DISPLAY_AREA_FAILED,//未检测到屏幕区域
    E_AUTO_CALIBRATOR_ERROR_CORNER_DETECT_FAILED,//角点检测失败
    E_AUTO_CALIBRATOR_ERROR_CORNER_MATCH_FAILED,//角点匹配失败
    E_AUTO_CALIBRATOR_ERROR_UNDEF,//未知错误    

    E_AUTO_MASK_ERROR_SEARCH_UPPER_SCREEN_BORDER_FAILED,
    E_AUTO_MASK_ERROR_SEARCH_LOWER_SCREEN_BORDER_FAILED,
    E_AUTO_CALIBRATOR_ERROR_NOT_FOUND_DEVICE,
};


//自动校正错误代码
enum EAutoMaskingError
{
    E_AUTO_MASKING_OK,
    E_AUTO_MASKING_ERROR_DETECT_DISPLAY_AREA_FAILED,//未检测到屏幕区域
    E_AUTO_MASKING_ERROR_UNDEF,//未知错误
    E_AUTO_MASKING_NOT_FOUND_DEVICE,

};



//校正调试级别
enum ECalibDebugLevel
{
    E_CALIB_DEBUG_LEVEL_CONCISE,//精简检出
    E_CALIB_DEBUG_LEVEL_DEBUG,  //调试输出
    E_CALIB_DEBUG_LEVEL_VERBOSE, //详细输出
};


 //自动校正单元坐标
struct TCalibCoordPair
{
    POINT    ptScreenCoord  ;//
    TPoint2D pt2DImageCoord ;//高精度
   
    TCalibCoordPair()
    {
        ptScreenCoord.x = 0;
        ptScreenCoord.y = 0;

        pt2DImageCoord.d[0] = 0.0;
        pt2DImageCoord.d[1] = 0.0;

   }
};

//屏幕校正方程系数
struct TMonitorCalibCoefs
{
    RECT rcMonitor;
    std::vector<double> calibCoefs;
    TMonitorCalibCoefs()
    {
        rcMonitor.left   = 0;
        rcMonitor.right  = 0;
        rcMonitor.top    = 0;
        rcMonitor.bottom = 0;
    }


};

//定位校正模型
enum E_CALIBRATE_MODEL
{
    E_CALIBRATE_MODEL_GENERICAL_CAMERA     = 0,//通用相机模型
    E_CALIBRATE_MODEL_4_POINST_PERSPECTIVE = 1,//4点透视模型
    E_CALIBRATE_MODEL_COUNT = 2                //校正模型数目
};

//校正类型
enum E_CALIBRATE_TYPE
{
    E_CALIBRATE_TYPE_MANUAL = 0,//手动校正
    E_CALIBRATE_TYPE_AUTO   =1, //自动校正
};

//所有屏幕的校正方程系数
typedef std::vector<TMonitorCalibCoefs>  ALL_CALIB_COEFS;
struct TCalibParams
{
    TCalibParams()
    {
        eCalibType = E_CALIBRATE_TYPE_AUTO;
        szImage.cx = 640;
        szImage.cy = 480;

        eCalibrateModel = E_CALIBRATE_MODEL_GENERICAL_CAMERA;


        bUsingScreenPhysicalDimensions = FALSE;//使用屏幕物理尺寸标志
        nScreenWidthInmm  = 0;//屏幕物理宽度
        nScreenHeightInmm = 0;//屏幕物理高度

    }

    E_CALIBRATE_MODEL eCalibrateModel;//定位校正使用的模型

    E_CALIBRATE_TYPE eCalibType;//手动/自动校正
    SIZE szImage;
    ALL_CALIB_COEFS allCalibCoefs;

    BOOL bUsingScreenPhysicalDimensions;//使用屏幕物理尺寸标志
    LONG nScreenWidthInmm;//屏幕物理宽度
    LONG nScreenHeightInmm;//屏幕物理高度

};

struct TMonitorCalibData
{
    int radius;//校正圆的半径
    RECT rcMonitor;//显示器矩形区域
    std::vector<TCalibCoordPair> calibData;

    TMonitorCalibData()
    {
        radius = 0;
        rcMonitor.left   = 0;
        rcMonitor.top    = 0;
        rcMonitor.right  = 0;
        rcMonitor.bottom = 0;
    }
};

typedef  std::vector<TMonitorCalibData> ALL_MOITOR_CALIB_DATA ;

struct TCalibData
{

    TCalibData()
    {
        eCalibrateModel = E_CALIBRATE_MODEL_GENERICAL_CAMERA;
        eCalibType = E_CALIBRATE_TYPE_AUTO;
        szImage.cx = 1920;
        szImage.cy = 1080;
        lpCtx = NULL;//

        bUsingScreenPhysicalDimensions = false;
        nScreenWidthInmm  = 0;
        nScreenHeightInmm = 0;
    }

    E_CALIBRATE_MODEL eCalibrateModel;//定位校正使用的模型

    E_CALIBRATE_TYPE eCalibType;
    SIZE szImage;

    ALL_MOITOR_CALIB_DATA allMonitorCalibData;
    LPVOID lpCtx;//上下文辅助数据

    BOOL bUsingScreenPhysicalDimensions;//使用屏幕物理尺寸标志
    LONG nScreenWidthInmm ;//屏幕物理宽度
    LONG nScreenHeightInmm;//屏幕物理高度


};

enum EChangeCalibCameraParams
{
    E_CAMERA_SHARPNESS          ,//设置摄像头的锐利度, param1参数指定锐利度
    E_CAMERA_SHARPNESS_MAX      ,//设置摄像头的最大锐利度
    E_CAMERA_SHARPNESS_MIN      ,//设置摄像头的最小锐利度
    
    E_CAMERA_BRIGHTNESS_SET     ,//设置摄像头亮度, param1参数指定亮度
    E_CAMERA_BRIGHTNESS_INCREASE,//增加摄像头的亮度
    E_CAMERA_BRIGHTNESS_DECREASE,//减少摄像头的亮度

    E_CAMERA_CONTRAST_MAX       ,//设置摄像头的最大对比度
    E_CAMERA_CONTRAST_DEFAULT   ,//设置摄像头的缺省对比度
    E_CAMERA_CONTRAST           ,//设置摄像头对比度
    E_CAMERA_AUTO_CALIBRATE     ,//自动校正时的相机参数
    E_CAMERA_AUTO_MASK          ,//自动屏蔽时的相机参数
};


//自动校正时,用来控制摄像头视频参数的回调函数
typedef BOOL (*CHANGECAMERAPARAMSPROC)(EChangeCalibCameraParams eCtrlMode, LPVOID lpCtx, BYTE param1, BYTE param2);

//静态屏蔽前的回调函数，校正模块可以通过它来控制相机的参数和滤光片的闭合。
typedef BOOL   (CALLBACK *PRE_STATIC_MASKING_ROUTINE)(LPVOID lpCtx);



//自动校正时指定的画面参数
struct AutoCalibrateImageParams
{
    AutoCalibrateImageParams()
    {
        autoCalibrateExpectedBrightness = 0x80;
        autoCalibrateHilightGray = 255;
        autoCalibrateSpeed = 1;
        videoDislayDelay = 0;

    }
    BYTE autoCalibrateExpectedBrightness; //自动校正亮度自动控制时期望的平均画面亮度(范围0~255)
    BYTE autoCalibrateHilightGray       ; //自动校正图案中高亮块的灰度值(0~255)
    BYTE autoCalibrateSpeed             ; //自动校正速度(1~10), 1最快，10最慢
    BYTE videoDislayDelay               ;//视频显示延迟
};

struct AutoCalibrateParams
{
    AutoCalibrateImageParams  autoCalibrateImageParams;
    TVideoProcAmpProperty     videoprocampproperty;
};

//画面参数列表, 每一组参数对应一次校正尝试的画面参数
//typedef std::vector<AutoCalibrateImageParams> AutoCalibrateImageParamsList;

typedef std::vector<AutoCalibrateParams> AutoCalibrateParamsList;


//@功能:屏幕信息
struct TScreenInfo
{
    RECT rcArea;
};
//@功能:检索扩展的显示器信息
//自动校正参数
struct TAutoCalibrateParams
{
    //======校正参数======
    HWND hNotifyWnd                              ;//消息通知窗体句柄
    E_AutoCalibratePattern ePattern              ;//校正方案枚举值    
    CHANGECAMERAPARAMSPROC ChangeCameraParamsProc;//调节摄像头参数的回调函数
    LPVOID  lpCtx                                ;//ChangeCameraParamsProc的回调的上下文参数    
    ECalibDebugLevel eDebugLevel                 ;//调试级别
    BOOL bSaveInermediatFile                     ;//是否保存校正的中间结果图片
    BOOL bRecordVideo                            ;//是否记录校正过程中的原始视频
    std::vector<TScreenInfo> vecScreenInfos      ;//关联的显示器信息


    //BYTE cAvgBrightness                        ;//校正时的平均亮度
    //COLORREF clrGridHighlight                  ;//校正图案中高亮块的颜色值

//    AutoCalibrateImageParamsList imageParamsList ;//画面参数列表,

    AutoCalibrateParamsList   autocalibrateparamslist;   //自动校正参数列表

	BOOL   bEnableOnlineScreenArea;

    std::vector<POINT> vecOnlineAreaVertices;//在线区域的顶点。

};


//自动屏蔽参数
struct TAutoMaskingParams
{
    //======校正参数======
    HWND hNotifyWnd                              ;//消息通知窗体句柄
    CHANGECAMERAPARAMSPROC ChangeCameraParamsProc;//调节摄像头参数的回调函数
    LPVOID  lpCtx                                ;//ChangeCameraParamsProc的回调的上下文参数    
    ECalibDebugLevel eDebugLevel                 ;//调试级别
    BOOL bSaveInermediatFile                     ;//是否保存校正的中间结果图片
    BOOL bRecordVideo                            ;//是否记录校正过程中的原始视频
    std::vector<TScreenInfo> vecScreenInfos       ;//关联的显示器信息


    //BYTE cAvgBrightness                        ;//校正时的平均亮度
    //COLORREF clrGridHighlight                  ;//校正图案中高亮块的颜色值

//    AutoCalibrateImageParamsList imageParamsList ;//画面参数列表,
    AutoCalibrateParamsList       autocalibrateparamslist;   //自动校正参数列表

    BOOL                    bDoStaticMaskingOnly ;//只作静态屏蔽标志

};



//静态屏蔽参数
struct TStaticMaskingParams
{
    //=======静态屏蔽参数=======
    PRE_STATIC_MASKING_ROUTINE fpPreStaticMaskingProc ;//静态屏蔽前回调函数    
    LPVOID lpPreStaticMaskingCtx                      ;//静态屏蔽前回调函数的上下文参数
    BYTE   cStaticMaskThreshold                       ;//静态屏蔽门限
    int    nMaskEroseSize                             ;//屏蔽图腐蚀尺寸。

    TStaticMaskingParams()
    {
        fpPreStaticMaskingProc = NULL;
        lpPreStaticMaskingCtx  = NULL;
        cStaticMaskThreshold   = 180;
        nMaskEroseSize         = 10;
    }

};



class CAutoCalibratorImpl2;
class ALGOLIB_API CAutoCalibrator
{
public:
    CAutoCalibrator();

    ~CAutoCalibrator();
    //@功能:开始校正, 供外部模块调用
    //@参数:
    //      hNotifyWnd, 消息通知窗体句柄
    //      E_AutoCalibratePattern, 校正方案枚举值
    //      cBrightness, 校正时画面亮度平均值
    //      ChangeCameraParamsProc, 调节摄像头参数的回调函数
    //      lpCtx, ChangeCameraParamsProc的上下文参数
    //      clrGridHighlight, 校正图案中高亮块的显示颜色
    //      eDebugLevel, 调试级别
    //      bSaveInermediatFile, 是否保存校正的中间结果图片
    //      bRecordVideo，是否记录校正过程中的原始视频
    //@返回值:成功返回 0;
    //@说明:开始自动校正后, 会生成校正窗体, "自动校正"状态机回到初始状态。
   // HRESULT StartCalibrating(HWND hNotifyWnd, E_AutoCalibratePattern eGridPattern=E_CALIBRATE_PATTERN_11_COLS, BYTE cBrightness = 50, CHANGECAMERAPARAMSPROC ChangeCameraParamsProc = NULL, LPVOID lpCtx = NULL, COLORREF clrGridHighlight=RGB(255,255,255),ECalibDebugLevel eDebugLevel=E_CALIB_DEBUG_LEVEL_CONCISE, BOOL bSaveInermediatFile = FALSE ,BOOL bRecordVideo = FALSE);
    
    //@功能:开始校正, 供外部模块调用
    //@参数:autoCalibrateParams, 自动校正参数
    //      staticMaskingParam, 静态屏蔽参数
     BOOL StartCalibrating(const TAutoCalibrateParams& autoCalibrateParams, const TStaticMaskingParams& staticMaskingParams);

     //@功能:开始自动屏蔽,供外部模块调用
     BOOL StartMasking(const TAutoMaskingParams& autoMaskingParams, const TStaticMaskingParams& staticMaskingParams, int nSrcWidth, int nSrcHeight);

    
    ////@功能:外部程序开启自动校正后, 将来自摄像头的图片帧送入自动校正模块, 驱动自动校正状态机的运行。
    ////@参数:pYUVFrame, 指向YUY2格式的位图帧
    ////@返回值:成功返回TRUE
    ////        失败返回FALSE
    //BOOL ProcessImage(const CImageFrame* pYUVFrame);

    //@功能:结束自动校正,
    //@说明:
    //void EndCalibrating();
    

     //@功能:获取自动校正生成的计算屏幕影像的屏蔽图
     //@说明:自动校正算法会自动识别出计算机屏幕在摄像头图像中所占的区域
    //       屏蔽图是一个黑白二值图, 值为0XFF的区域对应屏幕区域，0x00为
    //       需要遮蔽区域。
     const CImageFrame& GetMaskFrame()const;
	 
	 ///@功能:获取最后自动屏蔽的屏蔽图
	 ///vera_zhao 2020.2.20
	 const CImageFrame& GetMaskFinderFrame(CImageFrame& maskframe)const;


      //@功能:返回光点位置到屏幕编号的映射数组,以决定光斑处于哪个屏幕上
      const CImageFrame& GetScreenMap()const;


    //@功能:返回屏幕个数
    int GetMonitorCount()const;

     //@功能:获取指定屏幕的校正数据
     //@参数:nMonitorID， 显示器Id
     //@说明:
     //const TMonitorCalibData& GetMonitorCalibData(int nMonitorID) const;


    //@功能:获取校正窗体的句柄
    //@说明:外部程序获取的校正图片帧通过发送消息WM_MANUAL_CALIBRATE_DATA通知给校正窗体, 校正窗体
    //      将数据送进"校正状态机", 驱动状态机的运行。
    HWND GetCalibrateHWnd()const;

    //@功能:获取校正时的屏幕尺寸
    //@说明:在模拟校正时,校正时的屏幕尺寸和计算机当前屏幕尺寸可能不同。
     SIZE GetScreenSize()const;

     
    //@功能:获取校正时的视频图像尺寸
    //@说明:
    SIZE GetImageSize()const;


    CAtlString  GetCalibrateIntermediataDirName();
    //调试函数
    //@功能:通过分析校正过程中录制的录像文件, 再现校正过程, 以便于调试校正算法。
    //@参数:lpszAVIFilePath, 录制下的AVI文件路径
    //      hNotifyWnd,  当校正结束后, 接收校正完毕消息WM_AUTO_CALIBRATE_DONE的窗体句柄。
    //      eGridNumber, 自动校正时, 显示的棋盘格校正图案的模式

    BOOL DoSimulateCalibrate(LPCTSTR lpszAVIFilePath, HWND hNotifyWnd,  UINT uCx, UINT uCy, BYTE cBrightness, E_AutoCalibratePattern eGridsNumber=E_CALIBRATE_PATTERN_11_COLS,  ECalibDebugLevel eDebugLevel=E_CALIB_DEBUG_LEVEL_DEBUG);


    //调试函数:测试亮度自动调节功能
    void TestAutoBrightnessControl(CHANGECAMERAPARAMSPROC ChangeCameraParamsProc = NULL, LPVOID lpCtx = NULL);

    //调试函数:查看校正图案        
    void ViewCalibratePattern();


     //@功能:获取所有屏幕的校正数据
      const TCalibData* GetCalibrateData() const;

    //@功能:返回调试用录像文件完整路径
     const TCHAR* GetDebugVideoFilePath()const;

     void OnDeviceMissing();



protected:
    CAutoCalibratorImpl2* m_pAutoCalibratorImpl;//指向自动校正对象的实例

};


//================================================
//判断光斑是从左向右偏还是从右向左偏。
//如果偏向被设置的话，那么光斑是需要被分割的，如果偏向没有被设置的话，光斑是不需要被分割的。
enum E_SpotDirection
{
    E_SPOT_NODIRECTION,
    E_SPOT_LEFTDIRECTION,
    E_SPOT_RIGHTDIRECTION,
};

struct TObjDimension
{
    RECT rcArea;
    int radius;
};

struct  TInvalidSpotList
{
    RECT  rOriginalArea;    //最初的无效面积
    RECT  rExpansionInvalidArea ;    //扩展后无效面积
    int   nCount ;          //计数
    bool  bMatch   ;   //有效和无效是否匹配。如果是匹配的，count加2，如果不是匹配的话，计数减1
    bool  bInvalid ;   //重复的标记为无效的。
    E_SpotDirection   eSplitDirection;
    int   nSplitNum;  //被分割的个数

};
//struct TMatchSpot
//{
//	RECT  rInvalidArea;
//	
//};


//Part 2. 光点检测算法
//目标信息
struct TBlobObject
{
    //int nObjectId      ;//目标Id
    unsigned int mass    ;//质量
    unsigned int mx      ;//对x轴的力矩
    unsigned int my      ;//对y轴的力矩

    POINT GPos         ;//重心坐标
    RECT rcArea        ;//区域
    BOOL bIsValid      ;//正确标志
    BOOL bReferenced   ;//是否用来作为参考点。//过小的光斑和外部的光斑不能用来作为无效光斑的参考
    int  objToInvalidNumber;   //无效光斑列表对应
    E_SpotDirection   eSplitDirection;  //被分割的方向
    int   nSplitNum   ;  //被分割的个数
    //
    //LONG ActualArea    ;//光斑的实际面积    
    //LONG CalculateArea ;//计算的面积=
    //POINT debugPtContourCross[2];//

    bool operator < (TBlobObject& right)
    {
        return (mass < right.mass);
    }

    bool operator <= (TBlobObject& right)
    {
        return (mass <= right.mass);
    }


    bool operator > (TBlobObject& right)
    {
        return (mass > right.mass);
    }

    bool operator >= (TBlobObject& right)
    {
        return (mass >= right.mass);
    }

};

#define MAX_OBJ_NUMBER 256

template<int MAX_OBJECT_COUNT= MAX_OBJ_NUMBER,int MAX_IMAGE_WIDTH = 1920, int MAX_IMAGE_HEIGHT = 1080>
class CConnectedComponentScanlineAlogrithm;
class m_pBitFrame;
class m_YUY2Frame;
class ALGOLIB_API CBlobDetector
{
public:

    CBlobDetector();

    ~CBlobDetector();

    //@功能:检测位图中亮度大于指定门限值的不规则区域的最大外接矩形, 并计算此区域的重心。
    //@参数:frame, YUY2格式的位图帧
    //      YThreshold, 检测的亮度门限
    //      pClipArea, 可选参数, 屏蔽裁减区域尺寸
    //@返回:检测到的目标的个数
    //@说明:
    //UINT operator() (const CYUY2Frame& YUY2Frame, BYTE YThreshold, CYUY2Frame& StatisticalFrame,CYUY2Frame& DynamicFrame, UINT FixedSpotTime,const RECT* pClipArea=NULL/* const CBitFrame** ppBitFrame=NULL*/);
    //UINT CBlobDetector::operator() (const CYUY2Frame& YUY2Frame, BYTE YThreshold, CImageFrame& StatisticalFrame, const CYUY2Frame& DynamicMaskFrame, int StatisticStep = 1);
    UINT CBlobDetector::operator() (const CImageFrame& GrayFrame, BYTE YThreshold, CImageFrame& StatisticalFrame, const CImageFrame& DynamicMaskFrame, int StatisticStep = 1, BYTE YStatisicThreshold = 0,bool bDesktopMode= true);

    //@功能:获取目标列表
    const TBlobObject* GetObjs()const;
    //@功能：获得目标列表，不是静态的
    TBlobObject* GetObjs();

    //	void AnalysisFixedBolb(CYUY2Frame& YUY2Frame);
    const CImageFrame* GetBinarizedImage()const;
protected:

    CConnectedComponentScanlineAlogrithm<>* m_pBlobScanAlgo;
    CBitFrame*  m_pBitFrame;
    //CYUY2Frame* m_pYUY2Frame;
    CImageFrame*  m_pGrayFrame;
    unsigned  int m_nFrameCount;
    

};
//====================================================
//struct TBolbInfo
//{
//	RECT rcArea          ;        //区域
//	POINT Gpt            ;        //重心坐标
//	DWORD dwStartTime    ;        //采集到这个光斑的时间
//	BOOL  bSMatched      ;        //匹配的状态
//};


//====================================================
//Part 3.坐标校正算法
typedef std::vector<POINT> Points;

//
//@功能:计算摄像头图象坐标到屏幕坐标的映射的单应矩阵(Homography Matrix),
//@输入:screenPts ,输入, 屏幕坐标
//      imagePts  ,输入, 图象坐标
//      screenSize,输入, 屏幕尺寸
//      imageSize ,输入, 图象尺寸
//      mapMatrix ,输出,图象映射矩阵, 其尺寸=sizeof(POINT)*IMAGE_WIDTH*IAMGE_HEIGHT.
//      lpClipRect,输出,保存屏幕区域外接矩形
//@输出:错误代码
//      0:成功, 其他:失败。
//      错误码   含义
//        -1,加密锁错误,详见加密狗接口库
//         ...
//        -7, 加密锁错误,详见加密狗接口库
//        -10,输入数据错误
//
//
//@说明:screenPts[i] 与imagePts[i](i=1,...,N)是一对匹配点。
//      mapMatrix[i]中的数据已经归一化到0~65535范围内  
//extern "C" ALGOLIB_API LONG CoordCalibrate(const Points& screenPts, const Points& imagePts, const SIZE& screenSize, const SIZE& imageSize, SHORT_POINT* mapMatrix, RECT* lpClipRect);

class ICalibrate;

class  ALGOLIB_API CalibrateAlgo
{
public:

        CalibrateAlgo();

        ~CalibrateAlgo();

        //@功能:计算校正参数
        //@输入:data ,输入, 校正参数
       BOOL CaclCalibrationParams(const TCalibData& data, BOOL bDebug = FALSE);

       //@功能:根据摄像头的视频坐标计算得出屏幕坐标
       BOOL GetScreenCoord(const TPoint2D& ptVideo, TPoint2D* ptScreen, int nMonitorId = 0,BOOL bWithoutAutoCalibCompensate = FALSE, TCameraDebugData* pDebugOutput = NULL);
       
       /*
       //@功能:根据摄像头的视频坐标计算得出屏幕坐标
       //@参数:pImgPts, 指向光斑的相机坐标点数组的指针
       //      prcBounds, 指向光斑的外界矩形数组的指针
       //      nPtNumber, 光斑个数
       //      pScreenPts, 指向保存屏幕坐标数组的指针
       //      nMonitorId, 屏幕Id,在一个镜头看多个屏幕时才有效。
       BOOL GetScreenCoord_Aug(const POINT* pImgPts,  const RECT* prcBounds, int nPtNumber, POINT* pScreenPts, int nMonitorId = 0);
       */

       //@功能:将3D世界坐标映射为2D相机图像坐标
       BOOL MapWorldToImage(const TPoint3D* ptWorld, int nPtCount, TPoint2D* ptImage, int nMonitorId = 0);

       /*
       //@功能:根据摄像头的视频坐标计算得出屏幕坐标
       //@参数:pCentroid, 光斑的重心
       //      pContourPoints, 指向光斑轮廓点的数组
       //      nContourPointCount, 光斑轮廓点个数
       //      pScreenPts, 指向保存屏幕坐标的内存地址的指针
       //      nMonitorId, 屏幕Id,在一个镜头看多个屏幕时才有效。
       BOOL GetScreenCoord_WithLightSpotContour(const POINT& pCentroid,  const POINT* pContourPoints, int nContourPointCount, POINT* pScreenPts, int nMonitorId = 0, POINT* debugPtContourCross=NULL);
       */

    //@功能:设置镜头规格数据
    void SetLensSpecification(const TLensSpecification& lensSpec);

    //@功能:设置CMOS芯片规格数据
    void SetCMOSChipSpecification(const TCMOSChipSpecification& cmosChipSpec);

    //@功能:设置自动校正补偿数据
    void SetAutoCalibCompCoefs(const TAutoCalibCompCoefs& autoCalibCompCoefs);

    //@功能:设置镜头的已知的内部参数和对称畸变参数
    void SetLensInternalAndSymmmetricDistortParams(const TInternalAndSymmetricDistortParams* pParams);


     //@功能:返回校正方程参数
     const TCalibParams* GetCalibParams()const;

      //@功能:设置校正方程参数
      void SetCalibParams(const TCalibParams& param);

      //@功能:生成指定类型的校正实例
      void GenCalibratorInst(E_CALIBRATE_MODEL eCalibrateModel);
protected:
    void CreateCalibrateInst(E_CALIBRATE_MODEL eCalibrateModel);
    ICalibrate* m_pCalibrateInst;
    CComAutoCriticalSection* m_pcsForParam;//保护参数线程安全的关键段

};

//====================================================
#include "NewFrameSink.h"
//Part 4. 屏幕区域侦测
class ALGOLIB_API CScreenAreaDetector
{
public:
    //@功 能:开启侦测,同时显示全屏窗口
    //@参 数:hNotifyWnd, 接收侦测结束通知的窗体句柄
    //@返回值;	
    BOOL StartDetect(HWND hNotifyWnd);

    //@功  能:结束侦测功能,
    //@参  数:
    //@返回指:
    BOOL End();


    //@功能:返回屏幕区域屏蔽位图
    const CImageFrame& GetMaskFrame()const;

    operator INewFrameEventSink*();
    operator INewFrameEventSink&();

};


//====================================================
//Part 5. 输出目录设置函数
extern "C" ALGOLIB_API void ALGOAPI_SetIntermediateDir(const TCHAR* szIntermediateDir);
extern "C" ALGOLIB_API const TCHAR* ALGOAPI_GetIntermediateDir();