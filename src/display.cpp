#include <display.h>

namespace shvav8 {

bool Display::draw(const u8 x, const u8 y) {
    const usize i = index(x % Display::COLUMNS, y % Display::ROWS);
    const bool collision = m_frame_buffer[i];
    m_frame_buffer[i] ^= 1;
    return collision;
}

void Display::clear() { m_frame_buffer.fill(0); }

inline usize Display::index(const u8 x, const u8 y) const { return y * Display::COLUMNS + x; }

std::ostream& operator<<(std::ostream& ostream, const Display& display) {
    for (u8 y = 0; y < Display::ROWS; ++y) {
        for (u8 x = 0; x < Display::COLUMNS; ++x) {
            if (display.m_frame_buffer[display.index(x, y)]) {
                ostream << "* ";
            } else {
                ostream << "  ";
            }
        }
        ostream << std::endl;
    }

    return ostream;
}

}  // namespace shvav8
