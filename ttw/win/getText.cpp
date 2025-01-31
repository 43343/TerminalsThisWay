#include "getText.h"
#include <iostream>
#include <algorithm>
#include "Config/configManager.h"

GetText::GetText() {}

std::wstring GetText::GetClipboardText() {
	if (!OpenClipboard(nullptr)) {
		return L""; 
	}

	HANDLE hData = GetClipboardData(CF_UNICODETEXT); 
	if (hData == nullptr) {
		CloseClipboard();
		return L""; 
	}

	wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData)); 
	if (pszText == nullptr) {
		CloseClipboard();
		return L""; 
	}

	GlobalUnlock(hData); 
	CloseClipboard();    

	return std::wstring(pszText);
}
void GetText::SetClipboardText(const std::wstring& text) {
	if (!OpenClipboard(nullptr)) {
		return; 
	}

	EmptyClipboard();

	
	size_t sizeInBytes = (text.size() + 1) * sizeof(wchar_t);

	
	HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, sizeInBytes);
	if (hGlob != nullptr) {
		void* pGlobMem = GlobalLock(hGlob); 
		if (pGlobMem != nullptr) {
			memcpy(pGlobMem, text.c_str(), sizeInBytes); 
			GlobalUnlock(hGlob);                        
			SetClipboardData(CF_UNICODETEXT, hGlob);    
		}
		else {
			GlobalFree(hGlob); 
		}
	}

	CloseClipboard();
}
void GetText::SimulateCtrlC() {
	Simulating::SetSimulating(true);

	INPUT releaseShift[2] = {};
	releaseShift[0].type = INPUT_KEYBOARD;
	releaseShift[0].ki.wVk = VK_LSHIFT;
	releaseShift[0].ki.dwFlags = KEYEVENTF_KEYUP;

	releaseShift[1].type = INPUT_KEYBOARD;
	releaseShift[1].ki.wVk = VK_RSHIFT;
	releaseShift[1].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(2, releaseShift, sizeof(INPUT));

	INPUT inputs[4] = {};
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_CONTROL;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = 'C';

	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.wVk = 'C';
	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

	inputs[3].type = INPUT_KEYBOARD;
	inputs[3].ki.wVk = VK_CONTROL;
	inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(4, inputs, sizeof(INPUT));

	Simulating::SetSimulating(false);
}
std::wstring GetText::getSelectedText()
{
	std::wstring originalClipboardContent = GetClipboardText();
	if (OpenClipboard(nullptr)) {
		EmptyClipboard();
		CloseClipboard();
	}
	SimulateCtrlC(); 
	Sleep(100);
	 std::wstring selectedText = GetClipboardText();
	if (selectedText.empty()) {
		SetClipboardText(originalClipboardContent);
		return L"";
	}
	size_t start = selectedText.find_first_not_of(L" \t\r\n");
	size_t end = selectedText.find_last_not_of(L" \t\r\n");

	if (start != std::wstring::npos && end != std::wstring::npos) {
		selectedText = selectedText.substr(start, end - start + 1);
	}
	else if (start != std::wstring::npos) {
		selectedText = selectedText.substr(start);
	}
	else {
		selectedText = L"";
	}
	selectedText.erase(std::remove_if(selectedText.begin(), selectedText.end(), [](wchar_t c) {
		return c < 32 || c == 127;
		}), selectedText.end());
	if (selectedText.length() > 3276) {
		selectedText = selectedText.substr(0, 3276);
	}
	SetClipboardText(originalClipboardContent);
	return selectedText;
}