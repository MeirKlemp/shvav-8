#include <app.h>

#include <iostream>

shvav8::i32 main(shvav8::i32 argc, const char **argv) {
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " <ROM>\n";
        return 1;
    }

    shvav8::App app(argc == 2 ? argv[1] : "");
}
