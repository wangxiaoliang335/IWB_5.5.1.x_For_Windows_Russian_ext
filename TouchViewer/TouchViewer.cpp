// TouchViewer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TouchViewer.h"
#include "Stroke.h"
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
CStrokeCollection g_StrkColDrawing;
CStrokeCollection g_StrkColFinished;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TOUCHVIEWER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TOUCHVIEWER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TOUCHVIEWER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH); //(HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TOUCHVIEWER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   BYTE digitizerStatus = (BYTE)GetSystemMetrics(SM_DIGITIZER);
   if ((digitizerStatus & (0x80 + 0x40)) == 0) //Stack Ready + MultiTouch
   {
       MessageBox(0, L"No touch support is currently availible", L"Error", MB_OK);
       return 1;
   }

   BYTE nInputs = (BYTE)GetSystemMetrics(SM_MAXIMUMTOUCHES);
   wsprintf(szTitle, L"%s - %d touch inputs", szTitle, nInputs);

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // Register the application window for receiving multi-touch input.
   if (!RegisterTouchWindow(hWnd, 0))
   {
       MessageBox(hWnd, L"Cannot register application window for touch input", L"Error", MB_OK);
       return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
// Returns color for the newly started stroke.
// in:
//      bPrimaryContact     flag, whether the contact is the primary contact
// returns:
//      COLORREF, color of the stroke
COLORREF GetTouchColor(bool bPrimaryContact)
{
    static int s_iCurrColor = 0;    // Rotating secondary color index
    static COLORREF s_arrColor[] =  // Secondary colors array
    {
        RGB(255,   0,   0),          // Red
        RGB(  0, 255,   0),          // Green
        RGB(  0,   0, 255),          // Blue
        RGB(  0, 255, 255),          // Cyan
        RGB(255,   0, 255),          // Magenta
        RGB(255, 255,   0)           // Yellow
    };

    COLORREF color;
    if (bPrimaryContact)
    {
        // The application renders the primary contact in black.
        color = RGB(0, 0, 0);         // Black
    }
    else
    {
        // Take the current secondary color.
        color = s_arrColor[s_iCurrColor];

        // Move to the next color in the array.
        s_iCurrColor = (s_iCurrColor + 1) % (sizeof(s_arrColor) / sizeof(s_arrColor[0]));
    }
    return color;
}

// Extracts contact point in client area coordinates (pixels) from a 
// TOUCHINPUT structure.
// in:
//      hWnd        window handle
//      ti          TOUCHINPUT structure (info about contact)
// returns:
//      POINT with contact coordinates
POINT GetTouchPoint(HWND hWnd, const TOUCHINPUT& ti)
{
    POINT pt;
    pt.x = TOUCH_COORD_TO_PIXEL(ti.x);
    pt.y = TOUCH_COORD_TO_PIXEL(ti.y);
    ScreenToClient(hWnd, &pt);
    return pt;
}

// Handler for touch-down input.
// in:
//      hWnd        window handle
//      ti          TOUCHINPUT structure (info about contact)
void OnTouchDownHandler(HWND hWnd, const TOUCHINPUT& ti)
{
    // Create a new stroke, add a point, and assign a color to it.
    CStroke strkNew;
    POINT p = GetTouchPoint(hWnd, ti);

    strkNew.AddPoint(p);
    strkNew.SetLineColor(GetTouchColor((ti.dwFlags & TOUCHEVENTF_PRIMARY) != 0));
    strkNew.SetDotColor(RGB(255, 0, 0));
    strkNew.SetId(ti.dwID);

    // Add the new stroke to the collection of strokes being drawn.
    g_StrkColDrawing.AddStroke(strkNew);
}

// Handler for touch-move input.
// in:
//      hWnd        window handle
//      ti          TOUCHINPUT structure (info about contact)
void OnTouchMoveHandler(HWND hWnd, const TOUCHINPUT& ti)
{
    // Find the stroke in the collection of the strokes being drawn.
    CStroke* pStroke = g_StrkColDrawing.FindStrokeById(ti.dwID);
    if (pStroke == NULL) return;
    POINT p = GetTouchPoint(hWnd, ti);

    // Add the contact point to the stroke.
    pStroke->AddPoint(p);

    // Partial redraw: redraw only the last line segment.
    HDC hDC = GetDC(hWnd);
    pStroke->DrawLast(hDC);
    ReleaseDC(hWnd, hDC);
}

// Handler for touch-up message.
// in:
//      hWnd        window handle
//      ti          TOUCHINPUT structure (info about contact)
void OnTouchUpHandler(HWND hWnd, const TOUCHINPUT& ti)
{

    // Find the stroke in the collection of the strokes being drawn.
    CStroke* pStroke = g_StrkColDrawing.FindStrokeById(ti.dwID);

    // Add the finished stroke to the collection of finished strokes.
    g_StrkColFinished.AddStroke(*pStroke);

    // Remove finished stroke from the collection of strokes being drawn.
    g_StrkColDrawing.RemoveStroke(*pStroke);

    // Redraw the window.
    InvalidateRect(hWnd, NULL, FALSE);
}
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case IDM_CLEAR:
				g_StrkColFinished.Clear();
				g_StrkColDrawing.Clear() ;

				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);

				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here.
            g_StrkColFinished.Draw(hdc);
            g_StrkColDrawing.Draw(hdc);

            EndPaint(hWnd, &ps);
        }
        break;

    case WM_TOUCH:
    {
        UINT cInputs = LOWORD(wParam);
        PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs];
        if (NULL != pInputs)
        {
            if (GetTouchInputInfo((HTOUCHINPUT)lParam,
                cInputs,
                pInputs,
                sizeof(TOUCHINPUT)))
            {
                // process pInputs
                BOOL s_debug = TRUE;
                if (pInputs->x == 132 * 100 && pInputs->y == 373 * 100)
                {
                    s_debug = TRUE;
                }

                if (s_debug)
                {
                    TCHAR szDebug[256];
                    const TCHAR*  szEvent;
                    if (pInputs->dwFlags & TOUCHEVENTF_DOWN)
                    {
                        szEvent = _T("Down");
                    }
                    else if (pInputs->dwFlags & TOUCHEVENTF_MOVE)
                    {
                        szEvent = _T("Move");
                    }
                    else if (pInputs->dwFlags & TOUCHEVENTF_UP)
                    {
                        szEvent = _T("Up");
                    }
                    else
                    {
                        szEvent = _T("Unknown");
                    }

                    _stprintf_s(
                        szDebug,
                        _countof(szDebug), 
                        _T("<x,y>=<%d,%d>,%s\n"), 
                        pInputs->x / 100, 
                        pInputs->y / 100,
                        szEvent);
                    OutputDebugString(szDebug);
                }

                // handler.
                for (unsigned int i = 0; i < cInputs; ++i)
                {
                    if (pInputs[i].dwFlags & TOUCHEVENTF_DOWN)
                    {
                        OnTouchDownHandler(hWnd, pInputs[i]);
                    }
                    else if (pInputs[i].dwFlags & TOUCHEVENTF_MOVE)
                    {
                        OnTouchMoveHandler(hWnd, pInputs[i]);
                    }
                    else if (pInputs[i].dwFlags & TOUCHEVENTF_UP)
                    {
                        OnTouchUpHandler(hWnd, pInputs[i]);
                    }
                }
                
                if (!CloseTouchInputHandle((HTOUCHINPUT)lParam))
                {
                    // error handling
                }
            }
            else
            {
                // GetLastError() and error handling
            }
            delete[] pInputs;
        }
        else
        {
            // error handling, presumably out of memory
        }
        //OutputDebugString(_T("WM_TOUCH\n"));

        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
