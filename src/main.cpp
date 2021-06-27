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
    for (u32 i = 0; i < 6; ++i) {
        std::cout << i << ':' << interpreter.reg.pc << std::endl;
        interpreter.next();
	std::cout << "i = " << interpreter.reg.i << std::endl;
    }

    for (u32 y = 0; y < 32; ++y) {
        for (u32 x = 0; x < 64; ++x) {
            u64 mask = 1 << x;
            std::cout << ((bool)(interpreter.frame_buffer[y] & mask) ? '*' : ' ') << ' ';
        }
        std::cout << '\n';
    }

    return 0;
}
