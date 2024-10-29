#pragma once 
#include <unordered_map>
#include <windows.h>
#include <string>

DWORD stringTokey(const std::string& key);
bool runAsAdministrator();
bool isRunningAsAdministrator();
std::string keyToString(DWORD keyCode);
