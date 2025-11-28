#pragma once
//Reference:A New Method of Interpolation and Smooth Curve Fitting Based on
//          Local Procedures
//          Hiroshi Akima
//          Journal of the Association for Computing Machinery, Vol. 17,
//          No.4 October 1970, pp. 589-602
//Code: toxuke@gmail.com, November, 7th, 2012
//
template<int M=10>
class CAkima_Interpolator
{
public:
    CAkima_Interpolator()
        :
    m_nSamplePtsNumber(0)
    {
    
    }



    //@Parameter: ptInput, the sample point data
    //@Return   : The number of points interpolated
    //@Remark   : Do Akima Interpolation in realtime.
    int Process(const POINT& ptInput)
    {
        m_nInterpolatePtsNumber = 0;
        
        switch(m_nSamplePtsNumber)
        {
        case 0:
        case 1:
        case 2:        
            QueueSamplePts(ptInput);
            return 0;
            break;


        default:
            if(3 == m_nSamplePtsNumber)
            {
                int x5 = m_arySamplePts[2].x; int y5 = m_arySamplePts[2].y; 
                int x4 = m_arySamplePts[1].x; int y4 = m_arySamplePts[1].y; 
                int x3 = m_arySamplePts[0].x; int y3 = m_arySamplePts[0].y; 

                int x2 = x5 - 2*x4 + 3*x3; int y2 = y5 - 2*y4 + 3*y3;
                int x1 = x4 - 3*x3 + 3*x2; int y1 = y4 - 3*y3 + 3*y2;
                
                InsertSamplePts(0, x2, y2 );
                InsertSamplePts(0, x1, y1);
            
                m_bOk_t1 = CalcSlope(&m_dbSin_t1, &m_dbCos_t1);
            }

            QueueSamplePts(ptInput);

            {
                double dbSin_t2, dbCos_t2;
                BOOL   bOk_t2;

                bOk_t2 = CalcSlope(&dbSin_t2, &dbCos_t2);

                
                POINT& pt1 = m_arySamplePts[1];
                POINT& pt2 = m_arySamplePts[2];
                if(m_bOk_t1 && bOk_t2)
                {
                    Interpolate_Points(pt1, pt2, m_dbSin_t1, m_dbCos_t1, dbSin_t2, dbCos_t2, M);
                }
                else
                {
                    m_aryInterpolatePts[m_nInterpolatePtsNumber] = pt1;
                    m_nInterpolatePtsNumber ++;

                }
                
                m_bOk_t1   = bOk_t2;
                m_dbSin_t1 = dbSin_t2;
                m_dbCos_t1 = dbCos_t2;
            }



        }




        return m_nInterpolatePtsNumber;

    }


    //@
    int EndInterpolate()
    {
        m_nInterpolatePtsNumber = 0;

        if(m_nSamplePtsNumber <=3)
        {
            for(int i=0; i < m_nSamplePtsNumber; i++)
            {
                int nIndex = m_nInterpolatePtsNumber;
                m_arySamplePts[nIndex] = m_arySamplePts[i];
                m_nInterpolatePtsNumber ++;            
            }
        }
        else
        {
            
            //The second last  secant curve
            long x1,y1,x2,y2,x3,y3,x4,y4,x5,y5;
            x1 = m_arySamplePts[2].x; y1 = m_arySamplePts[2].y;
            x2 = m_arySamplePts[3].x; y2 = m_arySamplePts[3].y;
            x3 = m_arySamplePts[4].x; y3 = m_arySamplePts[4].y;
            x4 = 3*x3 - 3*x2 + x1   ; y4 = 3*y3 - 3*y2 + y1;


            QueueSamplePts(x4, y4);
              
             double dbSin_t2, dbCos_t2;
             BOOL   bOk_t2;
   
             bOk_t2 = CalcSlope(&dbSin_t2, &dbCos_t2);


              POINT& pt1 = m_arySamplePts[1];
              POINT& pt2 = m_arySamplePts[2];
              if(m_bOk_t1 && bOk_t2)
              {
                  Interpolate_Points(pt1, pt2, m_dbSin_t1, m_dbCos_t1, dbSin_t2, dbCos_t2, M);
               }
               else
               {
                   m_aryInterpolatePts[m_nInterpolatePtsNumber] = pt1;
                   m_nInterpolatePtsNumber ++;

               }
                
                m_bOk_t1   = bOk_t2;
                m_dbSin_t1 = dbSin_t2;
                m_dbCos_t1 = dbCos_t2;

                //The last one secant curve
                x5 = 6*x3 - 8*x2 + 3*x1;
                y5 = 6*y3 - 8*y2 + 3*y1;

                QueueSamplePts(x5, y5);



                bOk_t2 = CalcSlope(&dbSin_t2, &dbCos_t2);


                POINT& pt3 = m_arySamplePts[1];
                POINT& pt4 = m_arySamplePts[2];
                if(m_bOk_t1 && bOk_t2)
                {
                    Interpolate_Points(pt3, pt4, m_dbSin_t1, m_dbCos_t1, dbSin_t2, dbCos_t2, M, TRUE);
                }
                else
                {
                    m_aryInterpolatePts[m_nInterpolatePtsNumber] = pt1;
                    m_nInterpolatePtsNumber ++;
                }


        }

        return m_nInterpolatePtsNumber;
    }
    


    void Reset()
    {
        m_nSamplePtsNumber = 0;
        m_nInterpolatePtsNumber = 0;
        m_bOk_t1 = FALSE;

        memset(m_arySamplePts, 0, sizeof(m_arySamplePts));
    }

    const POINT* GetInterpolatePts()const
    {
        return &m_aryInterpolatePts[0];
    }

protected://member functions
    void QueueSamplePts(const POINT& ptNewSamplePt)
    {

        if(m_nSamplePtsNumber < _countof(m_arySamplePts))
        {
            int nIndex = m_nSamplePtsNumber;
            m_arySamplePts[nIndex] = ptNewSamplePt;
            m_nSamplePtsNumber ++;            
        }
        else
        {
            for(int i = 1;i < _countof(m_arySamplePts); i++)
            {
                m_arySamplePts[i-1] = m_arySamplePts[i];
            }

            int nIndex = _countof(m_arySamplePts) - 1;
            m_arySamplePts[nIndex] = ptNewSamplePt;
            

        }
    }

    void QueueSamplePts(long x, long y)
    {
        POINT pt;
        pt.x = x;
        pt.y = y;
        QueueSamplePts(pt);
    }

    void InsertSamplePts(int nInsertPos, const POINT& ptNewSamplePt)
    {
        //Insert at the end
        if(nInsertPos == - 1)
        {
            if(m_nSamplePtsNumber < _countof(m_arySamplePts))
            {
                m_arySamplePts[m_nSamplePtsNumber] = ptNewSamplePt;
                m_nSamplePtsNumber ++;
            }        

        }
        else
        {
 
            for(int i= m_nSamplePtsNumber - 1; i >= nInsertPos; i--)
            {

                if( i+1 >= _countof(m_arySamplePts)) continue;

                m_arySamplePts[i+1] = m_arySamplePts[i];
            }
            //
            m_arySamplePts[nInsertPos] = ptNewSamplePt;

            //
            if(m_nSamplePtsNumber < _countof(m_arySamplePts))
            {
                m_nSamplePtsNumber ++;
            }

        }

    };


    void InsertSamplePts(int nInsertPos, long x, long y)
    {
        POINT pt;
        pt.x = x;
        pt.y = y;
        InsertSamplePts(nInsertPos, pt);
    }





    BOOL CalcSlope(double* pdbSin_t, double* pdbCos_t)
    {
        assert(m_nSamplePtsNumber == _countof(m_arySamplePts));

        long a[_countof(m_arySamplePts)  - 1], b[_countof(m_arySamplePts) - 1];

        for(int i=1; i<_countof(m_arySamplePts); i++)
        {
            a[i - 1] = m_arySamplePts[i].x - m_arySamplePts[i-1].x;
            b[i - 1] = m_arySamplePts[i].y - m_arySamplePts[i-1].y;

        }//for

        double w2, w3, a0, b0, r;

        w2 = double(abs(a[2]*b[3] - a[3]*b[2]));
        
        w3 = double(abs(a[0]*b[1] - a[1]*b[0]));

        a0 = w2*a[1] + w3*a[2];
        b0 = w2*b[1] + w3*b[2];

        r = sqrt(a0*a0 + b0*b0);

        BOOL bOk = FALSE;
        if(0.0 == r)
        {
            if(pdbSin_t) *pdbSin_t = 0.0;
            if(pdbCos_t) *pdbCos_t = 0.0;

        }
        else
        {
            bOk = TRUE;
            pdbSin_t? *pdbCos_t = a0/r : 0;
            pdbCos_t? *pdbSin_t = b0/r : 0;
        }

        return bOk;
    }

    //
    //@function :
    //@parameter:
    //@return   :the number of points interpolated
    int Interpolate_Points(POINT& pt1, POINT& pt2, double dbSin_t1, double dbCos_t1, double dbSin_t2, double dbCos_t2, int m, BOOL bIncludeEndPoint = FALSE)
    {
        long x1,y1,x2,y2, x21, y21;
        double p0, p1, p2, p3, q0, q1, q2, q3, r;

        x1 = pt1.x; y1 = pt1.y;
        x2 = pt2.x; y2 = pt2.y;
        

        x21 = x2 - x1;
        y21 = y2 - y1;

        r  = sqrt(double(x21*x21 + y21*y21));

        p0 =  x1;
        p1 =  r*dbCos_t1;
        p2 =  3*x21 - r*(dbCos_t2 + 2*dbCos_t1);
        p3 = -2*x21 + r*(dbCos_t2 +   dbCos_t1);


        q0 =  y1;
        q1 =  r*dbSin_t1;
        q2 =  3*y21 - r*(dbSin_t2 + 2*dbSin_t1);
        q3 = -2*y21 + r*(dbSin_t2 +   dbSin_t1);

        double z_inc = 1.0/M;

        double z=0.0;


        if(bIncludeEndPoint)
        {
            m ++;
        }

        int nCount = m;
        do
        {
           double dbX,dbY;
           dbX = p0 + p1*z + p2*z*z + p3*z*z*z;
           dbY = q0 + q1*z + q2*z*z + q3*z*z*z;

            m_aryInterpolatePts[m_nInterpolatePtsNumber].x = int(dbX);
            m_aryInterpolatePts[m_nInterpolatePtsNumber].y = int(dbY);

            z += z_inc;
            m_nInterpolatePtsNumber ++;            

            assert(m_nInterpolatePtsNumber <= 2*M+1);

            m --;
        }while(m);
        

        return nCount;
    }

protected://member variables

    POINT  m_aryInterpolatePts[2*M + 1];//2M+1 interpolate points at most, when pen is up
    int    m_nInterpolatePtsNumber     ;//the number of points interpolated
    double m_dbSin_t1                  ;//sin(t), tangent(t) is the slope of the smooth curve at middle points
    double m_dbCos_t1                  ;//cost(t), tangent(t) is the slope of the soomth curve at middle points
    BOOL   m_bOk_t1                    ;//
    
    POINT m_arySamplePts[5]            ;//
    int   m_nSamplePtsNumber           ;//the number of sample data.

};
