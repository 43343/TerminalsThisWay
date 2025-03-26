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
	ParameterInputWindow(Terminal* cmd);
	~ParameterInputWindow();
private:
	HHOOK hHook;
	HWND hwnd;
	HWND hwndInputField;
	Terminal& hTerminal;
	GUI::InputField* input;
	GUI::Button* btnAddPathFolder;
	GUI::Button* btnAddPathFile;
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WindowInputFieldProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	static int CALLBACK BrowseSelectFolderCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
	bool isDialogOpen = false;
	bool keyA_pressed = false;
	bool keyB_pressed = false;
	bool firstPressDetected = false;
	void CreateInputWindow();
	void SelectFolder();
	void SelectFile();
	WNDPROC originalWndProc;
};
