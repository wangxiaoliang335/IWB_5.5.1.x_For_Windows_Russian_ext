#pragma once
#include <vector>



//@功能:以指定的参考点为原点建立直角坐标系,按照方位角从小到大的顺序排列各个点。
//@参数:pPoints, 指向点数组的指针
//      nPtNumber, 点的数目
//      ptOrigin, 原点坐标
void SortInDirection(POINT*  pPoints, int nPtNumber, const POINT& ptOrigin)
{
    //方向矢量
    struct TDirectionVector
    {
        int nQuadrant;//方位所在象限编号(1,2,3,4)
                      //方位角始边为x轴正向, 依照象限方位角逐渐增加
        LONG lDx     ;//相对于原点的水平矢量大小
        LONG lDy     ;//相对于原点的垂直矢量大小

        POINT  pt    ;//原数据
        //int nPtIndex ;//在原数组中的索引
    };

    std::vector<TDirectionVector>  vecPtDirections;
    vecPtDirections.resize(nPtNumber);

    for(int i = 0;i < nPtNumber; i++)
    {

        TDirectionVector temp;
        temp.pt       = pPoints[i];
        temp.lDx      = pPoints[i].x - ptOrigin.x;
        temp.lDy      = pPoints[i].y - ptOrigin.y;
        //temp.nPtIndex = i;


        if(temp.lDx > 0 && temp.lDy>=0)
        {
             temp.nQuadrant = 1;//QuadrantⅠ第一象限
        }
        else if(temp.lDx <= 0 && temp.lDy > 0)
        {
             temp.nQuadrant = 2;//Quadrant Ⅱ第二象限
        }
        else if(temp.lDx < 0 && temp.lDy <= 0)
        {
             temp.nQuadrant = 3;//Quadrant Ⅲ第三象限
        }
        else
        {
             temp.nQuadrant = 4;//QuadrantⅣ第四象限
        }


        //从前往后寻找插入位置
        int j= 0;
        for(j=0;  j < i; j++)
        {

            //第j个元素的象限 > "插入点"的象限
            //意味着要插在位置j处，原有位置j及其以后的数据后移。
            if(vecPtDirections[j].nQuadrant > temp.nQuadrant)
            {

                break;
            }
            else if(vecPtDirections[j].nQuadrant == temp.nQuadrant)
            {
                  LONG crossProduct = temp.lDx * vecPtDirections[j].lDy - temp.lDy*vecPtDirections[j].lDx ;
                  
                  //以当前矢量为始边, 被比较的矢量为终边, 
                  //矢量叉积>0,意味着被比较的矢量的方位角大于当前矢量的方位角，找到了插入位置
                   if(crossProduct > 0 )
                   {                    
                        break;
                   }//if
            }
        }

        for(int k = i; k > j; k --)
        {
            vecPtDirections[k] = vecPtDirections[k - 1];
        }

        vecPtDirections[j] = temp;

    }//for

    
    for(int i = 0; i < nPtNumber; i++)
    {
        pPoints[i] = vecPtDirections[i].pt;
    }

}

