#pragma once

#include <GLFW/glfw3.h>
#include <defines.h>

#include <optional>

namespace shvav8 {

/**
 * Wrapper class on glfw window.
 * Only one window can be created or else glfwTerminate will be called twice on destruction
 */
class Window {
   public:
    static std::optional<Window> create(i32 width, i32 height, const char* title);
    ~Window();

    void update();
    bool should_close() const;

    Window(Window&& rhs);

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

   private:
    Window(GLFWwindow* window);

   private:
    GLFWwindow* m_window;
};

}  // namespace shvav8