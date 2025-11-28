#pragma once

#ifdef WINGESTURERECOGNITION_EXPORTS
#define EASI_WGR_CLASS __declspec(dllexport)
#else
#define EASI_WGR_CLASS __declspec(dllimport)
#endif

#include "../inc/wgr/IWGRUtilContext.h"
#include "../inc/LightSpot.h"

class EASI_WGR_CLASS CWindowsGestureRecognition
{
public:
    CWindowsGestureRecognition(void);
    ~CWindowsGestureRecognition(void);

public:
    //@功能：设置光斑比例的函数
    void  SetSpotProportion(int nSpot)
    {
        m_nSpotProportion = nSpot;
    }

    //@功能：得到板擦倍数的函数
    void  SetMultiEraser(int nEraser)
    {
        m_nMultiEraser = nEraser;
    }

    //@功能：开启或关闭日志功能
    void SetLogPermitted(bool bPermit)
    {
        m_bLogPermited = bPermit;        
    }

public:
    //开启识别器
    bool Start();

    //停止识别器
    bool Stop();

    //关联功能的执行者
    void AttachExceContext(IWGRUtilContext *pContext);

    //切断功能的执行者
    void DetachExceContext();

    //bool DeterminationShape(const TLightSpot * aryLightSpots,  int nCount);  

    //判定是否进入Windows Gesture Recognition
    bool DetermineWhethertoEnterWGR(const TLightSpot * aryLightSpots,  int nCount);

    ////执行识别，在调用DoRecongition之前，切记调用AttachExceContext，是识别后能执行相关的功能
    //bool DoRecongition(const TLightSpot * aryLightSpots,  int nCount);

    //执行识别，在调用DoRecongition之前，切记调用AttachExceContext，是识别后能执行相关的功能
    bool DoRecongition();

    /*
        @功能：添加匹配后的触点
        @参数：uId，触点的id
               ptPos,触点的位置
               matchState，触点的匹配状态——0，代表匹配失败；1，匹配成功；2，新加入的点。
    */
    void AddTouchAfterMatch(const UINT &uId, const POINT &ptPos, int matchState);

private:    
    IWGRUtilContext             *m_pContextImpl        ;//手势识别功能执行者       
    int                         m_nMultiEraser       ;//光斑被认为是板擦时, 光斑的面积与面积门限的最低之比
    int                         m_nSpotProportion    ; //光斑响应时的最小比例。

    bool                        m_bLogPermited       ; //允许开启日志

    WGR_STATE_TYPE              m_curState           ;//当前状态  

    //EWGR_GESTURE_SHAPE          m_curGestureShape    ;//当前形状
};
