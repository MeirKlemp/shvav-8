#pragma once

#include <chrono>

#include "defines.h"

namespace shvav8 {
class Fps {
   public:
    Fps(f64 fps);

    u32 loops() const;
    void update();

   private:
    f64 m_loops;
    f64 m_wanted_fps;
    f64 m_current_fps;
    std::chrono::high_resolution_clock::time_point m_clock;
};
}  // namespace shvav8
