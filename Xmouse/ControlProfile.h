#ifndef CONTROL_PROFILE_H
#define CONTROL_PROFILE_H

#include "stdafx.h"
#include <string>

class ControlProfile
{
private:

	HWND *controlBoxes;
	int controlCount;

public:

	ControlProfile(HWND*);

	// save controls to ini file
	void saveProfile(std::wstring);
	// load controls from ini file
	void loadProfile(std::wstring);
};

#endif