#pragma once

#include <defines.h>

#include <string>

namespace shvav8 {

class App {
   public:
    App(const char* rom_path);

   private:
    static class Shader get_shader();
};

}  // namespace shvav8
