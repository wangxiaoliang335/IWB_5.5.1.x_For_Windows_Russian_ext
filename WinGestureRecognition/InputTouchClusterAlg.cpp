#include "StdAfx.h"
#include "InputTouchClusterAlg.h"

const int KXMAXDISLIMIT = 70;
const int KYMAXDISLIMIT = 70;

ST_RAWTOUCHINFO CInputTouchClusterAlg::m_rawTouchSet[KMAXTOUCHNUM];        //当前采集的触点集合
int             CInputTouchClusterAlg::m_nTouches = 0;                      //当前采集的触点个数
int             CInputTouchClusterAlg::m_nClusterCount[KMAXTOUCHNUM];   //m_nClusterCount[i]表示以触点i为中心聚类，聚集的触点个数（包括它自身）
int             CInputTouchClusterAlg::m_nClusterPos[KMAXTOUCHNUM][KMAXTOUCHNUM];
bool			CInputTouchClusterAlg::m_bHadReset = false;
int				CInputTouchClusterAlg::m_nXMaxDisLimit = KXMAXDISLIMIT;
int				CInputTouchClusterAlg::m_nYMaxDisLimit = KYMAXDISLIMIT;

ST_TouchInfo    CInputTouchClusterAlg::m_validTouchsAdded[KMAXTOUCHNUM];
int             CInputTouchClusterAlg::m_nValidTouchsAdded = 0;
ST_TouchInfo    CInputTouchClusterAlg::m_validTouchsMatched[KMAXTOUCHNUM];
int             CInputTouchClusterAlg::m_nValidTouchsMatched = 0;


CInputTouchClusterAlg::CInputTouchClusterAlg(void)
{
}

CInputTouchClusterAlg::~CInputTouchClusterAlg(void)
{
}

void CInputTouchClusterAlg::AddTouch(const UINT &uId, const POINT &ptPos, int matchState)
{
	if(matchState == 0 || m_nTouches >= KMAXTOUCHNUM - 1) return;

    ST_TouchInfo &refCurInfo = m_rawTouchSet[m_nTouches].rawInfo;
    m_rawTouchSet[m_nTouches].state = matchState;
	refCurInfo.uId = uId;
	refCurInfo.stMotionInfo.ptPos = ptPos;
	refCurInfo.stMotionInfo.nXDisplacement = 0;
	refCurInfo.stMotionInfo.nYDisplacement = 0;

	m_nClusterCount[m_nTouches] = 0;
	/*m_nClusterPos[m_nTouches][0] = m_nTouches;*/

	for (int i = 0; i < m_nTouches; i++)
	{
		POINT &refEverPtPos = m_rawTouchSet[i].rawInfo.stMotionInfo.ptPos;
		if (WGR_INT32_ABS(refEverPtPos.x - ptPos.x) < m_nXMaxDisLimit && WGR_INT32_ABS(refEverPtPos.y - ptPos.y) < m_nYMaxDisLimit)
		{//该两点符合聚类条件
			int &refEverClusterCount = m_nClusterCount[i];
			m_nClusterPos[i][refEverClusterCount] = m_nTouches;
			refEverClusterCount++;

			int &refCurClusterCount = m_nClusterCount[m_nTouches];
			m_nClusterPos[m_nTouches][refCurClusterCount] = i;
			refCurClusterCount++;
		}
	}

	m_nTouches++;

	m_bHadReset = false;
}

void CInputTouchClusterAlg::AddValidTouch(int nPos)
{
    if (m_rawTouchSet[nPos].state == 1)
    {
        m_validTouchsMatched[m_nValidTouchsMatched] = m_rawTouchSet[nPos].rawInfo;
        m_nValidTouchsMatched++;
    }
    else
    {
        m_validTouchsAdded[m_nValidTouchsAdded] = m_rawTouchSet[nPos].rawInfo;
        m_nValidTouchsAdded++;
    }
}

void CInputTouchClusterAlg::GetValidInputAfterCluster()
{
    if (m_nTouches <= 0) return;

    bool hadVerified[KMAXTOUCHNUM];
    bool hadAdded2List[KMAXTOUCHNUM];
    memset(hadVerified, 0, _countof(hadVerified));
    memset(hadAdded2List, 0, _countof(hadAdded2List));

    /*
    @聚类的方式，寻找伪最优值——
        1.根据求出的m_nClusterCount，寻找一个最大的m_nClusterCount[i];
        2.然后以i触点为出发点，广度遍历获取符合以i触点为中心的触点k,其中触点i和触点k的关系如下
            a.i != k
            b.若k不在m_nClusterPos[i]中，则触点i到触点k存在路径i,p1,p2,...,k，且这条路径上任意相邻的点pm、pn组成的路径长度符合聚类条件
        3.重复步骤2，直到遍历终止
    其中终止条件为——
        所有的触点都被验证过，或者所有验证过的点pi，都无法在符合聚类条件的前提下，以pi为中心，找到未被验证的点集类
    */

    //用于存储待验证点的广度优先遍历的循环列表
    int posListToBeVeri[KMAXTOUCHNUM];    
    int nPosListCount = 0;
    int nListHead = 0;

    int nPosClusterCent = 0; //聚类中心位置
    int nMaxCount = m_nClusterCount[0];
    for (int i = 1; i < m_nTouches; i++)
    {
        if (m_nClusterCount[i] > nMaxCount)
        {
            nPosClusterCent = i;
            nMaxCount = m_nClusterCount[i];
        }
    }

    do 
    {
        AddValidTouch(nPosClusterCent);
        hadVerified[nPosClusterCent] = true;

        for (int i = 0; i < m_nClusterCount[nPosClusterCent]; i++)
        {
            int &refClusterPos = m_nClusterPos[nPosClusterCent][i];
            if (!hadVerified[refClusterPos] && !hadAdded2List[refClusterPos])
            {
                int storePos = (nListHead + nPosListCount) % KMAXTOUCHNUM;
                posListToBeVeri[storePos] = refClusterPos;

                nPosListCount++;

                hadAdded2List[refClusterPos] = true;
            }
        }

        if(nPosListCount <= 0) break;

        nPosClusterCent = posListToBeVeri[nListHead];

        nListHead = (nListHead + 1) % KMAXTOUCHNUM;

        nPosListCount--;
    } while (true);    
}

void CInputTouchClusterAlg::ResetClusterInfo()
{
	if (m_bHadReset) return;

    m_nValidTouchsAdded = 0;
    m_nValidTouchsMatched = 0;

	m_nTouches = 0;
	memset(m_nClusterCount, 0, sizeof(int) * KMAXTOUCHNUM);
	memset(m_nClusterPos, 0, sizeof(int) * KMAXTOUCHNUM * KMAXTOUCHNUM);

	m_bHadReset = true;
}

void CInputTouchClusterAlg::UpdateMaxDistanceLimit(int nXLimit, int nYLimit)
{
	m_nXMaxDisLimit = nXLimit;
	m_nYMaxDisLimit = nYLimit;
}
