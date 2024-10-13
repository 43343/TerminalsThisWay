#pragma once
#include <windows.h>
#include <string>

namespace GUI {
    class Text {
    public:
        Text(HINSTANCE hInstance, HWND parentHwnd, int x, int y, int width, int height);

        HWND getHwnd() const;

        void setAllign(DWORD alignStyle);
        void setVisible(bool visible);

        void setColor(const std::string& hex);
        void setText(const std::string& text);
    private:
        HINSTANCE hInstance;
        HWND parentHwnd;
        HWND hwnd;
    };
}