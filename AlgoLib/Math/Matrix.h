#pragma once

#include "Vec.h"
#include <stdio.h>



template<class T,int nRows,int nCols>
class Matrix
{

public:
	typedef Vec<T, nCols> RowType;
	typedef Vec<T, nRows> ColType;
	typedef  Matrix<T, nRows, nCols> MatrixType;
	Matrix()
		:
	m_nRows(nRows),
	m_nCols(nCols)
	{
		
	}

	MatrixType& operator = (const MatrixType& right)
	{

		for(int nRow = 0; nRow < m_nRows; nRow++)
		{
			m_ary[nRow] = right[nRow];
		}

		return *this;
	}
	
	
	//@function :get a row of the matrix
	//@parameter:nRowNo, 0 based
	RowType& GetRow(int nRowNo)
	{

		if(nRowNo < 0 || nRowNo >= m_nRows) throw "Row no error!";

		return m_ary[m_nRowNo];


	}

	//@function :get a row of the matrix
	//@parameter:nRowNo, 0 based
	const RowType& GetRow(int nRowNo)const
	{

		if(nRowNo < 0 || nRowNo >= m_nRows) throw "Row no error!";

		return m_ary[m_nRowNo];


	}

	//@function :set a row of the matrix
	//@parameter:nRowNo, 0 based
	void SetRow(int nRowNo,RowType& row)
	{
		if(nRowNo < 0 || nRowNo >= m_nRows) throw "Row no error!";
		//if(vecRow.size() <=0 || vecRow.size() >= m_nCols) throw "Column size mismatched.";

		m_ary[m_nRowNo] = row;
	}



	//@function :get a col of the matrix
	//@parameter:nColNo, 0 based.
	ColType GetCol(int nColNo)  
	{
		 ColType col;
		if(nColNo < 0 || nColNo >= m_nCols) throw "Column no error"; 

		for(int i=0; i< m_nRows;i++)
		{
			col[i] = m_ary[m_nRows][nColNo];
		}
		return col;
	}

	//@function :the matrix multiplied by a col vector
	//@parameter:vecCol, 
	ColType operator*(const RowType& col)
	{
		ColType result;
		for(int iRow=0; iRow< m_nRows; iRow++)
		{
			result[iRow]=0;
			for(int iCol=0;iCol<m_nCols;iCol++)
			{
				result[iRow] += m_ary[iRow][iCol] * col[iCol];
			}
		}

		return result;
	}

	void  SetVal(int nRow, int nCol,const T& val)
	{


		if(nCol < 0 || nCol >= m_nCols) throw "Column no error."; 
		if(nRow < 0 || nRow >= m_nRows) throw "Row no error.";		

		m_ary[nRow][nCol]  = val;	
	}


	T GetVal(int nRow, int nCol) const
	{
		if(nCol < 0 || nCol >= m_nCols) throw "Column no error."; 
		if(nRow < 0 || nRow >= m_nRows) throw "Row no error.";		

		return m_ary[nRow][nCol];	
	}


	Matrix<T,nRows,nCols> operator*(const Matrix<T,nRows,nCols> & right)
	{

		Matrix<T,nRows,nCols> oResult;

		//if(m_nRows != right.GetCols())
		//	throw "Left matrix row number does not match the colum number of the right matrix.";
		//oResult.SetCols(right.GetCols());
		//oResult.SetRows(m_nRows);



		for(int iRow=0; iRow < m_nRows; iRow++)
		{

			for(int iCol=0; iCol< m_nCols; iCol++)
			{
				T val = (T)0;

				for(int k=0; k<m_nCols;k++)
				{
					val += m_ary[iRow][k] * right.GetVal(k,iCol);

				}

				oResult.SetVal(iRow,iCol,val);
			}//for iCol

		}//for iRow


		return oResult;

	}

	RowType& operator[](int nRow)
	{

		if(nRow < 0 || nRow >= m_nRows) throw "Row no error.";
		return m_ary[nRow];

	}


	const RowType& operator[](int nRow) const
	{

		if(nRow < 0 || nRow >= m_nRows) throw "Row no error.";
		return m_ary[nRow];

	}

	inline int GetRows() const 
	{
		return m_nRows;
	}

	//inline void SetRows(int nRows)
	//{
	//	m_nRows = nRows; 

	//	m_ary.resize(m_nRows);
	//	for(int i=0;i< m_nRows; i++)
	//	{
	//		m_ary[i].resize(m_nCols);
	//		memset(&m_ary[i][0],0,m_ary[i].size());
	//	}
	//}

	inline int GetCols()const 
	{
		return m_nCols;
	}

	//inline void SetCols(int nCols)
	//{
	//	m_nCols = nCols; 

	//	m_ary.resize(m_nRows);
	//	for(int i=0;i< m_nRows; i++)
	//	{
	//		m_ary[i].resize(m_nCols);
	//		memset(&m_ary[i][0],0,m_ary[i].size());
	//	}
	//}

	void Zero()
	{
		for(int iRow = 0; iRow < m_nRows; iRow++)
		{
			for(int iCol = 0; iCol < m_nCols; iCol++)
			{

				m_ary[iRow][iCol ] = 0;
			}
		}
	}


	void SwapRow(int row1, int row2)
	{
		for(int iCol = 0; iCol < m_nCols; iCol++)
		{
			T temp = m_ary[row1][iCol];
			m_ary[row1][iCol]  = m_ary[row2][iCol];
			m_ary[row2][iCol]  = temp;

		}	

	}


	//转置操作
	Matrix<T,nCols, nRows> Transpose() const
	{
		Matrix<T,nCols, nRows> m;

		for(int i = 0; i< nRows; i++)
		{
			for(int j=0; j < nCols; j++)
			{
				m[j][i] = m_ary[i][j];

			}//for-each(j)
			
		}//for-each(i)


		return m;


	}

	//@功能:将矩阵内容保存在ASCII文件中
	//参 数:szFileName, 文件名
	//@说明:每列数据之间以两个空格分隔开,和Matlab中保存的格式一致
	bool Save(const char* szFileName)const
	{

		FILE* file = NULL;
			
		errno_t err =  fopen_s(&file, szFileName, "wt");

		if(err == EINVAL)
		{
			return false;
		}


		//char buf[256];
		
		for(int r=0; r < m_nRows; r++)
		{
			const RowType& row = m_ary[r];
			
			for(int c=0; c < m_nCols; c++)
			{
				
				fprintf(file, "  %.7e", row[c]);

			}//for-each(col)

			fprintf(file, "\r\n");
		}//for-each(row)

		fclose(file);
		return true;
	}
protected:
	Vec<RowType, nRows> m_ary;
	int m_nRows;//
	int m_nCols;//
};


//方阵
template <class T, int Dimension>
class CSquareMatrix:public Matrix<T, Dimension, Dimension>
{
public:
	typedef CSquareMatrix<T, Dimension> SquareMatrixType;

	
	//@功能:拷贝构造函数
	const CSquareMatrix& operator=(const Matrix<T, Dimension, Dimension>& right)
	{
		this->m_ary;// = right.m_ary;
		return *this;
	}

	//@功  能:利用代数余子式计算方阵的行列式值
	//@返回值:T
	//
	T Det()
	{
		T value = T(0);


		for(int col=0; col< this->m_nCols; col++)
		{

			T sign  =  (col % 2) == 0? 1:-1; 

			CSquareMatrix<T, Dimension -1> cofactorMatrix;

			cofactorMatrix = CofactorMatrix(0, col);

			value += sign * m_ary[0][col] * cofactorMatrix.Det();
		}

		return value;
	}


	//@功 能:利用行列式变换计算行列式的值。
	//      将行列式左下角全部化为0
	//@说 明:利用代数余子式计算行列式的值时, 在计算高阶行列时, 效率很差
	//      例如10阶的行列式, 需要至少做10!次双精度浮点运算3,628,800次
	//@注意 :使用Det2必须保证T类型不能为整型，必须为浮点类型     
	T Det2()
	{
		SquareMatrixType m = *this;
		T value = T(0);


		//
		for(int row=0; row<m.m_nRows; row++)
		{
			
			if(m.m_ary[row][row] == 0 )
			{
				
				//在row+1...m_nRow-1之间查找第row列不为0的行, 并与第row行交换。
				int l = 0;
				for(l=row+1; l<m.m_nRows; l++)
				{
					if(m.m_ary[l][row] != 0)
					{
						//交换l行和row行
						m.SwapRow(l, row);
						break;
					}

				}

				//所有行的第row列均为0
				if(l == m.m_nRows)
				{
					return T(0);
				}

			}
			

			//变换row+1行...m_nRow-1行
			for(int l=row+1; l<m_nRows; l++)
			{
				
				T coef =   m.m_ary[l][row] / m.m_ary[row][row];

				for(int col=row; col<m.m_nCols; col++)
				{
					m.m_ary[l][col]  = m.m_ary[l][col]  - m.m_ary[row][col]*coef;

				}
			}	


		}



		//用对角线元素的乘积即为行列式的值。

		value = T(1);
		for(int i=0; i<m.m_nRows; i++)
		{
			value *= m.m_ary[i][i];
		}

		return value;
	}

	//@功  能:计算方阵第row行col列的代数余子式矩阵
	//@返回值:T
	CSquareMatrix<T, Dimension - 1 >  CofactorMatrix(int nRowPos, int nColPos)
	{

		CSquareMatrix<T, Dimension -1 > m;
		int i=0, j=0;
		for(int row=0; row <m_nRows; row ++)
		{
			if(row == nRowPos)
			{
				continue;
			}
			else
			{
				for(int col=0; col < m_nCols;  col++)
				{
					if(col == nColPos)
					{
						continue;
					}

					m[i][j] = m_ary[row][col];

					j++;
				}//for col
			}

			i++;
			j = 0;
		}//for row

		return m;
	}

/*
	virtual CSquareMatrix<T, Dimension> operator*(const CSquareMatrix<T, Dimension> & right)
	{

		CSquareMatrix<T, Dimension> oResult;

		//if(m_nRows != right.GetCols())
		//	throw "Left matrix row number does not match the colum number of the right matrix.";
		//oResult.SetCols(right.GetCols());
		//oResult.SetRows(m_nRows);



		for(int iRow=0; iRow < m_nRows; iRow++)
		{

			for(int iCol=0; iCol< m_nCols; iCol++)
			{
				T val = (T)0;

				for(int k=0; k<m_nCols;k++)
				{
					val += m_ary[iRow][k] * right.GetVal(k,iCol);

				}

				oResult.SetVal(iRow,iCol,val);
			}//for iCol

		}//for iRow


		return oResult;

	}
*/
private:

};

//1维方阵的模版实现, 以终止编译器的递归展开
template<class T>
class CSquareMatrix<T, 1>:public Matrix<T, 1,1>
{
public:
	//@
	//
	//
	T Det()
	{
		T value = m_ary[0][0];

		return value;
	}

private:

};
