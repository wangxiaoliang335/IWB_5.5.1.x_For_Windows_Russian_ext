#pragma once
#include "MultiDimensionPoint.h"
#include <limits>

//
//
//

//@功能:判断射线是否与线段相交
inline BOOL IsRayIntersectLine(
                               const TPoint2D&  ptRayOrigin,
                               const TVector2D& RayDirection,
                               const TPoint2D (&LineSegment)[2],
                               TPoint2D* pIntersects = NULL
                               )
{
    //线段AB的方程可以写为
    //Xp = Xa + (Xb - Xa)*λ1, λ1∈[0,1]
    //Yp = Ya + (Yb - Ya)*λ1
    //(Xp,Yp)为射线上的任意一点
    //(Xa,Ya)为线段AB的A端点坐标
    //(Xb,Yb)为线段AB的B端点坐标
    //射线的方程写为
    //Xp = Xo + Xd*λ2, λ2>1
    //Yp = Yo + Yd*λ2
    //(Xp,Yp)为射线上的任意一点
    //(Xo,Yo)为射线起点的坐标
    //(Xd,Yd)为射线的方向矢量
    //射线和线段AB的交点同时满足两个方程, 
    //
    //Xa + (Xb - Xa)*λ1 =  Xo + Xd*λ2
    //Ya + (Yb - Ya)*λ1 =  Yo + Yd*λ2
    //写成矩阵形式
    //
    //┌          ┐┌    ┐ ┌     ┐
    //│Xb-Xa  -Xd││λ1 │=│Xo-Xa│
    //│Yb-Ya  -Yd││λ2 │ │Yo-Ya│
    //└          ┘└    ┘ └     ┘
    //利用Cram法则求解λ1和λ2
    //若
    //│Xb-Xa  -Xd│
    //│          │ = 0, 则线段与射线平行，有无数个交点或者无交点。
    //│Yb-Ya  -Yd│
    //
    //通过判断λ1, λ2是否属于λ1∈[0,1]和 λ2>=0 来判断射线是否与线段相交,
    //
    //

    //Enpoints of line segment
    double Xa = LineSegment[0][0] ;
    double Ya = LineSegment[0][1];
    double Xb = LineSegment[1][0];
    double Yb = LineSegment[1][1];;

    //Direction of the ray
    double Xd = RayDirection[0];
    double Yd = RayDirection[1];

    //Origin of the ray
    double X0 = ptRayOrigin[0];
    double Y0 = ptRayOrigin[1];

    //方程系数矩阵
    double A[2][2];
    A[0][0] = Xb - Xa;
    A[0][1] = -Xd ;
    A[1][0] = Yb -Ya ;
    A[1][1] = -Yd ;

    //
    double b[2];
    b[0] = X0 - Xa;
    b[1] = Y0 - Ya;

    double detA = A[0][0]*A[1][1] - A[0][1]*A[1][0];
    if( detA < std::numeric_limits<double>::epsilon())
    {
        return FALSE;
    }

    double lambda1 = (b[0]*A[1][1] - b[1]*A[0][1])/detA;
    double lambda2 = (b[1]*A[0][0] - b[0]*A[1][0])/detA;

    if(0.0 <= lambda1 && lambda1 <= 1.0 && lambda2 >= 0.0)
    {
        if(pIntersects)
        {
            pIntersects->d[0] = X0 + lambda2*Xd;
            pIntersects->d[1] = Y0 + lambda2*Yd;
        }

        return TRUE;
    }//if


    return FALSE;
}


//@功能:指定矩形区域A内的m*n个点，将这些点排成m行n列的行列式，行列式内部相邻的四个点组成一个四边形，
//      将矩阵外圈中的线段沿从中心往外的方向延伸，这些线段必与矩形区域A的四条边相交，得到若干个交点。
//      由这些交点和矩阵最外圈上的点以及矩形的四个顶点，构成若干个三角形, 四边形,和无边形。
//      现在就是要找出这些位于矩形边界处的多边形。
//

class CAreaSplitter
{
public:
    BOOL SplitBorderArea(
        const RECT& rect,//指定的矩形区域
        const TPoint2D* pInteriorPoints,//矩形区域内部的点组成的矩阵
        int m,//m行 
        int n //n 列
        )
    {
        if(n <= 1 || m <= 1 ) return FALSE;


        int nRayCount = 2*(m + n);

        //区域个数也是2*(m+n)
        int nBorderAreaCount = nRayCount;


        //矩形区域的四个角点
        TPoint2D cornerPoints[4];

        cornerPoints[0].d[0] = rect.left ;
        cornerPoints[0].d[1] = rect.top;

        cornerPoints[1].d[0] = rect.right;
        cornerPoints[1].d[1] = rect.top;

        cornerPoints[2].d[0] = rect.right;
        cornerPoints[2].d[1] = rect.bottom;

        cornerPoints[3].d[0] = rect.left;
        cornerPoints[3].d[1] = rect.bottom;



        //边界上的射线条数2*(m + n)

        //======初始化射线矢量=======
        m_rays.resize(nRayCount);

        int rayIndex  = 0;

        //1.top-most
        for(int c = 0; c < n; c ++)
        {
            TPoint2D p1,p2;
            p1 = pInteriorPoints[c];//第一行的点
            p2 = pInteriorPoints[n + c];//第二行的点

            m_rays[rayIndex].ptDirection.d[0] = p1.d[0] - p2.d[0];
            m_rays[rayIndex].ptDirection.d[1] = p1.d[1] - p2.d[1];
            m_rays[rayIndex].ptOrigin         = p1;
            rayIndex ++;
        }


        //2.right-most
        for(int r = 0; r < m; r++)
        {
            TPoint2D p1,p2;
            p1 = pInteriorPoints[n - 1 + r*n];//第n-1列的点
            p2 = pInteriorPoints[n - 2 + r*n];//第n-2列的点

            m_rays[rayIndex].ptDirection.d[0] = p1.d[0] - p2.d[0];
            m_rays[rayIndex].ptDirection.d[1] = p1.d[1] - p2.d[1];
            m_rays[rayIndex].ptOrigin = p1;
            rayIndex ++;
        }

        //3.bottom-most
        for(int c = n-1; c >= 0; c --)
        {
            TPoint2D p1,p2;
            p1 = pInteriorPoints[(m-1)*n + c];//第m-1行的点
            p2 = pInteriorPoints[(m-2)*n + c];//第m-2行的点

            m_rays[rayIndex].ptDirection.d[0] = p1.d[0] - p2.d[0];
            m_rays[rayIndex].ptDirection.d[1] = p1.d[1] - p2.d[1];
            m_rays[rayIndex].ptOrigin = p1;
            rayIndex ++;
        }

        //4.left-most
        for(int r = m-1; r >= 0; r -- )
        {
            TPoint2D p1,p2;
            p1 = pInteriorPoints[r*n + 0];//第1列的点
            p2 = pInteriorPoints[r*n + 1];//第2列的点

            m_rays[rayIndex].ptDirection.d[0] = p1.d[0] - p2.d[0];
            m_rays[rayIndex].ptDirection.d[1] = p1.d[1] - p2.d[1];
            m_rays[rayIndex].ptOrigin = p1;
            rayIndex ++;
        }



        //求各条射线与矩形边界的交点
        m_intersections.resize(nRayCount);

        for(int rayIndex = 0; rayIndex < nRayCount;  rayIndex ++)
        {

            BOOL bFound = FALSE;
            //计算边界上的射线与矩形四条边的交点。
            for(int lineSegmentIndex = 0; lineSegmentIndex <= 3; lineSegmentIndex ++)
            {
                TPoint2D lineSegment[2];
                int nCornerIndex = lineSegmentIndex;
                lineSegment[0] = cornerPoints[nCornerIndex];

                nCornerIndex += 1;
                if(4 == nCornerIndex) nCornerIndex = 0;
                lineSegment[1] = cornerPoints[nCornerIndex];

                BOOL bIntersect =   IsRayIntersectLine(
                    m_rays[rayIndex].ptOrigin,
                    m_rays[rayIndex].ptDirection,
                    lineSegment,
                    &m_intersections[rayIndex]);

                if(bIntersect)
                {
                    bFound = TRUE;
                    break;//break out inner loop
                }

            }//for


            if(!bFound)
            {
                return FALSE;
            }

        }//for-each(rayIndex)


        //初始化区域数组
        m_areas.resize(nBorderAreaCount);
        int nAreaIndex = 0;

        //建立每个区域的多边形点集
        for(int nAreaIndex = 0; nAreaIndex < nBorderAreaCount; nAreaIndex ++)
        {
            int nRay_A_Index  = nAreaIndex - 1;
            if(nAreaIndex == 0)
            {
                nRay_A_Index = nRayCount - 1;
            }

            int nRay_B_Index = nAreaIndex;

            TPoint2D ptRayAOrigin = m_rays[nRay_A_Index].ptOrigin;
            TPoint2D ptRayBOrigin = m_rays[nRay_B_Index].ptOrigin;;

            TPoint2D ptRayAIntersection = m_intersections[nRay_A_Index];
            TPoint2D ptRayBIntersection = m_intersections[nRay_B_Index];

            TPoint2D v1 = ptRayAIntersection - ptRayAOrigin;
            TPoint2D v2 = ptRayBIntersection - ptRayAOrigin;

            double norm_v1 = norm(v1);
            double norm_v2 = norm(v2);

            //判断v1,v2的夹角在[0,π]之间。
            //利用叉积的正负来判断夹角的方向(顺时针或者逆时针)
            //正值意味着失量之间的夹角范围为[0,π)
            //负值意味着失量之间的夹角范围为[π,2π]。
            double  cross_product = (v1.d[0] * v2.d[1] - v1.d[1] * v2.d[0]);

            BOOL bReverse = FALSE;

            TPoint2D startVector = v1;
            TPoint2D endVector   = v2;
            double   norm_start_vector = norm_v1;
            if(cross_product < 0)
            {
                startVector = v2;
                endVector   = v1;
                norm_start_vector = norm_v2;
                bReverse = TRUE;
            }

            double costheta_AB = (v1.d[0] * v2.d[0] + v1.d[1] * v2.d[1])/(norm_v1 * norm_v2);

            //判断矩形的四个角点是否落在A,B矢量之间
            TPoint2D ptCornerCandidate;
            BOOL bFoundCorner = FALSE;
            for(int i = 0; i < 4 ; i++)
            {
                TPoint2D v3 = cornerPoints[i] - ptRayAOrigin;
                double norm_v3 = norm(v3);

                //通过叉积判断矢量在始边矢量以外。
                //startVector×v3
                double crossproduct_v3  = startVector.d[0]*v3.d[1] - startVector.d[1] * v3.d[0] ;
                if(crossproduct_v3 < 0)
                {//矢量v3在A,B矢量之外
                    continue;
                }

                //通过点积判断矢量是否在终边以外。
                double dotproduct_v3 = v3.d[0] * startVector.d[0] + v3.d[1] * startVector.d[1];
                double costheta_v3   = dotproduct_v3 /(norm(v3)*norm(v1));

                if(costheta_v3 < costheta_AB)
                {//角度大于夹角AB, 继续.
                    continue;
                }
                else
                {
                    bFoundCorner = TRUE;
                    ptCornerCandidate = cornerPoints[i];
                    break;
                }

            }//for-each(corner)

            m_areas[nAreaIndex].PolygonPoints.clear();
            m_areas[nAreaIndex].PolygonPoints.push_back(ptRayAIntersection);
            if(bFoundCorner)
            {
                m_areas[nAreaIndex].PolygonPoints.push_back(ptCornerCandidate);
            }
            m_areas[nAreaIndex].PolygonPoints.push_back(ptRayBIntersection);
            m_areas[nAreaIndex].PolygonPoints.push_back(ptRayBOrigin);
            if(ptRayAOrigin != ptRayBOrigin)
            {
                m_areas[nAreaIndex].PolygonPoints.push_back(ptRayAOrigin);
            }

        }//for-each(nAreaIndex)

        return TRUE;

    }//SplitBorderArea


    int GetAreaCount()const
    {
        return m_areas.size();
    }

    BOOL GetAreaPolygonPoints(unsigned int nIndex, const TPoint2D** pPolygonPoints, int* pPointCount)
    {
        if(nIndex >= m_areas.size()) return FALSE;
        if(pPolygonPoints) *pPolygonPoints = &m_areas[nIndex].PolygonPoints[0];

        if(pPointCount)*pPointCount = m_areas[nIndex].PolygonPoints.size();
        return TRUE;

    }

protected:

    struct TPolygonArea
    {

        std::vector<TPoint2D> PolygonPoints;//多边形上的点
    };

    std::vector<TPolygonArea>     m_areas;


    struct TRay
    {
        TPoint2D ptOrigin;
        TPoint2D ptDirection;
    };
    std::vector<TRay> m_rays;

    std::vector<TPoint2D>  m_intersections;



};