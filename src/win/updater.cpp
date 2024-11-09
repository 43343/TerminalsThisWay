#include "updater.h"
#include "../version.h"
#include <fstream>

Updater::Updater()
{
}
void Updater::checkUpdate()
{

	if (!isActive) {
		isActive = true;
		const std::string latestVersion = GetLatestVersion();
		modalWindow = new ModalWindow();
		if (latestVersion != currentVersion)
		{
			std::cout << "createWindow";
			if (modalWindow->createWindow(currentVersion, latestVersion))
			{
				RestartApplication();
			}
			std::cout << "end";
			//if (DownloadFile(downloadUrl, localPath));
			//{
			//	RestartApplication(localPath);
			//}
		}
		else
		{
			MessageBox(NULL, "You have the latest version of the app installed", "Update", MB_ICONEXCLAMATION | MB_OK);
		}
		isActive = false;
	}
}
	Updater::~Updater()
	{
		delete modalWindow;
	}
	bool Updater::getActive()
	{
		return isActive;
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
		const std::string versionUrl = "https://api.github.com/repos/yourusername/yourrepo/releases/latest";

		return "0.0.2"; 
	}

	void Updater::RestartApplication() {

		char path[MAX_PATH];
		if (GetModuleFileName(NULL, path, MAX_PATH) == 0) {
			std::cerr << "Failed to get module file name. Error: " << GetLastError() << std::endl;
			return;
		}

		std::string szPath = path;
		std::string szDir = szPath.substr(0, szPath.rfind("\\") + 1);
		std::string installerPath = szDir + "\TerminalsThisWay-0.0.1-win64.exe";

		std::string installParameter = "/S /D=\"" + szDir + "\"";

		SHELLEXECUTEINFO sei = { sizeof(sei) };
		sei.lpVerb = "runas";  // Running with elevated privileges
		sei.lpFile = installerPath.c_str();
		sei.lpParameters = installParameter.c_str();
		sei.nShow = SW_NORMAL;

		// Completion of the current process

		if (!ShellExecuteEx(&sei)) {
			DWORD dwError = GetLastError();
			if (dwError == ERROR_CANCELLED) {
				// The user canceled the privilege escalation
				MessageBox(NULL, "The installer was canceled by the user.", "Error", MB_OK | MB_ICONERROR);
			}
			else {
				// Another mistake
				MessageBox(NULL, "The installer could not be started.", "Error", MB_OK | MB_ICONERROR);
			}
		}
	}