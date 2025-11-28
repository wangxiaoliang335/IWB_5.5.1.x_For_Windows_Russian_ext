#include "stdafx.h"
#include <tchar.h>
//#include "../inc/CameraModel.h"
#include "PerspectiveCameraModel.h"
#include <Matrix.h>

CPerspectiveCameraModel::CPerspectiveCameraModel()
    :
    m_H(*((double(*)[3][3])&m_Parameters[0]))
    ,m_cxImage(m_Parameters[9])
    ,m_cyImage(m_Parameters[10])
{
    m_cxImage = 640.0;
    m_cyImage = 480.0;
    memset(m_Parameters, 0, sizeof(m_Parameters));
}

BOOL CPerspectiveCameraModel::CalcParams(const TPoint2D* pPointsInImage, const TPoint2D* pPointsOnPlane, int nPtNumber, const SIZE& szImage)
{
    
    if (nPtNumber == 0) return FALSE;

    //m_szImage = szImage;
    m_cxImage = szImage.cx;
    m_cyImage = szImage.cy;

    //H = [h(1), h(2), h(3);
    //    h(4), h(5), h(6);
    //    h(7), h(8), 1];
    // 
    // |x     |     |u|
    // |y     | = H*|v|
    // |lambda|     |1| 

    //利用DLT(Direct Linear Transform)方法计算单应矩阵
    //x = (u*h11 + v*h12 + h13)/(u*h31 + v*h32 + h33)
    //y = (u*h21 + v*h22 + h23)/(u*h31 + v*h32 + h33)
    //这里h33恒等于1
    //
    //写成齐次方程格式
    //|u, v, 1, 0, 0, 0, -x[0]*u, -x[0]*v| | h11 |   |x[0]|
    //|0, 0, 0, u, v, 1, -y[0]*u, -y[0]*v| | h12 | = |y[0]|
    //|u, v, 1, 0, 0, 0, -x[1]*u, -x[1]*v| | h13 |   |x[1]|
    //|0, 0, 0, u, v, 1, -y[1]*u, -y[1]*v| | h21 | = |y[1]|    
    //|                                  | | h22 |   |    |
    //|                                  | | h23 |   |    |
    //|                                  | | h31 |   |    |
    //|                                  | | h32 |   |    |
    //                A                       H        B
    //
    //    
    //Solution:
    //A*H=B
    //H=pinv(A*A')*A'*B;

    MatlabSimu::Matrix<double> A(2 * nPtNumber, 8);
    MatlabSimu::Matrix<double> B(2 * nPtNumber, 1);

    for (int i = 0; i < nPtNumber; i++)
    {
        double x = pPointsOnPlane[i].d[0];
        double y = pPointsOnPlane[i].d[1];
        double u = pPointsInImage[i].d[0] / m_cxImage;
        double v = pPointsInImage[i].d[1] / m_cyImage;

        //矩阵A
        A[i * 2 + 0][0] = u;
        A[i * 2 + 0][1] = v;
        A[i * 2 + 0][2] = 1;
        A[i * 2 + 0][3] = 0;
        A[i * 2 + 0][4] = 0;
        A[i * 2 + 0][5] = 0;
        A[i * 2 + 0][6] = -x*u;
        A[i * 2 + 0][7] = -x*v;

        A[i * 2 + 1][0] = 0;
        A[i * 2 + 1][1] = 0;
        A[i * 2 + 1][2] = 0;
        A[i * 2 + 1][3] = u;
        A[i * 2 + 1][4] = v;
        A[i * 2 + 1][5] = 1;
        A[i * 2 + 1][6] = -y*u;
        A[i * 2 + 1][7] = -y*v;


        //矢量B
        B[i * 2 + 0][0] = x;
        B[i * 2 + 1][0] = y;

    }


    MatlabSimu::Matrix<double> AT = A;
    AT.Transpose();

    MatlabSimu::Matrix<double> ATA = AT*A;

    MatlabSimu::Matrix<double> INV_ATA;
    MatlabSimu::Inverse(ATA, INV_ATA);

    MatlabSimu::Matrix<double> H = INV_ATA*AT*B;

    for (int i = 0; i < 8; i++)
    {
        m_Parameters[i] = H[i][0];
    }
    m_Parameters[8] = 1.0;



    //debug
    std::vector<TPoint2D> vecPtScreenFromModel;
    vecPtScreenFromModel.resize(nPtNumber);

    GetScreenPt(pPointsInImage, &vecPtScreenFromModel[0], nPtNumber);

    for (int i = 0; i < nPtNumber; i++)
    {
        double x, y, x_model, y_model;
        x = pPointsOnPlane[i].d[0];
        y = pPointsOnPlane[i].d[1];

        x_model = vecPtScreenFromModel[i].d[0];
        y_model = vecPtScreenFromModel[i].d[1];

        double offsetX = x_model - x;
        double offsetY = y_model - y;
        
        TCHAR debugInfo[256];
        _stprintf_s(
            debugInfo,
            _countof(debugInfo),
            _T("x=%.2f, y=%.2f, x_model=%.2f, y_model=%.2f, offsetx=%e offeset=%e\n"),
            x,
            y,
            x_model,
            y_model,
            offsetX,
            offsetY);

        OutputDebugString(debugInfo);

    }


    //debug
    return TRUE;
}


//@功能;根据视频坐标计算得出计算机屏幕坐标
//@参数:ptImage, 光斑的视频图像坐标
//      ptScreen, 保存计算机屏幕坐标的地址
BOOL CPerspectiveCameraModel::GetScreenPt(const TPoint2D* ptImage, TPoint2D* ptScreen, int nPtNumber)
{

    if (m_cxImage == 0 || m_cyImage == 0) return FALSE;

    for (int i = 0; i < nPtNumber; i++)
    {
        double u = ptImage[i].d[0] / m_cxImage;
        double v = ptImage[i].d[1] / m_cyImage;

        double x = m_H[0][0] * u + m_H[0][1] * v + m_H[0][2] * 1;
        double y = m_H[1][0] * u + m_H[1][1] * v + m_H[1][2] * 1;
        double z = m_H[2][0] * u + m_H[2][1] * v + m_H[2][2] * 1;

        if (abs(z) > 1e-10)
        {
            x = x / z;
            y = y / z;
        }

        ptScreen[i].d[0] = x;
        ptScreen[i].d[1] = y;
    }

    return TRUE;
}


BOOL CPerspectiveCameraModel::IsValidParameters()
{
    for (int i = 0; i < _countof(m_Parameters); i++)
    {
        if (m_Parameters[i] != 0) return TRUE;
    }

    return FALSE;
}

int CPerspectiveCameraModel::GetParameterCount()const
{
    return _countof(m_Parameters);
}

const double* CPerspectiveCameraModel::GetParameters()const
{

    return &m_Parameters[0];
}


void CPerspectiveCameraModel::SetParameters(const double* pParameters, int nParametersCount)
{
    if (nParametersCount != _countof(m_Parameters))
    {
        return;
    }

    for (int i = 0; i < nParametersCount; i++)
    {
        m_Parameters[i] = pParameters[i];
    }
}

//@功能:映射3D世界坐标系中的点到相机图片上
//@参数:ptWorld, 3D世界坐标数组
//      nPtNumber, 3D世界坐标个数
//      ptImage, 输出的2D图像坐标
BOOL CPerspectiveCameraModel::MapWorldToImage(const TPoint3D* ptWorld, int nPtNumber, TPoint2D* ptImage) const
{
//	BOOL bRet = ForwardProject3D(
//		ptWorld,
//		nPtNumber,
//		m_Parameters,
//		EXTENDED_MODEL_INTERNAL_PARAMS_COUNT,
//		m_Rs,
//		m_ts,
//		ptImage);

//	return bRet;
	return true;
}