#include "localization.h"

Localization::Localization(std::string lang)
{
	if (lang == "English")
	{
		langMap["settings"] = "Settings";
		langMap["general"] = "General";
		langMap["pathToTerminal"] = "Path to terminal:";
		langMap["localization"] = "Localization:";
		langMap["launchByDefault"] = "Launch by default:";
		langMap["runAsAdministrator"] = "Run As Administrator:";
		langMap["keyAssignment"] = "Key Assignment";
		langMap["sendCommand"] = "Send command:";
		langMap["sendCommandParameter"] = "Send command (Change parameter):";
		langMap["chooseFolder"] = "Choose folder:";
		langMap["warning"] = "To apply the settings, you need to close the current terminal process";
		langMap["save"] = "Save";
		langMap["cancel"] = "Cancel";
		langMap["restartAsAnAdministrator"] = "Restart as an administrator";
		langMap["updateConfiguration"] = "Update Configuration";
		langMap["changeConfiguration"] = "Change Configuration";
		langMap["exit"] = "Exit";
		langMap["applicationAlreadyRunning"] = "Application already running";
		langMap["applicationAlreadyRunningDescription"] = "Another instance of the application is already running.";
	}
	if (lang == "Russian")
	{
		langMap["settings"] = "Настройки";
		langMap["general"] = "Основное";
		langMap["pathToTerminal"] = "Путь к терминалу:";
		langMap["localization"] = "Локализация:";
		langMap["launchByDefault"] = "Запуск по умолчанию:";
		langMap["runAsAdministrator"] = "Запуск от имени администратора:";
		langMap["keyAssignment"] = "Назначение клавиш";
		langMap["sendCommand"] = "Отправить комманду:";
		langMap["sendCommandParameter"] = "Отправить комманду (Изменение параметра):";
		langMap["chooseFolder"] = "Выбрать папку:";
		langMap["warning"] = "Для применения настроек нужно закрыть текущий процесс терминала";
		langMap["save"] = "Сохранить";
		langMap["cancel"] = "Отмена";
		langMap["restartAsAnAdministrator"] = "Перезапустить от имени администратора";
		langMap["updateConfiguration"] = "Обновить конфигурацию";
		langMap["changeConfiguration"] = "Изменить конфигурацию";
		langMap["exit"] = "Выход";
		langMap["applicationAlreadyRunning"] = "Приложение уже запущено.";
		langMap["applicationAlreadyRunningDescription"] = "Другой экземпляр приложения уже запущен.";
	}
}

std::string Localization::getText(const std::string& text)
{
	auto it = langMap.find(text);
	if (it != langMap.end())
	{
		return it->second;
	}
	else
	{
		return "";
	}
}