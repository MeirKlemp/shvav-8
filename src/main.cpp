#include <exceptions.h>
#include <shvav8.h>

#include <fstream>
#include <iostream>

i32 main(i32 argc, const char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <ROM>\n";
        return 1;
    }

    try {
        throw shvav8::StackOverflowException(0x201, 0x00EE);
    } catch (const shvav8::StackOverflowException &e) {
        std::cout << e << std::endl;
    }
    try {
        throw shvav8::StackUnderflowException(0x201, 0x00EE);
    } catch (const shvav8::StackUnderflowException &e) {
        std::cout << e << std::endl;
    }
    try {
        throw shvav8::MemoryOverflowException(0x1000, 0x00EE);
    } catch (const shvav8::MemoryOverflowException &e) {
        std::cout << e << std::endl;
    }
    try {
        throw shvav8::KeyOutOfRangeException(0x201, 0x00EE);
    } catch (const shvav8::KeyOutOfRangeException &e) {
        std::cout << e << std::endl;
    }

    shvav8::Display display;
    shvav8::Shvav8 interpreter(display);
    std::ifstream rom(argv[1]);
    u8 memory[0xDFF];
    rom.readsome((char *)memory, 0xDFF);
    interpreter.load(memory);

    std::cout << "Running...\n";
    u16 pc, new_pc;
    while (true) {
        pc = interpreter.m_reg.pc;
        interpreter.next();
        new_pc = interpreter.m_reg.pc;
        if (pc == new_pc) {
            break;
        }
    }

    std::cout << display;
}
