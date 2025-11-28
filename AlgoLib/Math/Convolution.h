#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
//the row  reference object 
template<class T, int N>
class RowReference
{
public:
	RowReference(T (*pRow)[N])
		:
	m_pRow(pRow)
	{

	};


	T& operator[] (int col)
	{
		return m_pRow[0][col];
	}

	const T& operator[](int col)const
	{
		return m_pRow[0][col];
	}
protected:
	T (* m_pRow)[N];
};



//the column reference object
template<class T, int N>
class ColReference
{
public:

	ColReference()
	{
		memset(m_pCol, 0, sizeof(m_pCol));
	}


	T& operator[](int row)
	{
		return m_pCol[0][row];
	}

	const T& operator[](int row)const
	{
		return m_pCol[0][row];
	}


	void SetElemAddr(int iIndex, T * pElem)
	{
		m_pCol[iIndex] = pElem;
	}

protected:
	T* m_pCol[N];

};


//2D convolution Kernel
template<class T, int ROW,  int COL>
class Kernel
{
public:
	Kernel()
	{
		m_nRow = ROW;
		m_nCol = COL;

		memset(m_aryData, 0 ,sizeof(m_aryData));
		
	}


	~Kernel()
	{
		
	}



	RowReference<T, COL> operator[](int nRow)
	{
		RowReference<T, COL> refRow( reinterpret_cast<T (*)[COL]>(&m_aryData[nRow]));		
		return  refRow;
	}


	const RowReference<T, COL> operator[](int nRow)const
	{
		RowReference<T, COL> refRow( const_cast< T (*)[COL]>(&m_aryData[nRow]));		
		return  refRow;
	}



	ColReference<T, ROW> Col(int nCol)
	{
		ColReference<T, ROW> col;
		for(int row=0; row<ROW; row++)
		{
			
			col.SetElemAddr(row, &m_aryData[row][nCol]);

		}//for

		return col;
	}


	const ColReference<T, ROW> Col(int nCol) const
	{
		ColReference<T, ROW> col;
		for(int row=0; row<ROW; row++)
		{
			col.SetElemAddr(row, const_cast<T*>(&m_aryData[row][nCol]));

		}//for

		return col;
	}

	//@功能:给矩阵赋值
	BOOL Assign(const T* pData, int elemCount)
	{
		BOOL bRet = FALSE;

		do
		{
			if( elemCount != m_nRow*m_nCol) 
			{
				break;
			}

			memcpy(m_aryData, pData, sizeof(T)* elemCount);

			bRet = TRUE;
		}while(1);

		return bRet;
	}

	//@功能:返回行数
	const int  Row()const
	{
		return m_nRow;
	}

	//@功能:返回列数
	const int Col()const
	{
		return m_nCol;
	}

protected:
	T   m_aryData[ROW][COL];
	int m_nRow;
	int m_nCol;
};



//2D M×N gaussian distribution matrix
//
//
//G(x,y,σ)= 1/(2πσ^2) * exp[-(x^2 + y^2)/(2*σ^2)];
//σ^2, is the variance
//
template<int M, int N>
class GaussianKernel:public Kernel<double, M, N>
{
public:
	GaussianKernel(double sigma = 1.0)
	{

		int nCenterY = M/2;
		int nCenterX = N/2;

		 double coef  = 1/(2.0*M_PI*sigma*sigma);
		 double twoSigmaSquare = 2 * sigma * sigma;

		 //
		 for(int m = 0;  m < M ; m++)
		 {
			 for(int n = 0; n < N; n++)
			 {
				m_aryData[m][n] = coef * exp(-((m-nCenterY)*(m-nCenterY) + (n-nCenterX)*(n-nCenterX))/twoSigmaSquare);

			 }//for-each(col)
		 }//for-each(row)

	}


};


//the 2D X gradient of the guassian probability distribution
//
//
//Gx(x,y,σ)= 1/(2πσ^2) * exp[-(x^2 + y^2)/(2*σ^2)];
//σ^2, is the variance
//
template<int M, int N>
class GaussianXGradientKernel:public Kernel<double, M, N>
{
public:
	GaussianXGradientKernel(double sigma = 1.0)
	{

		int nCenterY = M/2;
		int nCenterX = N/2;

		 //double coef  = 1/(2.0*M_PI*sigma*sigma*sigma*sigma);
		 double coef  = 1/(sqrt(2.0*M_PI)*sigma*sigma*sigma);
		 double twoSigmaSquare = 2 * sigma * sigma;

		 //
		 for(int m = 0;  m < M ; m++)
		 {
			 for(int n = 0; n < N; n++)
			 {
				int x = (n - nCenterX);
				int y = (m - nCenterY);

				m_aryData[m][n] = -x * coef * exp(-(y*y + x*x)/twoSigmaSquare);

			 }//for-each(col)
		 }//for-each(row)

	}
};



//the 2D Y gradient of the guassian probability distribution
//
//
//Gx(x,y,σ)= 1/(2πσ^2) * exp[-(x^2 + y^2)/(2*σ^2)];
//σ^2, is the variance
//
template<int M, int N>
class GaussianYGradientKernel:public Kernel<double, M, N>
{
public:
	GaussianYGradientKernel(double sigma = 1.0)
	{

		int nCenterY = M/2;
		int nCenterX = N/2;

		 //double coef  = 1/(2.0*M_PI*sigma*sigma*sigma*sigma);
		 double coef  = 1/(sqrt(2.0*M_PI)*sigma*sigma*sigma);
		 double twoSigmaSquare = 2 * sigma * sigma;

		 //
		 for(int m = 0;  m < M ; m++)
		 {
			 for(int n = 0; n < N; n++)
			 {
				int x = (n - nCenterX);
				int y = (m - nCenterY);

				m_aryData[m][n] = -y * coef * exp(-(y*y + x*x)/twoSigmaSquare);

			 }//for-each(col)
		 }//for-each(row)

	}
};




//卷积算法
//
//
//template<class T, int ROW, int COL, T MIN=0, T MAX=255>
class Convolution
{
public:

	//@功能:对图片做卷积操作
	//@参数:pImageSrcData, 图像原始数据
	//      nImageWidth, 图像宽度
	//      nImageHeight, 图像高度
	//      Kernel, kernel对象	
	template<class T, class T2, int ROW, int COL/*, T MIN=0, T MAX=255*/>
	BOOL operator()(const T* pImageSrcData, UINT nImageWidth, UINT nImageHeight, T* pDestData, const Kernel<T2, ROW, COL>& kernel)
	{
		BOOL bRet = FALSE;
		
		int kCols = kernel.Col();
		int kRows = kernel.Row();
		int nCenterKx = kCols/2;
		int nCenterKy = kRows/2;

		for(UINT row=0; row < nImageHeight; row++)
		{
			for(UINT col=0; col < nImageWidth; col++)
			{

				double  sum = 0.0;
			

				//和Kernel进行卷积操作
				for(int i=0;i<kRows;i++)
				{
					for(int j=0; j<kCols; j++)
					{
						int  m = (int)row  + (i - nCenterKy);
						 int n = (int)col  + (j - nCenterKx);
						 

						 if(m<0 || m >= (int)nImageHeight || n<0 || n >= (int)nImageWidth)
						 {
							continue;
						 }
						
						 double k =  (double)kernel[i][j];
						 sum += (double)pImageSrcData[m*nImageWidth + n] * k;

						 
					
					}
				}
				//
				//
				sum = abs(sum);
				//if(sum > T(MAX))
				//{
				//	sum = T(MAX);
				//}
				//else if(sum < T(MIN))
				//{
				//	sum = T(MIN);
				//}

				//
				pDestData[row*nImageWidth + col] = T(sum);
				//
				//pDestData[row*nImageWidth + col] = pImageSrcData[row*nImageWidth + col];

				


			}//for each col

		}//for each row


		bRet = TRUE;

		return bRet;
	}
};

//
