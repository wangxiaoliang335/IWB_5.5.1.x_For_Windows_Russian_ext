// Copyright (C) 2009 foam
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// from http://www.public.iastate.edu/~dicook/JSS/paper/paper.html

/************************************************************
*                                                          *
*  Permission is hereby granted  to  any  individual   or  *
*  institution   for  use,  copying, or redistribution of  *
*  this code and associated documentation,  provided       *
*  that   such  code  and documentation are not sold  for  *
*  profit and the  following copyright notice is retained  *
*  in the code and documentation:                          *
*     Copyright (c) held by Dianne Cook                    *
*  All Rights Reserved.                                    *
*                                                          *
*  Questions and comments are welcome, and I request       *
*  that you share any modifications with me.               *
*                                                          *
*                Dianne Cook                               *
*             dicook@iastate.edu                           *
*                                                          *
************************************************************/

/*
* svdcomp - SVD decomposition routine.
* Takes an mxn matrix a and decomposes it into udv, where u,v are
* left and right orthogonal transformation matrices, and d is a
* diagonal matrix of singular values.
*
* This routine is adapted from svdecomp.c in XLISP-STAT 2.1 which is
* code from Numerical Recipes adapted by Luke Tierney and David Betz.
*
* Input to dsvd is as follows:
*   a = mxn matrix to be decomposed, gets overwritten with u
*   m = row dimension of a
*   n = column dimension of a
*   w = returns the vector of singular values of a
*   v = returns the right orthogonal transformation matrix
*/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Matrix.h"
#define SIGN(a, b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
#define MAX(x,y) ((x)>(y)?(x):(y))

//The Pythagorean Theorem, 毕达哥拉斯定理,即勾股定理
//
//Computes sqrt(a*a + b*b) without destructive underflow or overflow
//
//@return:sqrt(a*a + b*b)
//@comments:
//If both a and b are very large, the risk of destructive overflow may happen
//If botn a and b are very small, the risk of destructive underlow may happen
//overflow example:
// double a = (std::numeric_limits<double>::max)() * 0.1;
// double b = (std::numeric_limits<double>::max)() * 0.1;
// double result = sqrt(a*a + b*b); /*result = 1.#INF000000000000*/
// result = PYTHAG(a,b); /*result=2.5423220123072925e+307*/



inline static double PYTHAG(double a, double b)
{
	double at = fabs(a), bt = fabs(b), ct, result;

	if (at > bt)
	{
		ct = bt / at; 
		result = at * sqrt(1.0 + ct * ct); 
	}
	else if (bt > 0.0) 
	{
		ct = at / bt; 
		result = bt * sqrt(1.0 + ct * ct); 
	}
	else 
	{
		result = 0.0;
	}
	return(result);
}


//Vector<float> SVD(Matrix<float> &m)
//{
//        Vector<float> w(m.GetRows());
//        Matrix<float> v(m.GetRows(),m.GetCols());
//        dsvd(m, m.GetRows(), m.GetCols(), w.GetRawData(), v);
//        return w;
//}
//
//
//@params:
//        
//
//
//
//@comments:
// A = UWV
// RowSize(A) = m, ColSize(A) = n
//
//因为输入参数矩阵A的纬度为m×n
//参数a作为输入输出参数,输出的矩阵U的纬度为m×m
//又因为 m > n, 所以范围在a[1...m][1...n]的元素
//在调用函数应以矩阵A的内容赋值
//
//
//
template< int m, int n>
bool dsvd(Matrix<double,m,m> &a, double *w, Matrix<double, n,n> &v)
{
	int flag, i, its, j, jj, k, l, nm;
	double c, f, h, s, x, y, z;
	double anorm = 0.0, g = 0.0, scale = 0.0;
	//<<begin
	//modified by toxuke@gmail.com, 2011/07/13
	//double* rv1;
	//end>>
	double  rv1[n];

	if (m < n)
	{
		fprintf(stderr, "#rows must be > #cols \n");
		return(0);
	}

	//<<begin
	//commented out by toxuke@gmail.com, 2011/07/13
	//rv1 = (double *)malloc((unsigned int) n*sizeof(double));
	//end>>

	/* Householder reduction to bidiagonal form */
	for (i = 0; i < n; i++)
	{
		/* left-hand reduction */
		l = i + 1;
		rv1[i] = scale * g;
		g = s = scale = 0.0;
		if (i < m)
		{
			//第i列元素的模和
			for (k = i; k < m; k++)
				scale += fabs((double)a[k][i]);
			if (scale)//该列不为0
			{
				for (k = i; k < m; k++)
				{
					a[k][i] = (float)((double)a[k][i]/scale);
					s += ((double)a[k][i] * (double)a[k][i]);
				}
				f = (double)a[i][i];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i][i] = (float)(f - g);
				if (i != n - 1)
				{
					for (j = l; j < n; j++)
					{
						for (s = 0.0, k = i; k < m; k++)
							s += ((double)a[k][i] * (double)a[k][j]);
						f = s / h;
						for (k = i; k < m; k++)
							a[k][j] += (float)(f * (double)a[k][i]);
					}
				}
				for (k = i; k < m; k++)
					a[k][i] = (float)((double)a[k][i]*scale);
			}
		}
		w[i] = (float)(scale * g);

		/* right-hand reduction */
		g = s = scale = 0.0;
		if (i < m && i != n - 1)
		{
			for (k = l; k < n; k++)
				scale += fabs((double)a[i][k]);
			if (scale)
			{
				for (k = l; k < n; k++)
				{
					a[i][k] = (float)((double)a[i][k]/scale);
					s += ((double)a[i][k] * (double)a[i][k]);
				}
				f = (double)a[i][l];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i][l] = (float)(f - g);
				for (k = l; k < n; k++)
					rv1[k] = (double)a[i][k] / h;
				if (i != m - 1)
				{
					for (j = l; j < m; j++)
					{
						for (s = 0.0, k = l; k < n; k++)
							s += ((double)a[j][k] * (double)a[i][k]);
						for (k = l; k < n; k++)
							a[j][k] += (float)(s * rv1[k]);
					}
				}
				for (k = l; k < n; k++)
					a[i][k] = (float)((double)a[i][k]*scale);
			}
		}
		anorm = MAX(anorm, (fabs((double)w[i]) + fabs(rv1[i])));
	}

	/* accumulate the right-hand transformation */
	for (i = n - 1; i >= 0; i--)
	{
		if (i < n - 1)
		{
			if (g)
			{
				for (j = l; j < n; j++)
					v[j][i] = (float)(((double)a[i][j] / (double)a[i][l]) / g);
				/* double division to avoid underflow */
				for (j = l; j < n; j++)
				{
					for (s = 0.0, k = l; k < n; k++)
						s += ((double)a[i][k] * (double)v[k][j]);
					for (k = l; k < n; k++)
						v[k][j] += (float)(s * (double)v[k][i]);
				}
			}
			for (j = l; j < n; j++)
				v[i][j] = v[j][i] = 0.0;
		}
		v[i][i] = 1.0;
		g = rv1[i];
		l = i;
	}

	/* accumulate the left-hand transformation */
	for (i = n - 1; i >= 0; i--)
	{
		l = i + 1;
		g = (double)w[i];
		if (i < n - 1)
			for (j = l; j < n; j++)
				a[i][j] = 0.0;
		if (g)
		{
			g = 1.0 / g;
			if (i != n - 1)
			{
				for (j = l; j < n; j++)
				{
					for (s = 0.0, k = l; k < m; k++)
						s += ((double)a[k][i] * (double)a[k][j]);
					f = (s / (double)a[i][i]) * g;
					for (k = i; k < m; k++)
						a[k][j] += (float)(f * (double)a[k][i]);
				}
			}
			for (j = i; j < m; j++)
				a[j][i] = (float)((double)a[j][i]*g);
		}
		else
		{
			for (j = i; j < m; j++)
				a[j][i] = 0.0;
		}
		++a[i][i];
	}

	/* diagonalize the bidiagonal form */
	for (k = n - 1; k >= 0; k--)
	{                             /* loop over singular values */
		for (its = 0; its < 30; its++)
		{                         /* loop over allowed iterations */
			flag = 1;
			for (l = k; l >= 0; l--)
			{                     /* test for splitting */
				nm = l - 1;
				if (fabs(rv1[l]) + anorm == anorm)
				{
					flag = 0;
					break;
				}
				if (fabs((double)w[nm]) + anorm == anorm)
					break;
			}
			if (flag)
			{
				c = 0.0;
				s = 1.0;
				for (i = l; i <= k; i++)
				{
					f = s * rv1[i];
					if (fabs(f) + anorm != anorm)
					{
						g = (double)w[i];
						h = PYTHAG(f, g);
						w[i] = (float)h;
						h = 1.0 / h;
						c = g * h;
						s = (- f * h);
						for (j = 0; j < m; j++)
						{
							y = (double)a[j][nm];
							z = (double)a[j][i];
							a[j][nm] = (float)(y * c + z * s);
							a[j][i] = (float)(z * c - y * s);
						}
					}
				}
			}
			z = (double)w[k];
			if (l == k)
			{                  /* convergence */
				if (z < 0.0)
				{              /* make singular value nonnegative */
					w[k] = (float)(-z);
					for (j = 0; j < n; j++)
						v[j][k] = (-v[j][k]);
				}
				break;
			}
			if (its >= 30) {
				//<<begin
				//commented out by toxuke@gmail.com
				//free((void*) rv1);
				//fprintf(stderr, "No convergence after 30,000! iterations \n");
				//end>>
				return(0);
			}

			/* shift from bottom 2 x 2 minor */
			x = (double)w[l];
			nm = k - 1;
			y = (double)w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
			g = PYTHAG(f, 1.0);
			f = ((x - z) * (x + z) + h * ((y / (f + SIGN(g, f))) - h)) / x;

			/* next QR transformation */
			c = s = 1.0;
			for (j = l; j <= nm; j++)
			{
				i = j + 1;
				g = rv1[i];
				y = (double)w[i];
				h = s * g;
				g = c * g;
				z = PYTHAG(f, h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y = y * c;
				for (jj = 0; jj < n; jj++)
				{
					x = (double)v[jj][j];
					z = (double)v[jj][i];
					v[jj][j] = (float)(x * c + z * s);
					v[jj][i] = (float)(z * c - x * s);
				}
				z = PYTHAG(f, h);
				w[j] = (float)z;
				if (z)
				{
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = (c * g) + (s * y);
				x = (c * y) - (s * g);
				for (jj = 0; jj < m; jj++)
				{
					y = (double)a[jj][j];
					z = (double)a[jj][i];
					a[jj][j] = (float)(y * c + z * s);
					a[jj][i] = (float)(z * c - y * s);
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = (float)x;
		}
	}

	//<<begin
	//commented out by toxuke@gmail.com
	//free((void*) rv1);
	//end>>
	reorder(a, w, v);
	return(1);
}


//@功能:对特征值
template< int m, int n>
void reorder(Matrix<double,m,m> &u, double *w, Matrix<double, n,n> &v)
{                                                                           
	/*Given the output of decompose, this routine sorts the singular values, and corresponding columns
	of u and v, by decreasing magnitude. Also, signs of corresponding columns are flipped so as to
	maximize the number of positive elements. */      
	int i,j,k,s,inc=1;                                                                              
	double sw;                                                                                        
	
	double su[m],sv[n];
	
	//希尔排序
	//步长序列
	//1, 4, 13, 40, ...
	do { inc *= 3; inc++; } while (inc <= n);   //Sort. The method is Shell's sort.                  
												//(The work is negligible as compared                                                             
												//to that already done in                                                                         
												//decompose.)                                                                                     
	do 
	{                                                                                            
		inc /= 3;//inc 步长                                                                                       
		for (i=inc;i<n;i++) 
		{                                                                           
			sw = w[i]; 
			
			//预先保存U的第i列, 第i列数据在插入排序时会被冲毁
			for (k=0;k<m;k++) su[k] = u[k][i];                                                              
			
			//预先保存V的第i列,, 第i列数据在插入排序时会被冲毁
			for (k=0;k<n;k++) sv[k] = v[k][i];                                                              

			j = i;   

			//按照从大到小进行插入排序			
			while (w[j-inc] < sw) 
			{
				//
				w[j] = w[j-inc];                                                                                
				
				//矩阵U的第j列=第(j-inc)列
				for (k=0;k<m;k++) u[k][j] = u[k][j-inc];                                                        
				
				//矩阵v的第j列=(j-inc)列
				for (k=0;k<n;k++) v[k][j] = v[k][j-inc];                                                        

				j -= inc;//往前遍历子列                                                                                       
				if (j < inc) break;                                                                             
			}//while                      

			w[j] = sw;                                                                                 

			//将第i列数据,放到找到的列插入位置j处
			for (k=0;k<m;k++) u[k][j] = su[k];                                                              

			for (k=0;k<n;k++) v[k][j] = sv[k];                                                              
		}//                            

	} while (inc > 1); //对每个步长循环


	//Flip signs
	//保证矩阵U,V中>0的元素最多。
	//1...n列
	for (k=0;k<n;k++) 
	{ 
		s=0;                                                                                            
		for (i=0;i<m;i++) if (u[i][k] < 0.) s++;                                                        
		for (j=0;j<n;j++) if (v[j][k] < 0.) s++;                                                        
		if (s > (m+n)/2) 
		{                                                                              
			for (i=0;i<m;i++) u[i][k] = -u[i][k];                                                           
			for (j=0;j<n;j++) v[j][k] = -v[j][k];                                                           
		}                                                                                               
	}                                                                                               
}  

//@功能:计算广义逆矩阵
//@方法:矩阵A的广义逆矩阵的计算方法,
//      1.对A进行SVD分解
//        A=UΣV
//
//      2.A的广义逆矩阵
//        A+=V*[Σ(-1)]*U'
//
//
//注意:m>n
//因为a
template< int m, int n>
bool pinv(const Matrix<double,m,n> &a,  Matrix<double,n,m> &X, double epsilon)
{
	double w[m];
	memset(&w, 0, sizeof(w));

	
	//输入输出参数
	Matrix<double, m, m> u;
	
	for(int i=0; i<m; i++)
		for(int j=0; j<n; j++)	u[i][j] = a[i][j];

	//输出参数
	Matrix<double, n, n> v;

   if(!dsvd(u, w, v))
   {
		return 0;
   }


   for( int i = 0; i < m; i++ )
   {
      if( fabs(w[i]) > epsilon )
                w[i] = 1.0 /  w[i];
        else w[i] = 0.0;
    }

   Matrix<double, n, m> D_Inverse;

   for(int i=0; i<n; i++)
   {
		D_Inverse[i][i] = w[i];
   }
     
	Matrix<double, m, m> u_t= u.Transpose();//u的转置矩阵

	X = v * D_Inverse * u_t;

   return 1;
}