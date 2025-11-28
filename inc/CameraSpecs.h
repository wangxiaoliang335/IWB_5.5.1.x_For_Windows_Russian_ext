#pragma once
//物理规格数据结构

//镜头规格
struct TLensSpecification
{
    double backFocalLength;//像方焦距, 单位:mm
    double FOV_horz       ;//水平视角, 单位:度
    double FOV_vert       ;//垂直视角, 单位:度
    double FOV_diagonal   ;//对角线视角, 单位:度
    double throwRatio     ;//投射比

};

//CMOS芯片规格
struct TCMOSChipSpecification
{
    double width_in_mm    ;//绝对物理宽度, 单位:mm
    double height_in_mm   ;//绝对物理高度,单位:mm
    int    width_in_pixel ;//像素宽度
    int    height_in_pixel;//像素高度
    double pixel_size     ;//像素绝对物理尺寸, 单位:mm, eg, 0.006mm=6um

};

//自动校正补偿系数
struct TAutoCalibCompCoefs
{
    TAutoCalibCompCoefs()
    {
        u0 = 0.0;
        v0 = 0.0;
        k[0] = 0.0;
        k[1] = 0.0;
        k[2] = 0.0;

    }

    double u0;//光心垂直坐标,归一化到(0,1)范围,以独立于具体图像分辨率
    double v0;//光心垂直坐标,归一化到(0,1)范围,以独立于具体图像分辨率
    double k[3];//


};

//镜头内部参数
struct TInternalAndSymmetricDistortParams
{
	double mu;
	double mv;
	double u0;
	double v0;
	double radialDistrt[5];//径向畸变系数
	//UINT   dwFlags;//判断各个字段有效的标志
};

//typedef enum
//{
// LENS_INTERNAL_PARAMS_FALG_MU_MV_VALID = 0x01,
// LENS_INTERNAL_PARAMS_FLAG_U0_M0_VALID = 0x02,
// LENS_INTERNAL_PARAMS_FLAG_DISTORTION_VALID = 0x04
//}LEN_INTERNAL_PARAMS_FLAG;

//
struct TCameraDebugData
{
	TPoint2D pt2DContactInImage;//光笔触点在图像中的坐标。
	TPoint2D pt2DCentroid;
};