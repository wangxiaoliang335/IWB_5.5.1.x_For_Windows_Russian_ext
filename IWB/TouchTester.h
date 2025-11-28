#pragma once

//template <int PENCOUNT = 30>

class CTouchTester
{
public:
	CTouchTester();
	~CTouchTester();

	void Reset();

	BOOL Process(const TContactInfo* pPenInfos);

	const TContactInfo* GetContactInfo() const;
	int GetContactCount();

protected:

	int   m_nScreenWidth ;   //屏幕的长度。
	int   m_nScreenHeight;   //屏幕的高度。

	int   m_nOffsetX   ;     //X轴的偏移量的值。
	int   m_nOffsetY   ;     //Y轴的偏移量的值。
	int   m_nIntervalX ;     //每个线的间隔，
	int   m_nIncreaseY ;     //每条线的Y轴的增加值
	int   m_nLeftNum   ;     //参考值左边的条数

	static const int PENCOUNT = 30;
	TContactInfo m_ContactInfo[PENCOUNT];

};