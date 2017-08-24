#include "stdafx.h"
#include <string>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <sstream>

#include "ControlProfile.h"
#include "ControlCodes.h"

#define LOAD_ERROR L"lder"

ControlProfile::ControlProfile(HWND *controlBoxes)
{
	this->controlBoxes = controlBoxes;

	currentMouseMultiplier = 1;
	currentScrollMultiplier = 1;

	mouseSensitivity = 10;
	mouseSpeedMultiplier = 3;
}

/*
function:	saveProfile(wstring, wstring, bool)

purpose:	save the current control settings in an INI

params:		wstring roamingPath - path to the ~\appdata\roaming\ folder
			wstring profileName - the name (and filename) of the control profile
			bool showMessage - default set to TRUE, can otherwise set to show or not show messages

returns:	TRUE if successful, FALSE if not	
*/
bool ControlProfile::saveProfile(std::wstring roamingPath, std::wstring profileName, bool showMessage)
{
	std::wstring path = roamingPath + profileName + L".ini";


	int selectionIndex;
	for (int i = 0; i < CONTROL_COUNT; ++i)
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
	if (showMessage)
	{
		std::wstring successMessage = L"The profile " + profileName + L" has been saved.";
		::MessageBox(GetParent(controlBoxes[0]), successMessage.c_str(), _T("Profile Saved"), MB_OK);
	}
	return TRUE;
}

/*
function:	loadProfile(wstring, wstring, bool)

purpose:	load control settings from an INI

params:		wstring roamingPath - path to the ~\appdata\roaming\ folder
			wstring profileName - the name (and filename) of the control profile
			bool showMessage - default set to TRUE, can otherwise set to show or not show messages

returns:	TRUE if successful, FALSE if not
*/
bool ControlProfile::loadProfile(std::wstring roamingPath, std::wstring profileName, bool showMessage)
{
	std::wstring path = roamingPath + profileName + L".ini";

	// check if file exists
	if (!PathFileExists(path.c_str()))
	{
		if (showMessage)
			::MessageBox(GetParent(controlBoxes[0]), _T("Profile could not be found."), _T("Load Profile Error"), MB_OK);
		return FALSE;
	}

	for (int i = 0; i < CONTROL_COUNT; ++i)
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
	if (showMessage)
	{
		std::wstring successMessage = L"The profile " + profileName + L" has been loaded.";
		::MessageBox(GetParent(controlBoxes[0]), successMessage.c_str(), _T("Profile Loaded"), MB_OK);
	}
	mapControls(FALSE);
	return TRUE;
}

/*
function:	mapControls()

purpose:	map the current control settings by populating the controlMap

params:		bool showMessage - default set to TRUE, can otherwise set to show or not show messages

returns:	TRUE if successful, FALSE if not
*/
bool ControlProfile::mapControls(bool showMessage)
{
	int selectionIndex;
	for (int i = 0; i < CONTROL_COUNT; ++i)
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
	if (showMessage)
		::MessageBox(GetParent(controlBoxes[0]), L"Controls applied.", L"Success", MB_OK);

	return TRUE;
}

/*
function:	controlInput()

purpose:	translate the input and execute the appropriate command

params:		int controlCode - the code for what control was
			float paramA, paramB - generic parameters used for sending data,
									default to 0

returns:	TRUE if successful, FALSE if not
*/
void ControlProfile::controlInput(int controlCode, float paramA, float paramB)
{
	std::wstringstream wss(L"");

	wss << controlCode;

	// translate
	controlCode = controlMap[controlCode];

	if (controlCode != MOUSE)
	{
		wss << " " << controlCode << "\n";
		OutputDebugString(wss.str().c_str());
	}

	switch (controlCode)
	{
	case NO_CONTROL:
		break;
	case SPEED_UP_MOUSE:
		currentMouseMultiplier = 1 + paramA * (mouseSpeedMultiplier - 1);
		break;
	case SPEED_UP_SCROLL:
		currentScrollMultiplier = 1 + paramA * (scrollSpeedMultiplier - 1);
		break;
	case MOUSE:
		mouse_event(MOUSEEVENTF_MOVE, paramA * mouseSensitivity * currentMouseMultiplier, -paramB * mouseSensitivity * currentMouseMultiplier, 0, 0);
		break;
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
	default:
		break;
	}
}