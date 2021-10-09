#include "fps.h"

using namespace std::chrono;

namespace shvav8 {
Fps::Fps(f64 fps)
    : m_loops(0), m_wanted_fps(fps), m_current_fps(-1), m_clock(high_resolution_clock::now()) {}

void Fps::update() {
    auto delta = duration_cast<milliseconds>(high_resolution_clock::now() - m_clock).count();
    f64 new_fps = 1000.0 / delta;
    m_loops = m_loops - std::floor(m_loops);

    if (m_current_fps == -1) {
        m_current_fps = new_fps;
    } else {
        m_current_fps = 0.5 * m_current_fps + 0.5 * new_fps;
    }

    m_loops += m_wanted_fps / m_current_fps;
    m_clock = high_resolution_clock::now();
}

u32 Fps::loops() const { return m_loops; }

}  // namespace shvav8
