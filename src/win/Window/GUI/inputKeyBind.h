#pragma once
#include <windows.h>
#include <string>
#include <sstream>

namespace GUI {
	class InputKeyBind {
	public:
		InputKeyBind(HINSTANCE hInstance, HWND parentHwnd, int x, int y, int width, int height);
		//bool isActive = false;
		void HandleParentClick(POINT pt);
		void setText(std::wstring text);
		std::wstring getText();
		
	private:
		HWND hwnd;
		HWND parentHwnd;
		std::wstring keySequence;
		void GetEnglishKeyNameText(WPARAM wParam, LPARAM lParam, wchar_t* keyName, int keyNameSize);

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		WNDPROC originalWndProc;
	};
}