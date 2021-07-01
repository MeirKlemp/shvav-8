#include "shvav8.h"

#include <exceptions.h>

namespace shvav8 {

Shvav8::Shvav8(Display& display)
    : m_memory{ // builtin font-set
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
      },
	m_display(display) {}

void Shvav8::reset() {
    m_reg = Shvav8::Registers();
    m_keypad.fill(false);
    m_display.clear();
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
    const Operation op = s_optable[m_opcode >> 12];

    // execute
    (this->*op)();
}

void Shvav8::table0() {
    if (m_opcode >> 4 == 0xE) {
        (this->*s_optable0[m_opcode & 0xF])();
    } else {
        nop();
    }
}
void Shvav8::table8() { (this->*s_optable8[m_opcode & 0xF])(); }
void Shvav8::tableE() {
    const u8 first_byte_hi = (m_opcode & 0xF0) >> 4;
    if (first_byte_hi == 0x9 || first_byte_hi == 0xA) {
        (this->*s_optableE[m_opcode & 0xF])();
    } else {
        nop();
    }
}
void Shvav8::tableF() { (this->*s_optableF[m_opcode & 0xFF])(); }

inline u16 Shvav8::get_nnn() const { return m_opcode & 0x0FFF; }
inline u8 Shvav8::get_kk() const { return m_opcode & 0x00FF; }
inline u8 Shvav8::get_x() const { return (m_opcode & 0x0F00) >> 8; }
inline u8 Shvav8::get_y() const { return (m_opcode & 0x00F0) >> 4; }
inline u8 Shvav8::get_n() const { return m_opcode & 0x000F; }

void Shvav8::op_00E0_cls() { m_display.clear(); }
void Shvav8::op_00EE_ret() { m_reg.pc = m_stack[m_reg.sp--]; }
void Shvav8::op_1nnn_jp() { m_reg.pc = get_nnn(); }
void Shvav8::op_2nnn_call() {
    if (m_reg.sp >= m_stack.size()) {
        throw StackOverflowException(m_reg.pc, m_opcode);
    }

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
    if (m_reg.v[get_x()] == m_reg.v[get_y()]) {
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
    const u8 x = get_x(), y = get_y();
    m_reg.v[0xF] = m_reg.v[x] > (0xFF - m_reg.v[y]);  // Vf = carry
    m_reg.v[x] += m_reg.v[y];
}
void Shvav8::op_8xy5_sub() {
    const u8 x = get_x(), y = get_y();
    m_reg.v[0xF] = m_reg.v[x] > m_reg.v[y];  // Vf = NOT borrow
    m_reg.v[x] -= m_reg.v[y];
}
void Shvav8::op_8xy6_shr() {
    const u8 x = get_x();
    m_reg.v[0xF] = m_reg.v[x] & 0x01;
    m_reg.v[x] >>= 1;
}
void Shvav8::op_8xy7_subn() {
    const u8 x = get_x(), y = get_y();
    m_reg.v[0xF] = m_reg.v[y] > m_reg.v[x];  // Vf = NOT borrow
    m_reg.v[x] = m_reg.v[y] - m_reg.v[x];
}
void Shvav8::op_8xyE_shl() {
    const u8 x = get_x();
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
    const u8 n = get_n();
    const u8 vx = m_reg.v[get_x()], vy = m_reg.v[get_y()];
    for (u8 dy = 0; dy < n; ++dy) {
        for (u8 dx = 0; dx < SPRITE_WIDTH; ++dx) {
            const u8 mask = 1 << (SPRITE_WIDTH - dx - 1);
            const bool draw = m_memory[m_reg.i + dy] & mask;
            if (draw && m_display.draw(vx + dx, vy + dy)) {
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
    for (u8 i = 0; i < m_keypad.size(); ++i) {
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
    const u8 vx = m_reg.v[get_x()];
    m_memory[m_reg.i] = vx / 100;
    m_memory[m_reg.i + 1] = vx % 100 / 10;
    m_memory[m_reg.i + 2] = vx % 10;
}
void Shvav8::op_Fx55_ld() {
    for (u8 i = 0; i < m_reg.v.size(); ++i) {
        m_memory[m_reg.i + i] = m_reg.v[i];
    }
}
void Shvav8::op_Fx65_ld() {
    for (u8 i = 0; i < m_reg.v.size(); ++i) {
        m_reg.v[i] = m_memory[m_reg.i + i];
    }
}
void Shvav8::nop() {}

constexpr std::array<Shvav8::Operation, 0x10> Shvav8::init_optable() {
    return {
        &Shvav8::table0,      &Shvav8::op_1nnn_jp,  &Shvav8::op_2nnn_call, &Shvav8::op_3xkk_se,
        &Shvav8::op_4xkk_sne, &Shvav8::op_5xy0_se,  &Shvav8::op_6xkk_ld,   &Shvav8::op_7xkk_add,
        &Shvav8::table8,      &Shvav8::op_9xy0_sne, &Shvav8::op_Annn_ld,   &Shvav8::op_Bnnn_jp,
        &Shvav8::op_Cxkk_rnd, &Shvav8::op_Dxyn_drw, &Shvav8::tableE,       &Shvav8::tableF,
    };
}
constexpr std::array<Shvav8::Operation, 0xF> Shvav8::init_optable0() {
    std::array<Shvav8::Operation, 0xF> result = {};
    for (usize i = 0; i < result.size(); ++i) {
        result[i] = &Shvav8::nop;
    }
    result[0x0] = &Shvav8::op_00E0_cls;
    result[0xE] = &Shvav8::op_00EE_ret;
    return result;
}
constexpr std::array<Shvav8::Operation, 0xF> Shvav8::init_optable8() {
    std::array<Shvav8::Operation, 0xF> result = {};
    for (usize i = 0; i < result.size(); ++i) {
        result[i] = &Shvav8::nop;
    }
    result[0x0] = &Shvav8::op_8xy0_ld;
    result[0x1] = &Shvav8::op_8xy1_or;
    result[0x2] = &Shvav8::op_8xy2_and;
    result[0x3] = &Shvav8::op_8xy3_xor;
    result[0x4] = &Shvav8::op_8xy4_add;
    result[0x5] = &Shvav8::op_8xy5_sub;
    result[0x6] = &Shvav8::op_8xy6_shr;
    result[0x7] = &Shvav8::op_8xy7_subn;
    result[0xE] = &Shvav8::op_8xyE_shl;
    return result;
}
constexpr std::array<Shvav8::Operation, 0xF> Shvav8::init_optableE() {
    std::array<Shvav8::Operation, 0xF> result = {};
    for (usize i = 0; i < result.size(); ++i) {
        result[i] = &Shvav8::nop;
    }
    result[0x1] = &Shvav8::op_ExA1_sknp;
    result[0xE] = &Shvav8::op_Ex9E_skp;
    return result;
}
constexpr std::array<Shvav8::Operation, 0x66> Shvav8::init_optableF() {
    std::array<Shvav8::Operation, 0x66> result = {};
    for (usize i = 0; i < result.size(); ++i) {
        result[i] = &Shvav8::nop;
    }
    result[0x07] = &Shvav8::op_Fx07_ld;
    result[0x0A] = &Shvav8::op_Fx0A_ld;
    result[0x15] = &Shvav8::op_Fx15_ld;
    result[0x18] = &Shvav8::op_Fx18_ld;
    result[0x1E] = &Shvav8::op_Fx1E_add;
    result[0x29] = &Shvav8::op_Fx29_ld;
    result[0x33] = &Shvav8::op_Fx33_ld;
    result[0x55] = &Shvav8::op_Fx55_ld;
    result[0x65] = &Shvav8::op_Fx65_ld;
    return result;
}

const std::array<Shvav8::Operation, 0x10> Shvav8::s_optable = Shvav8::init_optable();
const std::array<Shvav8::Operation, 0xF> Shvav8::s_optable0 = Shvav8::init_optable0();
const std::array<Shvav8::Operation, 0xF> Shvav8::s_optable8 = Shvav8::init_optable8();
const std::array<Shvav8::Operation, 0xF> Shvav8::s_optableE = Shvav8::init_optableE();
const std::array<Shvav8::Operation, 0x66> Shvav8::s_optableF = Shvav8::init_optableF();

}  // namespace shvav8
