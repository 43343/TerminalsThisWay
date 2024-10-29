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
		langMap["settings"] = "���������";
		langMap["general"] = "��������";
		langMap["pathToTerminal"] = "���� � ���������:";
		langMap["localization"] = "�����������:";
		langMap["launchByDefault"] = "������ �� ���������:";
		langMap["runAsAdministrator"] = "������ �� ����� ��������������:";
		langMap["keyAssignment"] = "���������� ������";
		langMap["sendCommand"] = "��������� ��������:";
		langMap["sendCommandParameter"] = "��������� �������� (��������� ���������):";
		langMap["chooseFolder"] = "������� �����:";
		langMap["warning"] = "��� ���������� �������� ����� ������� ������� ������� ���������";
		langMap["save"] = "���������";
		langMap["cancel"] = "������";
		langMap["restartAsAnAdministrator"] = "������������� �� ����� ��������������";
		langMap["updateConfiguration"] = "�������� ������������";
		langMap["changeConfiguration"] = "�������� ������������";
		langMap["exit"] = "�����";
		langMap["applicationAlreadyRunning"] = "���������� ��� ��������.";
		langMap["applicationAlreadyRunningDescription"] = "������ ��������� ���������� ��� �������.";
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