#include "window.h"

namespace shvav8 {

Window& Window::create(const i32 width, const i32 height, const std::string& title) {
    return create_impl(width, height, title);
}
Window& Window::get() { return create_impl(0, 0, "", false); }

Window& Window::create_impl(const i32 width, const i32 height, const std::string& title,
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

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!window) {
            throw std::runtime_error{"Glfw failed to create window"};
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        /* Callbacks */
        glfwSetFramebufferSizeCallback(
            window, [](GLFWwindow* const window, const i32 width, const i32 height) {
                Window& w = get();
                for (const auto& callback : w.m_on_resize_callbacks) {
                    callback(width, height);
                }
            });
        glfwSetKeyCallback(window, [](GLFWwindow* const window, const i32 key, const i32 scancode,
                                      const i32 action, const i32 mods) {
            Window& w = get();
            for (const auto& callback : w.m_on_key_event_callbacks) {
                callback(key, action);
            }
        });
    }

    static Window instance(window, title);
    return instance;
}

Window::Window(GLFWwindow* window, const std::string& title) : m_window(window), m_title(title) {
    if (window == nullptr) {
        throw std::logic_error{"Window not initialized"};
    }
}

Window::~Window() { glfwTerminate(); }

void Window::poll_events() { glfwPollEvents(); }

void Window::swap_buffers() { glfwSwapBuffers(m_window); }

void Window::title(const std::string& title) {
    glfwSetWindowTitle(m_window, title.c_str());
    m_title = title;
}

std::string Window::title() const { return m_title; }

void Window::on_resize(std::function<void(i32 width, i32 height)> callback) {
    m_on_resize_callbacks.push_back(callback);
}

void Window::on_key_event(std::function<void(i32 keycode, i32 action)> callback) {
    m_on_key_event_callbacks.push_back(callback);
}

bool Window::should_close() const { return glfwWindowShouldClose(m_window); }
std::pair<i32, i32> Window::get_size() const {
    i32 width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return std::make_pair(width, height);
}

}  // namespace shvav8
