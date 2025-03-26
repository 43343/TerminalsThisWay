#include "parameterInputWindow.h"
#include "../Config/configManager.h"
#include <chrono>
#include <iostream>
#include <windowsx.h> 
#include <shellapi.h>
#include <shlobj.h>
#include <commdlg.h>

ParameterInputWindow* g_ParameterInputWindowInstance = nullptr;
auto firstPressTimeParameterInput = std::chrono::steady_clock::now();

ParameterInputWindow::ParameterInputWindow(Terminal* cmd) : hTerminal(*cmd)
{
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
	g_ParameterInputWindowInstance = this;
}

ParameterInputWindow::~ParameterInputWindow()
{
	if (hHook) {
		UnhookWindowsHookEx(hHook);
	}
}

void ParameterInputWindow::CreateInputWindow()
{
	isDialogOpen = true;
	POINT p;
	GetCursorPos(&p);
	const wchar_t CLASS_NAME[] = L"ParameterInputWindowClass";
	const wchar_t CLASS_NAME_INPUT_FIELD[] = L"ParameterInputFieldWindowClass";

	WNDCLASSW wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = NULL;

	WNDCLASSW wcif = {};
	wcif.lpfnWndProc = WindowInputFieldProc;
	wcif.hInstance = GetModuleHandle(NULL);
	wcif.lpszClassName = CLASS_NAME_INPUT_FIELD;
	wcif.hbrBackground = NULL;


	RegisterClassW(&wcif);
	RegisterClassW(&wc);

	int width = 300;
	int height = 20;
	RECT screenRect;
	GetWindowRect(GetDesktopWindow(), &screenRect);
	if (p.x + width > screenRect.right) p.x = screenRect.right - width;
	if (p.y + height > screenRect.bottom) p.y = screenRect.bottom - height;
	hwnd = CreateWindowExW(
		WS_EX_TOPMOST,
		CLASS_NAME,
		L"",
		WS_POPUP | WS_CLIPCHILDREN,
		p.x, p.y, width, height,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		this
	);

	if (!hwnd) {
		std::cerr << "Failed to create window: " << GetLastError() << std::endl;
		return;
	}


	hwndInputField = CreateWindowExW(
		WS_EX_COMPOSITED,
		CLASS_NAME_INPUT_FIELD,
		L"",
		WS_CHILD,
		0, 0, 200, height,
		hwnd,
		NULL,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		nullptr
	);

	if (!hwndInputField) {
		std::cerr << "Failed to create window: " << GetLastError() << std::endl;
		return;
	}

	input = new GUI::InputField(GetModuleHandle(NULL), 0, 0, 200, height, hwndInputField);
	input->setLimit(3276);
	const std::wstring& command = getSelectedText();
	input->setInput(command);
	btnAddPathFolder = new GUI::Button(GetModuleHandle(NULL), hwnd, 215, 0, 30, height);
	btnAddPathFolder->setSystemIcon(SIID_FOLDER);
	btnAddPathFolder->setCallback([&]() { SelectFolder(); });
	btnAddPathFile = new GUI::Button(GetModuleHandle(NULL), hwnd, 260, 0, 30, height);
	btnAddPathFile->setSystemIcon(SIID_DOCNOASSOC);
	btnAddPathFile->setCallback([&]() { SelectFile(); });

	ShowWindow(hwnd, SW_SHOW);
	ShowWindow(hwndInputField, SW_SHOW);
	UpdateWindow(hwndInputField);
	SetFocus(input->getHwnd());

}
void ParameterInputWindow::SelectFolder()
{
	const wchar_t CLASS_NAME[] = L"BrowserInfoWindowClass";
	DWORD startPos, endPos;
	SendMessage(input->getHwnd(), EM_GETSEL, (WPARAM)&startPos, (LPARAM)&endPos);
	CoInitialize(NULL);
	BROWSEINFOW bi = { 0 };
	std::wstring title = L"Select folder";
	std::wcout << "Выбранный путь: " << title << std::endl;
	printf("Выбранный путь:");
	bi.lpszTitle = title.c_str();
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	bi.hwndOwner = hwnd;
	bi.lpfn = BrowseSelectFolderCallbackProc;

	LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);


	std::wstring result;

	if (pidl != 0)
	{

		wchar_t path[MAX_PATH];
		if (SHGetPathFromIDListW(pidl, path))
		{
			result = std::wstring(path);

			std::wstring currentText = input->getInput();
			currentText.insert(startPos, result);
			input->setInput(currentText);

			DWORD newPos = startPos + result.length();
			SendMessage(input->getHwnd(), EM_SETSEL, newPos, newPos);
		}
	}
	CoTaskMemFree(pidl);
	UnregisterClassW(CLASS_NAME, GetModuleHandle(NULL));
	CoUninitialize();
	CoFreeUnusedLibraries();
	SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
	SetFocus(input->getHwnd());
	std::wcout << "Выбранный путь: " << result << std::endl;
}
void ParameterInputWindow::SelectFile()
{
	const wchar_t CLASS_NAME[] = L"BrowserInfoWindowClass";
	DWORD startPos, endPos;
	SendMessage(input->getHwnd(), EM_GETSEL, (WPARAM)&startPos, (LPARAM)&endPos);
	CoInitialize(NULL);

	OPENFILENAMEW ofn = { 0 };
	wchar_t szFile[MAX_PATH] = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"All Files\0*.*\0\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	std::wstring result;

	if (GetOpenFileNameW(&ofn))
	{
		result = std::wstring(ofn.lpstrFile);

		std::wstring currentText = input->getInput();
		currentText.insert(startPos, result);
		input->setInput(currentText);

		DWORD newPos = startPos + result.length();
		SendMessage(input->getHwnd(), EM_SETSEL, newPos, newPos);
	}
	UnregisterClassW(CLASS_NAME, GetModuleHandle(NULL));
	CoUninitialize();
	CoFreeUnusedLibraries();
	SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
	SetFocus(input->getHwnd());
	std::wcout << "Выбранный путь: " << result << std::endl;
}

int CALLBACK ParameterInputWindow::BrowseSelectFolderCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_SELCHANGED) {
		wchar_t path[MAX_PATH];
		if (SHGetPathFromIDListW((LPITEMIDLIST)lParam, path)) {
			SendMessage(hwnd, BFFM_ENABLEOK, 0, TRUE);
		}
		else {
			SendMessage(hwnd, BFFM_ENABLEOK, 0, FALSE);
		}
	}
	return 0;
}
LRESULT CALLBACK ParameterInputWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static POINT dragStartPoint;
	static bool isDragging = false;

	switch (uMsg) {
	case WM_ERASEBKGND:
		return 1; 

	case WM_LBUTTONDOWN: {
		POINT pt = {
			static_cast<short>(LOWORD(lParam)),
			static_cast<short>(HIWORD(lParam))
		};

		HWND hChild = ChildWindowFromPoint(hwnd, pt);

		if (!hChild || hChild == hwnd) {
			isDragging = true;
			dragStartPoint = pt;
			ClientToScreen(hwnd, &dragStartPoint);
			SetCapture(hwnd);
			return 0;
		}
		break;
	}

	case WM_MOUSEMOVE: {
		if (isDragging && (wParam & MK_LBUTTON)) {
			POINT pt = {
				static_cast<short>(LOWORD(lParam)),
				static_cast<short>(HIWORD(lParam))
			};
			ClientToScreen(hwnd, &pt);

			int dx = pt.x - dragStartPoint.x;
			int dy = pt.y - dragStartPoint.y;

			RECT rect;
			GetWindowRect(hwnd, &rect);
			MoveWindow(hwnd,
				rect.left + dx,
				rect.top + dy,
				rect.right - rect.left,
				rect.bottom - rect.top,
				TRUE);

			dragStartPoint = pt;
			return 0;
		}
		break;
	}

	case WM_LBUTTONUP: {
		if (isDragging) {
			isDragging = false;
			ReleaseCapture();
			return 0;
		}
		break;
	}

	case WM_SETCURSOR: {
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(hwnd, &pt);

		HWND hChild = ChildWindowFromPoint(hwnd, pt);
		if (hChild && hChild != hwnd) {
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return TRUE;
	}

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK ParameterInputWindow::WindowInputFieldProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_ERASEBKGND:
		return 1; 

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK ParameterInputWindow::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const int DOUBLE_PRESS_INTERVAL_MS = 500;
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
		if (Simulating::IsSimulating())
		{
			return CallNextHookEx(g_ParameterInputWindowInstance->hHook, nCode, wParam, lParam);
		}
		if (g_ParameterInputWindowInstance->isDialogOpen)
		{
			if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && pKeyboard->vkCode == VK_RETURN && IsWindowEnabled(g_ParameterInputWindowInstance->hwnd))
			{
				std::wstring command = g_ParameterInputWindowInstance->input->getInput();
				const wchar_t CLASS_NAME_INPUT_FIELD[] = L"ParameterInputFieldWindowClass";
				const wchar_t CLASS_NAME[] = L"ParameterInputWindowClass";
				if (!command.empty())
				{
					g_ParameterInputWindowInstance->hTerminal.sendCommandToCMD(command, true);   
				}
				DestroyWindow(g_ParameterInputWindowInstance->hwndInputField);
				DestroyWindow(g_ParameterInputWindowInstance->hwnd);
				UnregisterClassW(CLASS_NAME_INPUT_FIELD, GetModuleHandle(NULL));
				UnregisterClassW(CLASS_NAME, GetModuleHandle(NULL));
				SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
				g_ParameterInputWindowInstance->isDialogOpen = false;
			}
		}
		if (ConfigManager::getInstance().getConfig().keySendCommandParameter2 == 0)
		{
			if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommandParameter1)
			{
				if (!g_ParameterInputWindowInstance->isDialogOpen)
				{
					g_ParameterInputWindowInstance->CreateInputWindow();
				}
			}
		}
		else
		{
			if (ConfigManager::getInstance().getConfig().keySendCommandParameter1 == ConfigManager::getInstance().getConfig().keySendCommandParameter2)
			{
				if ((wParam == WM_KEYUP || wParam == WM_SYSKEYUP) && pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommandParameter1)
				{
					auto currentTime = std::chrono::steady_clock::now();
					if (!g_ParameterInputWindowInstance->firstPressDetected) {
						g_ParameterInputWindowInstance->firstPressDetected = true;
						firstPressTimeParameterInput = currentTime;
					}
					else {
						auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - firstPressTimeParameterInput).count();

						if (duration <= DOUBLE_PRESS_INTERVAL_MS) {
							if (!g_ParameterInputWindowInstance->isDialogOpen)
							{
								g_ParameterInputWindowInstance->CreateInputWindow();
							}
						}
						else {
							firstPressTimeParameterInput = currentTime; 
						}
					}
				}
			}
			else
			{
				if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommandParameter1) { 
						g_ParameterInputWindowInstance->keyA_pressed = true;
					}
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommandParameter2) {  
						g_ParameterInputWindowInstance->keyB_pressed = true;                
					}
				}
				else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommandParameter1) {
						g_ParameterInputWindowInstance->keyA_pressed = false;	
					}
					if (pKeyboard->vkCode == ConfigManager::getInstance().getConfig().keySendCommandParameter2) {
						g_ParameterInputWindowInstance->keyB_pressed = false;
					}
				}

				if (g_ParameterInputWindowInstance->keyA_pressed && g_ParameterInputWindowInstance->keyB_pressed) {
					std::cout << "gParameter1 " << g_ParameterInputWindowInstance->keyA_pressed << " gParameter2 " << g_ParameterInputWindowInstance->keyB_pressed;
					if (!g_ParameterInputWindowInstance->isDialogOpen)
					{
						g_ParameterInputWindowInstance->CreateInputWindow();;
					}
				}
			}
		}

	}

	return CallNextHookEx(g_ParameterInputWindowInstance->hHook, nCode, wParam, lParam);
}