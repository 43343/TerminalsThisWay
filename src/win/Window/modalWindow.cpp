#include "modalWindow.h"

ModalWindow::ModalWindow(HINSTANCE hInstance) : hInstance(hInstance)
{

}

ModalWindow::~ModalWindow()
{

}


void ModalWindow::createWindow(const std::string& currentVersion, const std::string& latestVersion, std::function<void()> cb)
{
	const char CLASS_NAME[] = "UpdateWindowClass";

	WNDCLASS wc = {};
	wc.lpfnWndProc = ModalWndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(NULL), "IDI_ICON1");

	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		"Доступно обновление",
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
	btnYes->setText("Да");
	btnYes->setCallback(cb);
	btnNo = new GUI::Button(hInstance, hwnd, 260, 150, 140, 35);
	btnNo->setText("Нет"); 
	btnNo->setCallback([&]() {
		ShowWindow(hwnd, SW_HIDE);
		});
	ShowWindow(hwnd, SW_SHOW);
}

LRESULT CALLBACK ModalWindow::ModalWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;

	switch (uMsg)
	{
	case WM_CTLCOLORSTATIC:
		HDC hdcStatic = (HDC)wParam;
		SetBkMode(hdcStatic, TRANSPARENT);
		return (LRESULT)GetStockObject(NULL_BRUSH);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}