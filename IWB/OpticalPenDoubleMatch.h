#pragma once
#include <assert.h>

//{begin}, 2014/07/08
//@功能:笔的标识信息
struct TPenIdentity
{
    int    nPenNo;//笔编号, 从0开始的编号
    UINT   wMsg  ;//关联的消息编号
    BOOL   bAvail;//可用标志
};


//MaxPenCount, 笔的最大支数
//FirstPenMessage, 第一支笔关联的消息
//MsgCountEachPen, 每支笔关联的消息的个数
template<unsigned int uMaxPenCount, unsigned int uFirstPenMessage, unsigned int uMsgCountEachPen>
class CPenResource
{
public:

    CPenResource()
    {
        unsigned int uMsg =  uFirstPenMessage;
        for(unsigned int i=0; i < _countof(m_aryIdentity); i++)
        {
            m_aryIdentity[i].nPenNo = i;
            m_aryIdentity[i].wMsg   = uMsg;
            m_aryIdentity[i].bAvail = TRUE;
            uMsg += uMsgCountEachPen;
        }
    }

    //@功  能:请求笔的身份信息
    //@返回值:失败返回UINT(-1)
    //        否则返回找到的第一支可用笔的编号
    //@功能:该方法线程安全
    UINT RequestPenIdentity()
    {
        CComCritSecLock<CComAutoCriticalSection> lock(m_csForResource);
        for(unsigned int i=0; i < _countof(m_aryIdentity); i++)
        {
            if(m_aryIdentity[i].bAvail)
            {
                m_aryIdentity[i].bAvail = FALSE;
                return i;
            }


        }
        return UINT(-1);
    }

    //@功  能:回收笔的编号信息
    //@返回值:失败返回FALSE
    //        否则返回TRUE
    //@功能:该方法线程安全
    BOOL RecyclePenIdentity(unsigned int nIndex)
    {
        CComCritSecLock<CComAutoCriticalSection> lock(m_csForResource);
        if(nIndex >= _countof(m_aryIdentity)) return FALSE;
        m_aryIdentity[nIndex].bAvail = TRUE;
		return TRUE;
    }


    const TPenIdentity& operator[](unsigned int nIndex)
    {
        return m_aryIdentity[nIndex];
    }

protected:
    TPenIdentity m_aryIdentity[uMaxPenCount];
    CComAutoCriticalSection m_csForResource ;
};
//{end}, 2014/07/08

class CVirtualPenInfo
{
public:
    CVirtualPenInfo()
        :
        m_bPenActive(FALSE),    //笔是否是活动的
        m_bPenMatched(FALSE),   //笔是否是匹配的
        m_nPenMissCounter(0),   //笔允许容错的次数
        //m_bFakeMouse(FALSE)  ,   //笔是否是模拟鼠标的
        m_eSimulMousePossageState(SIMUL_MOUSE_POSSAGE_STATE_IDLE),
        m_PenMsg(0),
        m_uPenNo(UINT(-1))
    {

    }
    ///////////////////////////////////////////////////////////////////////////////////////
    //@功能:使能光笔，并分配笔的身份信息
    //@功能:若分配身份信息失败则返回FALSE
    BOOL Enable()
    {
       m_uPenNo = m_PenResource.RequestPenIdentity();
       if(UINT(-1) == m_uPenNo)
       {
            return FALSE;
       }

       m_PenMsg = m_PenResource[m_uPenNo].wMsg;

        m_bPenActive = TRUE;
        m_nPenMissCounter  = 0;

		return TRUE;
    }
    /////////////////////////////////////////////////////////////////////////////////////
    void Disable()
    {
        m_PenResource.RecyclePenIdentity(m_uPenNo);
        m_uPenNo = UINT(-1);

        m_bPenActive = FALSE;
    }
    ////////////////////////////////////////////////////////////////////////////////////
    BOOL IsPenActive()
    {
        return m_bPenActive;
    }
    ////////////////////////////////////////////////////////////////////////////////////
    BOOL IsFakeMouse()
    {
        //编号为0的光笔模拟鼠标操作
        //其他编号的则要通过Windows消息进行虚拟
        return m_uPenNo != 0 ;
    }
    //////////////////////////////////////////////////////////////////////////////////
    //void SetSimMouse(BOOL bFakeMouse)
    //{
    //    m_bFakeMouse = bFakeMouse;
    //}


    /////////////////////////////////////////////////////////////////////////////////

    void SetHistoryPos(const POINT& Pos)
    {
        m_ptHistoryPos = Pos;
        m_bPenMatched = TRUE;
    }
    /////////////////////////////////////////////////////////////////////////////////
    const POINT& GetHistoryPos()const
    {
        return m_ptHistoryPos;
    }
    ////////////////////////////////////////////////////////////////////////////////

    void ClearMatchedFlag()
    {
        m_bPenMatched = FALSE;
    }
    ///////////////////////////////////////////////////////////////////////////////

    BOOL IsPenMatch()const
    {
        return m_bPenMatched;
    }
    //////////////////////////////////////////////////////////////////////////////
    void incMissCounter()
    {
        m_nPenMissCounter++;
    }
    ////////////////////////////////////////////////////////////////////////////

    int GetMissCounter()const
    {
        return m_nPenMissCounter;
    }
    void ReSetHistoryPos()
    {
        m_ptHistoryPos.x = 0;
        m_ptHistoryPos.y = 0;
        m_nPenMissCounter = 0;
    }
    void ReSetMissCount()
    {
        m_nPenMissCounter  = 0;
    }
    ////////////////////////////////////////////////////////////////////////////////笔的操作

    void SimulMouseMotion(HWND hwnd , const POINT *ptPencilPos)
    {
        BOOL bFindPenCil = TRUE;
        POINT SimulMousePoint;
        SimulMousePoint.x = 0;
        SimulMousePoint.y = 0;
        if (ptPencilPos == NULL)
        {
            bFindPenCil = FALSE ;
            if (!m_objStrokeSmoother.IsReset())
            {
                SimulMousePoint = m_objStrokeSmoother.Reset();
                //RunSimulStateMachine(TRUE, SimulMousePoint/*无效参数*/,hwnd);   //保证滤波后的最后一点不遗漏
            }
            RunSimulStateMachine(FALSE, SimulMousePoint, hwnd );    //提笔
        }
        else
        {
            SimulMousePoint = m_objStrokeSmoother.Smooth( *ptPencilPos);
            RunSimulStateMachine(bFindPenCil, SimulMousePoint,hwnd );
        }
    }



    void RunSimulStateMachine(BOOL bFindPen, const POINT &ptPencilPos, HWND hwnd )
    {

        POINT ptPencilPosTemp;
        ptPencilPosTemp.x = ptPencilPos.x;
        ptPencilPosTemp.y = ptPencilPos.y;

        switch(m_eSimulMousePossageState)
        {
        case SIMUL_MOUSE_POSSAGE_STATE_IDLE:
            {
                if (bFindPen)
                {
                    PostMessage(hwnd , m_PenMsg , ptPencilPosTemp.x , ptPencilPosTemp.y );
                    m_eSimulMousePossageState = SIMUL_MOUSE_POSSAGE_STATE_DOWN;
                }
                break;
            }
        case  SIMUL_MOUSE_POSSAGE_STATE_DOWN:
            {
                if (bFindPen)
                {
                    PostMessage(hwnd, m_PenMsg+1 , ptPencilPosTemp.x , ptPencilPosTemp.y );
                    m_eSimulMousePossageState = SIMUL_MOUSE_POSSAGE_STATE_MOVE;
                }
                else
                {
                    PostMessage(hwnd, m_PenMsg+2 , ptPencilPosTemp.x , ptPencilPosTemp.y );
                    m_eSimulMousePossageState = SIMUL_MOUSE_POSSAGE_STATE_IDLE;
                }

                break;
            }
        case SIMUL_MOUSE_POSSAGE_STATE_MOVE:
            {
                if (bFindPen)
                {
                    PostMessage(hwnd, m_PenMsg+1 , ptPencilPosTemp.x , ptPencilPosTemp.y );
                }
                else
                {
                    PostMessage(hwnd, m_PenMsg+2 , ptPencilPosTemp.x , ptPencilPosTemp.y );
                    m_eSimulMousePossageState = SIMUL_MOUSE_POSSAGE_STATE_IDLE;
                }
                break;
            }

        }  //switch

    }
    void SetPenMsge(UINT msg)
    {
        m_PenMsg = msg;	
    }
protected:
    POINT   m_ptHistoryPos      ;
    BOOL    m_bPenActive        ;
    BOOL    m_bPenMatched       ;
    int     m_nPenMissCounter   ;
    //BOOL   m_bFakeMouse         ;
    UINT   m_PenMsg             ;

    UINT   m_uPenNo             ;//笔的编号
    CStrokeSmoother<2> m_objStrokeSmoother;//笔画平滑器

    enum E_SIMUL_MOUSE_POSSAGE_STATE
    {
        SIMUL_MOUSE_POSSAGE_STATE_IDLE    ,
        SIMUL_MOUSE_POSSAGE_STATE_DOWN    ,
        SIMUL_MOUSE_POSSAGE_STATE_MOVE    ,
    }m_eSimulMousePossageState;

    //静态资源,笔的身份资源
    static CPenResource<10u, 1027u, 3u> m_PenResource;
};
_declspec(selectany)  CPenResource<10, 1027, 3> CVirtualPenInfo::m_PenResource;

template <int MaxPenCount>
class CMulPenMatch
{
public:
    CMulPenMatch(CMouseEventGenerator* pMouseEventGenerator)
        :
    m_bMulPenMatched(FALSE),
        m_pMouseEventGenerator(pMouseEventGenerator)
    {
        /*
        m_AryPen[0].SetPenMsge(1027);   //1
        m_AryPen[1].SetPenMsge(1030);   //2
        m_AryPen[2].SetPenMsge(1033);   //3
        m_AryPen[3].SetPenMsge(1036);   //4
        m_AryPen[4].SetPenMsge(1039);   //5
        m_AryPen[5].SetPenMsge(1042);   //6
        m_AryPen[6].SetPenMsge(1045);   //7
        m_AryPen[7].SetPenMsge(1048);   //8
        m_AryPen[8].SetPenMsge(1051);   //9
        m_AryPen[9].SetPenMsge(1054);   //10
        */
    }

    /////////////////////////////////////////////////////////////////////////////////
    ~CMulPenMatch()
    {
    }
    /////////////////////////////////////////////////////////////////////////////////
    CVirtualPenInfo& Getpen(int iIndex)
    {
        ASSERT(0 <= iIndex && iIndex < MaxPenCount);
        return m_AryPen[iIndex];
    }

    ////////////////////////////////////////////////////////////////////////////////
    ///////*********************************************************////////////////
    ////////////////////////////////////////////////////////////////////////////////@功能：匹配开始
    void BeforeMulPenMatch()
    {
        for(int i =0 ; i< MaxPenCount ; i++)
        {
            CVirtualPenInfo & MulPen = m_AryPen[i];
            if (MulPen.IsPenActive())                 //如果这个笔是活动的话，在开始之前，把这个的匹配改为不匹配的。这样，方便这次的查找。
            {
                MulPen.ClearMatchedFlag();
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    ///////*********************************************************////////////////
    ////////////////////////////////////////////////////////////////////////////////@功能：正在匹配
    void  DoMulMatch(HWND hwnd ,const POINT& ptNow)
    {
        int nMinDisTance = 3000;
        int iPenIndex = -1 ;

        for( int i = 0; i < MaxPenCount ; i++)
        {
            CVirtualPenInfo& MulPen = m_AryPen[i];
            ////////////////////首先查找有没有匹配的活动笔，如果有活动的笔。说明有笔在运行。
            if (!MulPen.IsPenActive())
            {
                continue;
            }
            const POINT& ptHistory = MulPen.GetHistoryPos();
            int nx =(ptNow.x - ptHistory.x) ;
            int ny =(ptNow.y - ptHistory.y) ;

            int nSquareDistance = nx* nx + ny * ny;

            if (nSquareDistance < nMinDisTance)
            {
                iPenIndex = i;
                nMinDisTance = nSquareDistance;
            }
        } //for

        if (iPenIndex != -1)              //说明找到了活动笔的信息
        {
            CVirtualPenInfo& MulPen = m_AryPen[iPenIndex];
            if (!MulPen.IsPenMatch())
            {
                MulPen.SetHistoryPos(ptNow);   //m_bMatch 置为 TURE  .找到匹配的信息了，就需要发送消息了。
                MulPen.ReSetMissCount();

                //找到活动笔的话就需要判断这支笔是模拟什么响应的
                if(!MulPen.IsFakeMouse())
                {
                    ////////说明是响应鼠标的。在这里添加鼠标的信息
                    this->m_pMouseEventGenerator->GenMouseEvent(
                        &ptNow
#ifdef _DEBUG
                        ,true
#endif 					
                        );
                }
                else
                {
                    MulPen.SimulMouseMotion(hwnd, &ptNow);   //找到匹配的发送消息就可。
                }
            }
        } //if

        else//没有活动的笔或者是没有与之匹配的笔的信息
        {
            iPenIndex = -1;
            int iMouseIndex = -1;

            for (int i = 0; i < MaxPenCount ; i++)
            {
                CVirtualPenInfo &MulPen = m_AryPen[i];
                if (!MulPen.IsPenActive())
                {
                    iPenIndex = i;
                    break;
                }
            }

            if (iPenIndex != -1)    //找到没有活动的笔
            {
                CVirtualPenInfo &pen = Getpen(iPenIndex);

                if(!pen.Enable()) return;//使能失败立即返回

                pen.SetHistoryPos(ptNow);
                pen.ReSetMissCount();

                /*
                int iMouseIndex = -1;
                for(int j = 0 ; j < MaxPenCount ; j++)
                {
                    CVirtualPenInfo & pen =  m_AryPen[j];

                    if (pen.IsFakeMouse())    //说明已经存在着鼠标响应了
                    {
                        iMouseIndex = j;
                        break;
                    }
                }*/

            

//                if (iMouseIndex != -1)    //说明鼠标响应已经被占用了。
//                {
//                    pen.SimulMouseMotion(hwnd, &ptNow );	
//                }
//                else                       //说明鼠标响应没有被用到
//                {
//                    pen.SetFakeMouse(TRUE);
//                    //g_oMouseEventGen.GenMouseEvent(
//                    this->m_pMouseEventGenerator->GenMouseEvent(
//                        &ptNow
//#ifdef _DEBUG
//                        ,true
//#endif 					
//                        );
//                }


                if (pen.IsFakeMouse())    //说明鼠标响应已经被占用了,虚拟出一个鼠标
                {
                    pen.SimulMouseMotion(hwnd, &ptNow );
                }
                else//说明鼠标响应没有被用到
                {
                    this->m_pMouseEventGenerator->GenMouseEvent(
                        &ptNow
#ifdef _DEBUG
                        ,true
#endif 
                        );
                }



            }

        } //else
    }
    ////////////////////////////////////////////////////////////////////////////////
    ///////*********************************************************////////////////
    ////////////////////////////////////////////////////////////////////////////////@功能：匹配结束
    void AfterMulPenMatch(HWND hwnd )
    {
        for ( int i = 0; i < MaxPenCount ; i++)
        {
            CVirtualPenInfo &pen = m_AryPen[i];
            if (pen.IsPenActive() && !pen.IsPenMatch())                   //这个笔是活动的但是没有找到匹配的笔的信息。可能已经停止笔的操作了。
            {
                pen.incMissCounter();                                        //计数没有匹配上的笔失误信息
                if(pen.GetMissCounter() >=  MAX_MISS_TIME)                  //如果笔的没有匹配上的信息个数超过5个的时候，说明这这笔就彻底的停止了下来。纪录这支笔的信息就应该初始化掉，即在这个数组中删除掉这个笔的信息，
                {
                    
                    pen.Disable();
                    pen.ReSetHistoryPos();
                    if(!pen.IsFakeMouse())
                    {
                        //pen.SetSimMouse(FALSE);
                        //g_oMouseEventGen.GenMouseEvent(NULL);
                        this->m_pMouseEventGenerator->GenMouseEvent(NULL);
                    }
                    else
                    {
                        pen.SimulMouseMotion(hwnd,NULL);
                    }

                }
            }
        }
    }
private:
    CVirtualPenInfo m_AryPen[MaxPenCount]  ;
    BOOL m_bMulPenMatched                  ;
    static const  int MAX_MISS_TIME = 3    ;
    CMouseEventGenerator* m_pMouseEventGenerator;
};


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////板擦的响应信息
class CVirtualEraserInfo
{
public:
    CVirtualEraserInfo()
        :   
    m_eSimulEarserPossageState(SIMUL_EARSER_POSSAGE_STATE_IDLE)
    {

    }
    void EarserEnable()
    {
        m_bEarserActive =TRUE     ;
        m_nEarserMissCounter = 0  ;
    }
    void EarserDisable()
    {
        m_bEarserActive = FALSE  ;
    }

    BOOL IsEarserActive()
    {
        return m_bEarserActive   ;
    }

    void SetEarserHistoryPos(const POINT& Pos)
    {
        m_ptEarserHistoryPos = Pos ;
        m_bEraserMatched  = TRUE   ;
    }

    const POINT& GetEarserHistoryPos()const
    {
        return m_ptEarserHistoryPos  ;
    }

    void ClearEarserMatchFlag()
    {
        m_bEraserMatched = FALSE;
    }

    BOOL isEarserMatch()const
    {
        return m_bEraserMatched;
    }
    void incEarserMissCounter()
    {
        m_nEarserMissCounter++;
    }

    int GetMissCounter()const
    {
        return m_nEarserMissCounter;
    }

    //////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////
    void SimulEraserMotion(HWND hwnd , const POINT *ptEarserPos)    //模拟板擦的擦除
    {
        BOOL bFindEarser = TRUE;
        POINT SimulEarserPoint;
        SimulEarserPoint.x = 0;
        SimulEarserPoint.y = 0;
        if (ptEarserPos == NULL)
        {
            bFindEarser = FALSE ;
        }
        else
        {
            SimulEarserPoint = *ptEarserPos ;
        }

        RunSimulEarserStateMachine(bFindEarser,SimulEarserPoint,hwnd);
    }
    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    void RunSimulEarserStateMachine(BOOL bFindEarser, const POINT &ptPencilPos, HWND hwnd)
    {
        switch(m_eSimulEarserPossageState)
        {
        case SIMUL_EARSER_POSSAGE_STATE_IDLE:
            {
                if (bFindEarser)
                {
                    PostMessage(hwnd , m_EarserMsg , ptPencilPos.x , ptPencilPos.y )   ;
                    m_eSimulEarserPossageState = SIMUL_EARSER_POSSAGE_STATE_DOWN ;
                }
                break;
            }
        case SIMUL_EARSER_POSSAGE_STATE_DOWN:
            {
                if (bFindEarser)
                {
                    PostMessage(hwnd , m_EarserMsg+1 , ptPencilPos.x , ptPencilPos.y )   ;
                    m_eSimulEarserPossageState = SIMUL_EARSER_POSSAGE_STATE_MOVE ;
                }
                else
                {
                    PostMessage(hwnd , m_EarserMsg+2 , ptPencilPos.x , ptPencilPos.y )   ;
                    m_eSimulEarserPossageState = SIMUL_EARSER_POSSAGE_STATE_IDLE ;
                }
                break;
            }
        case SIMUL_EARSER_POSSAGE_STATE_MOVE:
            {
                if (bFindEarser)
                {
                    PostMessage(hwnd , m_EarserMsg+1 , ptPencilPos.x , ptPencilPos.y )   ;

                    AtlTrace(_T("x= %d , y = %d "), ptPencilPos.x ,ptPencilPos.y );
                }
                else
                {
                    PostMessage(hwnd , m_EarserMsg+2 , ptPencilPos.x , ptPencilPos.y )   ;
                    m_eSimulEarserPossageState = SIMUL_EARSER_POSSAGE_STATE_IDLE ;
                }

                break;
            }
        }
    }

    void SetEarserMsge(UINT msg)
    {
        m_EarserMsg = msg;	
    }

protected:
    BOOL     m_bEarserActive               ;
    POINT    m_ptEarserHistoryPos          ;
    BOOL   m_bEraserMatched                ;
    int   m_nEarserMissCounter             ;
    UINT   m_EarserMsg                     ;

    enum E_SIMUL_EARSER_POSSAGE_STATE
    {
        SIMUL_EARSER_POSSAGE_STATE_IDLE    ,
        SIMUL_EARSER_POSSAGE_STATE_DOWN    ,
        SIMUL_EARSER_POSSAGE_STATE_MOVE    ,
    }m_eSimulEarserPossageState;
};

////////////板擦的响应类
template <int MaxEarserCount>
class CEraserMatch
{
public:
    CEraserMatch(CMouseEventGenerator* pMouseEventGenerator)
        :m_bEraserMatched(FALSE),
        m_pMouseEventGenerator(pMouseEventGenerator)
    {
        m_AryEarser[0].SetEarserMsge(1057) ;   ////1
        m_AryEarser[1].SetEarserMsge(1060) ;   ////2
    }

    ~CEraserMatch()
    {

    }

    CVirtualEraserInfo& GetEarser(int iIndex)
    {
        ASSERT(0 <= iIndex && iIndex < MaxEarserCount);
        return m_AryEarser[iIndex];
    }

    /////////////////////////////////////////////////////////////
    void BeforeEarserMatch()
    {
        for (int i = 0 ; i < MaxEarserCount ; i++)
        {
            CVirtualEraserInfo & MulEarser = m_AryEarser[i] ;
            if(MulEarser.IsEarserActive())
            {
                MulEarser.ClearEarserMatchFlag();
            }
        }

    }
    void DoEarserMatch(HWND hwnd ,const POINT& ptNow)
    {
        int nMinDisTance = 3000;
        int iEraserIndex = -1 ;
        for(int i = 0 ; i <MaxEarserCount ; i++ )
        {
            CVirtualEraserInfo & MulEarser = m_AryEarser[i];

            if (!MulEarser.IsEarserActive())
            {
                continue;
            }
            const POINT& ptHistory = MulEarser.GetEarserHistoryPos();

            int nx =(ptNow.x - ptHistory.x) ;
            int ny =(ptNow.y - ptHistory.y) ;

            int nSquareDistance = nx* nx + ny * ny;

            if (nSquareDistance < nMinDisTance)
            {
                iEraserIndex = i;
                nMinDisTance = nSquareDistance;
            }
        }

        if (iEraserIndex != -1)
        {
            CVirtualEraserInfo & MulEarser = m_AryEarser[iEraserIndex];

            if (!m_AryEarser->isEarserMatch())
            {
                MulEarser.SetEarserHistoryPos(ptNow);
                MulEarser.SimulEraserMotion(hwnd,&ptNow);
            }
        }
        else
        {
            iEraserIndex = -1;
            for(int i = 0 ;i < MaxEarserCount ; i++)
            {
                CVirtualEraserInfo& MulEarser = m_AryEarser[i];
                if (!MulEarser.IsEarserActive())
                {
                    iEraserIndex = i ;
                    break;
                }
            }

            if (iEraserIndex != -1)
            {
                CVirtualEraserInfo &MulEarser = GetEarser(iEraserIndex) ;
                MulEarser.EarserEnable();
                MulEarser.SetEarserHistoryPos(ptNow);
                MulEarser.SimulEraserMotion(hwnd,&ptNow);
            }
        }
    }
    void AfterEarserMatch(HWND hwnd )
    {
        for(int i = 0 ; i <MaxEarserCount ; i++ )
        {
            CVirtualEraserInfo &MulEarser = m_AryEarser[i] ;
            if (MulEarser.IsEarserActive() && !MulEarser.isEarserMatch())
            {
                MulEarser.incEarserMissCounter();
                if (MulEarser.GetMissCounter() > MAX_MISS_TIME)
                {
                    MulEarser.EarserDisable();
                    MulEarser.SimulEraserMotion(hwnd,NULL);
                }
            }
        }
    }

private:
    CVirtualEraserInfo m_AryEarser[MaxEarserCount] ;
    BOOL m_bEraserMatched                          ;
    static const  int MAX_MISS_TIME = 3            ;
    CMouseEventGenerator* m_pMouseEventGenerator;

};







