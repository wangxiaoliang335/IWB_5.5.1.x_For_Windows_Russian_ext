
#include "stdafx.h"
#include "BaseStoneMarker.h"
CBaseStoneMarker::CBaseStoneMarker()
    :
    m_nCurrentMarkIndex(0),
    m_nActiveLightSpotCount(0),
    m_bDataIsValid(FALSE),
    m_eMachineState(E_MACHINE_STATE_END)
{

}


CBaseStoneMarker::~CBaseStoneMarker()
{


}


void CBaseStoneMarker::Reset()
{
    m_nCurrentMarkIndex = 0;

    for (int i = 0; i < MAX_LIGHTSPOT; i++)
    {
        m_MarkLightSpots[i].m_eState = E_MARK_STATE_READY;
        // m_MarkLightSpots[i].m_bValid     = FALSE;
        m_MarkLightSpots[i].m_bProcessed = FALSE;

    }//for

    m_eMachineState = E_MACHINE_STATE_READY;
    m_bDataIsValid = FALSE;
    m_nActiveLightSpotCount = 0;

}
//@功能:获得当前基點的索引号
int CBaseStoneMarker::GetCurrentBaseStoneIndex() const
{
    return m_nCurrentMarkIndex;
}

const TPoint2D* CBaseStoneMarker::GetBasePoints(UINT* pCount)const
{
    if (pCount)
    {
        *pCount = _countof(m_BaseStones);
    }
    return &m_BaseStones[0];
}

void CBaseStoneMarker::LoadBasePoints(const TPoint2D* pBasePoints, UINT nCount)
{
    if (nCount > _countof(m_BaseStones))
    {
        nCount = _countof(m_BaseStones);
    }

    for (UINT i = 0; i < nCount; i++)
    {
        m_BaseStones[i] = pBasePoints[i];
    }

    m_bDataIsValid = TRUE;
}

BOOL CBaseStoneMarker::IsDataValid()const
{
    return m_bDataIsValid;
}

//@返回值:全部基點标定完毕，则返回TRUE, 否则返回FALSE
BOOL CBaseStoneMarker::Process(const TPoint2D* pNewLightSpots, int nlightSpotsCount)
{
    BOOL bRet = FALSE;
    switch (m_eMachineState)
    {
        case  E_MACHINE_STATE_READY://就绪阶段
            InternalProcess(pNewLightSpots, nlightSpotsCount);
            this->m_eMachineState = E_MACHINE_STATE_RUNNING;
            break;
            
        case E_MACHINE_STATE_RUNNING://运行阶段
            if (InternalProcess(pNewLightSpots, nlightSpotsCount))
            {
                this->m_eMachineState = E_MACHINE_STATE_END;
                bRet = TRUE;
            }
           
        break;

        case E_MACHINE_STATE_END:  //结束阶段

        break;

    }//switch

    return bRet;
}



//@返回值:全部基點标定完毕，则返回TRUE, 否则返回FALSE
BOOL CBaseStoneMarker::InternalProcess(const TPoint2D* pNewLightSpots, int nlightSpotsCount)
{
    if (nlightSpotsCount == 0 && m_nActiveLightSpotCount == 0) return FALSE;

    //复位活动光斑中的已处理标志
    for (int i = 0; i < m_nActiveLightSpotCount; i++)
    {
        m_MarkLightSpots[i].m_bProcessed = FALSE;
    }

    BOOL bLightSpotMatched[MAX_LIGHTSPOT];

    if (nlightSpotsCount > MAX_LIGHTSPOT)
    {
        nlightSpotsCount = MAX_LIGHTSPOT;
    }

    for (int i = 0; i < nlightSpotsCount; i++)
    {
        bLightSpotMatched[i] = FALSE;
    }


    //判断是否在已标定点附近
    double MINIMUM_DISTANCE = 100 * 100;
    for (int i = 0; i < nlightSpotsCount; i++)
    {
        const TPoint2D& ptNewPt = pNewLightSpots[i];

        for (int j = 0; j < m_nCurrentMarkIndex; j++)
        {
            const TPoint2D& ptBasePt = m_BaseStones[j];

            double distanceX = ptNewPt.d[0] - ptBasePt.d[0];
            double distanceY = ptNewPt.d[1] - ptBasePt.d[1];
            double distance = distanceX*distanceX + distanceY*distanceY;
            if (distance < MINIMUM_DISTANCE)
            {
                bLightSpotMatched[i] = TRUE;
                break;
            }

        }//for

    }//for



    //首先,匹配到已有的活动光斑
    double NEIGHBOURHOOD_RANGE = 3 * 3;
    for (int i = 0; i < nlightSpotsCount; i++)
    {
        if (bLightSpotMatched[i]) continue;

        const TPoint2D& ptNewPt = pNewLightSpots[i];

        for (int j = 0; j < m_nActiveLightSpotCount; j++)
        {
            if (m_MarkLightSpots[j].m_bProcessed) continue;

            const TPoint2D& ptOldPt = m_MarkLightSpots[i].m_pt2dPos;

            double distanceX = ptNewPt.d[0] - ptOldPt.d[0];
            double distanceY = ptNewPt.d[1] - ptOldPt.d[1];
            double distance = distanceX*distanceX + distanceY*distanceY;
            if (distance < NEIGHBOURHOOD_RANGE)
            {
                ProcessLightSpot(&m_MarkLightSpots[j], &ptNewPt);

                //置位光斑已处理标志
                bLightSpotMatched[i] = TRUE;

                break;

            }//if

        }//for-each(j)

    }//for-each(i)



    //其次,未匹配的输入光斑，作为新的活动光斑加入“活动光斑数组”
    for (int i = 0; i < nlightSpotsCount; i++)
    {
        if (bLightSpotMatched[i]) continue;

        if (m_nActiveLightSpotCount < MAX_LIGHTSPOT)
        {
            MarkLightspot* pNewMark = &m_MarkLightSpots[m_nActiveLightSpotCount];
            pNewMark->m_eState = E_MARK_STATE_READY;
            ProcessLightSpot(pNewMark, &pNewLightSpots[i]);
            m_nActiveLightSpotCount++;
        }
    }


    //接下来, 未被处理过的活动光斑,意味着光笔抬起。
    for (int i = 0; i < m_nActiveLightSpotCount; i++)
    {
        if (!m_MarkLightSpots[i].m_bProcessed)
        {
            ProcessLightSpot(&m_MarkLightSpots[i], NULL);
        }
    }



    //最后，光笔二次弹起状态的活动光斑作为标定完毕的基點，从“活动光斑数组”中剔除，并进行下一个基點的标定。
    //如果全部标定完毕，则返回TRUE;
    double max_sum_dist_to_buddies = 0;
    
    BOOL bRemove = FALSE;
    for (int i = 0; i < m_nActiveLightSpotCount; i++)
    {
        if (m_MarkLightSpots[i].m_eState == E_MARK_STATE_PEN_SECOND_UP)
        {
            BOOL bIsCandidateFound = TRUE;
            //可能有多个光斑处于二次弹起状态，选取距离其他已知基點距离和最大的坐标点作为当前基點坐标
            if (m_nCurrentMarkIndex != 0)
            {
                double sum_dist_to_buddies = 0.0;
                for (int j = 0; j < m_nCurrentMarkIndex; j++)
                {
                    double distanceX = m_MarkLightSpots[i].m_pt2dPos.d[0] - m_BaseStones[j].d[0];
                    double distanceY = m_MarkLightSpots[i].m_pt2dPos.d[1] - m_BaseStones[j].d[1];

                    double dist_to_buddy = distanceX*distanceX + distanceY*distanceY;

                    if(dist_to_buddy < MINIMUM_DISTANCE)
                    {
                        bIsCandidateFound = FALSE;
                        break;

                    }

                    sum_dist_to_buddies += distanceX*distanceX + distanceY*distanceY;

                    if (sum_dist_to_buddies > max_sum_dist_to_buddies)
                    {
                        max_sum_dist_to_buddies = sum_dist_to_buddies;
                        m_BaseStones[m_nCurrentMarkIndex] = m_MarkLightSpots[i].m_pt2dPos;                      

                    }//if

                }//for

            }//if
            else
            {
                m_BaseStones[m_nCurrentMarkIndex] = m_MarkLightSpots[i].m_pt2dPos;               
            }

            if (bIsCandidateFound)
            {
                m_nCurrentMarkIndex++;
            }

            
            bRemove = TRUE;
        }
        else if (m_MarkLightSpots[i].m_eState == E_MARK_STATE_TIMEOUT)
        {
            bRemove = TRUE;
        }

        if (bRemove)
        {
            //从“活动光斑数组”m_MarkLightSpots中移除
            for (int j = i; j < m_nActiveLightSpotCount - 1; j++)
            {
                m_MarkLightSpots[j] = m_MarkLightSpots[j + 1];
            }

            m_nActiveLightSpotCount--;
        }

    }//for-each-lightspot

    //基點标定完毕
    if (m_nCurrentMarkIndex == BASE_STONE_NUMBER)
    {
        m_bDataIsValid = TRUE;
        return TRUE;
    }

    return FALSE;

}


void CBaseStoneMarker::ProcessLightSpot(MarkLightspot* pLightSpot, const TPoint2D* pPtNewPos)
{
    DWORD TIMEOUT = 500;//500秒钟超时
    switch (pLightSpot->m_eState)
    {
    case E_MARK_STATE_READY:
        if (pPtNewPos)
        {
            pLightSpot->m_pt2dPos = *pPtNewPos;
            pLightSpot->m_eState = E_MARK_STATE_PEN_FIRST_DOWN;
            pLightSpot->m_dwLastTickcount = GetTickCount();
        }
        break;

    case E_MARK_STATE_PEN_FIRST_DOWN:
        if (NULL == pPtNewPos)
        {
            pLightSpot->m_dwLastTickcount = GetTickCount();
            pLightSpot->m_eState = E_MARK_STATE_PEN_FIRST_UP;
        }
        else
        {
            DWORD dwNow = GetTickCount();
            DWORD dwElapse = dwNow - pLightSpot->m_dwLastTickcount;
            if (dwElapse > TIMEOUT)
            {
                pLightSpot->m_eState = E_MARK_STATE_TIMEOUT;
            }

        }

        break;

    case E_MARK_STATE_PEN_FIRST_UP:
        if (pPtNewPos)
        {
            pLightSpot->m_dwLastTickcount = GetTickCount();
            pLightSpot->m_eState = E_MARK_STATE_PEN_SECOND_DOWN;
        }
        else
        {
            DWORD dwNow = GetTickCount();
            DWORD dwElapse = dwNow - pLightSpot->m_dwLastTickcount;
            if (dwElapse > TIMEOUT)
            {
                pLightSpot->m_eState = E_MARK_STATE_TIMEOUT;
            }

        }
        break;

    case E_MARK_STATE_PEN_SECOND_DOWN:
        if (NULL == pPtNewPos)
        {
            pLightSpot->m_eState = E_MARK_STATE_PEN_SECOND_UP;
        }
        break;

    }//switch

    //置位已处理标志
    pLightSpot->m_bProcessed = TRUE;
}
