#pragma once
#include <windows.h>
#include <string>

namespace GUI {
    class Dropdown {
    public:
        Dropdown(HINSTANCE hInstance, HWND parentHwnd, int x, int y, int width, int height);

        void addItem(const std::wstring& item);

        void setValue(std::string item);
        std::string getValue() const;


        HWND getHwnd() const;

    private:
        HINSTANCE hInstance;
        HWND parentHwnd;
        HWND hwnd;
    };
}