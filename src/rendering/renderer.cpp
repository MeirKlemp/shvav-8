#include "renderer.h"

namespace shvav8 {

Renderer& Renderer::create() { return create_impl(); }
Renderer& Renderer::get() { return create_impl(false); }

Renderer& Renderer::create_impl(bool create) {
    if (create) {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error{"Glad failed to init"};
        }
    }

    static Renderer instance;
    return instance;
}

Renderer::Renderer() {
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    VertexVector vertices = initialize_vertices();
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    /* Allocate element buffer once with max capacity */
    const usize max_size = INDICES_PER_SQUARE * FrameBuffer::PIXELS * sizeof(f32);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_size, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(0);

    /* Unbind all */
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Renderer::clear_screen(const f32 r, const f32 g, const f32 b, const f32 a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::set_viewport(const i32 x, const i32 y, const i32 width, const i32 height) {
    glViewport(x, y, width, height);
}

constexpr Renderer::VertexVector Renderer::initialize_vertices() {
    VertexVector vertices = {};
    for (u32 row = 0; row < VERTEX_ROWS; ++row) {
        for (u32 column = 0; column < VERTEX_COLUMNS; ++column) {
            u32 index = 2 * (VERTEX_COLUMNS * row + column);
            vertices[index] = -1 + 2 * SQUARE_WIDTH * column;
            vertices[index + 1] = -(-1 + 2 * SQUARE_HEIGHT * row);
        }
    }
    return vertices;
}

}  // namespace shvav8
