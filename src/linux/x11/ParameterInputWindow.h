#pragma once
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xlocale.h>
#include <iostream>
#include <cstring>
#include <chrono>

class ParameterInputWindow
{
public:
  ParameterInputWindow(Display* display);
  ~ParameterInputWindow();
  void show(int x, int y);
  void hide();
  void handleEvent(XEvent& event);
  void update();
  bool isVisible() const;
private:
  Display* display;
  int screen;
  Window window;
  GC gc;
  std::string inputText;
  bool visible;
  
  void handleExpose();
  void handleKeyPress(XEvent& event);
  void handleButtonPress(XEvent& event);
  void handleButtonRelease(XEvent& event);
  void handleMotionNotify(XEvent& event);
  void handleSelectionRequest(XEvent& event);
  void copyToClipboard();
  void pasteFromClipboard();


  XIM xim;
  XIC xic;

  Cursor enterNotifyCursor;

  bool cursorVisible = true;
  std::chrono::steady_clock::time_point lastBlinkTime;
  const std::chrono::milliseconds blinkInterval{500};

  int selectionStart = 0;
  int selectionEnd = 0;
  bool selecting = false;
  std::string clipboardText;
  Pixmap backBuffer;
};

