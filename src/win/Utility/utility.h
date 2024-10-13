#pragma once 
#include <unordered_map>
#include <windows.h>
#include <string>

int stringTokey(const std::string& key);
bool runAsAdministrator();
bool isRunningAsAdministrator();
