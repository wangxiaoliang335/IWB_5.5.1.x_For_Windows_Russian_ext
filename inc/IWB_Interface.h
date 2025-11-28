#pragma once
#ifdef IWB_INTERFACE_EXPORTS
	#define  IWB_API _declspec(dllexport)
#else
	#define IWB_API _declspec(dllimport)
#endif

extern "C"
{


//@function: Tell the position program to send optical pen message to the specified window or the thread of the caller 
//
//@parameters: hWnd, the handle of the window which will receive the optical pen message.
//
//@return values: The message identifer that will be sent from position system. Zero indicates failure.
//
//@remarks: When the hWnd parameter is NULL, then the thread which is calling OPI_PleaseNotifyMe function will receive the message. 
//          The OPI_PleaseNotifyMe function actually uses windows API function RegisterWindowMessage to create the message identifier 
//          which is unique throghout the system.
//      
IWB_API UINT OPI_PleaseNotifyMe(HWND hWnd = NULL);



//@function:Tell the position program not to send the optical pen message to the specified window or the thread of the caller any more
//
//@parameters:hWnd, the same handle of the window which passed to funtion OPI_PleaseNotifyMe
//@return values:If the function failes, the return value is 0.
//
//@remarks:If the function is called, the position program will clear the corresponding entry in the message receiver table. 
//         If the function is not called, the position program will remove the corresponding entry from the message receiver 
//         table automatically when it detects the window handle or thread id is not correct anymore.
//  
IWB_API BOOL OPI_StopNotifyMe(HWND hWnd = NULL);


}
