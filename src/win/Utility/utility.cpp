#include"utility.h"

DWORD stringTokey(const std::string& key)
{
	static const std::unordered_map<std::string, int> keyMap = {
		{"Esc", VK_ESCAPE},
		{"F1", VK_F1},
		{"F2", VK_F2},
		{"F3", VK_F3},
		{"F4", VK_F4},
		{"F5", VK_F5},
		{"F6", VK_F6},
		{"F7", VK_F7},
		{"F8", VK_F8},
		{"F9", VK_F9},
		{"F10", VK_F10},
		{"F11", VK_F11},
		{"F12", VK_F12},
		{"ScrollLock", VK_SCROLL},
		{"Pause", VK_PAUSE},
		{"`", VK_OEM_3},
		{"1", 0x31},
		{"2", 0x32},
		{"3", 0x33},
		{"4", 0x34},
		{"5", 0x35},
		{"6", 0x36},
		{"7", 0x37},
		{"8", 0x38},
		{"9", 0x39},
		{"0", 0x30},
		{"-", VK_OEM_MINUS},
		{"=", VK_OEM_PLUS},
		{"Backspace", VK_BACK},
		{"Insert", VK_INSERT},
		{"Home", VK_HOME},
		{"PageUp", VK_PRIOR},
		{"NumLock", VK_NUMLOCK},
		{"Num/", VK_DIVIDE},
		{"Num*", VK_MULTIPLY},
		{"Num-", VK_SUBTRACT},
		{"Tab", VK_TAB},
		{"Q", 0x51},
		{"W", 0x57},
		{"E", 0x45},
		{"R", 0x52},
		{"T", 0x54},
		{"Y", 0x59},
		{"U", 0x55},
		{"I", 0x49},
		{"O", 0x4F},
		{"P", 0x50},
		{"[", 0xDB},
		{"]", 0xDD},
		{"Enter", VK_RETURN},
		{"Delete", VK_DELETE},
		{"End", VK_END},
		{"PageDown", VK_NEXT},
		{"Num7", VK_NUMPAD7},
		{"Num8", VK_NUMPAD8},
		{"Num9", VK_NUMPAD9},
		{"Num+", VK_ADD},
		{"CapsLock", VK_CAPITAL},
		{"A", 0x41},
		{"S", 0x53},
		{"D", 0x44},
		{"F", 0x46},
		{"G", 0x47},
		{"H", 0x48},
		{"J", 0x4A},
		{"K", 0x4B},
		{"L", 0x4C},
		{";", VK_OEM_1},
		{"'", VK_OEM_7},
		{"\\", VK_OEM_5},
		{"Num4", VK_NUMPAD4},
		{"Num5", VK_NUMPAD5},
		{"Num6", VK_NUMPAD6},
		{"Shift", VK_SHIFT},
		{"LShift", VK_LSHIFT},
		{"RShift", VK_RSHIFT},
		{"Z", 0x5A},
		{"X", 0x58},
		{"C", 0x43},
		{"V", 0x56},
		{"B", 0x42},
		{"N", 0x4E},
		{"M", 0x4D},
		{",", VK_OEM_COMMA},
		{".", VK_OEM_PERIOD},
		{"/", VK_OEM_2},
		{"ArrowUp", VK_UP},
		{"Num1", VK_NUMPAD1},
		{"Num2", VK_NUMPAD2},
		{"Num3", VK_NUMPAD3},
		{"Control", VK_CONTROL},
		{"RControl", VK_RCONTROL},
		{"LControl", VK_LCONTROL},
		{"Alt", VK_MENU },
		{"LAlt", VK_LMENU },
		{"RAlt", VK_RMENU },
		{"LWin", VK_LWIN },
		{"RWin", VK_RWIN },
		{"Space", VK_SPACE },
		{"ArrowLeft", VK_LEFT },
		{"ArrowDown", VK_DOWN },
		{"ArrowRight", VK_RIGHT },
		{"Num0", VK_NUMPAD0 },
		{"NumDel", VK_DECIMAL },
		{"Clear", 0x0C}
	};
	auto it = keyMap.find(key);
	if (it != keyMap.end())
	{
		return it->second;
	}
	else
	{
		return 0;
	}
}

std::string keyToString(DWORD keyCode)
{
	static const std::unordered_map<DWORD, std::string> keyMap = {
		{VK_ESCAPE, "Esc"},
		{VK_F1, "F1"},
		{VK_F2, "F2"},
		{VK_F3, "F3"},
		{VK_F4, "F4"},
		{VK_F5, "F5"},
		{VK_F6, "F6"},
		{VK_F7, "F7"},
		{VK_F8, "F8"},
		{VK_F9, "F9"},
		{VK_F10, "F10"},
		{VK_F11, "F11"},
		{VK_F12, "F12"},
		{VK_SCROLL, "ScrollLock"},
		{VK_PAUSE, "Pause"},
		{VK_OEM_3, "`"},
		{0x31, "1"},
		{0x32, "2"},
		{0x33, "3"},
		{0x34, "4"},
		{0x35, "5"},
		{0x36, "6"},
		{0x37, "7"},
		{0x38, "8"},
		{0x39, "9"},
		{0x30, "0"},
		{VK_OEM_MINUS, "-"},
		{VK_OEM_PLUS, "="},
		{VK_BACK, "Backspace"},
		{VK_INSERT, "Insert"},
		{VK_HOME, "Home"},
		{VK_PRIOR, "PageUp"},
		{VK_NUMLOCK, "NumLock"},
		{VK_DIVIDE, "Num/"},
		{VK_MULTIPLY, "Num*"},
		{VK_SUBTRACT, "Num-"},
		{VK_TAB, "Tab"},
		{0x51, "Q"},
		{0x57, "W"},
		{0x45, "E"},
		{0x52, "R"},
		{0x54, "T"},
		{0x59, "Y"},
		{0x55, "U"},
		{0x49, "I"},
		{0x4F, "O"},
		{0x50, "P"},
		{0xDB, "["},
		{0xDD, "]"},
		{VK_RETURN, "Enter"},
		{VK_DELETE, "Delete"},
		{VK_END, "End"},
		{VK_NEXT, "PageDown"},
		{VK_NUMPAD7, "Num7"},
		{VK_NUMPAD8, "Num8"},
		{VK_NUMPAD9, "Num9"},
		{VK_ADD, "Num+"},
		{VK_CAPITAL, "CapsLock"},
		{0x41, "A"},
		{0x53, "S"},
		{0x44, "D"},
		{0x46, "F"},
		{0x47, "G"},
		{0x48, "H"},
		{0x4A, "J"},
		{0x4B, "K"},
		{0x4C, "L"},
		{VK_OEM_1, ";"},
		{VK_OEM_7, "'"},
		{VK_OEM_5, "\\"},
		{VK_NUMPAD4, "Num4"},
		{VK_NUMPAD5, "Num5"},
		{VK_NUMPAD6, "Num6"},
		{VK_SHIFT, "Shift"},
		{VK_LSHIFT, "LShift"},
		{VK_RSHIFT, "RShift"},
		{0x5A, "Z"},
		{0x58, "X"},
		{0x43, "C"},
		{0x56, "V"},
		{0x42, "B"},
		{0x4E, "N"},
		{0x4D, "M"},
		{VK_OEM_COMMA, ","},
		{VK_OEM_PERIOD, "."},
		{VK_OEM_2, "/"},
		{VK_UP, "ArrowUp"},
		{VK_NUMPAD1, "Num1"},
		{VK_NUMPAD2, "Num2"},
		{VK_NUMPAD3, "Num3"},
		{VK_CONTROL, "Control"},
		{VK_RCONTROL, "RControl"},
		{VK_LCONTROL, "LControl"},
		{VK_MENU, "Alt" },
		{VK_LMENU, "LAlt"},
		{VK_RMENU, "RAlt"},
		{VK_LWIN, "LWin" },
		{VK_RWIN, "RWin" },
		{VK_SPACE, "Space" },
		{VK_LEFT, "ArrowLeft" },
		{VK_DOWN, "ArrowDown" },
		{VK_RIGHT, "ArrowRight" },
		{VK_NUMPAD0, "Num0" },
		{VK_DECIMAL, "NumDel"},
		{0x0C, "Clear"}
	};

	auto it = keyMap.find(keyCode);
	if (it != keyMap.end())
	{
		return it->second;
	}
	else
	{
		return "Unknown";
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
