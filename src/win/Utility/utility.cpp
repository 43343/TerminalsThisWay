#include"utility.h"

int stringTokey(const std::string& key)
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
		{"[", 0xDB},
		{"]", 0xDD},
		{"Enter", VK_RETURN},
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
bool isRunningAsAdministrator()
{
	BOOL isAdmin = FALSE;
	PSID administratorsGroup = NULL;

	// Создаем SID для группы администраторов
	SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
	if (AllocateAndInitializeSid(
		&ntAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&administratorsGroup)) {
		// Проверяем, является ли токен процесса членом группы администраторов
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
			NULL,                   // Дескриптор окна
			L"runas",               // Действие - запуск от имени администратора
			path, // Путь к приложению
			NULL,                   // Параметры командной строки
			NULL,                   // Рабочая директория
			SW_SHOWNORMAL           // Показать окно нормально
		);
		if ((int)result <= 32) {
			return false;
		}

		return true;
	}
	else return false;
}
