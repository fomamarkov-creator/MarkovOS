/**
 * ================================================================================
 * BANACHOS v8.3 — FREQUENCY-DRIVEN WINDOW COMPOSITOR (banachos_bmux_wave.hpp)
 * Author: Efim S. Markov & AI Collaborator (2026 Ultimate Revision)
 * Formalism: Huygens Multiplexing, Shumann Window Frequency Superposition (Chapter 7.2)
 * Strictly Zero-Fit Parameter Architecture (Academic Clean Edition)
 * Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
 * ================================================================================
 */

#ifndef BANACHOS_BMUX_WAVE_HPP
#define BANACHOS_BMUX_WAVE_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <string>
#include <algorithm>
#include "banachos_bgraphics.hpp"

struct WindowWaveSource {
    int anchor_y;
    int anchor_x;
    float carrier_frequency; // Частотная мода Шумана, выделенная приложению (Fx, Fy или Fz)
    float amplitude;          // Интенсивность (яркость/прозрачность окна)
};

class BanachOSMuxWave {
public:
    static constexpr float L_E0 = 0.024f;            // Люфт упругости затухания пограничных слоев
    static constexpr float ZETA = 1.024f;            // Винтовой шаг фазы
    static constexpr int SCREEN_WIDTH = 128;
    static constexpr int SCREEN_HEIGHT = 128;

    BanachOSMuxWave() {}

    /**
     * Регистрация нового гостевого окна в частотной матрице композитора
     */
    void register_window_wave(int window_id, int y, int x, float freq, float alpha_amplitude) {
        active_windows[window_id] = {y, x, freq, std::clamp(alpha_amplitude, 0.0f, 1.0f)};
    }

    /**
     * --- ЭТАП 2: ЧАСТОТНЫЙ КОМПОЗИТИНГ ОКON bMUX_WAVE ---
     * Накладывает и смешивает гармоники всех окон напрямую во фреймбуфер bGRAPHICS.
     * Эффект прозрачности и размытия получается как естественная интерференция волн.
     */
    void compose_framebuffer_frequencies(BanachOSWaveGraphics& graphics) {
        // Проходим по всей кристаллической сетке VRAM дисплея
        for (int y = 0; y < SCREEN_HEIGHT; ++y) {
            for (int x = 0; x < SCREEN_WIDTH; ++x) {
                float total_interference_signal = 0.0f;

                // Суперпозиция полей: складываем волновые вклады от каждого активного окна
                for (const auto& [id, win] : active_windows) {
                    float distance = std::sqrt(std::pow(y - win.anchor_y, 2) + std::pow(x - win.anchor_x, 2));
                    if (distance < 1.0f) distance = 1.0f;

                    // Уравнение частотного наложения из Главы 7.2:
                    // Сигнал затухает с расстоянием, модулируясь несущей частотой Шумана и зазором Покоя
                    float phase = (distance * win.carrier_frequency * ZETA) - L_E0;
                    float wave_contribution = (win.amplitude * 100.0f / distance) * std::sin(phase);

                    total_interference_signal += wave_contribution;
                }

                // Инжектируем результирующий суммарный волновой фронт обратно в пиксель
                if (!active_windows.empty()) {
                    int floored_excitation = static_cast<int>(std::floor(total_interference_signal));
                    // Квантование цвета результирующего кадра в кольце вычетов Z/256Z
                    int final_color = (floored_excitation % 256 + 256) % 256;
                    
                    // Передаем скомпонованный пиксель в волновой движок графики
                    graphics.inject_graphics_pulse(y, x, static_cast<float>(final_color) * 0.01f);
                }
            }
        }
    }

    void clear_compositor() {
        active_windows.clear();
    }

private:
    std::map<int, WindowWaveSource> active_windows; // Карта частотных источников окон
};

#endif // BANACHOS_BMUX_WAVE_HPP
