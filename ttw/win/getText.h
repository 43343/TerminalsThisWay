#pragma once
#include <windows.h>
#include "simulating.h"
#include <chrono>
#include <string>


class GetText : protected Simulating
{
public:
	explicit GetText();
protected:
	std::wstring getSelectedText();
private:
	void SimulateCtrlC();
	std::wstring GetClipboardText();
	void SetClipboardText(const std::wstring& text);
};
