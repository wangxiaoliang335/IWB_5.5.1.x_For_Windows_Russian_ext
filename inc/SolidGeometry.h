#pragma once

//立体几何形状
struct Cylinder
{
    TPoint3D   base;//圆柱轴线基点
    TVector3D  axis;//圆柱轴线矢量
    double   radius;//圆柱半径
    double   length;//圆柱长度
};


//3维直线
struct Line3D
{
    TPoint3D  base;//直线上的一点
    TVector3D w   ;//直线矢量

};