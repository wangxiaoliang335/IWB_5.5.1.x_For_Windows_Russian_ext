#pragma once

template<int MAX_ARGS_COUNT = 16>
class CmdLineParser
{
public:
    CmdLineParser()
    {
        Init();
    }

    ~CmdLineParser()
    {

    }

    void Init()
    {
        m_argCount = 0;
        for(int i = 0; i < _countof(m_argInfos); i++)
        {
            m_argInfos[i].pName  = NULL;
            m_argInfos[i].uNameLength = 0;
            m_argInfos[i].pValue = NULL;
            m_argInfos[i].uValueLength = 0;
        }

    }



    void Parse(LPCTSTR lpCmdLine)
    {
      const TCHAR* pChar = lpCmdLine;
      static const int STAGE_SEARCH_NAME  = 0;
      static const int STAGE_SEARCH_VALUE = 1;
      int STAGE_NO = 0;

      m_argCount = 0;
      TArgInfo* pArg = &m_argInfos[0];

      const TCHAR* pStart;
      int length;
      BOOL bContinue = TRUE;
      while(bContinue)
      {
          switch(STAGE_NO)
          {
            case STAGE_SEARCH_NAME://搜搜参数名称
                 //参数以/和-作为起始字符
                while(*pChar && (*pChar != _T('/') && *pChar != _T('-') ))
                {
                    pChar ++;
                }

                if( *pChar == _T('\0'))
                {
                    bContinue = FALSE;
                    break;
                }

                pChar ++;

                pStart = pChar;

                

                //查找空格和Tab字符作为名称的结束
                while(*pChar && (*pChar != _T(' ') &&  *pChar != _T('\t')))
                {
                    pChar ++;
                }
                

                 length = pChar - pStart ;//-1不包括最后的结尾空格和起始的/或者-
                 
                 if(length > 0)
                 {
                     pArg->pName       = pStart;
                     pArg->uNameLength = length;
                     STAGE_NO = STAGE_SEARCH_VALUE;
                     break;
                 }

                 if( *pChar == _T('\0'))
                 {
                     bContinue = FALSE;
                     break;
                 }
                 break;

            case STAGE_SEARCH_VALUE://搜索参数值

                //过滤空格和TAB字符
                 while(*pChar && (*pChar == _T(' ') || *pChar == _T('\t')))
                {
                    pChar ++;
                }

                if(*pChar == _T('\0'))
                {
                    bContinue = FALSE;
                    break;
                }
                
                if(*pChar == _T('\\') || *pChar == _T('-') )
                {//无参数值
                    pArg->uValueLength = 0;
                    pArg->pValue       = NULL;

                    STAGE_NO = STAGE_SEARCH_NAME;
                    pArg ++;
                    m_argCount ++;
                    if(m_argCount == MAX_ARGS_COUNT) bContinue =FALSE;
                    break;
                }

                if(*pChar == _T('\"'))
                {
                    
                    pChar ++;
                    pStart = pChar;

                    //搜索结尾的"号
                     while(*pChar)
                    {
                        if(*pChar == _T('\"') && *(pChar-1) != _T('\\'))
                        {
                            break;
                        }
                        pChar ++;
                    }

                    length = pChar - pStart;//不包括起始和结尾的"号
                     

                     if(length > 0 )
                     {
                         pArg->pValue       = pStart;
                         pArg->uValueLength = length;
                         STAGE_NO = STAGE_SEARCH_NAME;
                         pArg ++;
                         m_argCount ++;
                         if(m_argCount == MAX_ARGS_COUNT) bContinue =FALSE;
                         break;
                     }

                     if(*pChar == _T('\0'))
                     {
                        bContinue = FALSE;
                        break;
                      }
                }
                else
                {
                    pStart = pChar;

                    while(*pChar && (*pChar != _T(' ') &&  *pChar != _T('\t')))
                    {
                        pChar ++;
                    }
                
                    length = pChar - pStart;
                     
                    if(length)
                     {
                         pArg->pValue       = pStart + 1;
                         pArg->uValueLength = length;
                         STAGE_NO = STAGE_SEARCH_NAME;
                         pArg ++;
                         m_argCount ++;
                         if(m_argCount == MAX_ARGS_COUNT) bContinue =FALSE;
                         break;
                     }
                     
                    if(*pChar == _T('\0'))
                    {
                        bContinue = FALSE;
                        break;
                    }

                }

                break;
          }//swtich

      }

       
    }

    BOOL GeArgsValue(LPCTSTR szArgName, LPTSTR lpszValue, UINT lpBufSize)
    {
        for(UINT i = 0; i <  m_argCount; i++)
        {
            if(_tcsnicmp(szArgName, m_argInfos[i].pName, m_argInfos[i].uNameLength) == 0)
            {
                if(m_argInfos[i].pValue && lpszValue)
                {
                    _tcsncpy_s(lpszValue, lpBufSize, m_argInfos[i].pValue, m_argInfos[i].uValueLength);
                    
                    return TRUE;
                }
                break;
            }
        }

        return FALSE;
    }


    UINT GetValueLength(LPCTSTR szArgName) const
    {

        for(int i = 0; i <  m_argCount i++)
        {
            if(_stricmp(szArgName, m_argInfos[i].pName) == 0)
            {
                return m_argInfos[i].valueLength;
                break;
            }
        }

        return 0;
    }

    const INT GetArgsCount() const
    {
        return m_argCount;
    }
    
    struct TArgInfo
    {
        const TCHAR* pName;
        UINT   uNameLength;
        const TCHAR* pValue;
        UINT   uValueLength; 
    };

    
    TArgInfo m_argInfos[MAX_ARGS_COUNT];
    UINT m_argCount;
    
};