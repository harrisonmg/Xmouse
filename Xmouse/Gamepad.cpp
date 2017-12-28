#include "stdafx.h"
#include <Xinput.h>
#include <Shlwapi.h>
#include <sstream>
#include <bitset>

#include "Gamepad.h"
#include "ControlCodes.h"

#define LOAD_ERROR L"lder"

std::map<WORD, int> Gamepad::XinputButtons =
{ { XINPUT_GAMEPAD_DPAD_UP, UP },
{ XINPUT_GAMEPAD_DPAD_DOWN, DOWN },
{ XINPUT_GAMEPAD_DPAD_LEFT, LEFT },
{ XINPUT_GAMEPAD_DPAD_RIGHT, RIGHT },
{ XINPUT_GAMEPAD_A, A },
{ XINPUT_GAMEPAD_B, B },
{ XINPUT_GAMEPAD_X, X },
{ XINPUT_GAMEPAD_Y, Y },
{ XINPUT_GAMEPAD_LEFT_SHOULDER, LEFT_BUMPER },
{ XINPUT_GAMEPAD_RIGHT_SHOULDER, RIGHT_BUMPER },
{ XINPUT_GAMEPAD_START, START },
{ XINPUT_GAMEPAD_BACK, SELECT } };

Gamepad::Gamepad(HWND mainWnd, ControlProfile* ctrlProf, std::wstring roamingPath)
{
    this->mainWnd = mainWnd;
    this->ctrlProf = ctrlProf;

    this->iniPath = roamingPath + L"GamepadSettings.ini";

    checkConnection();

    if (PathFileExists(iniPath.c_str()))
    {
        // read and set all 4 deadzones
        wchar_t LXval[10], LYval[10], RXval[10], RYval[10];

        GetPrivateProfileString(L"Default", L"deadzoneLX", LOAD_ERROR, LXval, 10, iniPath.c_str());
        GetPrivateProfileString(L"Default", L"deadzoneLY", LOAD_ERROR, LYval, 10, iniPath.c_str());
        GetPrivateProfileString(L"Default", L"deadzoneRX", LOAD_ERROR, RXval, 10, iniPath.c_str());
        GetPrivateProfileString(L"Default", L"deadzoneRY", LOAD_ERROR, RYval, 10, iniPath.c_str());

        if (wcscmp(LXval, LOAD_ERROR) == 0 || wcscmp(LYval, LOAD_ERROR) == 0
            || wcscmp(RXval, LOAD_ERROR) == 0 || wcscmp(RYval, LOAD_ERROR) == 0)
        {
            ::MessageBox(mainWnd, _T("GamepadSettings.ini is incomplete. New analog stick deadzones will be set."), _T("Load Deadzones Error"), MB_OK);
            setDeadzones();
        }
        else
        {
            deadzoneLX = wcstof(LXval, NULL);
            deadzoneLY = wcstof(LYval, NULL);
            deadzoneRX = wcstof(RXval, NULL);
            deadzoneRY = wcstof(RYval, NULL);
        }
    }
    else
        setDeadzones();
}

/*
function:   checkConnection()

purpose:    check attempt to find the controller ID, prompt user if it can't be found
*/
void Gamepad::checkConnection()
{
    if (getControllerId() != controllerId)
    {
        bool broken = FALSE;
        // get controller id, retry if none is found
        while ((controllerId = getControllerId()) < 0 && !broken)
        {
            switch (::MessageBox(mainWnd, L"No gamepad found.", L"Error", MB_RETRYCANCEL))
            {
            case IDRETRY:
                // loop around to try to find controller again
                break;
            case IDCANCEL:
                // close if controller detection is cancelled
                broken = TRUE;
                break;
            default:
                break;
            }
        }
    }
}

/*
function:   getControllerId()

purpose:    retrieve the controller id of the first available controller

return:     returns the id of the controller or -1 if none is found
*/
int Gamepad::getControllerId()
{
    int controllerId = -1;

    for (DWORD i = 0; i < XUSER_MAX_COUNT && controllerId == -1; i++)
    {
        ZeroMemory(&state, sizeof(XINPUT_STATE));

        if (XInputGetState(i, &state) == ERROR_SUCCESS)
            controllerId = i;
    }
    return controllerId;
}

/*
function:   setDeadzones()

purpose:    sets deadzones for the gamepad sticks based on their resting positions
*/
void Gamepad::setDeadzones()
{
    ::MessageBox(mainWnd, L"Please put your analog sticks in their resting positions and press \"OK\".", L"Setting Stick Deadzones", MB_OK);


    ZeroMemory(&state, sizeof(XINPUT_STATE));
    if (XInputGetState(controllerId, &state) != ERROR_SUCCESS)
    {
        ::MessageBox(mainWnd, L"No gamepad found.", L"Error", MB_OK);
        return;
    }

    // get stick positions and set them as deadzones
    deadzoneLX = abs(fmaxf(-1, (float)state.Gamepad.sThumbLX / 32767));
    deadzoneLY = abs(fmaxf(-1, (float)state.Gamepad.sThumbLY / 32767));

    deadzoneRX = abs(fmaxf(-1, (float)state.Gamepad.sThumbRX / 32767));
    deadzoneRY = abs(fmaxf(-1, (float)state.Gamepad.sThumbRY / 32767));

    // write it all to the ini
    wchar_t LXval[10], LYval[10], RXval[10], RYval[10];
    swprintf(LXval, (size_t)10, L"%f", deadzoneLX);
    swprintf(LYval, (size_t)10, L"%f", deadzoneLY);
    swprintf(RXval, (size_t)10, L"%f", deadzoneRX);
    swprintf(RYval, (size_t)10, L"%f", deadzoneRY);

    WritePrivateProfileString(L"Default", L"deadzoneLX", LXval, iniPath.c_str());
    WritePrivateProfileString(L"Default", L"deadzoneLY", LYval, iniPath.c_str());
    WritePrivateProfileString(L"Default", L"deadzoneRX", RXval, iniPath.c_str());
    WritePrivateProfileString(L"Default", L"deadzoneRY", RYval, iniPath.c_str());

    ::MessageBox(mainWnd, L"Deadzones set.", L"Success", MB_OK);
}

/*
fucntion:   refresh()

purpose:    check connection and refresh XINPUT state

return:     false if no controller is connected
*/
bool Gamepad::refresh()
{
    if (controllerId == -1)
        checkConnection();

    if (controllerId != -1)
    {
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        if (XInputGetState(controllerId, &state) != ERROR_SUCCESS)
        {
            controllerId = -1;
            return false;
        }
        return true;
    }
    return false;
}

/*
fucntion:   listen()

purpose:    listen for gamepad input and take appropriate actions
*/
void Gamepad::stickListen(listenerArgs *largs)
{
    Gamepad *gpad = largs->gpad;
    bool *threadEnd = largs->threadEnd;

    float leftTrigger, rightTrigger;
    bool leftZeroed = FALSE, rightZeroed = FALSE;
    float normLX, normLY, leftStickX, leftStickY;
    float normRX, normRY, rightStickX, rightStickY;
    // shortening 
    int sleepTime, altSleepTime = 5;

    while (!*threadEnd)
    {
        gpad->refresh();
        sleepTime = 10;

        // get trigger positions (float 0-1)
        // these must be input before the sticks in order for the modifiers to be set
        leftTrigger = (float)gpad->state.Gamepad.bLeftTrigger / 255;
        rightTrigger = (float)gpad->state.Gamepad.bRightTrigger / 255;

        // only send zero inputs once to allow multiple triggers mapped to the same function
        if (leftTrigger > 0)
        {
            gpad->ctrlProf->controlInput(LEFT_TRIGGER, leftTrigger);
            leftZeroed = FALSE;
            sleepTime = altSleepTime;
        }
        else if (leftTrigger <= 0 && !leftZeroed)
        {
            gpad->ctrlProf->controlInput(LEFT_TRIGGER, leftTrigger);
            leftZeroed = TRUE;
            sleepTime = altSleepTime;
        }

        if (rightTrigger > 0)
        {
            gpad->ctrlProf->controlInput(RIGHT_TRIGGER, rightTrigger);
            rightZeroed = FALSE;
            sleepTime = altSleepTime;
        }
        else if (rightTrigger <= 0 && !rightZeroed)
        {
            gpad->ctrlProf->controlInput(RIGHT_TRIGGER, rightTrigger);
            rightZeroed = TRUE;
            sleepTime = altSleepTime;
        }

        // get left and right stick positions, then apply deadzone compensation (float 0-1)
        // left stick
        normLX = fmaxf(-1, (float)gpad->state.Gamepad.sThumbLX / 32767);
        normLY = fmaxf(-1, (float)gpad->state.Gamepad.sThumbLY / 32767);

        leftStickX = (abs(normLX) < gpad->deadzoneLX ? 0 : (abs(normLX) - gpad->deadzoneLX) * (normLX / abs(normLX)));
        leftStickY = (abs(normLY) < gpad->deadzoneLY ? 0 : (abs(normLY) - gpad->deadzoneLY) * (normLY / abs(normLY)));

        if (gpad->deadzoneLX > 0) leftStickX *= 1 / (1 - gpad->deadzoneLX);
        if (gpad->deadzoneLY > 0) leftStickY *= 1 / (1 - gpad->deadzoneLY);

        // right stick
        normRX = fmaxf(-1, (float)gpad->state.Gamepad.sThumbRX / 32767);
        normRY = fmaxf(-1, (float)gpad->state.Gamepad.sThumbRY / 32767);

        rightStickX = (abs(normRX) < gpad->deadzoneRX ? 0 : (abs(normRX) - gpad->deadzoneRX) * (normRX / abs(normRX)));
        rightStickY = (abs(normRY) < gpad->deadzoneRY ? 0 : (abs(normRY) - gpad->deadzoneRY) * (normRY / abs(normRY)));

        if (gpad->deadzoneRX > 0) rightStickX *= 1 / (1 - gpad->deadzoneRX);
        if (gpad->deadzoneRY > 0) rightStickY *= 1 / (1 - gpad->deadzoneRY);

        gpad->ctrlProf->controlInput(LEFT_STICK, leftStickX, leftStickY);
        gpad->ctrlProf->controlInput(RIGHT_STICK, rightStickX, rightStickY);

        // sleep to limit the rate of mouse movement
        Sleep(sleepTime);
    }
}

void Gamepad::buttonListen(listenerArgs *largs)
{
    Gamepad *gpad = largs->gpad;
    bool *threadEnd = largs->threadEnd;
    
    WORD prevButtons = 0, curButtons = 0;

    std::wstringstream wss(L"");

    while (!*threadEnd)
    {
        gpad->refresh();
        curButtons = gpad->state.Gamepad.wButtons;

        //wss << std::bitset<sizeof(WORD) * 8>(curButtons) << "\n";
        //OutputDebugString(wss.str().c_str());

        // send input for any depressed buttons
        // paramA is positive for down, negative for up
        for (const auto &button : XinputButtons)
            if ((curButtons & button.first) != (prevButtons & button.first))
            {
                gpad->ctrlProf->controlInput(button.second, curButtons & button.first);
            }

        prevButtons = curButtons;
    }
}