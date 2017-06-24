// Xmouse.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Xmouse.h"

#include <Xinput.h>

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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_XMOUSE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_XMOUSE));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_XMOUSE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_XMOUSE);
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, 1250, 800, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
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
	/*
	case WM_LBUTTONDOWN:
		{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		wchar_t waCoord[20];
		wsprintf(waCoord, _T("(%i, %i)"), x, y);
		::MessageBox(hWnd, waCoord, _T("LMB Click"), MB_OK);
		}
		break;
	*/
	case WM_CREATE:
		{
			HWND leftStickBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				136,         // x position
				274,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

			HWND rightStickBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				776,         // x position
				606,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

			HWND upBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				88,         // x position
				403,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

			HWND downBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				302,         // x position
				603,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

			HWND leftBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				81,         // x position
				463,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

			HWND rightBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				568,         // x position
				587,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

			HWND aBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				990,         // x position
				401,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

			HWND bBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				985,         // x position
				358,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

			HWND xBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				971,         // x position
				315,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

			HWND yBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				958,         // x position
				272,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

			HWND leftBumperBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				166,         // x position
				150,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

			HWND leftTriggerBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				203,         // x position
				44,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

			HWND rightBumperBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				916,         // x position
				158,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

			HWND rightTriggerBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				894,         // x position
				44,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

			HWND startBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				624,         // x position
				148,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

			HWND selectBox = CreateWindow(
				L"COMBOBOX",  // Predefined class; Unicode assumed
				L"OK",      // Box text
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
				459,         // x position
				148,         // y position
				150,        // Box width
				10,        // Box height
				hWnd,     // Parent window
				NULL,       // No menu.
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);      // Pointer not needed.

		}
		break;
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
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
			PAINTSTRUCT     ps;
			HDC             hdc;
			BITMAP          bitmap;
			HDC             hdcMem;
			HGDIOBJ         oldBitmap;

            hdc = BeginPaint(hWnd, &ps);
			
			HBITMAP hBitmap = (HBITMAP)LoadImage(hInst, L"controller.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

			hdcMem = CreateCompatibleDC(hdc);
			oldBitmap = SelectObject(hdcMem, hBitmap);

			GetObject(hBitmap, sizeof(BITMAP), &bitmap);
			BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

			SelectObject(hdcMem, oldBitmap);
			DeleteDC(hdcMem);

            EndPaint(hWnd, &ps);
        }
        break;
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
