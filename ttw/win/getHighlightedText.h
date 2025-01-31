#pragma once
#include "getText.h"
#include "terminal.h"
#include <string>
class GetHighlitedText : private GetText
{
public:
	explicit GetHighlitedText(Terminal* cmd);
	~GetHighlitedText();
private:
	HHOOK hHook;
	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	Terminal* hTerminal;
	bool keyA_pressed = false;
	bool keyB_pressed = false;
	bool firstPressDetected = false; 
};
