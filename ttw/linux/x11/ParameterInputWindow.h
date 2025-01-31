#pragma once
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class ParameterInputWindow
{
public:
    ParameterInputWindow();
    ~ParameterInputWindow();
    void create(int x, int y);
    void destroy();
    bool isVisible();
private:
    bool m_visible = false;
    std::thread m_thread;
    std::atomic<bool> m_running;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    int m_x;
    int m_y;
    void window(int x, int y);
};
