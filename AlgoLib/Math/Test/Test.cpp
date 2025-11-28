// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Convolution.h"
int _tmain(int argc, _TCHAR* argv[])
{
	Kernel<BYTE, 2,2> filter;
	filter[0][0] = 0;
	filter[0][1] = 1;
	filter[1][0] = 2;
	filter[1][1] = 3;

	ColReference<BYTE,2> col;
	col = filter.Col(1);
	col = filter.Col(0);

	const ColReference<BYTE,2>& const_col =((const Kernel<BYTE, 2,2>&)filter).Col(1);

	BYTE image[32];

	Convolution<BYTE, 2, 2> c;
	c(image, 4, 8, filter);

	return 0;
}

