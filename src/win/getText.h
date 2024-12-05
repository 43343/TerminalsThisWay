#pragma once
#include <windows.h>
#include "terminal.h"
#include <chrono>


class GetText
{
public:
	explicit GetText(Terminal* cmd);
	~GetText();
	std::wstring getSelectedText();
private:
	HHOOK hHook;
	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	Terminal* hTerminal;
	bool keyA_pressed = false;
	bool keyB_pressed = false;
	bool firstPressDetected = false;
	std::wstring GetClipboardText();
	void SetClipboardText(const std::wstring& text);
	void SimulateCtrlC();
};