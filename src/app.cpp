#include "app.h"

#include <emulation/exceptions.h>
#include <emulation/shvav8.h>
#include <rendering/renderer.h>
#include <rendering/shader.h>
#include <rendering/window.h>

#include <fstream>
#include <iostream>

namespace shvav8 {

App::App(const char* rom_path) {
    srand((u32)time(0));

    auto& window = Window::create(640, 480, "Shvav-8");
    auto& renderer = Renderer::create();

    Shader shader = get_beauty_shader();
    shader.bind();
    shader.set_uniform_2f("screen_size", 640, 480);

    window.on_resize([&renderer, &shader](const i32 width, const i32 height) {
        renderer.set_viewport(0, 0, width, height);
        shader.set_uniform_2f("screen_size", width, height);
    });

    std::ifstream rom(rom_path, std::ios::binary);
    if (!rom) {
        std::cerr << "Cannot open rom at " << rom_path << std::endl;
        exit(2);
    }

    u8 memory[shvav8::Shvav8::ROM_SIZE];
    rom.read((char*)memory, sizeof(memory));

    shvav8::FrameBuffer display;
    shvav8::Shvav8 interpreter(display);
    interpreter.load(memory);

    window.title(std::string("Shvav8 ") + rom_path);

    try {
        /* Loop until the user closes the window */
        while (!window.should_close()) {
            interpreter.next();

            renderer.clear_screen(0.1f, 0.1f, 0.1f);

            auto squares = display.get_drawn_pixels();
            renderer.draw_squares(squares, squares.size());

            window.update();
        }
    } catch (const shvav8::Exception& e) {
        std::cerr << e << std::endl;
        std::cerr << display;
    }
}

Shader App::get_shader() {
    return Shader(
        R"(
#shader vertex
#version 330 core

layout(location = 0) in vec4 pos;

void main() {
    gl_Position = vec4(pos.xyz, 1.0);
}

#shader fragment
#version 330 core

out vec4 color;

void main() {
    color = vec4(0.9f, 0.9f, 0.9f, 1.0f);
}
)");
}

Shader App::get_beauty_shader() {
    return Shader(
        R"(
#shader vertex
#version 330 core

layout(location = 0) in vec4 pos;

void main() {
    gl_Position = vec4(pos.xyz, 1.0);
}

#shader fragment
#version 330 core

out vec4 color;

uniform vec2 screen_size;

void main() {
    vec2 coord = vec2(gl_FragCoord.xy) / screen_size;
    color = vec4(coord.xy, 0.9f, 1.0f);
}
)");
}

}  // namespace shvav8
