#include "app.h"

#include <audio/beeper.h>
#include <defines.h>
#include <emulation/shvav8.h>
#include <emulation/state_exception.h>
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
    Beeper beeper(1000);

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

    u8 memory[Shvav8::ROM_SIZE];
    rom.read((char*)memory, sizeof(memory));

    FrameBuffer display;
    Shvav8 interpreter(display);
    interpreter.load(memory);
    u32 cycles_per_frame = 5;
    const u32 max_cycles_per_frame = 10;

    window.title(std::string("Shvav-8 ") + rom_path);
    window.on_key_event([&](const i32 keycode, const i32 action) {
        if (action == SHVAV8_ACTION(REPEAT)) {
            return;
        }

        if (action == SHVAV8_ACTION(RELEASE)) {
            switch (keycode) {
                case SHVAV8_KEY(RIGHT):
                    if (cycles_per_frame == max_cycles_per_frame) {
                        cycles_per_frame = 0;
                        return;
                    }
                    cycles_per_frame += 1;
                    return;
                case SHVAV8_KEY(LEFT):
                    if (cycles_per_frame == 0) {
                        cycles_per_frame = max_cycles_per_frame;
                        return;
                    }
                    cycles_per_frame -= 1;
                    return;
                case SHVAV8_KEY(UP):
                    beeper.frequency(beeper.frequency() + 100);
                    return;
                case SHVAV8_KEY(DOWN):
                    if (beeper.frequency() <= 100) {
                        return;
                    }
                    beeper.frequency(beeper.frequency() - 100);
                    return;
                case SHVAV8_KEY(SPACE):
                    interpreter.reset();
                    return;
            }
        }

        if (s_keybindings.find(keycode) != s_keybindings.end()) {
            const bool pressed = action == SHVAV8_ACTION(PRESS);
            interpreter.set_key_state(s_keybindings[keycode], pressed);
        }
    });

    try {
        /* Loop until the user closes the window */
        while (!window.should_close()) {
            for (u32 i = 0; i < cycles_per_frame; ++i) {
                interpreter.cycle();
            }

            if (interpreter.should_beep()) {
                if (!beeper.playing()) {
                    beeper.play();
                }
            } else {
                if (beeper.playing() && beeper.beeped()) {
                    beeper.stop();
                }
            }
            interpreter.update_timers();

            renderer.clear_screen(0.1f, 0.1f, 0.1f);

            auto squares = display.get_drawn_pixels();
            renderer.draw_squares(squares, squares.size());

            window.update();
        }
    } catch (const shvav8::StateException& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << display;
    }
}

std::unordered_map<i32, u8> App::s_keybindings{
    {SHVAV8_KEY(1), 1},   {SHVAV8_KEY(2), 2}, {SHVAV8_KEY(3), 3},   {SHVAV8_KEY(4), 0xC},
    {SHVAV8_KEY(Q), 4},   {SHVAV8_KEY(W), 5}, {SHVAV8_KEY(E), 6},   {SHVAV8_KEY(R), 0xD},
    {SHVAV8_KEY(A), 7},   {SHVAV8_KEY(S), 8}, {SHVAV8_KEY(D), 9},   {SHVAV8_KEY(F), 0xE},
    {SHVAV8_KEY(Z), 0XA}, {SHVAV8_KEY(X), 0}, {SHVAV8_KEY(C), 0xB}, {SHVAV8_KEY(V), 0xF},
};

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
