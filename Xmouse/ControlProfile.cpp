#include "stdafx.h"
#include <string>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <sstream>
#include <functional>

#include "ControlProfile.h"
#include "ControlCodes.h"

#define LOAD_ERROR L"LDER"

ControlProfile::ControlProfile(HWND *controlBoxes)
{
    this->controlBoxes = controlBoxes;

    currentMouseMod = 1;
    currentScrollMod = 1;
}

/*
function:   saveProfile(wstring, wstring, bool)

purpose:    save the current control settings in an INI

params:     wstring roamingPath - path to the ~\appdata\roaming\ folder
            wstring profileName - the name (and filename) of the control profile
            bool showMessage - default set to TRUE, can otherwise set to show or not show messages

returns:    TRUE if successful, FALSE if not
*/
bool ControlProfile::saveProfile(std::wstring roamingPath, std::wstring profileName, bool showMessage)
{
    std::wstring path = roamingPath + profileName + L".ini";


    int selectionIndex;
    for (int i = 0; i < MOUSE_SPEED; ++i)
    {
        if ((selectionIndex = SendMessage(controlBoxes[i], CB_GETCURSEL, 0, 0)) == CB_ERR && showMessage)
        {
            ::MessageBox(GetParent(controlBoxes[0]), _T("Invalid control selected. \"Nothing\" will be substituted."), _T("Error Saving Profile"), MB_OK);
            selectionIndex = 0;
        }
        wchar_t keyName[5], value[5];
        _itow_s(i, keyName, 5, 10);
        _itow_s(selectionIndex, value, 5, 10);

        WritePrivateProfileString(L"Default", keyName, value, path.c_str());
    }

    for (int i = MOUSE_SPEED; i <= SCROLL_MODIFIER; ++i)
    {
        LRESULT pos = SendMessage(controlBoxes[i], TBM_GETPOS, 0, 0);
        wchar_t keyName[5], value[5];
        wsprintfW(value, L"%d", pos);
        _itow_s(i, keyName, 5, 10);

        WritePrivateProfileString(L"Default", keyName, value, path.c_str());
    }

    if (showMessage)
    {
        std::wstring successMessage = L"The profile " + profileName + L" has been saved.";
        ::MessageBox(GetParent(controlBoxes[0]), successMessage.c_str(), _T("Profile Saved"), MB_OK);
    }
    return TRUE;
}

/*
function:   loadProfile(wstring, wstring, bool)

purpose:    load control settings from an INI

params:     wstring roamingPath - path to the ~\appdata\roaming\ folder
            wstring profileName - the name (and filename) of the control profile
            bool showMessage - default set to TRUE, can otherwise set to show or not show messages

returns:    TRUE if successful, FALSE if not
*/
bool ControlProfile::loadProfile(std::wstring roamingPath, std::wstring profileName, std::function<void()> updateLabels, bool showMessage)
{
    std::wstring path = roamingPath + profileName + L".ini";

    // check if file exists
    if (!PathFileExists(path.c_str()))
    {
        if (showMessage)
            ::MessageBox(GetParent(controlBoxes[0]), _T("Profile could not be found."), _T("Load Profile Error"), MB_OK);
        return FALSE;
    }

    for (int i = 0; i < MOUSE_SPEED; ++i)
    {
        wchar_t keyName[5], value[5];

        _itow_s(i, keyName, 5, 10);

        GetPrivateProfileString(L"Default", keyName, LOAD_ERROR, value, 5, path.c_str());

        if (wcscmp(value, LOAD_ERROR) == 0)
        {
            if (showMessage)
                ::MessageBox(GetParent(controlBoxes[0]), _T("Profile is incomplete."), _T("Load Profile Error"), MB_OK);
            return FALSE;
        }
        else
        {
            int selectionIndex = _wtoi(value);
            if (SendMessage(controlBoxes[i], CB_SETCURSEL, selectionIndex, 0) == CB_ERR && showMessage)
                ::MessageBox(GetParent(controlBoxes[0]), _T("Profile contains and invalid value. \"Nothing\" will be substituted."), _T("Load Profile Warning"), MB_OK);
        }
    }

    for (int i = MOUSE_SPEED; i <= SCROLL_MODIFIER; ++i)
    {
        wchar_t keyName[5], value[5];

        _itow_s(i, keyName, 5, 10);

        GetPrivateProfileString(L"Default", keyName, LOAD_ERROR, value, 5, path.c_str());

        if (wcscmp(value, LOAD_ERROR) == 0)
        {
            if (showMessage)
                ::MessageBox(GetParent(controlBoxes[0]), _T("Profile is incomplete."), _T("Load Profile Error"), MB_OK);
            return FALSE;
        }
        else
        {
            if (SendMessage(controlBoxes[i], TBM_SETPOS, TRUE, (LPARAM)_wtoi(value)) == CB_ERR && showMessage)
                ::MessageBox(GetParent(controlBoxes[0]), _T("Profile contains and invalid value. \"Nothing\" will be substituted."), _T("Load Profile Warning"), MB_OK);
        }
    }

    // update value lables
    updateLabels();

    if (showMessage)
    {
        std::wstring successMessage = L"The profile " + profileName + L" has been loaded.";
        ::MessageBox(GetParent(controlBoxes[0]), successMessage.c_str(), _T("Profile Loaded"), MB_OK);
    }
    mapControls(FALSE);
    return TRUE;
}

/*
function:   mapControls()

purpose:    map the current control settings by populating the controlMap

params:     bool showMessage - default set to TRUE, can otherwise set to show or not show messages

returns:    TRUE if successful, FALSE if not
*/
bool ControlProfile::mapControls(bool showMessage)
{
    int selectionIndex;
    for (int i = 0; i < MOUSE_SPEED; ++i)
    {
        if ((selectionIndex = SendMessage(controlBoxes[i], CB_GETCURSEL, 0, 0)) == CB_ERR && showMessage)
        {
            ::MessageBox(GetParent(controlBoxes[0]), _T("Invalid control selected. Controls must be remapped."), _T("Error Applying Controls"), MB_OK);
            return FALSE;
        }
        else
        {
            if (i <= 1) selectionIndex *= -1;
            else if (i <= 3) selectionIndex *= -4;
            controlMap[i] = selectionIndex;
        }
    }

    float* trackbarValues[] = { &mouseSpeed, &mouseMod, &scrollSpeed, &scrollMod };
    for (int i = MOUSE_SPEED; i <= SCROLL_MODIFIER; ++i)
    {
        if ((selectionIndex = SendMessage(controlBoxes[i], TBM_GETPOS, 0, 0)) == CB_ERR && showMessage)
        {
            ::MessageBox(GetParent(controlBoxes[0]), _T("Invalid control selected. Controls must be remapped."), _T("Error Applying Controls"), MB_OK);
            return FALSE;
        }
        else
        {
            *trackbarValues[i - MOUSE_SPEED] = selectionIndex;
        }
    }

    if (showMessage)
        ::MessageBox(GetParent(controlBoxes[0]), L"Controls applied.", L"Success", MB_OK);

    return TRUE;
}

/*
function:   controlInput()

purpose:    translate the input and execute the appropriate command

params:     int controlCode - the code for what control was
            float paramA, paramB - generic parameters used for sending data,
                                    default to 0

returns:    TRUE if successful, FALSE if not
*/
void ControlProfile::controlInput(int controlCode, float paramA, float paramB)
{
    // stream for debugging
    std::wstringstream wss(L"");

    wss << controlCode;

    // translate
    controlCode = controlMap[controlCode];

    if (controlCode > 0)
    {
        wss << " " << controlCode << "\n";
        OutputDebugString(wss.str().c_str());
    }

    // array of INPUT structures for keystrokes
    INPUT ips[3];

    switch (controlCode)
    {
    case NO_CONTROL:
        break;

        // stick controls
    case MOUSE:
        mouse_event(MOUSEEVENTF_MOVE, paramA * mouseSpeed * currentMouseMod, -paramB * mouseSpeed * currentMouseMod, 0, 0);
        break;
    case SCROLL:
        mouse_event(MOUSEEVENTF_HWHEEL, 0, 0, paramA * scrollSpeed * currentScrollMod, 0);
        mouse_event(MOUSEEVENTF_WHEEL, 0, 0, paramB * scrollSpeed * currentScrollMod, 0);
        break;

        // trigger controls
    case SPEED_UP_MOUSE:
        currentMouseMod = 1 + paramA * (mouseMod - 1);
        break;
    case SPEED_UP_SCROLL:
        currentScrollMod = 1 + paramA * (scrollMod - 1);
        break;

        // button controls
    case LEFT_CLICK:
        if (paramA > 0)
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        else
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        break;
    case RIGHT_CLICK:
        if (paramA > 0)
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
        else
            mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
        break;
    case MIDDLE_CLICK:
        if (paramA > 0)
            mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
        else
            mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
        break;
    case COPY:
        ips[0].type = INPUT_KEYBOARD;
        ips[0].ki.wScan = 0;
        ips[0].ki.time = 0;
        ips[0].ki.dwExtraInfo = 0;

        ips[0].ki.wVk = VK_CONTROL; // control key

        ips[1].type = INPUT_KEYBOARD;
        ips[1].ki.wScan = 0;
        ips[1].ki.time = 0;
        ips[1].ki.dwExtraInfo = 0;

        ips[1].ki.wVk = 0x43; // C key

        if (paramA > 0)
        {
            ips[0].ki.dwFlags = 0;
            ips[1].ki.dwFlags = 0;
            SendInput(2, ips, sizeof(INPUT));
        }
        else
        {
            ips[0].ki.dwFlags = KEYEVENTF_KEYUP;
            ips[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(2, ips, sizeof(INPUT));
        }
        break;
    case PASTE:
        ips[0].type = INPUT_KEYBOARD;
        ips[0].ki.wScan = 0;
        ips[0].ki.time = 0;
        ips[0].ki.dwExtraInfo = 0;

        ips[0].ki.wVk = VK_CONTROL; // control key

        ips[1].type = INPUT_KEYBOARD;
        ips[1].ki.wScan = 0;
        ips[1].ki.time = 0;
        ips[1].ki.dwExtraInfo = 0;

        ips[1].ki.wVk = 0x56; // V key

        if (paramA > 0)
        {
            ips[0].ki.dwFlags = 0;
            ips[1].ki.dwFlags = 0;
            SendInput(2, ips, sizeof(INPUT));
        }
        else
        {
            ips[0].ki.dwFlags = KEYEVENTF_KEYUP;
            ips[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(2, ips, sizeof(INPUT));
        }
        break;
    case SHOW_DESKTOP:
        ips[0].type = INPUT_KEYBOARD;
        ips[0].ki.wScan = 0;
        ips[0].ki.time = 0;
        ips[0].ki.dwExtraInfo = 0;

        ips[0].ki.wVk = VK_LWIN; // left windows key

        ips[1].type = INPUT_KEYBOARD;
        ips[1].ki.wScan = 0;
        ips[1].ki.time = 0;
        ips[1].ki.dwExtraInfo = 0;

        ips[1].ki.wVk = 0x44; // D key

        if (paramA > 0)
        {
            ips[0].ki.dwFlags = 0;
            ips[1].ki.dwFlags = 0;
            SendInput(2, ips, sizeof(INPUT));
        }
        else
        {
            ips[0].ki.dwFlags = KEYEVENTF_KEYUP;
            ips[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(2, ips, sizeof(INPUT));
        }
        break;
    case VIEW_WINDOWS:
        ips[0].type = INPUT_KEYBOARD;
        ips[0].ki.wScan = 0;
        ips[0].ki.time = 0;
        ips[0].ki.dwExtraInfo = 0;

        ips[0].ki.wVk = VK_CONTROL; // control key

        ips[1].type = INPUT_KEYBOARD;
        ips[1].ki.wScan = 0;
        ips[1].ki.time = 0;
        ips[1].ki.dwExtraInfo = 0;

        ips[1].ki.wVk = VK_MENU; // alt key

        ips[2].type = INPUT_KEYBOARD;
        ips[2].ki.wScan = 0;
        ips[2].ki.time = 0;
        ips[2].ki.dwExtraInfo = 0;

        ips[2].ki.wVk = VK_TAB; // tab key

        if (paramA > 0)
        {
            ips[0].ki.dwFlags = 0;
            ips[1].ki.dwFlags = 0;
            ips[2].ki.dwFlags = 0;
            SendInput(3, ips, sizeof(INPUT));
        }
        else
        {
            ips[0].ki.dwFlags = KEYEVENTF_KEYUP;
            ips[1].ki.dwFlags = KEYEVENTF_KEYUP;
            ips[2].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(3, ips, sizeof(INPUT));
        }
        break;
    case PREVIOUS_WINDOW:
        ips[0].type = INPUT_KEYBOARD;
        ips[0].ki.wScan = 0;
        ips[0].ki.time = 0;
        ips[0].ki.dwExtraInfo = 0;

        ips[0].ki.wVk = VK_MENU; // alt key

        ips[1].type = INPUT_KEYBOARD;
        ips[1].ki.wScan = 0;
        ips[1].ki.time = 0;
        ips[1].ki.dwExtraInfo = 0;

        ips[1].ki.wVk = VK_TAB; // tab key

        if (paramA > 0)
        {
            ips[0].ki.dwFlags = 0;
            ips[1].ki.dwFlags = 0;
            SendInput(2, ips, sizeof(INPUT));
        }
        else
        {
            ips[0].ki.dwFlags = KEYEVENTF_KEYUP;
            ips[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(2, ips, sizeof(INPUT));
        }
        break;
    case BROWSER_BACK:
        ips[0].type = INPUT_KEYBOARD;
        ips[0].ki.wScan = 0;
        ips[0].ki.time = 0;
        ips[0].ki.dwExtraInfo = 0;

        ips[0].ki.wVk = VK_MENU; // alt key

        ips[1].type = INPUT_KEYBOARD;
        ips[1].ki.wScan = 0;
        ips[1].ki.time = 0;
        ips[1].ki.dwExtraInfo = 0;

        ips[1].ki.wVk = VK_LEFT; // left arrow key

        if (paramA > 0)
        {
            ips[0].ki.dwFlags = 0;
            ips[1].ki.dwFlags = 0;
            SendInput(2, ips, sizeof(INPUT));
        }
        else
        {
            ips[0].ki.dwFlags = KEYEVENTF_KEYUP;
            ips[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(2, ips, sizeof(INPUT));
        }
        break;
    case BROWSER_FORWARD:
        ips[0].type = INPUT_KEYBOARD;
        ips[0].ki.wScan = 0;
        ips[0].ki.time = 0;
        ips[0].ki.dwExtraInfo = 0;

        ips[0].ki.wVk = VK_MENU; // alt key

        ips[1].type = INPUT_KEYBOARD;
        ips[1].ki.wScan = 0;
        ips[1].ki.time = 0;
        ips[1].ki.dwExtraInfo = 0;

        ips[1].ki.wVk = VK_RIGHT; // right arrow key

        if (paramA > 0)
        {
            ips[0].ki.dwFlags = 0;
            ips[1].ki.dwFlags = 0;
            SendInput(2, ips, sizeof(INPUT));
        }
        else
        {
            ips[0].ki.dwFlags = KEYEVENTF_KEYUP;
            ips[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(2, ips, sizeof(INPUT));
        }
        break;
    case START_MENU:
        ips[0].type = INPUT_KEYBOARD;
        ips[0].ki.wScan = 0;
        ips[0].ki.time = 0;
        ips[0].ki.dwExtraInfo = 0;

        ips[0].ki.wVk = VK_LWIN; // left windows key

        if (paramA > 0)
        {
            ips[0].ki.dwFlags = 0;
            SendInput(1, ips, sizeof(INPUT));
        }
        else
        {
            ips[0].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(1, ips, sizeof(INPUT));
        }
        break;

    default:
        break;
    }
}