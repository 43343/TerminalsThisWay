#include "xlib_helper.h"
#include "main_xlib.h"
#include <iostream>
#include <cstdlib>
#include "ParameterInputWindow.h"

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


  while(true)
  {
    std::cout << "Event";
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
        
      }
    }
    if(inputWindow.isVisible())
    {
      std::cout << "govno";
      inputWindow.handleEvent(event);
    }
  }
}
