#include "settings.h"
#include <iostream>
#include "../Utility/utility.h"
#include "../Config/configManager.h"

Settings::Settings(HINSTANCE hInstance) :
	hInstance(hInstance) {
}
Settings::~Settings()
{
	delete settingsText;
	delete generalText;
	delete pathToTerminalInput;
	delete pathToTerminalText;
	delete launchByDefaultText;
	delete runAsAdministratorText;
	delete keyAssignmentText;
	delete sendCommandText;
	delete sendCommandInput;
	delete sendCommandParameterText;
	delete sendCommandParameterInput;
	delete chooseFolderText;
	delete chooseFolderInput;
	delete saveBtn;
	delete cancelBtn;
}

bool Settings::createWindow() {

	const char CLASS_NAME[] = "SettingsWindowClass";

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(NULL), "IDI_ICON1");

	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		WS_EX_COMPOSITED,
		CLASS_NAME,
		"Settings",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 580, 350,
		nullptr,
		nullptr,
		hInstance,
		this
	);

	if (hwnd == nullptr) {
		return false;
	}

	settingsText = new GUI::Text(hInstance, hwnd, 135, 10, 260, 20);
	settingsText->setAllign(SS_CENTER);

	generalText = new GUI::Text(hInstance, hwnd, 10, 30, 260, 20);

	pathToTerminalInput = new GUI::InputField(hInstance, 325, 60, 230, 20, hwnd);
	pathToTerminalText = new GUI::Text(hInstance, hwnd, 10, 60, 230, 20);


	launchByDefaultText = new GUI::Text(hInstance, hwnd, 10, 90, 230, 20);
	launchByDefaultToggle = new GUI::Toggle(hInstance, hwnd, 325, 90, 20, 20);

	runAsAdministratorText = new GUI::Text(hInstance, hwnd, 10, 120, 230, 20);
	runAsAdministratorToggle = new GUI::Toggle(hInstance, hwnd, 325, 120, 20, 20);

	keyAssignmentText = new GUI::Text(hInstance, hwnd, 10, 150, 230, 20);

	sendCommandText = new GUI::Text(hInstance, hwnd, 10, 180, 230, 20);
	sendCommandInput = new GUI::InputKeyBind(hInstance, hwnd, 325, 180, 230, 20);

	sendCommandParameterText = new GUI::Text(hInstance, hwnd, 10, 210, 320, 20);
	sendCommandParameterInput = new GUI::InputKeyBind(hInstance, hwnd, 325, 210, 230, 20);

	chooseFolderText = new GUI::Text(hInstance, hwnd, 10, 240, 230, 20);
	chooseFolderInput = new GUI::InputKeyBind(hInstance, hwnd, 325, 240, 230, 20);

	saveBtn = new GUI::Button(hInstance, hwnd, 190, 290, 180, 20);
	saveBtn->setCallback([&]() {
		Config config;
		config.sendCommand = sendCommandInput->getText();
		config.sendCommandParameter = sendCommandParameterInput->getText();
		config.chooseFolder = chooseFolderInput->getText();
		config.launchByDefault = launchByDefaultToggle->getState();
		config.runAsAdministrator = runAsAdministratorToggle->getState();
		config.pathToTerminal = pathToTerminalInput->getInput();
		ConfigManager::getInstance().overwritingConfig(getAppdataFolder() + L"\\config.conf", config);
		DestroyWindow(hwnd);
		UnregisterClass(CLASS_NAME, GetModuleHandle(NULL));
		});
	cancelBtn = new GUI::Button(hInstance, hwnd, 380, 290, 180, 20);
	cancelBtn->setCallback([&]() {
		DestroyWindow(hwnd);
		UnregisterClass(CLASS_NAME, GetModuleHandle(NULL));
		});
	setText();
	setInput();
	ShowWindow(hwnd, SW_SHOW);
	return true;
}

void Settings::setText() {
	settingsText->setText("Settings");
	generalText->setText("General");
	pathToTerminalText->setText("Path to terminal:");
	launchByDefaultText->setText("Launch by default:");
	runAsAdministratorText->setText("Run as administrator:");
	keyAssignmentText->setText("Key assignment");
	sendCommandText->setText("Send command:");
	sendCommandParameterText->setText("Send command (Change parameter):");
	chooseFolderText->setText("Choose folder:");
	saveBtn->setText("Save");
	cancelBtn->setText("Cancel");
	InvalidateRect(hwnd, NULL, TRUE);
	UpdateWindow(hwnd);
}

void Settings::setInput() {
	pathToTerminalInput->setInput(ConfigManager::getInstance().getConfig().pathToTerminal);
	launchByDefaultToggle->setState(ConfigManager::getInstance().getConfig().launchByDefault);
	runAsAdministratorToggle->setState(ConfigManager::getInstance().getConfig().runAsAdministrator);
	sendCommandInput->setText(ConfigManager::getInstance().getConfig().sendCommand);
	sendCommandParameterInput->setText(ConfigManager::getInstance().getConfig().sendCommandParameter);
	chooseFolderInput->setText(ConfigManager::getInstance().getConfig().chooseFolder);
	InvalidateRect(hwnd, NULL, TRUE);
	UpdateWindow(hwnd);
}
HWND Settings::getHwnd()
{
	return hwnd;
}

LRESULT CALLBACK Settings::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	Settings* pThis = reinterpret_cast<Settings*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
	static std::wstring inputText;

	switch (uMsg)
	{
	case WM_CREATE:
		pThis = (Settings*)pCreate->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
		pThis->hwnd = hwnd;
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		UnregisterClass("SettingsWindowClass", GetModuleHandle(NULL));
		SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
		return 0;
	case WM_SIZE:
		if (wParam != SIZE_RESTORED) {
			return 0;
		}
		break;
	case WM_LBUTTONDOWN:
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		pThis->sendCommandInput->HandleParentClick(pt);
		pThis->sendCommandParameterInput->HandleParentClick(pt);
		pThis->chooseFolderInput->HandleParentClick(pt);
		SetFocus(NULL);
		break;
	case WM_DESTROY:
		DestroyWindow(hwnd);
		UnregisterClass("SettingsWindowClass", GetModuleHandle(NULL));
		SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
		break;
	case WM_NCDESTROY:
		if (pThis) {
			delete pThis;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
		}
		break;
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		SetBkMode(hdcStatic, TRANSPARENT);
		return (LRESULT)GetStockObject(NULL_BRUSH);
	}
	case WM_ERASEBKGND:
		return 1;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		HDC hdcMem = CreateCompatibleDC(hdc);
		RECT rc;
		GetClientRect(hwnd, &rc);
		HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

		FillRect(hdcMem, &rc, (HBRUSH)(COLOR_WINDOW + 1));

		EnumChildWindows(hwnd, [](HWND hChild, LPARAM lParam)->BOOL __stdcall {
			HDC hdcMem = (HDC)lParam;

			RECT rcChild;
			GetWindowRect(hChild, &rcChild);

			POINT pt = { rcChild.left, rcChild.top };
			ScreenToClient(GetParent(hChild), &pt);

			SetWindowOrgEx(hdcMem, -pt.x, -pt.y, NULL);
			SendMessage(hChild, WM_PRINT, (WPARAM)hdcMem,
				PRF_CLIENT | PRF_NONCLIENT | PRF_CHILDREN);
			SetWindowOrgEx(hdcMem, 0, 0, NULL);

			return TRUE;
		}, (LPARAM)hdcMem);

		BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, hbmOld);
		DeleteObject(hbmMem);
		DeleteDC(hdcMem);
		EndPaint(hwnd, &ps);
		return 0;
	}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}