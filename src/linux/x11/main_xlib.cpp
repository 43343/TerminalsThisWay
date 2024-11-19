#include "xlib_helper.h"
#include "main_xlib.h"
#include <iostream>
#include <cstdlib>
#include "ParameterInputWindow.h"
#include <thread>
#include <chrono>

void run_xlib()
{
  Display* display = XOpenDisplay(nullptr);
  if(!display)
  {
    std::cerr << "Failed to open display";
    exit(0);
  }
  ParameterInputWindow inputWindow(display);
  Window root = DefaultRootWindow(display);

  XGrabKey(display, XKeysymToKeycode(display, XK_Insert), Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);
  XGrabKey(display, XKeysymToKeycode(display, XK_Insert), 0, root, True, GrabModeAsync, GrabModeAsync);

  auto lastInsertPressTime = std::chrono::steady_clock::now();
  const auto doubleClickTreshhold = std::chrono::milliseconds(500);

  while(true)
  {
    while(XPending(display))
    {
      XEvent event;
      XNextEvent(display, &event);
      if(event.type == KeyPress)
      {
        KeySym keysym = XLookupKeysym(&event.xkey, 0);
        if((event.xkey.state & Mod4Mask) && keysym == XK_Insert)
        {
          if(!inputWindow.isVisible())
          {
            int x,y;
            Window returnedRoot, returnedChild;
            int rootX, rootY;
            unsigned int mask;
            XQueryPointer(display, root, &returnedRoot, &returnedChild, &rootX, &rootY, &x, &y, &mask);
            inputWindow.show(x,y);
          }
        } else if(keysym == XK_Insert)
        {
          auto now = std::chrono::steady_clock::now();
          if(now - lastInsertPressTime < doubleClickTreshhold)
          {
            if(!inputWindow.isVisible())
            {
              int x, y;
              Window returnedRoot, returnedChild;
              int rootX, rootY;
              unsigned int mask;
              XQueryPointer(display, root, &returnedRoot, &returnedChild, &rootX, &rootY, &x, &y, &mask);
              inputWindow.show(x,y);
            }
          }
          lastInsertPressTime = now;
        }
     }
      if(inputWindow.isVisible())
      {
        inputWindow.handleEvent(event);
      }
    }
    if(inputWindow.isVisible())
    {
      std::cout << "govno";
      inputWindow.update();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}
