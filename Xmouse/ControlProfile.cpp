#include "stdafx.h"
#include <string>
#include <ShlObj.h>

#include "ControlProfile.h"
#include "ControlCodes.h"

#define LOAD_ERROR L"-1"

ControlProfile::ControlProfile(HWND *controlBoxes)
{
	this->controlBoxes = controlBoxes;
}

void ControlProfile::saveProfile(std::wstring roamingPath, std::wstring profileName, bool showMessage)
{
	std::wstring path = roamingPath + profileName + L".ini";

	int selectionIndex;
	for (int i = 0; i < CONTROL_COUNT; ++i)
	{
		if ((selectionIndex = SendMessage(controlBoxes[i], CB_GETCURSEL, 0, 0)) == CB_ERR)
		{
			::MessageBox(GetParent(controlBoxes[0]), _T("Invalid control selected"), _T("Error Saving Profile"), MB_OK);
			showMessage = FALSE;
			break;
		}
		else
		{
			wchar_t keyName[5], value[5];

			_itow(i, keyName, 10);
			_itow(selectionIndex, value, 10);

			WritePrivateProfileString(L"Default", keyName, value, path.c_str());
		}
	}
	if (showMessage)
	{
		std::wstring successMessage = L"The profile " + profileName + L" has been saved.";
		::MessageBox(GetParent(controlBoxes[0]), successMessage.c_str(), _T("Profile Saved"), MB_OK);
	}
}

void ControlProfile::loadProfile(std::wstring roamingPath, std::wstring profileName, bool showMessage)
{
	std::wstring path = roamingPath + profileName + L".ini";

	for (int i = 0; i < CONTROL_COUNT; ++i)
	{
		wchar_t keyName[5], value[5];

		_itow(i, keyName, 10);

		GetPrivateProfileString(L"Default", keyName, LOAD_ERROR, value, 5, path.c_str());

		int selectionIndex = _wtoi(value);

		if (selectionIndex == -1)
		{
			::MessageBox(GetParent(controlBoxes[0]), _T("Profile could not be found or is incomplete."), _T("Load Profile Error"), MB_OK);
			showMessage = FALSE;
			break;
		}
		else if (SendMessage(controlBoxes[i], CB_SETCURSEL, selectionIndex, 0) == CB_ERR)
		{
				::MessageBox(GetParent(controlBoxes[0]), _T("Profile contains and invalid value. \"Nothing\" will be substituted."), _T("Load Profile Warning"), MB_OK);
		}
	}
	if (showMessage)
	{
		std::wstring successMessage = L"The profile " + profileName + L" has been loaded.";
		::MessageBox(GetParent(controlBoxes[0]), successMessage.c_str(), _T("Profile Loaded"), MB_OK);
	}
}