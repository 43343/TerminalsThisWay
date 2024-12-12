#include "ParameterInputWindow.h"
#include <gtk/gtk.h>

ParameterInputWindow::ParameterInputWindow()
{
}
ParameterInputWindow::~ParameterInputWindow()
{

}
void ParameterInputWindow::create(int x, int y)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_x = x;
        m_y = y;
        if (m_running) {
            m_condition.notify_one();
            return;
        }
        m_running = true;
    }
    m_thread = std::thread([this]() {
        while (m_running)
        {
            int x, y;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                x = m_x;
                y = m_y;
            }
            window(x, y);
            std::unique_lock<std::mutex> lock(m_mutex);
            if (!m_running) break;
            m_condition.wait(lock);
        }
        });


}
void ParameterInputWindow::destroy()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_running) return;
        m_running = false;
        m_condition.notify_one();
    }
    if (m_thread.joinable())
    {
        m_thread.join();
    }


}
void ParameterInputWindow::window(int x, int y)
{
    gtk_init(nullptr, nullptr);

    GtkWidget* window = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 20);
    gtk_window_move(GTK_WINDOW(window), x, y);
    gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
    gtk_window_set_focus_on_map(GTK_WINDOW(window), TRUE);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window), TRUE);
    gtk_window_set_skip_pager_hint(GTK_WINDOW(window), TRUE);

    GtkWidget* entry = gtk_entry_new();
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_widget_set_vexpand(entry, TRUE);

    GtkWidget* container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(container), entry);
    gtk_container_add(GTK_CONTAINER(window), container);

    GtkCssProvider* cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(cssProvider, "entry {background-color:white; color:black}", -1, nullptr);

    GtkStyleContext* context = gtk_widget_get_style_context(entry);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_grab_focus(entry);
    gtk_widget_show_all(window);
    gtk_main();
}
bool ParameterInputWindow::isVisible()
{
    return m_visible;
}