#pragma once
//@功能:基点标定器
//@说明:标定基点时，由于没有屏蔽图,为了减少外部干扰，要求通过光光笔的双击
//      来完成每个基点的标定。
class CBaseStoneMarker
{
public:
    CBaseStoneMarker();
    ~CBaseStoneMarker();

    void Reset();

    //@功能:校正基點
    //@返回值:全部基點标定完毕，则返回TRUE, 否则返回FALSE
    BOOL Process(const TPoint2D* pLightSpots, int nlightSpotsCount);

    const TPoint2D* GetBasePoints(UINT* pCount = NULL)const;
    
    void LoadBasePoints(const TPoint2D* pBasePoints, UINT nCount);

    //@功能:获得当前基點的索引号
    int GetCurrentBaseStoneIndex() const;


    BOOL IsDataValid()const;

    enum EMachinState
    {
        E_MACHINE_STATE_READY,//就绪阶段
        E_MACHINE_STATE_RUNNING,//运行阶段
        E_MACHINE_STATE_END,  //结束阶段

    };

    EMachinState GetMachineState() const
    {
        return m_eMachineState;
    }

    

    static const int BASE_STONE_NUMBER = 4;

protected:

    //标定笔状态
    enum ELightSpotState
    {
        E_MARK_STATE_READY          ,//就绪状态
        E_MARK_STATE_PEN_FIRST_DOWN ,//光笔第一次按下状态
        E_MARK_STATE_PEN_FIRST_UP   ,//光笔第一次弹起状态  
        E_MARK_STATE_PEN_SECOND_DOWN,//光笔第一次按下状态
        E_MARK_STATE_PEN_SECOND_UP  ,//光笔第二次弹起状态
        E_MARK_STATE_TIMEOUT        ,//标定超时
    };


    struct MarkLightspot//标定光斑
    {
        ELightSpotState m_eState         ;//标定状态
        TPoint2D        m_pt2dPos        ;//标定笔在上一时刻的位置。
        DWORD           m_dwLastTickcount;//光笔在上一时刻的时钟计数,单位:ms
        BOOL            m_bProcessed     ;//已处理标志
        //BOOL       m_bValid            ;//数据有效标志
    };
    
    BOOL InternalProcess(const TPoint2D* pNewLightSpots, int nlightSpotsCount);

    void ProcessLightSpot(MarkLightspot* lightSpot, const TPoint2D* pPtNewPos);
    
    

    int m_nCurrentMarkIndex;//当前标定的基点的索引号。

    TPoint2D m_BaseStones[BASE_STONE_NUMBER];
    BOOL m_bDataIsValid;
    

    static const int MAX_LIGHTSPOT = 8;
    MarkLightspot m_MarkLightSpots[MAX_LIGHTSPOT];//光笔数组
    int m_nActiveLightSpotCount;//活动的光斑个数

    EMachinState m_eMachineState;
};