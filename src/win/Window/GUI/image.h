#include <windows.h>
#include <string>

namespace GUI
{
	class Image
	{
	public:
		Image(HINSTANCE hInstance, HWND parentHwnd, int x, int y, int width, int height, std::string resourceId);
		~Image();
	private:
		HWND hwnd;

	};
}
