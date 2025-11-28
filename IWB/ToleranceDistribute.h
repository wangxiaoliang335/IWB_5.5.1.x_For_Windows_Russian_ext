#pragma once
//@说明:
//校正后的公差分布,坐标归一化到0~1
#include <vector>
#include <utility>
#include <limits>
class CToleranceDistribute
{
public:
    CToleranceDistribute(UINT nRows = 10, UINT nCols = 10)
    {
        Init(nRows, nCols);

        m_ScreenSize.cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        m_ScreenSize.cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		//modify by vera_zhao 2020.07.03
        m_dbMinModulateCoef = 0.0;
//        m_dbMinModulateCoef = 0.3;
		m_dbMaxModulateCoef = 1.0;

        m_dbMinToleranceX = 0.0;
        m_dbMaxToleranceX = 0.0;

        m_dbMinToleranceY = 0.0;
        m_dbMaxToleranceY = 0.0;

        //载入数据
        Load();

		double dbMinToleranceX = (std::numeric_limits<double>::max)();
		double dbMaxToleranceX = (std::numeric_limits<double>::min)();
		double dbMinToleranceY = (std::numeric_limits<double>::max)();
		double dbMaxToleranceY = (std::numeric_limits<double>::min)();

		int nCellCount = m_nRows*m_nCols;
		for (size_t i = 0; i < nCellCount; i++)
		{
			if (dbMinToleranceX > m_vecToleranceX[i])
			{
				dbMinToleranceX = m_vecToleranceX[i];
			}

			if (dbMaxToleranceX < m_vecToleranceX[i])
			{
				dbMaxToleranceX = m_vecToleranceX[i];
			}

			if (dbMinToleranceY > m_vecToleranceY[i])
			{
				dbMinToleranceY = m_vecToleranceY[i];
			}

			if (dbMaxToleranceY < m_vecToleranceY[i])
			{
				dbMaxToleranceY = m_vecToleranceY[i];
			}
		}

		m_dbMinToleranceX = dbMinToleranceX;
		m_dbMaxToleranceX = dbMaxToleranceX;

		m_dbMinToleranceY = dbMinToleranceY;
		m_dbMaxToleranceY = dbMaxToleranceY;
		
    }

    ~CToleranceDistribute()
    {

    }

    void SetScreenSize(const SIZE& newScreenSize)
    {
        m_ScreenSize = newScreenSize;
    }

    //@功能:更新公差分布。将整个屏幕划分为nRows*nCols个小方格，
    //      采样方格重心点处的定位公差。
    //      按行优先排列，排完一行后，开始新的一行。
    //@参数:
    // nRows, 函数
    // nCols, 列数
    // pTolX, nRows*nCols个x坐标定位公差数组
    // pTolY, nRows*nCols个y坐标定位公差数组
    void UpdateToleranceDistribute(int nRows, int nCols, const double* pTolX, const double* pTolY)
    {
        UINT nNewCellCount = nRows*nCols;
        
        Init(nRows, nCols);
        
        double dbMinToleranceX = (std::numeric_limits<double>::max)();
        double dbMaxToleranceX = (std::numeric_limits<double>::min)();
        double dbMinToleranceY = (std::numeric_limits<double>::max)();
        double dbMaxToleranceY = (std::numeric_limits<double>::min)();


        for (size_t i = 0; i < nNewCellCount; i++)
        {
            m_vecToleranceX[i] = pTolX[i];

            if (dbMinToleranceX > pTolX[i])
            {
                dbMinToleranceX = pTolX[i];
            }

            if (dbMaxToleranceX < pTolX[i])
            {
                dbMaxToleranceX = pTolX[i];
            }

            m_vecToleranceY[i] = pTolY[i];
            if (dbMinToleranceY > pTolY[i])
            {
                dbMinToleranceY = pTolY[i];
            }

            if (dbMaxToleranceY < pTolY[i])
            {
                dbMaxToleranceY = pTolY[i];
            }
        }

        m_dbMinToleranceX = dbMinToleranceX;
        m_dbMaxToleranceX = dbMaxToleranceX;

        m_dbMinToleranceY = dbMinToleranceY;
        m_dbMaxToleranceY = dbMaxToleranceY;
        
        Save();
    }

    //@功能:获取x方向和y方向的调制系数
    void GetModulateFactors(const POINT& ptScreen,double nMinModulateCoef, double* pModulateFactorX, double* pModulateFactorY)
    {
		m_dbMinModulateCoef = nMinModulateCoef;

        double dbToleranceX = 0.0;
        double dbToleranceY = 0.0;

        UINT nC = ptScreen.x * m_nCols / m_ScreenSize.cx;
        UINT nR = ptScreen.y * m_nRows / m_ScreenSize.cy;

        if (nC < 0) nC = 0;
        if (nC >= m_nCols) nC = m_nCols - 1;

        if (nR < 0) nR = 0;
        if (nR >= m_nRows) nR = m_nRows - 1;

        dbToleranceX = m_vecToleranceX[nR * m_nCols + nC];

        if (pModulateFactorX)
        {
            double dbDenominatorX = (m_dbMaxToleranceX - m_dbMinToleranceX);
            if (fabs(dbDenominatorX) < std::numeric_limits<double>::epsilon())
            {
                *pModulateFactorX = m_dbMaxModulateCoef;
            }
            else
            {
                *pModulateFactorX = m_dbMinModulateCoef + (m_dbMaxModulateCoef - m_dbMinModulateCoef)*(dbToleranceX - m_dbMinToleranceX) / dbDenominatorX;
            }
        }

        dbToleranceY = m_vecToleranceY[nR * m_nCols + nC];
        if (pModulateFactorY)
        {
            double dbDenominatorY = (m_dbMaxToleranceY - m_dbMinToleranceY);
            if (fabs(dbDenominatorY) < std::numeric_limits<double>::epsilon())
            {
                *pModulateFactorY = m_dbMaxModulateCoef;
            }
            else
            {
                *pModulateFactorY = m_dbMinModulateCoef + (m_dbMaxModulateCoef - m_dbMinModulateCoef)*(dbToleranceY - m_dbMinToleranceY) / dbDenominatorY;
            }
        }

    }

protected:
    void Init(int nRows, int nCols)
    {
        UINT nNewCellCount = nRows*nCols;
        UINT nOldCellCount = m_nRows*m_nCols;

        if (nNewCellCount != nOldCellCount)
        {
            m_vecToleranceX.resize(nNewCellCount);
            m_vecToleranceY.resize(nNewCellCount);
        }

        for (size_t i = 0; i < nNewCellCount; i++)
        {
            m_vecToleranceX[i] = 0.0;
            m_vecToleranceY[i] = 0.0;
        }

        m_nRows = nRows;
        m_nCols = nCols;


    }
    #define TOL_DISTRIBUTE_DATA_FILE_NAME  "ToleranceDistribute.txt"
    BOOL Save()
    {
        FILE * dataFile = _fsopen(TOL_DISTRIBUTE_DATA_FILE_NAME, "w", _SH_DENYWR);

        if (dataFile == NULL )
        {
            return FALSE;
        }

        char szText[1024];
        const char* szComment = "#Rows\n";
        fwrite(szComment, 1, strlen(szComment), dataFile);
        sprintf_s(szText, _countof(szText), "%d\n", this->m_nRows);
        fwrite(szText, 1, strlen(szText), dataFile);

        szComment = "#Cols\n";
        fwrite(szComment, 1, strlen(szComment), dataFile);
        sprintf_s(szText, _countof(szText), "%d\n", this->m_nCols);
        fwrite(szText, 1, strlen(szText), dataFile);

        szComment = "#Tolerance Distribute Of X\n";
        fwrite(szComment, 1, strlen(szComment), dataFile);

        for (UINT r = 0; r < m_nRows; r++)
        {
            memset(szText, 0, sizeof(szText));
            char* pPos = szText;
            int bufSize = _countof(szText);
            for (UINT c = 0; c < m_nCols; c++)
            {
                int nChars = sprintf_s(pPos, bufSize, "%5.3f,", m_vecToleranceX[r*m_nCols + c]);
                pPos += nChars;
                bufSize -= nChars;
            }
            szText[strlen(szText) - 1] = '\n';
            fwrite(szText, 1, strlen(szText), dataFile);
        }//


        szComment = "#Tolerance Distribute Of Y\n";
        fwrite(szComment, 1, strlen(szComment), dataFile);

        for (UINT r = 0; r < m_nRows; r++)
        {
            memset(szText, 0, sizeof(szText));
            char* pPos = szText;
            int bufSize = _countof(szText);
            for (UINT c = 0; c < m_nCols; c++)
            {
                int nChars = sprintf_s(pPos, bufSize, "%5.3f,", m_vecToleranceY[r*m_nCols + c]);
                pPos += nChars;
                bufSize -= nChars;
            }
            szText[strlen(szText) - 1] = '\n';
            fwrite(szText, 1, strlen(szText), dataFile);
        }//


        fclose(dataFile);
        return TRUE;
    }

    BOOL IsCommentLine(const char* lpText)
    {
        const char* pPos = lpText;

        char c = *pPos++;
        while(c)
        {
            
            //行首空格跳过
            if (c == ' ' || c == '\t')
            {
                c = *pPos++;
                continue;
            }

            //行首非空白字符为#,则为注释字符
            if (c == '#' || c== '\n' || c == '\r')
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }
            
        } 

        return FALSE;
    };



    BOOL Load()
    {
        FILE * dataFile = _fsopen(TOL_DISTRIBUTE_DATA_FILE_NAME, "r", _SH_DENYNO);

        if (dataFile == NULL)
        {
            return FALSE;
        }

        enum EParseStage
        {
            E_PARSE_STAGE_READ_ROWS,
            E_PARSE_STAGE_READ_COLS,
            E_PARSE_STAGE_READ_TOLX,
            E_PARSE_STAGE_READ_TOLY,
        }eParseStage;

        eParseStage = E_PARSE_STAGE_READ_ROWS;

        char line[1024];
        
        UINT nRows = 0;
        UINT nCols = 0;
        int   nRet = 0;
        char *next_token = NULL;
        const char* delim = ",";
        const char* token = NULL;
        double  dbValue = 0.0;
        UINT r = 0, c = 0;
        while (fgets(line, _countof(line), dataFile))
        {
            if (IsCommentLine(line))
            {
                continue;
            }

            switch (eParseStage)
            {
            case E_PARSE_STAGE_READ_ROWS:
                nRet = sscanf_s(line, "%d", &nRows);
                if (nRet == 1)
                {
                    eParseStage = E_PARSE_STAGE_READ_COLS;
                }
                break;

            case E_PARSE_STAGE_READ_COLS:
                nRet = sscanf_s(line, "%d", &nCols);
                if (nRet == 1)
                {
                    Init(nRows, nCols);
                    r = 0;
                    c = 0;
                    eParseStage = E_PARSE_STAGE_READ_TOLX;
                }
                break;

            case E_PARSE_STAGE_READ_TOLX:
                token = strtok_s(line, delim, &next_token);
                while (token)
                {
                    nRet = sscanf_s(token, "%lf", &dbValue);

                    if (nRet == 1)
                    {
                        m_vecToleranceX[r*m_nCols + c] = dbValue;
                        
                        c++;

                        if (c == m_nCols)
                        {//一行数据读取完毕
                            c = 0;
                            r ++;
                            break;
                        }
                    }

                    token = strtok_s(NULL, delim, &next_token);

                }//while

                if (r == m_nRows)
                {
                    r = 0;
                    c = 0;
                    eParseStage = E_PARSE_STAGE_READ_TOLY;
                }
                break;

            case E_PARSE_STAGE_READ_TOLY:
                token = strtok_s(line, delim, &next_token);
                while (token)
                {
                    nRet = sscanf_s(token, "%lf", &dbValue);

                    if (nRet == 1)
                    {
                        m_vecToleranceY[r*m_nCols + c] = dbValue;

                        c++;

                        if (c == m_nCols)
                        {//一行数据读取完毕
                            c = 0;
                            r++;
                            break;
                        }
                    }

                    token = strtok_s(NULL, delim, &next_token);

                }//while

                if (r == m_nRows)
                {
                    r = 0;
                    c = 0;
                    eParseStage = E_PARSE_STAGE_READ_TOLY;
                }
                break;
            }
        }

        fclose(dataFile);

        return TRUE;
    }

    SIZE m_ScreenSize;
    UINT m_nRows;//采样点函数
    UINT m_nCols;//采样点列数

    double m_dbMaxModulateCoef;//最大调制系数，范围(0~1.0)
    double m_dbMinModulateCoef;//最小调制系数，范围(0~1.0)

    double m_dbMinToleranceX;//x坐标的最小定位公差
    double m_dbMaxToleranceX ;//x坐标的最大定位公差

    double m_dbMinToleranceY;//y坐标的最小定位公差
    double m_dbMaxToleranceY;//y坐标的最大定位公差

      
    //公差分布数组
    std::vector<double> m_vecToleranceX;//水平方向的公差分布, m_nRows*m_nCols
    std::vector<double> m_vecToleranceY;//垂直方向的公差分布, m_nRows*m_nCols


};