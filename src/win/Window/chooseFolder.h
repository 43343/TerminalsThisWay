#pragma once
#include <windows.h>
#include "../terminal.h"
#include <chrono>

class ChooseFolder
{
public:
	ChooseFolder(Terminal* cmd);
	~ChooseFolder();
private:
	HHOOK hHook;
	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); 
	std::string SelectFolder();
	bool isDialogOpen = false;
	bool keyA_pressed = false;
	bool keyB_pressed = false;
	bool firstPressDetected = false;
	Terminal& hTerminal;
};