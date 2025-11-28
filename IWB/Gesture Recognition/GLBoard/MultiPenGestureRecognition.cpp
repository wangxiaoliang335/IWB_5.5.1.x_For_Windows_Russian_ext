#include "StdAfx.h"
#include <map>
#include "..\..\headers.h"

//////////////////////////////////////////////////////////////////////////
//消息号定义开始

/*模拟多笔消息号开始*/
const UINT MyWM_PEN_BASE    = 1027                               ;
////每只笔的状态消息号定义如下，其中i = 0, ..., MAXPENNUM
//const UINT MyWM_PEN_I_Down   = MyWM_PEN_BASE + 2 * i           ;
//const UINT MyWM_PEN_I_UP     = MyWM_PEN_BASE + 2 * i + 1       ;
/*模拟多笔消息号结束*/

/*模拟多笔消息号开始*/
const UINT MyWM_ERASER_BASE    = 1127                                 ;
////每只橡皮擦的状态消息号定义如下，其中i = 0, ..., MAXERASERNUM
//const UINT MyWM_ERASERA_Down      = MyWM_ERASER_BASE + 2 * i        ;
//const UINT MyWM_ERASERA_UP        = MyWM_PEN_BASE + 2 * i + 1       ;
/*模拟多笔消息号结束*/

//消息号定义结束
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//CSmartMultiPen

//////////////////////////////////////////////////////////////////////////
bool g_bHistoryEraserInfo[GLBOARDMAXERASERNUM]; //历史板擦信息，下标是对应笔id
//////////////////////////////////////////////////////////////////////////

CSmartMultiPen::CSmartMultiPen()
: m_bIsTouchPadMode(false)
 ,m_bIsIdle(true)
 ,m_hWndGLBoad(NULL)
 ,m_isSmartMatchReset(true)
{
    m_oSmartPenMatch.Reset();
}

CSmartMultiPen::~CSmartMultiPen()
{
}

void CSmartMultiPen::ResetSmartMatch()
{
    if (!m_isSmartMatchReset)
    {
        //m_oSmartPenMatch.Reset();
        DoRecognition(NULL, 0);
        m_isSmartMatchReset = true;
    }
}

void CSmartMultiPen::OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension)
{
    m_oStrokFilter.OnSetTouchScreenDimension(nPhysicalDiagonalLength, szLogicalDimension);
}

bool CSmartMultiPen::DoRecognition(POINT *points, int nPointCount)
{
    if (m_bIsIdle && nPointCount == 0) return true;    

    m_oSmartPenMatch.DoMatch(points, nPointCount);
    m_isSmartMatchReset = false;

    int nElementCount = 0;
    const TMatchInfo* pMatchInfo =  m_oSmartPenMatch.GetAllMatchInfo(&nElementCount);

    TContactInfo penInfo[GLBOARDMAXPENNUM];

    //if (m_bIsTouchPadMode)
    //{
    //    if(nElementCount > _countof(penInfo)) nElementCount = _countof(penInfo);    

    //    for(int i = 0; i < nElementCount; i++)
    //    {
    //        penInfo[i].ePenState = (pMatchInfo[i].eMatchState == E_MISMATCHED)?E_PEN_STATE_UP:E_PEN_STATE_DOWN;
    //        penInfo[i].uId       = pMatchInfo[i].uId;
    //        penInfo[i].pt        = pMatchInfo[i].ptPos;
    //    }  


    //    if (m_bIsTouchPadMode) CSpotListProcessor::GetVirtualHID().InputPoints(penInfo, nElementCount);   
    //}
    //else
    //{            
    //    for (int i = 0; i < nElementCount; i++)
    //    {  
    //        if (pMatchInfo[i].uId == 0)
    //        {//若uId = 0的笔存在，则给鼠标用，其他的用发送消息模拟  
    //            penInfo[0].ePenState = (pMatchInfo[i].eMatchState == E_MISMATCHED)?E_PEN_STATE_UP:E_PEN_STATE_DOWN;
    //            penInfo[0].uId       = pMatchInfo[i].uId;
    //            penInfo[0].pt        = pMatchInfo[i].ptPos;

    //            CSpotListProcessor::GetVirtualHID().InputPoints(penInfo, 1);
    //        }
    //        else
    //        {
    //            UINT msg = MyWM_PEN_BASE + ((pMatchInfo[i].uId - 1) << 1); //由于uId=0的为鼠标所用，则编号为uId的笔绑定的消息号为MyWM_PEN_BASE + 2*(uId - 1)
    //            msg += ((pMatchInfo[i].eMatchState == E_MISMATCHED)?1:0); //
    //            PostMessage(m_hWndGLBoad, msg, pMatchInfo[i].ptPos.x,  pMatchInfo[i].ptPos.y);
    //        }
    //    }
    //}

    if(nElementCount > _countof(penInfo)) nElementCount = _countof(penInfo);  

    for (int i = 0; i < nElementCount; i++)
    {
        penInfo[i].ePenState = (pMatchInfo[i].eMatchState == E_MISMATCHED)?E_PEN_STATE_UP:E_PEN_STATE_DOWN;
        penInfo[i].uId       = pMatchInfo[i].uId;
        penInfo[i].pt        = pMatchInfo[i].ptPos;
    }

    m_oStrokFilter.DoFilter(penInfo, nElementCount);

    for (int i = 0; i < nElementCount; i++)
    {  
        if (penInfo[i].uId == 0)
        {//若uId = 0的笔存在，则给鼠标用，其他的用发送消息模拟  
            /*penInfo[0].ePenState = (pMatchInfo[i].eMatchState == E_MISMATCHED)?E_PEN_STATE_UP:E_PEN_STATE_DOWN;
            penInfo[0].uId       = pMatchInfo[i].uId;
            penInfo[0].pt        = pMatchInfo[i].ptPos;*/

            CSpotListProcessor::GetVirtualHID().InputPoints(&(penInfo[i]), 1);
        }
        else
        {
            UINT msg = MyWM_PEN_BASE + ((penInfo[i].uId - 1) << 1); //由于uId=0的为鼠标所用，则编号为uId的笔绑定的消息号为MyWM_PEN_BASE + 2*(uId - 1)
            msg += ((penInfo[i].ePenState == E_PEN_STATE_UP)?1:0); //
            PostMessage(m_hWndGLBoad, msg, penInfo[i].pt.x,  penInfo[i].pt.y);
        }
    }
  
    if (nPointCount == 0)
    {
         m_bIsIdle = true;
    }
    else
    {
        m_bIsIdle = false;
    }

    return false;
}

//CSmartMultiPen
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//CSmartMultiEraser
CSmartMultiEraser::CSmartMultiEraser()
: m_bIsIdle(true)
,m_hWndGLBoad(NULL)
,m_isSmartMatchReset(true)
{
    m_oSmartEraserMatch.Reset();
}

CSmartMultiEraser::~CSmartMultiEraser()
{
}

void CSmartMultiEraser::ResetSmartMatch()
{
    if (!m_isSmartMatchReset)
    {
        DoRecognition(NULL, 0);
        m_isSmartMatchReset = true;
    }
}

bool CSmartMultiEraser::DoRecognition(POINT *points, int nPointCount)
{
    if (m_bIsIdle && nPointCount == 0) return true;    

    m_oSmartEraserMatch.DoMatch(points, nPointCount);

    m_isSmartMatchReset = false;

    int nElementCount = 0;
    const TMatchInfo* pMatchInfo =  m_oSmartEraserMatch.GetAllMatchInfo(&nElementCount);

    TContactInfo penInfo[GLBOARDMAXPENNUM];

    if(nElementCount > _countof(penInfo)) nElementCount = _countof(penInfo);  

    for (int i = 0; i < nElementCount; i++)
    {
        penInfo[i].ePenState = (pMatchInfo[i].eMatchState == E_MISMATCHED)?E_PEN_STATE_UP:E_PEN_STATE_DOWN;
        penInfo[i].uId       = pMatchInfo[i].uId;
        penInfo[i].pt        = pMatchInfo[i].ptPos;
    }

    m_oStrokFilter.DoFilter(penInfo, nElementCount);

    for (int i = 0; i < nElementCount; i++)
    {  
        UINT msg = MyWM_ERASER_BASE + (penInfo[i].uId  << 1); //编号为uId的橡皮擦绑定的消息号为MyWM_ERASER_BASE + 2*uId
        msg += ((penInfo[i].ePenState == E_PEN_STATE_UP)?1:0); //
        PostMessage(m_hWndGLBoad, msg, penInfo[i].pt.x,  penInfo[i].pt.y);
    }

    if (nPointCount == 0)
    {
        m_bIsIdle = true;
    }
    else
    {
        m_bIsIdle = false;
    }

    return false;
}
//CSmartMultiEraser
//////////////////////////////////////////////////////////////////////////


CMultiPenGestureRecognition::CMultiPenGestureRecognition(void)
: m_nActualSplitPenCount(0)
 ,m_nActualSplitEraserCount(0)
 ,m_isEnableGR(true)
 ,m_nHistoryErasers(0)
{
}

CMultiPenGestureRecognition::~CMultiPenGestureRecognition(void)
{
}

void CMultiPenGestureRecognition::ResetSmartMatch()
{
    m_multiPen.ResetSmartMatch();
    m_multiEraser.ResetSmartMatch();

    //m_lastEraserMap.clear();
}

void CMultiPenGestureRecognition::OnSetTouchScreenDimension(int  nPhysicalDiagonalLength, SIZE szLogicalDimension)
{
    m_multiPen.OnSetTouchScreenDimension(nPhysicalDiagonalLength, szLogicalDimension);
}

bool CMultiPenGestureRecognition::SplitLightSpotByMass(const TLightSpot * aryLightSpots,  const int &nCount, const int &nMultiEraser)
{
    //为测试，先把所有的光斑看成笔
    /*int actualCount = nCount > GLBOARDMAXPENNUM ? GLBOARDMAXPENNUM : nCount;
    for (int i = 0; i < nCount; i++)
    {
        m_ptSplitPens[i] = aryLightSpots[i].ptPosInScreen;
    }

    m_nActualSplitPenCount = actualCount;*/
    const int MAXALTERASERSIZE = 128;
    const int MAXLIGHTSPOTSPERMITTED = 128;
    
    int thresh_120 = g_oWGRConfig.MapStandardX2ActualValue(120);

    int MERGE_THRESHOLD = thresh_120 * thresh_120;//(m_nMergeAreaWidth*m_nMergeAreaWidth)/8;

    int posAlterEraser[MAXALTERASERSIZE] = {-1};                //存储扫描得到的候选橡皮擦光斑的下标
    //POINT ptAlterEraser[MAXALTERASERSIZE];
    //bool flagAlterEraser[MAXLIGHTSPOTSPERMITTED] = {false};     //标志aryLightSpots对应位置是否被候选为橡皮擦
    bool flagMerged[MAXLIGHTSPOTSPERMITTED] = {false};          //标志aryLightSpots对应位置是否已经被合并了
    int nActualAlterErasers = 0;

    int nSpotCount = nCount > MAXLIGHTSPOTSPERMITTED ? MAXLIGHTSPOTSPERMITTED : nCount;    

    if(m_isEnableGR)
    {        
        AtlTrace(_T("Before Merge: nSpotCount = %d \r\n"), nSpotCount);

        //扫描aryLightSpots，获取候选橡皮擦    
        for (int i = 0; i < nSpotCount; i++)
        {
            if ((aryLightSpots[i].lStdSpotAreaInVideo) * nMultiEraser < aryLightSpots[i].mass)
            {//满足该条件，将被作为候选橡皮擦
                posAlterEraser[nActualAlterErasers] = i;
                //flagAlterEraser[nActualAlterErasers] = true;
                nActualAlterErasers++;
            }
            //else
            //{
            //    //由于光斑是按mass从大到小排列的，如果当前mass不满足要求，
            //    break;
            //}
        }
        //
        AtlTrace(_T("Before Merge: AlterEraserCount = %d \r\n"), nActualAlterErasers);

        //合并候选橡皮擦MERGE_THRESHOLD周围的光斑
        int j = 0;
        m_nActualSplitEraserCount = 0;
        while(j < nActualAlterErasers)
        {
            if (flagMerged[posAlterEraser[j]] == true)
            {//已经被合并
                j++;
                continue;
            }

            int nToBeMerged = posAlterEraser[j];
            int nMass = aryLightSpots[nToBeMerged].mass;
            POINT pt = aryLightSpots[nToBeMerged].ptPosInScreen;

            flagMerged[nToBeMerged] = true;   //标志nToBeMerged位置光斑已经被合并

            for (int k = 0; k < nSpotCount; k++)
            {
                if (flagMerged[k] == true)
                {//已经被合并
                    continue;
                }

                int dx = pt.x - aryLightSpots[k].ptPosInScreen.x;
                int dy = pt.y - aryLightSpots[k].ptPosInScreen.y;

                int R2 = dx*dx + dy*dy;//两个光斑之间的距离

                if (R2 < MERGE_THRESHOLD)
                {
                    //光斑够近，需要合并
                    pt.x = (pt.x * nMass + aryLightSpots[k].ptPosInScreen.x * aryLightSpots[k].mass) / (nMass + aryLightSpots[k].mass);
                    pt.y = (pt.y * nMass + aryLightSpots[k].ptPosInScreen.y * aryLightSpots[k].mass) / (nMass + aryLightSpots[k].mass);

                    nMass += aryLightSpots[k].mass;

                    flagMerged[k] = true;  //标志k位置光斑已经被合并
                }
            }

            if (m_nActualSplitEraserCount < GLBOARDMAXERASERNUM)
            {
                m_ptSplitErasers[m_nActualSplitEraserCount] = pt;
                m_nActualSplitEraserCount++;
            }

            j++;
        }
    }  
    else
    {
        //不允许手势识别，则清除历史和现在的板擦信息
        m_nHistoryErasers = 0;
        m_nActualSplitEraserCount = 0;
        m_multiEraser.ResetSmartMatch();
    }

    //
    if (m_nActualSplitEraserCount >= GLBOARDMAXERASERNUM)
    {//新的板擦个数不少于允许的最大板擦个数，则把更新历史板擦信息为最大的板擦数，供下次使用
        m_nHistoryErasers = GLBOARDMAXERASERNUM;        
    }
    else if(nSpotCount > 0)
    {
        //首先用新板擦匹配历史板擦，然后用未匹配的历史板擦合并剩余的笔信息       
        bool flagHisMatched[GLBOARDMAXERASERNUM] = {false};

        int i = 0;
        while(i < m_nActualSplitEraserCount)
        {
            POINT &refCurEraser = m_ptSplitErasers[i];
            for(int j = 0; j < m_nHistoryErasers; j++)
            {
                if (flagHisMatched[j] == false)
                {
                    POINT &refHisEraser = m_ptHistoryEraser[j];

                    int dx = refCurEraser.x - refHisEraser.x;
                    int dy = refCurEraser.y - refHisEraser.y;

                    int R2 = dx*dx + dy*dy;//两个光斑之间的距离

                    if (R2 < MERGE_THRESHOLD)
                    {
                        flagHisMatched[j] = true;
                    }
                }
            }

            i++;
        }

        //用未合并的前nCanAddedErasers个历史板擦合并新的笔点
        i = 0;        
        while(i < m_nHistoryErasers)
        {
            if (flagHisMatched[i]) {i++; continue;} //该历史板擦已经被匹配到新板擦中            

            POINT &refHisEraser = m_ptHistoryEraser[i];
            
            bool toBeMerged = false;
            for (int j = 0; j < nSpotCount; j++)
            {
                if (flagMerged[j] == false)
                {
                    //该光斑未被合并到新的板擦中，尝试与（未与新版擦匹配的）历史板擦中
                    const POINT &refSpot = aryLightSpots[j].ptPosInScreen;

                    int dx = refSpot.x - refHisEraser.x;
                    int dy = refSpot.y - refHisEraser.y;

                    int R2 = dx*dx + dy*dy;//两个光斑之间的距离

                    if (R2 < MERGE_THRESHOLD)
                    {
                        flagMerged[j] = true;                        
                        refHisEraser = refSpot;
                        toBeMerged = true;
                    }
                }
            }            

            if(toBeMerged)
            {                
                m_ptSplitErasers[m_nActualSplitEraserCount] = refHisEraser;

                m_nActualSplitEraserCount++;
            }            

            i++;

            if (m_nActualSplitEraserCount >= GLBOARDMAXERASERNUM) break;            
        }
    }

    if (m_nActualSplitEraserCount > 0) memcpy(m_ptHistoryEraser, m_ptSplitErasers, sizeof(POINT) * m_nActualSplitEraserCount); //更新历史板擦信息
    m_nHistoryErasers = m_nActualSplitEraserCount;

    //把未被合并成橡皮擦的光斑存放如笔点数组
    int k = 0;
    m_nActualSplitPenCount = 0;
    while(k < nSpotCount)
    {
        if (flagMerged[k] == false)
        {
            if (m_nActualSplitPenCount < GLBOARDMAXPENNUM)
            {
                m_ptSplitPens[m_nActualSplitPenCount] = aryLightSpots[k].ptPosInScreen;
                m_nActualSplitPenCount++;
            }
        }

        k++;
    }

    AtlTrace(_T("After Merge: PensCount = %d \r\n"), m_nActualSplitPenCount);
    AtlTrace(_T("After Merge: EraserCount = %d \r\n"), m_nActualSplitEraserCount);

    return true;
}

bool CMultiPenGestureRecognition::DoGestureRecognition(const TLightSpot * aryLightSpots,  const int &nCount, const int &nMultiEraser)
{
    if (m_multiEraser.IsIdle() && m_multiPen.IsIdle() && nCount == 0) return true;

    bool isEraserResponse = true;
    bool isPenResponse = true;
    
    SplitLightSpotByMass(aryLightSpots, nCount, nMultiEraser);
    
    isPenResponse = m_multiPen.DoRecognition(m_ptSplitPens, m_nActualSplitPenCount);    
    isEraserResponse = m_multiEraser.DoRecognition(m_ptSplitErasers, m_nActualSplitEraserCount);

    return false;
}
