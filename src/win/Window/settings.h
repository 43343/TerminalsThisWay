#pragma once
#include <windows.h>
#include <string>
#include "GUI/inputField.h"
#include "GUI/toggle.h"
#include "GUI/inputKeyBind.h"
#include "GUI/text.h"
#include "GUI/button.h"

class Settings {
public:
    Settings(HINSTANCE hInstance);
    ~Settings();

    bool createWindow();

    HWND hwnd;
    void setText();
    void setInput();
private:
    HINSTANCE hInstance;

   
    GUI::Text* generalText;
    
    GUI::Text* settingsText;

    GUI::Text* pathToTerminalText;
    GUI::InputField* pathToTerminalInput;

    GUI::Text* launchByDefaultText;
    GUI::Toggle* launchByDefaultToggle;

    GUI::Text* runAsAdministratorText;
    GUI::Toggle* runAsAdministratorToggle;

    GUI::Text* keyAssignmentText;

    GUI::Text* sendCommandText;
    GUI::InputKeyBind* sendCommandInput;

    GUI::Text* sendCommandParameterText;
    GUI::InputKeyBind* sendCommandParameterInput;

    GUI::Text* chooseFolderText;
    GUI::InputKeyBind* chooseFolderInput;

    GUI::Text* warningText;

    GUI::Button* saveBtn;
    GUI::Button* cancelBtn;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};