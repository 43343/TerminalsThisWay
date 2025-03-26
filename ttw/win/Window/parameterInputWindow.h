#pragma once
#include <windows.h>
#include "../terminal.h"
#include "GUI/inputField.h"
#include "GUI/button.h"
#include "../getText.h"
#include <string>

class ParameterInputWindow : private GetText
{
public:
	ParameterInputWindow(Terminal* cmd, HINSTANCE instance);
	~ParameterInputWindow();
private:
	HHOOK hHook;
	HWND hwnd;
	HWND hwndInputField;
	HINSTANCE hInstance;
	Terminal& hTerminal;
	GUI::InputField* input;
	GUI::Button* btnAddPathFolder;
	GUI::Button* btnAddPathFile;
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WindowInputFieldProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	bool isDialogOpen = false;
	bool keyA_pressed = false;
	bool keyB_pressed = false;
	bool firstPressDetected = false;
	void CreateInputWindow();
	WNDPROC originalWndProc;
};
