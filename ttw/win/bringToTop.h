#pragma once
#include <windows.h>
#include "terminal.h"
#include "simulating.h"

class BringToTop : private Simulating
{
public:
	BringToTop(Terminal* cmd);
	~BringToTop();
private:
	HHOOK hHook;
	Terminal& hTerminal;
	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	bool keyA_pressed = false;
	bool keyB_pressed = false;
	bool firstPressDetected = false;
};
