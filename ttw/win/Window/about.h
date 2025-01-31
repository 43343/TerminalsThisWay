#pragma once
#include <windows.h>
#include "GUI/image.h"
#include "GUI/text.h"
#include "GUI/button.h"
#include "../updater.h"

class About
{
public:
	About(HINSTANCE hInstance);
	~About();
	bool createWindow();
	void setText();
	HWND getHwnd();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	HWND hwnd;
	HINSTANCE hInstance;
	GUI::Image* image;
	GUI::Text* text1;
	GUI::Button* btnOk;
	GUI::Button* btnCheckUpdate;
	Updater* updater;
};
