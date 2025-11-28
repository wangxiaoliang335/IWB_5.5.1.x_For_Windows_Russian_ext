/* Calling Lapack from C */

/* Solves A = LU;compiled as gcc -o dgetrf dgetrf.c -llapack  */

/*   DGETRF  computes  an  LU factorization of a general M-by-N matrix A using partial pivoting with row interchanges.
     The factorization has the form A = P * L * U where P is the permutation matrix, L is lower triangular with unit diagonal elements
     (lower trapezoidal if m > n), and U is upper tri-angular (upper trapezoidal if m < n) . */



#include <math.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdio>
#include <time.h>

#define MAX 100000


#define DYNAMIC_VECTOR(Q,nrows)\
double* Q = NULL;\
Q = new double[nrows];


void count_rows_columns(int *rows, int *columns, FILE *fp,  double *Q);


using namespace std;

clock_t start, end;
double elapsed;

FILE *fpA;
char filename[10][FILENAME_MAX];

void print_matrix(const double* A, int m, int n);
void count_rows_columns(int *rows, int *columns, FILE *fp,  double *Q);



extern "C" {
    // LU decomoposition of a general matrix
    void dgetrf_(int* M, int *N, double* A, int* lda, int* IPIV, int* INFO);

    // generate inverse of a matrix given its LU decomposition
    void dgetri_(int* N, double* A, int* lda, int* IPIV, double* WORK, int* lwork, int* INFO);
}


void inverse(double* A, int N)
{
    int *IPIV = new int[N+1];
    int LWORK = N*N;
    double *WORK = new double[LWORK];
    int INFO;

    dgetrf_(&N,&N,A,&N,IPIV,&INFO);

    dgetri_(&N,A,&N,IPIV,WORK,&LWORK,&INFO);

    delete IPIV;
    delete WORK;
}


int main()
{

   printf("\n matrix_inverse.cpp  ver 1.0  November 22, 2011" );
   printf("\n by Tom Irvine  Email: tomirvine@aol.com ") ;
   printf("\n ");
   printf("\n This program calculates the inverse of a square matrix. \n");

   int Arows,Acols;
   int i,j;

   DYNAMIC_VECTOR(a,MAX)

   while(1)
   {
       printf("\n Enter the input A matrix filename \n");
       scanf("%s",filename[0]);
       fpA = fopen(filename[0], "rb");

       if(fpA != NULL){break;}

       printf("\n Warning: File not found. \n");
   }
   count_rows_columns(&Arows, &Acols, fpA, a);


    if(Arows != Acols)
    {
        printf("\n Error:  input matrix must be square ");
        exit(1);
    }

    int size=Arows;

    DYNAMIC_VECTOR(A,(size*size))

    printf("\n");

    for(i=0;i<(size*size);i++)
    {
        A[i]=a[i];
    }

    delete [] a;


    printf("\n original matrix \n\n");


    long nv=0;

    for (i=0;i<size;i++)
    {
        for(j=0;j<size;j++)
        {
            printf("%10.4lf",A[nv]);
            nv++;
        }
        printf("\n");
    }
    printf("\n\n");


    start = clock();

    inverse(A, size);

    end = clock();
    elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;


    printf("\n inverse matrix \n\n");

    nv=0;

    for (i=0;i<size;i++)
    {
        for(j=0;j<size;j++)
        {
            printf("%10.4lf",A[nv]);
            nv++;
        }
        printf("\n");
    }
    printf("\n\n");

    printf("\n elapsed cpu time = %8.4g sec \n",elapsed);

    delete [] A;


}
void count_rows_columns(int *rows, int *columns, FILE *fp,  double *Q)
{

   long numBytes = 10000;

   long num,numb;


   char string[numBytes];

   char *p;
   char *pb;

   char *q;


    fgets(string,numBytes,fp );

//    printf("\n %s \n",string);

     pb = strchr(string,' ');
     p  = strchr(string,' ');

     (*columns)=0;

     numb=0;


    for(int i=1;i<strlen(string);i++)
    {
        if(string[i]==' ' && string[i-1]!=' ')
        {
            (*columns)++;
        }
    }

    p = strrchr( string, ' ' );

    long iflag=0;

    q = strrchr( string, '.' );

    long PP = (long) p;
    long QQ = (long) q;

    if(QQ>PP)
    {
       (*columns)++;

       iflag=1;
    }
    if(iflag==0)
    {
       for(long i=0;i<=9;i++)
       {
           char c=i;

           q = strrchr( string, c );

            long QQ = (long) q;

            if(QQ>PP)
            {
                (*columns)++;

                iflag=1;
                break;
            }
       }
    }

   rewind(fp);


   double ffff;

   long i=0;

   printf("\n");

	while( fscanf(fp, "%lf ", &ffff) > 0 )
	{
	    Q[i]=ffff;

//	    printf(" Q[%ld]=%8.4g  \n",i,Q[i]);

		i++;

		if(i==MAX)
		{
			printf("\n Error: maximum cell limit reached. \n");

			exit(1);
		}

	}

    long cells=i;


    (*rows)=int(double(cells)/double((*columns)));

    printf("\n  cells=%ld  columns=%d  rows=%d \n",cells,*columns,*rows);

    long nv=0;

// convert from row to column major


}
