#include "button.h"
#include <iostream>

namespace GUI {
    Button::Button(HINSTANCE hInstance, HWND parentHwnd, int x, int y, int width, int height)
        : hInstance(hInstance), parentHwnd(parentHwnd), hwnd(nullptr) {

        hwnd = CreateWindowEx(
            0,
            "BUTTON",
            "",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            x, y, width, height,
            parentHwnd,
            nullptr,
            (HINSTANCE)GetWindowLongPtr(parentHwnd, GWLP_HINSTANCE),
            nullptr
        );
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        originalProc = reinterpret_cast<WNDPROC>(
            SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(ButtonProc)));
    }

    HWND Button::getHwnd() const {
        return hwnd;
    }
    void Button::setCallback(std::function<void()> cb)
    {
        callback = cb;

    }
    void Button::setText(const std::string& text) {
        SetWindowText(hwnd, text.c_str());
    }
    LRESULT CALLBACK Button::ButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        Button* btn = reinterpret_cast<Button*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (btn) {
            if (msg == WM_LBUTTONUP) {
                std::cout << "test";
                if (btn && btn->callback) {
                    btn->callback();
                }
            }
            return CallWindowProc(btn->originalProc, hwnd, msg, wParam, lParam);
        }
        else return CallWindowProc(nullptr, nullptr, msg, wParam, lParam);
    }
}