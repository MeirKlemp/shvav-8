#pragma once

#include <defines.h>

#include <array>
#include <iostream>

namespace shvav8 {

class FrameBuffer {
   public:
    /**
     * Draws a pixel on the display.
     * If the pixel is already 1 then a "collision" happens
     * and the pixel is set to 0.
     * @param x The column position in range of [0, COLUMNS).
     * 	        If not in range, then the value will be wraped around.
     * @param y The row position in range of [0, ROWS)
     * 	        If not in range, then the value will be wraped around.
     * @return True if collided, else False.
     */
    bool draw(u8 x, u8 y);
    /**
     * Sets all the pixels on the display to 0.
     */
    void clear();

    bool is_drawn(u8 x, u8 y) const;

   public:
    constexpr static usize ROWS = 32;
    constexpr static usize COLUMNS = 64;

   private:
    std::array<bool, ROWS* COLUMNS> m_frame_buffer = {0};

    usize index(u8 x, u8 y) const;

    friend std::ostream& operator<<(std::ostream& ostream, const FrameBuffer& display);
};

}  // namespace shvav8
