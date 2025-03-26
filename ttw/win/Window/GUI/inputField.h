#pragma once
#include <windows.h>
#include <string>
#include <functional>

namespace GUI {
    class InputField {
    public:
        InputField(HINSTANCE hInstance, int x, int y, int width, int height, HWND parentHwnd = nullptr);
        std::wstring getInput() const;
        void setInput(const std::wstring& text);
        void setCallback(std::function<void()> cb);
        void setLimit(int limit);

        HWND getHwnd() const;
    private:
        std::function<void()> callback;
        HINSTANCE hInstance;
        HWND parentHwnd;
        HWND hwnd;

        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        WNDPROC originalWndProc;
    };
}
