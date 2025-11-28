#ifndef SORTVETEX_HEADER
#define SORTVETEX_HEADER

//@功能:点集中的点近似四边形分布, 传入的顶点数组事先是不知道谁先谁后的，
//      现在要将点集按沿四边形的边按顺序排列。
//
//@说明:模板参数类型TVertex中必须有POINT类型的成员变量ptCoord,它表示顶点在画面中的坐标。
//算法描述:n个顶点, 根据两点决定一条直线，共有n(n-1)条直线。
//
//
#include <math.h>



template<class T>
class CVertexSort
{
public:
    CVertexSort()
        :
    m_aryLineSegmentLength(NULL),
        m_nVerticesCount(0)
    {


    }

    ~CVertexSort()
    {
        delete [] m_aryLineSegmentLength;
        m_aryLineSegmentLength = NULL;

    }

    //@功能:将点集按照多边形排列
    BOOL operator() (T* vertices, int n, BOOL clockwise)
    {

        int nSegmentCount = (n*(n-1)) >> 1;

        if(m_nVerticesCount < n)
        {
            if(m_aryLineSegmentLength) delete [] m_aryLineSegmentLength;
            m_aryLineSegmentLength = new long[nSegmentCount];
        }

        m_nVerticesCount  = n;
        m_vecVertexInfo.resize(n);


        //计算所有线段的长度
        for(int K = 0; K < n - 1; K  ++)
        {
            const T& vertex_K = vertices[K];

            for(int L = K + 1;  L < n; L ++)
            {
                const T& vertex_L = vertices[L];

                long dx = vertex_L.ptCoord.x - vertex_K.ptCoord.x;
                long dy = vertex_L.ptCoord.y - vertex_K.ptCoord.y;

                long length_square = dx*dx + dy*dy;

                SetLineSegmentLength(K, L,length_square);

            }//for-each(l)

        }//for-each(k)


        //搜索每个结点距离其最近的M个结点
        //保留最有可能是其邻接点的两个结点。
        //const int M = 7;
        const int M = 11;

        TNeighbour neighbours[M];//按照距离升序排列的数组
        int neighbourCount = 0;

        for(int  K = 0; K < n; K++)
        {

            //初始化neighbour数据
            for(int I = 0; I < M; I++)
            {
                neighbours[I].distance = (std::numeric_limits<long>::max)();
                neighbours[I].nVertex  = -1;
            }
            neighbourCount = 0;

            for(int L = 0; L < n;  L++)
            {
                if(K == L) continue;//自己和自己不比较

                int distance = GetLineSegmentLength(K,L);

                if(neighbours[M-1].distance < distance)//距离大于距离最远的邻接点，显然不是邻接点的候选。
                {
                    continue;
                }

                int nInsertPos = 0;

                while(nInsertPos < M)
                {
                    if(distance < neighbours[nInsertPos].distance)
                    {
                        break;
                    }
                    nInsertPos ++;
                }

                if(nInsertPos == M)
                {
                    continue;
                }

                //插入排序
                for(int I = M-1; I > nInsertPos; I --)
                {
                    neighbours[I] = neighbours[I-1];

                }//for

                neighbours[nInsertPos].nVertex  = L;
                neighbours[nInsertPos].distance = distance;

                if(neighbourCount < M)
                {
                    neighbourCount ++;
                }

            }

            //计算矢量
            for(int I = 0; I < neighbourCount; I++)
            {
                int L = neighbours[I].nVertex;
                neighbours[I].Vx = vertices[L].ptCoord.x -  vertices[K].ptCoord.x;
                neighbours[I].Vy = vertices[L].ptCoord.y -  vertices[K].ptCoord.y;
            }

            //static double sin_theta_threshold = sin(15.0*M_PI /180.0);
            //static double sin_theta__square_threshold = sin_theta_threshold*sin_theta_threshold;
            static double sin_theta_threshold =  sin(15.0*M_PI /180.0);
            static long    cross_product_threshold =  long(100*sin_theta_threshold);
            static long    cross_product_square_threshold = cross_product_threshold*cross_product_threshold;
            static long    norm_square_product_threshold  = 10000;


            //合并位于同一方向的邻接点
            for(int I = 0; I < neighbourCount - 1; I++)
            {
                //long norm_vector_I_square = neighbours[I].Vx*neighbours[I].Vx +  neighbours[I].Vy*neighbours[I].Vy;
                double norm_vector_I = sqrt(double(neighbours[I].Vx*neighbours[I].Vx +  neighbours[I].Vy*neighbours[I].Vy));

                int J = I +1;
                while(J < neighbourCount)
                {
                    //计算矢量I,J的点积和叉积,
                    long dotProduct   = neighbours[I].Vx*neighbours[J].Vx + neighbours[I].Vy*neighbours[J].Vy;
                    long crossProduct = neighbours[I].Vx*neighbours[J].Vy - neighbours[I].Vy*neighbours[J].Vx;
                    
                    /*
                    long crossProduct_square = crossProduct*crossProduct;

                    long norm_vector_J_square =  neighbours[J].Vx*neighbours[J].Vx +  neighbours[J].Vy*neighbours[J].Vy;


                    if(dotProduct > 0 &&  __int64(crossProduct_square)*__int64(norm_square_product_threshold) < __int64(cross_product_square_threshold) * __int64(norm_vector_I_square) * __int64(norm_vector_J_square))
                  
                    //sin(theta) > sin_theta_threshold
                    //crossProdut=|Vi||Vj|sin(theta);
                    //
                    //sin(theta) = crossProdut/(|Vi||Vj|) <  sin_theta_threshold <==> crossProdut >=  sin_theta_threshold * |Vi| * |Vj|
                    //用dotProduct的符号>0来限定两矢量夹角属于[-pi/2, pi/2]
                    //用crossProduct的绝对值来判定是否两矢量的夹角来判定夹角小于设定的门限角
                    //当然也可以用dotProduct的值来判定， dotProduct值越大，则意味着两矢量夹角越大。              

			        */
					double norm_vector_J = sqrt(neighbours[J].Vx*neighbours[J].Vx +  neighbours[J].Vy*neighbours[J].Vy);
					
					//sin(theta) > sin_theta_threshold
					//crossProdut=|Vi||Vj|sin(theta);
					//
					//sin(theta) = crossProdut/(|Vi||Vj|) <  sin_theta_threshold <==> crossProdut >=  sin_theta_threshold * |Vi| * |Vj|
					//
					if(dotProduct > 0 && abs(crossProduct) < sin_theta_threshold*norm_vector_I*norm_vector_J)

                    {//位于同一方向，即夹角<15度
                        //移除J
                        for(int pos = J; pos < neighbourCount - 1; pos ++)
                        {
                            neighbours[pos] = neighbours[pos + 1];
                        }

                        neighbourCount --;
                    }
                    else
                    {
                        J ++;
                    }

                }//while(J)

            }//for(I)


            //以当前结点为原点, 按方位角排列各矢量。
            SortNeighbours(neighbours, neighbourCount);


            //在剩下的邻接点中, 以本结点为原点, 选取夹角最大的两个邻接点。
            //角度比较时, 为了取消浮点除法运算,推导算法如下
            //SGN(dot(A,B))*cos(A,B)^2 = SGN(dot(A,B))* [dot(A,B)^2]/[norm_A^2* norm_B^2];
            //SGN(dot(C,D))*cos(C,D)^2 = SGN(dot(C,D))* [dot(C,D)^2]/[norm_C^2 * norm_D^2];
            //不等式SGN(dot(A,B))*cos(A,B)^2  > SGN(dot(C,D))*cos(C,D)^2等价于
            //不等式
            //[SGN(dot(A,B))* [dot(A,B)^2]/[norm_A^2* norm_B^2] > SGN(dot(C,D))* [dot(C,D)^2]/[norm_C^2 * norm_D^2]
            //
            //进一步等价于
            //[SGN(dot(A,B))* [dot(A,B)^2]*[norm_C^2 * norm_D^2] > SGN(dot(C,D)) * [dot(C,D)^2]*[norm_A^2* norm_B^2]
            //所以需要记录夹角最小时的失量的点积平方和矢量的模。

            //__int64 dotProduct_Sign_Square_candidate = (std::numeric_limits<long>::max)();
            //__int64 norm_square_product_candidate = 0;
            
            int index[2];
            index[0] = index[1] = -1;

            int nNearestVectorCount = 0;
            if(neighbourCount < 2)
            {
                for(int I = 0; I < neighbourCount; I++)
                {
                    index[I] = neighbours[I].nVertex;
                }
                nNearestVectorCount = neighbourCount;
            }
            else
            {
                //候选两个邻接矢量的点积
                double  dotProduct_candidate = (std::numeric_limits<long>::max)();

                //选两个邻接矢量的模的乘积
                double  norm_product_candidate = 0;

                for(int I = 0; I <= neighbourCount - 1; I++)
                {
                    /*
                    __int64 norm_vector_I_square = neighbours[I].Vx*neighbours[I].Vx +  neighbours[I].Vy*neighbours[I].Vy;

                    int J = I + 1;//方位邻结点索引
                    if( J == neighbourCount)
                    {
                        J = 0;
                    }

                    __int64 norm_vector_J_square =  neighbours[J].Vx*neighbours[J].Vx +  neighbours[J].Vy*neighbours[J].Vy;
                    __int64 dotProduct   = neighbours[I].Vx*neighbours[J].Vx + neighbours[I].Vy*neighbours[J].Vy;

                    __int64 dotProduct_Sign_Square = dotProduct*dotProduct*((dotProduct > 0)?1:-1);

                    //在12800*720P情形下,容易造成溢出
                    //if( (__int64(dotProduct_Sign_Square) * __int64(norm_square_product_candidate)) < __int64(dotProduct_Sign_Square_candidate) * __int64(norm_vector_I_square) * __int64(norm_vector_J_square))
                    {
                        dotProduct_Sign_Square_candidate = dotProduct_Sign_Square;

                        norm_square_product_candidate = norm_vector_I_square*norm_vector_J_square;

                        index[0] = neighbours[I].nVertex;
                        index[1] = neighbours[J].nVertex;

                        nNearestVectorCount = 2;
                    }
                    */

                    double norm_vector_I = sqrt(double(neighbours[I].Vx*neighbours[I].Vx + neighbours[I].Vy*neighbours[I].Vy));

                    int J = I + 1;//方位邻结点索引
                    if (J == neighbourCount)
                    {
                        J = 0;
                    }

                    double norm_vector_J = sqrt(double(neighbours[J].Vx*neighbours[J].Vx + neighbours[J].Vy*neighbours[J].Vy));

                    double dotProduct = neighbours[I].Vx*neighbours[J].Vx + neighbours[I].Vy*neighbours[J].Vy;

                    //cosine函数在[0,pi]上单调递减函数
                    //cos(t1)<cos(t0), 则t1 > t0
                    //
                    if(dotProduct*norm_product_candidate < dotProduct_candidate*norm_vector_I*norm_vector_J)
                    {
                        dotProduct_candidate = dotProduct;

                        norm_product_candidate = norm_vector_I*norm_vector_J;

                        index[0] = neighbours[I].nVertex;
                        index[1] = neighbours[J].nVertex;

                        nNearestVectorCount = 2;
                    }


                }//for(I)
            }//else

            for(int I = 0; I < nNearestVectorCount; I++)
            {
                m_vecVertexInfo[K].aryNeighborVertices[I ] = index[I];
            }

            m_vecVertexInfo[K].nNeighborCount = nNearestVectorCount;


        }//for(K)

        std::vector<int> vecIndex;
        vecIndex.resize(n);

        int nInitVertex   = 0;
        int nPrevVertexIndex = nInitVertex;
        
        int nStep = 0;
        vecIndex[nStep] = nPrevVertexIndex;
        nStep ++;

        if(m_vecVertexInfo[nPrevVertexIndex].nNeighborCount < 2)
        {
            return FALSE;
        }
        int nCurrentVertex = m_vecVertexInfo[nPrevVertexIndex].aryNeighborVertices[0];

        do
        {
             vecIndex[nStep] = nCurrentVertex;
             nStep ++;

            if(m_vecVertexInfo[nCurrentVertex].nNeighborCount < 2)
            {
                break;
            }
            if(m_vecVertexInfo[nCurrentVertex].aryNeighborVertices[0] == nPrevVertexIndex)
            {
                nPrevVertexIndex = nCurrentVertex;
                nCurrentVertex   = m_vecVertexInfo[nCurrentVertex].aryNeighborVertices[1];
            }
            else if(m_vecVertexInfo[nCurrentVertex].aryNeighborVertices[1] == nPrevVertexIndex)
            {
                nPrevVertexIndex = nCurrentVertex;
                nCurrentVertex   = m_vecVertexInfo[nCurrentVertex].aryNeighborVertices[0];
            }
            else
            {//两个结点不互为最近邻接点, 前后不相关。
               break;
             }
            
        }while(nStep < n);

        //结束后起点和终点应该一致
        if(nCurrentVertex != nInitVertex)
        {
            return FALSE;
        }


        BOOL  bSortInClockwise = IsClockwise(vertices, &vecIndex[0], n);

        BOOL bReverse = FALSE;
        if(clockwise ^ bSortInClockwise)
        {
            bReverse = TRUE;
        }

        //备份原有的输入数据
        std::vector<T> vecTemp;
        vecTemp.resize(n);
        for(int I = 0; I < n; I++)
        {
            vecTemp[I] = vertices[I];
        }
        
        //输出排序后的数据
        for(int I = 0; I < n; I++)
        {
            int  index = bReverse?vecIndex[n-1 - I]: vecIndex[I];

            vertices[I] = vecTemp[index];
        }

        return TRUE;
    }


protected:
    //@功能:判断结点的排列顺序是否是顺时针排列。
    //@参数:pVertices, 结点数组
    //      pSequence, 排列的结点编号数组
    //      n, 结点个数
    BOOL IsClockwise(const T* pVertices, int* pSequence, int n)
    {

        //计算结点重心
        long sum_x  = 0;
        long sum_y  = 0;

        for(int I = 0; I < n; I ++)
        {
            sum_x += pVertices[I].ptCoord.x;
            sum_y += pVertices[I].ptCoord.y;
        }
        
        
        POINT ptG;//力矩
        ptG.x = sum_x/n;
        ptG.y = sum_y/n;

        
        
        //将前后结点矢量看作力的矢量
        //将重心到结点的矢量, 
        //计算对重心的力矩之和, 其正负号表示方向,
        //> 0, 逆时针
        //< 0, 顺时针
        int M = 0;
        for(int I = 0; I < n; I++)
        {
            int vertexIndex = pSequence[I];

            long force_x = pVertices[vertexIndex].ptCoord.x - ptG.x;
            long force_y = pVertices[vertexIndex].ptCoord.y - ptG.y;

            int nNextVertexIndex = pSequence[I + 1];
            if(I == n-1)
            {
                nNextVertexIndex = pSequence[0];
            }
            
            long r_x = pVertices[nNextVertexIndex].ptCoord.x - pVertices[vertexIndex].ptCoord.x;
            long r_y = pVertices[nNextVertexIndex].ptCoord.y - pVertices[vertexIndex].ptCoord.y;

            //M = R×F

            M += (r_x*force_y - r_y*force_x);
            
            
        }//for-each(I)
        

        return M>0?FALSE:TRUE;
    }

    int GetLineSegmentLength(int  K, int L) const
    {
        if(K == L) return  -1;//线段两端的结点编号不能相同

        //保证:k < l
        if(K > L)
        {
            int tmp = K;
            K = L;
            L = tmp;
        }

        //  0 1 2 3 4
        //0 □■■■■
        //1 □□■■■
        //2 □□□■■
        //3 □□□□■
        //4 □□□□□
        //
        int n = m_nVerticesCount;
        int offset = 0;

        for(int r = 0; r < K; r++)
        {
            offset +=  n - r -1;
        }
        offset +=  (L - (K + 1));

        return m_aryLineSegmentLength[offset];

    }



    void SetLineSegmentLength(int K, int L, long length)
    {
        if(K == L) return ;//线段两端的结点编号不能相同

        //保证:k < l
        if(K > L)
        {
            int tmp = K;
            K = L;
            L = tmp;
        }

        //  0 1 2 3 4
        //0 □■■■■
        //1 □□■■■
        //2 □□□■■
        //3 □□□□■
        //4 □□□□□
        //
        int n = m_nVerticesCount;
        int offset = 0;

        for(int r = 0; r < K; r++)
        {
            offset +=  n - r -1;
        }
        offset +=  (L - (K + 1));

        m_aryLineSegmentLength[offset] = length;

    }


    //@功能:判断2失量是否共线
    BOOL IsCollineation(const POINT& left, const POINT& middle, const POINT& right)
    {
        //以中心为原点，计算失量
        int v1_x = left.x - middle.x;
        int v1_y = left.y - middle.y;
        int v1_norm_square = v1_x*v1_x + v1_y*v1_y;

        int v2_x = right.x - middle.x;
        int v2_y = right.y - middle.y;
        int v2_norm_square = v2_x*v2_x + v2_y*v2_y;

        if(v1_norm_square  == 0 || v2_norm_square == 0) return FALSE;

        //计算失量叉积
        int cross_product = v1_x * v2_y - v1_y*v2_x;
        int cross_product_squre = cross_product*cross_product;


        //计算失量点积
        int dot_product = v1_x*v2_x + v1_y*v2_y;

        static double sin_15_square = sin(15.0*M_PI/180.0)*sin(15.0*M_PI/180.0);

        //点积为负，表明失量夹角大于90度
        if(dot_product < 0)
        {
            double sin_theta_square = (double)cross_product_squre/double(v1_norm_square * v2_norm_square);

            if(sin_theta_square < sin_15_square)
            {
                return TRUE;
            }
        }

        return FALSE;
    }


     //@功能:返回失量所在象限的编号(0~3),正方向取逆时针方向
     inline int GetQuadrant(int nVx, int nVy)
     {
         int nQudarant = 0;

         if(nVy >0)
         {
            if(nVx > 0) nQudarant = 0;
            else        nQudarant = 1;
         }
         else if(nVy < 0)
         {
            if(nVx < 0) nQudarant = 2;
            else        nQudarant = 3;
         }
         else if(nVy == 0)
         {
            if(nVx >= 0) nQudarant = 0;
            else        nQudarant  = 2;
         }
         return nQudarant;
     }

    struct TNeighbour
    {
        long distance;
        int nVertex;
        long Vx,Vy;//以当前结点为原点时的矢量
    };

    //@功能:按照方位排列邻接点
    //@参数:neighbours,邻接点数组
    //      nCount, 邻接点个数
    void SortNeighbours(TNeighbour* neighbours, int nCount)
    {
        //采用插入排序方法
        for(int I = 1; I < nCount; I++)
        {
            int Vx_I = neighbours[I].Vx;
            int Vy_I = neighbours[I].Vy;

            int quadrant_I = GetQuadrant(Vx_I, Vy_I);

            int nInsertPos = 0;
           for( nInsertPos = 0; nInsertPos <= I -1; nInsertPos ++)
           {
                int Vx_cmp =  neighbours[nInsertPos].Vx;
                int Vy_cmp =  neighbours[nInsertPos].Vy;
                int quadrant_cmp = GetQuadrant(Vx_cmp, Vy_cmp);

                if(quadrant_I >  quadrant_cmp)
                {//比较象限大小
                    continue;
                }
                else if(quadrant_I == quadrant_cmp)
                {
                    //计算叉积的正负来判断方位角的大小
                    int cross_product  =  Vx_cmp * Vy_I - Vy_cmp*Vx_I;
                    
                    if(cross_product >=  0)
                    {//V_I的方位角大于V_cmp
                        continue;
                    }
                    else
                    {
                        break;
                    }

                }
                else if(quadrant_I < quadrant_cmp)
                {//
                    break;
                }


           }//for

           if(I == nInsertPos) continue;
           
           TNeighbour backup = neighbours[I];

           //从[nInerstPos...I-1]往后挪动一个单元
           for(int J = I; J >nInsertPos ; J --)
           {
                neighbours[J] = neighbours[J - 1];
           }

           neighbours[nInsertPos] = backup;
        }

    }

    struct TVertexInfo //结点信息
    {
        int aryNeighborVertices[2];//记录结点邻接点的数组。
        int nNeighborCount;
    };



    std::vector<TVertexInfo>  m_vecVertexInfo;
    LONG* m_aryLineSegmentLength;//记录所有线段长度的数组
    int   m_nVerticesCount;

};


//@功能:在已经有序的结点数组中搜索拐点
//@参数:pVertices, 结点数组
//      nVerticeCount, 结点个数
//      pTurnPoints, 拐点数组
//      nTurnPoints, 拐点数组元素个数
template<class TVertex, class TTurnPoint>

int SearchTurnPoint(const TVertex* pVertices, int nVerticeCount, TTurnPoint* pTurnPoints, int nTurnPointBuf)
{
    int nFound = 0;

    for(int I = 0; I < nVerticeCount; I ++)
    {
        int iPrevIndex, iNextIndex;

        if(I == 0)
        {
            iPrevIndex = nVerticeCount - 1;
        }
        else
        {
            iPrevIndex =  I - 1;
        }

        if(I == nVerticeCount - 1)
        {
            iNextIndex = 0;
        }
        else
        {
            iNextIndex = I + 1;
        }

        POINT ptCurrent = pVertices[I].ptCoord;
        POINT ptPrev    = pVertices[iPrevIndex].ptCoord;
        POINT ptNext    = pVertices[iNextIndex].ptCoord;

        long v1_dx, v1_dy, v2_dx, v2_dy;

        //失量v1
        v1_dx = ptCurrent.x - ptPrev.x ;
        v1_dy = ptCurrent.y - ptPrev.y;

        //失量v2
        v2_dx = ptNext.x - ptCurrent.x;
        v2_dy = ptNext.y - ptCurrent.y;

        
        //
        long cross_product = v1_dx * v2_dy - v1_dy * v2_dx;

        long dot_product = v1_dx*v2_dx + v1_dy*v2_dy;

        //V1×V2 = |V1||V2|sinθ
        //=>sinθ^2 = (V1×V2)^2 /[(|V1||V2|)^2]
        //注意此处防止整型计算溢出错误,2013/04/11
        long v1_length_sqare = v1_dx * v1_dx + v1_dy * v1_dy;
        long v2_length_sqare = v2_dx * v2_dx + v2_dy * v2_dy;

        //double sin_v1_v2_square = double(cross_product * cross_product)/double(v1_dx * v1_dx + v1_dy * v1_dy)/double(v2_dx * v2_dx + v2_dy * v2_dy);
        double sin_v1_v2_square = (double(cross_product)/double(v1_length_sqare))*(double(cross_product)/double(v2_length_sqare));


        //const double sin_30_square = 0.5*0.5;
        const double sin20 = sin(20.0/180.0 * M_PI);
        const double sin20_square = sin20 * sin20;

        //当矢量夹角在[340, 360]和[0,20]之间时, 则认为是在边上的校正点,不是角上的点。
        if( dot_product > 0 //保证在第一、四象限
            &&
            sin_v1_v2_square < sin20_square)
        {
            //为在边上的校正点
        }
        else
        {
            if(nFound < nTurnPointBuf)
            {
                //防止在异常情况下内存访问越界
                pTurnPoints[nFound].ptCoord = pVertices[I].ptCoord;
                pTurnPoints[nFound].nLayerPtIndex = I;
                nFound++;
            }

        }//else

    }//for(I)

    return nFound;
}



#endif