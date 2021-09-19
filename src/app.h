#pragma once

#include <defines.h>

#include <unordered_map>

namespace shvav8 {

class App {
   public:
    App(const char* rom_path);

   private:
    static class Shader get_shader();
    static class Shader get_beauty_shader();

   private:
    static std::unordered_map<i32, u8> s_keybindings;
};

}  // namespace shvav8
