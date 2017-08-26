// Xmouse.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Xmouse.h"

#include <ShlObj.h>
#include <Commdlg.h>

#include "ControlCodes.h"
#include "ControlProfile.h"
#include "Gamepad.h"

#define MAX_LOADSTRING 100

// window dimensions
#define WND_WIDTH 1250
#define WND_HEIGHT 800

// Global Variables:
HINSTANCE hInst;							// current instance
wchar_t szTitle[MAX_LOADSTRING];			// The title bar text
wchar_t szWindowClass[MAX_LOADSTRING];		// the main window class name

HWND mainWnd;								// main program window

HWND controlBoxes[CONTROL_COUNT];			// array holding the combo boxes for all the controls

std::wstring roamingPath;					// path to Xmouse folder in AppData/Roaming
ControlProfile *ctrlProf;					// object for performing ControlProfile functions
Gamepad *gpad;								// gamepad object

DWORD stickListenerThreadId;				// analog stick listener thread ID
HANDLE stickListenerHandle;					// analog stick listener thread handle

DWORD buttonListenerThreadId;				// button listener thread ID
HANDLE buttonListenerHandle;				// button listener thread handle

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void				addMenuItems(HWND, const wchar_t*[], int);

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
    if ((mainWnd = InitInstance(hInstance, nCmdShow)) == NULL)
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_XMOUSE));

	// find path of AppData\Roaming and create the Xmouse folder in it
	LPWSTR wszPath = NULL;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &wszPath);
	roamingPath = (std::wstring) wszPath + L"\\Xmouse\\";
	CreateDirectory(roamingPath.c_str(), NULL);

	// initialize control profile
	ctrlProf = new ControlProfile(controlBoxes);

	// load last config, default config, or create default config, in order depending on existence

	// bool for debugging the initial profile load (activates messages)
	bool debugMode = FALSE;

	if (!ctrlProf->loadProfile(roamingPath.c_str(), L"LastConfig", debugMode))
	{
		if (!ctrlProf->loadProfile(roamingPath.c_str(), L"DefaultConfig", debugMode))
		{
			// set defaults for each control box
			int defaultControlValues[] = { 1,2,1,2,7,5,8,4,1,2,2,3,9,10,11,6 };
			for (int i = 0; i < CONTROL_COUNT; ++i)
				SendMessage(controlBoxes[i], CB_SETCURSEL, defaultControlValues[i], 0);

			// save the current settings as the profile "Default", don't show message
			ctrlProf->saveProfile(roamingPath.c_str(), L"DefaultConfig", debugMode);
			ctrlProf->mapControls(debugMode);
		}
	}

	// initialize control listener
	gpad = new Gamepad(mainWnd, ctrlProf, roamingPath);

	// create and start gamepad listener threads
	stickListenerHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) Gamepad::stickListen, gpad, 0, &stickListenerThreadId);
	buttonListenerHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE) Gamepad::buttonListen, gpad, 0, &buttonListenerThreadId);

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
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | CW_USEDEFAULT,
	   GetSystemMetrics(SM_CXSCREEN) / 2 - WND_WIDTH / 2, GetSystemMetrics(SM_CYSCREEN) / 2 - WND_HEIGHT / 2, WND_WIDTH, WND_HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return NULL;
   }

   // create menu items to be added to each combo box
   // ensure the index of each menu item is in accordance with the appropriate code in ControlCodes.h
   const wchar_t *stickBoxItems[] = { L"Nothing", L"Mouse", L"Scroll" };
   int stickBoxItemCount = sizeof(stickBoxItems) / sizeof(const wchar_t *);

   const wchar_t *buttonBoxItems[] = { L"Nothing", L"Left Click", L"Right Click", L"Middle Click", L"Copy", L"Paste", L"Show Desktop",
	   L"View Windows", L"Previous Window",L"Browser Back", L"Browser Forward", L"Start Menu" };
   int buttonBoxItemCount = sizeof(buttonBoxItems) / sizeof(const wchar_t *);

   const wchar_t *triggerBoxItems[] = { L"Nothing", L"Speed Up Mouse", L"Speed Up Scroll" };
   int triggerBoxItemCount = sizeof(triggerBoxItems) / sizeof(const wchar_t *);
   

   // store the (x,y) coordinates of each box in order *see ControlCodes.h
   int controlBoxCoords[16][2] =
   { { 136,274 },	// left stick
   { 776,606 },		// right stick
   { 203,44 },		// left trigger
   { 894,44 },		// right trigger
   { 88,403 },		// up
   { 302,603 },		// down
   { 81,463 },		// left
   { 568,587 },		// right
   { 990,401 },		// a
   { 985,358 },		// b
   { 958,272 },		// x
   { 971,315 },		// y
   { 166,150 },		// left bumper
   { 916,158 },		// right bumper
   { 624,148 },		// start
   { 459,148 } };	// select

	// create combo boxes for each control in controlBoxes
   for (int i = 0; i < CONTROL_COUNT; ++i)
   {
	   controlBoxes[i] = CreateWindow(
		   L"COMBOBOX",												// Predefined class; Unicode assumed
		   NULL,													// Deafult box text (none)
		   WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,				// Styles
		   controlBoxCoords[i][0],									// x position
		   controlBoxCoords[i][1],									// y position
		   150,														// Box width
		   500,														// Box height
		   hWnd,													// Parent window
		   NULL,													// No menu.
		   hInst,													// Handle to instance
		   NULL);													// Pointer not needed.
   }

   // add the correct menu options for each box

   addMenuItems(controlBoxes[LEFT_STICK], stickBoxItems, stickBoxItemCount);
   addMenuItems(controlBoxes[RIGHT_STICK], stickBoxItems, stickBoxItemCount);

   addMenuItems(controlBoxes[LEFT_TRIGGER], triggerBoxItems, triggerBoxItemCount);
   addMenuItems(controlBoxes[RIGHT_TRIGGER], triggerBoxItems, triggerBoxItemCount);

   for (int i = 4; i < CONTROL_COUNT; ++i)
   {
	   addMenuItems(controlBoxes[i], buttonBoxItems, buttonBoxItemCount);
   }

   // create apply button
   RECT rect;
   GetClientRect(hWnd, &rect);

   CreateWindow(L"BUTTON", L"Apply",
	   WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
	   rect.right - rect.left - 110,
	   rect.bottom - rect.top - 40,
	   100, 30,
	   hWnd, (HMENU)IDM_APPLY_CONTROLS, hInst, NULL);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return hWnd;
}

/*
function:	addMenuItems(HWND, const wchar_t *[], int)

purpose:	adds menu items to a combo box

params:		HWND cbox - combo box to add items to
			const wchar_t *[] menuItems - items to add to box
			int itemCount - number of items being added
*/
void addMenuItems(HWND cbox, const wchar_t *menuItems[], int itemCount)
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
			case IDM_SAVE_PROFILE:
			{
				OPENFILENAME ofn;
				wchar_t szFilePath[MAX_PATH] = L"";

				ZeroMemory(&ofn, sizeof(ofn));

				ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
				ofn.hwndOwner = hWnd;
				ofn.lpstrFilter = L"Config Settings Files (*.ini)\0*.ini\0";
				ofn.lpstrFile = szFilePath;
				ofn.nMaxFile = MAX_PATH;
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
				ofn.lpstrDefExt = L"ini";

				// change working dir to default location of INIs
				_wchdir(roamingPath.c_str());
				if (GetSaveFileName(&ofn))
				{
					wchar_t profileName[MAX_PATH];
					_wsplitpath_s(szFilePath, NULL, NULL, NULL, NULL, profileName, MAX_PATH, NULL, NULL);
					ctrlProf->saveProfile(roamingPath, profileName);
				}
			}
				break;
			case IDM_LOAD_PROFILE:
			{
				OPENFILENAME ofn;
				wchar_t szFilePath[MAX_PATH] = L"";

				ZeroMemory(&ofn, sizeof(ofn));

				ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
				ofn.hwndOwner = hWnd;
				ofn.lpstrFilter = L"Config Settings Files (*.ini)\0*.ini\0";
				ofn.lpstrFile = szFilePath;
				ofn.nMaxFile = MAX_PATH;
				ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
				ofn.lpstrDefExt = L"ini";

				// change working dir to default location of INIs
				_wchdir(roamingPath.c_str());
				if (GetOpenFileName(&ofn))
				{
					wchar_t profileName[MAX_PATH];
					_wsplitpath_s(szFilePath, NULL, NULL, NULL, NULL, profileName, MAX_PATH, NULL, NULL);
					ctrlProf->loadProfile(roamingPath, profileName);
				}
			}
				break;
			case IDM_APPLY_CONTROLS:
			{
				ctrlProf->mapControls();
			}
				break;
			case IDM_RESET_CONTROLS:
			{
				for (int i = 0; i < CONTROL_COUNT; ++i)
					SendMessage(controlBoxes[i], CB_SETCURSEL, 0, 0);
				ctrlProf->mapControls();
			}
			break;
			case IDM_SET_DEADZONES:
			{
				gpad->setDeadzones();
			}
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
		ctrlProf->saveProfile(roamingPath.c_str(), L"LastConfig", FALSE);
		CloseHandle(stickListenerHandle);
		CloseHandle(buttonListenerHandle);
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
