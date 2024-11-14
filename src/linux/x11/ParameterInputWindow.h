#pragma once
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <iostream>
#include <cstring>
//#include <chrono>

class ParameterInputWindow
{
public:
  ParameterInputWindow(Display* display);
  ~ParameterInputWindow();
  void show(int x, int y);
  void hide();
  void handleEvent(XEvent& event);
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

  //bool cursorVisible = true;
  //std::chrono::steady_clock::time_point lastBlinkTime;
  //const std::chrono::milliseconds blinkInterval{500};
};

