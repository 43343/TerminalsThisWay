#include "ParameterInputWindow.h"
#include <X11/Xutil.h>

ParameterInputWindow::ParameterInputWindow(Display* display) : display(display), visible(false)
{
  screen = DefaultScreen(display);
  Window root = RootWindow(display,screen);
  window = XCreateSimpleWindow(display, root, 0, 20, 200, 20, 1, BlackPixel(display,screen), WhitePixel(display, screen));
  XStoreName(display, window , "Input Window");
  XSelectInput(display, window, ExposureMask | KeyPressMask );
  gc = XCreateGC(display, window, 0, nullptr);
  XSetForeground(display, gc, BlackPixel(display, screen));

  XSizeHints* size_hints = XAllocSizeHints();
  if(size_hints != nullptr)
  {
    size_hints->flags = PMinSize | PMaxSize;
    size_hints->min_width = size_hints->max_width = 200;
    size_hints->min_height = size_hints->max_height = 20;
    XSetWMNormalHints(display, window, size_hints);
    XFree(size_hints);
  }
  //lastBlinkTime = std::chrono::steady_clock::now();
}
ParameterInputWindow::~ParameterInputWindow()
{
  XFreeGC(display, gc);
  XDestroyWindow(display, window);
}
void ParameterInputWindow::show(int x, int y)
{
  XMoveWindow(display, window, x, y);
  XMapWindow(display, window);
  visible = true;
}
void ParameterInputWindow::hide()
{
  XUnmapWindow(display, window);
  visible = false;
}
bool ParameterInputWindow::isVisible() const
{
  return visible;
}
void ParameterInputWindow::handleExpose()
{
  XClearWindow(display, window);
  XDrawString(display, window, gc, 10, 15, inputText.c_str(), inputText.length());
  //if(cursorVisible)
 // {
  //  int cursorX = 10 + XTextWidth(XQueryFont(display, XGContextFromGC(gc)), inputText.c_str(), inputText.length());
  //  XDrawLine(display, window, gc, cursorX, 5, cursorX, 15);
 // }
}
void ParameterInputWindow::handleEvent(XEvent& event)
{
  if(event.type == Expose)
  {
    handleExpose();
  }
  else if (event.type == KeyPress)
  {
    handleKeyPress(event);
  }
 // auto now = std::chrono::steady_clock::now();
  //if(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBlinkTime) >= blinkInterval)
 // {
 //   cursorVisible = !cursorVisible;
 //   handleExpose();
 //   lastBlinkTime = now;
//  }
}
void ParameterInputWindow::handleKeyPress(XEvent& event)
{
  char buffer[32];
  KeySym keysym;
  int len = XLookupString(&event.xkey, buffer, sizeof(buffer), &keysym, nullptr);
  if(len > 0)
  {
    if(buffer[0] == '\b')
    {
      if(!inputText.empty())
      {
        inputText.pop_back();
      }
    }
    else if(buffer[0] == '\r')
    {
      hide();
    }
    else {
      inputText += std::string(buffer, len);
    }
    handleExpose();
  }

}
