#include "app.h"

#include "defines.h"
#include "emulation/state_exception.h"
#include "fps.h"

namespace shvav8 {

App::App(const char* rom_path)
    : m_window(Window::create(640, 480, std::string("Shvav-8") + rom_path)),
      m_renderer(Renderer::create()),
      m_shader(s_gradient_shader),
      m_beeper(initialize_beeper(1000)),
      m_max_cycles_per_frame(10),
      m_cycles_per_frame(5) {
    srand((u32)time(0));

    std::ifstream rom(rom_path, std::ios::binary);
    if (!rom) {
        std::stringstream message;
        message << "Cannot open rom at " << rom_path;
        throw std::runtime_error{message.str()};
    }
    u8 memory[Shvav8::ROM_SIZE];
    rom.read((char*)memory, sizeof(memory));
    m_interpreter.load(memory);

    m_shader.set_uniform_2f("screen_size", 640, 480);
    m_shader.bind();
    m_window.on_resize([&](const i32 width, const i32 height) {
        m_renderer.set_viewport(0, 0, width, height);
        m_shader.set_uniform_2f("screen_size", width, height);
    });
    m_window.on_key_event([&](const i32 keycode, const i32 action) {
        if (action == SHVAV8_ACTION(REPEAT)) {
            return;
        }

        if (action == SHVAV8_ACTION(RELEASE)) {
            switch (keycode) {
                case SHVAV8_KEY(RIGHT):
                    if (m_cycles_per_frame == m_max_cycles_per_frame) {
                        m_cycles_per_frame = 0;
                        return;
                    }
                    m_cycles_per_frame += 1;
                    return;
                case SHVAV8_KEY(LEFT):
                    if (m_cycles_per_frame == 0) {
                        m_cycles_per_frame = m_max_cycles_per_frame;
                        return;
                    }
                    m_cycles_per_frame -= 1;
                    return;
                case SHVAV8_KEY(UP):
                    if (m_beeper) {
                        m_beeper->frequency(m_beeper->frequency() + 100);
                    }
                    return;
                case SHVAV8_KEY(DOWN):
                    if (m_beeper) {
                        if (m_beeper->frequency() <= 100) {
                            return;
                        }
                        m_beeper->frequency(m_beeper->frequency() - 100);
                    }
                    return;
                case SHVAV8_KEY(SPACE):
                    m_interpreter.reset();
                    return;
                case SHVAV8_KEY(ESCAPE):
                    m_window.close();
                    return;
            }
        }

        if (s_keybindings.find(keycode) != s_keybindings.end()) {
            const bool pressed = action == SHVAV8_ACTION(PRESS);
            m_interpreter.set_key_state(s_keybindings[keycode], pressed);
        }
    });
}

void App::run() {
    Fps fps(60);

    /* Loop until the user closes the window */
    while (!m_window.should_close()) {
        for (u32 i = 0; i < fps.loops(); ++i) {
            try {
                for (u32 i = 0; i < m_cycles_per_frame; ++i) {
                    m_interpreter.cycle();
                }
            } catch (const StateException& e) {
                std::cerr << e.what() << std::endl;
            }

            if (m_beeper) {
                if (m_interpreter.should_beep()) {
                    if (!m_beeper->playing()) {
                        m_beeper->play();
                    }
                } else {
                    if (m_beeper->playing() && m_beeper->beeped()) {
                        m_beeper->stop();
                    }
                }
            }
            m_interpreter.update_timers();
        }

        auto squares = m_interpreter.get_drawn_pixels();
        m_renderer.clear_screen(0.1f, 0.1f, 0.1f);
        m_renderer.draw_squares(squares, squares.size());

        m_window.swap_buffers();
        m_window.poll_events();

        fps.update();
    }
}

std::unique_ptr<Beeper> App::initialize_beeper(u32 frequency) {
    try {
        return std::make_unique<Beeper>(frequency);
    } catch (const std::runtime_error& e) {
        /* m_beeper is null and the app won't beep. */
        std::cerr << "Warning: Couldn't initialize sound: " << e.what() << std::endl;
        return nullptr;
    }
}

std::unordered_map<i32, u8> App::s_keybindings{
    {SHVAV8_KEY(1), 0x1}, {SHVAV8_KEY(2), 0x2}, {SHVAV8_KEY(3), 0x3}, {SHVAV8_KEY(4), 0xC},
    {SHVAV8_KEY(Q), 0x4}, {SHVAV8_KEY(W), 0x5}, {SHVAV8_KEY(E), 0x6}, {SHVAV8_KEY(R), 0xD},
    {SHVAV8_KEY(A), 0x7}, {SHVAV8_KEY(S), 0x8}, {SHVAV8_KEY(D), 0x9}, {SHVAV8_KEY(F), 0xE},
    {SHVAV8_KEY(Z), 0XA}, {SHVAV8_KEY(X), 0x0}, {SHVAV8_KEY(C), 0xB}, {SHVAV8_KEY(V), 0xF},
};

const char* App::s_normal_shader =
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
)";

const char* App::s_gradient_shader =
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
)";

}  // namespace shvav8
