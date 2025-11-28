#pragma once
//自动校正接口
class IAutoCalibratorInterface
{
public:
    //@功能:开始校正, 供外部模块调用
    //@参数:
    //      hNotifyWnd, 消息通知窗体句柄
    //      E_AutoCalibratePattern, 校正方案枚举值
    //      cBrightness, 校正时画面亮度平均值
    //      ChangeCameraParamsProc, 调节摄像头参数的回调函数
    //      lpCtx, ChangeCameraParamsProc的上下文参数
    //      clrGridHighlight, 校正图案中高亮块的显示颜色
    //      bSaveInermediatFile, 是否保存校正的中间结果图片
    //      bRecordVideo，是否记录校正过程中的原始视频
    //   
    //virtual BOOL StartCalibrating(HWND hNotifyWnd, E_AutoCalibratePattern eGridPattern=E_CALIBRATE_PATTERN_11_COLS, BYTE cBrightness = 50, CHANGECAMERAPARAMSPROC ChangeCameraParamsProc = NULL, LPVOID lpCtx = NULL, COLORREF clrGridHighlight=RGB(255,255,255),BOOL bSaveInermediatFile = FALSE ,BOOL bRecordVideo = FALSE)=0;


    //@功能:开始校正, 供外部模块调用
    //@参数:autoCalibrateParams, 自动校正参数
    //      staticMaskingParams, 静态屏蔽参数
     virtual BOOL StartCalibrating(const TAutoCalibrateParams& autoCalibrateParams, const TStaticMaskingParams& staticMaskingParams) = 0;


     //@功能:开始自动屏蔽,供外部模块调用
     virtual BOOL StartMasking(const TAutoMaskingParams& autoMaskingParams, const TStaticMaskingParams& staticMaskingParams, int nSrcImageWidth, int nSrcImageHeight) = 0;



    //@功能:结束校正
    virtual void Finish() = 0;

    
    //@功能:给自动校正器投喂图片，并处理之
    //@返回值:异常返回FALSE
    virtual BOOL FeedImage(const CImageFrame* pGrayFrame) = 0;


     //@功能:返回屏幕个数
    virtual int GetMonitorCount()const= 0;

	 //@功能:获取指定屏幕的校正数据
	 //@参数:nMonitorID， 显示器Id
	 //@说明:
	 //virtual  TMonitorCalibData GetMonitorCalibData(int nMonitorID) const = 0;


     //@功能:获取所有屏幕的校正数据
     virtual  const TCalibData* GetCalibrateData() const = 0;


    virtual const CImageFrame& GetMaskFrame() const = 0;
    
    virtual const CImageFrame& GetScreenMap() const = 0;

    //@功能:返回校正窗体句柄
    virtual HWND GetCalibrateHWnd()const=0;

    //@功能:设置调试级别
    virtual void SetDebugLevel(ECalibDebugLevel eDebugLevel) = 0;

    //@功能:设置屏幕尺寸
    virtual SIZE GetScreenSize()const = 0;
    

   //@功能:设置屏幕尺寸
    virtual SIZE GetImageSize()const = 0;

    
    //@功能：返回自动校正后失败的中间结果图片路径
    virtual  CAtlString GetCalibrateIntermediataDirName() = 0 ;


    //========调试函数========
    //@功能:通过输入AVI录像，进行模拟校正
    //
    virtual BOOL DoSimulateCalibrate(LPCTSTR lpszAVIFilePath, HWND hNotifyWnd, UINT uCx, UINT uCy, BYTE cBrightness, E_AutoCalibratePattern eGridsNumber)
    {
        return FALSE;
    }

    //@功能:查看校正图案
    //
    virtual void ViewCalibratePattern()
    {

    }

    //@功能:测试自动亮度调节
    virtual void TestAutoBrightnessControl(CHANGECAMERAPARAMSPROC ChangeCameraParamsProc = NULL, LPVOID lpCtx = NULL)
    {

    }

    //@功能:返回调试用录像文件完整路径
    virtual const TCHAR* GetDebugVideoFilePath()const = 0;
};