#pragma once
#include <windows.h>
#include <string>
#include <functional>

namespace GUI {
    class Button {
    public:
        Button(HINSTANCE hInstance, HWND parentHwnd, int x, int y, int width, int height);

        HWND getHwnd() const;

        void setCallback(std::function<void()> cb);
        void setText(const std::string& text);
        void setFontSize(int fontSize);
    private:
        HINSTANCE hInstance;
        HWND parentHwnd;
        HWND hwnd;
        HFONT hFont;
        std::function<void()> callback;
        WNDPROC originalProc;
        static LRESULT CALLBACK ButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };
}
