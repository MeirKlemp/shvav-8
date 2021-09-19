#pragma once

#include <GLFW/glfw3.h>
#include <defines.h>

#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

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

    void update();
    void title(const std::string& title);
    std::string title() const;
    void on_resize(std::function<void(i32 width, i32 height)> callback);
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
};

}  // namespace shvav8
