#include <exceptions.h>
#include <shvav8.h>

#include <fstream>
#include <iostream>

i32 main(i32 argc, const char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <ROM>\n";
        return 1;
    }

    shvav8::Display display;
    shvav8::Shvav8 interpreter(display);
    std::ifstream rom(argv[1]);
    u8 memory[0xDFF];
    rom.readsome((char *)memory, 0xDFF);
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
        std::cout << e << std::endl;
    }

    std::cout << display;
}
