#include "application.h"
#include <shlobj.h>
#include <iostream>
#include <string>
#include <tlhelp32.h>
#include <fstream>

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
	if (!isValidToken(argv[1]))
	{
		stopWorkingThread = true;
		exit(0);
		return;
	}
	SetConsoleOutputCP(65001);
	SetConsoleCP(65001);
	workerThread = std::thread(&Application::commandProcessingLoop, this);
	char appDataPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath)))
	{
		std::string newFolderPath = std::string(appDataPath) + "\\TerminalsThisWay";
		if (!SetCurrentDirectory((LPTSTR)newFolderPath.c_str())) {
			std::cerr << "Failed to set current directory. Error: " << GetLastError() << std::endl;
		}
	}
	readFromSharedMemory();
}

bool Application::isValidToken(const std::string& token)
{
	std::ifstream infile("temp.txt");
	if (!infile.is_open()) {
		return false;
	}
	std::string line;
	std::getline(infile, line);
	infile.close();
	if (std::remove("temp.txt") == 0)
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
			Sleep(100);
		      

			
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
		//stopWorkingThread = true;
		return false;
	}
	WCHAR buffer[4096];
	DWORD charsRead = 0;

	COORD coord = { 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(hConsoleOutput, &csbi)) {
		MessageBoxA(NULL, "GetConsoleScreenBufferInfo.", "Error", MB_ICONERROR | MB_OK);
		//stopWorkingThread = true;
		return false;
	}

	DWORD consoleSize = std::min<DWORD>(csbi.dwSize.X * csbi.dwSize.Y, 4096);

	if (!ReadConsoleOutputCharacterW(hConsoleOutput, buffer, consoleSize, coord, &charsRead)) {
		MessageBoxA(NULL, "ReadConsoleOutputCharacterW.", "Error", MB_ICONERROR | MB_OK);
		//stopWorkingThread = true;
		return false;
	}
	std::wstring output(buffer, charsRead);
	return output.find(L">") != std::wstring::npos;
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
	const size_t SHARED_MEMORY_SIZE = 4000;
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