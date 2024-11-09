#include <windows.h>
#include <string>
#include "GUI/button.h"
#include "GUI/text.h"
#include <functional>

class ModalWindow
{
public:
	ModalWindow();
	~ModalWindow();
	bool createWindow(const std::string& currentVersion, const std::string& latestVersion);
private:
	static LRESULT CALLBACK ModalWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	HWND hwnd;
	GUI::Button* btnYes;
	GUI::Button* btnNo;
	GUI::Text* text;
};