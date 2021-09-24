#pragma once

#include <GLFW/glfw3.h>

#include <functional>
#include <string>
#include <vector>

#include "defines.h"

#define SHVAV8_KEY(keyname) GLFW_KEY_##keyname
#define SHVAV8_ACTION(action) GLFW_##action

namespace shvav8 {

/**
 * Wrapper class of glfw window.
 * This class is a singleton.
 * Should be created before usage with create method.
 */
class Window {
   public:
    static Window& create(i32 width, i32 height, const std::string& title);
    static Window& get();
    ~Window();

    void poll_events();
    void swap_buffers();
    void title(const std::string& title);
    std::string title() const;
    void on_resize(std::function<void(i32 width, i32 height)> callback);
    void on_key_event(std::function<void(i32 keycode, i32 action)> callback);
    bool should_close() const;
    std::pair<i32, i32> get_size() const;

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

   private:
    Window(GLFWwindow* window, const std::string& title);
    static Window& create_impl(i32 width, i32 height, const std::string& title, bool create = true);

   private:
    GLFWwindow* m_window;
    std::string m_title;
    std::vector<std::function<void(i32 width, i32 height)>> m_on_resize_callbacks;
    std::vector<std::function<void(i32 keycode, i32 action)>> m_on_key_event_callbacks;
};

}  // namespace shvav8
