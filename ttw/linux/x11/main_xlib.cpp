#include "xlib_helper.h"
#include "main_xlib.h"
#include <iostream>
#include <cstdlib>
#include "ParameterInputWindow.h"
#include "../Terminal.h"
#include <thread>
#include <chrono>
#include "../log.h"

unsigned int hotkey_keycode = 0;
unsigned int hotkey_modmask = 0;
Terminal *terminal;

void event_callback(XPointer priv, XRecordInterceptData *recorded_data)
{
  if(recorded_data->category != XRecordFromServer || recorded_data->data_len == 0)
  {
    write_log("Incorrect data XRecord");
    XRecordFreeData(recorded_data);
    return;
  }
  write_log("Event callback");  
  XEvent *event = reinterpret_cast<XEvent *>(recorded_data->data);
  int actualType = event->type & 0xFF;
  write_log("EventType, type: " + std::to_string(actualType));
  if(actualType == KeyPress)
  {
    XKeyEvent *keyEvent = reinterpret_cast<XKeyEvent*>(event);
write_log("KeyPress, state: " + std::to_string((keyEvent->state & 0xFF) & ControlMask));
    Display* dpy = XOpenDisplay(NULL);
    if(dpy)
    {
      char keys[32];
      XQueryKeymap(dpy, keys);

      KeyCode leftCtrl = XKeysymToKeycode(dpy, XK_Control_L);
      KeyCode rightCtrl = XKeysymToKeycode(dpy, XK_Control_R);

      bool controlPressed = false;
      if(leftCtrl != 0)
      {
        controlPressed |= (keys[leftCtrl / 8] & (1 << (leftCtrl % 8))) != 0;
      }
      if(rightCtrl != 0)
      {
        controlPressed |= (keys[rightCtrl / 8] & (1 << (rightCtrl & 8))) != 0;
      }
      if(controlPressed)
      {
        write_log("hotkey");
        terminal->sendCommandToCmd("govno", true);
      }
    }
    XCloseDisplay(dpy);
  }
  if(event->type == KeyRelease)
    write_log("KeyRelease event");  
  XRecordFreeData(recorded_data);
}



void run_xlib()
{
    clear_log();
    write_log("Started...");
    terminal = new Terminal();
    char *display_name = XDisplayName(nullptr);
    Display* display = XOpenDisplay(display_name);
    if (!display)
    {
        write_log("Failed to open display");
        exit(0);
    }
    Display* display_data = XOpenDisplay(display_name);
    if(!display_data)
    {
      write_log("Failed to open display data");
      exit(0);
    }
    //Window root = DefaultRootWindow(display);
    char* mdifiers_str = "Control+Shift";

    int major, minor;
    if(!XRecordQueryVersion(display, &major, &minor))
    {
      write_log("XRecordExtension is undefined");
      exit(0);
    }
    std::cout << "XRecord version " << major << "." << minor << " used";

    XRecordRange *recordRange = XRecordAllocRange();
    if(!recordRange)
    {
      write_log("Unable to allocate XRecordRange");
      exit(0);
    }
    recordRange->device_events.first = KeyPress;
    recordRange->device_events.last = KeyPress;

    XRecordClientSpec clientSpec = XRecordAllClients;

    XRecordContext context = XRecordCreateContext (display, 0, &clientSpec, 1, &recordRange, 1);
    if(!context)
    {
      write_log("Failed to create XRecord context");
      exit(0);
    }
    XSync(display, False);
    XFree(recordRange);

    if(!XRecordEnableContext(display_data, context, event_callback, nullptr))
    {
      write_log("Failed to enable XRecord context");
      exit(0);
    }

    //while(true) {}

    //auto lastInsertPressTime = std::chrono::steady_clock::now();
   // const auto doubleClickTreshhold = std::chrono::milliseconds(500);

    /*while (true)
    {
        while (XPending(display))
        {
            XEvent event;
            XNextEvent(display, &event);
            if (event.type == KeyPress)
            {
                KeySym keysym = XLookupKeysym(&event.xkey, 0);
                if ((event.xkey.state & Mod4Mask) && keysym == XK_Insert)
                {
                    terminal->sendCommandToCmd("cd", true);
                }
                else if (keysym == XK_Insert)
                {
                    auto now = std::chrono::steady_clock::now();
                    if (now - lastInsertPressTime < doubleClickTreshhold)
                    {
                        int x, y;
                        Window returnedRoot, returnedChild;
                        int rootX, rootY;
                        unsigned int mask;
                        XQueryPointer(display, root, &returnedRoot, &returnedChild, &rootX, &rootY, &x, &y, &mask);
                        inputWindow.create(x, y);
                        XGrabKey(display, XKeysymToKeycode(display, XK_Return), 0, root, True, GrabModeAsync, GrabModeAsync);
                    }
                    lastInsertPressTime = now;
                }
                else if (keysym = XK_Return)
                {
                    inputWindow.destroy();
                    XUngrabKey(display, XKeysymToKeycode(display, XK_Return), 0, root);
                }
            }
        }
    }*/ 

}
