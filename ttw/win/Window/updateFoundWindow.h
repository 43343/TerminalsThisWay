#pragma once
#include <windows.h>
#include <string>
#include "GUI/button.h"
#include "GUI/text.h"
#include "GUI/image.h"

class UpdateFoundWindow
{
public:
	UpdateFoundWindow();
	~UpdateFoundWindow();
	bool createWindow(const std::string& currentVersion, const std::string& latestVersion);
private:
	static LRESULT CALLBACK UpdateFoundWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	HWND hwnd;
	GUI::Button* btnYes;
	GUI::Button* btnNo;
	GUI::Text* text;
	GUI::Image* image;
};
