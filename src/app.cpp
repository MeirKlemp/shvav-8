#include "app.h"

#include <emulation/exceptions.h>
#include <emulation/shvav8.h>
#include <rendering/window.h>

#include <fstream>
#include <iostream>

namespace shvav8 {

App::App(const std::string& rom_path) : m_rom_path(rom_path) {}

i32 App::run() {
    if (!m_rom_path.empty()) {
        std::ifstream rom(m_rom_path, std::ios::binary);
        if (!rom) {
            std::cerr << "Cannot open rom at " << m_rom_path << std::endl;
            return 2;
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

    if (auto window = shvav8::Window::create(640, 480, "Shvav8")) {
        /* Loop until the user closes the window */
        while (!window->should_close()) {
            window->update();
        }
    }

}

}  // namespace shvav8