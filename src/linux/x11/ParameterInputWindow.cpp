#include "ParameterInputWindow.h"
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>

ParameterInputWindow::ParameterInputWindow(Display* display) : display(display), visible(false)
{
  screen = DefaultScreen(display);
  Window root = RootWindow(display,screen);
  window = XCreateSimpleWindow(display, root, 0, 20, 200, 20, 1, BlackPixel(display,screen), WhitePixel(display, screen));
  XStoreName(display, window , "Input Window");
  XSelectInput(display, window, ExposureMask | KeyPressMask );
  gc = XCreateGC(display, window, 0, nullptr);
  XSetForeground(display, gc, BlackPixel(display, screen));
  Cursor textCursor = XCreateFontCursor(display, XC_xterm);

  XSizeHints* size_hints = XAllocSizeHints();
  if(size_hints != nullptr)
  {
    size_hints->flags = PMinSize | PMaxSize;
    size_hints->min_width = size_hints->max_width = 200;
    size_hints->min_height = size_hints->max_height = 20;
    XSetWMNormalHints(display, window, size_hints);
    XFree(size_hints);
  }
  lastBlinkTime = std::chrono::steady_clock::now();
  xim = XOpenIM(display, nullptr, nullptr, nullptr);
  if(xim != nullptr)
  {
    xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, window, nullptr);
  }
  enterNotifyCursor = textCursor;
  backBuffer = XCreatePixmap(display, window, 200, 20, DefaultDepth(display, screen));
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
  XSetForeground(display, gc, WhitePixel(display,screen));
  XFillRectangle(display, backBuffer, gc, 0, 0, 200, 20);

  int startX = 10;
  int endX = startX + XTextWidth(XQueryFont(display, XGContextFromGC(gc)), inputText.c_str(), inputText.length());
  Colormap colormap = DefaultColormap(display,screen);
  XColor blueColor;
  if(!XAllocNamedColor(display, colormap, "blue", &blueColor, &blueColor))
  {
    return;
  }

  if(selectionStart != selectionEnd)
  {
    int selStartX = startX + std::min(selectionStart, selectionEnd);
    int selEndX = startX + std::max(selectionStart, selectionEnd);

    XSetForeground(display, gc, blueColor.pixel);
    XFillRectangle(display, window, gc, selStartX, 5, selEndX - selStartX, 15);
    XSetForeground(display, gc, WhitePixel(display, screen));
  }
  else 
  {
    XSetForeground(display, gc, BlackPixel(display, screen));
  }

  XDrawString(display, window, gc, 10, 15, inputText.c_str(), inputText.length());
  if(cursorVisible)
  {
    int cursorX = 10 + XTextWidth(XQueryFont(display, XGContextFromGC(gc)), inputText.c_str(), inputText.length());
    XDrawLine(display, window, gc, cursorX, 5, cursorX, 15);
  }
  XCopyArea(display, backBuffer, window, gc, 0, 0, 200, 20, 0, 0);
}
void ParameterInputWindow::update()
{
  auto now = std::chrono::steady_clock::now();
  if(std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBlinkTime) >= blinkInterval)
  {
    cursorVisible = !cursorVisible;
    handleExpose();
    lastBlinkTime = now;
  }

}
void ParameterInputWindow::handleEvent(XEvent& event)
{
  switch(event.type)
  {
    case Expose:
      handleExpose();
      break;
    case KeyPress:
      handleKeyPress(event);
      break;
    case EnterNotify:
      XDefineCursor(display, window, enterNotifyCursor);
      break;
    case LeaveNotify:
      XUndefineCursor(display, window);
      break;
    case ButtonPress:
      handleButtonPress(event);
      break;
    case ButtonRelease:
      handleButtonRelease(event);
      break;
    case MotionNotify:
      handleMotionNotify(event);
      break;
    case SelectionRequest:
      handleSelectionRequest(event);
      break;
    define:
      break;
  }
}
void ParameterInputWindow::handleKeyPress(XEvent& event)
{
  char buffer[32];
  KeySym keysym;
  Status status;
  int len = Xutf8LookupString(xic, &event.xkey, buffer, sizeof(buffer) -1, &keysym, &status);
  if((event.xkey.state & ControlMask) && keysym == XK_C)
  {
    copyToClipboard();
  }
  else if((event.xkey.state & ControlMask) && keysym == XK_V)
  {
    pasteFromClipboard();
  }
  else if(len > 0)
  {
    buffer[len] = '0';
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
void ParameterInputWindow::handleButtonPress(XEvent& event)
{
  if(event.xbutton.button == Button1)
  {
    selectionStart = event.xbutton.x - 10;
    selectionEnd = selectionStart;
    selecting = true;
  }
}
void ParameterInputWindow::handleButtonRelease(XEvent& event)
{
  if(event.xbutton.button == Button1 && selecting)
  {
    selecting = false;
    selectionEnd = event.xbutton.x - 10;
    handleExpose();
  }
}
void ParameterInputWindow::handleMotionNotify(XEvent& event)
{
  if(selecting)
  {
    selectionEnd = event.xbutton.x;
    handleExpose();
  }
}
void ParameterInputWindow::copyToClipboard()
{
  if(selectionStart != selectionEnd)
  {
    int start = std::min(selectionStart, selectionEnd);
    int end = std::max(selectionStart, selectionEnd);

    start = start / XTextWidth(XQueryFont(display, XGContextFromGC(gc)), " ", 1);
    end = end / XTextWidth(XQueryFont(display, XGContextFromGC(gc)), " ", 1);

    std::string selectedText = inputText.substr(start, end - start);

    Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
    XSetSelectionOwner(display, clipboard, window, CurrentTime);

    clipboardText = selectedText;

  }
}
void ParameterInputWindow::pasteFromClipboard()
{
  Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
  Atom utf8_string = XInternAtom(display, "CLIPBOARD", False);
  XConvertSelection(display, clipboard, utf8_string, XA_PRIMARY, window, CurrentTime);
}
void ParameterInputWindow::handleSelectionRequest(XEvent& event)
{
  if(event.xselectionrequest.selection == XA_PRIMARY || event.xselectionrequest.selection == XInternAtom(display, "CLIPBOARD", False))
  {
    XSelectionRequestEvent* req = &event.xselectionrequest;
    XSelectionEvent ev = {0};
    ev.type = SelectionNotify;
    ev.display = req->display;
    ev.requestor = req->requestor;
    ev.selection = req->selection;
    ev.time = req->time;
    ev.target = req->target;
    ev.property = None;

    if(req->target == XA_STRING || req->target == XInternAtom(display, "CLIPBOARD", False))
    {
      XChangeProperty(req->display, req->requestor, req->property, req->target, 8,
          PropModeReplace, (unsigned char*)clipboardText.c_str(), clipboardText.length());
      ev.property = req->property;
    }
    XSendEvent(display, req->requestor, True, 0, (XEvent*)&ev);
  }
}
