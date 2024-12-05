#include "about.h"
#include "../../version.h"

About::About(HINSTANCE hInstance) : hInstance(hInstance)
{

}

About::~About()
{

}

bool About::createWindow()
{

	const char CLASS_NAME[] = "AboutWindowClass";

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(NULL), "IDI_ICON1");

	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		"About",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 430, 145,
		nullptr,
		nullptr,
		hInstance,
		this
	);

	if (hwnd == NULL) {
		return false;
	}

	image = new GUI::Image(hInstance, hwnd, 10, 15, 230, 20, "IDI_ICON1");
	text1 = new GUI::Text(hInstance, hwnd, 45, 15, 200, 40);
	text1->setStyle(false, true);
	text1->setFontSize(10);
	btnOk = new GUI::Button(hInstance, hwnd, 260, 15, 140, 35);
	btnOk->setCallback([&]() {
		DestroyWindow(hwnd);
		});
	btnCheckUpdate = new GUI::Button(hInstance, hwnd, 260, 60, 140, 35);
	btnCheckUpdate->setFontSize(8);
	setText();
	updater = new Updater();
	btnCheckUpdate->setCallback([&]() {
		updater->checkUpdate();
		});
	ShowWindow(hwnd, SW_SHOW);
	return true;
}

void About::setText()
{
	text1->setText("TerminalsThisWay  " + currentVersion + " Alpha\n © 2024 43343");
	btnOk->setText("OK");
	btnCheckUpdate->setText("Check for updates");
}

HWND About::getHwnd()
{
	return hwnd;
}

LRESULT CALLBACK About::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	About* pThis = reinterpret_cast<About*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
	static std::wstring inputText;

	switch (uMsg)
	{
	case WM_CREATE:
		pThis = (About*)pCreate->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
		pThis->hwnd = hWnd;
		break;
	case WM_DESTROY:
		DestroyWindow(hWnd);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
	case WM_CTLCOLORSTATIC:
		HDC hdcStatic = (HDC)wParam;
		SetBkMode(hdcStatic, TRANSPARENT);
		return (LRESULT)GetStockObject(NULL_BRUSH);
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}