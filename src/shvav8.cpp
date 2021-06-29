#include "shvav8.h"

#include <cstdlib>
#include <cstring>

namespace shvav8 {

Shvav8::Shvav8()
    : m_memory{
          0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
          0x20, 0x60, 0x20, 0x20, 0x70,  // 1
          0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
          0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
          0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
          0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
          0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
          0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
          0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
          0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
          0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
          0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
          0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
          0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
          0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
          0xF0, 0x80, 0xF0, 0x80, 0x80,  // F
      } {}

void Shvav8::reset() {
    m_reg = Shvav8::Registers();
    clear_frame_buffer();
}

void Shvav8::load(u8 *memory, usize size) {
    usize min = size <= 0xDFF ? size : 0xDFF;
    memcpy(m_memory + 0x200, memory, min);
}

void Shvav8::next() {
    // fetch
    m_opcode = m_memory[m_reg.pc] << 8;
    m_opcode += m_memory[m_reg.pc + 1];
    m_reg.pc += 2;
    if (m_reg.dt > 0) {
        m_reg.dt -= 1;
    }
    if (m_reg.st > 0) {
        m_reg.st -= 1;
    }

    // decode
    Operation op = optable[m_opcode >> 12];

    // execute
    (this->*op)();
}

bool Shvav8::draw_pixel(u8 x, u8 y, bool draw) {
    y %= 32;
    u64 mask = draw << (x % 64);
    bool collision = m_frame_buffer[y] & mask;
    m_frame_buffer[y] ^= mask;
    return collision;
}

void Shvav8::clear_frame_buffer() { memset(m_frame_buffer, 0, 0x40); }

std::array<Shvav8::Operation, 0x10> Shvav8::optable = {
    &Shvav8::table0,      &Shvav8::op_1nnn_jp,  &Shvav8::op_2nnn_call, &Shvav8::op_3xkk_se,
    &Shvav8::op_4xkk_sne, &Shvav8::op_5xy0_se,  &Shvav8::op_6xkk_ld,   &Shvav8::op_7xkk_add,
    &Shvav8::table8,      &Shvav8::op_9xy0_sne, &Shvav8::op_Annn_ld,   &Shvav8::op_Bnnn_jp,
    &Shvav8::op_Cxkk_rnd, &Shvav8::op_Dxyn_drw, &Shvav8::tableE,       &Shvav8::tableF,
};
std::array<Shvav8::Operation, 0xF> Shvav8::optable0;
std::array<Shvav8::Operation, 0xF> Shvav8::optable8;
std::array<Shvav8::Operation, 0xF> Shvav8::optableE;
std::array<Shvav8::Operation, 0x66> Shvav8::optableF;

Shvav8::Shvav8(bool) {
    for (usize i = 0; i < optable0.size(); ++i) {
        optable0[i] = &Shvav8::nop;
    }
    for (usize i = 0; i < optable8.size(); ++i) {
        optable8[i] = &Shvav8::nop;
    }
    for (usize i = 0; i < optableE.size(); ++i) {
        optableE[i] = &Shvav8::nop;
    }
    for (usize i = 0; i < optableF.size(); ++i) {
        optableF[i] = &Shvav8::nop;
    }

    optable0[0x0] = &Shvav8::op_00E0_cls;
    optable0[0xE] = &Shvav8::op_00EE_ret;

    optable8[0x0] = &Shvav8::op_8xy0_ld;
    optable8[0x1] = &Shvav8::op_8xy1_or;
    optable8[0x2] = &Shvav8::op_8xy2_and;
    optable8[0x3] = &Shvav8::op_8xy3_xor;
    optable8[0x4] = &Shvav8::op_8xy4_add;
    optable8[0x5] = &Shvav8::op_8xy5_sub;
    optable8[0x6] = &Shvav8::op_8xy6_shr;
    optable8[0x7] = &Shvav8::op_8xy7_subn;
    optable8[0xE] = &Shvav8::op_8xyE_shl;

    optableE[0x1] = &Shvav8::op_ExA1_sknp;
    optableE[0xE] = &Shvav8::op_Ex9E_skp;

    optableF[0x07] = &Shvav8::op_Fx07_ld;
    optableF[0x0A] = &Shvav8::op_Fx0A_ld;
    optableF[0x15] = &Shvav8::op_Fx15_ld;
    optableF[0x18] = &Shvav8::op_Fx18_ld;
    optableF[0x1E] = &Shvav8::op_Fx1E_add;
    optableF[0x29] = &Shvav8::op_Fx29_ld;
    optableF[0x33] = &Shvav8::op_Fx33_ld;
    optableF[0x55] = &Shvav8::op_Fx55_ld;
    optableF[0x65] = &Shvav8::op_Fx65_ld;

}
Shvav8 Shvav8::s_optables_initializer(true);

void Shvav8::table0() {
    if (m_opcode >> 4 == 0xE) {
        (this->*optable0[m_opcode & 0xF])();
    } else {
        nop();
    }
}
void Shvav8::table8() { (this->*optable8[m_opcode & 0xF])(); }
void Shvav8::tableE() {
    u8 v = (m_opcode & 0xF0) >> 4;
    if (v == 0x9 || v == 0xA) {
        (this->*optableE[m_opcode & 0xF])();
    } else {
        nop();
    }
}
void Shvav8::tableF() { (this->*optableF[m_opcode & 0xFF])(); }

inline u16 Shvav8::get_nnn() const { return m_opcode & 0x0FFF; }
inline u8 Shvav8::get_kk() const { return m_opcode & 0x00FF; }
inline u8 Shvav8::get_x() const { return (m_opcode & 0x0F00) >> 8; }
inline u8 Shvav8::get_y() const { return (m_opcode & 0x00F0) >> 4; }
inline u8 Shvav8::get_n() const { return m_opcode & 0x000F; }

void Shvav8::op_00E0_cls() { clear_frame_buffer(); }
void Shvav8::op_00EE_ret() { m_reg.pc = m_stack[m_reg.sp--]; }
void Shvav8::op_1nnn_jp() { m_reg.pc = get_nnn(); }
void Shvav8::op_2nnn_call() {
    m_reg.sp += 1;
    m_stack[m_reg.sp] = m_reg.pc;

    m_reg.pc = get_nnn();
}
void Shvav8::op_3xkk_se() {
    if (m_reg.v[get_x()] == get_kk()) {
        m_reg.pc += 2;
    }
}
void Shvav8::op_4xkk_sne() {
    if (m_reg.v[get_x()] != get_kk()) {
        m_reg.pc += 2;
    }
}
void Shvav8::op_5xy0_se() {
    if (m_reg.v[get_x()] != m_reg.v[get_y()]) {
        m_reg.pc += 2;
    }
}
void Shvav8::op_6xkk_ld() { m_reg.v[get_x()] = get_kk(); }
void Shvav8::op_7xkk_add() { m_reg.v[get_x()] += get_kk(); }
void Shvav8::op_8xy0_ld() { m_reg.v[get_x()] = m_reg.v[get_y()]; }
void Shvav8::op_8xy1_or() { m_reg.v[get_x()] |= m_reg.v[get_y()]; }
void Shvav8::op_8xy2_and() { m_reg.v[get_x()] &= m_reg.v[get_y()]; }
void Shvav8::op_8xy3_xor() { m_reg.v[get_x()] ^= m_reg.v[get_y()]; }
void Shvav8::op_8xy4_add() {
    u8 x = get_x(), y = get_y();
    m_reg.v[0xF] = m_reg.v[x] > 0xFF - m_reg.v[y];  // Vf = carry
    m_reg.v[x] += m_reg.v[y];
}
void Shvav8::op_8xy5_sub() {
    u8 x = get_x(), y = get_y();
    m_reg.v[0xF] = m_reg.v[x] > m_reg.v[y];  // Vf = NOT borrow
    m_reg.v[x] -= m_reg.v[y];
}
void Shvav8::op_8xy6_shr() {
    u8 x = get_x();
    m_reg.v[0xF] = m_reg.v[x] & 0x01;
    m_reg.v[x] >>= 1;
}
void Shvav8::op_8xy7_subn() {
    u8 x = get_x(), y = get_y();
    m_reg.v[0xF] = m_reg.v[y] > m_reg.v[x];  // Vf = NOT borrow
    m_reg.v[x] = m_reg.v[y] - m_reg.v[x];
}
void Shvav8::op_8xyE_shl() {
    u8 x = get_x();
    m_reg.v[0xF] = m_reg.v[x] & 0x80;
    m_reg.v[x] <<= 1;
}
void Shvav8::op_9xy0_sne() {
    if (m_reg.v[get_x()] != m_reg.v[get_y()]) {
        m_reg.pc += 2;
    }
}
void Shvav8::op_Annn_ld() { m_reg.i = get_nnn(); }
void Shvav8::op_Bnnn_jp() { m_reg.pc = m_reg.v[0] + get_nnn(); }
void Shvav8::op_Cxkk_rnd() { m_reg.v[get_x()] = rand() && get_kk(); }
void Shvav8::op_Dxyn_drw() {
    bool collision = false;
    u8 n = get_n();
    u8 vx = m_reg.v[get_x()], vy = m_reg.v[get_y()];
    for (u8 dy = 0; dy < n; ++dy) {
        for (u8 dx = 0; dx < 8; ++dx) {
            u8 mask = 1 << (7 - dx);
            bool pixel = m_memory[m_reg.i + dy] & mask;
            if (draw_pixel(vx + dx, vy + dy, pixel)) {
                collision = true;
            }
        }
    }

    m_reg.v[0xF] = collision;
}
void Shvav8::op_Ex9E_skp() {
    if (m_keypad[m_reg.v[get_x()]]) {
        m_reg.pc += 2;
    }
}
void Shvav8::op_ExA1_sknp() {
    if (!m_keypad[m_reg.v[get_x()]]) {
        m_reg.pc += 2;
    }
}
void Shvav8::op_Fx07_ld() { m_reg.v[get_x()] = m_reg.dt; }
void Shvav8::op_Fx0A_ld() {
    i8 keypress = -1;
    for (u8 i = 0; i < 0xF; ++i) {
        if (m_keypad[i]) {
            keypress = i;
        }
    }

    if (keypress == -1) {
        m_reg.pc -= 2;
    } else {
        m_reg.v[get_x()] = keypress;
    }
}
void Shvav8::op_Fx15_ld() { m_reg.dt = m_reg.v[get_x()]; }
void Shvav8::op_Fx18_ld() { m_reg.st = m_reg.v[get_x()]; }
void Shvav8::op_Fx1E_add() { m_reg.i += m_reg.v[get_x()]; }
void Shvav8::op_Fx29_ld() { m_reg.i = m_reg.v[get_x()] * 5; }
void Shvav8::op_Fx33_ld() {
    u8 vx = m_reg.v[get_x()];
    m_memory[m_reg.i] = vx / 100;
    m_memory[m_reg.i + 1] = vx % 100 / 10;
    m_memory[m_reg.i + 2] = vx % 10;
}
void Shvav8::op_Fx55_ld() {
    for (u8 i = 0; i < 0xF; ++i) {
        m_memory[m_reg.i + i] = m_reg.v[i];
    }
}
void Shvav8::op_Fx65_ld() {
    for (u8 i = 0; i < 0xF; ++i) {
        m_reg.v[i] = m_memory[m_reg.i + i];
    }
}
void Shvav8::nop() {}

}  // namespace shvav8
