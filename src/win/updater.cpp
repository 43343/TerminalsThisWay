#include "updater.h"
#include "../version.h"

Updater::Updater(HINSTANCE hInstance)
{
	const std::string latestVersion = GetLatestVersion();
	modalWindow = new ModalWindow(hInstance);
	if (latestVersion != currentVersion)
	{
		modalWindow->createWindow(currentVersion, latestVersion, [&]()
			{
				if (DownloadFile("https://github.com/yourusername/yourrepo/releases/download/v" + latestVersion + "/TerminalsThisWaySetup.exe", "TerminalsThisWay-" + latestVersion + "-win64.exe"));
				{
					RestartApplication("TerminalsThisWay-" + latestVersion + "-win64.exe");
				}
			});
				//if (DownloadFile(downloadUrl, localPath));
				//{
				//	RestartApplication(localPath);
				//}
			}
	else
	{
		MessageBox(NULL, "You have the latest version of the app installed", "Update", MB_ICONEXCLAMATION | MB_OK);
	}
	}
	Updater::~Updater()
	{
		delete modalWindow;
	}
	bool Updater::DownloadFile(const std::string & url, const std::string & localPath)
	{
		HINTERNET hInternet = InternetOpenA("Updater", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if (!hInternet) return false;

		HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
		if (!hConnect) {
			InternetCloseHandle(hInternet);
			std::cout << "The file could not be uploaded";
			return false;
		}

		std::ofstream outFile(localPath, std::ios::binary);
		if (!outFile) {
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hInternet);
			std::cout << "The file could not be uploaded";
			return false;
		}

		char buffer[4096];
		DWORD bytesRead;
		while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead != 0) {
			outFile.write(buffer, bytesRead);
		}

		outFile.close();
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);

		return true;
	}

	std::string Updater::GetLatestVersion() {
		// ���� URL ������ ��������� �� API ��� ���� � ����������� � ������
		const std::string versionUrl = "https://api.github.com/repos/yourusername/yourrepo/releases/latest";

		// ����� �� ������ ������������ HTTP-������ ��� ��������� ���������� � ��������� ������
		// ��� ��������� ���. ���������� ������� JSON ��� ����������� ���������� ��� ������ � HTTP � JSON

		return "0.0.2"; // ������ ������������ ������
	}

	void Updater::RestartApplication(const std::string & newAppPath) {
		//std::string command = "cmd.exe /C timeout 1 && move /Y \"" + newAppPath + "\"TermainlsThisWay.exe\" && start \"yourapp.exe\"";

		// ��������� �������
		//system(command.c_str());

		// ��������� ������� ����������
		//exit(0);
	}