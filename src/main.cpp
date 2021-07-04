#include <GLFW/glfw3.h>
#include <exceptions.h>
#include <shvav8.h>

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

        shvav8::Display display;
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

    GLFWwindow *window;

    /* Initialize glfw */
    if (!glfwInit()) {
        return 3;
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 4;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
