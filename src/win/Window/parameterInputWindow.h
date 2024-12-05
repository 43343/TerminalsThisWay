#pragma once
#include <windows.h>
#include "../terminal.h"
#include "GUI//inputField.h"
#include "../getText.h"

class ParameterInputWindow
{
public:
	ParameterInputWindow(Terminal* cmd, HINSTANCE instance, GetText& getText);
	~ParameterInputWindow();
private:
	HHOOK hHook;
	HWND hwnd;
	HINSTANCE hInstance;
	GetText& hGetText;
	Terminal& hTerminal;
	GUI::InputField* input;
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	bool isDialogOpen = false;
	bool keyA_pressed = false;
	bool keyB_pressed = false;
	bool firstPressDetected = false;
	void CreateInputWindow();
	WNDPROC originalWndProc;
};