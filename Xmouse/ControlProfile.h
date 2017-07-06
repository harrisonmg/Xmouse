#ifndef CONTROL_PROFILE_H
#define CONTROL_PROFILE_H

#include "stdafx.h"
#include <string>
#include <map>

class ControlProfile
{
private:

	HWND *controlBoxes;
	std::map<int, int> controlMap;

public:

	ControlProfile(HWND*);
	
	// save controls to ini file
	void saveProfile(std::wstring, std::wstring, bool showMessage = TRUE);
	// load controls from ini file
	void loadProfile(std::wstring, std::wstring, bool showMessage = TRUE);
	// map controls (populate controlMap)
	void mapControls();
};

#endif