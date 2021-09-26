#include "beeper.h"

#include <SDL.h>

namespace shvav8 {

bool Beeper::s_initialized = false;

Beeper::Beeper(u32 frequency, i32 sample_frequency)
    : m_freq(frequency),
      m_sample_freq(sample_frequency),
      m_samples_per_wave(sample_frequency / frequency),
      m_sample_pos(0),
      m_playing(false),
      m_beeped(false),
      m_wave_vector(m_samples_per_wave) {
    if (!s_initialized) {
        if (SDL_Init(SDL_INIT_AUDIO) != 0) {
            std::stringstream message;
            message << "Failed to initialize SDL: " << SDL_GetError();
            throw std::runtime_error{message.str()};
        }

        s_initialized = true;
    }

    fill_sine_wave(m_wave_vector);

    SDL_AudioSpec want_spec, have_spec;

    SDL_zero(want_spec);
    want_spec.freq = m_sample_freq;
    want_spec.format = AUDIO_F32;
    want_spec.channels = 1;
    want_spec.samples = SAMPLES;
    want_spec.callback = sdl_audio_callback;
    want_spec.userdata = this;

    m_device = SDL_OpenAudioDevice(NULL, 0, &want_spec, &have_spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if (m_device == 0) {
        std::stringstream message;
        message << "Failed to open audio: " << SDL_GetError();
        throw std::runtime_error{message.str()};
    }
    if (want_spec.format != have_spec.format) {
        SDL_CloseAudioDevice(m_device);
        throw std::runtime_error{"Couldn't get Float32 audio format."};
    }
}

Beeper::~Beeper() {
    if (m_device) {
        SDL_CloseAudioDevice(m_device);
    }
}

void Beeper::play() {
    SDL_PauseAudioDevice(m_device, 0);
    m_playing = true;
    m_beeped = false;
}

void Beeper::stop() {
    SDL_PauseAudioDevice(m_device, 1);
    m_playing = false;
}

bool Beeper::playing() const { return m_playing; }
bool Beeper::beeped() const { return m_beeped; }
u32 Beeper::frequency() const { return m_freq; }
void Beeper::frequency(u32 frequency) {
    if (frequency == 0) {
        throw std::invalid_argument{"Frequency cannot be 0\n"};
    }

    m_freq = frequency;
    m_samples_per_wave = m_sample_freq / frequency;
    fill_sine_wave(m_wave_vector);
}

void Beeper::sdl_audio_callback(void *context, u8 *buffer, const i32 length) {
    Beeper *beeper = (Beeper *)context;

    f32 *samples = (f32 *)buffer;
    for (u16 i = 0; i < SAMPLES; ++i) {
        samples[i] = beeper->m_wave_vector[beeper->m_sample_pos % beeper->m_samples_per_wave];
        beeper->m_sample_pos += 1;
    }

    beeper->m_beeped = true;
}

void Beeper::fill_sine_wave(std::vector<f32> &wave_vector) const {
    if (wave_vector.size() < m_samples_per_wave) {
        wave_vector.resize(m_samples_per_wave);
    }

    for (u16 i = 0; i < m_samples_per_wave; ++i) {
        wave_vector[i] = AMPLITUDE * std::sin(2 * M_PI * i / m_samples_per_wave);
    }
}

}  // namespace shvav8
