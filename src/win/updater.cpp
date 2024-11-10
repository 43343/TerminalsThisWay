#include "updater.h"
#include "../version.h"
#include <fstream>
#include "Utility/json.hpp"

using json = nlohmann::json;

Updater::Updater()
{
}
void Updater::checkUpdate()
{

	if (!isActive) {
		isActive = true;
		std::string latestVersion;
		std::string downloadUrl;
		std::string localPath = "TerminalsThisWayNew.exe";
		GetJsonData(downloadUrl, latestVersion);
		modalWindow = new ModalWindow();
		if (latestVersion.empty())
		{
			MessageBox(NULL, "Check your internet connection and try again", "Error", MB_ICONEXCLAMATION | MB_OK);
		}
		else
		{
			if (latestVersion != currentVersion)
			{
				if (modalWindow->createWindow(currentVersion, latestVersion))
				{
					if (DownloadFile(downloadUrl, localPath))
					{
						RestartApplication();
					}
				}
			}
			else
			{
				MessageBox(NULL, "You have the latest version of the app installed", "Update", MB_ICONEXCLAMATION | MB_OK);
			}
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
	bool Updater::DownloadFile(const std::string& url, const std::string& filePath)
	{
		HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;
		LPSTR pszOutBuffer;
		BOOL  bResults = FALSE;
		DWORD dwStatusCode = 0;
		DWORD dwStatusSize = sizeof(dwStatusCode);

		// Convert std::string to std::wstring
		std::wstring wUrl(url.begin(), url.end());
		std::wcout << wUrl;
		std::wstring wFilePath(filePath.begin(), filePath.end());

		// Initialize WinHTTP session
		hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS, 0);
		if (hSession) {
			URL_COMPONENTS urlComp;
			ZeroMemory(&urlComp, sizeof(urlComp));
			urlComp.dwStructSize = sizeof(urlComp);

			// Set required component lengths to non-zero so that they are cracked.
			urlComp.dwSchemeLength = -1;
			urlComp.dwHostNameLength = -1;
			urlComp.dwUrlPathLength = -1;
			urlComp.dwExtraInfoLength = -1;

			wchar_t hostName[256];
			wchar_t urlPath[256];
			urlComp.lpszHostName = hostName;
			urlComp.dwHostNameLength = ARRAYSIZE(hostName);
			urlComp.lpszUrlPath = urlPath;
			urlComp.dwUrlPathLength = ARRAYSIZE(urlPath);

			// Crack the URL
			if (WinHttpCrackUrl(wUrl.c_str(), 0, 0, &urlComp)) {
				// Connect to the server
				std::cout << " hostname " << urlComp.lpszHostName << "wUrl" << wUrl.c_str();
				hConnect = WinHttpConnect(hSession, urlComp.lpszHostName,
					urlComp.nPort, 0);
				if (hConnect) {
					// Create an HTTP request handle
					hRequest = WinHttpOpenRequest(hConnect, L"GET",
						urlComp.lpszUrlPath,
						NULL, WINHTTP_NO_REFERER,
						WINHTTP_DEFAULT_ACCEPT_TYPES,
						(urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0);
					if (hRequest) {
						// Send a request
						bResults = WinHttpSendRequest(hRequest,
							WINHTTP_NO_ADDITIONAL_HEADERS, 0,
							WINHTTP_NO_REQUEST_DATA, 0,
							0, 0);
						if (bResults) {
							// Receive a response
							bResults = WinHttpReceiveResponse(hRequest, NULL);
							if (bResults) {
								// Check HTTP status code
								if (WinHttpQueryHeaders(hRequest,
									WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
									NULL, &dwStatusCode, &dwStatusSize, NULL)) {
									if (dwStatusCode == 200) { // HTTP_STATUS_OK
										std::ofstream outFile(filePath, std::ios::binary);
										if (!outFile) {
											std::cerr << "Failed to open file for writing: " << filePath << std::endl;
											return false;
										}

										// Read data
										do {
											dwSize = 0;
											if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
												std::cerr << "Error in WinHttpQueryDataAvailable." << std::endl;
												break;
											}

											pszOutBuffer = new char[dwSize + 1];
											if (!pszOutBuffer) {
												std::cerr << "Out of memory" << std::endl;
												break;
											}

											ZeroMemory(pszOutBuffer, dwSize + 1);

											if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
												dwSize, &dwDownloaded)) {
												std::cerr << "Error in WinHttpReadData." << std::endl;
											}
											else {
												outFile.write(pszOutBuffer, dwDownloaded);
											}

											delete[] pszOutBuffer;

										} while (dwSize > 0);

										outFile.close();
										std::cout << "File downloaded successfully to " << filePath << std::endl;
									}
									else {
										std::cerr << "Failed to download file. HTTP Status Code: " << dwStatusCode << std::endl;
									}
								}
							}
						}
						else
						{
							std::cerr << "Error in WinHttpSendRequest: " << GetLastError() << std::endl;
							return false;
						}
					}
					else
					{
						std::cerr << "Error send HTTP Request: " << GetLastError() << std::endl;
						return false;
					}
				}
				else
				{
					std::cerr << "Error creating HTTP Request: " << GetLastError() << std::endl;
					return false;
				}
			}
			else
			{
				std::cerr << "Error cracking URL: " << GetLastError() << std::endl;
				return false;
			}
		}
		else {
			std::cerr << "Error creating session: " << GetLastError() << std::endl;
			return false;
		}

		// Cleanup
		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);

		return true;
	}

	std::string Updater::GetJsonFromUrl(const std::wstring& url)
	{
		std::wstring host = L"api.github.com";
		std::wstring path = L"/repos/43343/TerminalsThisWay/releases/latest";
		HINTERNET hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS, 0);
		if (!hSession) {
			std::cerr << "WinHttpOpen failed" << std::endl;
			return "";
		}
		HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
		if (!hConnect) {
			std::cerr << "WinHttpConnect failed" << std::endl;
			WinHttpCloseHandle(hSession);
			return "";
		}
		HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(),
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_SECURE);
		if (!hRequest) {
			std::cerr << "WinHttpOpenRequest failed" << std::endl;
			WinHttpCloseHandle(hConnect);
			WinHttpCloseHandle(hSession);
			return "";
		}
		BOOL bResults = WinHttpSendRequest(hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS, 0,
			WINHTTP_NO_REQUEST_DATA, 0,
			0, 0);
		if (!bResults) {
			std::cerr << "WinHttpSendRequest failed" << std::endl;
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			WinHttpCloseHandle(hSession);
			return "";
		}

		bResults = WinHttpReceiveResponse(hRequest, NULL);

		if (!bResults) {
			std::cerr << "WinHttpReceiveResponse failed" << std::endl;
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			WinHttpCloseHandle(hSession);
			return "";
		}

		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;
		std::string responseData;
		do {
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
				std::cerr << "WinHttpQueryDataAvailable failed" << std::endl;
				break;
			}

			if (dwSize == 0)
				break;

			char* buffer = new char[dwSize + 1];
			ZeroMemory(buffer, dwSize + 1);

			if (!WinHttpReadData(hRequest, (LPVOID)buffer, dwSize, &dwDownloaded)) {
				std::cerr << "WinHttpReadData failed" << std::endl;
				delete[] buffer;
				break;
			}

			responseData.append(buffer, dwDownloaded);
			delete[] buffer;
		} while (dwSize > 0);
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);

		return responseData;
	}

	void Updater::GetJsonData(std::string& dowloadUrl, std::string& lastVersion) {
		std::string jsonString = GetJsonFromUrl(L"https://api.github.com/repos/43343/TerminalsThisWay/releases/latest");

		if (jsonString.empty()) {
			std::cerr << "Failed to get JSON from URL" << std::endl;
		}
		try {
			auto jsonResponse = json::parse(jsonString);

			// Извлечение значений
			dowloadUrl = jsonResponse["assets"][0]["browser_download_url"];
			lastVersion = jsonResponse["tag_name"];

		}
		catch (const json::exception& e) {
			std::cerr << "Ошибка парсинга JSON: " << e.what() << std::endl;
		}
	}

	void Updater::RestartApplication() {

		char path[MAX_PATH];
		if (GetModuleFileName(NULL, path, MAX_PATH) == 0) {
			std::cerr << "Failed to get module file name. Error: " << GetLastError() << std::endl;
			return;
		}

		std::string szPath = path;
		std::string szDir = szPath.substr(0, szPath.rfind("\\") + 1);
		std::string installerPath = "TerminalsThisWayNew.exe";

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