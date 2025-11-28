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
#include <bitset>
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

        if(n <= 1) return FALSE;
        
        int nSegmentCount = (n*(n-1)) >> 1;

        if(m_nVerticesCount < n)
        {
            if(m_aryLineSegmentLength) delete [] m_aryLineSegmentLength;
            m_aryLineSegmentLength = new long[nSegmentCount];
        }

        m_nVerticesCount  = n;

        m_vecVertexInfo.resize(n);

        //计算所有结点之间的距离
        for (int K = 0; K < n - 1; K++)
        {
            const T& vertex_K = vertices[K];

            for (int L = K + 1; L < n; L++)
            {
                const T& vertex_L = vertices[L];

                long dx = vertex_L.ptCoord.x - vertex_K.ptCoord.x;
                long dy = vertex_L.ptCoord.y - vertex_K.ptCoord.y;

                long length_square = dx*dx + dy*dy;

                SetLineSegmentLength(K, L, length_square);

            }//for-each(l)

        }//for-each(k)




        TVertexInfo* pVertexInfo = &m_vecVertexInfo[0];
        
        //寻找第0个结点的最近两个邻结点。
        SearchTwoNeighbours(vertices, 0, pVertexInfo);
        
        if(pVertexInfo->nNeighborCount < 2) return FALSE;

        int nProcessCount = 1;

        //寻找邻结点的邻接点
        int nCurrentVertex;
        int nEndVerTex = pVertexInfo->aryNeighborVertices[0];
        
        nCurrentVertex = pVertexInfo->aryNeighborVertices[1];

        //当前结点的一个已知邻结点结点是0
        int nAnotherNeightborVertex = 0;
        
        pVertexInfo  = &m_vecVertexInfo[nCurrentVertex];
        
        //标记可用候选结点的标志数组
        std::vector<bool> vecNeighourCandidates;
        vecNeighourCandidates.resize(n);
        for(int i = 0; i < n; i++)
        {
            vecNeighourCandidates[i] = true;
        }

        vecNeighourCandidates[0] = false;

        while (nProcessCount < n )
        {
            pVertexInfo->aryNeighborVertices[0] = nAnotherNeightborVertex;
            pVertexInfo->aryNeighborVertices[1] = -1;
            pVertexInfo->nNeighborCount = 1;

            SearchNextNeighbour(vertices, nCurrentVertex, pVertexInfo, vecNeighourCandidates);
            
            vecNeighourCandidates[nCurrentVertex] = false;
             
            nProcessCount++;

            nAnotherNeightborVertex = nCurrentVertex;
            nCurrentVertex = pVertexInfo->aryNeighborVertices[1];

            pVertexInfo = &m_vecVertexInfo[nCurrentVertex];
           
            if (nCurrentVertex == nEndVerTex)
            {
                pVertexInfo->aryNeighborVertices[0] = nAnotherNeightborVertex;
                pVertexInfo->aryNeighborVertices[1] = 0;
                pVertexInfo->nNeighborCount = 2;
                nProcessCount ++;
                break;
            }
            

        }//while

        if (nProcessCount != n)
        {//只找到了一个局部闭合图形，不是全局闭合图形

            return FALSE;
        }

        //接下来把所有的邻接点串连起来。
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
        nCurrentVertex = m_vecVertexInfo[nPrevVertexIndex].aryNeighborVertices[0];

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

    struct TVertexInfo //结点信息
    {
        int aryNeighborVertices[2];//记录结点邻接点的数组。
        int nNeighborCount;
    };


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


    //@功能:设置Lth结点和Kth结点之间的距离，保存在一个一维数组中而不是一个二维的矩阵中，节省
    //      一半的空间避免了冗余的信息存储。
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


    //@功能:寻找索引为K的结点的左右两个邻结点
    //@参数:K, 结点索引号    
    //      pVertexInfo, 输出参数，保存搜索信息
    void SearchTwoNeighbours(T* vertices, int K, TVertexInfo* pVertexInfo)
    {
        int n = m_nVerticesCount;
        //需要记录保存的M个最近距离邻结点
        const int M = 11;

        //按照距离升序排列的数组
        TNeighbour neighbours[M];
        int neighbourCount = 0;

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


            //插入排序，按照距结点K的距离从小到大排列
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

        //以结点K为原点计算邻接矢量
        for(int I = 0; I < neighbourCount; I++)
        {
            int L = neighbours[I].nVertex;
            neighbours[I].Vx = vertices[L].ptCoord.x -  vertices[K].ptCoord.x;
            neighbours[I].Vy = vertices[L].ptCoord.y -  vertices[K].ptCoord.y;
        }


        //矢量夹角小于15度，可以认为两矢量方向一致
        static double  sin_theta_threshold =  sin(15.0*M_PI /180.0);
        static double  distance_diff_times = 5;//20%
        //合并位于同一方向的邻接点
        for(int I = 0; I < neighbourCount - 1; I++)
        {
            //Ith矢量的模
            double norm_vector_I = sqrt(double(neighbours[I].Vx*neighbours[I].Vx +  neighbours[I].Vy*neighbours[I].Vy));

            //判断Ith矢量和其他矢量的夹角小于"方向一致"门限
            int J = I +1;
            while(J < neighbourCount)
            {
                //计算矢量I,J的点积和叉积,
                double dotProduct   = neighbours[I].Vx*neighbours[J].Vx + neighbours[I].Vy*neighbours[J].Vy;
                double crossProduct = neighbours[I].Vx*neighbours[J].Vy - neighbours[I].Vy*neighbours[J].Vx;
                

                //sin(theta) > sin_theta_threshold
                //crossProdut=|Vi||Vj|sin(theta);
                //
                //sin(theta) = crossProdut/(|Vi||Vj|) <  sin_theta_threshold <==> crossProdut >=  sin_theta_threshold * |Vi| * |Vj|
                //用dotProduct的符号>0来限定两矢量夹角属于[-pi/2, pi/2]
                //用crossProduct的绝对值来判定是否两矢量的夹角来判定夹角小于设定的门限角
                //当然也可以用dotProduct的值来判定， dotProduct值越小，则意味着两矢量夹角越大。

                double norm_vector_J = sqrt(neighbours[J].Vx*neighbours[J].Vx +  neighbours[J].Vy*neighbours[J].Vy);

                //dotProduct > 0，矢量夹角属于[0,π/2];并且两矢量夹角小于15.
                if(dotProduct > 0 && abs(crossProduct) < sin_theta_threshold*norm_vector_I*norm_vector_J)
                {//位于同一方向，即夹角<15度
                 //移除Jth矢量
                    long  dist_diff = abs(neighbours[I].distance - neighbours[J].distance);
                    long max_dist = neighbours[I].distance > neighbours[J].distance ? neighbours[I].distance : neighbours[J].distance;

                    if (dist_diff*distance_diff_times > max_dist)
                    {//距离相差悬殊的才合并
                        for (int pos = J; pos < neighbourCount - 1; pos++)
                        {
                            neighbours[pos] = neighbours[pos + 1];
                        }

                        neighbourCount--;
                        continue;
                    }

                   
                }
                J ++;


            }//while(J)

        }//for(I)


        //以当前结点为原点, 按方位角排列各矢量。
        SortNeighbours(neighbours, neighbourCount);



        //在剩下的邻接点中, 以本结点为原点, 选取夹角最大的两个邻接点。
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

                double norm_vector_I = sqrt(double(neighbours[I].Vx*neighbours[I].Vx + neighbours[I].Vy*neighbours[I].Vy));

                int J = I + 1;//邻矢量索引
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


        if(pVertexInfo)
        {
            for(int I = 0; I < nNearestVectorCount; I++)
            {
                pVertexInfo->aryNeighborVertices[I] = index[I];
            }

            pVertexInfo->nNeighborCount = nNearestVectorCount;

        }
    }


    //@功能:已知索引为K的结点的的一个邻结点，寻找索引为K的结点的另外一个邻结点
    //@参数:K, 结点索引号
    //      pVertexInfo, 输出参数，保存搜索信息
    //     pCandidateFlags, 结点可否选择标志，pCandidateFlag[i]=true, 则意味者ith结点可以作为搜索候选
    void SearchNextNeighbour(T* vertices, int K, TVertexInfo* pVertexInfo, const std::vector<bool>& vecNeighourCandidates)
    {
        assert(pVertexInfo->nNeighborCount == 1 && pVertexInfo->aryNeighborVertices[0] != -1);

        int nOtherNeighbourVertex = pVertexInfo->aryNeighborVertices[0];
        int nOtherNeighbourVx = vertices[nOtherNeighbourVertex].ptCoord.x - vertices[K].ptCoord.x;
        int nOtherNeighbourVy = vertices[nOtherNeighbourVertex].ptCoord.y - vertices[K].ptCoord.y;
        double norm_vector_other_neighbour = sqrt(nOtherNeighbourVx*nOtherNeighbourVx + nOtherNeighbourVy*nOtherNeighbourVy);

        int n = m_nVerticesCount;
        //需要记录保存的M个最近距离邻结点
        const int M = 11;

        //按照距离升序排列的数组
        TNeighbour neighbours[M];
        int neighbourCount = 0;

        //初始化neighbour数据
        for (int I = 0; I < M; I++)
        {
            neighbours[I].distance = (std::numeric_limits<long>::max)();
            neighbours[I].nVertex = -1;
        }

        neighbourCount = 0;

        for (int L = 0; L < n; L++)
        {
            if (K == L) continue;//自己不和自己比较

            if (!vecNeighourCandidates[L]) continue;//非候选则直接跳过

            int distance = GetLineSegmentLength(K, L);

            if (neighbours[M - 1].distance < distance)//距离大于距离最远的邻接点，显然不是邻接点的候选。
            {
                continue;
            }


            //插入排序，按照距结点K的距离从小到大排列
            int nInsertPos = 0;

            while (nInsertPos < M)
            {
                if (distance < neighbours[nInsertPos].distance)
                {
                    break;
                }
                nInsertPos++;
            }

            if (nInsertPos == M)
            {
                continue;
            }

            //插入排序
            for (int I = M - 1; I > nInsertPos; I--)
            {
                neighbours[I] = neighbours[I - 1];

            }//for

            neighbours[nInsertPos].nVertex = L;
            neighbours[nInsertPos].distance = distance;

            if (neighbourCount < M)
            {
                neighbourCount++;
            }

        }

        //以结点K为原点计算邻接矢量
        for (int I = 0; I < neighbourCount; I++)
        {
            int L = neighbours[I].nVertex;
            neighbours[I].Vx = vertices[L].ptCoord.x - vertices[K].ptCoord.x;
            neighbours[I].Vy = vertices[L].ptCoord.y - vertices[K].ptCoord.y;
        }


        //矢量夹角小于15度，可以认为两矢量方向一致
        static double  sin_theta_threshold = sin(15.0*M_PI / 180.0);
        static double  distance_diff_times = 5;//20%
        //合并位于同一方向的邻接点
        for (int I = 0; I < neighbourCount - 1; I++)
        {
            //Ith矢量的模
            double norm_vector_I = sqrt(double(neighbours[I].Vx*neighbours[I].Vx + neighbours[I].Vy*neighbours[I].Vy));

            //判断Ith矢量和其他矢量的夹角小于"方向一致"门限
            int J = I + 1;
            while (J < neighbourCount)
            {
                //计算矢量I,J的点积和叉积,
                double dotProduct = neighbours[I].Vx*neighbours[J].Vx + neighbours[I].Vy*neighbours[J].Vy;
                double crossProduct = neighbours[I].Vx*neighbours[J].Vy - neighbours[I].Vy*neighbours[J].Vx;


                //sin(theta) > sin_theta_threshold
                //crossProdut=|Vi||Vj|sin(theta);
                //
                //sin(theta) = crossProdut/(|Vi||Vj|) <  sin_theta_threshold <==> crossProdut >=  sin_theta_threshold * |Vi| * |Vj|
                //用dotProduct的符号>0来限定两矢量夹角属于[-pi/2, pi/2]
                //用crossProduct的绝对值来判定是否两矢量的夹角来判定夹角小于设定的门限角
                //当然也可以用dotProduct的值来判定， dotProduct值越小，则意味着两矢量夹角越大。

                double norm_vector_J = sqrt(neighbours[J].Vx*neighbours[J].Vx + neighbours[J].Vy*neighbours[J].Vy);

                //dotProduct > 0，矢量夹角属于[0,π/2];并且两矢量夹角小于15.
                if (dotProduct > 0 && abs(crossProduct) < sin_theta_threshold*norm_vector_I*norm_vector_J)
                {//位于同一方向，即夹角<15度
                 //移除Jth矢量

                    long  dist_diff = abs(neighbours[I].distance - neighbours[J].distance);
                    long max_dist = neighbours[I].distance > neighbours[J].distance ? neighbours[I].distance : neighbours[J].distance;

                    if (dist_diff*distance_diff_times > max_dist)
                    {//距离相差悬殊的才合并

                        for (int pos = J; pos < neighbourCount - 1; pos++)
                        {
                            neighbours[pos] = neighbours[pos + 1];
                        }

                        neighbourCount--;

                        continue;
                    }
                }

               J++;


            }//while(J)

        }//for(I)


         //以当前结点为原点, 按方位角排列各矢量。
        SortNeighbours(neighbours, neighbourCount);



        //在剩下的邻接点中, 以本结点为原点, 选取与已有邻结点夹角最大的1个邻接点。
        int index[1];
        index[0] = -1;

        int nNearestVectorCount = 0;
        if (neighbourCount < 1)
        {
            for (int I = 0; I < neighbourCount; I++)
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

            for (int I = 0; I <= neighbourCount - 1; I++)
            {

                double norm_vector_I = sqrt(double(neighbours[I].Vx*neighbours[I].Vx + neighbours[I].Vy*neighbours[I].Vy));


                double dotProduct = neighbours[I].Vx*nOtherNeighbourVx + neighbours[I].Vy*nOtherNeighbourVy;

                //cosine函数在[0,pi]上单调递减函数
                //cos(t1)<cos(t0), 则t1 > t0
                //
                if (dotProduct*norm_product_candidate < dotProduct_candidate*norm_vector_I * norm_vector_other_neighbour)
                {
                    dotProduct_candidate = dotProduct;
                    norm_product_candidate = norm_vector_I*norm_vector_other_neighbour;
                    index[0] = neighbours[I].nVertex;
                }

            }//for(I)
        }//else


        if (pVertexInfo)
        {
            pVertexInfo->aryNeighborVertices[1] = index[0];
            pVertexInfo->nNeighborCount = 2;

        }


    }





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