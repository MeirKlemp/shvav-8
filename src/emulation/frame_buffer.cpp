#include "frame_buffer.h"

namespace shvav8 {

bool FrameBuffer::draw(const u8 x, const u8 y) {
    const usize i = index(x % FrameBuffer::COLUMNS, y % FrameBuffer::ROWS);
    const bool collision = m_frame_buffer[i];
    m_frame_buffer[i] ^= 1;
    return collision;
}

void FrameBuffer::clear() { m_frame_buffer.fill(0); }

inline usize FrameBuffer::index(const u8 x, const u8 y) const { return y * FrameBuffer::COLUMNS + x; }

std::ostream& operator<<(std::ostream& ostream, const FrameBuffer& display) {
    for (u8 y = 0; y < FrameBuffer::ROWS; ++y) {
        for (u8 x = 0; x < FrameBuffer::COLUMNS; ++x) {
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
