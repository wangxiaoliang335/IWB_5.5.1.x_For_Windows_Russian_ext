#pragma once
#include <string>

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > 	tstring;
class CPerfDetector
{
public:
	CPerfDetector(const TCHAR* szInfo)
	{
		m_strInfo = szInfo;
		
		QueryPerformanceFrequency(&m_liFreq);
		QueryPerformanceCounter(&m_liStart);


	}


	~CPerfDetector()
	{
		TCHAR buffer[2048];
		LARGE_INTEGER liEnd;
		QueryPerformanceCounter(&liEnd);

		LARGE_INTEGER  liElapse;
		liElapse.QuadPart = (liEnd.QuadPart  - m_liStart.QuadPart);

		LARGE_INTEGER liElapseInMicroSecond;
		liElapseInMicroSecond.QuadPart = liElapse.QuadPart * 1000 * 1000/ m_liFreq.QuadPart;
		
		_stprintf_s(
			buffer, 
			_countof(buffer),
			_T("%s: elapse %I64u us\r\n"),
			m_strInfo.c_str(),
			liElapseInMicroSecond.QuadPart);

		OutputDebugString(buffer);
	} 

protected:
	tstring m_strInfo;
	LARGE_INTEGER m_liFreq;
	LARGE_INTEGER m_liStart;

};