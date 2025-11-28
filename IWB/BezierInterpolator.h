#pragma once
#include <assert.h>
#include <math.h>


//分层次的插值结果
//比如两条笔迹A, B
//分别要插入3个点(P0,P1,P2)，和2个点(Q0,Q1)
//往驱动发送插值点时，同一条笔迹相邻两时刻两个插值点之间需要延时1ms
//插值点发送顺序为
// P0,Q0
//sleep(1)
// P1,Q1
//sleep(2)
// P2
//
template <int SLOT_COUNT, int MAX_ELEMENT_COUNT_EACH_SLOT, class T>
class CSlotContainer
{
public:
    CSlotContainer()
    {
        Reset();
    }
    void Reset()
    {
        for (int i = 0; i < SLOT_COUNT; i++)
        {
            m_count[i] = 0;
        }
    }


    BOOL Add(int nSlotId, const T& data)
    {
        if (nSlotId < 0 || nSlotId >= SLOT_COUNT)
            return FALSE;

        if (m_count[nSlotId] > MAX_ELEMENT_COUNT_EACH_SLOT)
        {
            return FALSE;
        }
        
        m_data[nSlotId][m_count[nSlotId]] = data;
        m_count[nSlotId] ++;

        return TRUE;
    }

    int GetSlotData(int nSlotId, const T** ppData) const
    {
        if (nSlotId < 0 || nSlotId >= SLOT_COUNT)
            return 0;

        if(ppData)
            *ppData = &m_data[nSlotId][0];

        return m_count[nSlotId];
    }

    int  GetSlotCount() const
    { 
        return SLOT_COUNT; 
    }



protected:
    T    m_data [SLOT_COUNT][MAX_ELEMENT_COUNT_EACH_SLOT];
    int  m_count[SLOT_COUNT];
};

//@功能:根据三个结点的坐标，计算出Bezier曲线的两个控制点坐标。
//@参数:
//      ratio_bottom, ∈(0,1),The  percentage between  the minimum length of a control point handle and
//                    the length of a curve which the control point handle describes
//
//      ratio_top, ∈(0,1), the maximum lengt of such a control point handle.
//
inline BOOL CalculateControlPoint(double ratio_bottom, double ratio_top, const TPoint2D junctionPoints[3], TPoint2D controlPoints[2])
{
    //v_1 = P_0 - P_1
    //v_2 = P_2 - P_1
    //cosα = dot(V_1, V_2) / ‖V_1 ‖‖V_2 ‖
    //α = acos⁡(cosα) / π
    //λ = α*(ratio_top - ratio_low) + ratio_low
    //C_0 = P_1 + (P_0 - P_1)*λ
    //C_1 = P_1 + (P_2 - P_1)*λ
    assert(
        ratio_bottom < ratio_top
        &&
        0.0 <= ratio_bottom
        &&
        ratio_bottom < 1.0
        &&
        0.0 <= ratio_top
        &&
        ratio_top < 1.0);

    TPoint2D v1, v2, v3, v4;
    const TPoint2D& P0 = junctionPoints[0];
    const TPoint2D& P1 = junctionPoints[1];
    const TPoint2D& P2 = junctionPoints[2];
    v1 = P0 - P1;
    v2 = P2 - P1;

    double norm_v1_square = (v1.d[0] * v1.d[0] + v1.d[1] * v1.d[1]);
    double norm_v2_square = (v2.d[0] * v2.d[0] + v2.d[1] * v2.d[1]);
    double L1 = sqrt(norm_v1_square);
    double L2 = sqrt(norm_v2_square);

    if (norm_v1_square < std::numeric_limits<double>::epsilon() || norm_v2_square < std::numeric_limits<double>::epsilon())
    {
        return FALSE;
    }


    v3 = P0 - P2;
    double norm_v3 = norm(v3);


    if (norm_v3 < std::numeric_limits<double>::epsilon())
    {
        return FALSE;
    }

    v3 /= norm_v3;
    v4 = (-1.0)*v3;

    //double cos_alpha = (v1.d[0] * v2.d[0] + v1.d[1] * v2.d[1])/(norm_v1 * norm_v2);
    double dot = (v1.d[0] * v2.d[0] + v1.d[1] * v2.d[1]);
    double dot_square = dot*dot;
    double cos_alpha_square = dot_square / (norm_v1_square*norm_v2_square);
    double cos_alpha = sqrt(cos_alpha_square);

    if (cos_alpha > 1.0)
    {
        cos_alpha = 1.0;
    }

    if (dot < 0) cos_alpha = -cos_alpha;


    double alpha = acos(cos_alpha) / M_PI;
    double lambda = alpha*(ratio_top - ratio_bottom) + ratio_bottom;



    controlPoints[0].d[0] = P1.d[0] + v3.d[0]*lambda*L1;
    controlPoints[0].d[1] = P1.d[1] + v3.d[1]*lambda*L1;

    controlPoints[1].d[0] = P1.d[0] + v4.d[0]*lambda*L2;
    controlPoints[1].d[1] = P1.d[1] + v4.d[1]*lambda*L2;


    return TRUE;
}

//@功能:实现鼠标位置插值功能的插值器
template <int MAX_STROKE_NUM>
class CBezierInterpolator
{
public:
    CBezierInterpolator()
        :
        RATIO_TOP(0.35),
        RATIO_BOTTOM(0.1)
    {
        Reset();
    }

    ~CBezierInterpolator()
    {
    
    }

    //@功  能:实时进行触控点的Bezier插值
    //@参  数:pContactInfo, 触控点位置信息
    //        nCount, 触控点个数
    //@返回值:增加的插入点, 插入点包括括值基点
    int DoInterpolate(const TContactInfo* pContactInfos, int nCount)
    {
        
        m_interpolatePoints.Reset();

        for (int i = 0; i < nCount; i++)
        {
            const TContactInfo& contactInfo = pContactInfos[i];

            UINT32 uId = contactInfo.uId;

            if (uId >= MAX_STROKE_NUM)
            {//数组下标越界。
                continue;
            }

            TDataForStrokeInterpolate& strokeInterpolateData = m_aryDataForStrokeInterpolate[uId];
            EStrokeInterpolateState eState = strokeInterpolateData.state;
            int interpolate_count = 0;
            double t = 0.0;
            double t_inc = 0.0;
            switch (eState)
            {
            case E_INTERPOLATE_IDLE://空闲状态
                if (contactInfo.ePenState == E_PEN_STATE_DOWN)
                {
                    strokeInterpolateData.junctionPoints[0].d[0] = (double)contactInfo.pt.x;
                    strokeInterpolateData.junctionPoints[0].d[1] = (double)contactInfo.pt.y;
                    strokeInterpolateData.state = E_FIRST_JUNCT_READY;
                }
            
                //输出第一点
                AddInterpolatePoint(0, contactInfo);

                break;

            case E_FIRST_JUNCT_READY://第一个笔迹结点就绪

                if (contactInfo.ePenState == E_PEN_STATE_UP)
                {
                    //输出当前触控
                    AddInterpolatePoint(0, contactInfo);
                    
                    //转到插值空闲状态
                    strokeInterpolateData.state = E_INTERPOLATE_IDLE;
                }
                else if (contactInfo.ePenState == E_PEN_STATE_DOWN)
                {
                    if (IsSamePoint(strokeInterpolateData.junctionPoints[0], contactInfo.pt))
                    {	//同一个点，输出这个点，状态保持不变
                        AddInterpolatePoint(0, contactInfo);
                        break;
                    }
                    else
                    {   
                        strokeInterpolateData.junctionPoints[1].d[0] = (double)contactInfo.pt.x;
                        strokeInterpolateData.junctionPoints[1].d[1] = (double)contactInfo.pt.y;
                        strokeInterpolateData.state = E_INTERPOLATE_FIRST_SEGMENT;
                    }
                }
                
                break;


            case E_INTERPOLATE_FIRST_SEGMENT://插值第一条曲线,只使用一个控制点进行二阶Bezier插值

                if (contactInfo.ePenState == E_PEN_STATE_UP)
                {//无法插值直接输出P1, P2
                    TContactInfo ciP1;
                    ciP1.ePenState = E_PEN_STATE_DOWN;
                    ciP1.uId = uId;
                    ciP1.pt.x = int(strokeInterpolateData.junctionPoints[1].d[0]);
                    ciP1.pt.y = int(strokeInterpolateData.junctionPoints[1].d[1]);
                    //ciP1.bIgnored = FALSE;

                    AddInterpolatePoint(0, ciP1);
                    
                    //!!!!输出弹起事件
                    AddInterpolatePoint(1, contactInfo);

                    strokeInterpolateData.state = E_INTERPOLATE_IDLE;

                }
                else if (contactInfo.ePenState == E_PEN_STATE_DOWN)
                {
                    if (IsSamePoint(strokeInterpolateData.junctionPoints[1], contactInfo.pt)
                        ||
                        IsSamePoint(strokeInterpolateData.junctionPoints[0], contactInfo.pt)
                        )
                    {//当前输入触控点和junctionPoints[1]为同一个点，则跳过插值，直接输出junctionPoints[1]
                     //另外一种极端情形:
                    //juntionPoints[0]和当前输入触控点为同一点，

                        TContactInfo ciP1;
                        ciP1.ePenState = E_PEN_STATE_DOWN;
                        ciP1.uId = uId;
                        ciP1.pt.x = int(strokeInterpolateData.junctionPoints[1].d[0]);
                        ciP1.pt.y = int(strokeInterpolateData.junctionPoints[1].d[1]);
                        //ciP1.bIgnored = FALSE;

                        //无法插值直接输出P1, P2
                        AddInterpolatePoint(0, ciP1);
                        AddInterpolatePoint(1, contactInfo);

                        strokeInterpolateData.junctionPoints[0].d[0] = (double)contactInfo.pt.x;
                        strokeInterpolateData.junctionPoints[0].d[1] = (double)contactInfo.pt.y;
                        strokeInterpolateData.state = E_FIRST_JUNCT_READY;
                    }
                    else
                    {
                        //
                        strokeInterpolateData.junctionPoints[2].d[0] = (double)contactInfo.pt.x;
                        strokeInterpolateData.junctionPoints[2].d[1] = (double)contactInfo.pt.y;

                        //计算控制点
                        BOOL bSuccess = CalculateControlPoint(RATIO_BOTTOM, RATIO_TOP, strokeInterpolateData.junctionPoints, strokeInterpolateData.controlPoints);

                        if (!bSuccess)
                        {
                            OutputDebugString(_T("Interpolate CalculateControlPoint failed!\n"));
                            break;
                        }


                        //计算插值点个数
                        interpolate_count = CalcInterpolateNumber(strokeInterpolateData.junctionPoints[0], strokeInterpolateData.junctionPoints[1]);

                        if(interpolate_count > 1)
                        t_inc = 1.0 /(double)(interpolate_count);
                        t = t_inc;

                        TContactInfo ciInterpolate = contactInfo;
                        TPoint2D& P0 = strokeInterpolateData.junctionPoints[0];
                        TPoint2D& P1 = strokeInterpolateData.junctionPoints[1];
                        TPoint2D& C0 = strokeInterpolateData.controlPoints[0];

                        //按照二阶Bezier曲线方程进行插值
                        //P(t) = (1 - t)^2*P0 + 2(1 - t)t*C0 + t ^ 2*P1
                        for (int j = 0; j < interpolate_count; j++)
                        {
                            double coef0 = (1 - t)*(1 - t);
                            double coef1 = 2*(1 - t)*t;
                            double coef2 = t*t;

                            ciInterpolate.pt.x = int(coef0*P0.d[0] + coef1*C0.d[0] + coef2*P1.d[0] + 0.5);
                            ciInterpolate.pt.y = int(coef0*P0.d[1] + coef1*C0.d[1] + coef2*P1.d[1] + 0.5);

                            AddInterpolatePoint(j, ciInterpolate);

                            t += t_inc;

                        }


                        //C0 <= C1
                        strokeInterpolateData.controlPoints[0] = strokeInterpolateData.controlPoints[1];

                        //P0 <= P1, P1 <= P2
                        strokeInterpolateData.junctionPoints[0] = strokeInterpolateData.junctionPoints[1];
                        strokeInterpolateData.junctionPoints[1] = strokeInterpolateData.junctionPoints[2];

                        strokeInterpolateData.state = E_INTERPOLATE_INTERM_SEGMENT;
                    }
                }
                break;


            case E_INTERPOLATE_INTERM_SEGMENT://插值中间线段,每条线段可以使用两个控制点进行三阶Bezier插值
                if(contactInfo.ePenState == E_PEN_STATE_UP)
                {   //光笔弹起，插值最后一条线段

                    //计算插值点个数
                    interpolate_count = CalcInterpolateNumber(strokeInterpolateData.junctionPoints[0], strokeInterpolateData.junctionPoints[1]);

                    if (interpolate_count >= MAX_INEROLATE_EACH_STROKE)
                    {//为弹起事件留出存储空间
                        interpolate_count = MAX_INEROLATE_EACH_STROKE - 1;
                    }

                    
                    if (interpolate_count > 1)	t_inc = 1.0 / (double)interpolate_count;
                    t = t_inc;

                    TContactInfo ciInterpolate;
                    ciInterpolate.uId = uId;
                    ciInterpolate.ePenState = E_PEN_STATE_DOWN;
    

                    TPoint2D& P0 = strokeInterpolateData.junctionPoints[0];
                    TPoint2D& P1 = strokeInterpolateData.junctionPoints[1];
                    TPoint2D& C0 = strokeInterpolateData.controlPoints[0];

                    //按照二阶Bezier曲线方程进行插值
                    //P(t) = (1 - t)^2*P0 + 2(1 - t)t*C0 + t ^ 2*P1
                    for (int j = 0; j < interpolate_count; j++)
                    {
                        double coef0 = (1 - t)*(1 - t);
                        double coef1 = 2 * (1 - t)*t;
                        double coef2 = t*t;

                        ciInterpolate.pt.x = int(coef0*P0.d[0] + coef1*C0.d[0] + coef2*P1.d[0] + 0.5);
                        ciInterpolate.pt.y = int(coef0*P0.d[1] + coef1*C0.d[1] + coef2*P1.d[1] + 0.5);

                        AddInterpolatePoint(j, ciInterpolate);

                        t += t_inc;

                    }

                    //!!!!输出弹起事件
                    AddInterpolatePoint(interpolate_count, contactInfo);

                    strokeInterpolateData.state = E_INTERPOLATE_IDLE;


                }
                else if (contactInfo.ePenState == E_PEN_STATE_DOWN)
                {
                    if (IsSamePoint(strokeInterpolateData.junctionPoints[1], contactInfo.pt))
                    {
                       // 当前输入触控点和junctionPoints[1]为同一个点，则跳过插值，直接输出junctionPoints[1]
                       //计算插值点个数
                        interpolate_count = CalcInterpolateNumber(strokeInterpolateData.junctionPoints[0], strokeInterpolateData.junctionPoints[1]);

                        if (interpolate_count > 1)	t_inc = 1.0 / (double)interpolate_count;
                        t = t_inc;

                        TPoint2D& P0 = strokeInterpolateData.junctionPoints[0];
                        TPoint2D& P1 = strokeInterpolateData.junctionPoints[1];
                        TPoint2D& C0 = strokeInterpolateData.controlPoints[0];

                        TContactInfo ciInterpolate;
                        ciInterpolate.uId = uId;
                        ciInterpolate.ePenState = E_PEN_STATE_DOWN;


                        //按照二阶Bezier曲线方程进行插值
                        //P(t) = (1 - t)^2*P0 + 2(1 - t)t*C0 + t ^ 2*P1
                        for (int j = 0; j < interpolate_count; j++)
                        {
                            double coef0 = (1 - t)*(1 - t);
                            double coef1 = 2 * (1 - t)*t;
                            double coef2 = t*t;

                            ciInterpolate.pt.x = int(coef0*P0.d[0] + coef1*C0.d[0] + coef2*P1.d[0] + 0.5);
                            ciInterpolate.pt.y = int(coef0*P0.d[1] + coef1*C0.d[1] + coef2*P1.d[1] + 0.5);

                            AddInterpolatePoint(j, ciInterpolate);

                            t += t_inc;
                        }//for

                        strokeInterpolateData.junctionPoints[0].d[0] = (double)contactInfo.pt.x;
                        strokeInterpolateData.junctionPoints[0].d[1] = (double)contactInfo.pt.y;

                        strokeInterpolateData.state = E_FIRST_JUNCT_READY;

                    }
                    else if (IsSamePoint(strokeInterpolateData.junctionPoints[0], contactInfo.pt))
                    {//另外一种极端情形:
                     //juntionPoints[0]和当前输入触控点为同一点，则不能做插值操作，直接输出中间结点和当前输入点
                        TContactInfo ciP1;
                        ciP1.uId = uId;
                        ciP1.ePenState = E_PEN_STATE_DOWN;
                        ciP1.pt.x = int(strokeInterpolateData.junctionPoints[1].d[0]);
                        ciP1.pt.y = int(strokeInterpolateData.junctionPoints[1].d[1]);

                        //Add ciP1
                        AddInterpolatePoint(0, ciP1);
                        AddInterpolatePoint(1, contactInfo);

                        strokeInterpolateData.junctionPoints[0].d[0] = (double)contactInfo.pt.x;
                        strokeInterpolateData.junctionPoints[0].d[1] = (double)contactInfo.pt.y;
                        strokeInterpolateData.state = E_FIRST_JUNCT_READY;
                    }
                    else
                    {
                        //光笔按下
                        strokeInterpolateData.junctionPoints[2].d[0] = (double)contactInfo.pt.x;
                        strokeInterpolateData.junctionPoints[2].d[1] = (double)contactInfo.pt.y;

                        //计算控制点
                        BOOL bSuccess = CalculateControlPoint(RATIO_BOTTOM, RATIO_TOP, strokeInterpolateData.junctionPoints, &strokeInterpolateData.controlPoints[1]);

                        if (!bSuccess)
                        {
                            OutputDebugString(_T("Interpolate CalculateControlPoint failed!\n"));
                            break;
                        }


                        //计算插值点个数
                        interpolate_count = CalcInterpolateNumber(strokeInterpolateData.junctionPoints[0], strokeInterpolateData.junctionPoints[1]);

                        if (interpolate_count > 1)	t_inc = 1.0 / (double)interpolate_count;
                        t = t_inc;
                        TPoint2D& P0 = strokeInterpolateData.junctionPoints[0];
                        TPoint2D& P1 = strokeInterpolateData.junctionPoints[1];
                        TPoint2D& C0 = strokeInterpolateData.controlPoints[0];
                        TPoint2D& C1 = strokeInterpolateData.controlPoints[1];

                        TContactInfo ciInterpolate;
                        ciInterpolate.uId = uId;
                        ciInterpolate.ePenState = E_PEN_STATE_DOWN;


                        //按照三阶Bezier曲线方程进行插值
                        //P(t) = (1 - t)^3*P0 + 3(1 - t)^2* t*C0 + 3*(1 - t)*t^2*C1 + t^3*P1
                        for (int j = 0; j < interpolate_count; j++)
                        {
                            double coef0 = (1 - t)*(1 - t)*(1-t);
                            double coef1 = 3 * (1 - t)*(1 - t)*t;
                            double coef2 = 3 * (1 - t)*t*t;
                            double coef3 = t*t*t;

                            ciInterpolate.pt.x = int(coef0*P0.d[0] + coef1*C0.d[0] + coef2*C1.d[0] + coef3*P1.d[0] + 0.5);
                            ciInterpolate.pt.y = int(coef0*P0.d[1] + coef1*C0.d[1] + coef2*C1.d[1] + coef3*P1.d[1] + 0.5);

                            AddInterpolatePoint(j, ciInterpolate);
                            t += t_inc;
                        }

                        //C0 <= C2
                        strokeInterpolateData.controlPoints[0] = strokeInterpolateData.controlPoints[2];

                        //P0 <= P1, P1 <= P2
                        strokeInterpolateData.junctionPoints[0] = strokeInterpolateData.junctionPoints[1];
                        strokeInterpolateData.junctionPoints[1] = strokeInterpolateData.junctionPoints[2];
                    }
                }

                break;
            }//switch

        }//for-each stroke

        return 0;
    }


    //@功能:复位插值对象
    //@说明:在鼠标左键按下时,才进行鼠标插值, 
    //      当鼠标左键弹起时,要清空插值基点序列
    void Reset()
    {

        for (int i = 0; i < MAX_STROKE_NUM; i++)
        {
            m_aryDataForStrokeInterpolate[i].state = E_INTERPOLATE_IDLE;

            for (int j = 0; j < 3; j++)
            {
                m_aryDataForStrokeInterpolate[i].junctionPoints[j].d[0] = 0.0;
                m_aryDataForStrokeInterpolate[i].junctionPoints[j].d[1] = 0.0;

                m_aryDataForStrokeInterpolate[i].controlPoints [j].d[0]  = 0.0;
                m_aryDataForStrokeInterpolate[i].controlPoints [j].d[1]  = 0.0;
            }
        }

        m_interpolatePoints.Reset();
    }


protected:
    static const int MAX_INEROLATE_EACH_STROKE = 3;


public:
    typedef
        CSlotContainer<MAX_INEROLATE_EACH_STROKE, MAX_STROKE_NUM, TContactInfo> InterpolateContainer;


    const InterpolateContainer& GetInterpolateResults()
    {
        return m_interpolatePoints;
    }
protected://成员变量

    enum  EStrokeInterpolateState
    {
        E_INTERPOLATE_IDLE,           //空闲状态
        E_FIRST_JUNCT_READY,          //第一个笔迹结点就绪
        //E_SECOND_JUNCT_READY,       //第二个并笔迹结点就绪
        E_INTERPOLATE_FIRST_SEGMENT,  //插值第一条线段
        E_INTERPOLATE_INTERM_SEGMENT,//插值中间线段
    };

    
    struct TDataForStrokeInterpolate
    {
        TPoint2D junctionPoints[3];
        TPoint2D controlPoints [3];
        EStrokeInterpolateState state;
    } m_aryDataForStrokeInterpolate[MAX_STROKE_NUM];





    //说明：
    //每条笔迹只能在一个Time Slot中插入一个点
    //每个Time Slot中保存不同笔迹在该Time Slot中插入的点。
    InterpolateContainer m_interpolatePoints;

    const double RATIO_TOP   ;
    const double RATIO_BOTTOM;

protected://成员函数
    void AddInterpolatePoint(int nSlot,const TContactInfo& ci)
    {
        /*
        static int s_debug = 0;
        s_debug++;
        TCHAR buffer[256];
        _stprintf_s(buffer, _countof(buffer), _T("[my_debug %03d],x=%d,y=%d\n"), s_debug, ci.pt.x, ci.pt.y);
        OutputDebugString(buffer);
        */

        m_interpolatePoints.Add(nSlot, ci);

    }


    int CalcInterpolateNumber(TPoint2D p1, TPoint2D p2)
    {
        int dx = abs(int(p2.d[0] - p1.d[0])) + 1;
        int dy = abs(int(p2.d[1] - p1.d[1])) + 1;

         int t = max(dx, dy);

        t = min(MAX_INEROLATE_EACH_STROKE, t);

        return t;

    }

    BOOL IsSamePoint(const TPoint2D& left, const POINT& right)
    {
        double dx = left.d[0] - double(right.x);
        double dy = left.d[1] - double(right.y);

        if (fabs(dx) >= 1.0 || fabs(dy) >= 1.0)
            return FALSE;

        return TRUE;
    }
};
