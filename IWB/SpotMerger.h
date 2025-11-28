#pragma once
//@功能:融合区的光斑合并类
class CSpotMerger
{
public:
     CSpotMerger();
    ~CSpotMerger();

    //@功能:执行屏幕融合区光斑合并操作
    //@参数:pLightSpots，输入/输出参数, 光斑数组
    //      pLightSpotCount, 输入/输出参数，指向光斑个数的指针
    //@说明:当光斑分别在屏幕分割区域2侧
    void DoMerge(TLightSpot* pLightSpots, int* pLightSpotCount);


    //@功能:设置融合区域的信息
    //@参数:nSeperateX, 两屏幕拼接边的X坐标
    //      nMergeAreaWidth, 屏幕融合区的宽度
    void SetMergeAreaInfo(int nSeperateX, int nMergeAreaWidth);

    int GetMergeAreaRightBorder()const {return m_nMergeAreaRightBorder;}
    int GetMergeAreaLeftBorder()const  {return m_nMergeAreaLeftBorder ;}
    int GetMergeDistThreshold() const  {return m_nMergeDistThreshold;  }

    
    //@功能:屏幕分辨率变化事件响应函数
    //@参数:nScreenWidth, 新的屏幕宽度
    //      nScreenHeight,新的屏幕高度
    void OnDisplayChange(int nScreenWidth, int nScreenHeight);

    
    
    //@功能:设置融合区域
    //@参数:mergeAreas, 融合区数组
    //      nCount, 融合区数目
    void SetMergeAreas(const RECT  mergeAreas[], int nAreaCount);


    //@功能:返回指定的融合区
    const RECT* GetMergeArea(UINT nIndex) const;
protected:
    //int m_nLeftBorder ;//屏幕融合区域的左边界
    //int m_nRightBorder;//屏幕融合区域的右边界
    //static const int MERGE_THRESHOLD = 20*20;
    int m_nSeperateX             ;//两屏幕拼接边的X坐标
    int m_nMergeAreaLeftBorder   ;//融合区左边界
    int m_nMergeAreaRightBorder  ;//融合区右边界
    int m_nMergeDistThreshold    ;//融合距离门限，小于该值即融合



    std::vector<RECT> m_vecMergeAreas;//触控合并区



};