#pragma once

DEFINE_GUID (CLSID_MotionDetectFilter, 0x6378695c, 0x478e, 0x4096, 0xa7, 0x62, 0xbd, 0xce, 0xfd, 0x76, 0x3d, 0x47);
struct YUVColor
{
    BYTE Y;
    BYTE U;
    BYTE V;
};
class CPenPosDetector;
class CVideoPlayer;
class CIWBSensor;
#include "../AlgoLib/AutoCalibrator/AutoBrightnessRegulator.h"

class CInterceptFilter:
    public CTransformFilter
{


public:
    //CInterceptFilter(CPenPosDetector* pPenPosDetector = NULL,CVideoPlayer* pVideoPlayer = NULL);

    CInterceptFilter(CIWBSensor* pSensor = NULL);

    ~CInterceptFilter(void);

    void AttachPenPosDetector(CPenPosDetector* pPenPosDetector);
    void AttachVideoPlayer(CVideoPlayer* pPlayer);


    //The folloing 4 virtual function must be implemented
    // check if you can support mtIn
    virtual HRESULT CheckInputType(const CMediaType* mtIn) ;

    // check if you can support the transform from this input to this output
    virtual HRESULT CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut) ;

    // this goes in the factory template table to create new instances
    // static CCOMObject * CreateInstance(LPUNKNOWN, HRESULT *);

    // call the SetProperties function with appropriate arguments
    virtual HRESULT DecideBufferSize(
        IMemAllocator * pAllocator,
        ALLOCATOR_PROPERTIES *pprop) ;

    // override to suggest OUTPUT pin media types
    virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType) ;


    virtual HRESULT Transform(IMediaSample * pIn, IMediaSample *pOut);

    virtual CBasePin * CInterceptFilter::GetPin(int n);

    BOOL StartDetect(HWND hDisplayWnd, int nSrcImageWidth, int nSrcImageHeight);
    BOOL StopDetect ();

    void ViewMonoImage (HWND hWnd);


    //设置接收通知消息的窗体
    void SetNotifyWindow(HWND hWnd, UINT uMsg);


    const CPenPosDetector* GetPenPosDetector()const;
    CPenPosDetector* GetPenPosDetector();


    const CVideoPlayer* GetVideoPlayer()const;
    CVideoPlayer* GetVideoPlayer();



    //void DrawManualScreenArea(BYTE* pYUV2Canvas);

    //@功能:使用手动指定的屏幕区域矩形屏蔽数据
    //void MaskFilterData(BYTE* pSrcData, LONG srcSize,const CYUY2Frame& mask );
    void MaskFilterData(BYTE* pSrcData, LONG srcSize,const CImageFrame& mask );


    //录像调试工具, 2012-06-20, by toxuke@gmail.com
    //@功能:判断是否正在录像
    BOOL IsRecording() const;

    //@功能:开始录像
    BOOL StartRecording(LPCTSTR lpszFilePath = NULL);

    //@功能:停止录像
    BOOL StopRecording();


    //@功能:以AVI文件代替输入源
    BOOL  InputFromAVIFile(LPCTSTR aviFilePath);

    void AddEventSink(INewFrameEventSink* pSink);

    void RemoveSink(INewFrameEventSink* pSink);

    //@功能:通知事件槽
    //void NotifySink(CYUY2Frame* pFrame);
     void NotifySink(CImageFrame* pFrame);


    //@功能:使能静态屏蔽。
//    void EnableStaticMasking(BOOL bMasking);

    //@功能:返回静态屏蔽状态
//    BOOL IsStaticMasking()const;

    //@功能:使能画面亮度自动调节
    void EnableBrightnessAutoRegulating(BOOL bEnable);

    //@功能:设置画面的平均亮度
    //@参数:brightness:亮度值
    void SetImageAverageBrightness(BYTE brightness);
	void SetAutoCalibrateParamsIndex(BYTE nIndex);

    void CaptureImage();

	void  SetFrameSkipCount(int nFrameskipcount);

	void  SetStartDrawMaskFrame(bool Start) { m_bStartDrawOnlineScreenArea= Start; }
	void  SetDrawMovePt(CPoint &point) { m_PtMove = point; }

private:
    //<<2015/8/18
    //亮度控制函数
    static bool BrightnessControl(bool bInc, BYTE absDiff, BYTE nIndex, LPVOID lpCtxData);

    //@功能:实际执行亮度控制
    void DoAutoBrightnessControl(const BYTE* YUY2Frame, int nWidth, int nHeight);
    //>>

    //@功能:更新ARGB图片帧,不使用屏蔽图
    void UpdateARGBFrameWithoutMask();

    //@功能:更新ARGB图片帧,使用屏蔽图
    void UpdateARGBFrameWithMask();

protected:
    CPenPosDetector* m_pPenPosDetector;//光斑检测对象
    CVideoPlayer*    m_pVideoPlayer   ;//视频播放对象
    CIWBSensor*      m_pSensor        ;//传感器对象

    int             m_nRawImageWidth;
    int             m_nRawImageHeight;
    //丢失帧检测对象
    CLostFrameCountDetector m_oLostFrameDetector;

    //帧率侦测器
    CFPSDetector m_oFpsDetector;


    //录像调试工具, 2012-06-20, by toxuke@gmail.com
    BOOL       m_bIsRecording;//正在录像标志
    CAviWriter m_oAviRecorder;//录像录制器


    BOOL       m_bSrcFromAVI ;//视频源来自AVI
    CAviReader m_oAviInput   ;//录像读取器



    std::map<INewFrameEventSink*, INewFrameEventSink*> m_mapEventSinks;
    CComAutoCriticalSection m_csEventSinks;

    CMJPG_Decoder m_oMJPGDecoder;//MJPG解码器


//    CYUY2Frame m_YUY2SrcFrame;
    CImageFrame m_MJPGFrame      ;
    CImageFrame m_GraySrcFrame   ;// 
    CBRGAFrame  m_BGRAFrame      ;//显示时使用的ARGB格式
    int         m_nDisplayCounter;//显示次数计数器
    DWORD       m_dwImageType    ;//图像类型

    //<<2015/8/18 显示画面的自动亮度调节，同自动校正保持一致。
    CAutoBrightnessRegulator<BYTE> m_oAutoBrightnessRegulator;

    //使能亮度自动调节标志
    BOOL m_bEnableBrightnessAutoRegulating;


    BOOL m_bCaptureImage;
	
	int     m_nFrameSkipCount;
	bool    m_bStartDrawOnlineScreenArea;
	CPoint  m_PtMove;


    BOOL m_bIsDetecting;
};