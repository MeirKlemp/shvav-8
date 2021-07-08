#include "window.h"

namespace shvav8 {

std::optional<Window> Window::create(const i32 width, const i32 height, const char* const title) {
    if (!glfwInit()) {
        return std::nullopt;
    }

    auto* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        return std::nullopt;
    }

    glfwMakeContextCurrent(window);

    return Window(window);
}

Window::Window(GLFWwindow* window) : m_window(window) {}

Window::Window(Window&& rhs) : m_window(rhs.m_window) {
    rhs.m_window = nullptr;
}

Window::~Window() {
    if (m_window) {
        glfwTerminate();
    }
}

void Window::update() {
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

bool Window::should_close() const { return glfwWindowShouldClose(m_window); }

}  // namespace shvav8