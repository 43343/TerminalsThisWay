#pragma once
#include <windows.h>

namespace GUI {
    class Toggle {
    public:
        Toggle(HINSTANCE hInstance, HWND parentHwnd, int x, int y, int width, int height);
        bool getState() const;

        void setState(bool state);

    private:
        HINSTANCE hInstance;
        HWND parentHwnd;
        HWND hwnd;
    };
}
