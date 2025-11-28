#include "stdafx.h"
#include "headers.h"
#include <vector>
#include "../inc/CameraModel.h"
#include "../inc/MultiDimensionPoint.h"
#include "../inc/ImageFrame.h"
#include "../inc/SolidGeometry.h"
#include "CylinderBulbModel.h"


//@功能:计算过圆柱外一点P,与圆柱相切的两个平面与圆柱面的切交直线L1,L2。
//
//@参数:P, 圆柱外任意一点坐标
//      cy, 圆柱体参数
//      line1, 第一条切平面与圆柱面的切交直线L1
//      line2, 第二条切平面与圆柱面的切交直线L2
//@方法:过P点做平行与圆柱轴线的直线, 过该直线作切于圆柱体柱面的两个切平面。
void PointTangentCylinder(const TPoint3D& P, const Cylinder& cyl, Line3D* line1, Line3D* line2)
{

    //
    //求过P点与圆轴轴线平行的直线L和轴线Cyliner.w之间的距离d
    TPoint3D BP = P - cyl.base;

    TVector3D vBP = BP/norm(BP);

    //D矢量, 以圆柱基点为原点, 到过P的平行直线的距离矢量
    TVector3D vD = BP - dot(cyl.axis, BP)*cyl.axis;
    
    //距离
    double d = norm(vD);

    //归一化为单位矢量
    vD = vD/d;

   //E单位矢量, 轴线和点P所在平面π的法线单位矢量
    TVector3D vE = cross(cyl.axis, vBP);
    
    //归一化为单位矢量
    vE = vE/norm(vE);

    //圆柱截面上的两切点组成的弦的长度的一半
    //根据直角三角形的面积S=1/2*两直角边的乘积的一半=1/2*斜边的长度*斜边上的高
    double e = cyl.radius * sqrt(d*d - cyl.radius*cyl.radius)/d;


    //圆心到弦的距离
    double c = (cyl.radius * cyl.radius)/d;

    //圆柱基点在弦上的投影点Proj_B
    TPoint3D proj_B = cyl.base + c*vD;

    if(line1)
    {
        line1->base = proj_B + e*vE;
        line1->w    = cyl.axis;
    }

    if(line2)
    {
        line2->base = proj_B - e*vE;
        line2->w    = cyl.axis;
    }


}


//@功能:在center，t1, t2三点所在的平面上, 以center为中心， radius为半径做圆
//      t1,t2是圆上的两个点, 等圆心角采样t1,t2之间的圆弧上的M个点。

void SampleCircleArc(
    const TPoint3D& center,
    const double& radius,
    const TPoint3D& t1, 
    const TPoint3D& t2, 
    int M,  
    TPoint3D* pSampleData)
{

    //计算以圆心为起点的矢量
     TVector3D v1 = t1 - center;
     TVector3D v2 = t2 - center;

     //v1和v2的叉积
     TVector3D cross_v1v2 = cross(v1,v2);

     if(cross_v1v2.d[2] < 0)
     {
        TVector3D tmp = v1;
        v1 = v2;
        v2 = tmp;
     }

     double norm_v1 = norm(v1);
     double norm_v2 = norm(v2);

     //以vx作为圆面坐标的x轴的单位矢量
     TVector3D vx = v1/norm_v1;

     //以vx作为圆面坐标的y轴的单位矢量
     double proj_v2_vx = dot(v2,vx);
     TVector3D vy = v2 - proj_v2_vx * vx;
     vy = vy/norm(vy);

     //计算矢量v1,v2之间的夹角
     double angle = asin(norm(cross_v1v2)/(norm_v1*norm_v2));

     if(proj_v2_vx < 0 )//夹角大于π/2
         angle = M_PI - angle;


     //采样的角度间隔
     double sample_angle_interval = angle/(M - 1);

     double sample_angle = 0.0;
     for(int i = 0; i < M; i++)
     {

         //圆平面坐标转为3D坐标的矩阵R
         //
         //|x|      |x'|                       |x'|
         //|y| = R *|y'| = [vx vy cross(vx,vy)]|y'| = vx*x' + vy* y'
         //|z|      |0 |                       |0 |
         //

         double x = radius * cos(sample_angle );
         double y = radius * sin(sample_angle);


        pSampleData[i] = vx*x + vy*y + center;


        sample_angle += sample_angle_interval;

     }//for

}

//@功能:过圆柱外一点P求其与圆柱相切的切轮廓，并在轮廓区域内沿圆柱圆周采样M次, 沿轴向采样N次,
//      获得M*N个采样点
//@输入参数:
//     P, 圆柱外任意一点坐标
//    cyl, 圆柱体参数
//    M,切轮廓中沿圆柱截面圆周的采样次数
//    N,切轮廓中沿圆柱轴向的采样次数
//
//@输出参数:
//    pSampleData，保存采样点的数组，尺寸大小为M*N
//
 void PointTangentCylinderGrid(const TPoint3D& P, const Cylinder& cyl, int M, int N, TPoint3D* pSampleData)
 {

     Line3D line1, line2;

     //计算过点P的圆柱切平面与圆柱面
     PointTangentCylinder(P, cyl, &line1, &line2);

     //在过基点垂直于圆柱轴线的横截面上, line1.b和line2.b是两个切平面在该横截面上留下的切点。
     //两个切点之间的圆弧采样M次,
     SampleCircleArc(cyl.base, cyl.radius, line1.base, line2.base, M, pSampleData);

     double length_interval = cyl.length / (N-1);

     pSampleData += M;

     //沿圆轴轴线方向扩展采样点
     for(int i = 1; i < N; i++)
     {
            TPoint3D vInc = cyl.axis * length_interval;
            for(int j = 0; j < M; j++)
            {
                pSampleData[j] = pSampleData[j-M] + vInc;//在上一行M个点的基础上计算出新的
            }

            pSampleData += M;
     }



 }



CylinderBulbModel::CylinderBulbModel()
:
m_pGenericCameraModel(NULL)
{

}


//Copy Constructor
//必须要定义拷贝构造函数, 因为类成员变量有很多是引用。
CylinderBulbModel::CylinderBulbModel(const CylinderBulbModel& clone)
{
    m_pGenericCameraModel   = clone.m_pGenericCameraModel;
    m_vecContatcPointOffset = clone.m_vecContatcPointOffset;

}


//@功能:对于相机中每个像素点对应的世界坐标位置Pi处,直与z=0平面放置一个半径为R,长度为L的
//      圆柱形灯泡, 利用相机成像，计算灯泡在相片中的重心坐标Gi, Δ=Pi-Gi的值用来修正计算光笔触点的相机坐标。
//@参数:pCameraModel, 相机模型指针
//      pMaskImage, 屏蔽图指针
//      nImageWidth, 相机图片宽度
//      nImageHeight, 相机图片高度
void CylinderBulbModel::CalcProjectData(const CGenericCameraModel* pCameraModel, const RECT& rcMonitor, int nImageWidth, int nImageHeight)
{
    // CImageFrame grayImage;

    //<<debug
    //CImageFrame debugFrame;//调试帧
    //debugFrame.SetSize(nImageWidth, nImageHeight);
    //debugFrame.Clear();
    //debug>>

    m_nImageWidth  = nImageWidth;
    m_nImageHeight = nImageHeight;

    m_pGenericCameraModel = pCameraModel;

    //grayImage.SetSize(nImageWidth, nImageHeight);

    m_vecContatcPointOffset.resize((nImageWidth>> 2)*(nImageHeight>>2));

    TPoint3D p3dOrg;
    p3dOrg.d[0] = 0.0;
    p3dOrg.d[1] = 0.0;
    p3dOrg.d[2] = 0.0;


    //圆柱体尺寸信息
    //radius =1.5, height=6在使用投射比为0.15的镜头下,屏幕四个角落的触控精度满足要求
    //double Cylinder_Radius = 1.5;//单位:屏幕像素
    //ouble Cylinder_Height = 6  ;//单位:屏幕像素

	/*
    double Cylinder_Radius = 4  ;//单位:屏幕像素
    double Cylinder_Height = 10 ;//单位:屏幕像素
	*/
	double Cylinder_Radius = 1.5;//单位:屏幕像素
	double Cylinder_Height = 5;//单位:屏幕像素

    double Cylinder_Offset_From_Board = 0.0;//圆柱体离板面的距离(沿Z轴负向)

    int M = 13;
    int N = 13;



    //圆柱体表面采样点的3D坐标。
    std::vector<TPoint3D> vecSurface3DPoints;
    vecSurface3DPoints.resize(M*N);

    //圆柱体表面采样点的图像数据
    std::vector<TPoint2D> vecSurfaceImagePoints;
    vecSurfaceImagePoints.resize(M*N);


    TContactPointOffsetInformation* pOffsetInformation = &m_vecContatcPointOffset[0];


    RECT rcMaskArea = rcMonitor;
    long Margin = 20;
    rcMaskArea.left   -= Margin;
    rcMaskArea.right  += Margin;
    rcMaskArea.top    -= Margin;
    rcMaskArea.bottom += Margin;

    //每16个像素生成一个采样点
    for(int r = 0; r < nImageHeight; r+=4)
    {

        for(int c = 0; c < nImageWidth; c+=4)
        {
            //if(pMaskImage[r*nImageWidth + c] == 0x00)
            //{
            //    pOffsetInformation->vOffset.d[0] = 0.0;
            //    pOffsetInformation->vOffset.d[1] = 0.0;
            //    pOffsetInformation->pt2dCentroidPos.d[0] = -1.0;
            //    pOffsetInformation->pt2dCentroidPos.d[1] = -1.0;
            //    pOffsetInformation->bValid               = FALSE;
            //    pOffsetInformation ++;

            //    continue;
            //}
            //else
            //{
            //图像坐标映射为世界坐标, 
            //该坐标即为圆柱形灯泡与垂直板面的接触点。
            TPoint2D pt2DImgContact;
            pt2DImgContact.d[0] =  (double)c;
            pt2DImgContact.d[1] =  (double)r;

            TPoint2D ptWorldContactPos;

            m_pGenericCameraModel->FromImage2World(&pt2DImgContact, 1, &ptWorldContactPos);

            if(ptWorldContactPos.d[0]  < rcMaskArea.left 
                || 
                ptWorldContactPos.d[0] > rcMaskArea.right
                ||
                ptWorldContactPos.d[1] < rcMaskArea.top
                ||
                ptWorldContactPos.d[1] > rcMaskArea.bottom)
            {

                pOffsetInformation->bValid = FALSE;

            }
            else
            {

                //圆柱参数
                Cylinder cyl;

                //圆柱的基点坐标
                cyl.base.d[0] = ptWorldContactPos.d[0];//x坐标
                cyl.base.d[1] = ptWorldContactPos.d[1];//y坐标
                cyl.base.d[2] = -Cylinder_Offset_From_Board;//在Z轴负侧


                //圆柱轴线矢量
                cyl.axis.d[0] = 0;
                cyl.axis.d[1] = 0;
                cyl.axis.d[2] = -1;


                //圆柱的半径
                cyl.radius = Cylinder_Radius;

                //圆柱的长度
                cyl.length = Cylinder_Height;


                //生成相机看到的灯泡轮廓及轮廓内部的点。
                PointTangentCylinderGrid(pCameraModel->GetCameraWorldPosition(), cyl, M, N, &vecSurface3DPoints[0]);


                //for(size_t i=0; i < m_vecSurfacePoints.size(); i++)
                //{
                //    vecSurface3DPoints[i].d[0] = m_vecSurfacePoints[i].d[0] + pt2DWorld.d[0];
                //    vecSurface3DPoints[i].d[1] = m_vecSurfacePoints[i].d[1] + pt2DWorld.d[1];
                //    vecSurface3DPoints[i].d[2] = m_vecSurfacePoints[i].d[2];
                //}

                //将柱体表面上的点从3D世界坐标映射到2D图像坐标。
                m_pGenericCameraModel->MapWorldToImage(
                    &vecSurface3DPoints[0],
                    vecSurface3DPoints.size(),
                    &vecSurfaceImagePoints[0]);

                //像素化2D图像
                //grayImage.Clear();

                RECT rcSpotArea;
                rcSpotArea.left    = (std::numeric_limits<LONG>::max)();
                rcSpotArea.top     = (std::numeric_limits<LONG>::max)();
                rcSpotArea.right   = (std::numeric_limits<LONG>::min)();
                rcSpotArea.bottom  = (std::numeric_limits<LONG>::min)();


                double dbMx = 0.0; double dbMy = 0.0;
                double dbMass = (double)vecSurfaceImagePoints.size();
                for(size_t i=0; i < vecSurfaceImagePoints.size(); i++)
                {
                    const TPoint2D& ptImage = vecSurfaceImagePoints[i];

                    dbMx += ptImage.d[0];
                    dbMy += ptImage.d[1];

                    /*
                    long x = long(ptImage.d[0] + .5);
                    long y = long(ptImage.d[1] + .5);

                    if(x <0 || x >= nImageWidth) continue;
                    if(y <0 || y >= nImageHeight) continue;

                    grayImage.SetPixel(x, y, 0x80);

                    if(x < rcSpotArea.left)
                    {
                    rcSpotArea.left = x;
                    }

                    if(x > rcSpotArea.right)
                    {
                    rcSpotArea.right = x + 1;
                    }

                    if(y < rcSpotArea.top)
                    {
                    rcSpotArea.top = y;
                    }

                    if(y > rcSpotArea.bottom)
                    {
                    rcSpotArea.bottom = y + 1;
                    }*/

                }//for-each(i)

                //grayImage.SetPixel(c, r, 0xFF);//触点所成的像
                //
                ////计算光斑在图像中的重心
                //int mass = 0;
                //int mx   = 0;
                //int my   = 0;

                //BYTE* pRowData = grayImage.GetData() + rcSpotArea.top*nImageWidth + rcSpotArea.left;
                //for(long y = rcSpotArea.top; y < rcSpotArea.bottom; y++)
                //{
                //    BYTE* pImageData = pRowData;

                //    for(long x = rcSpotArea.left; x < rcSpotArea.right; x++)
                //    {
                //        if(*pImageData++ > 0)
                //        {
                //            mass ++;
                //            mx += x;
                //            my += y;
                //        }
                //    }

                //    pRowData += nImageWidth;
                //}

                //
                //计算出重心
                //TPoint2D pt2DCentroid;
                //pt2DCentroid.d[0] = (double)mx / (double)mass;
                //pt2DCentroid.d[1] = (double)my / (double)mass;
                TPoint2D pt2DCentroid;
                pt2DCentroid.d[0] = (double)dbMx / (double)dbMass;
                pt2DCentroid.d[1] = (double)dbMy / (double)dbMass;
                //
                //grayImage.SetPixel(int(pt2DCentroid[0] + .5), int(pt2DCentroid[1] + .5), 0xC0);//重心

                //计算触点距离重心的偏移量
                pOffsetInformation->vOffset.d[0] =  pt2DCentroid.d[0] - pt2DImgContact.d[0];
                pOffsetInformation->vOffset.d[1] =  pt2DCentroid.d[1] - pt2DImgContact.d[1];
                pOffsetInformation->pt2dCentroidPos.d[0] = pt2DCentroid.d[0];
                pOffsetInformation->pt2dCentroidPos.d[1] = pt2DCentroid.d[1];
                //pOffsetInformation->nLightSpotMass = (int)dbMass;
                //pOffsetInformation->rcBound        = rcSpotArea;
                pOffsetInformation->bValid         = TRUE;

                //}//else


                //调试代码
                //if(0 == r % 32  && 0 == c % 64)
                //{

                //    debugFrame |= grayImage;

                //}
                //
            }

            pOffsetInformation ++;
        }//for-each(c)


    }//for-each(r)

    /*
    {
    SYSTEMTIME st;
    GetLocalTime(&st);
    TCHAR szFilePath[MAX_PATH];

    _stprintf_s(
    szFilePath,
    _countof(szFilePath),
    _T(".\\Intermediate\\Debug\\%4d-%2d-%2d\\%02d-%02d-%02d-%3d.png"),
    st.wYear,
    st.wMonth,
    st.wDay,
    st.wHour,
    st.wMinute,
    st.wSecond,
    st.wMilliseconds
    );

    CAtlString strFullPath  = CheckAndCreatePath(szFilePath);

    SaveGrayAsPng(CT2CA(strFullPath), debugFrame.GetData(), debugFrame.Width(), debugFrame.Height());
    }

    */

}


//@功能:根据激光笔光斑的重心和轮廓，求触点与重心的偏移量。
//@参数:pCentroid,光斑重心的坐标。
//      pContourPts,光斑轮廓点数组。
//      nContourPointsCount, 轮廓点个数。

//TVector2D  CylinderBulbModel::GetOffset(const TPoint2D& ptCentroid, const TPoint2D* pContourPts, int nContourPointsCount, TPoint2D* pContactPoint)
TVector2D  CylinderBulbModel::GetOffset(const TPoint2D& ptCentroid)
{

    TVector2D offset;
    offset.d[0] = 0.0;
    offset.d[1] = 0.0;

    long x = long(ptCentroid.d[0]);
    long y = long(ptCentroid.d[1]);


    size_t nIndex  = (y>>2)*(m_nImageWidth>>2) + (x >> 2);

    if(nIndex <0 || nIndex >= m_vecContatcPointOffset.size()) return offset;

    if(x < 0) x = 0;
    if(x >= m_nImageWidth) x = m_nImageWidth - 1;

    if(y < 0) y =0;
    if(y >= m_nImageHeight) y = m_nImageHeight - 1;

    TContactPointOffsetInformation& offsetInformation = m_vecContatcPointOffset[nIndex];


    static const double scale = 1.0;


    offset.d[0] = (offsetInformation.bValid?offsetInformation.vOffset.d[0]*scale:0.0);
    offset.d[1] = (offsetInformation.bValid?offsetInformation.vOffset.d[1]*scale:0.0);



    return offset;
}



TVector2D  CylinderBulbModel::GetRealOffset(const TPoint2D& ptImagePoint)
{

	TPoint2D ptWorldContactPos;
	m_pGenericCameraModel->FromImage2World(&ptImagePoint, 1, &ptWorldContactPos);

	double Cylinder_Offset_From_Board = 0.0;//圆柱体离板面的距离(沿Z轴负向)

	//圆柱参数
	Cylinder cyl;

	//圆柱的基点坐标
	cyl.base.d[0] = ptWorldContactPos.d[0];//x坐标
	cyl.base.d[1] = ptWorldContactPos.d[1];//y坐标
	cyl.base.d[2] = -Cylinder_Offset_From_Board;//在Z轴负侧


												//圆柱轴线矢量
	cyl.axis.d[0] = 0;
	cyl.axis.d[1] = 0;
	cyl.axis.d[2] = -1;


	double Cylinder_Radius = 1.5;//单位:屏幕像素
	double Cylinder_Height = 5;//单位:屏幕像素

	//圆柱的半径
	cyl.radius = Cylinder_Radius;

	//圆柱的长度
	cyl.length = Cylinder_Height;

	int M = 6;
	int N = 6;

	std::vector<TPoint3D> vecSurface3DPoints;
	vecSurface3DPoints.resize(M*N);

	//生成相机看到的灯泡轮廓及轮廓内部的点。
	PointTangentCylinderGrid(m_pGenericCameraModel->GetCameraWorldPosition(), cyl, M, N, &vecSurface3DPoints[0]);


	//圆柱体表面采样点的图像数据
	std::vector<TPoint2D> vecSurfaceImagePoints;
	vecSurfaceImagePoints.resize(M*N);

	
	//将柱体表面上的点从3D世界坐标映射到2D图像坐标。
	m_pGenericCameraModel->MapWorldToImage(
		&vecSurface3DPoints[0],
		vecSurface3DPoints.size(),
		&vecSurfaceImagePoints[0]);


	double dbMx = 0.0; double dbMy = 0.0;
	double dbMass = (double)vecSurfaceImagePoints.size();
	for (size_t i = 0; i < vecSurfaceImagePoints.size(); i++)
	{
		const TPoint2D& ptImage = vecSurfaceImagePoints[i];

		dbMx += ptImage.d[0];
		dbMy += ptImage.d[1];

	}


	TPoint2D pt2DCentroid;
	pt2DCentroid.d[0] = (double)dbMx / (double)dbMass;
	pt2DCentroid.d[1] = (double)dbMy / (double)dbMass;
	

	TVector2D offset;
	offset = pt2DCentroid - ptImagePoint;

	return offset;

}
