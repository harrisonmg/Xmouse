#include "stdafx.h"
#include <string>
#include <ShlObj.h>
#include <Shlwapi.h>

#include "ControlProfile.h"
#include "ControlCodes.h"

#define LOAD_ERROR L"load error"

ControlProfile::ControlProfile(HWND *controlBoxes)
{
	this->controlBoxes = controlBoxes;
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
			controlMap[i] = selectionIndex;
		}
	}
	if (showMessage)
		::MessageBox(GetParent(controlBoxes[0]), L"Controls applied.", L"Success", MB_OK);

	return TRUE;
}