#include "updater.h"
#include "../version.h"
#include <fstream>
#include <filesystem>
#include "Utility/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

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
		updateFoundWindow = new UpdateFoundWindow();
		if (latestVersion.empty())
		{
			MessageBox(NULL, "Check your internet connection and try again", "Error", MB_ICONEXCLAMATION | MB_OK);
		}
		else
		{
			if (latestVersion > currentVersion)
			{
				if (updateFoundWindow->createWindow(currentVersion, latestVersion))
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
		SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
	}
}
	Updater::~Updater()
	{
		delete updateFoundWindow;
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
		DWORD protocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
		WinHttpSetOption(hSession, WINHTTP_OPTION_SECURE_PROTOCOLS, &protocols, sizeof(protocols));
		DWORD securityFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID;
		WinHttpSetOption(hSession, WINHTTP_OPTION_SECURITY_FLAGS, &securityFlags, sizeof(securityFlags));

		// Convert std::string to std::wstring
		std::wstring wUrl(url.begin(), url.end());
		std::wstring wFilePath(filePath.begin(), filePath.end());

		// Initialize WinHTTP session
		hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS, 0);
		if (hSession) {
			URL_COMPONENTS urlComp = { 0 };
			urlComp.dwStructSize = sizeof(urlComp);
			urlComp.dwSchemeLength = (DWORD)-1;
			urlComp.dwHostNameLength = (DWORD)-1;
			urlComp.dwUrlPathLength = (DWORD)-1;

			wchar_t szHostName[256] = { 0 };
			wchar_t szUrlPath[2048] = { 0 };
			urlComp.lpszHostName = szHostName;
			urlComp.dwHostNameLength = ARRAYSIZE(szHostName);
			urlComp.lpszUrlPath = szUrlPath;
			urlComp.dwUrlPathLength = ARRAYSIZE(szUrlPath);

			// Crack the URL
			if (WinHttpCrackUrl(wUrl.c_str(), 0, 0, &urlComp)) {
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
											MessageBoxA(NULL, "Failed to open file for writing: " + *filePath.c_str(), "Error", MB_ICONEXCLAMATION | MB_OK);
											return false;
										}

										// Read data
										do {
											dwSize = 0;
											if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
												MessageBoxA(NULL, "Error in WinHttpQueryDataAvailable ", "Error", MB_ICONEXCLAMATION | MB_OK);
												break;
											}

											pszOutBuffer = new char[dwSize + 1];
											if (!pszOutBuffer) {
												MessageBoxA(NULL, "Out of memory", "Error", MB_ICONEXCLAMATION | MB_OK);
												break;
											}

											ZeroMemory(pszOutBuffer, dwSize + 1);

											if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
												dwSize, &dwDownloaded)) {
												MessageBoxA(NULL, "Error in WinHttpReadData", "Error", MB_ICONEXCLAMATION | MB_OK);
											}
											else {
												outFile.write(pszOutBuffer, dwDownloaded);
											}

											delete[] pszOutBuffer;

										} while (dwSize > 0);

										outFile.close();
									}
									else {
										MessageBoxA(NULL, "Failed to download file.HTTP Status Code : " + dwStatusCode, "Error", MB_ICONEXCLAMATION | MB_OK);
									}
								}
							}
						}
						else
						{
							MessageBoxA(NULL, "Error in WinHttpSendRequest: " + GetLastError(), "Error", MB_ICONEXCLAMATION | MB_OK);
							return false;
						}
					}
					else
					{
						MessageBoxA(NULL, "Error send HTTP Request: " + GetLastError(), "Error", MB_ICONEXCLAMATION | MB_OK);
						return false;
					}
				}
				else
				{
					MessageBoxA(NULL, "Error creating HTTP Request: " + GetLastError(), "Error", MB_ICONEXCLAMATION | MB_OK);
					return false;
				}
			}
			else
			{
				MessageBoxA(NULL, "Error cracking URL: " + GetLastError(), "Error", MB_ICONEXCLAMATION | MB_OK);
				return false;
			}
		}
		else {
			MessageBoxA(NULL, "Error creating session: " + GetLastError(), "Error", MB_ICONEXCLAMATION | MB_OK);
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
		HINTERNET hSession = WinHttpOpen(L"TerminalsThisWay/1.0 (Windows NT)",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS, 0);
		if (!hSession) {
			MessageBoxA(NULL, "WinHttpOpen failed", "Error", MB_ICONEXCLAMATION | MB_OK);
			return "";
		}
		DWORD protocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
		WinHttpSetOption(hSession, WINHTTP_OPTION_SECURE_PROTOCOLS, &protocols, sizeof(protocols));
		DWORD securityFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID;
		WinHttpSetOption(hSession, WINHTTP_OPTION_SECURITY_FLAGS, &securityFlags, sizeof(securityFlags));
		HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
		if (!hConnect) {
			MessageBoxA(NULL, "WinHttpConnect failed", "Error", MB_ICONEXCLAMATION | MB_OK);
			WinHttpCloseHandle(hSession);
			return "";
		}
		HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(),
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_SECURE);
		if (!hRequest) {
			MessageBoxA(NULL, "WinHttpOpenRequest failed", "Error", MB_ICONEXCLAMATION | MB_OK);
			WinHttpCloseHandle(hConnect);
			WinHttpCloseHandle(hSession);
			return "";
		}
		BOOL bResults = WinHttpSendRequest(hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS, 0,
			WINHTTP_NO_REQUEST_DATA, 0,
			0, 0);
		if (!bResults) {
			MessageBoxA(NULL, "WinHttpSendRequest failed", "Error", MB_ICONEXCLAMATION | MB_OK);
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			WinHttpCloseHandle(hSession);
			return "";
		}

		bResults = WinHttpReceiveResponse(hRequest, NULL);

		if (!bResults) {
			MessageBoxA(NULL, "WinHttpReceiveResponse failed", "Error", MB_ICONEXCLAMATION | MB_OK);
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
				MessageBoxA(NULL, "WinHttpQueryDataAvailable failed", "Error", MB_ICONEXCLAMATION | MB_OK);
				break;
			}

			if (dwSize == 0)
				break;

			char* buffer = new char[dwSize + 1];
			ZeroMemory(buffer, dwSize + 1);

			if (!WinHttpReadData(hRequest, (LPVOID)buffer, dwSize, &dwDownloaded)) {
				MessageBoxA(NULL, "WinHttpReadData failed", "Error", MB_ICONEXCLAMATION | MB_OK);
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

#if defined(_WIN64)
			dowloadUrl = jsonResponse["assets"][1]["browser_download_url"];
#elif defined(_WIN32)
			dowloadUrl = jsonResponse["assets"][0]["browser_download_url"];
#endif
			lastVersion = jsonResponse["tag_name"];
		}
		catch (const json::exception& e) {
			std::cerr << "JSON parsing error: " << e.what() << std::endl;
		}
	}

	void Updater::RestartApplication() {

		wchar_t path[MAX_PATH];
		if (GetModuleFileNameW(NULL, path, MAX_PATH) == 0) {
			std::cerr << "Failed to get module file name. Error: " << GetLastError() << std::endl;
			return;
		}

		std::wstring szPath = path;
		std::wstring szDir = szPath.substr(0, szPath.rfind(L"\\") + 1);
		std::wstring installerPath = L"TerminalsThisWayNew.exe";

		std::wstring installParameter = L"/S /D=" + szDir + L"";
		SHELLEXECUTEINFOW sei = { sizeof(sei) };
		sei.lpVerb = L"runas";  // Running with elevated privileges
		sei.lpFile = installerPath.c_str();
		sei.lpParameters = installParameter.c_str();
		sei.nShow = SW_NORMAL;

		// Completion of the current process

		if (!ShellExecuteExW(&sei)) {
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