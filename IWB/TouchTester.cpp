#include "stdafx.h"
#include "TouchTester.h"

CTouchTester::CTouchTester()
{
	m_nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	m_nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	m_nOffsetX = 50 ;   //X轴的偏移量
	m_nOffsetY = 200 ;   //Y轴的偏移量。
	m_nIntervalX = (m_nScreenWidth - m_nOffsetX * 2) / PENCOUNT;
	m_nIncreaseY = 0;
	m_nLeftNum = 0;
}

CTouchTester::~CTouchTester()
{
	Reset(); 
}

void CTouchTester::Reset()
{
	m_nOffsetX = 50 ; 
	m_nOffsetY = 200;   
	m_nIntervalX = (m_nScreenWidth - m_nOffsetX * 2) / PENCOUNT;
	m_nIncreaseY = 0;
	m_nLeftNum = 0;
}

BOOL CTouchTester::Process(const TContactInfo* pPenInfos)
{

//	m_nIncreaseY++;
//	for(int i = 0 ; i < PENCOUNT;i++)
//	{
//		m_ContactInfo[i].uId = i ;
//		////模拟的点的状态和参照点的状态保持一致，这样就可以控制所有点的按下和抬起了。
//		m_ContactInfo[i].ePenState = pPenInfos[0].ePenState;
//		m_ContactInfo[i].pt.x = m_nOffsetX + i*m_nIntervalX;
//		m_ContactInfo[i].pt.y = m_nOffsetY + m_nIncreaseY;
//    }
	//先计算参考值左边或者右边的值
	if (m_nIncreaseY == 0 )
	{
		m_nLeftNum = pPenInfos[0].pt.x / m_nIntervalX;
	    m_nIncreaseY++;
	}

	////参考线左边的线数
	for (int i = 0 ; i < m_nLeftNum; i++)
	{
		m_ContactInfo[i].uId = i;
		m_ContactInfo[i].ePenState = pPenInfos[0].ePenState;
		m_ContactInfo[i].pt.x = (pPenInfos[0].pt.x - (m_nLeftNum -1-i)*m_nIntervalX);
	    m_ContactInfo[i].pt.y = pPenInfos[0].pt.y;
	}

	////参考线右边的线数
	for(int j = m_nLeftNum; j < PENCOUNT; j++)
	{
		m_ContactInfo[j].uId = j;
		m_ContactInfo[j].ePenState = pPenInfos[0].ePenState;
		m_ContactInfo[j].pt.x = pPenInfos[0].pt.x + (j- m_nLeftNum +1)*m_nIntervalX;
		m_ContactInfo[j].pt.y = pPenInfos[0].pt.y;
	}
	return TRUE;
}

const TContactInfo* CTouchTester::GetContactInfo()const
{
	return &m_ContactInfo[0];
}

int CTouchTester::GetContactCount()
{
	return _countof(m_ContactInfo);
}