#include "modalWindow.h"
#include <iostream>

ModalWindow::ModalWindow()
{

}

ModalWindow::~ModalWindow()
{

}


bool ModalWindow::createWindow(const std::string& currentVersion, const std::string& latestVersion)
{
	const char CLASS_NAME[] = "UpdateWindowClass";

	bool output = false;

	HINSTANCE hInstance = GetModuleHandle(NULL);

	WNDCLASS wc = {};
	wc.lpfnWndProc = ModalWndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hIcon = LoadIcon(hInstance, "IDI_ICON1");

	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		"Update is available!",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 430, 250,
		nullptr,
		nullptr,
		hInstance,
		this
	);
	text = new GUI::Text(hInstance, hwnd, 45, 15, 300, 100);
	text->setStyle(false, true);
	text->setFontSize(10);
	text->setText("A new version TerminalThisWay is available!\n\nCurrent Version: " + currentVersion + "\nLatest Version: " + latestVersion + "\n\nDo you want to update?");
	btnYes = new GUI::Button(hInstance, hwnd, 100, 150, 140, 35);
	btnYes->setCallback([&]()
		{
			output = true;
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		});
	btnYes->setText("Да");
	btnNo = new GUI::Button(hInstance, hwnd, 260, 150, 140, 35);
	btnNo->setText("Нет"); 
	btnNo->setCallback([&]() {
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		});
	ShowWindow(hwnd, SW_SHOW);
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!IsDialogMessage(hwnd, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return output;
}

LRESULT CALLBACK ModalWindow::ModalWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;

	switch (uMsg)
	{
	case WM_COMMAND:
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		PostQuitMessage(0);
		break;
	case WM_DESTROY:
		DestroyWindow(hwnd);
		PostQuitMessage(0);
		break;
	case WM_CTLCOLORSTATIC:
		HDC hdcStatic = (HDC)wParam;
		SetBkMode(hdcStatic, TRANSPARENT);
		return (LRESULT)GetStockObject(NULL_BRUSH);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}