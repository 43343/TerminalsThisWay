#pragma once
#include <string>
#include <unordered_map>

class Localization
{
public:
	explicit Localization(std::string lang);
	std::string getText(const std::string& text);
private:
	std::unordered_map<std::string, std::string> langMap;
};