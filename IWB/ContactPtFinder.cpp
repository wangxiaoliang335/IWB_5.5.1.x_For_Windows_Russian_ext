#include "stdafx.h"
//#include "headers.h"
CContactPtFinder::CContactPtFinder()
:
m_dbTangentHorz(0.0),
m_dbTangentVert(0.0),
//m_dbR_D_Ratio_Vert(0.05),
//m_dbR_D_Ratio_Horz(0.015),
m_dbR_D_Ratio_Vert(0.01),
m_dbR_D_Ratio_Horz(0.01),
m_nImageCx(640<<INT_SCALE_SHIFT_SIZE),
m_nImageCy(480<<INT_SCALE_SHIFT_SIZE),
m_eLensType(E_LENS_TR_0_DOT_34)
{
    //<<2014/04/21
    //m_dbTangentVert = tan(M_PI*0.0/180.0);//LCD 液晶屏
    //m_dbTangentVert = tan(M_PI*15.0/180.0);//2015/02/11
     m_dbTangentVert = tan(M_PI*45.0/180.0);//2015/02/11
    //m_dbTangentVert = tan(M_PI*0.0/180.0);//2015/02/11
    //m_dbTangentVert = tan(M_PI*10.0/180.0);
    //m_dbTangentVert = tan(M_PI*45.0/180.0);
    //2014/04/21>>
    m_dbHalfCx = m_nImageCx/2.0;
    m_dbHalfCy = m_nImageCy/2.0;
    SetViewAngle(120.0);

    
    m_Model1_K[0] = 1.0;
    m_Model1_K[1] = 0.0;

    
    //SetThrowRatio(0.34);//短焦镜头投射比  :0.34
                          //超短焦镜头投射比:0.28,0.19
                          //长焦投射比:1.34
}


//@功  能:根据光斑的外接矩形信息，计算光笔与板面切点的位置, 
//@返回值:
POINT CContactPtFinder::operator()(const RECT& rcBound)
{
    POINT pt;
    
    ////垂直方向
    ////光斑重心点坐标
    //double y = (rcBound.bottom + rcBound.top)/2.0 - m_dbHalfCy;

    //double tangent_alpha = y/m_dbFocalLength;

    //double tangent_alpha_sub_delta   = (tangent_alpha - m_dbTangentVert)/(1 + tangent_alpha*m_dbTangentVert);
    //double tangent_espilon_sub_delta = tangent_alpha_sub_delta * (1 - m_dbR_D_Ratio);
    //double tangent_epsilon           = (m_dbTangentVert + tangent_espilon_sub_delta)/(1 - m_dbTangentVert*tangent_espilon_sub_delta);
    //       y                         = (tangent_epsilon*m_dbFocalLength + m_dbHalfCy)*(double)MAGNIFY_FACTOR;
    //pt.y                             = int(y);



    ////水平方向计算
    //double x = (rcBound.left + rcBound.right)/2.0 - m_dbHalfCx;

    //tangent_alpha = x/m_dbFocalLength;

    //tangent_alpha_sub_delta   = (tangent_alpha - m_dbTangentHorz)/(1 + tangent_alpha*m_dbTangentHorz);
    //tangent_espilon_sub_delta = tangent_alpha_sub_delta * (1 - m_dbR_D_Ratio);
    //tangent_epsilon           = (m_dbTangentHorz + tangent_espilon_sub_delta)/(1 - m_dbTangentHorz*tangent_espilon_sub_delta);
    //       x                  = (tangent_epsilon*m_dbFocalLength + m_dbHalfCx)*(double)MAGNIFY_FACTOR;
    //pt.x                      = int(x);


    //double dy = pt.y - ((rcBound.bottom + rcBound.top)<<INT_SCALE_SHIFT_SIZE)/2.0;
    //double dx = pt.x - ((rcBound.left + rcBound.right)<<INT_SCALE_SHIFT_SIZE)/2.0;
    //AtlTrace(_T("dx=%.2f, dy=%.2f\n"), dx/(double)MAGNIFY_FACTOR, dy/(double)MAGNIFY_FACTOR);



    //pt.x = (long)(((rcBound.left   + rcBound.right)<<INT_SCALE_SHIFT_SIZE)/2.0);
    //pt.y = (long)(((rcBound.bottom + rcBound.top  )<<INT_SCALE_SHIFT_SIZE)/2.0);


    pt.x  = (long)(((rcBound.left + rcBound.right  )<<INT_SCALE_SHIFT_SIZE)/2.0);
    pt.y = rcBound.top << INT_SCALE_SHIFT_SIZE;

    return pt;
}

//@功能:根据光斑重心的位置,找出
//      ptCentroid, 光斑重心位置, 注意该坐标已经放大了2^INT_SCALE_SHIFT_SIZE倍。
POINT CContactPtFinder::operator()(const POINT& ptCentroid)
{

   // //return ptCentroid;
   // POINT pt;
   // 
   // //垂直方向
   // //光斑重心点坐标
   // double y = ptCentroid.y - m_dbHalfCy;

   // //光点与光轴的夹角。
   // double tangent_alpha = y/m_dbFocalLength;

   // 
   // //tg(α-δ)=(tgα - tgδ)/(1 + tgα*tgδ)
   ////double tangent_alpha_sub_delta   = (tangent_alpha - m_dbTangentVert)/(1 + tangent_alpha*m_dbTangentVert);

   // //tg(α+δ)=(tgα + tgδ)/(1 - tgα*tgδ)
   // //注意此处δ为摄像头光轴与水平方向的夹角。
   // //tgδ即为m_dbTangentVert
   // double tangent_alpha_add_delta   = (tangent_alpha + m_dbTangentVert)/(1 - tangent_alpha*m_dbTangentVert);

   // //tg(ε-δ)=tg(α-δ)*/(1 - m_dbR_D_Ratio)
   // //double tangent_espilon_sub_delta = tangent_alpha_sub_delta * (1 - m_dbR_D_Ratio);

   // //tg(ε+δ)=tg(α+δ)*/(1 - m_dbR_D_Ratio)
   // double tangent_espilon_add_delta = tangent_alpha_add_delta * (1 - m_dbR_D_Ratio_Vert);

   // //tg(ε) =  tg(ε+δ-δ) = [tg(ε+δ) - tgδ]/[1 + tg(ε+δ)*tgδ]
   // //double tangent_epsilon           = (m_dbTangentVert + tangent_espilon_sub_delta)/(1 - m_dbTangentVert*tangent_espilon_sub_delta);
   // double tangent_epsilon             = (tangent_espilon_add_delta - m_dbTangentVert)/(1 + m_dbTangentVert*tangent_espilon_add_delta);

   // //
   //        y                         = tangent_epsilon*m_dbFocalLength + m_dbHalfCy;

   // pt.y                             = int(y);





   // //水平方向计算
   // double x = ptCentroid.x - m_dbHalfCx;

   // tangent_alpha = x/m_dbFocalLength;

   //// double tangent_alpha_sub_delta   = (tangent_alpha - m_dbTangentHorz)/(1 + tangent_alpha*m_dbTangentHorz);
   // tangent_alpha_add_delta   = (tangent_alpha + m_dbTangentHorz)/(1 - tangent_alpha*m_dbTangentHorz);
   // //double tangent_espilon_sub_delta = tangent_alpha_sub_delta * (1 - m_dbR_D_Ratio);
   // tangent_espilon_add_delta = tangent_alpha_add_delta * (1 - m_dbR_D_Ratio_Horz);

   // //tangent_epsilon           = (m_dbTangentHorz + tangent_espilon_sub_delta)/(1 - m_dbTangentHorz*tangent_espilon_sub_delta);
   //  tangent_epsilon           = (tangent_espilon_add_delta - m_dbTangentHorz)/(1 + m_dbTangentHorz*tangent_espilon_add_delta);


   //        x                  = tangent_epsilon*m_dbFocalLength + m_dbHalfCx;
   // pt.x                      = int(x);


   // double dy = pt.y - ptCentroid.y;
   // double dx = pt.x - ptCentroid.x;
   // AtlTrace(_T("x=%f, y=%f, dx=%.2f, dy=%.2f\n"), 
   //     ptCentroid.x/(double)MAGNIFY_FACTOR, 
   //     ptCentroid.y/(double)MAGNIFY_FACTOR, 
   //     dx/(double)MAGNIFY_FACTOR, 
   //     dy/(double)MAGNIFY_FACTOR);

    //pt.x = ((rcBound.left   + rcBound.right)<<INT_SCALE_SHIFT_SIZE)/2.0;
    //pt.y = ((rcBound.bottom + rcBound.top  )<<INT_SCALE_SHIFT_SIZE)/2.0;


    POINT pt = Model_1(ptCentroid);
    //POINT pt = Model_2(ptCentroid);

    return pt;
}


//补偿模型1
//设θ为光斑与镜头主光轴的夹角
//根据光斑的u,v坐标可以计算得到距离画面中心的距离r, 和焦距(单位:xiangsu)，可以求出θ角。
//另设畸变模型
//θ'= k1*θ + k2*θ^3
//θ'为修正后的光斑与镜头主光轴的夹角
//根据θ'计算修正后的光斑坐标u',v'。
//注意该坐标已经放大了2^INT_SCALE_SHIFT_SIZE倍
POINT CContactPtFinder::Model_1(const POINT& ptCentroid)
{
   POINT pt;

    //相机画面中心的坐标
    int u0 = m_nImageCx >> 1;
    int v0 = m_nImageCy >> 1;

    //光斑重心在以画面中心为原点的坐标系中的矢量长度。
    int u = ptCentroid.x  - u0;
    int v = ptCentroid.y  - v0;

    double radius = sqrt((double)(u*u + v*v));


    double theta = atan(radius/m_dbFocalLength);

    //double theta_modified =  (1 - lambda)*theta;

    double theta_modified = m_Model1_K[0]*theta + m_Model1_K[1]*theta*theta*theta;

    double radius_modified = m_dbFocalLength * tan(theta_modified);


    int u_modified = u;
    int v_modified = v;

    if(radius > 1e-6)
    {
       u_modified =  u * radius_modified / radius;
       v_modified =  v * radius_modified / radius;
    }

    //修正后的光斑位置
    pt.x = u_modified  + u0;
    pt.y = v_modified  + v0;

    return pt;

}


//补偿模型2
//设θ为光斑与镜头主光轴的夹角
//根据光斑坐标(u,v), 可以求出距离光心的距离r,设畸变误差dr
//dr = k*θ
//r' = r + dr
POINT CContactPtFinder::Model_2(const POINT& ptCentroid)
{
    POINT pt;

    //相机画面中心的坐标
    int u0 = m_nImageCx >> 1;
    int v0 = m_nImageCy >> 1;

    //光斑重心在以画面中心为原点的坐标系中的矢量长度。
    int u = ptCentroid.x  - u0;
    int v = ptCentroid.y  - v0;

    double radius = sqrt((double)(u*u + v*v));


    double theta = atan(radius/m_dbFocalLength);

    double delta_radius = m_Model2_K[0] * theta;


    double radius_modified = radius + delta_radius;


    int u_modified = u;
    int v_modified = u;

    if(radius > 1e-6)
    {
        u_modified = u*radius_modified/radius;
        v_modified = v*radius_modified/radius;
    }

    pt.x = u_modified + u0;
    pt.y = v_modified + v0;

    return pt;

}



//@功能:根据光斑重心的位置,找出
//      ptCentroid, 光斑重心位置, 注意该坐标已经放大了2^INT_SCALE_SHIFT_SIZE倍。
//      lambda,偏移修正参数
//@说明:设光斑与相机中心的夹角为θ, 修正后的夹角为(1-lambda)θ,夹角修正后的
//      的相机坐标
//      x=f*sin[(1-lambda)θ]*cosα
//      y=f*sin[(1-lambda)θ]*sinα
POINT CContactPtFinder::operator()(const POINT& ptCentroid, double lambda1, double lambad2)
{
    POINT pt;

    //相机画面中心的坐标
    int x0 = m_nImageCx >> 1;
    int y0 = m_nImageCy >> 1;

    //光斑重心在以画面中心为原点的坐标系中的矢量长度。
    int x = ptCentroid.x  - x0;
    int y = ptCentroid.y  - y0;

    double radius = sqrt((double)(x*x + y*y));


    double theta = atan(radius/m_dbFocalLength);

    //double theta_modified =  (1 - lambda)*theta;

    double theta_modified = lambda1*theta + lambad2*theta*theta;

    double radius_modified = m_dbFocalLength * tan(theta_modified);


    int x_modified = x;
    int y_modified = y;

    if(radius > 1e-6)
    {
       x_modified =  x*radius_modified/radius;
       y_modified =  y*radius_modified/radius;
    }

    //修正后的光斑位置
    pt.x = x_modified  + x0;
    pt.y = y_modified  + y0;


    return pt;
}

//@功能:设置"过光心且与投影幕和地面都垂直的平面"与摄像头光轴的夹角φ的正切
void CContactPtFinder::SetTangentHorz(const double& dbTangentHorz)
{
    m_dbTangentHorz = dbTangentHorz;
}


//@功能:设置"过光心、垂直投影幕，且与水平面的平面"与摄像头光轴的夹角ψ的正切
void CContactPtFinder::SetTangentVert(const double& dbTangentVert)
{
    m_dbTangentVert = dbTangentVert;
}


//@功能:设置
void CContactPtFinder::SetViewAngle(const double& dbDegree)
{
    m_dbMaxViewAngle = dbDegree;


    //画面对角线长度
    double d = sqrt(double(m_nImageCx * m_nImageCx + m_nImageCy * m_nImageCy));

    double tang_half_view_angle = tan((double)dbDegree*M_PI/360);

    //计算等效焦距长度
    m_dbFocalLength = d/tang_half_view_angle;


    //计算投射比
    m_dbThrowRatio = m_dbFocalLength/m_nImageCx;
}


//@功能:设置投射比
//      
void CContactPtFinder::SetThrowRatio(const double& dbThrowRatio)
{
    m_dbThrowRatio = dbThrowRatio;
    m_dbFocalLength = m_nImageCx*m_dbThrowRatio;//像素等效焦距，单位:像素


    //画面对角线长度
    double d = sqrt(double(m_nImageCx * m_nImageCx + m_nImageCy * m_nImageCy));

    m_dbMaxViewAngle = 2*atan(d/2/m_dbFocalLength)*180.0/M_PI;

}


//@功能:设置镜头类型
void CContactPtFinder::SetLensType(ELensType eLensType)
{
    m_eLensType = eLensType;


    switch(m_eLensType)
    {
    case E_LENS_TR_1_DOT_34:
        SetThrowRatio(1.34);
        //m_Model1_K[0] = 0.9915;////matlab仿真结果;
        m_Model1_K[0] = 0.9920;////matlab实验结果
        m_Model1_K[1] = 0.0;

        m_Model2_K[0] =-8.8285* double(1<<INT_SCALE_SHIFT_SIZE);
        m_Model2_K[1] = 0.0;
        break;


    case E_LENS_TR_0_DOT_34://投射比0.34
        SetThrowRatio(0.34);
        m_Model1_K[0] =  0.9891;//matlab仿真结果
        m_Model1_K[0] = 0.9891 ;//实验数据
        m_Model1_K[1] = 0.0;

        m_Model2_K[0] = -4.285 * double(1<<INT_SCALE_SHIFT_SIZE);
        m_Model2_K[1] = 0.0;
        break;

    case E_LENS_TR_0_DOT_21://投射比0.28
        /*
      //  SetThrowRatio(0.28);        
      //m_Coefs[0]   = 0.98988;//Matlab仿真数据
        m_Model1_K[0]   = 0.99225;//试验数据
        m_Model1_K[1] = 0.0;
        */

        //实验发现, 设置投射比0.28后,偏差较大
        //反而取视角120度,畸变参数取0.98569时校正较为精确，原因待查。
        //
        SetViewAngle(120.0);
        m_Model1_K[0] = 0.98560;
        m_Model1_K[1] = 0.0;


        m_Model2_K[0] = -3.459 * double(1<<INT_SCALE_SHIFT_SIZE);
        m_Model2_K[1] = 0.0;
        break;

    case E_LENS_TR_0_DOT_15:
        /*
        SetThrowRatio(0.19);
        //m_Model1_K[0] = 0.9921;//Matlab仿真结果
        //m_Model1_K[0] = 0.9936  ;//试验数据
        m_Model1_K[0] = 0.9930    ;//试验数据
        m_Model1_K[1] = 0.0;
        */
        //实验发现, 设置投射比0.28后,偏差较大
        //反而取视角120度,畸变参数取0.98569时校正较为精确，原因待查。
        //
        SetViewAngle(120.0);
        m_Model1_K[0] = 0.98560;
        m_Model1_K[1] = 0.0;

        m_Model2_K[0] = -2.7821 * double(1<<INT_SCALE_SHIFT_SIZE);
        m_Model2_K[1] = 0.0;
        break;

    }

}