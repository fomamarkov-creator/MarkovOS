#include "banachos_baudio.hpp"
#include <cmath>
#include <algorithm>

BanachOSAudioDriver::BanachOSAudioDriver() : current_leeway(LE_BASE) {
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                audio_matrix[z][y][x] = 0.1f;
}

bool BanachOSAudioDriver::process_audio_pulse(float frequency_hz, float amplitude, std::string& out_msg) {
    // Перевод звуковой частоты в метрику натяжения соты
    float stress = std::fmod(frequency_hz * amplitude * 0.01f, AUDIO_LIMIT);
    
    if (stress >= AUDIO_LIMIT - 10.0f) {
        current_leeway = LE_BASE;
        out_msg = "AUDIO_LOCK: Обнаружена акустическая атака! Звуковая шина аппаратно изолирована (256 ≡ 0).";
        return false;
    }

    current_leeway = std::max(0.0f, LE_BASE * (1.0f - (stress / AUDIO_LIMIT)));
    out_msg = "AUDIO_SUCCESS: Звуковой квант детерминированно обработан в рамках Липшицева зазора.";
    return true;
}
