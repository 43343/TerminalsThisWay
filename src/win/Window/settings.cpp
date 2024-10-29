#include "settings.h"
#include <iostream>
#include "../Config/configManager.h"
#include "localization.h"

Settings::Settings(HINSTANCE hInstance) :
	hInstance(hInstance) {}
Settings::~Settings()
{
	delete settingsText;
	delete generalText;
	delete pathToTerminalInput;
	delete pathToTerminalText;
	delete localizationText;
	delete localizationDropdown;
	delete launchByDefaultText;
	delete runAsAdministratorText;
	delete keyAssignmentText;
	delete sendCommandText;
	delete sendCommandInput;
	delete sendCommandParameterText;
	delete sendCommandParameterInput;
	delete chooseFolderText;
	delete chooseFolderInput;
	delete warningText;
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
		0,
		CLASS_NAME,
		"Настройки",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 580, 380,
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
	pathToTerminalInput->setCallback([&]() {
		warningText->setVisible(true);
		//warningText->setColor("FF0000");
		});
	pathToTerminalText = new GUI::Text(hInstance, hwnd, 10, 60, 230, 20);

	localizationText = new GUI::Text(hInstance, hwnd, 10, 90, 230, 20);
	localizationDropdown = new GUI::Dropdown(hInstance, hwnd, 325, 90, 230, 100);
	localizationDropdown->addItem(L"English");
	localizationDropdown->addItem(L"Russian");


	launchByDefaultText = new GUI::Text(hInstance, hwnd, 10, 120, 230, 20);
	launchByDefaultToggle = new GUI::Toggle(hInstance, hwnd, 325, 120, 20, 20);

	runAsAdministratorText = new GUI::Text(hInstance, hwnd, 10, 150, 230, 20);
	runAsAdministratorToggle = new GUI::Toggle(hInstance, hwnd, 325, 150, 20, 20);

	keyAssignmentText = new GUI::Text(hInstance, hwnd, 10, 180, 230, 20);

	sendCommandText = new GUI::Text(hInstance, hwnd, 10, 210, 230, 20);
	sendCommandInput = new GUI::InputKeyBind(hInstance, hwnd, 325, 210, 230, 20);

	sendCommandParameterText = new GUI::Text(hInstance, hwnd, 10, 240, 320, 20);
	sendCommandParameterInput = new GUI::InputKeyBind(hInstance, hwnd, 325, 240, 230, 20);

	chooseFolderText = new GUI::Text(hInstance, hwnd, 10, 270, 230, 20);
	chooseFolderInput = new GUI::InputKeyBind(hInstance, hwnd, 325, 270, 230, 20);

	warningText = new GUI::Text(hInstance, hwnd, 10, 300, 500, 20);
	//warningText->setColor("FF0000");
	warningText->setVisible(false);

	saveBtn = new GUI::Button(hInstance, hwnd, 190, 320, 180, 20);
	saveBtn->setCallback([&]() {
		ShowWindow(hwnd, SW_HIDE);
		Config config;
		config.sendCommand = sendCommandInput->getText();
		config.sendCommandParameter = sendCommandParameterInput->getText();
		config.chooseFolder = chooseFolderInput->getText();
		config.language = localizationDropdown->getValue();
		config.launchByDefault = launchByDefaultToggle->getState();
		config.runAsAdministrator = runAsAdministratorToggle->getState();
		config.pathToTerminal = pathToTerminalInput->getInput();
		ConfigManager::getInstance().overwritingConfig("config.conf", config);
		setText();
		warningText->setVisible(false);
		});
	cancelBtn = new GUI::Button(hInstance, hwnd, 380, 320, 180, 20);
	cancelBtn->setCallback([&]() {
		ShowWindow(hwnd, SW_HIDE);
		warningText->setVisible(false);
		setInput();
		});
	setText();
	setInput();
	ShowWindow(hwnd, SW_SHOW);
	return true;
}

void Settings::setText() {
	Localization* local = new Localization(ConfigManager::getInstance().getConfig().language);
	settingsText->setText(local->getText("settings"));
	generalText->setText(local->getText("general"));
	pathToTerminalText->setText(local->getText("pathToTerminal"));
	localizationText->setText(local->getText("localization"));
	launchByDefaultText->setText(local->getText("launchByDefault"));
	runAsAdministratorText->setText(local->getText("runAsAdministrator"));
	keyAssignmentText->setText(local->getText("keyAssignment"));
	sendCommandText->setText(local->getText("sendCommand"));
	sendCommandParameterText->setText(local->getText("sendCommandParameter"));
	chooseFolderText->setText(local->getText("chooseFolder"));
	warningText->setText(local->getText("warning"));
	saveBtn->setText(local->getText("save"));
	cancelBtn->setText(local->getText("cancel"));
	InvalidateRect(hwnd, NULL, TRUE);
	UpdateWindow(hwnd);
	delete local;
}

void Settings::setInput() {
	pathToTerminalInput->setInput(ConfigManager::getInstance().getConfig().pathToTerminal);
	localizationDropdown->setValue(ConfigManager::getInstance().getConfig().language);
	launchByDefaultToggle->setState(ConfigManager::getInstance().getConfig().launchByDefault);
	runAsAdministratorToggle->setState(ConfigManager::getInstance().getConfig().runAsAdministrator);
	sendCommandInput->setText(ConfigManager::getInstance().getConfig().sendCommand);
	sendCommandParameterInput->setText(ConfigManager::getInstance().getConfig().sendCommandParameter);
	chooseFolderInput->setText(ConfigManager::getInstance().getConfig().chooseFolder);
	InvalidateRect(hwnd, NULL, TRUE);
	UpdateWindow(hwnd);
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
		break;
	case WM_NCDESTROY:
		if (pThis) {
			delete pThis;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
		}
		break;
	case WM_CTLCOLORSTATIC:
		HDC hdcStatic = (HDC)wParam;
		SetBkMode(hdcStatic, TRANSPARENT);
		return (LRESULT)GetStockObject(NULL_BRUSH);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}