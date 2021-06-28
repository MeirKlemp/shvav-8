#include <shvav8.h>

#include <fstream>
#include <iostream>

i32 main(i32 argc, const char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <ROM>\n";
        return 1;
    }

    shvav8::Shvav8 interpreter;
    std::ifstream rom(argv[1]);
    u8 memory[0xDFF];
    rom.readsome((char *)memory, 0xDFF);
    interpreter.load(memory);

    std::cout << "Running...\n" << std::hex;
    u16 pc, new_pc;
    while (true) {
        pc = interpreter.reg.pc;
        std::cout << pc << '\n';
        interpreter.next();
        new_pc = interpreter.reg.pc;
        if (pc == new_pc) {
            break;
        }
    }

    for (u32 y = 0; y < 32; ++y) {
        for (u32 x = 0; x < 64; ++x) {
            u64 mask = 1ull << x;
            std::cout << ((bool)(interpreter.frame_buffer[y] & mask) ? '*' : ' ') << ' ';
        }
        std::cout << '\n';
    }

    return 0;
}
