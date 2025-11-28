#pragma once
#include <list>
#include <vector>

class CStroke
{
public:
    void SetId(DWORD dwID )
    {
        m_dwID = dwID;
    }

    DWORD GetId() const 
    {
        return m_dwID;
    }
    void SetDotColor(COLORREF clr)
    {
        m_clrDot = clr;
    }

    void SetLineColor(COLORREF clr)
    {
        m_clrLine = clr;
    }

    void AddPoint(const POINT& pt)
    {
        m_vecPoints.push_back(pt);
    }

    void DrawLast(HDC hDC)
    {
        if (m_vecPoints.size() < 2)
        {
            if (m_vecPoints.size() == 1)
            {
                POINT p0 = m_vecPoints[0];
                //Draw Dot
                ::SetPixel(hDC, p0.x, p0.y, m_clrDot);
            }

            return;
        }
        int nCount = m_vecPoints.size();


        POINT p0 = m_vecPoints[nCount - 2];
        POINT p1 = m_vecPoints[nCount - 1];

        //Draw  Thick Line Segment
        //HPEN hPen = ::CreatePen(PS_SOLID, PEN_WIDTH, m_clrLine);
        //HPEN hPenOld = (HPEN)SelectObject(hDC, hPen);
        //::MoveToEx(hDC, p0.x, p0.y, NULL);
        //::LineTo(hDC, p1.x, p1.y);
        //SelectObject(hDC, hPenOld);
        //DeleteObject(hPen);

        //Draw Thin Line Segment
        //当前的细笔迹会被下一次的粗笔迹擦除。
        //因此采用重绘所有细笔迹的策略
        
       // HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(0,0,255));
	   //HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(0, 0, m_clrLine

		HPEN hPen = ::CreatePen(PS_SOLID, 1, m_clrLine);

        HPEN hPenOld = (HPEN)SelectObject(hDC, hPen);
        
        ::MoveToEx(hDC, p0.x, p0.y, NULL);
        ::LineTo(hDC, p1.x, p1.y);

        //p0 = m_vecPoints[0];
        //size_t index = 1;
        //while (index <  m_vecPoints.size())
        //{
        //    p1 = m_vecPoints[index];
        //    ::MoveToEx(hDC, p0.x, p0.y, NULL);
        //    ::LineTo(hDC, p1.x, p1.y);
        //    p0 = p1;
        //    index++;

        //}//while
        
        DrawDot(hDC, p0, m_clrDot);
        DrawDot(hDC, p1, m_clrDot);

        /*for (index = 0; index < m_vecPoints.size(); index++)
        {
            DrawDot(hDC, m_vecPoints[index], m_clrDot);
        }*/

        SelectObject(hDC, hPenOld);
        DeleteObject(hPen);
    }

    void Draw(HDC hDC)
    {
        if (m_vecPoints.size() < 2) return;
        
        POINT p0, p1;
        ////Draw Thick Line Segment
        //HPEN hPen = ::CreatePen(PS_SOLID, PEN_WIDTH, m_clrLine);
        //HPEN hPenOld = (HPEN)SelectObject(hDC, hPen);

        //POINT p0 = m_vecPoints[0];
        //size_t index = 1;

        //while (index <  m_vecPoints.size())
        //{
        //    POINT p1 = m_vecPoints[index];
        //    ::MoveToEx(hDC, p0.x, p0.y, NULL);
        //    ::LineTo(hDC, p1.x, p1.y);
        //    p0 = p1;
        //    index++;

        //}//while

        //SelectObject(hDC, hPenOld);
        //DeleteObject(hPen);
        //
        //Draw Thin Line Segment
//         HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(0,0,255));

//		 HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 255));

		 HPEN hPen = ::CreatePen(PS_SOLID, 1, m_clrLine);
         HPEN hPenOld = (HPEN)SelectObject(hDC, hPen);

        p0 = m_vecPoints[0];
        size_t index = 1;

        while (index <  m_vecPoints.size())
        {
            p1 = m_vecPoints[index];
            ::MoveToEx(hDC, p0.x, p0.y, NULL);
            ::LineTo(hDC, p1.x, p1.y);
            p0 = p1;
            index++;
        }//while

        for (index = 0; index < m_vecPoints.size(); index++)
        {
            DrawDot(hDC, m_vecPoints[index], m_clrDot);
        }
        
        SelectObject(hDC, hPenOld);
        DeleteObject(hPen);


    }

    void DrawDot(HDC hDC, POINT pt, COLORREF clr)
    {
        //SetPixel(hDC, pt.x - 1, pt.y - 1, clr);
        //SetPixel(hDC, pt.x - 0, pt.y - 1, clr);
        //SetPixel(hDC, pt.x + 1, pt.y - 1, clr);
        //SetPixel(hDC, pt.x - 1, pt.y - 0, clr);
        SetPixel(hDC, pt.x - 0, pt.y - 0, clr);
        //SetPixel(hDC, pt.x + 1, pt.y - 0, clr);
        //SetPixel(hDC, pt.x - 1, pt.y + 1, clr);
        //SetPixel(hDC, pt.x - 0, pt.y + 1, clr);
        //SetPixel(hDC, pt.x + 1, pt.y + 1, clr);
    }

//	void Clear()
//	{
//		m_vecPoints.clear();
//	}


protected:
    std::vector<POINT> m_vecPoints;
    DWORD m_dwID;
    COLORREF m_clrDot;
    COLORREF m_clrLine;
    static const int PEN_WIDTH = 10;
};


class CStrokeCollection
{
public:
    ~CStrokeCollection()
    {
        for (std::list<CStroke*>::iterator it = m_lstStrokes.begin();
            it != m_lstStrokes.end();
            it++)
        {
            delete *it;
            *it = NULL;
        }
    }
    void AddStroke(const CStroke& strok)
    {
        CStroke *stroke = new CStroke(strok);
        m_lstStrokes.push_back(stroke);
    }


    BOOL RemoveStroke(const CStroke& strok)
    {
        for (std::list<CStroke*>::iterator it = m_lstStrokes.begin();
            it != m_lstStrokes.end();
            it++)
        {
            if ((*it)->GetId() == strok.GetId())
            {

                m_lstStrokes.erase(it);
                return TRUE;
                
            }

        }//for
        
        return FALSE;
    }

    CStroke* FindStrokeById(DWORD dwId)
    {
        for (std::list<CStroke*>::iterator it = m_lstStrokes.begin();
            it != m_lstStrokes.end();
            it++)
        {
            if ((*it)->GetId() == dwId)
            {
                return *it;
            }

        }//for

        return NULL;

    }


    void Draw(HDC hDC)
    {
        for (std::list<CStroke*>::iterator it = m_lstStrokes.begin();
            it != m_lstStrokes.end();
            it++)
        {
            (*it)->Draw(hDC);
        }
    }

	void Clear()
	{
		m_lstStrokes.clear();
	}
protected:
    std :: list<CStroke*> m_lstStrokes;

};