#pragma once

#include <defines.h>
#include <display.h>

#include <array>

// TODO: temporary, remove it when no need access to pc from main
i32 main(i32 argc, const char** argv);

namespace shvav8 {

class Shvav8 {
    // TODO: temporary, remove it when no need access to pc from main
    friend int ::main(i32 argc, const char** argv);

   public:
    Shvav8(Display& display);
    /**
     * Runs next instruction.
     * Should run with frequency of 60hz.
     */
    void next();
    void reset();
    void load(u8* memory, usize size = 0xDFF);

   private:
    struct Registers {
        u8 v[0x10] = {0};  // general purpose registers
        u16 i;             // register ususally used as a pointer
        u8 dt = 0;         // delay timer register
        u8 st = 0;         // sound timer register
        u16 pc = 0x200;    // program counter register
        u8 sp = 0;         // stack pointer register
    } m_reg;

    u16 m_stack[0x10];
    u8 m_memory[0xFFF];
    bool m_keypad[0xF];
    Display& m_display;

    u16 m_opcode;

    using Operation = void (Shvav8::*)();
    static std::array<Operation, 0x10> optable;
    static std::array<Operation, 0xF> optable0;
    static std::array<Operation, 0xF> optable8;
    static std::array<Operation, 0xF> optableE;
    static std::array<Operation, 0x66> optableF;

    Shvav8(bool);
    static Shvav8 s_optables_initializer;

    void table0();
    void table8();
    void tableE();
    void tableF();

    // opcode parsers
    inline u16 get_nnn() const;
    inline u8 get_kk() const;
    inline u8 get_x() const;
    inline u8 get_y() const;
    inline u8 get_n() const;

    // instructions
    void op_00E0_cls();
    void op_00EE_ret();
    void op_1nnn_jp();
    void op_2nnn_call();
    void op_3xkk_se();
    void op_4xkk_sne();
    void op_5xy0_se();
    void op_6xkk_ld();
    void op_7xkk_add();
    void op_8xy0_ld();
    void op_8xy1_or();
    void op_8xy2_and();
    void op_8xy3_xor();
    void op_8xy4_add();
    void op_8xy5_sub();
    void op_8xy6_shr();
    void op_8xy7_subn();
    void op_8xyE_shl();
    void op_9xy0_sne();
    void op_Annn_ld();
    void op_Bnnn_jp();
    void op_Cxkk_rnd();
    void op_Dxyn_drw();
    void op_Ex9E_skp();
    void op_ExA1_sknp();
    void op_Fx07_ld();
    void op_Fx0A_ld();
    void op_Fx15_ld();
    void op_Fx18_ld();
    void op_Fx1E_add();
    void op_Fx29_ld();
    void op_Fx33_ld();
    void op_Fx55_ld();
    void op_Fx65_ld();
    void nop();
};

}  // namespace shvav8
