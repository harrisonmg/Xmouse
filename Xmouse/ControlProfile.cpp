#include "stdafx.h"
#include <string>

#include "ControlProfile.h"
#include "ControlCodes.h"

ControlProfile::ControlProfile(HWND *controlBoxes)
{
	this->controlBoxes = controlBoxes;

	// set defaults for each control box
	for (int i = 0; i < CONTROL_COUNT; ++i)
		SendMessage(controlBoxes[i], CB_SETCURSEL, 0, 0);

	// save the current settings as the profile "Default"
	this->saveProfile(L"test");
}

void ControlProfile::saveProfile(std::wstring profileName)
{
	int selectionIndex;
	for (int i = 0; i < CONTROL_COUNT; ++i)
	{
		if ((selectionIndex = SendMessage(controlBoxes[i], CB_GETCURSEL, 0, 0)) == CB_ERR)
		{
			::MessageBox(GetParent(controlBoxes[0]), _T("Invalid Control selected"), _T("Error Saving Profile"), MB_OK);
			break;
		}
		else
		{
			wchar_t keyName[5], value[5];

			_itow(i, keyName, 10);
			_itow(selectionIndex, value, 10);

			std::wstring path = L"C:\\Users\\Harrison\\Desktop\\" + profileName + L".ini";

			WritePrivateProfileString(L"Default", keyName, value, path.c_str());
		}
	}
	std::wstring successMessage = L"The profile " + profileName + L" has been saved.";
	::MessageBox(GetParent(controlBoxes[0]), successMessage.c_str(), _T("Profile Saved"), MB_OK);
}

void ControlProfile::loadProfile(std::wstring profileName)
{
	int selectionIndex;
	for (int i = 0; i < CONTROL_COUNT; ++i)
	{
		wchar_t keyName[5], value[5];

		_itow(i, keyName, 10);
		_itow(selectionIndex, value, 10);
	}
}