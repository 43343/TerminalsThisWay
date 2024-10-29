#include <windows.h>
#include <string>
#include "GUI/button.h"
#include "GUI/text.h"
#include <functional>

class ModalWindow
{
public:
	ModalWindow(HINSTANCE hInstance);
	~ModalWindow();
	void createWindow(const std::string& currentVersion, const std::string& latestVersion, std::function<void()> cb);
private:
	static LRESULT CALLBACK ModalWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	HINSTANCE hInstance;
	HWND hwnd;
	GUI::Button* btnYes;
	GUI::Button* btnNo;
	GUI::Text* text;
};