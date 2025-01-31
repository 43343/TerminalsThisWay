#pragma once
#include <windows.h>
#include "../terminal.h"
#include "../simulating.h"
#include <chrono>

class ChooseFolder : private Simulating
{
public:
	ChooseFolder(Terminal* cmd);
	~ChooseFolder();
private:
	HHOOK hHook;
	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
	std::wstring SelectFolder();
	bool isDialogOpen = false;
	bool keyA_pressed = false;
	bool keyB_pressed = false;
	bool firstPressDetected = false;
	Terminal& hTerminal;
};
