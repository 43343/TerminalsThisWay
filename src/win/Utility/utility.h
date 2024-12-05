#pragma once 
#include <unordered_map>
#include <windows.h>
#include <string>

DWORD stringTokey(const std::wstring& key);
bool runAsAdministrator();
bool isRunningAsAdministrator();
std::wstring keyToString(DWORD keyCode);
