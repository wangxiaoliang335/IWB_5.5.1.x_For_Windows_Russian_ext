//MaxWeightBipartiteMatch.h
#pragma once
#include <limits>
#include <string.h>
//二分图最大权重匹配算法(Max-Weight Matching Algorithm)
template<int N>
class CMaxWeightBipartiteMatch
{
public:
        //参数:pWeights, 以`维数组表示的n×n维的权重矩阵，前n个元素对应矩阵的第一行。
        //            n, 权重矩阵的维数
        //bMininumValue,求最小代价标志
        //返回值: 最大权重匹配的权重和
        //
        //说明:n必须小于 N
        bool operator()(const int* pWeights, int n)
        {
            if(n > N)
            {
                return false;
            }

            int x, y;

            m_n = n;
            for(x = 0; x < m_n; x++)
                for(y = 0 ; y < m_n; y++)
                    m_Weights[x][y] = *pWeights++;

            Hungarian();

            
            return true;
        }

        const int* GetMatchResult()
        {
            return &this->m_Xy[0];
        }
private:

   void InitLabels()
   {
        int x,y;
        memset(m_Lx, 0, sizeof(m_Lx));
        memset(m_Ly, 0, sizeof(m_Ly));
        for(x = 0; x < m_n; x++)
            for(y = 0; y < m_n; y++)
                m_Lx[x] = (m_Lx[x]> m_Weights[x][y]) ? m_Lx[x]:m_Weights[x][y];
   }

   void UpdateLabels()
   {
       int x,y, delta = (std::numeric_limits<int>::max)();//init delta s infinity

       //serarch minimum slack  value in elements of Y set  which are not belong to set T.
       for (y = 0; y < m_n; y ++)
            if( !m_T[y])
                delta = (delta < m_slack[y])? delta:m_slack[y];

       //update X labels
       for(x = 0; x < m_n; x ++)
           if(m_S[x]) m_Lx[x] -= delta;

       //update Y labels
       for(y = 0; y < m_n; y ++)
            if(m_T[y]) m_Ly[y] += delta;

       //update slack array
       for(y = 0; y < m_n; y++)
           if(!m_T[y])
           {   //已知m_slack[y]的定义如下:
               //m_slack[y]= min{m_Lx[x] + m_Ly[y] - m_weights[x][y]}, x∈S
               //对任意x∈S, m_Lx[x]都减去了delta,
               //对任意y不属于T, m_Ly[y]保持不变
                //所以slack[y]中也要减去delta.
                m_slack[y] -= delta;
           }
   }


   void AddToTree(int x, int prevx)
   //x, current vertex
   //prevx, vertex from set X before x in the alternating path,
   //so we add edges <prevX, m_Xy[x]>, <m_Xy[x], X>
   {
        m_S[x] = true;//add x to set S
        m_prev[x] = prevx;//

        //
        //From the definiton of m_slakc[y], m_slakc[y] is the minimum of a set of values.
        //m_slack[y]= min{m_Lx[x] + m_Ly[y] - m_weights[x][y]}, x∈S
        //Because we add a new vertex to S, so we must update slack
        for(int y = 0; y < m_n; y++)
            if(m_Lx[x] + m_Ly[y] - m_Weights[x][y] < m_slack[y])
            {//slack[y] can be smaller
                m_slack [y] = m_Lx[x] + m_Ly[y] - m_Weights[x][y];

                //the vertex in set X whick makes the smallest slack[y]
                m_slackX[y] = x;
            }


   }

   //@功能:通过查找增广路径增加匹配
   //@说明:该函数执行一次，则找到一条匹配边。
   void Augment()
   {
        
       if(m_nMaxMatch == m_n) return;//check whether matching is alreay perfect, 
                                     //the if condition is also the condition to finish the recursive calling of this function
        int x, y, root;
        int q[N], wr = 0, rd = 0    ;//广度优先搜索使用的队列， wr, rd非别为队列的当前读取和写入位置。

        //
       memset(m_S, false, sizeof(m_S));
       memset(m_T, false, sizeof(m_T));
       memset(m_prev, -1, sizeof(m_prev));

       //为了寻找增广路径，查找X点集中未匹配的第一个结点作为交替树的根(Alternating Tree)
       for( x = 0; x < m_n; x ++)
           if(m_Xy[x] == -1)//when x is not a matched vertex
           {
                q[wr++] = root = x;
                m_prev[x] = -2;//标记路径起点，前面不再有x结点。
                m_S[x] = true ;//
                break;
           }

       //init set slack
       for(y = 0; y < m_n; y++)
       {
            m_slack[y] = m_Lx[root] + m_Ly[y] - m_Weights[root][y];
            m_slackX[y] = root;
       }

       while(true)//main cycle
       {
           while(rd < wr)
           {
               x = q[rd++];//current vertex form X part

               for(y = 0; y < m_n; y++)
                    if(m_Weights[x][y] == m_Lx[x] + m_Ly[y] && !m_T[y])
                    {//iterats through all edges in equality graph

                        if(m_Yx[y] == -1) break;//an exposed vertex in Y found, so
                                                //augmenting path exists!

                        m_T[y] = true;

                        //说明:此时m_Yx[y]不可能属于集合S。
                        //     因为程序执行到的此前面时刻，每个S中的元素都有一个匹配的T元素yk
                        //     若此时由另外一个元素y与m_Yx[y]匹配，意味着yk,y同时与同一个结点
                        //     m_Yx[y]匹配
                        q[wr++] = m_Yx[y];//add vertex yx[y], which is matched
                                              //with y, to the queue
                         AddToTree(m_Yx[y], x);//add edges(x,y) and (y, yx[y]) to the tree

 
                    }
           }//while

        if(y < m_n) break; //augmenting path found!
        
        UpdateLabels();//augmenting path not found, so improve labeling
        
        //clear queue
        wr = rd = 0;

       for(y = 0; y < m_n; y++)
        //in this cycle we add edges that were added to the equality graph as a
        //result of improving the labeling, we add edge(slackx[y], y) to the tree if
        //and only if !T[y] && slack[y] == 0, also with this edge we add another one
        //(y, yx[y]) or augment the matching, if y was exposed
        if(!m_T[y] && m_slack[y] == 0 )
        {
            if(m_Yx[y] == -1)//vertex y is unmatched
            {
                x = m_slackX[y];
                break;
            }
            else
            {//vertex y has been matched
                m_T[y] = true;
                //
                //增加了新的equality edge, 
                if(!m_S[m_Yx[y]])//
                {
                    q[wr++] = m_Yx[y];
                    AddToTree(m_Yx[y], m_slackX[y]);
                }

            }
        }

        if(y < m_n) break;                          //augment path found!
       }//while(true) main cycle

       if(y < m_n)//augment path found
       {
           //增加匹配个数
            m_nMaxMatch ++;


            for(int cx = x, cy = y, ty; cx != -2; cx = m_prev[cx], cy = ty)
            {//in this cycle, inverse edges along augmenting path
                ty = m_Xy[cx];
                m_Yx[cy]  = cx;
                m_Xy[cx ] = cy;
            }

            //recall function.
            Augment();
       }


    }


   void Hungarian()
   {
       m_nMaxMatch = 0;
       memset(m_Xy, -1, sizeof(m_Xy));
       memset(m_Yx, -1, sizeof(m_Yx));
       InitLabels();
       Augment();
   }

protected:
    int m_Weights[N][N]   ;//权重矩阵

    int m_n               ;//实际的权重矩阵的维数
    int m_nMaxMatch       ;//
    int m_Xy[N]           ;//数组中的每一个单元存放每个"X点集中的结点"匹配的"Y点集中的结点"的编号, '-1'表示未匹配
    int m_Yx[N]           ;//数组中的每一个单元存放每个"Y点集中的结点"匹配的"X点集中的结点"的编号, '-1'表示未匹配
    int m_Lx[N]           ;//"X点集中各结点"的标记值
    int m_Ly[N]           ;//"Y点集中个结点"的标记值

    //========增广路径查找相关变量=======
    bool m_S[N], m_T[N]    ;//S:交替路径中属于X的点集;T:交替路径中属于Y的点集
    int  m_slack[N]        ;//对Y点集中的第y个结点, m_slack[y]= min{m_Lx[x] + m_Ly[y] - m_weights[x][y]}, x∈S
    int  m_slackX[N]       ;//m_slack[y]取最小值时，对应的X点集中的结点编号
    int  m_prev[N]         ;//记录交替路径中属于X点集的结点的编号数组，m_prev[i]的含义是交替路径中编号为i的结点在交替
                            //路径中前面且仍然属于X点集的结点编号。
                            //-2,表示根节点，表示到达了交替路径的起点。
                            //-1,表示未赋值
                            //一条交替路径由m_Xy数组和m_prev数组共同来描述
};
