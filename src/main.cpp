#include "app.h"

shvav8::i32 main(shvav8::i32 argc, const char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <ROM>\n";
        return 1;
    }

    shvav8::App app(argv[1]);
    app.run();

    return 0;
}
