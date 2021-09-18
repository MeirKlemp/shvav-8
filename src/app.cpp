#include "app.h"

#include <emulation/exceptions.h>
#include <emulation/shvav8.h>
#include <rendering/renderer.h>
#include <rendering/window.h>

#include <fstream>
#include <iostream>

namespace shvav8 {

App::App(const char* rom_path) {
    auto& window = Window::create(640, 480, "Shvav-8");
    auto& renderer = Renderer::create();

    window.on_resize([&renderer](const i32 width, const i32 height) {
        renderer.set_viewport(0, 0, width, height);
    });

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

            renderer.clear_screen(0.2f, 0.3f, 0.3f);

            auto squares = display.get_drawn_pixels();
            renderer.draw_squares(squares, squares.size());

            window.update();
        }
    } catch (const shvav8::Exception& e) {
        std::cerr << e << std::endl;
    }

    std::cout << display;
}

}  // namespace shvav8
