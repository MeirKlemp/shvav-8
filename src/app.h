#pragma once

#include <defines.h>

#include <string_view>

namespace shvav8 {

class App {
    public:
    App(const std::string& rom_path);

    i32 run();

    App(const App&) = delete;
    App& operator=(const App&) = delete;

    private:
    std::string m_rom_path;
};

}