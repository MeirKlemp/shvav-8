#include "frame_buffer.h"

namespace shvav8 {

bool FrameBuffer::draw(const u8 x, const u8 y) {
    const usize i = index(x % FrameBuffer::COLUMNS, y % FrameBuffer::ROWS);
    const bool collision = m_frame_buffer[i];
    m_frame_buffer[i] ^= 1;
    return collision;
}

void FrameBuffer::clear() { m_frame_buffer.fill(0); }

std::vector<u32> FrameBuffer::get_drawn_pixels() const {
    std::vector<u32> squares;
    for (u8 row = 0; row < FrameBuffer::ROWS; ++row) {
        for (u8 column = 0; column < FrameBuffer::COLUMNS; ++column) {
            if (is_drawn(column, row)) {
                squares.push_back(index(column, row));
            }
        }
    }
    return squares;
}

bool FrameBuffer::is_drawn(const u8 x, const u8 y) const { return m_frame_buffer[index(x, y)]; }

usize FrameBuffer::index(const u8 x, const u8 y) const { return y * FrameBuffer::COLUMNS + x; }

std::ostream& operator<<(std::ostream& ostream, const FrameBuffer& display) {
    for (u8 y = 0; y < FrameBuffer::ROWS; ++y) {
        for (u8 x = 0; x < FrameBuffer::COLUMNS; ++x) {
            if (display.is_drawn(x, y)) {
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
