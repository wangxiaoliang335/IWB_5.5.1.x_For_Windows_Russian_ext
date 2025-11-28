#pragma once

//@功能:光笔与板面的接触位置检测器
//      根据光斑在画面中的位置以及设想头的水平角度和垂直角度, 计算出光笔与板面的切点所成的像。
//       
class CContactPtFinder
{
public:
    CContactPtFinder();

    //@功能:根据光斑的外接矩形信息，计算光笔与板面切点的位置
    POINT operator()(const RECT& rcBound);
    

    //@功能:根据光斑重心的位置,找出
    //      ptCentroid, 光斑重心位置, 注意该坐标已经放大了2^INT_SCALE_SHIFT_SIZE倍。
    POINT operator()(const POINT& ptCentroid);


    POINT operator()(const POINT& ptCentroid, double lambda1, double lambad2);

    

    //@功能:设置"过光心且与投影幕和地面都垂直的平面"与摄像头光轴的夹角φ的正切
    void SetTangentHorz(const double& dbTangentHorz);

    //@功能:设置"过光心、垂直投影幕，且与水平面的平面"与摄像头光轴的夹角ψ的正切
    void SetTangentVert(const double& dbTangentVert);


    //@功能:设置最大视角,单位:度
    void SetViewAngle(const double& nDegree);


    //@功能:设置投射比
    void SetThrowRatio(const double& dbThrowRatio);

    //@功能:设置镜头类型
    void SetLensType(ELensType eLensType);
protected:
    //补偿模型1
    //
    POINT Model_1(const POINT& ptCentroid);

    //补偿模型2
    POINT Model_2(const POINT& ptCentroid);

protected:
    double m_dbTangentHorz;//"过光心且与投影幕和地面垂直的平面"与摄像头光轴的夹角φ的正切
    double m_dbTangentVert;//"过光心、垂直投影幕，与地面平行的平面"与摄像头光轴的夹角ψ的正切
    double m_dbR_D_Ratio_Vert ;//光笔灯泡直径与"镜头光心到投影幕距离"之比, 一般为0.01左右。
    double m_dbR_D_Ratio_Horz ;//光笔灯泡直径与"镜头光心到投影幕距离"之比, 一般为0.01左右。
    int    m_nImageCx     ;//画面宽度，单位:像素，注意放大了(2^INT_SCALE_SHIFT_SIZE)倍
    int    m_nImageCy     ;//画面高度，单位:像素，注意放大了(2^INT_SCALE_SHIFT_SIZE)倍
    double m_dbFocalLength;//等效焦距，单位:像素，注意放大了(2^INT_SCALE_SHIFT_SIZE)倍
    double m_dbMaxViewAngle;//对角线最大视角，单位:°,画面对角点与光心的连线的夹角。

    double m_dbThrowRatio  ;//投射比

    //

    ELensType m_eLensType ;//设置镜头类型

    double    m_Model1_K[2] ;//θ_modified = m_Coefs[0]*θ + m_Coefs[1]*θ^3;
    double    m_Model2_K[2] ;//θ_modified = m_Coefs[0]*θ + m_Coefs[1]*θ^3;
private:
    double m_dbHalfCx;
    double m_dbHalfCy;
};