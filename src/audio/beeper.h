#pragma once

#include <SDL.h>
#include <defines.h>

#include <climits>
#include <vector>

namespace shvav8 {

class Beeper {
   public:
    Beeper(u32 frequency, i32 sample_frequency = 44100);
    ~Beeper();

    void play(u32 frequency);
    void play();
    void stop();

    bool playing() const;
    bool beeped() const;

   public:
    constexpr static f32 AMPLITUDE = 1.0f;
    constexpr static u16 SAMPLES = 1024;

   private:
    u32 m_freq;
    i32 m_sample_freq;
    u32 m_samples_per_wave;
    u32 m_sample_pos;
    bool m_playing;
    bool m_beeped;
    std::vector<f32> m_wave_vector;
    SDL_AudioDeviceID m_device;

    static bool s_initialized;

   private:
    void fill_sine_wave(std::vector<f32> &wave_vector) const;

    static void sdl_audio_callback(void *context, u8 *buffer, i32 length);
};

}  // namespace shvav8
