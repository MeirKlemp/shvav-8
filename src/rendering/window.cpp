#include "window.h"

#include <stdexcept>

namespace shvav8 {

Window& Window::create(const i32 width, const i32 height, const char* const title) {
    return create_impl(width, height, title);
}
Window& Window::get() { return create_impl(0, 0, 0, false); }

Window& Window::create_impl(const i32 width, const i32 height, const char* title,
                            const bool create) {
    GLFWwindow* window = nullptr;
    if (create) {
        if (!glfwInit()) {
            throw std::runtime_error{"Glfw failed to init"};
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef SHVAV8_MACOS
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window) {
            throw std::runtime_error{"Glfw failed to create window"};
        }

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(
            window, [](GLFWwindow* const window, const i32 width, const i32 height) {
                Window& w = get();
                for (const auto& callback : w.m_on_resize_callbacks) {
                    callback(width, height);
                }
            });
    }

    static Window instance(window);
    return instance;
}

Window::Window(GLFWwindow* window) : m_window(window) {
    if (window == nullptr) {
        throw std::runtime_error{"Window not initialized"};
    }
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

void Window::on_resize(std::function<void(i32 width, i32 height)> callback) {
    m_on_resize_callbacks.push_back(callback);
}

bool Window::should_close() const { return glfwWindowShouldClose(m_window); }
std::pair<i32, i32> Window::get_size() const {
    i32 width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return std::make_pair(width, height);
}

}  // namespace shvav8
