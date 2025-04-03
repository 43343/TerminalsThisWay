#include "application.h"
#include <shlobj.h>
#include <iostream>
#include <string>
#include <tlhelp32.h>
#include <fstream>
#include <aclapi.h>
#include <codecvt>
#include <cstdio>

Application::Application(char* argv[])
{
	wchar_t exePath[MAX_PATH];
	if (GetModuleFileNameW(NULL, exePath, MAX_PATH)) {
		std::wstring exeDir = exePath;
		size_t lastSlashPos = exeDir.find_last_of(L"\\");
		if (lastSlashPos != std::wstring::npos) {
			exeDir = exeDir.substr(0, lastSlashPos);
			if (!SetCurrentDirectoryW(exeDir.c_str())) {
				std::cerr << "Failed to set current directory to executable path. Error: " << GetLastError() << std::endl;
			}
			else {
				std::wcout << "Current directory set to: " << exeDir << std::endl;
			}
		}
	}
	else {
		std::wcerr << "Failed to get executable path. Error: " << GetLastError() << std::endl;
	}
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstring token = converter.from_bytes(argv[1]);
	//MessageBoxW(NULL, token.c_str(), L"Application already running", MB_ICONEXCLAMATION | MB_OK);
	if (!isValidToken(token))
	{
		stopWorkingThread = true;
		exit(0);
		return;
	}
	//MessageBoxW(NULL, L"токен прошел", L"Application already running", MB_ICONEXCLAMATION | MB_OK);
	SetConsoleOutputCP(65001);
	SetConsoleCP(65001);
	char appDataPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath)))
	{
		std::string newFolderPath = std::string(appDataPath) + "\\TerminalsThisWay";
		if (!SetCurrentDirectory((LPTSTR)newFolderPath.c_str())) {
			std::cerr << "Failed to set current directory. Error: " << GetLastError() << std::endl;
		}
	}
	workerThread = std::thread(&Application::commandProcessingLoop, this);
	readFromSharedMemory();
}
std::wstring Application::getAppdataFolder()
{
	wchar_t appDataPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, appDataPath)))
	{
		std::wstring newFolderPath = std::wstring(appDataPath) + L"\\TerminalsThisWay";
		if (CreateDirectoryW(newFolderPath.c_str(), NULL)) {
			std::wcout << "The folder was created successfully: " << newFolderPath << std::endl;
		}
		else if (GetLastError() == ERROR_ALREADY_EXISTS) {
			std::wcout << "The folder already exists: " << newFolderPath << std::endl;
		}
		else {
			std::wcerr << "Failed to create folder: " << newFolderPath << ", ошибка: " << GetLastError() << std::endl;
		}
		return newFolderPath;
	}
}

bool Application::isValidToken(const std::wstring& token)
{
	std::wstring filePath = getAppdataFolder() + L"\\temp.txt";
	std::wifstream infile(filePath);
	if (!infile.is_open()) {
		return false;
	}
	std::wstring line;
	std::getline(infile, line);
	infile.close();
	if (_wremove(filePath.c_str()) == 0)
	{
		std::cout << "The file was successfully deleted";
	}
	else
	{
		std::cout << "Error deleting a file";
	}

	return token == line;
}
bool Application::IsProcessRunning(DWORD processID)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		return false;
	}

	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(PROCESSENTRY32W);

	if (Process32FirstW(snapshot, &entry)) {
		do {
			if (entry.th32ProcessID == processID) {
				CloseHandle(snapshot);
				return true;
			}
		} while (Process32NextW(snapshot, &entry));
	}

	CloseHandle(snapshot);
	return false;
}
void Application::commandProcessingLoop() 
{
	while (!stopWorkingThread)
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		if (commandQueue.empty()) {
			lock.unlock();
			Sleep(300);
		      

			
			if (commandQueue.empty()) {
				stopWorkingThread = true; 
				break;  
			}
			lock.lock();
		}
		cv.wait(lock, [this]() {
			return !commandQueue.empty() || stopWorkingThread;
			});
		if (!commandQueue.empty()) {
			std::wstring nextCommand = commandQueue.front();
			commandQueue.pop();
			lock.unlock(); 

			processNextCommand(nextCommand); 

			lock.lock();
		}
	}
}
void Application::processNextCommand(std::wstring& command)
{
	if (IsProcessRunning(startedProcessIDsCMD) && startedProcessIDsCMD != 0) {
		FreeConsole();
		if (!AttachConsole(startedProcessIDsCMD)) {
			MessageBoxA(NULL, "Couldn't connect to the terminal.", "Error", MB_ICONERROR | MB_OK);
			return;
		}
		HANDLE hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);

		DWORD eventsWritten;
		INPUT_RECORD clearRecords[4];
		clearRecords[0].EventType = KEY_EVENT;
		clearRecords[0].Event.KeyEvent.bKeyDown = TRUE;
		clearRecords[0].Event.KeyEvent.dwControlKeyState = LEFT_CTRL_PRESSED;
		clearRecords[0].Event.KeyEvent.uChar.UnicodeChar = 0;
		clearRecords[0].Event.KeyEvent.wRepeatCount = 1;
		clearRecords[0].Event.KeyEvent.wVirtualKeyCode = VK_HOME;
		clearRecords[0].Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_HOME, MAPVK_VK_TO_VSC);

		clearRecords[1] = clearRecords[0];
		clearRecords[1].Event.KeyEvent.bKeyDown = FALSE;

		clearRecords[2].EventType = KEY_EVENT;
		clearRecords[2].Event.KeyEvent.bKeyDown = TRUE;
		clearRecords[2].Event.KeyEvent.dwControlKeyState = LEFT_CTRL_PRESSED;
		clearRecords[2].Event.KeyEvent.uChar.UnicodeChar = 0;
		clearRecords[2].Event.KeyEvent.wRepeatCount = 1;
		clearRecords[2].Event.KeyEvent.wVirtualKeyCode = VK_END;
		clearRecords[2].Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_END, MAPVK_VK_TO_VSC);

		clearRecords[3] = clearRecords[2];
		clearRecords[3].Event.KeyEvent.bKeyDown = FALSE;

		INPUT_RECORD inputRecords[3280];
		size_t len = command.length();
		size_t currentPos = 0;

		inputRecords[currentPos].EventType = KEY_EVENT;
		inputRecords[currentPos].Event.KeyEvent.bKeyDown = TRUE;
		inputRecords[currentPos].Event.KeyEvent.dwControlKeyState = LEFT_CTRL_PRESSED;
		inputRecords[currentPos].Event.KeyEvent.uChar.UnicodeChar = 0;
		inputRecords[currentPos].Event.KeyEvent.wRepeatCount = 1;
		inputRecords[currentPos].Event.KeyEvent.wVirtualKeyCode = VK_HOME;
		inputRecords[currentPos].Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_HOME, MAPVK_VK_TO_VSC);
		currentPos++;

		inputRecords[currentPos] = inputRecords[currentPos - 1];
		inputRecords[currentPos].Event.KeyEvent.bKeyDown = FALSE;
		currentPos++;

		inputRecords[currentPos].EventType = KEY_EVENT;
		inputRecords[currentPos].Event.KeyEvent.bKeyDown = TRUE;
		inputRecords[currentPos].Event.KeyEvent.dwControlKeyState = LEFT_CTRL_PRESSED;
		inputRecords[currentPos].Event.KeyEvent.uChar.UnicodeChar = 0;
		inputRecords[currentPos].Event.KeyEvent.wRepeatCount = 1;
		inputRecords[currentPos].Event.KeyEvent.wVirtualKeyCode = VK_END;
		inputRecords[currentPos].Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_END, MAPVK_VK_TO_VSC);
		currentPos++;

		inputRecords[currentPos] = inputRecords[currentPos - 1];
		inputRecords[currentPos].Event.KeyEvent.bKeyDown = FALSE;
		currentPos++;
		for (size_t i = 0; i < len; ++i) {
			inputRecords[currentPos].EventType = KEY_EVENT;
			inputRecords[currentPos].Event.KeyEvent.bKeyDown = TRUE;
			inputRecords[currentPos].Event.KeyEvent.dwControlKeyState = 0;
			inputRecords[currentPos].Event.KeyEvent.uChar.UnicodeChar = command[i];
			inputRecords[currentPos].Event.KeyEvent.wRepeatCount = 1;
			inputRecords[currentPos].Event.KeyEvent.wVirtualKeyCode = 0;
			inputRecords[currentPos].Event.KeyEvent.wVirtualScanCode = 0;
			currentPos++;
		}
		inputRecords[currentPos].EventType = KEY_EVENT;
		inputRecords[currentPos].Event.KeyEvent.bKeyDown = TRUE;
		inputRecords[currentPos].Event.KeyEvent.dwControlKeyState = 0;
		inputRecords[currentPos].Event.KeyEvent.uChar.UnicodeChar = '\r';
		inputRecords[currentPos].Event.KeyEvent.wRepeatCount = 1;
		inputRecords[currentPos].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
		inputRecords[currentPos].Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);
		currentPos++;

		inputRecords[currentPos] = inputRecords[currentPos - 1];
		inputRecords[currentPos].Event.KeyEvent.bKeyDown = FALSE;
		currentPos++;
		if (!SetConsoleCtrlHandler(IgnoreCtrlHandler, TRUE)) {
			MessageBoxA(NULL, "Failed to set control handler.", "Error", MB_ICONERROR | MB_OK);
			return;
		}
		while (!isTerminalReady() && IsProcessRunning(startedProcessIDsCMD)) {
			WriteConsoleInputW(hConsoleInput, clearRecords, 4, &eventsWritten);
			Sleep(100);
		}
		if (!WriteConsoleInputW(hConsoleInput, inputRecords, currentPos, &eventsWritten)) {

			std::cerr << "Failed to write to console input buffer." << std::endl;
		}
		if (!IsProcessRunning(startedProcessIDsCMD)) {
			commandQueue = {};
		}
	}
}
bool Application::isTerminalReady() {
	HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsoleOutput == INVALID_HANDLE_VALUE) {
		MessageBoxA(NULL, "Invalid console output handle.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(hConsoleOutput, &csbi)) {
		MessageBoxA(NULL, "GetConsoleScreenBufferInfo failed.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	const DWORD bufferSize = csbi.dwSize.X * 2;
	std::vector<WCHAR> buffer(bufferSize);
	DWORD charsRead = 0;

	COORD coord = { 0, csbi.dwCursorPosition.Y };
	DWORD consoleSize = csbi.dwSize.X; 

	if (!ReadConsoleOutputCharacterW(hConsoleOutput, buffer.data(), consoleSize, coord, &charsRead)) {
		MessageBoxA(NULL, "ReadConsoleOutputCharacterW failed.", "Error", MB_ICONERROR | MB_OK);
		return false;
	}

	std::wstring output(buffer.data(), charsRead);

	while (!output.empty() && (output.back() == L' ' || output.back() == L'\r' || output.back() == L'\n')) {
		output.pop_back();
	}

	bool isReady = !output.empty() && output.back() == L'>';

	return isReady;
}
BOOL WINAPI Application::IgnoreCtrlHandler(DWORD dwCtrlType) {
	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:
		std::cout << "Control-C event detected! Performing cleanup..." << std::endl;
		return TRUE;

	case CTRL_BREAK_EVENT:
		std::cout << "Control-Break event detected!" << std::endl;
		return TRUE;

	case CTRL_CLOSE_EVENT:
		std::cout << "Console window closing event detected!" << std::endl;
		return TRUE;

	case CTRL_LOGOFF_EVENT:
		std::cout << "User is logging off!" << std::endl;
		return TRUE;

	case CTRL_SHUTDOWN_EVENT:
		std::cout << "System is shutting down!" << std::endl;
		return TRUE;

	default:
		return FALSE;
	}
}
void Application::readFromSharedMemory()
{
	const wchar_t* SHARED_MEMORY_NAME = L"TTWMemory";
	const size_t SHARED_MEMORY_SIZE = 6800;
	HANDLE hMapFile = OpenFileMappingW(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		SHARED_MEMORY_NAME
	);

	if (hMapFile == NULL) {
		std::cerr << "Could not open file mapping object: " << GetLastError() << std::endl;
		return;
	}
	LPVOID pBuf = MapViewOfFile(
		hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		SHARED_MEMORY_SIZE
	);
	if (pBuf == NULL) {
		std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
		CloseHandle(hMapFile);
		return;
	}
	wchar_t* sharedData = static_cast<wchar_t*>(pBuf);
	if (wcslen(sharedData) > 0) {
		std::wstring data(sharedData);
		size_t delimiterPos = data.find(L'|');
		if (delimiterPos != std::wstring::npos) {
			std::wstring processIDStr = data.substr(0, delimiterPos);
			startedProcessIDsCMD = std::stoi(processIDStr);

			std::wstring command = data.substr(delimiterPos + 1);
			{
				std::lock_guard<std::mutex> lock(queueMutex);
				commandQueue.push(command);
				stopWorkingThread = false;
			}
			cv.notify_all();

			std::wcout << L"Read from shared memory: Process ID = " << startedProcessIDsCMD
				<< L", Command = " << command << std::endl;
		}
		else
		{
			std::wcerr << L"Invalid data format in shared memory." << std::endl;
		}
		ZeroMemory(pBuf, SHARED_MEMORY_SIZE);
	}

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}

int Application::Run()
{
	while (!stopWorkingThread)
	{
		readFromSharedMemory();
		Sleep(50);
	}
	if (workerThread.joinable())
		workerThread.join();
	return 0;
}