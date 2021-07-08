#include <GLFW/glfw3.h>
#include <emulation/exceptions.h>
#include <emulation/shvav8.h>
#include <rendering/window.h>

#include <fstream>
#include <iostream>

i32 main(i32 argc, const char **argv) {
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " <ROM>\n";
        return 1;
    }

    if (argc == 2) {
        std::ifstream rom(argv[1], std::ios::binary);
        if (!rom) {
            std::cerr << "Cannot open rom at " << argv[1] << std::endl;
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
