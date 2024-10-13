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
		void setText(std::string text);
		std::string getText();
		
	private:
		HWND hwnd;
		HWND parentHwnd;
		std::string keySequence;
		void RemoveSpaces(char* str);
		void GetEnglishKeyNameText(WPARAM wParam, char* keyName, int keyNameSize);

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		WNDPROC originalWndProc;
	};
}