#include "StdAfx.h"
#include "..\..\headers.h"

CWGRPermissionParse::CWGRPermissionParse(void)
{
}

CWGRPermissionParse::~CWGRPermissionParse(void)
{
}

bool CWGRPermissionParse::IsTheEventPermited(E_TRIGGEREVENT evt, const DWORD &permissionSet)
{
    DWORD dwTmp;
    switch(evt)
    {
    case E_TE_ALL:
        {
            return (permissionSet == INIT_WITH_ALL_PERMITED);
        }
        break;

    case E_TE_PART:
        {
            return (permissionSet != INIT_WITH_ALL_PROHIBITED);
        }
        break;

    case E_TEF_ALL:
        {
            dwTmp = permissionSet & PERMFLAGSET_A_REGION;
            dwTmp |= (permissionSet & PERMFLAGSET_B_REGION);
            dwTmp |= (permissionSet & PERMFLAGSET_C_REGION);

            return (dwTmp == INIT_WITH_FINGER_PERMITED);
        }
        break;

    case E_TEF_PART:
        {
            dwTmp = permissionSet & PERMFLAGSET_A_REGION;
            dwTmp |= (permissionSet & PERMFLAGSET_B_REGION);
            dwTmp |= (permissionSet & PERMFLAGSET_C_REGION);            
        }
        break;

    case E_TEF_2TOUCHS_ALL:
        {            
            dwTmp = permissionSet & PERMFLAGSET_A_REGION;

            return (dwTmp == PERMFLAGSET_A_REGION);
        }

        break;

    case E_TEF_2TOUCHS_PART:
        {
            dwTmp = permissionSet & PERMFLAGSET_A_REGION;
        }
        break;

    case E_TEF_2TOUCHS_HMW:
        {            
            dwTmp = permissionSet & PERMFLAGSET_A_REGION_0;         
        }

        break;

    case E_TEF_2TOUCHS_VMW:
        {           
            dwTmp = permissionSet & PERMFLAGSET_A_REGION_1;
        }

        break;

    case E_TEF_35TOUCHS_ALL:
        {            
            dwTmp = permissionSet & PERMFLAGSET_B_REGION;

            return (dwTmp == PERMFLAGSET_B_REGION);
        }
        break;

    case E_TEF_35TOUCHS_PART:
        {            
            dwTmp = permissionSet & PERMFLAGSET_B_REGION;
        }
        break;

    case E_TEF_35TOUCHS_SWHWND:
        {            
            dwTmp = permissionSet & PERMFLAGSET_B_REGION_0;           
        }

        break;

    case E_TEF_35TOUCHS_ALTRARROW:
        {            
            dwTmp = 0;
        }

        break;

    case E_TEF_35TOUCHS_MAXICURWND:
        {            
            dwTmp = permissionSet & PERMFLAGSET_B_REGION_1;
        }

        break;

    case E_TEF_35TOUCHS_MINICURWND:
        {            
            dwTmp = permissionSet & PERMFLAGSET_B_REGION_2;
        }

        break;

    case E_TEF_6MTOUCHS_ALL:
        {            
            dwTmp = permissionSet & PERMFLAGSET_C_REGION;

            return (dwTmp == PERMFLAGSET_C_REGION);
        }

        break;

    case E_TEF_6MTOUCHS_PART:
        {            
            dwTmp = permissionSet & PERMFLAGSET_C_REGION;
        }

        break;

    case E_TEF_6MTOUCHS_DESPDESKTOP:
        {            
            dwTmp = permissionSet & PERMFLAGSET_C_REGION_0;
        }

        break;

    case E_TEP_ALL:
        {
            dwTmp = permissionSet & PERMFLAGSET_D_REGION;
            dwTmp |= (permissionSet & PERMFLAGSET_E_REGION);

            return (dwTmp == INIT_WITH_PALM_PERMITED);
        }

        break;

    case E_TEP_PART:
        {
            dwTmp = permissionSet & PERMFLAGSET_D_REGION;
            dwTmp |= (permissionSet & PERMFLAGSET_E_REGION);
        }

        break;

    case E_TEP_LOWLIM_ALL:
        {
            dwTmp = permissionSet & PERMFLAGSET_D_REGION;

            return (dwTmp == PERMFLAGSET_D_REGION);
        }
        break;

    case E_TEP_LOWLIM_PART:
        {
            dwTmp = permissionSet & PERMFLAGSET_D_REGION;
        }
        break;

    case E_TEP_LOWLIM_SHOWWNDSBS:
        {
            dwTmp = permissionSet & PERMFLAGSET_D_REGION_0;
        }

        break;

    case E_TEP_NSPLOC_ALL:
        {
            dwTmp = permissionSet & PERMFLAGSET_E_REGION;
            return (dwTmp == PERMFLAGSET_E_REGION);
        }
        break;

    case E_TEP_NSPLOC_PART:
        {
            dwTmp = permissionSet & PERMFLAGSET_E_REGION;
        }
        break;

    case E_TEP_NSPLOC_CLSCURWND:
        {
            dwTmp = permissionSet & PERMFLAGSET_E_REGION_0;
        }
        break;

    case E_TEP_NSPLOC_REFRESHWND:
        {
            dwTmp = permissionSet & PERMFLAGSET_E_REGION_1;
        }

        break;

    case E_TEP_NSPLOC_MOVWND:
        {
            dwTmp = permissionSet & PERMFLAGSET_E_REGION_2;
        }
        break;

    case E_TEP_NSPLOC_SVDOC:
        {
            dwTmp = 0;
        }
        break;

    default:
        break;
    }

    return (dwTmp != 0x0);;
}


void CWGRPermissionParse::ConvertToPermissionSet(E_TRIGGEREVENT evt, bool isPermited, DWORD &permissionSet)
{
    DWORD dwAnd, dwOr;   

    switch(evt)
    {
    case E_TE_ALL:
        {
            if (isPermited)
            {
                permissionSet = INIT_WITH_ALL_PERMITED;
            }
            else
            {
                permissionSet = INIT_WITH_ALL_PROHIBITED;
            }

            return;
        }
        break;

    case E_TEF_ALL:
        {
            dwAnd = permissionSet & (~INIT_WITH_FINGER_PERMITED);
            dwOr = isPermited == true ? INIT_WITH_FINGER_PERMITED : (0x0);
        }

        break;

    case E_TEP_ALL:
        {
            dwAnd = permissionSet & (~INIT_WITH_PALM_PERMITED);
            dwOr = isPermited == true ? INIT_WITH_PALM_PERMITED : (0x0);
        }

        break;

    case E_TEF_2TOUCHS_ALL:
        {            
            dwAnd  = permissionSet & (~PERMFLAGSET_A_REGION);                        
            dwOr = isPermited == true ? PERMFLAGSET_A_REGION : (0x0);
        }

        break;

    case E_TEF_2TOUCHS_HMW:
        {
            dwAnd  = permissionSet & (~PERMFLAGSET_A_REGION_0);            
            dwOr = isPermited == true ? PERMFLAGSET_A_REGION_0 : (0x0);
        }

        break;

    case E_TEF_2TOUCHS_VMW:
        {
            dwAnd  = permissionSet & (~PERMFLAGSET_A_REGION_1);            
            dwOr = isPermited == true ? PERMFLAGSET_A_REGION_1 : (0x0);
        }

        break;

    case E_TEF_35TOUCHS_ALL:
        {
            dwAnd  = permissionSet & (~PERMFLAGSET_B_REGION);            
            dwOr = isPermited == true ? PERMFLAGSET_B_REGION : (0x0);
        }
        break;

    case E_TEF_35TOUCHS_SWHWND:
        {
            dwAnd  = permissionSet & (~PERMFLAGSET_B_REGION_0);
            dwOr = isPermited == true ? PERMFLAGSET_B_REGION_0 : (0x0);
        }

        break;

    case E_TEF_35TOUCHS_ALTRARROW:
        {
            dwAnd  = 0;
            dwOr = 0;
        }

        break;

    case E_TEF_35TOUCHS_MAXICURWND:
        {
            dwAnd  = permissionSet & (~PERMFLAGSET_B_REGION_1);
            dwOr = isPermited == true ? PERMFLAGSET_B_REGION_1 : (0x0);
        }

        break;

    case E_TEF_35TOUCHS_MINICURWND:
        {
            dwAnd  = permissionSet & (~PERMFLAGSET_B_REGION_2);
            dwOr = isPermited == true ? PERMFLAGSET_B_REGION_2 : (0x0);
        }

        break;

    case E_TEF_6MTOUCHS_ALL:
        {
            dwAnd  = permissionSet & (~PERMFLAGSET_C_REGION);
            dwOr = isPermited == true ? PERMFLAGSET_C_REGION : (0x0);
        }

        break;

    case E_TEF_6MTOUCHS_DESPDESKTOP:
        {
            dwAnd  = permissionSet & (~PERMFLAGSET_C_REGION_0);
            dwOr = isPermited == true ? PERMFLAGSET_C_REGION_0 : (0x0);
        }

        break;

    case E_TEP_LOWLIM_ALL:
        {
            dwAnd  = permissionSet & (~PERMFLAGSET_D_REGION);
            dwOr = isPermited == true ? PERMFLAGSET_D_REGION : (0x0);
        }
        break;

    case E_TEP_LOWLIM_SHOWWNDSBS:
        {
            dwAnd  = permissionSet & (~PERMFLAGSET_D_REGION_0);
            dwOr = isPermited == true ? PERMFLAGSET_D_REGION_0 : (0x0);
        }

        break;

    case E_TEP_NSPLOC_ALL:
        {
            dwAnd  = permissionSet & (~PERMFLAGSET_E_REGION);
            dwOr = isPermited == true ? PERMFLAGSET_E_REGION : (0x0);
        }
        break;

    case E_TEP_NSPLOC_CLSCURWND:
        {
            dwAnd  = permissionSet & (~PERMFLAGSET_E_REGION_0);
            dwOr = isPermited == true ? PERMFLAGSET_E_REGION_0 : (0x0);
        }
        break;

    case E_TEP_NSPLOC_REFRESHWND:
        {
            dwAnd  = permissionSet & (~PERMFLAGSET_E_REGION_1);
            dwOr = isPermited == true ? PERMFLAGSET_E_REGION_1 : (0x0);
        }

        break;

    case E_TEP_NSPLOC_MOVWND:
        {
            dwAnd  = permissionSet & (~PERMFLAGSET_E_REGION_2);
            dwOr = isPermited == true ? PERMFLAGSET_E_REGION_2 : (0x0);
        }
        break;

    case E_TEP_NSPLOC_SVDOC:
        {
            dwAnd  = 0;
            dwOr = 0;
        }
        break;

    default:
        break;
    }

    permissionSet = dwOr | dwAnd;   
}