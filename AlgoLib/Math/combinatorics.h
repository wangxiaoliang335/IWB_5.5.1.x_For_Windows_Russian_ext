#pragma once
//排列组合计算类
#include <assert.h>
#include "Vec.h"
#include <vector>
//combination result
//template <class T, int Count>
//class CbR
//{
//public:
//	CbR()
//	{
//
//	}
//
//	CbR(const CbR& r)
//	{
//		memcpy(data, r.data, sizeof(data));
//	}
//
//	CbR& operator=(const CbR& right)
//	{
//		for(int i=0; i< Count; i++)
//		{
//			data[i] =  right.data[i];
//		}
//		return *this;
//	}
//	T& operator[](int index)
//	{
//		return data[index];
//	}
//protected:
//	T data[Count];
//};


template <class T, int M, int N>
class CCombinatorics
{
public:
	typedef Vec<T,N> ResultElement;
	typedef std::vector<ResultElement> Result;


	T& operator [] (int i)
	{
		assert( i>= 0 && i<M);
		return m_data[i];
		
	}

	const T& operator [] (int i) const
	{
		assert( i>= 0 && i<M);
		return m_data[i];
		
	}

	//@功能:计算排列组合结果
	//      
	Result CalcResult()
	{
		Result result;

		CCombinatorics<T, M-1, N  > sub1;
		CCombinatorics<T, M-1, N-1> sub2;
		for(int i=1; i < M; i++)
		{
			sub1[i-1] = m_data[i];
			sub2[i-1] = m_data[i];
		}
		
		CCombinatorics<T, M-1, N> ::Result result1 = sub1.CalcResult();

		result = result1;


		CCombinatorics<T, M-1, N-1> ::Result result2 = sub2.CalcResult();

		for(int i=0; i < (int)result2.size(); i++)
		{
			ResultElement element;
			
			element[0] = m_data[0];

			for(int j=1; j<N; j++)
			{
				element[j] = result2[i][j-1];
			}


			result.push_back(element);

		}

		return result;
	}

protected:
	T m_data[M];
	

};

template<class T, int M>
class CCombinatorics<T, M, M>
{
public:
	typedef Vec<T,M> ResultElement;
	typedef std::vector<ResultElement> Result;
	
	T& operator [] (int i)
	{
		assert( i>= 0 && i<M);
		return m_data[i];
		
	}

	const T& operator [] (int i) const
	{
		assert( i> 0 && i<M);
		return m_data[i];
		
	}

	Result CalcResult()
	{
		Result  result;
		ResultElement element;
		for(int i=0; i < M; i++)
		{
			element[i] = m_data[i];
		}
		result.push_back(element);


		
		return result;
	}

protected:
	T m_data[M];
};



template<class T, int M>
class CCombinatorics<T, M, 1>
{
public:
	typedef Vec<T,1> ResultElement;
	typedef  std::vector<ResultElement> Result;
	T& operator [] (int i)
	{
		assert( i>= 0 && i<M);
		return m_data[i];
		
	}

	const T& operator [] (int i) const
	{
		assert( i> 0 && i<M);
		return m_data[i];
		
	}

	Result CalcResult()
	{

		Result allResult;
		
		for(int i=0; i<M; i++)
		{
			ResultElement	result;
			result[0] = m_data[i];
			allResult.push_back(result);
		}

		return allResult;
	}

protected:
	T m_data[M];
};


#include <iostream>
//测试实例
inline void CCombinatorics_Test()
{
	std::cout<<"C<5,3> result:" << std::endl;
	CCombinatorics<int, 5, 3> comb;
	comb[0] = 1;
	comb[1] = 2;
	comb[2] = 3;
	comb[3] = 4;
	comb[4] = 5;

	CCombinatorics<int, 5,3>::Result result = comb.CalcResult();

	for(int i=0; i< (int)result.size(); i++)
	{
		std::cout<<"No"<<i;
		for(int j = 0; j < 3; j++)
		{
			std::cout<< result[i][j];
		}//for j

		std::cout<<std::endl;
	}//for i
	


}
