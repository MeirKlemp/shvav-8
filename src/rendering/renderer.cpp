#include "renderer.h"

#include <iostream>

namespace shvav8 {

Renderer& Renderer::create() { return create_impl(); }
Renderer& Renderer::get() { return create_impl(false); }

Renderer& Renderer::create_impl(bool create) {
    if (create) {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error{"Glad failed to init"};
        }

        Window& window = Window::get();
        window.on_resize(
            [](const i32 width, const i32 height) { glViewport(0, 0, width, height); });
    }
    static Renderer instance;
    return instance;
}

void Renderer::clear_screen(const f32 r, const f32 g, const f32 b, const f32 a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

}  // namespace shvav8
