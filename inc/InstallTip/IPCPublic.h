#pragma once
#include <Windows.h>

//////////////////////////////////////////////////////////////////////////
//通信相关接管
enum E_TipState
{
    E_State_Idle = 0,    
    E_State_Connected,
    E_State_Close   ,
    //E_State_
};

enum E_ACK_RES
{
    E_ACK_Unknown = -1,
    E_ACK_Succed	,
    E_ACK_Failed
};

enum E_SCREEN_MODE
{
    E_SCREEN_Single = 0,
    E_SCREEN_Double
};

enum E_IPC_CMD
{
    E_IPC_CMD_Unknown = -1	,
    E_IPC_CMD_Conn	  = 2901 	,
    E_IPC_CMD_Conn_ACK	,
    E_IPC_CMD_ReLayout	,
    E_IPC_CMD_ReLayout_ACK	,
    E_IPC_CMD_TuningWebcam  ,
    E_IPC_CMD_TuningWebcam_ACK  ,
    E_IPC_CMD_TuningLaser  ,
    E_IPC_CMD_TuningLaser_ACK  ,
    E_IPC_CMD_MakeBackSplash,
    E_IPC_CMD_MakeBackSplash_ACK,
    E_IPC_CMD_EliminateBackSplash,
    E_IPC_CMD_EliminateBackSplash_ACK,
    E_IPC_CMD_BackSplashVanished,
    E_IPC_CMD_BackSplashVanished_ACK,
    E_IPC_CMD_AutoCali	,
    E_IPC_CMD_AutoCali_ACK	,
    E_IPC_CMD_Sample	,
    E_IPC_CMD_Sample_ACK	,
    E_IPC_CMD_Exist		,
    E_IPC_CMD_Exist_ACK	,
    E_IPC_CMD_Close		
};

typedef struct ST_CONN_INFO
{
    DWORD	dwMinTestExisted;
    DWORD	dwTimeout;
    HWND    hClientWnd;
    HWND    hDisplayWnd;
}CONN_INFO;

typedef struct ST_CONN_ACK_INFO
{
    E_ACK_RES res;
}CONN_ACK_INFO;

typedef struct ST_RELAYOUT_INFO
{
    int	l;
    int	t;
    int	w;
    int	h;
}RELAYOUT_INFO;

typedef struct ST_RELAYOUT_ACK_INFO
{
    E_ACK_RES res;	
}RELAYOUT_ACK_INFO;

typedef struct ST_TUNINGWEBCAM_INFO
{    
}TUNINGWEBCAM_INFO;

typedef struct ST_TUNINGWEBCAM_ACK_INFO
{
    E_ACK_RES res;	
    /*E_SCREEN_MODE screenMode;*/
}TUNINGWEBCAM_ACK_INFO;

typedef struct ST_TUNINGLASER_INFO
{    
}TUNINGLASER_INFO;

typedef struct ST_TUNINGLASER_ACK_INFO
{
    E_ACK_RES res;	
}TUNINGLASER_ACK_INFO;

typedef struct ST_MAKEBACKSPLASH_INFO
{    
}MAKEBACKSPLASH_INFO;

typedef struct ST_MAKEBACKSPLASH_ACK_INFO
{
    E_ACK_RES res;	
}MAKEBACKSPLASH_ACK_INFO;

typedef struct ST_ELIMINATEBACKSPLASH_INFO
{    
}ELIMINATEBACKSPLASH_INFO;

typedef struct ST_ELIMINATEBACKSPLASH_ACK_INFO
{
    E_ACK_RES res;	
}ELIMINATEBACKSPLASH_ACK_INFO;

typedef struct ST_BACKSPLASHVANISHED_INFO
{    
    bool isVanished;
}BACKSPLASHVANISHED_INFO;

typedef struct ST_BACKSPLASHVANISHED_ACK_INFO
{
    E_ACK_RES res;	
}BACKSPLASHVANISHED_ACK_INFO;

typedef struct ST_SAMPLE_INFO
{
    //
}SAMPLE_INFO;


typedef struct ST_SAMPLE_ACK_INFO
{	
    E_ACK_RES res;
}SAMPLE_ACK_INFO;

typedef struct ST_AUTOCALI_INFO
{
}AUTOCALI_INFO;

typedef struct ST_AUTOCALI_ACK_INFO
{	
    E_ACK_RES res;
}AUTOCALI_ACK_INFO;

typedef struct ST_EXIST_INFO
{
}EXIST_INFO;

typedef struct ST_EXIST_ACK_INFO
{	
    E_ACK_RES res;
}EXIST_ACK_INFO;

typedef struct ST_CLOSE_INFO
{
}CLOSE_INFO;

typedef struct ST_CLOSE_ACK_INFO
{	
    E_ACK_RES res;
}CLOSE_ACK_INFO;

typedef struct ST_IPC_CMD_INFO
{
    DWORD dwAuthToken;
    E_IPC_CMD cmd;

    union
    {
        CONN_INFO 		        connInfo;
        RELAYOUT_INFO 		    relayoutInfo;
        TUNINGWEBCAM_INFO       tuningWebcamInfo;
        TUNINGLASER_INFO        tuningLaserInfo;
        BACKSPLASHVANISHED_INFO backsplashvanishedInfo;
        SAMPLE_INFO		        sampleInfo;
        AUTOCALI_INFO		    autoCaliInfo;
        EXIST_INFO		        existInfo;
        CONN_ACK_INFO 		    connAckInfo;
        RELAYOUT_ACK_INFO 	    relayoutAckInfo;
        TUNINGWEBCAM_ACK_INFO   tuningWebcamAckInfo;
        TUNINGLASER_ACK_INFO    tuningLaserAckInfo;
        BACKSPLASHVANISHED_ACK_INFO backsplashvanishedAckInfo;
        SAMPLE_ACK_INFO		    sampleAckInfo;
        AUTOCALI_ACK_INFO	    autoCaliAckInfo;
        EXIST_ACK_INFO		    existAckInfo;
    }info;
}IPC_CMD_INFO;

//通信相关接管
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#define WM_IPC_TOSERVER          (WM_USER + 4444)
#define WM_IPC_TOCLIENT          (WM_USER + 4445)
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
////命令行参数格式相关
//const TCHAR *ITEMPREFIX = _T("-@");
//const TCHAR *ITEMSUFFIX = _T("@-");
//
//const TCHAR *LANGCODEITEM_TAG       = _T("LG:");
//const TCHAR *SERVERWNDITTEM_TAG     = _T("HW:");
////
////////////////////////////////////////////////////////////////////////////
