#pragma once

#include <array>
#include <iostream>
#include <vector>

#include "defines.h"

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
    bool draw(u32 x, u32 y);
    /**
     * Sets all the pixels on the display to 0.
     */
    void clear();

    std::vector<u32> get_drawn_pixels() const;
    bool is_drawn(u32 x, u32 y) const;

   public:
    constexpr static u32 ROWS = 32;
    constexpr static u32 COLUMNS = 64;
    constexpr static u32 PIXELS = ROWS * COLUMNS;

   private:
    std::array<bool, PIXELS> m_frame_buffer = {0};

    u32 index(u32 x, u32 y) const;

    friend std::ostream& operator<<(std::ostream& ostream, const FrameBuffer& display);
};

}  // namespace shvav8
