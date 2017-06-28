// Xmouse.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Xmouse.h"

#include <Xinput.h>

#include "ControlCodes.h"
#include "ControlProfile.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND controlBoxes[CONTROL_COUNT];				// array holding the combo boxes for all the controls

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// item

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

/*
function:	addMenuItems(HWND, const WCHAR *[], int)

purpose:	adds menu items to a combo box

params:		HWND cbox - combo box to add items to
			const WCHAR *[] menuItems - items to add to box
			int itemCount - number of items being added
*/
void addMenuItems(HWND cbox, const WCHAR *menuItems[], int itemCount)
{
	for (int i = 0; i < itemCount; ++i)
		SendMessage(cbox, CB_ADDSTRING, 0, (LPARAM)menuItems[i]);
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
			// create menu items to be added to each combo box
			const WCHAR *stickBoxItems[] = { L"Mouse", L"Scroll", L"Inverted Scroll" };
			int stickBoxItemCount = sizeof(stickBoxItems) / sizeof(const WCHAR *);

			const WCHAR *triggerBoxItems[] = { L"Speed Up Mouse", L"Speed Up Scroll" };
			int triggerBoxItemCount = sizeof(triggerBoxItems) / sizeof(const WCHAR *);
			
			const WCHAR *buttonBoxItems[] = { L"Left Click", L"Right Click", L"Keyboard", L"Show Desktop", L"Next Window", L"Previous Window", L"Browser Back", L"Browser Forward", L"Start Menu" };
			int buttonBoxItemCount = sizeof(buttonBoxItems) / sizeof(const WCHAR *);

			// store the (x,y) coordinates of each box in order *see ControlCodes.h
			int controlBoxCoords[16][2] =
			{	{ 136,274 },	// left stick
				{ 776,606 },	// right stick
				{ 88,403 },		// up
				{ 302,603 },	// down
				{ 81,463 },		// left
				{ 568,587 },	// right
				{ 990,401 },	// a
				{ 985,358 },	// b
				{ 971,315 },	// x
				{ 958,272 },	// y
				{ 166,150 },	// left bumper
				{ 203,44 },		// left trigger
				{ 916,158 },	// right bumper
				{ 894,44 },		// right trigger
				{ 624,148 },	// start
				{ 459,148 } };	// select
			
			// create combo boxes for each control in controlBoxes
			
			for (int i = 0; i < CONTROL_COUNT; ++i)
			{
				controlBoxes[i] = CreateWindow(
					L"COMBOBOX",										// Predefined class; Unicode assumed
					L"OK",												// Box text
					WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,  // Styles
					controlBoxCoords[i][0],								// x position
					controlBoxCoords[i][1],								// y position
					150,												// Box width
					500,												// Box height
					hWnd,												// Parent window
					NULL,												// No menu.
					(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),		// Handle to instance
					NULL);												// Pointer not needed.
			}

			// add the correct menu options for each box
			
			addMenuItems(controlBoxes[LEFT_STICK], stickBoxItems, stickBoxItemCount);
			addMenuItems(controlBoxes[RIGHT_STICK], stickBoxItems, stickBoxItemCount);

			addMenuItems(controlBoxes[UP], buttonBoxItems, buttonBoxItemCount);
			addMenuItems(controlBoxes[DOWN], buttonBoxItems, buttonBoxItemCount);
			addMenuItems(controlBoxes[LEFT], buttonBoxItems, buttonBoxItemCount);
			addMenuItems(controlBoxes[RIGHT], buttonBoxItems, buttonBoxItemCount);
			addMenuItems(controlBoxes[A], buttonBoxItems, buttonBoxItemCount);
			addMenuItems(controlBoxes[B], buttonBoxItems, buttonBoxItemCount);
			addMenuItems(controlBoxes[X], buttonBoxItems, buttonBoxItemCount);
			addMenuItems(controlBoxes[Y], buttonBoxItems, buttonBoxItemCount);
			addMenuItems(controlBoxes[LEFT_BUMPER], buttonBoxItems, buttonBoxItemCount);
			addMenuItems(controlBoxes[RIGHT_BUMPER], buttonBoxItems, buttonBoxItemCount);
			addMenuItems(controlBoxes[START], buttonBoxItems, buttonBoxItemCount);
			addMenuItems(controlBoxes[SELECT], buttonBoxItems, buttonBoxItemCount);

			addMenuItems(controlBoxes[LEFT_TRIGGER], triggerBoxItems, triggerBoxItemCount);
			addMenuItems(controlBoxes[RIGHT_TRIGGER], triggerBoxItems, triggerBoxItemCount);
			

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
