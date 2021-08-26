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
    auto renderer = Renderer::create();

    /* Loop until the user closes the window */
    while (!window.should_close()) {
        renderer.clear_screen(0.2f, 0.3f, 0.3f);
        window.update();
    }
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
