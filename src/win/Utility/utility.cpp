#include"utility.h"

DWORD stringTokey(const std::wstring& key) {
	// Используем static для инициализации карты только один раз
	static const std::unordered_map<std::wstring, DWORD> keyMap = {
		{L"Esc", VK_ESCAPE},
		{L"F1", VK_F1},
		{L"F2", VK_F2},
		{L"F3", VK_F3},
		{L"F4", VK_F4},
		{L"F5", VK_F5},
		{L"F6", VK_F6},
		{L"F7", VK_F7},
		{L"F8", VK_F8},
		{L"F9", VK_F9},
		{L"F10", VK_F10},
		{L"F11", VK_F11},
		{L"F12", VK_F12},
		{L"ScrollLock", VK_SCROLL},
		{L"Pause", VK_PAUSE},
		{L"`", VK_OEM_3},
		{L"1", 0x31},
		{L"2", 0x32},
		{L"3", 0x33},
		{L"4", 0x34},
		{L"5", 0x35},
		{L"6", 0x36},
		{L"7", 0x37},
		{L"8", 0x38},
		{L"9", 0x39},
		{L"0", 0x30},
		{L"-", VK_OEM_MINUS},
		{L"=", VK_OEM_PLUS},
		{L"Backspace", VK_BACK},
		{L"Insert", VK_INSERT},
		{L"Home", VK_HOME},
		{L"PageUp", VK_PRIOR},
		{L"NumLock", VK_NUMLOCK},
		{L"Num/", VK_DIVIDE},
		{L"Num*", VK_MULTIPLY},
		{L"Num-", VK_SUBTRACT},
		{L"Tab", VK_TAB},
		{L"Q", 0x51},
		{L"W", 0x57},
		{L"E", 0x45},
		{L"R", 0x52},
		{L"T", 0x54},
		{L"Y", 0x59},
		{L"U", 0x55},
		{L"I", 0x49},
		{L"O", 0x4F},
		{L"P", 0x50},
		{L"[", 0xDB},
		{L"]", 0xDD},
		{L"Enter", VK_RETURN},
		{L"Delete", VK_DELETE},
		{L"End", VK_END},
		{L"PageDown", VK_NEXT},
		{L"Num7", VK_NUMPAD7},
		{L"Num8", VK_NUMPAD8},
		{L"Num9", VK_NUMPAD9},
		{L"Num+", VK_ADD},
		{L"CapsLock", VK_CAPITAL},
		{L"A", 0x41},
		{L"S", 0x53},
		{L"D", 0x44},
		{L"F", 0x46},
		{L"G", 0x47},
		{L"H", 0x48},
		{L"J", 0x4A},
		{L"K", 0x4B},
		{L"L", 0x4C},
		{L";", VK_OEM_1},
		{L"'", VK_OEM_7},
		{L"\\", VK_OEM_5},
		{L"Num4", VK_NUMPAD4},
		{L"Num5", VK_NUMPAD5},
		{L"Num6", VK_NUMPAD6},
		{L"Shift", VK_SHIFT},
		{L"LShift", VK_LSHIFT},
		{L"RShift", VK_RSHIFT},
		{L"Z", 0x5A},
		{L"X", 0x58},
		{L"C", 0x43},
		{L"V", 0x56},
		{L"B", 0x42},
		{L"N", 0x4E},
		{L"M", 0x4D},
		{L",", VK_OEM_COMMA},
		{L".", VK_OEM_PERIOD},
		{L"/", VK_OEM_2},
		{L"ArrowUp", VK_UP},
		{L"Num1", VK_NUMPAD1},
		{L"Num2", VK_NUMPAD2},
		{L"Num3", VK_NUMPAD3},
		{L"Control", VK_CONTROL},
		{L"LControl", VK_LCONTROL},
		{L"RControl", VK_RCONTROL},
		{L"Alt", VK_MENU },
		{L"LAlt", VK_LMENU },
		{L"RAlt", VK_RMENU },
		{L"LWin", VK_LWIN },
		{L"RWin", VK_RWIN },
		{L"Space", VK_SPACE },
		{L"ArrowLeft", VK_LEFT },
		{L"ArrowDown", VK_DOWN },
		{L"ArrowRight", VK_RIGHT },
		{L"Num0", VK_NUMPAD0 },
		{L"NumDel", VK_DECIMAL },
		{L"Clear", 0x0C}
	};

	// Пытаемся найти ключ в карте
	auto it = keyMap.find(key);
	if (it != keyMap.end()) {
		return it->second; // Возвращаем найденное значение
	}
	else {
		return 0; // Если ключ не найден, возвращаем значение по умолчанию
	}
}

std::wstring keyToString(DWORD keyCode)
{
	static const std::unordered_map<DWORD, std::wstring> keyMap = {
		{VK_ESCAPE, L"Esc"},
		{VK_F1, L"F1"},
		{VK_F2, L"F2"},
		{VK_F3, L"F3"},
		{VK_F4, L"F4"},
		{VK_F5, L"F5"},
		{VK_F6, L"F6"},
		{VK_F7, L"F7"},
		{VK_F8, L"F8"},
		{VK_F9, L"F9"},
		{VK_F10, L"F10"},
		{VK_F11, L"F11"},
		{VK_F12, L"F12"},
		{VK_SCROLL, L"ScrollLock"},
		{VK_PAUSE, L"Pause"},
		{VK_OEM_3, L"`"},
		{0x31, L"1"},
		{0x32, L"2"},
		{0x33, L"3"},
		{0x34, L"4"},
		{0x35, L"5"},
		{0x36, L"6"},
		{0x37, L"7"},
		{0x38, L"8"},
		{0x39, L"9"},
		{0x30, L"0"},
		{VK_OEM_MINUS, L"-"},
		{VK_OEM_PLUS, L"="},
		{VK_BACK, L"Backspace"},
		{VK_INSERT, L"Insert"},
		{VK_HOME, L"Home"},
		{VK_PRIOR, L"PageUp"},
		{VK_NUMLOCK, L"NumLock"},
		{VK_DIVIDE, L"Num/"},
		{VK_MULTIPLY, L"Num*"},
		{VK_SUBTRACT, L"Num-"},
		{VK_TAB, L"Tab"},
		{0x51, L"Q"},
		{0x57, L"W"},
		{0x45, L"E"},
		{0x52, L"R"},
		{0x54, L"T"},
		{0x59, L"Y"},
		{0x55, L"U"},
		{0x49, L"I"},
		{0x4F, L"O"},
		{0x50, L"P"},
		{0xDB, L"["},
		{0xDD, L"]"},
		{VK_RETURN, L"Enter"},
		{VK_DELETE, L"Delete"},
		{VK_END, L"End"},
		{VK_NEXT, L"PageDown"},
		{VK_NUMPAD7, L"Num7"},
		{VK_NUMPAD8, L"Num8"},
		{VK_NUMPAD9, L"Num9"},
		{VK_ADD, L"Num+"},
		{VK_CAPITAL, L"CapsLock"},
		{0x41, L"A"},
		{0x53, L"S"},
		{0x44, L"D"},
		{0x46, L"F"},
		{0x47, L"G"},
		{0x48, L"H"},
		{0x4A, L"J"},
		{0x4B, L"K"},
		{0x4C, L"L"},
		{VK_OEM_1, L";"},
		{VK_OEM_7, L"'"},
		{VK_OEM_5, L"\\"},
		{VK_NUMPAD4, L"Num4"},
		{VK_NUMPAD5, L"Num5"},
		{VK_NUMPAD6, L"Num6"},
		{VK_SHIFT, L"Shift"},
		{VK_LSHIFT, L"LShift"},
		{VK_RSHIFT, L"RShift"},
		{0x5A, L"Z"},
		{0x58, L"X"},
		{0x43, L"C"},
		{0x56, L"V"},
		{0x42, L"B"},
		{0x4E, L"N"},
		{0x4D, L"M"},
		{VK_OEM_COMMA, L","},
		{VK_OEM_PERIOD, L"."},
		{VK_OEM_2, L"/"},
		{VK_UP, L"ArrowUp"},
		{VK_NUMPAD1, L"Num1"},
		{VK_NUMPAD2, L"Num2"},
		{VK_NUMPAD3, L"Num3"},
		{VK_CONTROL, L"Control"},
		{VK_RCONTROL, L"RControl"},
		{VK_LCONTROL, L"LControl"},
		{VK_MENU, L"Alt" },
		{VK_LMENU, L"LAlt"},
		{VK_RMENU, L"RAlt"},
		{VK_LWIN, L"LWin" },
		{VK_RWIN, L"RWin" },
		{VK_SPACE, L"Space" },
		{VK_LEFT, L"ArrowLeft" },
		{VK_DOWN, L"ArrowDown" },
		{VK_RIGHT, L"ArrowRight" },
		{VK_NUMPAD0, L"Num0" },
		{VK_DECIMAL, L"NumDel"},
		{0x0C, L"Clear"}
	};

	// Пытаемся найти код клавиши в карте
	auto it = keyMap.find(keyCode);
	if (it != keyMap.end())
	{
		return it->second; // Если нашли значение
	}
	else
	{
		return L"Unknown"; // Если не нашли
	}
}
bool isRunningAsAdministrator()
{
	BOOL isAdmin = FALSE;
	PSID administratorsGroup = NULL;

	SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
	if (AllocateAndInitializeSid(
		&ntAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&administratorsGroup)) {
		if (!CheckTokenMembership(NULL, administratorsGroup, &isAdmin)) {
			isAdmin = FALSE;
		}
		FreeSid(administratorsGroup);
	}

	return isAdmin;
}
bool runAsAdministrator()
{
	if (!isRunningAsAdministrator())
	{
		wchar_t path[MAX_PATH];
		if (GetModuleFileNameW(NULL, path, MAX_PATH) == 0) {

		}
		HINSTANCE result = ShellExecuteW(
			NULL,
			L"runas",
			path,
			NULL,
			NULL,
			SW_SHOWNORMAL
		);
		if ((int)result <= 32) {
			return false;
		}

		return true;
	}
	else return false;
}
