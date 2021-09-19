#pragma once

#include <emulation/frame_buffer.h>
#include <glad/glad.h>
#include <rendering/window.h>

namespace shvav8 {

/**
 * OpenGL renderer using glad.
 * This class is a singleton.
 * Should be created before usage with create method.
 */
class Renderer {
   public:
    static Renderer& create();
    static Renderer& get();

    ~Renderer();
    void clear_screen(f32 r, f32 g, f32 b, f32 a = 1.0f);
    void set_viewport(i32 x, i32 y, i32 width, i32 height);

    template <typename Container>
    void draw_squares(const Container& squares, usize size) {
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

        std::vector<u32> indices(size * INDICES_PER_SQUARE);
        usize index = 0;
        for (usize i = 0; i < size; ++i) {
            u32 row = squares[i] / FrameBuffer::COLUMNS;
            u32 column = squares[i] % FrameBuffer::COLUMNS;
            u32 vertex_rows[] = {row, row + 1};
            u32 vertex_columns[] = {column, column + 1};

            /* first triangle */
            indices[index++] = VERTEX_COLUMNS * vertex_rows[0] + vertex_columns[0];
            indices[index++] = VERTEX_COLUMNS * vertex_rows[0] + vertex_columns[1];
            indices[index++] = VERTEX_COLUMNS * vertex_rows[1] + vertex_columns[0];
            /* second triangle */
            indices[index++] = VERTEX_COLUMNS * vertex_rows[1] + vertex_columns[0];
            indices[index++] = VERTEX_COLUMNS * vertex_rows[0] + vertex_columns[1];
            indices[index++] = VERTEX_COLUMNS * vertex_rows[1] + vertex_columns[1];
        }

        if (!indices.empty()) {
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(f32), &indices[0]);
        }
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

   private:
    Renderer();
    static Renderer& create_impl(bool create = true);

   private:
    u32 m_vao;
    u32 m_vbo;
    u32 m_ebo;

    constexpr static u32 VERTEX_COLUMNS = FrameBuffer::COLUMNS + 1;
    constexpr static u32 VERTEX_ROWS = FrameBuffer::ROWS + 1;
    constexpr static u32 NUM_VERTICES = VERTEX_COLUMNS * VERTEX_ROWS;
    constexpr static u8 INDICES_PER_SQUARE = 3 * 2;
    constexpr static f32 SQUARE_WIDTH = 1.0f / FrameBuffer::COLUMNS;
    constexpr static f32 SQUARE_HEIGHT = 1.0f / FrameBuffer::ROWS;

    using VertexVector = std::array<f32, 2 * NUM_VERTICES>;
    constexpr static VertexVector initialize_vertices();
};

}  // namespace shvav8
