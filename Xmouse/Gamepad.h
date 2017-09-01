#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "stdafx.h"
#include <Xinput.h>

#include "ControlProfile.h"

class Gamepad
{
private:

	HWND mainWnd;
	ControlProfile *ctrlProf;
	static std::map<WORD, int> XinputButtons;

	std::wstring iniPath;

	XINPUT_STATE state;
	int controllerId;

	float deadzoneLX, deadzoneLY, deadzoneRX, deadzoneRY;

public:

	Gamepad(HWND, ControlProfile*, std::wstring);
	
	void checkConnection();
	static void stickListen(Gamepad*);
	static void buttonListen(Gamepad*);
	void setDeadzones();
	int getControllerId();
	bool refresh();
};

#endif