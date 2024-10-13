#include "inputField.h"
#include <iostream>

namespace GUI {
    InputField::InputField(HINSTANCE hInstance, int x, int y, int width, int height, HWND parentHwnd)
        : hInstance(hInstance), parentHwnd(parentHwnd), hwnd(nullptr) {
        hwnd = CreateWindowEx(
            0,
            "EDIT",
            "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
            x, y, width, height,
            parentHwnd,
            nullptr,
            hInstance,
            nullptr
        );
        SendMessage(hwnd, EM_LIMITTEXT, 0, 0);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        originalWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);
    }
    std::string InputField::getInput() const {
        // Get the length of the text in the edit control
        int length = GetWindowTextLength(hwnd);
        if (length == 0) {
            return ""; // Return an empty string if there's no text
        }

        // Create a buffer to hold the text
        char* buffer = new char[length + 1]; // +1 for null-terminator

        // Get the text from the edit control
        GetWindowText(hwnd, buffer, length + 1);

        // Convert the buffer to a std::string
        std::string text(buffer);

        // Clean up the buffer
        delete[] buffer;

        return text;
    }
    void InputField::setCallback(std::function<void()> cb)
    {
        callback = cb;

    }

    void InputField::setInput(const std::string& text) {
        // Set the text in the edit control
        SetWindowText(hwnd, text.c_str());
    }

    HWND InputField::getHwnd() const {
        return hwnd;
    }

    LRESULT CALLBACK InputField::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        InputField* inputField = reinterpret_cast<InputField*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (uMsg == WM_KEYUP) {
            if(inputField->callback)
                inputField->callback();
        }
        return CallWindowProc(inputField->originalWndProc,hwnd, uMsg, wParam, lParam);
    }
}