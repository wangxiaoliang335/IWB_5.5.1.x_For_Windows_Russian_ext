#pragma once

#include  <utility>
#include  <vector>
#include "../inc/CommonDefine.h"
#include  "CalibrationDataDef.h"
#include "../inc/AlgoLib.h"

class CVideToScreenMap
{
public:
    CVideToScreenMap (int nID);
    ~CVideToScreenMap();

    //POINT GetPt(int x, int y, bool* pbIsOutside=NULL);

    /*
    //@功能:在镜头畸变严重的情形下,根据光笔光斑的重心和外界矩形的位置，获取其对应的屏幕坐标
    //@参数:pPtCentroid, 指向光斑重心坐标数组的指针
    //      prcBound, 指向光斑外接矩形数组的指针
    //      pbIsOutside, 指向判断光斑是否位于屏幕区域以外数组的指针
    //added by xuke, 2016/11/10
    //POINT GetPt_Aug(const POINT* pPtCentroid, const RECT* prcBound, int nPtNumber, bool* pbIsOutsideScreen=NULL);
    BOOL GetPt_Aug(const POINT* pPtCentroid, const RECT* prcBound, int nPtNumber, POINT* pPtScreen, bool* pbIsOutsideScreen);
    */


    //@功能:根据光斑轮廓来得到光球与板面的切点位置
    //@参数:pCentroid, 光斑重心坐标
    //      pContourPoints, 指向边界轮廓点列表的指针
    //      nContourPointCount, 轮廓点
    //      pPtWorld, 输出参数,保存映射结果的内存地址
    //      pbIsOutsideScreen, 保存映射点在屏幕外的标志。
    //BOOL GetPt_WithLightSpotContour(const POINT& pCentroid,  const POINT* pContourPoints, int nContourPointCount, POINT* pPtWorld, bool* pbIsOutsideScreen, POINT* debugPtContourCross = NULL);
    BOOL GetPt(const TPoint2D& ptImage, TPoint2D* pPtScreen, bool* pbIsOutsideScreen, BOOL bWithoutAutoCalibCompensate = FALSE, TCameraDebugData* pDebugData = NULL);


    //@功能:更新坐标映射
    BOOL UpdateCoordMapData();


    //@功能:考虑K次项的最小距离坐标校正算法
    //template<int K> 
    //BOOL  Calibrate_LSLK(int N);

    //@功能:校正算法
    //BOOL DoCalibrate(E_CALIBRATE_MODE eCalibrateMode ,int N);

    //@功能:校正操作
    BOOL DoCalibrate();

    ////@功能:利用最小距离法更新坐标映射
    ////		Linear Square Least(LSL)
    //BOOL Calibrate_LSL();

    //BOOL Calibrate_LSL2();


    ////@功能:考虑了3次项的坐标映射算法
    //BOOL Calibrate_LSL3();


    ////@功能:考虑了3次项的坐标映射算法
    //BOOL Calibrate_LSL4();


    //@功能:设置校正点的屏幕坐标和实际输入的光笔的坐标(相对视频窗口左上角的坐标)
    //BOOL SetCalibrateData(E_Adjust_Point_Pos ePos, POINT& ptCalibrateSymbol, POINT& ptAdjust);
    //BOOL SetCalibrateData(int nPos, const POINT& ptScreen, const POINT& ptImage,BOOL RearProjection);
    BOOL SetCalibrateData(const TCalibData& calibData);


    //void SetCalibrateData(const TCalibrateData& adjustData);

    const TCalibData& GetCalibrateData() const;

    BOOL ChangeImageSize(int nNewImageSize, int nNewImageHeight);

    //BOOL OnOnChangeScreenSize(int nNewScreenWidth, int nNewScreenHeight);

    //void SetScreenSize(int nNewScreenWidth, int nNewScreenHeight);

//    BOOL SetAdjustPtNumber(size_t nNewNumber);

    //@功能:获取屏幕尺寸
    //const SIZE GetScreenSize()const;

    ////@功能:设置正在校正标志
    //void SetAdjustingTag(BOOL bIsAdjusting);

    //RECT GetVideoClipArea()const;
    //	BOOL GetExMouseScreenPosition();         //zhaoweina 2013-01-04



    //2012/07/25, 人工补正数据相关

    //@功能:更新人工补正数据
    //void UpdateManualCorrectData(const std::vector<CalibratePair>& newManualCorrectData);

    //@功能:作废人工补正数据
   // void InvalidateManualCorrectData();


    //@功能:判断人工补正数据是否有效
    //BOOL IsValidManualCorrectData();



    const std::vector<CalibratePair>& GetManualCorrectData()const{ return m_vecManualCorrectData;}


    //@功能:设置鼠标水平和垂直位置偏差的整体修正参数
    //@参数:nXDeviationCompensation, 水平偏差修正值, 单位像素,可正可负
    //      nYDeviationCompensation, 垂直偏差修正值, 单位像素,可正可负
    void SetDeviationCompensationValue(int nXDeviationCompensationVal, int nYDeviationCompensationVal);


    //@功能:获取鼠标水平和垂直位置偏差的修正设定值。
    //@参数:
    //      pXDeivationCompensationVal, 保存水平偏移修正值的缓冲区地址。
    //      pYDeviationCompensationVal, 保存垂直偏移修正值得缓冲区地址。
    void GetDeviationCompensationValue(int* pXDeivationCompensationVal, int* pYDeviationCompensationVal);
    //>>

    //@功能:返回校正方程参数
    const TCalibParams* GetCalibParams()const;

    //@功能:设置校正方程参数
    void SetCalibParams(const TCalibParams& param);


    //@功能:设置光点位置到屏幕编号的映射数组,以决定光斑处于哪个屏幕上 
    void SetScreenMap(const CImageFrame& screenMap);

    //@功能:判断点处于哪个屏幕
    //@返回值:-1,错误
    //        0,1,2,...7,为屏幕编号
    int MointorFromPt(const POINT& pt);


    //@功能:载入屏幕映射数据（光点到各个屏幕编号的映射关系)
    BOOL LoadScreenMap();

    //@功能:保存屏幕映射数据（光点到各个屏幕编号的映射关系)
    BOOL SaveScreenMap();

    void SetRearProjectMode(BOOL bRearProject) {m_bIsRearProjectMode = bRearProject;}

    
    //<<added by toxuke@gmail.com, 2015/02/09
    //由于校正时使用的屏幕信息和实际使用时的屏幕信息不一致,
    //因此需要一个成员函数以便能够被外部调用来更新实际的屏幕
    //尺度信息
    //@参数:rcMonitorResolution, 屏幕的分辨率
    //@说明:由于设置屏幕尺寸信息和使用屏幕尺寸信息可能在不同的线程中，
    //      因此需要注意线程同步的问题。
    void SetActualMonitorResolution(const RECT& rcMonitorResolution)
    {
        CComCritSecLock<CComAutoCriticalSection> lock(m_csForCalibrateThreadSafe);
        m_rcMonitorResolution = rcMonitorResolution ;
    }
    //>>

    //<<added by toxuke@gmail.com, 2015/02/09
    //调试辅助函数，从校正数据文件中读取校正数据,进行校正
    BOOL CalibrateFromDataFile(LPCTSTR lpszFileName);
    //>>

    //<<added by toxuke@gmail.com, 2015/02/09
    //调试辅助函数，输出校正方程的系数
    //void OutputCalibParams(LPCTSTR lpszFileName = NULL);
    //>>
    
    CalibrateAlgo& GetCalibAlog() { return m_oCalibrateAlog;}

	void  ScreenOutAreaWidthInPixel(double screenDigonalInMM);

    //@功  能:判断屏幕坐标在相机管辖的屏幕以内
    //@返回值:true, 坐标点在屏幕内部
    //        false, 坐标点在屏幕外部
    bool IsOutsideOwnedArea(const POINT& ptScreen )const ;
    //
protected:
    //屏幕尺寸
    SIZE m_ScreenSize;
    ////视频尺寸
    //SIZE m_VideoSize;
    //std::vector<CalibratePair> m_vecAdjustsData;
    //TCalibrateData m_tCalibrateData; //校正数据
    TCalibData m_tCalibData; //校正数据


    std::vector<CalibratePair> m_vecManualCorrectData;//人工补正数据。
    BOOL m_bManualCorrectDataIsValid                  ;//表征人工补正数据是否正确标志


    //	BOOL m_bIsExScreenPositionToMouse;//光斑在"光斑检测区域"外部标志

    //BOOL m_bAdjusted ;//已校正标志
    //POINT* m_pMapData;//
    //SHORT_POINT* m_pMapData;//

    //RECT m_rcVideoClipArea;//视频剪切区域

    //2012/08/17,鼠标位置偏差修正
    //<<
    int m_nMouseXDeviationCompensationVal;//鼠标在校正位置水平方向上的偏差,量化到0~65535
    int m_nMouseYDeviationCompensationVal;//鼠标在校正位置垂直方向上的偏差,量化到0~65535
    static const int MAX_PERMIT_DEVIATION_COMPENSATION = 20;//最大偏差修正值, 单位是显示像素

	static const  double SCREEN_OUT_AREA_WIDTH_IN_MM;//屏幕外围有效区域的宽度，单位:毫米


    CalibrateAlgo m_oCalibrateAlog;
    
    //光点位置到屏幕编号的映射数组,以决定光斑处于哪个屏幕上
    CImageFrame   m_oScreenMap;

    int           m_nID;

    BOOL m_bIsRearProjectMode;//是否是背投模式

    //<<added by toxuke@gmail.com, 2015/02/09
    //由于校正时使用的屏幕信息和实际使用时的屏幕信息不一致,因此需要
    //一个成员函数以便能够被外部调用来更新实际的屏幕信息、
    //一个RECT类型的成员变量保存实际的屏幕信息。
    RECT m_rcMonitorResolution;


    CComAutoCriticalSection m_csForCalibrateThreadSafe;//保护校正线程安全的关键段

	double m_ScreenDigonalInMM;
	double m_ScreenOutAreaWidthInPixel;
    
    //>>
};

template<class T>
inline T POW(T base, int exponent)
{
    T val = T(1);
    for(int i=0; i<exponent; i++)
    {
        val*=base;
    }
    return val;
}



//
//Camera Calibration
//算法Calibrate_LSL中未考虑到摄像机造成的图像失真(Camera Distortion)
//考虑引进视频坐标<u,v>的K次项.
//修正转化矩阵为
//                                                                                                | u^K          | 
//                                                                                                | u^(K-1) * v  |
//                                                                                                | u^(K-2) * v^2| 
//                                                                                                |    .         |
//                                                                                                |    .         |
//                                                                                                |    .         |
//                                                                                                | v^K          |  
//                                                                                                |    .         |
//                                                                                                |    .         |
//                                                                                                |    .         |
//                                                                                                | v^3          |
//     |                                                                                          | u^2          |
//Mc = | Cxk0 C(k-1)1 Cx(k-2)2  ... Cxk0 ... Cx03 Cx20 Cx11 Cx02 Cx10 Cx01 Cx00               | * | uv           |
//     | Cyk0 Cy(k-1)1 Cy(k-2)2 ... Cyk0 ... Cy03 Cy20 Cy11 Cy02 Cy10 Cy01 Cy00               |   | v^2          |
//     |                                                                                          | u            |
//     |                                                                                          | v            |
//                                                                                                | 1            |
//
//
//
//
//
//
//
//
//
//
//
//
//
//同样利用最小距离法估算12个转换系数
//对于N个校正点和估计点的距离平方总和
//    N
//S = Σ|Pe(i) - Pa(i)|^2
//    i=1
//
//    N
//S = Σ[(Ye(i) - Ya(i))^2 + (Xe(i) - Xa(i))^2]
//    i=1
//
//其中
//Xe(i) =Σ[Cxpq* u(i)p * v(i)^q]
//       0<=p+q<=3
//
//Ye(i) = Σ[Cypq* u(i)^p * v(i)^q]
//        0<=p+q<=3
//
//
//S取最小值应满足,S对各个系数量的导数等于0
//
//0 <= m+n <= 4
//
//           N
//ds/dCxmn = Σ {[(Xe(i) - Xa(i)]*u(i)^m * v(i)^n}
//           i=1
//
//           N
//         = Σ <{ Σ[Cxpq* u(i)^p * v(i)^q] - Xa(i) } *u(i)^m * v(i)^n >
//           i=1  0<=p+q<=4
//         =0;
// ==>
//                        N                                                   N
//ds/dCxmn =      Σ    {Σ [u(i)^m * v(i)^n  * u(i)^p * v(i)^q } * Cxpq   =  Σ[Xa(i) * u(i)^m * v(i)^n]
//           0<=p+q<=k   i=1                                                  i=1
//得出第一组10元一次方程组
//
//
//                       N                                                     N
//ds/dCymn =      Σ    {Σ [u(i)^m * v(i)^n  * u(i)^p * v(i)^q } * Cypq   =  Σ[Ya(i) * u(i)^m * v(i)^n]
//           0<=p+q<=k   i=1                                                  i=1
//得出第二组10元一次方程组
//
//
//
//
//
//
//
//author:toxuke@gmail.com
//date  :2011-03-31

//




template<int N>
class OrderCombinator
{
public:

    OrderCombinator()
    {
        int index = 0;

        for(int q = N; q>=0; q--)
        {
            for(int p=q; p>=0; p--)
            {
                allOrderCombine[index].p = p;
                allOrderCombine[index].q = q-p;

                index++;
            }


        }

    }

    struct comb
    {
        int p;
        int q;
    };

    static const int COEF_NUMBER = typename CoefCounter<N>::count;

    //所有的幂次组合
    comb  allOrderCombine[COEF_NUMBER];

};

//template<int K>
//BOOL CVideToScreenMap::Calibrate_LSLK(int N)
//{
//	BOOL bRet = TRUE;
//	static const int COEF_NUMBER = CoefCounter<K>::count;
//	CLinearEquation<double, COEF_NUMBER>  eqation;
//	CLinearEquation<double, COEF_NUMBER>::CoefMatrix coefM;
//	CLinearEquation<double, COEF_NUMBER>::VecB Bx;
//	CLinearEquation<double, COEF_NUMBER>::VecB By;
//	CLinearEquation<double, COEF_NUMBER>::VecResult result;
//	Matrix<double, 2, COEF_NUMBER> convMatrix;//转换矩阵
//
//	OrderCombinator<K> orderCombine;
//	
//	int eqv,m,n,p,q,k,l,col;
//	const std::vector<CalibratePair> & vecAdjustData = m_tCalibrateData.vecCalibratePoints;
//	//int N = MANUAL_CALIBRATE_POINT_NUMBER;
//
//	for(eqv = 0; eqv < COEF_NUMBER; eqv++)
//	{
//		m = orderCombine.allOrderCombine[eqv].p;
//		n = orderCombine.allOrderCombine[eqv].q;
//
//		for(col = 0; col < COEF_NUMBER; col++)
//		{
//
//			p = orderCombine.allOrderCombine[col].p;
//			q = orderCombine.allOrderCombine[col].q;
//           
//			Bx[eqv]=0;
//			By[eqv]=0;
//			coefM[eqv][col] = 0;
//
//			for(k=0; k<N; k++)
//			{
//				double u = (double)vecAdjustData[k].second.x ;
//				double v = (double)vecAdjustData[k].second.y ;
//				double X = (double)vecAdjustData[k].first .x ;
//				double Y = (double)vecAdjustData[k].first .y ;
//
//				coefM[eqv][col] += POW(u, m+p) * POW(v,n+q);
//				Bx[eqv] +=  X * POW(u,m) * POW(v,n); 
//				By[eqv] +=  Y * POW(u,m) * POW(v,n);
// 
//			}
//
//		}//for(col)
//
//	}//for(eqv)
//
//
//	//依照方程组[1]计算Cx系数
//	eqation.InitCoef(coefM, Bx);
//
//	if(!eqation.CalcResult2(result))
//	{
//		LOG_ERR("The first equation group has error!");
//		return FALSE;
//	}
//
//	LOG("X coefficents:");
//	for(l=1; l<=COEF_NUMBER; l++)
//	{
//		LOG("C%d = %E, ", l, result[l-1]);
//	}
//	LOG("\r\n");
//
//	convMatrix[0] = result;
//
//	//依照方程组[2]计算Cy系数
//	eqation.InitCoef(coefM, By);
//
//	if(!eqation.CalcResult2(result))
//	{
//		LOG_ERR("The second equation group has error!");
//		return FALSE;
//	}
//
//	LOG("Y coefficents:");
//	for(l=1; l<=COEF_NUMBER; l++)
//	{
//		LOG("C%d = %E, ", l, result[l-1]);
//	}
//	LOG("\r\n");
//
//	convMatrix[1] = result;
//
//
//	//计算每个视频坐标对应的屏幕坐标(归一化到0~65535)
//
//	//视频剪切区域, 只在此区域内寻找光笔的输入目标
//	int nScreenWidth  = m_tCalibrateData.nCalibrateScreenWidth;
//	int nScreenHeight = m_tCalibrateData.nCalibrateScreenHeight;
//
//	int nHalfScreenWidth  = nScreenWidth/2;
//	int nHalfScreenHeight = nScreenHeight/2;
//
//	int nVideoWidth  = m_tCalibrateData.nCalibrateImageWidth;
//	int nVideoHeight = m_tCalibrateData.nCalibrateImageHeight;
//
//	RECT rcVideoClipArea;
//	rcVideoClipArea.left   = nVideoWidth;
//	rcVideoClipArea.right  = 0;
//	rcVideoClipArea.top    = nVideoHeight;
//	rcVideoClipArea.bottom = 0;
//
//	for(int v = 0; v < nVideoHeight; v ++)
//	{
//		for(int u = 0; u < nVideoWidth; u ++)
//		{
//			Matrix<double, 2, COEF_NUMBER>::RowType vecCoordInput;//1X15
//			Matrix<double, 2, COEF_NUMBER>::ColType vecCoordOutput;//1X2
//
//			double _u_ = (double)u ;
//			double _v_ = (double)v ;
//
//
//			for(col=0;col<COEF_NUMBER;col++)
//			{
//				p = orderCombine.allOrderCombine[col].p;
//				q = orderCombine.allOrderCombine[col].q;
//				
//				vecCoordInput[col] = POW(_u_, p) * POW(_v_, q);
//			}
//
//			vecCoordOutput = convMatrix*vecCoordInput;
//
//			POINT pt;
//			pt.x = (int)(vecCoordOutput [0]);
//			pt.y = (int)(vecCoordOutput [1]);
//
//
//			//决定屏幕在视频窗口中的外接矩形
//			if(pt.x >=0 && pt.x < nScreenWidth && pt.y >=0 && pt.y < nScreenHeight)
//			{
//				if(u < rcVideoClipArea.left)
//				{
//					rcVideoClipArea.left = u;
//				}
//
//				if(u > rcVideoClipArea.right)
//				{
//					rcVideoClipArea.right = u;
//				}
//
//				if(v < rcVideoClipArea.top)
//				{
//					rcVideoClipArea.top = v;
//				}
//
//				if(v > rcVideoClipArea.bottom)
//				{
//					rcVideoClipArea.bottom = v;
//				}
//
//			}
//			////决定屏幕在视频窗口中的上下边界
//			//if(pt.y >=0 && pt.y < nScreenHeight)
//			//{
//			//	if(v < rcVideoClipArea.top)
//			//	{
//			//		rcVideoClipArea.top = v;
//			//	}
//
//			//	if(v > rcVideoClipArea.bottom)
//			//	{
//			//		rcVideoClipArea.bottom = v;
//			//	}
//			//}
//
//			if(pt.x < 0) pt.x = 0;
//			if(pt.x >= nScreenWidth) pt.x = nScreenWidth - 1;
//			if(pt.y <0) pt.x  = 0;
//			if(pt.y >=nScreenHeight) pt.y = nScreenHeight - 1;
//
//			pt.x =  (pt.x * 65535 +  nHalfScreenWidth /*四舍五入*/)/ nScreenWidth;
//			pt.y =  (pt.y * 65535 +  nHalfScreenHeight/*四舍五入*/)/ nScreenHeight;
//
//			m_pMapData[u + v * nVideoWidth] = pt;
//		}
//	}
//
//	m_rcVideoClipArea = rcVideoClipArea;
//
//	LOG("Video Clip Area (left=%d, right=%d, top=%d, bottom=%d)",
//		rcVideoClipArea.left,
//		rcVideoClipArea.right,
//		rcVideoClipArea.top,
//		rcVideoClipArea.bottom);
//
//	AtlTrace(_T("Video Clip Area (left=%d, right=%d, top=%d, bottom=%d)\r\n"),
//		rcVideoClipArea.left,
//		rcVideoClipArea.right,
//		rcVideoClipArea.top,
//		rcVideoClipArea.bottom);
//
//
//	//验证经过转化矩阵计算的估计屏幕坐标和实际的校正点之间的差值,Δx,Δy
//
//	for(int l=0; l<N; l++)
//	{
//			double u = (double)vecAdjustData[l].second.x ;
//			double v = (double)vecAdjustData[l].second.y ;
//
//			Matrix<double, 2, COEF_NUMBER>::RowType vecCoordInput;
//			Matrix<double, 2, COEF_NUMBER>::ColType vecCoordOutput;
//			
//			for(col=0;col<COEF_NUMBER;col++)
//			{
//				p = orderCombine.allOrderCombine[col].p;
//				q = orderCombine.allOrderCombine[col].q;
//				
//				vecCoordInput[col] = POW(u, p) * POW(v, q);
//			}
//
//			vecCoordOutput = convMatrix*vecCoordInput;
//
//
//		LOG("[template Calibrate_LSL%d]Adjust Point <%d> Image Coord = <%8.5f, %8.5f> Optimized Screen Coord = <%8.5f,%8.5f>, Actual Screen Coord = <%8.5f,%8.5f>, offset=<%8.5f,%8.5f>",
//				 K,
//			     l+1,
//				 (double)u,
//				 (double)v,
//				 (double)vecCoordOutput[0],
//				 (double)vecCoordOutput[1],
//				 (double)vecAdjustData[l].first.x,
//				 (double)vecAdjustData[l].first.y,
//				 (double)(vecCoordOutput[0] - vecAdjustData[l].first.x),
//				 (double)(vecCoordOutput[1] - vecAdjustData[l].first.y)
//				 );
//	}
//
//
//
//
//
//	bRet = TRUE;
//	return bRet;
//}

//extern CVideToScreenMap g_oVideo2ScreenMap;