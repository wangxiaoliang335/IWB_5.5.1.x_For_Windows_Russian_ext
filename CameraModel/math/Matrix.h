#pragma once
//@Author: toxuke@gmail.com
//@date  : 2014/05/04
#include <exception>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <float.h>
#define LAPACK_COMPLEX_STRUCTURE
#define LAPACK_GLOBAL_PATTERN_UC
#include <lapacke_config.h>
#include <lapacke.h>
#include <assert.h>
#include <vector>


namespace MatlabSimu
{
    template<class T> class Matrix;
    template<class T> class Vector;
    template<class T> class RowIterator;
    template<class T> class Const_RowIterator;
    template<class T> class ColIterator;
    template<class T> class Const_ColIterator;
    template<class T> class SubmatrixAccessor;

    template<class T> Matrix<T>  Mean(const Matrix<T> & m);
    template<class T>  T         Mean(const RowIterator<T> & row);
    template<class T>  T         Mean(const Const_RowIterator<T> & row);
	template<class T>  T         Mean(const ColIterator<T> & col);
    template<class T>  T         Mean(const Const_ColIterator<T> & col);
	
	        

    template<class T>  Matrix<T> Abs(const Matrix<T>& m);
    template<class T>  Matrix<T> Abs(const RowIterator<T>& row_it);
    template<class T>  Matrix<T> Abs(const ColIterator<T>& col_it);

    template<class T>  Matrix<T> Transpose(const Matrix<T>& m);
    template<class T>  int             SVD(const Matrix<T>& m, Matrix<T>* S ,Matrix<T>* U = NULL, Matrix<T> *V= NULL);
    template<class T>  int         Inverse(const Matrix<T>& A, Matrix<T>& inv);
    template<class T>  T     FrobeniusNorm(const Matrix<T>& A);
    template<class T>  T              Cond(const Matrix<T>& A);
    //
    struct SubscriptRange
    {
        int start;//subscript start value
        int step ;//subscript increase step
        int end  ;//subscript end value
    };

    //@function  :parse a subscript string and return a struct SubscriptRange
    //@parameters:szRowRange, 
    //            nMinValue, minimum subscript value
    //            nMaxValue, maximum subscript value
    inline SubscriptRange parseRange(const char* szRowRange, int nMinValue, int nMaxValue)
    {
        SubscriptRange  range;

        const char* p = szRowRange;

        bool bFindDigit = 0;
        const char* pDigit = szRowRange;


        int  integerCount = 0;
        int  integers[3];
        bool bHasColon   = false;

        while(*p != '\0')
        {
            if(':' == *p)
            {
                if(bFindDigit)
                {
                    if(integerCount < _countof(integers))
                    {
                        integers[integerCount] = atoi(pDigit);
                        integerCount ++;
                    }


                    bFindDigit = false;
                }

                bHasColon = true;
            }

            else if('0' <= *p && *p <= '9')
            {
                if(!bFindDigit)
                {
                    bFindDigit = true;
                    pDigit = p;
                }    
            }

            p++;
        }//while


        if(bFindDigit)
        {
            if(integerCount < _countof(integers))
            {
                integers[integerCount] = atoi(pDigit);
                integerCount ++;
            }

            bFindDigit = false;
        }


        if(integerCount == 0)
        {//":"

            if(!bHasColon)
                //throw std::exception("invalid subscript expression");
                throw std::runtime_error("invalid subscript expression");

            range.start = nMinValue;
            range.step  = 1;
            range.end   = nMaxValue;
        }
        else if(integerCount == 1)
        {//"1", "2"
            if(integers[0] < nMinValue || integers[0] > nMaxValue)
                //throw std::exception("invalid subscript expression");
                throw std::runtime_error("invalid subscript expression");

            range.start = integers[0];
            range.step  = 1;
            range.end   = integers[0];
        }
        else if(integerCount == 2)
        {//"3:4"
            if(integers[0] < nMinValue || integers[0] > nMaxValue)
                //throw std::exception("invalid subscript expression");
                throw std::runtime_error("invalid subscript expression");

            if(integers[1] < nMinValue || integers[1] > nMaxValue)
                //throw std::exception("invalid subscript expression");
                throw std::runtime_error("nvalid subscript expression");

            range.start = integers[0];
            range.step  = 1;
            range.end   = integers[1];

        }
        else if(integerCount == 3)
        {//"1:2:4"
            if(integers[0] < nMinValue || integers[0] > nMaxValue)
                //throw std::exception("invalid subscript expression");
                throw std::runtime_error("invalid subscript expression");

            if(integers[1] < nMinValue || integers[1] > nMaxValue)
                //throw std::exception("invalid subscript expression");
                throw std::runtime_error("invalid subscript expression");

            range.start = integers[0];
            range.step  = integers[1];
            range.end   = integers[2];

        }

        return range;
    }

    //@function: return the count of the subscript range.
    inline int RangeCount(const SubscriptRange& range)
    {
        //"1:0:2"
        if(range.step == 0) return 0;

        //"1:-1:3"
        if(range.end > range.start && range.step<0 ) return 0;

        //"2:1:1"
        if(range.end < range.start && range.step>0 ) return 0;


        //"1:1:1"  returns 1
        //"2:2:10" returns 5
        int nCount = (range.end - range.start)/range.step + 1;

        return nCount;
    }


    //行迭代器
    template<class T>
    class RowIterator
    {
    public:
        typedef RowIterator<T> row_iterator_type;
        typedef class Matrix<T>   matrix_type;
        RowIterator(Matrix<T>* pMatrix, int nRowNo)
        {
            m_nRowNo  = nRowNo;
            m_pMatrix = pMatrix;
        }

        T& operator[](int nColNo)
        {
            if(nColNo < 0 || nColNo >= m_pMatrix->m_nCols)
                //throw std::exception("Invalid Column Number");
                throw std::runtime_error("Invalid Column Number");

            return m_pMatrix->m_pData[nColNo * m_pMatrix->m_nRows + m_nRowNo];
        }

        row_iterator_type& operator+= (const T& value)
        {

            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            for(int c = 0; c < N; c ++)
            {
                T& refData = m_pMatrix->m_pData[c * M + m_nRowNo];
                refData += value;
            }

            return *this;
        }


        row_iterator_type& operator-= (const T& value)
        {

            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            for(int c = 0; c < N; c ++)
            {
                T& refData = m_pMatrix->m_pData[c * M + m_nRowNo];
                refData -= value;
            }

            return *this;
        }

        row_iterator_type& operator*= (const T& value)
        {
            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            for(int c = 0; c < N; c ++)
            {
                T& refData = m_pMatrix->m_pData[c * M + m_nRowNo];
                refData *= value;
            }

            return *this;
        }

        row_iterator_type& operator/= (const T& value)
        {

            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            for(int c = 0; c < N; c ++)
            {
                T& refData = m_pMatrix->m_pData[c * M + m_nRowNo];

                refData /= value;
            }

            return *this;
        }


        Matrix<T> operator+ (const T& value)
        {
            Matrix<T> m(1, m_pMatrix->m_nCols);
           
            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            for(int c = 0; c < N; c ++)
            {
                m.m_pData[c] = m_pMatrix->m_pData[c * M + m_nRowNo] + value;                
            }

            return m;
        }

        Matrix<T> operator- (const T& value)
        {
            Matrix<T> m(1, m_pMatrix->m_nCols);
            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;
           
            for(int c = 0; c < N; c ++)
            {
                m.m_pData[c] = m_pMatrix->m_pData[c * M + m_nRowNo] - value;
            }

            return m;
        }


        Matrix<T> operator* (const T& value)
        {
            Matrix<T> m(1, m_pMatrix->m_nCols);
            
            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            for(int c = 0; c < N; c ++)
            {
                m.m_pData[c] = m_pMatrix->m_pData[c * M + m_nRowNo] * value;
            }

            return m;
        }



        Matrix<T> operator/ (const T& value)
        {
            Matrix<T> m(1, m_pMatrix->m_nCols);
           
            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            for(int c = 0; c < N; c ++)
            {
                m.m_pData[c] = m_pMatrix->m_pData[c * M + m_nRowNo] / value;                
            }

            return m;
        }


        row_iterator_type& operator+= (const row_iterator_type& right)
        {

            if(m_pMatrix->m_nCols != right.m_pMatrix->m_nCols)
                //throw std::exception("column number not matched!");
                throw std::runtime_error("column number not matched!");
            
            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;


            for(int c = 0; c < N; c ++)
            {
                int offset = c * M + m_nRowNo;

                T& refData = m_pMatrix->m_pData[offset];
                  refData += right.m_pMatrix->m_pData[offset];
            }

            return *this;
        }

        row_iterator_type& operator-= (const row_iterator_type& right)
        {

            if(m_pMatrix->m_nCols != right.m_pMatrix->m_nCols)
                //throw std::exception("column number not matched!");
                throw std::runtime_error("column number not matched!");

            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            for(int c = 0; c < N; c ++)
            {
                int offset = c * M + m_nRowNo;
                T& refData = m_pMatrix->m_pData[offset];
                 refData -= right.m_pMatrix->m_pData[offset];
            }

            return *this;
        }


        row_iterator_type& operator*= (const row_iterator_type& right)
        {

            if(m_pMatrix->m_nCols != right.m_pMatrix->m_nCols)
                //throw std::exception("column number not matched!");
                throw std::runtime_error("column number not matched!");

            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            int left_offset  =  m_nRowNo;
            int right_offset =  right.m_nRowNo;
            for(int c = 0; c < N; c ++)
            {
                
                T& refData = m_pMatrix->m_pData[left_offset];
                 refData *= right.m_pMatrix->m_pData[right_offset];

                 left_offset  += M;
                 right_offset += M;
            }
            return *this;
        }



        row_iterator_type& operator/= (const row_iterator_type& right)
        {

            if(m_pMatrix->m_nCols != right.m_pMatrix->m_nCols)
                //throw std::exception("column number not matched!");
                throw std::runtime_error("column number not matched!");

            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            int left_offset  =  m_nRowNo;
            int right_offset =  right.m_nRowNo;
            for(int c = 0; c < N; c ++)
            {
                
                T& refData = m_pMatrix->m_pData[left_offset];
                 refData /= right.m_pMatrix->m_pData[right_offset];

                 left_offset  += M;
                 right_offset += M;
            }

            return *this;
        }




        friend   matrix_type;

        friend  T  Mean<T> (const RowIterator& row);
        friend matrix_type Abs<T>(const row_iterator_type& row_it);
    

        

    protected:
        Matrix<T>* m_pMatrix;
        int     m_nRowNo ;
    };

    //常量行迭代器
    template<class T>
    class Const_RowIterator
    {
    public:
        typedef Const_RowIterator<T> const_row_iterator_type;
        Const_RowIterator(const Matrix<T>* pMatrix, int nRowNo)
        {
            m_nRowNo  = nRowNo;
            m_pMatrix = pMatrix;

        }

        const T& operator[](int nColNo)
        {
            if(nColNo < 0 || nColNo >= m_pMatrix->m_nCols)
                //throw std::exception("Invalid Column Number");
                throw std::runtime_error("Invalid Column Number");

            return m_pMatrix->m_pData[m_pMatrix->m_nRows * nColNo + m_nRowNo];
        }

        friend  T  Mean<T> (const Const_RowIterator& row);

    protected:
        const Matrix<T>* m_pMatrix;
        int   m_nRowNo ;
    };


    //列迭代器
    template<class T>
    class ColIterator
    {
    public:
        typedef class ColIterator<T> col_iterator_type;
        typedef class Matrix<T>      matrix_type;
        ColIterator(Matrix<T>* pMatrix, int nColNo)
        {
            m_nColNo  = nColNo;
            m_pMatrix = pMatrix;
        }

        T& operator[](int nRowNo)
        {
            if(nRowNo < 0 || nRowNo >= m_pMatrix->m_nRows)
                //throw std::exception("Invalid Row Number");
                throw std::runtime_error("Invalid Row Number");

            return m_pMatrix->m_pData[m_nColNo * m_pMatrix->m_nRows + nRowNo];
        }



        col_iterator_type operator-= (const T& value)
        {
            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            for(int r = 0; r < M; r ++)
            {
                T& refData = m_pMatrix->m_pData[m_nColNo * M + r];

                refData -= value;
            }

            return *this;
        }


        col_iterator_type operator+= (const T& value)
        {
            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            for(int r = 0; r < M; r ++)
            {
                T& refData = m_pMatrix->m_pData[m_nColNo * M + r];

                refData += value;
            }

            return *this;
        }



        col_iterator_type operator*= (const T& value)
        {
            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;


            for(int r = 0; r < M; r ++)
            {
                T& refData = m_pMatrix->m_pData[m_nColNo * M + r];

                refData *= value;
            }

            return *this;
        }




        col_iterator_type operator/= (const T& value)
        {
            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            for(int r = 0; r < M; r ++)
            {
                T& refData = m_pMatrix->m_pData[m_nColNo * M + r];

                refData /= value;
            }

            return *this;
        }



        Matrix<T> operator- (const T& value)
        {
            Matrix<T> m(m_pMatrix->m_nCols, 1);

            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;          

            for(int r = 0; r < M; r ++)
            {
                m.m_pData[r] = m_pMatrix->m_pData[m_nColNo * M + r] - value;                
            }

            return m;
        }


        Matrix<T> operator+ (const T& value)
        {
            Matrix<T> m(m_pMatrix->m_nCols, 1);
            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols; 
           
            for(int r = 0; r < M; r ++)
            {
                m.m_pData[r] = m_pMatrix->m_pData[m_nColNo * M + r] + value;                
            }

            return m;
        }


        Matrix<T> operator* (const T& value)
        {
            Matrix<T> m(m_pMatrix->m_nCols, 1);
            
            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols; 

            for(int r = 0; r < M; r ++)
            {
                m.m_pData[r] = m_pMatrix->m_pData[m_nColNo * M + r] * value;                
            }

            return m;
        }



        Matrix<T> operator/ (const T& value)
        {
            Matrix<T> m(m_pMatrix->m_nCols, 1);
            
            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols; 

            for(int r = 0; r < M; r ++)
            {
                m.m_pData[r] = m_pMatrix->m_pData[m_nColNo * M + r] / value;                
            }

            return m;
        }



        col_iterator_type& operator+= (const col_iterator_type& right)
        {

            if(m_pMatrix->m_nRows != right.m_pMatrix->m_nRows)
                //throw std::exception("column number not matched!");
                throw std::runtime_error("column number not matched!");

            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            for(int r = 0; r < M; r ++)
            {
                int offset = m_nColNo * M + r;
                T& refData = m_pMatrix->m_pData[offset];

                refData += right.m_pMatrix->m_pData[offset];
            }

            return *this;
        }

        col_iterator_type& operator-= (const col_iterator_type& right)
        {

            if(m_pMatrix->m_nRows != right.m_pMatrix->m_nRows)
                //throw std::exception("column number not matched!");
                throw std::runtime_error("column number not matched!");

            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            for(int r = 0; r < M; r ++)
            {
                int offset = m_nColNo * M + r;
                T& refData = m_pMatrix->m_pData[offset];

                refData -= right.m_pMatrix->m_pData[offset];
            }

            return *this;
        }


        col_iterator_type& operator*= (const col_iterator_type& right)
        {

            if(m_pMatrix->m_nRows != right.m_pMatrix->m_nRows)
                //throw std::exception("column number not matched!");
                throw std::runtime_error("column number not matched!");

            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            int left_offset  = m_nColNo*M;
            int right_offset = right.m_nColNo*M;

            for(int r = 0; r < M; r ++)
            {
                T& refData = m_pMatrix->m_pData[left_offset];
                refData *= right.m_pMatrix->m_pData[right_offset];
                left_offset  ++;
                right_offset ++;
            }

            return *this;
        }


        col_iterator_type& operator/= (const col_iterator_type& right)
        {

            if(m_pMatrix->m_nRows != right.m_pMatrix->m_nRows)
                //throw std::exception("column number not matched!");
                throw std::runtime_error("column number not matched!");

            int M = m_pMatrix->m_nRows;
            int N = m_pMatrix->m_nCols;

            int left_offset  = m_nColNo*M;
            int right_offset = right.m_nColNo*M;

            for(int r = 0; r < M; r ++)
            {

                T& refData = m_pMatrix->m_pData[left_offset];
                refData /= right.m_pMatrix->m_pData[right_offset];
                left_offset  ++;
                right_offset ++;
            }

            return *this;
        }


        friend  matrix_type;
        //friend  T  Mean<T>(const ColIterator& col);
        friend  T  Mean<T>(const ColIterator<T> & col);
        friend matrix_type Abs<T>(const col_iterator_type& col_it);
        
    protected:
        Matrix<T>* m_pMatrix;
        int     m_nColNo ;            
    };



    //常数列迭代器
    template<class T>
    class Const_ColIterator
    {
    public:
        Const_ColIterator(const Matrix<T>* pMatrix, int nColNo)
        {
            m_nColNo = nColNo;
            m_pMatrix = pMatrix;
        }

        const T& operator[](int nRowNo) const
        {
            if(nRowNo < 0 || nRowNo >= m_pMatrix->m_nRows)
                //throw std::exception("Invalid Row Number");
                throw std::runtime_error("Invalid Row Number");

            return m_pMatrix->m_pData[m_nColNo * m_pMatrix->m_nRows + nRowNo];
        }

        friend  T  Mean<> (const Const_ColIterator& col);
    protected:
        const Matrix<T>* m_pMatrix;
        int              m_nColNo ;            
    };


    //
    template<class T>
    class SubmatrixAccessor
    {
    public:
        SubmatrixAccessor(const Matrix<T>* pMatrix, const SubscriptRange& rowRange, const SubscriptRange& colRange)
        {
            m_pParentMatrix = pMatrix;
            m_rowRange      = rowRange;
            m_colRange      = colRange;

            m_subRows = RangeCount(rowRange);
            m_subCols = RangeCount(colRange);
        }

        
        SubmatrixAccessor<T>& operator=(const Matrix<T>& m)
        {
            if(this->m_subRows != m.m_nRows || this->m_subCols != m.m_nCols)
                //throw std::exception("Dimension not matched.");
                throw std::runtime_error("Dimension not matched.");
            
            int left_row = this->m_rowRange.start;
            int left_col = this->m_colRange.start;
            int left_M   = this->m_pParentMatrix->m_nRows;

            int right_M = m.m_nRows;
            int right_N = m.m_nCols;

            for(int r=0; r < right_M; r++)
            {
                left_col = this->m_colRange.start;

                for(int c=0; c < right_N; c++)
                {
                    this->m_pParentMatrix->m_pData[left_col * left_M + left_row] = m.m_pData[c * right_M + r];

                    left_col += this->m_colRange.step;
                }

                left_row += this->m_rowRange.step;
            }

            return *this;
        }

        SubmatrixAccessor<T>& operator=(const SubmatrixAccessor<T>& m)
        {
            if(this->m_subRows != m.m_subRows || this->m_subCols != m.m_subCols)
                //throw std::exception("Dimension not matched.");
                throw std::runtime_error("Dimension not matched.");

            int left_row = this->m_rowRange.start;
            int left_col = this->m_colRange.start;
            int left_M   = this->m_pParentMatrix->m_nRows;

            int right_row = m.m_rowRange.start;
            int right_col = m.m_colRange.start;
            int right_M   = m.m_pParentMatrix->m_nRows;


            for(int i=0; i < this->m_subRows; i++)
            {
                left_col  = this->m_colRange.start;
                right_col = m.m_colRange.start;
                for(int j=0; j<this->m_subCols; j++)
                {
                    this->m_pParentMatrix->m_pData[left_col * left_M + left_row] = m.m_pParentMatrix->m_pData[right_col * right_M + right_row];
                    left_col  += this->m_colRange.step;
                    right_col += m.m_colRange.step;
                }

                left_row  += this->m_rowRange.step;
                right_row +=  m.m_colRange.step;
            }


            return *this;
        }


        //@function: Print the submatrix
        void Print(const char* prefix = NULL)
        {

            if(prefix)
            {
                std::cout << prefix;
            }
            //std::cout<<std::setfill(' ')<<std::setw(12);            
            std::cout << std::setprecision(12);
            std::cout << std::setiosflags(std::ios_base::scientific);


            int M = m_pParentMatrix->m_nRows;
            int N = m_pParentMatrix->m_nCols;


             bool bRowIncrease = true;
             if(m_rowRange.end < m_rowRange.end) bRowIncrease = false;

              bool bColIncrease = true;
             if(m_colRange.end < m_colRange.end) bColIncrease = false;

             for(int r = m_rowRange.start; bRowIncrease?r <= m_rowRange.end: r >= m_rowRange.end; r += m_rowRange.step)
             {
  
                 for(int c = m_colRange.start; bColIncrease? c <= m_colRange.end: c >= m_colRange.end; c += m_colRange.step)
                 {
                       
                     const T& d=  m_pParentMatrix->m_pData[c*M +r];
                     std::cout<<std::setw(12)<<d<<'\t';

                 }
                    
                 std::cout<<std::endl;
             }


            std::cout << std::endl;
        }




    protected:
        friend class Matrix<T>;
        const Matrix<T>* m_pParentMatrix ;
        SubscriptRange   m_rowRange;
        SubscriptRange   m_colRange;
        int              m_subRows;
        int              m_subCols;
    };


    //Matrix class
    //Simulate Matlab syntax
    template<class T>
    class Matrix
    {

    public:
        typedef class Matrix<T>                   matrix_type;
        typedef class RowIterator<T>             row_iterator;
        typedef class Const_RowIterator<T> const_row_iterator;

        typedef class  ColIterator<T>             col_iterator;
        typedef class Const_ColIterator<T> const_col_iterator;

        typedef  class SubmatrixAccessor<T> submatrix_accessor;
    
        friend  row_iterator;
        friend  const_row_iterator;
        friend  col_iterator;
        friend  const_col_iterator;
        friend  submatrix_accessor;

        friend Matrix  Mean<T>(const Matrix             & m  );
        friend  T      Mean<T>(const row_iterator       & row);
        friend  T      Mean<T>(const const_row_iterator & row);
        friend  T      Mean<T>(const col_iterator       & col);
        friend  T      Mean<T>(const const_col_iterator & col);


        friend  matrix_type Abs<T>(const Matrix<T>& m);
        friend  matrix_type Abs<T>(const row_iterator& row_it);
        friend  matrix_type Abs<T>(const col_iterator& col_it);

        friend  matrix_type Transpose<T>(const matrix_type & m);
        friend  int              SVD<T>(const matrix_type & m, matrix_type * D ,matrix_type* U, matrix_type *V);
        friend  int          Inverse<T>(const Matrix<T>& m, Matrix<T>& inv);
        friend   T       FrobeniusNorm<T>(const Matrix<T>& A);
        friend double             Cond<T>(const Matrix<T>& A);

        Matrix()
            :
        m_nRows(1),
        m_nCols(1),
        m_bExternalBuf(false)
        {
            m_pData = &m_data[0];
        }


        Matrix(int nRows, int nCols)
            :
        m_pData(&m_data[0]),
        m_bExternalBuf(false)
        {
           SetDimension(nRows, nCols);
        }

        //@function:contruct a matrix from a string
        //@remark  :each matrix row is ened with an semi-colon, the last semi-colon can be omitted.
        //          each element in a row is separated with a space.
        //
        //"8.79   9.93   9.83   5.45   3.16;\
        // 6.11   6.91   5.04  -0.27   7.98;\
        // -9.15  -7.93   4.86   4.85   3.01;\
        // 9.57   1.64   8.83   0.74   5.80;\
        //-3.49   4.02   9.80  10.00   4.27;\
        // 9.84   0.15  -8.99  -6.02  -5.31";
        explicit Matrix(const char* szMatrix)
            :
        m_pData(&m_data[0]),
        m_bExternalBuf(false)
        {
            LoadFromString(szMatrix);
        }


        Matrix(const T* pData, int nRows, int nCols)
        :            
        m_pData(&m_data[0]),
        m_bExternalBuf(false)
        {
             SetDimension(nRows, nCols);
             memcpy(m_pData, pData,  sizeof(T) * nRows * nCols);
        }

        Matrix(T* pData, int nRows, int nCols)
        :            
        m_pData(&m_data[0]),
        m_bExternalBuf(true)
        {
             Attach(pData, nRows, nCols);
        }


        //@function: Copy constructor
        
        Matrix(const Matrix<T>& right)
            :
         m_pData(&m_data[0]),
        m_bExternalBuf(false)
        {
            SetDimension( right.m_nRows, right.m_nCols);

            memcpy(m_pData, right.m_pData, sizeof(T) * (m_nRows * m_nCols));
        }


        Matrix(const row_iterator& row_it)
            :
         m_pData(&m_data[0]),
        m_bExternalBuf(false)
        {
            SetDimension(1, row_it.m_pMatrix->m_nCols);

            for(int c = 0 ; c < m_nCols; c ++)
            {
                int nOffset =  c * row_it.m_pMatrix->m_nRows + row_it.m_nRowNo;
                m_pData[c]  =  row_it.m_pMatrix->m_pData[nOffset];
            }


        }

        Matrix(const col_iterator& col_it)
             :
         m_pData(&m_data[0]),
        m_bExternalBuf(false)
        {
            SetDimension( col_it.m_pMatrix->m_nRows, 1);

            for(int r = 0 ; r < m_nRows; r ++)
            {
                int nOffset = col_it.m_nColNo * col_it.m_pMatrix->m_nRows + r;
                m_pData[r]  = col_it.m_pMatrix->m_pData[nOffset];
            }
        }


        Matrix(const submatrix_accessor& m)
             :
         m_pData(&m_data[0]),
        m_bExternalBuf(false)
         {
             SetDimension(m.m_subRows, m.m_subCols);

            int left_row = m.m_rowRange.start;
            int left_col = m.m_colRange.start;
            int left_M   = m.m_pParentMatrix->m_nRows;
            
            for(int r=0; r < this->m_nRows; r++)
            {
                left_col  = m.m_colRange.start;
                

                for(int c=0; c<this->m_nCols; c++)
                {
                    m_pData[c * m_nRows + r] = m.m_pParentMatrix->m_pData[left_col * left_M + left_row];

                    left_col  += m.m_colRange.step;
                
                }

                left_row  += m.m_rowRange.step;
                
            }            
         }


        Matrix<T>& operator= (const Matrix<T>& right)
        {
            SetDimension(right.m_nRows, right.m_nCols);
            memcpy(m_pData, right.m_pData, sizeof(T) * (m_nRows * m_nCols));
            return *this;
        }

        
         Matrix<T>& operator=(const submatrix_accessor& m)
         {
             SetDimension(m.m_subRows, m.m_subCols);

            int left_row = m.m_rowRange.start;
            int left_col = m.m_colRange.start;
            int left_M   = m.m_pParentMatrix->m_nRows;
            
            for(int r=0; r < this->m_nRows; r++)
            {
                left_col  = m.m_colRange.start;

                for(int c=0; c < this->m_nCols; c++)
                {
                    m_pData[c * m_nRows + r] = m.m_pParentMatrix->m_pData[left_col * left_M + left_row];
                    left_col  += m.m_colRange.step;

                }

                left_row  += m.m_rowRange.step;

            }
            return *this;
         }




        virtual ~Matrix()
        {
            if(!m_bExternalBuf && m_pData != &m_data[0])
            {
                delete[]m_pData; 
            }
        }

        void Attach(T* pBuf, int nRows, int nCols)
        {
            m_pData = pBuf;
            m_nRows = nRows;
            m_nCols = nCols;
            m_bExternalBuf = true;
        }

        void Detach(T* pBuf)
        {
            if(m_pData == pBuf)
            {
                m_pData = &m_data[0];
                m_nRows = 0;
                m_nCols = 0;
                m_bExternalBuf = false;
            }
        }


        void LoadFromString(const char* szMatrix)
        {
            const char* p = szMatrix;

            int nRows = 0;
            int nCols = 0;
            int nRowElementCount = 0;
            bool bFindDigit = 0;

            //统计矩阵行数和烈数
            while(*p !='\0')
            {
                if(*p == ';') 
                {
                    //列数取一行的最大元素数目
                    if(nCols < nRowElementCount)
                    {
                        nCols = nRowElementCount;
                    }
                    nRows ++;
                    nRowElementCount = 0;

                }
                else if(
                    *p == '+'
                       ||
                    *p == '-'
                       ||
                     *p == '.'
                       ||
                    ('1' <= *p && *p <='9')
                 )
                {
                    bFindDigit = true;                   

                }
                else if(*p == ' '|| *p== '\t' || *p=='\r' || *p=='\n')
                {
                    if(bFindDigit)
                    {
                        nRowElementCount ++;
                        bFindDigit =false;
                    }
                }

                p++;
            }

            if(*(p -1) != ';')//最后一行未以';'结尾
            {
                nRows ++;
            }


            SetDimension(nRows, nCols);


            //存放数据
             p = szMatrix;
             int r=0;//行
             int c=0;//列
             T value = T(0);
             bFindDigit = false;
             const char* pDigit = p;
             while(*p != '\0')
             {
                if(*p == ';')
                {
                    if(bFindDigit)
                    {
                        this->m_pData[c*m_nRows + r] = atof(pDigit);
                        bFindDigit = false;
                     }

                    r ++;
                    c = 0;
                }

                else if(
                    *p == '+'
                       ||
                    *p == '-'
                       ||
                     *p == '.'
                       ||
                    ('1' <= *p && *p <='9')
                 )
                {
                    if(!bFindDigit)
                    {   
                        pDigit    = p;
                        bFindDigit = true;
                    }
                                   

                }
                else if(*p == ' '|| *p== '\t' || *p=='\r' || *p=='\n')
                {
                    if(bFindDigit)
                    {
                        this->m_pData[c*m_nRows + r] = atof(pDigit);
                        c ++;
                        bFindDigit =false;
                    }
                }


                p ++;
             }

              if(bFindDigit)//最后一个数据
              {
                 this->m_pData[c*m_nRows + r] = atof(pDigit);
                 bFindDigit = false;
              }
        }


        void SetDimension(int row, int col)
        {
            if(!m_bExternalBuf && m_pData != &m_data[0] )
            {
                delete[] m_pData;
                m_pData = &m_data[0];
            }


            if(row * col <=  _countof(m_data))
            {
                m_nRows = row;
                m_nCols = col;
                m_pData = &m_data[0];

            }
            else
            {
                if(row*col)
                {
                    m_pData = (double*)new T[row*col];

                    if(NULL == m_pData)
                        //throw std::exception("memory alloc failed!");
                        throw std::runtime_error("memory alloc failed!");
                }

                m_nRows = row;
                m_nCols = col;
            }


            
        }

        //@function:return an M-by-N matrix of zeros
        void Zeros(int m, int n)
        {
            SetDimension(m,n);
            memset(m_pData, 0, sizeof(T)*m_nRows*m_nCols);
        }


        //@function:return an m-by-n matrix with 1's on the diagonal and zeros elsewhere.
        void Eye(int m, int n)
        {
            Zeros(m,n);

            int rank = m_nRows > m_nCols?m_nCols : m_nRows;

            for(int i = 0; i < rank; i++)
            {
                m_pData[m_nRows*i + i] = T(1);
            }
        }

        int Size()
        {
            return m_nRows * m_nCols;
        }

        //@function :get a row of the matrix
        //@parameter:nRowNo, 0 based
        row_iterator GetRow(int nRowNo)
        {
            if(nRowNo < 0 || nRowNo >= m_nRows)
                //throw std::exception("Invalid Row Number!");
                throw std::runtime_error("Invalid Row Number!");
            row_iterator it(this, nRowNo);
            return it;
        }

        //@function :get a row of the matrix
        //@parameter:nRowNo, 0 based
        const_row_iterator GetRow(int nRowNo)const
        {
            if(nRowNo < 0 || nRowNo >= m_nRows)
                //throw std::exception("Invalid Row Number!");
                throw std::runtime_error("Invalid Row Number!");
            const_row_iterator it(this, nRowNo);
            return it;
        }


        operator T*()
        {
            return m_pData;
        }

        const T* GetDataPtr() const
        {
            return m_pData;
        }


        T* GetDataPtr()
        {
            return m_pData;
        }
        row_iterator operator[](int nRowNo)
        {
            if(nRowNo < 0 || nRowNo >= m_nRows)
                //throw std::exception("Invalid Row Number!");
                throw std::runtime_error("Invalid Row Number!");
            row_iterator it(this, nRowNo);
            return it;
        }


        const_row_iterator operator[](int nRowNo)const
        {
            if(nRowNo < 0 || nRowNo >= m_nRows)
                //throw std::exception("Invalid Row Number!");
                throw std::runtime_error("Invalid Row Number!");
            const_row_iterator it(this, nRowNo);
            return it;
        }

        //simulate matlab syntax
        //M("1:2", ":")
        //return a submatrix of M, the submatrix consists of the first two rows of matrix M.
        //
       SubmatrixAccessor<T> operator()(const char* szRow, const char * szCol)
        {
            SubscriptRange rowRange = parseRange(szRow, 0, this->m_nRows - 1);
            SubscriptRange colRange = parseRange(szCol, 0, this->m_nCols - 1);
            
            SubmatrixAccessor<T> subMatrix(this, rowRange, colRange);

            return subMatrix;

        }

        T& operator()(int i, int j)
        {
            if(i<0 || i>=m_nRows)
                //throw std::exception("Subscript out of range.");
                throw std::runtime_error("Subscript out of range.");
            if(j<0 || j>=m_nCols)
                //throw std::exception("Subscript out of range.");
                throw std::runtime_error("Subscript out of range.");

            return m_pData[j*m_nRows + i];
        }




        //@function :get a col of the matrix
        //@parameter:nColNo, 0 based
        col_iterator GetCol(int nColNo)
        {
            if(nColNo < 0 || nColNo >= m_nCols)
                //throw std::exception("Invalid Column Number!");
                throw std::runtime_error("Invalid Column Number!");
            col_iterator it(this, nColNo);
            return it;
        }


        //@function :get a col of the matrix
        //@parameter:nColNo, 0 based
        const_col_iterator GetCol(int nColNo)const
        {
            if(nColNo < 0 || nColNo >= m_nCols)
                //throw std::exception("Invalid Column Number!");
                throw std::runtime_error("Invalid Column Number!");
            const_col_iterator it(this, nColNo);
            return it;
        }



        //T* operator&()
        //{
        //    return this->m_pData;
        //}

        //const T* operator&() const
        //{
        //    return this->m_pData;
        //}

        const int RowSize() const
        {
            return this->m_nRows;
        }

        const int ColSize()const
        {
            return this->m_nCols;
        }

        //@function:Add a scalar value to the elements of the matrix.
        Matrix<T> operator+(const T& value)
        {

            Matrix<T> oResult;

            oResult.SetDimension(m_nRows, m_nCols);

            for(int r = 0; r < m_nRows; r++)
            {
                int offset = r;
                for(int c = 0; c < m_nCols; c++)
                {
                    oResult.m_pData[offset] = m_pData[offset] + value;
                    offset += m_nRows;
                }

            }
            return oResult;
        }


        //@function:Add a matrix with the same dimension and return a new matrix;
        Matrix<T> operator+(const Matrix<T> &right)
        {
            if(m_nCols != right.m_nCols || m_nRows != right.m_nRows) 
                //throw "Matrix addition needs two matrix have same dimension";
                throw std::runtime_error("Matrix addition needs two matrix have same dimension");

            Matrix<T> oResult;

            oResult.SetDimension(m_nRows, m_nCols);

            for(int r = 0; r < m_nRows; r++)
            {
                int offset = r;
                for(int c = 0; c < m_nCols; c++)
                {
                    oResult.m_pData[offset] = m_pData[offset] + right.m_pData[offset];                    
                    offset += m_nRows;
                }

            }
            return oResult;
        }


        //@function:Add a scalar value to each element of the matrix, and store the value in this matrix.
        Matrix<T>& operator+= (const T& value)
        {
            for(int r = 0; r < m_nRows; r++)
            {
                int offset = r;
                for(int c = 0; c < m_nCols; c++)
                {
                    m_pData[offset] += value;
                    offset += m_nRows;
                }
            }
            return *this;
        }


        //@function:Add a matrix with the same dimension, and store the value in this matrix.
        Matrix<T>& operator+= (const Matrix<T> &right)
        {
            if(m_nCols != right.m_nCols || m_nRows != right.m_nCols) 
                //throw "Matrix addition needs two matrix have same dimension";
                throw  std::runtime_error("Matrix addition needs two matrix have same dimension");


            for(int r = 0; r < m_nRows; r++)
            {
                int offset = r;
                for(int c = 0; c < m_nCols; c++)
                {
                    m_pData[offset] += right.m_pData[offset];
                    offset += m_nRows;
                }

            }
            return *this;
        }


        //@function:Subtract a scalar value from each elements of the matrix.
        Matrix<T> operator-(const T& value)
        {
            Matrix<T> oResult;

            oResult.SetDimension(m_nRows, m_nCols);

            for(int r = 0; r < m_nRows; r++)
            {
                int offset = r;
                for(int c = 0; c < m_nCols; c++)
                {
                    oResult.m_pData[offset] = m_pData[offset] - value;
                    offset += m_nRows;
                }

            }
            return oResult;
        }



        //@function:Subtract a matrix with the same dimension, and return a new matrix
        Matrix<T> operator-(const Matrix<T> &right)
        {
            if(m_nCols != right.m_nCols || m_nRows != right.m_nRows) 
                //throw "Matrix addition needs two matrix have same dimension";
                throw std::runtime_error("Matrix addition needs two matrix have same dimension");

            Matrix<T> oResult;

            oResult.SetDimension(m_nRows, m_nCols);

            for(int r = 0; r < m_nRows; r++)
            {
                int offset = r;
                for(int c = 0; c < m_nCols; c++)
                {
                    oResult.m_pData[offset] = m_pData[offset] - right.m_pData[offset];
                    offset += m_nRows;
                }

            }
            return oResult;
        }


        //@function:Subtract a scalar value from each elements of the matrix, and store the value in this matrix.   
        Matrix<T>& operator-=(const T& value)
        {
            for(int r = 0; r < m_nRows; r++)
            {
                int offset  = r;
                for(int c = 0; c < m_nCols; c++)
                {
                    m_pData[offset] -= value;
                    offset += m_nRows;
                }

            }
            return *this;
        }



        //@function:Subtract a matrix with the same dimension, and store the value in this matrix.   
        Matrix<T>& operator-=(const Matrix<T> &right)
        {
            if(m_nCols != right.m_nCols || m_nRows != right.m_nCols) 
                //throw "Matrix addition needs two matrix have same dimension";
                throw std::runtime_error("Matrix addition needs two matrix have same dimension");


            for(int r = 0; r < m_nRows; r++)
            {
                int offset = r;
                for(int c = 0; c < m_nCols; c++)
                {
                    m_pData[offset] -= right.m_pData[offset];
                    offset += m_nRows;
                }

            }
            return *this;
        }



        //@function:Multiple a scalar with each element of the matrix
        //@remark  :
        Matrix<T> operator*(const T & value)
        {
            Matrix<T> oResult;
            int nRows = this->m_nRows;
            int nCols = this->m_nCols;
            oResult.SetDimension(nRows, nCols);

            for(int r = 0; r < nRows; r++)
            {
                int offset = r;
                for(int c = 0; c < nCols; c++)
                {
                    oResult.m_pData[offset] =  this->m_pData[offset] * value;
                    offset += m_nRows;

                }//for
            }//for

            return oResult;
        }



        //@function:This matrix multiplies with other matrix.
        //@remark  :the column dimension of the matrix(Multiplier matrix) must match the row dimension of the Multiplicand matrix.
        Matrix<T> operator*(const Matrix<T> & right)
        {
            if(m_nCols != right.m_nRows)
                //throw "Left matrix row number does not match the colum number of the right matrix";
                throw std::runtime_error("Left matrix row number does not match the colum number of the right matrix");

            Matrix<T> oResult;
            int nRows = this->m_nRows;
            int nCols = right.m_nCols;
            oResult.SetDimension(this->m_nRows, right.m_nCols);

            for(int r = 0; r < nRows; r++)
            {
                
                for(int c = 0; c < nCols; c++)
                {
                    T data = T(0);

                    //A*B
                    //对矩阵A的r行元素与矩阵B的c列的对应元素之积求和
                    for(int i = 0; i < this->m_nCols; i++)
                    {
                        data += this->m_pData[i * this->m_nRows + r] * right.m_pData[c * right.m_nRows + i];
                    }

                    oResult.m_pData[c * nRows + r] = data;

                }//for
            }//for

            return oResult;
        }


        //@function:矩阵右成一个列矢量
        //@remark:
        std::vector<T> operator*(const std::vector<T> vecRight)
        {
            assert(this->m_nCols == vecRight.size());
            
            std::vector<T> result;
            result.resize(this->m_nRows);

            for(int r = 0; r < this->m_nRows; r++)
            {
                double value = 0.0;
                for(int c = 0; c < this->m_nCols; c++)
                {
                    value += this->m_pData[c * this->m_nRows + r] * vecRight[c];
                }

                result[r] = value;

            }

            return result;
        }


        //@function:Multiple a scalar with each element of the matrix, and store the values still in this matrix
        //@remark  :
        Matrix<T>& operator *= (const T & value)
        {

            for(int r = 0; r < m_nRows; r++)
            {
                int offset = r;
                for(int c = 0; c < m_nCols; c++)
                {
                    T data =this->m_pData[r] * value;

                    this->m_pData[r] = data;
                    offset += m_nRows;

                }//for
            }//for

            return *this;
        }


        //@function:Multiple each element of the matrix by a scalar value
        //@remark  :
        Matrix<T> operator/(const T & value)
        {
            Matrix<T> oResult;
            int nRows = this->m_nRows;
            int nCols = this->m_nCols;
            oResult.SetDimension(nRows, nCols);

            for(int r = 0; r < nRows; r++)
            {
                int offset = r;
                for(int c = 0; c < nCols; c++)
                {
                    T data = T(0);

                    data  = this->m_pData[offset] / value;
                    oResult.m_pData[offset] = data;

                    offset += nRows;

                }//for
            }//for

            return oResult;
        }



        //@function:Multiple each element of the matrix by a scalar value, and keep values storing in this matrix
        //@remark  :
        Matrix<T>& operator/=(const T & value)
        {
            int nRows = this->m_nRows;
            int nCols = this->m_nCols;

            for(int r = 0; r < nRows; r++)
            {
                int offset = r;
                for(int c = 0; c < nCols; c++)
                {
                    T data = T(0);

                    data  = this->m_pData[offset] / value;
                    m_pData[offset] = data;
                    offset += nRows;

                }//for
            }//for

            return *this;
        }




        Matrix<T>& Transpose()
        {
            int nOldRows = m_nRows;
            int nOldCols = m_nCols;
           
            //elements of <ith row, jth column>  swaps with element of <jth row, rth column>
            for(int r = 0; r < nOldRows; r++)
            {
                for(int c = 0; c < r; c++)
                {
                    T val = m_pData[c*nOldRows + r];

                    m_pData[c*nOldRows + r]  =  m_pData[r*nOldRows + c];//element(r,c) swaps with elment(c,r);
                    m_pData[r*nOldRows + c] = val;

                }//for-each(column)
    
            }//for-each(row)

            m_nRows = nOldCols;
            m_nCols = nOldRows;

            return *this;

        }

         Matrix<T>&  Reshape(int m, int n)
        {
            if(m*n > this->m_nCols* this->m_nRows)
                //throw std::exception("matrix reshape error");
                throw std::runtime_error("matrix reshape error");

            this->m_nCols = m;
            this->m_nRows = n;

            return *this;

        }
        //@功能: Print the matrix
        void Print(const char* prefix = NULL)
        {
            if(prefix)
            {
                std::cout << prefix;
            }
            //std::cout<<std::setfill(' ')<<std::setw(12);            
            std::cout << std::setprecision(16);
            std::cout << std::setiosflags(std::ios_base::scientific);

            for(int r = 0; r < m_nRows; r++)
            {
                int offset = r;
                for(int c = 0; c < m_nCols; c++)
                {
                    T&  d  = m_pData[offset];
                    std::cout<<std::setw(25)<<d<<'\t';

                    offset += m_nRows;
                }
                std::cout<<std::endl;
            }
            std::cout << std::endl;
        }



    protected:




        //说明:为了与LPACK中的矩阵存储格式一致
        //     先存放第一列的数据， 然后再存放下一列
        //
        T*  m_pData    ;//dynamic allocate memory
        T   m_data[3*3];//
        int m_nRows    ;//
        int m_nCols    ;//
        bool m_bExternalBuf;//Attach了外部内存
    };


    //@function:return row vector containing the mean value of
    //          each column.
    //@remark  :
    //
    template<class T>
    Matrix<T>  Mean(const Matrix<T>& m)
    {
        Matrix<T> mean;
        mean.SetDimension(1, m.m_nCols);

         int offset = 0;
        for(int c = 0; c < m.m_nCols;  c++)
        {
            T sum = T(0);
           
            for(int r = 0; r < m.m_nRows; r++)
            {
                sum += m.m_pData[offset];
                offset ++;
            }
            mean.m_pData[c] = sum/m.m_nRows;
        }
        return mean;    
    }


    //@function:return mean value of a row
    //@remark  :
    template<class T>
    T  Mean(const RowIterator<T>& row)
    {
        T mean;
        mean = T(0);

        int offset = row.m_nRowNo;
        for(int c = 0; c < row.m_pMatrix->m_nCols;  c++)
        {
            mean   += row.m_pMatrix->m_pData[offset];            
            offset += row.m_pMatrix->m_nRows;
        }
        mean = mean / row.m_pMatrix->m_nCols;
        return mean;    
    }


    //@function:return mean value of a row
    //@remark  :
    template<class T> 
    T  Mean(const Const_RowIterator<T>& row)
    {
        T mean;
        mean = T(0);

        int offset = row.m_nRowNo;
        for(int c = 0; c < row.m_pMatrix->m_nCols;  c++)
        {
            mean   += row.m_pMatrix->m_pData[offset];
            offset += row.m_pMatrix->m_nRows;
        }
        mean = mean / row.m_pMatrix->m_nCols;
        return mean;    
    }



    //@function:return mean value of a column
    //@remark  :
    template<class T> 
    T  Mean(const ColIterator<T>& col)
    {
        T mean;
        mean = T(0);

        int offset = col.m_nColNo * col.m_pMatrix->m_nRows;

        for(int r = 0; r < col.m_pMatrix->m_nRows;  r ++)
        {
            mean   += col.m_pMatrix->m_pData[offset];
            offset ++;
        }
        mean = mean / col.m_pMatrix->m_nRows;
        return mean;    
    }


    //@function:return mean value of a row
    //@remark  :
    template<class T> 
    T  Mean(const Const_ColIterator<T>& col)
    {
        T mean;
        mean = T(0);

        int offset = col.m_nColNo * col.m_pMatrix->m_nRows;

        for(int r = 0; r < col.m_pMatrix->m_nRows;  r ++)
        {
            mean   += col.m_pMatrix->m_pData[offset];
            offset ++;
        }
        mean = mean / col.m_pMatrix->m_nRows;
        return mean;     
    }


    //absolute value function object
    template<class T>
    class abs_func
    {
    public:
       T operator()(const T& value)
       {
            return abs(value);
       }
    };

    template<>
    class abs_func<double>
    {
       public:
       
       double operator()(const double& value)
       {
            return fabs(value);
       }
    };

    template<>
    class abs_func<float>
    {
       public:
      
       float operator()(const float& value)
       {
            return fabs(value);
       }
    };


    //@function:return a matrix whose elements is the absolute value of matrix m.
    template<class T>
    Matrix<T> Abs(const Matrix<T>& m)
    {
        Matrix<T> result;
        result.SetDimension(m.m_nRows, m.m_nCols);

        int offset = 0;
        for(int c = 0; c < m.m_nCols;  c++)
        {
             for(int r = 0; r < m.m_nRows; r++)
            {
                abs_func<T> abs;
                result.m_pData[offset] = abs(m.m_pData[offset]);

                offset ++;
            }
        }

        return result;
    }


    template<class T>
    Matrix<T> Abs(const RowIterator<T>& row_it)
    {
        Matrix<T> result;
        int nCols = row_it.m_pMatrix->m_nCols;
        result.SetDimension(1, row_it.m_pMatrix->m_nCols);

        int nOffset = row_it.m_nRowNo;
        for(int c = 0; c < nCols ;  c++)
        {
            abs_func<T> abs;

            result.m_pData[c] = abs(row_it.m_pMatrix->m_pData[nOffset]);

            nOffset += row_it.m_pMatrix->m_nRows;
        }

        return result;
    }


    template<class T>
    Matrix<T> Abs(const ColIterator<T>& col_it)
    {
        Matrix<T> result;
        int nRows = col_it.m_pMatrix->m_nRows;
        result.SetDimension(nRows, 1);

        int nOffset = col_it.m_nColNo * col_it.m_pMatrix->m_nRows;

        for(int r = 0; r < nRows;  r++)
        {
            abs_func<T> abs;         
            result.m_pData[r] = abs(col_it.m_pMatrix->m_pData[nOffset]);
            nOffset ++;
        }

        return result;
    }


    template<class T>  Matrix<T> Transpose(const Matrix<T>& m)
    {
        Matrix<T> result;
        int nRows = m.m_nRows;
        int nCols = m.m_nCols;
        result.SetDimension(nCols,nRows);

        int offset = 0;
        int transpose_offset = 0;

        for(int c = 0; c < nCols; c++)
        {
            transpose_offset = c;

            for(int r = 0; r < nRows; r++)
            {                  
                result.m_pData[transpose_offset] = m.m_pData[offset];

                offset ++;
                transpose_offset +=  nCols;//c行r列

            }
            
        }

        return result;
    }




    //@Function: FrobeniusNorm(A) = sqrt(∑∑(Aij^2))
     template<class T>  T FrobeniusNorm(const Matrix<T>& A)
     {
         T sum = T(0);

         T* pData = A.m_pData;
        for(int c = 0; c < A.m_nCols;  c++)
        {
             for(int r = 0; r < A.m_nRows; r++)
            {
                T val = *pData;
                sum += val*val;
            }
        }

        return T(sqrt(double(sum)));
     }

    //@功能:奇异值分解 
    //@功能:使用LAPACK库中的函数LAPACKE_dgesvd完成奇异值分解。
    template<class T>  int SVD(const Matrix<T>& A, Matrix<T>* pS, Matrix<T>* pU , Matrix<T> *pV)
    {
        //LAPACK函数
        //LAPACKE_dgesvd的参数说明:http://www.netlib.org/lapack/explore-html/d8/d2d/dgesvd_8f.html
        //LAPACKE_dgesvd的例程参考:https://software.intel.com/sites/products/documentation/doclib/mkl_sa/11/mkl_lapack_examples/dgesvd_ex.c.htm

        //DGESVD computes the singular value decomposition (SVD) of a real
        //M-by-N matrix A, optionally computing the left and/or right singular
        //vectors. The SVD is written

        //     A = U * SIGMA * transpose(V)

        //where SIGMA is an M-by-N matrix which is zero except for its
        //min(m,n) diagonal elements, U is an M-by-M orthogonal matrix, and
        //V is an N-by-N orthogonal matrix.  The diagonal elements of SIGMA
        //are the singular values of A; they are real and non-negative, and
        //are returned in descending order.  The first min(m,n) columns of
        //U and V are the left and right singular vectors of A.

        //Note that the routine returns V**T, not V.

        //[in]	JOBU	
        //          JOBU is CHARACTER*1
        //          Specifies options for computing all or part of the matrix U:
        //          = 'A':  all M columns of U are returned in array U:
        //          = 'S':  the first min(m,n) columns of U (the left singular
        //                  vectors) are returned in the array U;
        //          = 'O':  the first min(m,n) columns of U (the left singular
        //                  vectors) are overwritten on the array A;
        //          = 'N':  no columns of U (no left singular vectors) are
        //                  computed.
        //[in]	JOBVT	
        //          JOBVT is CHARACTER*1
        //          Specifies options for computing all or part of the matrix
        //          V**T:
        //          = 'A':  all N rows of V**T are returned in the array VT;
        //          = 'S':  the first min(m,n) rows of V**T (the right singular
        //                  vectors) are returned in the array VT;
        //          = 'O':  the first min(m,n) rows of V**T (the right singular
        //                  vectors) are overwritten on the array A;
        //          = 'N':  no rows of V**T (no right singular vectors) are
        //                  computed.

        //          JOBVT and JOBU cannot both be 'O'.
        //[in]	M	
        //          M is INTEGER
        //          The number of rows of the input matrix A.  M >= 0.
        //[in]	N	
        //          N is INTEGER
        //          The number of columns of the input matrix A.  N >= 0.
        //[in,out]	A	
        //          A is DOUBLE PRECISION array, dimension (LDA,N)
        //          On entry, the M-by-N matrix A.
        //          On exit,
        //          if JOBU = 'O',  A is overwritten with the first min(m,n)
        //                          columns of U (the left singular vectors,
        //                          stored columnwise);
        //          if JOBVT = 'O', A is overwritten with the first min(m,n)
        //                          rows of V**T (the right singular vectors,
        //                          stored rowwise);
        //          if JOBU .ne. 'O' and JOBVT .ne. 'O', the contents of A
        //                          are destroyed.
        //[in]	LDA	
        //          LDA is INTEGER
        //          The leading dimension of the array A.  LDA >= max(1,M).
        //[out]	S	
        //          S is DOUBLE PRECISION array, dimension (min(M,N))
        //          The singular values of A, sorted so that S(i) >= S(i+1).
        //[out]	U	
        //          U is DOUBLE PRECISION array, dimension (LDU,UCOL)
        //          (LDU,M) if JOBU = 'A' or (LDU,min(M,N)) if JOBU = 'S'.
        //          If JOBU = 'A', U contains the M-by-M orthogonal matrix U;
        //          if JOBU = 'S', U contains the first min(m,n) columns of U
        //          (the left singular vectors, stored columnwise);
        //          if JOBU = 'N' or 'O', U is not referenced.
        //[in]	LDU	
        //          LDU is INTEGER
        //          The leading dimension of the array U.  LDU >= 1; if
        //          JOBU = 'S' or 'A', LDU >= M.
        //[out]	VT	
        //          VT is DOUBLE PRECISION array, dimension (LDVT,N)
        //          If JOBVT = 'A', VT contains the N-by-N orthogonal matrix
        //          V**T;
        //          if JOBVT = 'S', VT contains the first min(m,n) rows of
        //          V**T (the right singular vectors, stored rowwise);
        //          if JOBVT = 'N' or 'O', VT is not referenced.
        //[in]	LDVT	
        //          LDVT is INTEGER
        //          The leading dimension of the array VT.  LDVT >= 1; if
        //          JOBVT = 'A', LDVT >= N; if JOBVT = 'S', LDVT >= min(M,N).
        //[out]	WORK	
        //          WORK is DOUBLE PRECISION array, dimension (MAX(1,LWORK))
        //          On exit, if INFO = 0, WORK(1) returns the optimal LWORK;
        //          if INFO > 0, WORK(2:MIN(M,N)) contains the unconverged
        //          superdiagonal elements of an upper bidiagonal matrix B
        //          whose diagonal is in S (not necessarily sorted). B
        //          satisfies A = U * B * VT, so it has the same singular values
        //          as A, and singular vectors related by U and VT.
        //[in]	LWORK	
        //          LWORK is INTEGER
        //          The dimension of the array WORK.
        //          LWORK >= MAX(1,5*MIN(M,N)) for the paths (see comments inside code):
        //             - PATH 1  (M much larger than N, JOBU='N') 
        //             - PATH 1t (N much larger than M, JOBVT='N')
        //          LWORK >= MAX(1,3*MIN(M,N)+MAX(M,N),5*MIN(M,N)) for the other paths
        //          For good performance, LWORK should generally be larger.

        //          If LWORK = -1, then a workspace query is assumed; the routine
        //          only calculates the optimal size of the WORK array, returns
        //          this value as the first entry of the WORK array, and no error
        //          message related to LWORK is issued by XERBLA.
        //[out]	INFO	
        //          INFO is INTEGER
        //          = 0:  successful exit.
        //          < 0:  if INFO = -i, the i-th argument had an illegal value.
        //          > 0:  if DBDSQR did not converge, INFO specifies how many
        //                superdiagonals of an intermediate bidiagonal form B
        //                did not converge to zero. See the description of WORK
        //                above for details.

         Matrix<T>  AA = A;
        const char* JOBU  = (NULL == pU)?"N":"A";
        const char* JOBVT = (NULL == pV)?"N":"A";

         /*long*/ int M = AA.m_nRows;
         /*long*/ int N = AA.m_nCols;
        if(pS)
        {
            pS->SetDimension(min(M,N),1);
        }

        if(pU)
        {
            pU->SetDimension(M, M);
        }

        if(pV)
        {
            pV->SetDimension(N,N);
        }


         /*long*/ int nLDA  = M      ;//The leading dimensionof the array A.
         /*long*/ int nLDU  = M      ;//The leading dimensionof the array U
         /*long*/ int nLDVT = N      ;//The leading dimensionof the array VT
        

         double wkopt;
         /*long*/ int lwork = -1;
         /*long*/ int info  = 0;



        // Query and allocate the optimal workspace 
        //DGESVD(
            DGESVD(
              (char*)JOBU,
              (char*)JOBVT ,
              &M          ,
              &N          ,
              AA.m_pData  ,//
              &nLDA       ,//The leading dimensionof the array A
              pS->m_pData ,
              pU?pU->m_pData:NULL,
              &nLDU       ,
              pV?pV->m_pData:NULL ,
              &nLDVT      ,
              &wkopt      ,
              &lwork      ,
              &info);
        
        lwork = int(wkopt);

        double* work;
        work = (double*)malloc( lwork*sizeof(double) );


        // Compute SVD 
            DGESVD(
              (char*)JOBU ,
              (char*)JOBVT,
              &M          ,
              &N          ,
              AA.m_pData  ,//
              &nLDA       ,//The leading dimensionof the array A
              pS->m_pData ,
              pU?pU->m_pData:NULL,
              &nLDU       ,
              pV?pV->m_pData:NULL,
              &nLDVT      ,
              work        ,
              &lwork      ,
              &info);

        // Free workspace 
        free( (void*)work );

        BOOL bRet = FALSE;


        // Check for convergence 
        if( info > 0 ) 
        {
            printf( "The algorithm computing SVD failed to converge.\n" );
                
        }
        else if(info < 0)
        {
              printf( "%ld th argument has illegal value\n", (abs)(info));
        }

        if(pV)//
        {
            pV->Transpose();
        }
        return info;

    }



     //@功能:计算方阵的逆
     template<class T>  int  Inverse(const Matrix<T>& A, Matrix<T>& inv)
     {

         if(A.m_nCols != A.m_nRows)
             //throw std::exception("inverse a matrix which is not square matrix.");
             throw std::runtime_error("inverse a matrix which is not square matrix.");

         inv = A;

         /*long*/ int M   = A.m_nRows;
         /*long*/ int N   = A.m_nCols;
         /*long*/ int LDA = M;
         /*long*/ int *IPIV = (/*long*/ int*)malloc(M*sizeof(int));
         /*long*/ int info = 0;
        
         //使用LINPACK函数 DGETRF, DGETRI
        //http://forge.scilab.org/index.php/p/scilab2c/source/tree/1345/trunk/scilab2c/src/fortran/lapack/dgetrf.f
		 //Double General Trianguation Factor
        //*  Purpose
        //16	*  =======
        //17	*
        //18	*  DGETRF computes an LU factorization of a general M-by-N matrix A
        //19	*  using partial pivoting with row interchanges.
        //20	*
        //21	*  The factorization has the form
        //22	*     A = P * L * U
        //23	*  where P is a permutation matrix, L is lower triangular with unit
        //24	*  diagonal elements (lower trapezoidal if m > n), and U is upper
        //25	*  triangular (upper trapezoidal if m < n).
        //26	*
        //27	*  This is the right-looking Level 3 BLAS version of the algorithm.
        //28	*
        //29	*  Arguments
        //30	*  =========
        //31	*
        //32	*  M       (input) INTEGER
        //33	*          The number of rows of the matrix A.  M >= 0.
        //34	*
        //35	*  N       (input) INTEGER
        //36	*          The number of columns of the matrix A.  N >= 0.
        //37	*
        //38	*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
        //39	*          On entry, the M-by-N matrix to be factored.␊
        //40	*          On exit, the factors L and U from the factorization
        //41	*          A = P*L*U; the unit diagonal elements of L are not stored.
        //42	*
        //43	*  LDA     (input) INTEGER
        //44	*          The leading dimension of the array A.  LDA >= max(1,M).
        //45	*
        //46	*  IPIV    (output) INTEGER array, dimension (min(M,N))
        //47	*          The pivot indices; for 1 <= i <= min(M,N), row i of the
        //48	*          matrix was interchanged with row IPIV(i).
        //49	*
        //50	*  INFO    (output) INTEGER
        //51	*          = 0:  successful exit
        //52	*          < 0:  if INFO = -i, the i-th argument had an illegal value
        //53	*          > 0:  if INFO = i, U(i,i) is exactly zero. The factorization
        //54	*                has been completed, but the factor U is exactly
        //55	*                singular, and division by zero will occur if it is used
        //56	*                to solve a system of equations.
        //57	*
        //58	*  =====================================================================

         //DGETRF(
           DGETRF(
             &M,
             &N,
             inv.m_pData,
             &LDA,
             IPIV,
             &info
             );
        

         if(info != 0)
         {
            return info;
         }

		 //Let A = L*U
		 //then we get
		 //INV(A) = inv(L*U) = inv(U)*inv(L)
		 //INV(A)*L = inv(U)

		//DGETRI: Double General Trianguation Inverse
        //*  Purpose
        //16	*  =======
        //17	*
        //18	*  DGETRI computes the inverse of a matrix using the LU factorization
        //19	*  computed by DGETRF.
        //20	*
        //21	*  This method inverts U and then computes inv(A) by solving the system
        //22	*  inv(A)*L = inv(U) for inv(A).
        //23	*
        //24	*  Arguments
        //25	*  =========
        //26	*
        //27	*  N       (input) INTEGER
        //28	*          The order of the matrix A.  N >= 0.
        //29	*
        //30	*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
        //31	*          On entry, the factors L and U from the factorization
        //32	*          A = P*L*U as computed by DGETRF.
        //33	*          On exit, if INFO = 0, the inverse of the original matrix A.
        //34	*
        //35	*  LDA     (input) INTEGER
        //36	*          The leading dimension of the array A.  LDA >= max(1,N).
        //37	*
        //38	*  IPIV    (input) INTEGER array, dimension (N)
        //39	*          The pivot indices from DGETRF; for 1<=i<=N, row i of the
        //40	*          matrix was interchanged with row IPIV(i).
        //41	*
        //42	*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK))
        //43	*          On exit, if INFO=0, then WORK(1) returns the optimal LWORK.
        //44	*
        //45	*  LWORK   (input) INTEGER
        //46	*          The dimension of the array WORK.  LWORK >= max(1,N).
        //47	*          For optimal performance LWORK >= N*NB, where NB is
        //48	*          the optimal blocksize returned by ILAENV.
        //49	*
        //50	*          If LWORK = -1, then a workspace query is assumed; the routine
        //51	*          only calculates the optimal size of the WORK array, returns
        //52	*          this value as the first entry of the WORK array, and no error
        //53	*          message related to LWORK is issued by XERBLA.
        //54	*
        //55	*  INFO    (output) INTEGER
        //56	*          = 0:  successful exit
        //57	*          < 0:  if INFO = -i, the i-th argument had an illegal value
        //58	*          > 0:  if INFO = i, U(i,i) is exactly zero; the matrix is
        //59	*                singular and its inverse could not be computed.
        //60	*
        //61	*  =====================================================================
        //62	*

        double opt = 0;
		// Query and allocate the optimal workspace 
        /*long*/ int lWork = -1;


        DGETRI(
            &N,
            inv.m_pData,
            &LDA,
            IPIV,
            &opt,
            &lWork,
            &info);

        if(info !=0 )
        {
            return info;
        }
        lWork = (long int)(opt);
        double *Work = (double*)malloc(lWork*sizeof(double));
        
        
        DGETRI(
            &N,
            inv.m_pData,//(input) The factors L and U from factorization. (output) the inverse of the orginial matrix A
            &LDA,
            IPIV,
            Work,
            &lWork,
            &info);

        if(info !=0 )
        {
            return info;
        }

        free(Work);
        free(IPIV);

        return info;
     }


     //Condition number with respect to inversion.
     //COND(X) returns the 2-norm condition number (the ratio of the
     //largest singular value of X to the smallest).
     template<class T>   T  Cond(const Matrix<T>& A)
     {
        Matrix<T> S;
        SVD<T>(A, &S);
 
        double max_sigular_value = S(0,0);
        double min_sigular_value = S(S.m_nRows - 1,0);

        if(min_sigular_value == 0.0) return DBL_MAX;

        return max_sigular_value/min_sigular_value;
     }



    inline void TestMatrix()
    {
        Matrix<double> m1;
        m1.SetDimension(3, 3);

        Matrix<double>::row_iterator ri = m1.GetRow(1);

        ri[2] = 5.0;
        double d = ri[2];

        m1[0][0] = 1.0;
        m1[0][1] = 2.0;
        m1[0][2] = 3.0;

        m1[1][0] = 4.0;
        m1[1][1] = 5.0;
        m1[1][2] = 6.0;

        m1[2][0] = 8.0;
        m1[2][1] = 7.0;
        m1[2][2] = 9.0;

       
        m1.Print("m1=\n");
        std::cout<<"Cond(m1)="<<Cond(m1)<<std::endl;;



        //测试逆矩阵的计算
        Matrix<double> inv_m1;
        Inverse(m1, inv_m1);
        inv_m1.Print("inv_m1=\n");


        Matrix<double> I;
        I = inv_m1*m1;
        I.Print("I=\n");




        m1.Transpose().Print("m1.Transpose()\n");

        m1("0:2:2","0:2:2").Print("m1(0:2:2,0:2:2)\n");

        m1(":","1") = m1(":", "2");

        m1.Print("m1(:,1)=m2(:,2)\n");

        Matrix<double> m2;
        m2.SetDimension(3, 2);

        m2[0][0] = 1.0;
        m2[0][1] = 2.0;


        m2[1][0] = 4.0;
        m2[1][1] = 5.0;


        m2[2][0] = 6.0;
        m2[2][1] = 7.0;

        m2.Print("m2=\n");

        m1("0:2", "1:2") = m2;

        m1.Print("m1(\"0:2\", \"0:2\") = m2\n");


        Matrix<double> m3;
        m3 = m1("0:2", "1:2");
        m3.Print("m3=\n");


        Matrix<double> m4 =   m1*m2;


        //m3 = m1*m2
        m4.Print("m1*m2=\n");


        //求每列的平均值
        Matrix<double> mean1 = Mean(m1);
        mean1.Print();

        //求m2的第三行的均值
        double m2_r2 = Mean(m2[2]);
        std::cout<<"mean value of second row="<<m2_r2<<std::endl;

        //求m2的第2列的均值
        double m2_c1 = Mean( m2.GetCol(1));
        std::cout<<"mean value of first col="<<m2_c1<<std::endl;


        const Matrix<double>& refM2 = m2;

        m2_r2 = Mean(refM2[2]);
        std::cout<<"mean value of second row="<<m2_r2<<std::endl;

        m2_c1 = Mean(refM2.GetCol(1));
        std::cout<<"mean value of first col="<<m2_c1<<std::endl;


        //SVD测试
        //Matrix A
        const char* szMatrix="\
         8.79   9.93   9.83   5.45   3.16;\
         6.11   6.91   5.04  -0.27   7.98;\
         -9.15  -7.93   4.86   4.85   3.01;\
         9.57   1.64   8.83   0.74   5.80;\
        -3.49   4.02   9.80  10.00   4.27;\
         9.84   0.15  -8.99  -6.02  -5.31";

         Matrix<double> A(szMatrix);
         A.Print("A:\n");

         Matrix<double> S, U, V;
         SVD(A, &S, &U, &V);
         S.Print("S:\n");
         U.Print("U:\n");
         V.Print("V:\n");


    }
}

//Routines.obj : error LNK2019: unresolved external symbol "double __cdecl MatlabSimu::Cond(class MatlabSimu::Matrix<double> const &)" (?Cond@MatlabSimu@@YANABV?$Matrix@N@1@@Z) referenced in function
