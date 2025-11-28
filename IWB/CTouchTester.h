#pragma once
template <int PENCOUNT = 30>
class CTouchTester
{
public:
	CTouchTester();
	~CTouchTester();
	
	void Reset();

	BOOL Process();

protected:
	TContactInfo m_ContactInfo[PENCOUNT];
};
