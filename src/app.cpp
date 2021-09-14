#include "app.h"

#include <emulation/exceptions.h>
#include <emulation/shvav8.h>
#include <rendering/renderer.h>
#include <rendering/window.h>

#include <ctime>
#include <fstream>
#include <iostream>

namespace shvav8 {

App::App(const char* rom_path) {
    auto& window = Window::create(640, 480, "Shvav-8");
    auto& renderer = Renderer::create();

    /* Vertex Shader */
    const char* vertex_shader_source =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}";
    const u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    /* Fragment Shader */
    const char* fragment_shader_source =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}";
    const u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    /* Shader Program */
    const u32 shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glUseProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    /* Display Verticies */
    const i32 num_vertex_columns = 65;
    const i32 num_vertex_rows = 33;
    const f32 square_width = 0.015625f;  // 1 / 64
    const f32 square_height = 0.03125;   // 1 / 32

    f32 vertices[2 * num_vertex_rows * num_vertex_columns];  // every vertex has 2 floats (x, y)
    for (i32 row = 0; row < num_vertex_rows; ++row) {
        for (i32 column = 0; column < num_vertex_columns; ++column) {
            i32 index = 2 * (num_vertex_columns * row + column);
            vertices[index] = -1 + 2 * square_width * column;
            vertices[index + 1] = -(-1 + 2 * square_height * row);
        }
    }

    /* Vertex Array */
    u32 vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    /* Vertex Buffer */
    u32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /* Element Buffer */
    u32 ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::ifstream rom(rom_path, std::ios::binary);
    if (!rom) {
        std::cerr << "Cannot open rom at " << rom_path << std::endl;
        exit(2);
    }

    u8 memory[0xDFF];
    rom.read((char*)memory, sizeof(memory));

    shvav8::FrameBuffer display;
    shvav8::Shvav8 interpreter(display);
    interpreter.load(memory);

    try {
        /* Loop until the user closes the window */
        while (!window.should_close()) {
            interpreter.next();

            std::vector<u32> indices;

            for (u32 row = 0; row < FrameBuffer::ROWS; ++row) {
                for (u32 column = 0; column < FrameBuffer::COLUMNS; ++column) {
                    if (display.is_drawn(column, row)) {
                        u32 vertex_row_1 = row;
                        u32 vertex_row_2 = row + 1;
                        u32 vertex_column_1 = column;
                        u32 vertex_column_2 = column + 1;

                        // first triangle
                        indices.push_back(num_vertex_columns * vertex_row_1 + vertex_column_1);
                        indices.push_back(num_vertex_columns * vertex_row_1 + vertex_column_2);
                        indices.push_back(num_vertex_columns * vertex_row_2 + vertex_column_1);
                        // second triangle
                        indices.push_back(num_vertex_columns * vertex_row_2 + vertex_column_1);
                        indices.push_back(num_vertex_columns * vertex_row_1 + vertex_column_2);
                        indices.push_back(num_vertex_columns * vertex_row_2 + vertex_column_2);
                    }
                }
            }
            if (!indices.empty()) {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), &indices[0],
                             GL_DYNAMIC_DRAW);
            }

            renderer.clear_screen(0.2f, 0.3f, 0.3f);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            window.update();
        }
    } catch (const shvav8::Exception& e) {
        std::cerr << e << std::endl;
    }

    std::cout << display;
}

/* Code to run a rom (maybe useful later):
    if (!rom_path.empty()) {
        std::ifstream rom(rom_path, std::ios::binary);
        if (!rom) {
            std::cerr << "Cannot open rom at " << rom_path << std::endl;
            exit(2);
        }

        u8 memory[0xDFF];
        rom.read((char *)memory, sizeof(memory));

        shvav8::FrameBuffer display;
        shvav8::Shvav8 interpreter(display);
        interpreter.load(memory);

        std::cout << "Running...\n";
        try {
            u16 pc, new_pc;
            while (true) {
                pc = interpreter.m_reg.pc;
                interpreter.next();
                new_pc = interpreter.m_reg.pc;
                if (pc == new_pc) {
                    break;
                }
            }
        } catch (const shvav8::Exception &e) {
            std::cerr << e << std::endl;
        }

        std::cout << display;
    }
*/

}  // namespace shvav8
