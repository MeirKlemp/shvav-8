#include "beeper.h"

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
            std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
            return;
        }

        s_initialized = true;
    }

    fill_sine_wave(m_wave_vector);

    SDL_AudioSpec wantSpec, haveSpec;

    SDL_zero(wantSpec);
    wantSpec.freq = m_sample_freq;
    wantSpec.format = AUDIO_F32;
    wantSpec.channels = 1;
    wantSpec.samples = SAMPLES;
    wantSpec.callback = sdl_audio_callback;
    wantSpec.userdata = this;

    m_device = SDL_OpenAudioDevice(NULL, 0, &wantSpec, &haveSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if (m_device == 0) {
        std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
        return;
    }
    if (wantSpec.format != haveSpec.format) {
        std::cerr << "Couldn't get Float32 audio format.\n";
        return;
    }
}

Beeper::~Beeper() { SDL_CloseAudioDevice(m_device); }

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
        std::cerr << "Frequency cannot be 0\n";
        return;
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
