// AlogLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

//#include "GlobalVar.h"
//
//#include "../inc/DispMonitorFinder.h"
////导出接口类及函数声明。
//#include "../inc/AlgoLib.h"
//#include "../inc/EasiSDKReg_Interface.h"
////#include "./AutoCalibrator/AutoCalibratorImpl.h"
//#include "./AutoCalibrator/AutoCalibratorImpl2.h"
//#include "./BlobDetector/BlobDetectorImpl.h"
//#include "./Calibrator/CalibrateAlgo.h"
//#include "./ScreenAreaDetector/ScreenAreaDetectorImpl.h"
#include "headers.h"

#include "../inc/EasiSDKReg_Interface.h"

#pragma comment(lib,"../lib/EasiSDKReg_Interface.lib")



//@功能:检查系统中的USB加密锁格式是否正确
#ifdef _DEBUG
HRESULT CheckUSBKey()
{
    return S_OK;
}

#else
HRESULT CheckUSBKey()
{
    return S_OK;

    HRESULT hr = S_OK;
    float fVersion = 0.0f;
    hr = SDKREG_GetVersion(&fVersion);

    if(hr!= S_OK)
    {
        return hr;
    }

    hr = SDKREG_IsBasicFunctionEnabled(14);
    if(hr != S_OK)
    {
        return hr;
    }


    // 	if(fVersion < 0.20111018f)
    // 	{
    // 		return ERR_SOFTKEY_VERSION;//版本错误
    // 	}
    // 
    // 	int nAppType = 0;
    // 	hr = SDKREG_GetAppType(&nAppType);
    // 
    // 	if(hr != S_OK)
    // 	{
    // 		return hr;		
    // 	}
    // 
    // 	if(nAppType != 1)
    // 	{
    // 		return ERR_SOFTKEY_APP_TYPE;
    // 	}

    return S_OK;
}
#endif

//Part 1.自动校正对象
//@功能:返回自动校正对象的实例。
//
//
//IAutoCalibratorInterface* GetAutoCalibratorInstance()
//{
//    static CAutoCalibratorImpl2 s_AutoCalibratorInstance;
//    //static CAutoCalibratorImp s_AutoCalibratorInstance;
//
//    return &s_AutoCalibratorInstance;
//}

CAutoCalibrator::CAutoCalibrator()
:
m_pAutoCalibratorImpl(NULL)
{
    m_pAutoCalibratorImpl = new CAutoCalibratorImpl2();

}


CAutoCalibrator::~CAutoCalibrator()
{
    delete m_pAutoCalibratorImpl;
}


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
//HRESULT CAutoCalibrator::StartCalibrating(HWND hNotifyWnd, E_AutoCalibratePattern eGridPattern, BYTE cBrightness, CHANGECAMERAPARAMSPROC ChangeCameraParamsProc, LPVOID lpCtx, COLORREF clrGridHighlight, ECalibDebugLevel eDebugLevel, BOOL bSaveInermediatFile,BOOL bRecordVideo)

//@功能:开始校正, 供外部模块调用
//@参数:autoCalibrateParams 自动校正参数
BOOL CAutoCalibrator::StartCalibrating(const TAutoCalibrateParams& autoCalibrateParams, const TStaticMaskingParams& staticMaskingParams)
{
    
     BOOL bRet = m_pAutoCalibratorImpl->StartCalibrating(autoCalibrateParams, staticMaskingParams);
    return bRet;

}


//@功能:开始自动屏蔽,供外部模块调用
BOOL CAutoCalibrator::StartMasking(const TAutoMaskingParams& autoMaskingParams, const TStaticMaskingParams& staticMaskingParams,int nSrcWidth ,int nSrcHeight)
{
    BOOL bRet = m_pAutoCalibratorImpl->StartMasking(autoMaskingParams, staticMaskingParams, nSrcWidth, nSrcHeight);

    return bRet;
}


BOOL CAutoCalibrator::DoSimulateCalibrate(LPCTSTR lpszAVIFilePath, HWND hNotifyWnd, UINT nCx, UINT nCy, BYTE cBrightness, E_AutoCalibratePattern eGridsNumber, ECalibDebugLevel eDebugLevel)
{
    m_pAutoCalibratorImpl->SetDebugLevel(eDebugLevel);
    return m_pAutoCalibratorImpl->DoSimulateCalibrate(lpszAVIFilePath, hNotifyWnd, nCx, nCy, cBrightness, eGridsNumber);
}

const CImageFrame& CAutoCalibrator::GetMaskFrame()const
{
    return m_pAutoCalibratorImpl->GetMaskFrame();

}

const CImageFrame& CAutoCalibrator::GetMaskFinderFrame(CImageFrame& maskframe)const
{
	return m_pAutoCalibratorImpl->GetMaskFinderFrame(maskframe);
}


//@功能:返回光点位置到屏幕编号的映射数组,以决定光斑处于哪个屏幕上
const CImageFrame& CAutoCalibrator::GetScreenMap()const
{
    return m_pAutoCalibratorImpl->GetScreenMap();
}

//@功能:返回屏幕个数
int CAutoCalibrator::GetMonitorCount()const
{
    return m_pAutoCalibratorImpl->GetMonitorCount();
}


//@功能:获取指定屏幕的校正数据
//@参数:nMonitorID， 显示器Id
//@说明:
//const TMonitorCalibData& CAutoCalibrator::GetMonitorCalibData(int nMonitorID) const
//{
//    return m_pAutoCalibratorImpl->GetMonitorCalibData(nMonitorID);
//}



HWND CAutoCalibrator::GetCalibrateHWnd()const
{
    return m_pAutoCalibratorImpl->GetCalibrateHWnd();
}

//@功能：获得校正失败的文件的路径
//@说明：在自动校正时，如果校正完全失败后，需要打开失败时保存的图片，此时就需要得到失败后文件的路径
CAtlString CAutoCalibrator::GetCalibrateIntermediataDirName()
{
	return m_pAutoCalibratorImpl->GetCalibrateIntermediataDirName();
}

//@功能:获取校正时的屏幕尺寸
//@说明:在模拟校正时,校正时的屏幕尺寸和计算机当前屏幕尺寸可能不同。
SIZE CAutoCalibrator::GetScreenSize()const
{
    return m_pAutoCalibratorImpl->GetScreenSize();
}



//@功能:获取校正时的视频图像尺寸
//@说明:
SIZE CAutoCalibrator::GetImageSize()const
{
    return m_pAutoCalibratorImpl->GetImageSize();
}




//调试函数:测试亮度自动调节功能
void CAutoCalibrator::TestAutoBrightnessControl(CHANGECAMERAPARAMSPROC ChangeCameraParamsProc , LPVOID lpCtx)
{
    return m_pAutoCalibratorImpl->TestAutoBrightnessControl(ChangeCameraParamsProc, lpCtx);
}



//调试函数:查看校正图案
void CAutoCalibrator::ViewCalibratePattern()
{
    m_pAutoCalibratorImpl->ViewCalibratePattern();
}



//@功能:获取所有屏幕的校正数据
const TCalibData* CAutoCalibrator::GetCalibrateData()const
{
        if(m_pAutoCalibratorImpl)
        {
            return m_pAutoCalibratorImpl->GetCalibrateData();
        }
        return  NULL;
}


//@功能:返回调试用录像文件完整路径
const TCHAR* CAutoCalibrator::GetDebugVideoFilePath()const
{
        if(m_pAutoCalibratorImpl)
        {
            return m_pAutoCalibratorImpl->GetDebugVideoFilePath();
        }
        return  NULL;
}

void CAutoCalibrator::OnDeviceMissing()
{
	if (m_pAutoCalibratorImpl)
	{
		m_pAutoCalibratorImpl->OnDeviceMissing();
	}
	return;
}


CBlobDetector::CBlobDetector()
:
m_nFrameCount(0)
{
    m_pBlobScanAlgo = new CConnectedComponentScanlineAlogrithm<>();
    m_pBitFrame     = new CBitFrame();
    //m_pYUY2Frame    = new CYUY2Frame();
    m_pGrayFrame    = new CImageFrame();
}


CBlobDetector::~CBlobDetector()
{
    delete m_pBlobScanAlgo;
    delete m_pBitFrame;
    //delete m_pYUY2Frame;
    delete m_pGrayFrame;

}


//@功能:1.统计并计算
//      2.检测位图中亮度大于指定门限值的不规则区域的最大外接矩形, 并计算此区域的重心。
//@参数:frame, 8Bit灰度格式的位图帧
//      YThreshold, 检测的亮度门限
//      StatisticalFrame, 像素大于门限的次数的统计表，每个像素用一个字节记录统计值
//      DynamicMaskFrame, 动态屏蔽位图
//      StatisticStep,统计图运算间隔,单位:帧, 即每隔StatisticStep帧，运行一次更新一次统计表
//      YStatisicThreshold, 动态屏蔽图的门限
//@返回:检测到的目标的个数
//@说明:

UINT CBlobDetector::operator() (const CImageFrame& grayFrame, BYTE YThreshold, CImageFrame& StatisticalFrame, const CImageFrame& DynamicMaskFrame, int StatisticStep, BYTE YStatisicThreshold,bool bDynamicMasking)
{

    //CPerfDetector perf(_T("CBlobDetector::operator()"));
    int nWidth  = grayFrame.Width();
    int nHeight = grayFrame.Height();

    int nPixelCount = nWidth * nHeight;

	//CPerfDetector perf(_T("YUY2ToBits"));
	m_pBitFrame->SetSize(nWidth, nHeight);
	*m_pGrayFrame = grayFrame;

    //统计自动屏蔽位图
	//如果需要动态屏蔽的话，那么就进行动态屏蔽
	if(bDynamicMasking)
    {
		//static  unsigned int s_FrameCount = 0;
		if (StatisticStep == 0)
		{
			StatisticStep = 1;
		}
		if (m_nFrameCount % StatisticStep == 0)
		{
			//CPerfDetector perf(_T("DynamicMaskStatistic_MMX"));
			if (YStatisicThreshold == 0)
			{
				YStatisicThreshold = YThreshold - 40;
			}
			///////进行动态屏蔽状态图的统计功能
			///////只是进行统计图的计算，不对grayFrame的数据进行改变。
			DynamicMaskStatistic_Gray_MMX(grayFrame.GetData(), StatisticalFrame.GetData(), YStatisicThreshold, nPixelCount);

			m_nFrameCount = 0;
		}
		m_nFrameCount++;

        //先用动态屏蔽图屏蔽源数据
        //YUY2FrameMask_MMX(m_pYUY2Frame->GetData(),DynamicMaskFrame.GetData(), nPixelCount);
        GrayFrameMask_MMX(m_pGrayFrame->GetData(), DynamicMaskFrame.GetData(),nPixelCount);
	}

    //YUY2图片帧根据门限转为单位位图
    {    
        //转化为单位位图
        //YUY2ToBitFrame_SSE(m_pYUY2Frame->GetData(), (BYTE*)m_pBitFrame->GetData(),YThreshold, nPixelCount);
        //GrayToBitFrame_SSE2(m_pGrayFrame->GetData(), (BYTE*)m_pBitFrame->GetData(),YThreshold, nPixelCount);
        BinarizeGrayToBitFrame_SSE2(m_pGrayFrame->GetData(), (BYTE*)m_pBitFrame->GetData(),YThreshold, nPixelCount);
        BitToGrayFrame_MMX((BYTE*)m_pBitFrame->GetData(), m_pGrayFrame->GetData(), nPixelCount);

    }
    //在单位位图中检测目标
    {
        m_pBlobScanAlgo->ProcessImage((const BYTE*)m_pBitFrame->GetData(), m_pBitFrame->Width(), m_pBitFrame->Height());
    }
    UINT nObjCount = m_pBlobScanAlgo->GetObjCount();
    return nObjCount;
}

//@功能:获取目标列表
const TBlobObject* CBlobDetector::GetObjs()const
{
    return m_pBlobScanAlgo->GetObjs();
}

TBlobObject* CBlobDetector::GetObjs()
{
	return m_pBlobScanAlgo->GetObjs();
}

const CImageFrame* CBlobDetector::GetBinarizedImage() const 
{
    return m_pGrayFrame;
}
//====================================================
//Part 3.坐标校正算法


CalibrateAlgo::CalibrateAlgo()
:
m_pCalibrateInst(NULL)
{
    //m_pCalibrateInst = new Calibrator_PinHoleCameraModel();
    m_pcsForParam = new CComAutoCriticalSection();
}

CalibrateAlgo::~CalibrateAlgo()
{
    if(m_pCalibrateInst)
    {
        delete m_pCalibrateInst;
    }

    if(m_pcsForParam)
    {
        delete m_pcsForParam;
    }
}

void CalibrateAlgo::CreateCalibrateInst(E_CALIBRATE_MODEL eCalibrateModel)
{
    if (m_pCalibrateInst)
    {
        delete m_pCalibrateInst;
        m_pCalibrateInst = NULL;
    }

    switch (eCalibrateModel)
    {
    case E_CALIBRATE_MODEL_GENERICAL_CAMERA:
        m_pCalibrateInst = new Calibrator_GenericCameraModel();
        break;

    case E_CALIBRATE_MODEL_4_POINST_PERSPECTIVE:
        m_pCalibrateInst = new Calibrator_4PointsPerspectiveCameraModel();
        break;
        
    }

}

//@功能:计算校正参数
//@输入:calibData ,输入, 所有屏幕的校正数据
//      bDebug, 调试标志
BOOL CalibrateAlgo::CaclCalibrationParams(const TCalibData& calibData, BOOL bDebug)
{
     CComCritSecLock<CComCriticalSection> lock(*m_pcsForParam);
    int N = calibData.allMonitorCalibData[0].calibData.size();
   
    if(NULL == m_pCalibrateInst || calibData.eCalibrateModel != m_pCalibrateInst->GetCalibrateModel())
    {
        CreateCalibrateInst(calibData.eCalibrateModel);
    }

    BOOL bRet = m_pCalibrateInst->CalcParams(calibData, bDebug);

    return bRet;

}

 //@功能:返回校正方程参数
const TCalibParams* CalibrateAlgo::GetCalibParams()const
{
    if(m_pCalibrateInst)
    {
        return m_pCalibrateInst->GetCalibParams();
    }
    return NULL;
}

//@功能:设置校正方程参数
void CalibrateAlgo::SetCalibParams(const TCalibParams& params)
{   
    CComCritSecLock<CComCriticalSection> lock(*m_pcsForParam);

    if( 0 == params.allCalibCoefs.size()) return;

    if (NULL == m_pCalibrateInst || params.eCalibrateModel != m_pCalibrateInst->GetCalibrateModel())
    {
        CreateCalibrateInst(params.eCalibrateModel);
    }

    m_pCalibrateInst->SetCalibParams(params);

}


//@功能:根据摄像头的视频坐标计算得出屏幕坐标
BOOL CalibrateAlgo::GetScreenCoord(const TPoint2D& ptVideo, TPoint2D* ptScreen, int nMonitorId, BOOL bWithoutAutoCalibCompensate, TCameraDebugData* pDebugOutput)
{
    CComCritSecLock<CComCriticalSection> lock(*m_pcsForParam);

    if(NULL == m_pCalibrateInst)
    {
        return FALSE;
    }
    return m_pCalibrateInst->GetScreenPt(ptVideo, ptScreen, nMonitorId, bWithoutAutoCalibCompensate, pDebugOutput);
}


//@功能:将3D世界坐标映射为2D相机图像坐标
BOOL CalibrateAlgo::MapWorldToImage(const TPoint3D* ptWorld, int nPtCount, TPoint2D* ptImage, int nMonitorId)
{
	CComCritSecLock<CComCriticalSection> lock(*m_pcsForParam);

	if (NULL == m_pCalibrateInst)
	{
		return FALSE;
	}

	return m_pCalibrateInst->MapWorldToImage(ptWorld, nPtCount, ptImage, nMonitorId);
}


/*
//@功能:根据摄像头的视频坐标计算得出屏幕坐标
//@参数:pImgPts, 指向光斑的相机坐标点数组的指针
//      prcBounds, 指向光斑的外界矩形数组的指针
//      nPtNumber, 光斑个数
//      pScreenPts, 指向保存屏幕坐标数组的指针
//      nMonitorId, 屏幕Id,在一个镜头看多个屏幕时才有效。
BOOL CalibrateAlgo::GetScreenCoord_Aug(const POINT* pImgPts,  const RECT* prcBounds, int nPtNumber, POINT* pScreenPts, int nMonitorId)
{
    CComCritSecLock<CComCriticalSection> lock(*m_pcsForParam);

    if(NULL == m_pCalibrateInst)
    {
        return FALSE;
    }

    return m_pCalibrateInst->GetScreenCoord_Aug(pImgPts, prcBounds, nPtNumber, pScreenPts, nMonitorId);
}
*/


/*
//@功能:根据摄像头的视频坐标计算得出屏幕坐标
//@参数:pCentroid, 光斑的重心
//      pContourPoints, 指向光斑轮廓点的数组
//      nContourPointCount, 光斑轮廓点个数
//      pScreenPts, 指向保存屏幕坐标的内存地址的指针
//      nMonitorId, 屏幕Id,在一个镜头看多个屏幕时才有效。

BOOL CalibrateAlgo::GetScreenCoord_WithLightSpotContour(const POINT& pCentroid,  const POINT* pContourPoints, int nContourPointCount, POINT* pScreenPts, int nMonitorId, POINT* debugPtContourCross)
{
    CComCritSecLock<CComCriticalSection> lock(*m_pcsForParam);

    if(NULL == m_pCalibrateInst)
    {
        return FALSE;
    }

    return m_pCalibrateInst->GetScreenCoord_WithLightSpotContour(
        pCentroid,
        pContourPoints,
        nContourPointCount,
        pScreenPts,
        nMonitorId,
        debugPtContourCross);

}
*/

//@功能:设置镜头规格数据
void CalibrateAlgo::SetLensSpecification(const TLensSpecification& lensSpec)
{
    if(NULL == m_pCalibrateInst)
    {
         m_pCalibrateInst = new Calibrator_GenericCameraModel();
    }
    m_pCalibrateInst->SetLensSpecification(lensSpec);
}

//@功能:设置CMOS芯片规格数据
void CalibrateAlgo::SetCMOSChipSpecification(const TCMOSChipSpecification& cmosChipSpec)
{
    if(NULL == m_pCalibrateInst)
    {
         m_pCalibrateInst = new Calibrator_GenericCameraModel();
    }
    m_pCalibrateInst->SetCMOSChipSpecification(cmosChipSpec);
}

//@功能:设置自动校正补偿数据
void CalibrateAlgo::SetAutoCalibCompCoefs(const TAutoCalibCompCoefs& autoCalibCompCoefs)
{
    if(NULL == m_pCalibrateInst)
    {
         m_pCalibrateInst = new Calibrator_GenericCameraModel();
    }
    m_pCalibrateInst->SetAutoCalibCompCoefs(autoCalibCompCoefs);
}


//@功能:设置镜头的已知的内部参数和对称畸变参数
void CalibrateAlgo::SetLensInternalAndSymmmetricDistortParams(const TInternalAndSymmetricDistortParams* pParams)
{
	if (NULL == m_pCalibrateInst)
	{
		m_pCalibrateInst = new Calibrator_GenericCameraModel();
	}
	m_pCalibrateInst->SetLensInternalAndSymmetricDistortParams(pParams);

}


//@功能:生成指定类型的校正实例
void CalibrateAlgo::GenCalibratorInst(E_CALIBRATE_MODEL eCalibrateModel)
{
    if (NULL == m_pCalibrateInst || eCalibrateModel != m_pCalibrateInst->GetCalibrateModel())
    {

        CreateCalibrateInst(eCalibrateModel);
    }
}
//=====================================================
//Part 4.屏幕区域侦测
CScreenAreaDetectorImpl* GetAreaDetectorInstance()
{
    static CScreenAreaDetectorImpl s_ScreenAreaDetectorImpl;
    return &s_ScreenAreaDetectorImpl;
}



//@功 能:开启侦测,同时显示全屏窗口
//@参 数:hNotifyWnd, 接收侦测结束通知的窗体句柄
//@返回值;	
BOOL CScreenAreaDetector::StartDetect(HWND hNotifyWnd)
{
    return GetAreaDetectorInstance()->StartDetect(hNotifyWnd);
}


//@功  能:结束侦测功能,
//@参  数:
//@返回指:
BOOL CScreenAreaDetector::End()
{
    return  GetAreaDetectorInstance()->End();
}


//@功能:返回屏幕区域屏蔽位图
const CImageFrame& CScreenAreaDetector::GetMaskFrame()const
{
    return GetAreaDetectorInstance()->GetMaskFrame();
}

CScreenAreaDetector::operator INewFrameEventSink*()
{
    return GetAreaDetectorInstance();
}


CScreenAreaDetector::operator INewFrameEventSink&()
{

    return *GetAreaDetectorInstance();
}



//=========================
//Part 5
//=========================
extern "C" ALGOLIB_API void ALGOAPI_SetIntermediateDir(const TCHAR* szIntermediateDir)
{
    g_strIntermediatRootDirName =  szIntermediateDir;
}

extern "C" ALGOLIB_API const TCHAR* ALGOAPI_GetIntermediateDir()
{
    return g_strIntermediatRootDirName;
}