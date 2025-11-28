#pragma once
#include <limits>
#include <vector>
//@功能:过滤掉多边形轮廓上冗余的共线的点
inline  int FilterOutColinearPoints(const POINT* pOrgVertices, int nVertexNum, POINT* pNewVertices)
{
    //剩余的不共线的顶点个数
	int nTurnVertexCount = 0;
    const POINT* pLastTurnVertex = NULL;//上次搜索到的边界拐点

	//共线判据:两直线夹角在(0~5)或者(175~179)之间,则可以认为共线
	double sin5 = sin(5.0*M_PI / 180.0);
	double sin5_square = sin5*sin5;

	const POINT* pPrev, *pCurrent, *pNext;
	for (int i = 0; i < nVertexNum; i++)
	{
		pCurrent = pOrgVertices + i;
		pPrev = (i == 0) ? pOrgVertices + (nVertexNum - 1) : pCurrent - 1;
		pNext = (i == nVertexNum - 1) ? pOrgVertices : pCurrent + 1;
		
        //令当前点和前一邻接点所在直线为A
		//va = (va_x, va_y, 0)
		long va_x = pPrev->x - pCurrent->x;
		long va_y = pPrev->y - pCurrent->y;
		
		double norm_va_square = va_x*va_x + va_y*va_y;

        //令当前点和下一邻接点所在直线为B
		//vb = (vb_x, vb_y, 0)
		long vb_x = pNext->x - pCurrent->x;
		long vb_y = pNext->y - pCurrent->y;
        double norm_vb_square = vb_x*vb_x + vb_y*vb_y;

        double cross_a_b = va_x*vb_y - va_y*vb_x;//Cross(A,B)

		//|Cross(A,B)|=|A||B|sin(theta)
		double cross_square = cross_a_b*cross_a_b;
		
		
		double threshold = norm_va_square*norm_vb_square*sin5_square;

		//(|A||B|sin(theta))^2 < (|A||B|*sin5)^2
		if (cross_square < threshold)
		{
            if (nTurnVertexCount > 0)
            {
                if (pLastTurnVertex != pPrev)
                {//上一拐点不是当前结点的前一邻接点， 则当前点和上一边界拐点所在的直线C和直线B比较
                    long vc_x = pLastTurnVertex->x - pCurrent->x;
                    long vc_y = pLastTurnVertex->y - pCurrent->y;
                    double norm_vc_square = vc_x * vc_x + vc_y*vc_y;

                    double cross_c_b = vc_x*vb_y - vc_y*vb_x;//Cross(C,B);
                    cross_square = cross_c_b*cross_c_b;

                    if (cross_square < threshold)
                    {//前后两条直线C和B共线，则当前结点不是拐点
                        continue;
                    }
                }
                else
                {//前后两条直线A,B共线，则当前结点不是拐点
                    continue;
                }

            }
		}
		
		
        pLastTurnVertex = pCurrent;
        pNewVertices[nTurnVertexCount++] = *pCurrent;
		

	}//for

	return nTurnVertexCount;
}

//@功能:将多边形由中央向四周膨胀
//@参数:pOrgVertices, 指向原始定点数组的指针
//      nVertexNum, 顶点数目
//      nExpandSize, 膨胀尺寸
inline BOOL ExpandPolygon(const POINT* pOrgVertices, int nVertexNum,  int nExpandSize, POINT* pNewVertices)
{
	POINT centroid;
	long sum_x = 0, sum_y = 0;

	if (nVertexNum == 0) return FALSE;

	for (int i = 0; i < nVertexNum; i++)
	{
		sum_x += pOrgVertices[i].x;
		sum_y += pOrgVertices[i].y;
	}
	centroid.x = sum_x / nVertexNum;
	centroid.y = sum_y / nVertexNum;

	//直线ax+by+c=0
	struct Line
	{
		long a;
		long b;
		long c;
	};
	std::vector<Line> lines;
	lines.resize(nVertexNum);

	POINT ptStart = pOrgVertices[0];
	ptStart.x -= centroid.x;
	ptStart.y -= centroid.y;

	for (int i = 0; i < nVertexNum; i++)
	{
		POINT ptEnd;
		
		if (i == nVertexNum - 1)
		{
			ptEnd = pOrgVertices[0];
		}
		else
		{
			ptEnd = pOrgVertices[i + 1];
		}
		ptEnd.x -= centroid.x;
		ptEnd.y -= centroid.y;

		//计算通过两个顶点ptStart, ptEnd的直线
		//L = cross(ptStart,ptEnd)
		lines[i].a = ptStart.y - ptEnd.y;
		lines[i].b = -(ptStart.x - ptEnd.x);
		lines[i].c = ptStart.x*ptEnd.y - ptStart.y*ptEnd.x;

		//a*x/sqrt(a^2+b^2) + b*y/sqrt(a^2+b^2) = -c/sqrt(a^2+b^2)
		//这里abs(-c/sqrt(a^2+b^2))即为原点到直线的距离
		LONG sign_of_c = (lines[i].c >= 0) ? 1 : -1;
		
		//原点到直线的距离增加expandSize
		//a*x + b*y= - sign(c)*[abs(c) + expandSize*sqrt(a^2+b^2))
		double dbNorm = sqrt(double(lines[i].a*lines[i].a + lines[i].b*lines[i].b));
		LONG norm = (int)(dbNorm + 0.5);
		lines[i].c = sign_of_c * (abs(lines[i].c) + nExpandSize*norm);

		//Next Loop
		ptStart = ptEnd;
    }//for


	
	Line prevLine = lines[nVertexNum - 1];
	for (int i = 0; i < nVertexNum; i++)
	{
		Line currentLine = lines[i];

		//计算两条直线的焦点
		//cross_point = cross(L1,L2);
		//
		long X =   prevLine.b*currentLine.c - prevLine.c*currentLine.b;
		long Y = -(prevLine.a*currentLine.c - prevLine.c*currentLine.a);
		long Z =   prevLine.a*currentLine.b - prevLine.b*currentLine.a;
		
		if (Z)  //Z>=1
		{
			X = X / Z;
			Y = Y / Z;
			pNewVertices[i].x = X + centroid.x;
			pNewVertices[i].y = Y + centroid.y;
		}
		else
		{
			//两条直线平行,
			//两条直线没有交点，Z=0, 时
			//或者有无穷个交点, 两直线重合此时X=0,Y=0, Z=0.
			double norm = sqrt(double(currentLine.a*currentLine.a + currentLine.b*currentLine.b));
			if (norm < std::numeric_limits<double>::epsilon())
			{
				return FALSE;
			}
			
			double vx = nExpandSize*currentLine.a / norm;
			double vy = nExpandSize*currentLine.b / norm;
			pNewVertices[i].x = pOrgVertices[i].x + long(vx + .5);
			pNewVertices[i].y = pOrgVertices[i].y + long(vy + .5);

		}

		prevLine = currentLine;
	}//for
	

	return TRUE;
}
