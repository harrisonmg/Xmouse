#ifndef CONTROL_PROFILE_H
#define CONTROL_PROFILE_H

#include "stdafx.h"
#include <string>
#include <map>
#include <functional>

class ControlProfile
{
private:

	HWND *controlBoxes;

	std::map<int, int> controlMap;

	float leftTrigger, rightTrigger;
	float mouseSpeed, mouseMod, currentMouseMod;
	float scrollSpeed, scrollMod, currentScrollMod;

public:

	ControlProfile(HWND*);

	// save controls to ini file
	bool saveProfile(std::wstring, std::wstring, bool showMessage = TRUE);
	// load controls from ini file
	bool loadProfile(std::wstring, std::wstring, std::function<void()>, bool showMessage = TRUE);
	// map controls (populate controlMap)
	bool mapControls(bool showMessage = TRUE);
	// translate control and execute command
	void controlInput(int, float paramA = 0, float paramB = 0);
};

#endif