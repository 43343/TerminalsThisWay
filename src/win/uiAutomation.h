#pragma once
#include <UIAutomation.h>
#include <windows.h>
#include "terminal.h"
#include <chrono>

#pragma comment(lib, "Ole32.lib")

class UIAutomation
{
public:
	explicit UIAutomation(Terminal* cmd);
	UIAutomation();
	~UIAutomation();
	std::string getSelectedText();
private:
	HHOOK hHook;
	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	//UIAutomation* g_UIAutomationInstance = nullptr;
	Terminal* hTerminal;
	bool keyA_pressed = false;
	bool keyB_pressed = false;
	bool firstPressDetected = false;
};