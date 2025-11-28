#pragma once

#include "WGestureRecPublic.h"

typedef struct _ST_RAWTOUCHINFO
{
    int state;  //0, mismatched;1，Matched; 2，newadded
    ST_TouchInfo rawInfo;
}ST_RAWTOUCHINFO;

class CInputTouchClusterAlg
{
private:
    CInputTouchClusterAlg(void);
    ~CInputTouchClusterAlg(void);

public:
    static void AddTouch(const UINT &uId, const POINT &ptPos, int matchState);
    static void GetValidInputAfterCluster();
	static void ResetClusterInfo();
	static void UpdateMaxDistanceLimit(int nXLimit, int nYLimit);

private:
    static void AddValidTouch(int nPos);

public:
    static ST_TouchInfo m_validTouchsAdded[KMAXTOUCHNUM];    
    static int          m_nValidTouchsAdded;
    static ST_TouchInfo m_validTouchsMatched[KMAXTOUCHNUM];
    static int          m_nValidTouchsMatched;

private:
    static ST_RAWTOUCHINFO m_rawTouchSet[KMAXTOUCHNUM];        //当前采集的触点集合
    static int             m_nTouches;                      //当前采集的触点个数
    static int             m_nClusterCount[KMAXTOUCHNUM];   //m_nClusterCount[i]表示以触点i为中心聚类，聚集的触点个数（包括它自身）
    static int             m_nClusterPos[KMAXTOUCHNUM][KMAXTOUCHNUM];     //m_nClusterPos[i]表示以触点i为中心聚类，聚集的触点在m_touchset中的位置
	static bool			   m_bHadReset;

	static int			   m_nXMaxDisLimit;
	static int			   m_nYMaxDisLimit;
};
