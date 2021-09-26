#pragma once

#include <optional>
#include <unordered_map>

#include "audio/beeper.h"
#include "defines.h"
#include "emulation/shvav8.h"
#include "rendering/renderer.h"
#include "rendering/shader.h"
#include "rendering/window.h"

namespace shvav8 {

class App {
   public:
    App(const char* rom_path);

    void run();

   private:
    Window& m_window;
    Renderer& m_renderer;
    Shader m_shader;
    std::optional<Beeper> m_beeper;
    Shvav8 m_interpreter;
    bool m_window_resized;
    u32 m_max_cycles_per_frame;
    u32 m_cycles_per_frame;

    static const char* s_normal_shader;
    static const char* s_gradient_shader;
    static std::unordered_map<i32, u8> s_keybindings;
};

}  // namespace shvav8
