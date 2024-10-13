#include "dropdown.h"

namespace GUI {
    Dropdown::Dropdown(HINSTANCE hInstance, HWND parentHwnd, int x, int y, int width, int height)
        : hInstance(hInstance), parentHwnd(parentHwnd), hwnd(nullptr) {
        hwnd = CreateWindowExW(
            0,
            L"COMBOBOX",
            nullptr,
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
            x, y, width, height,
            parentHwnd,
            nullptr,
            hInstance,
            nullptr
        );

        // Установка шрифта
        HFONT hFont = CreateFontW(
            16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
        );
        SendMessage(hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);
    }

    void Dropdown::addItem(const std::wstring& item) {
        SendMessageW(hwnd, CB_ADDSTRING, 0, (LPARAM)item.c_str());
    }

    void Dropdown::setValue(std::string item) {
        int count = static_cast<int>(SendMessageW(hwnd, CB_GETCOUNT, 0, 0));
        for (int i = 0; i < count; ++i) {
            int length = static_cast<int>(SendMessageW(hwnd, CB_GETLBTEXTLEN, i, 0));
            if (length != CB_ERR) {
                std::wstring wtext(length + 1, L'\0');
                SendMessageW(hwnd, CB_GETLBTEXT, i, reinterpret_cast<LPARAM>(&wtext[0]));
                wtext.resize(length);

                std::string text(wtext.begin(), wtext.end());

                if (text == item) {
                    SendMessageW(hwnd, CB_SETCURSEL, i, 0);
                    break;
                }
            }
        }
    }

    std::string Dropdown::getValue() const {
        int index = static_cast<int>(SendMessageW(hwnd, CB_GETCURSEL, 0, 0));
        if (index != CB_ERR) {
            int length = static_cast<int>(SendMessageW(hwnd, CB_GETLBTEXTLEN, index, 0));
            if (length != CB_ERR) {
                std::wstring wtext(length + 1, L'\0');
                SendMessageW(hwnd, CB_GETLBTEXT, index, reinterpret_cast<LPARAM>(&wtext[0]));
                wtext.resize(length);
                return std::string(wtext.begin(), wtext.end());
            }
        }
        return "";
    }

    HWND Dropdown::getHwnd() const {
        return hwnd;
    }
}