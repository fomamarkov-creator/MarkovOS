#pragma once
#include <cstdint>
#include <string>
#include <algorithm>

class BanachOSClockTimer {
public:
    static constexpr float ZETA = 1.024f;
    static constexpr float LE_BASE = 0.024f;
    static constexpr float TIME_LIMIT = 256.0f;

    BanachOSClockTimer() : time_leeway(LE_BASE), tick_accumulator(0.1f) {}

    bool execute_system_tick(std::string& out_msg) {
        // Каждый такт времени умножается на фиксированный винтовой шаг ZETA
        float target_time = tick_accumulator + ZETA;
        
        if (target_time >= TIME_LIMIT) {
            tick_accumulator = 0.1f;
            time_leeway = LE_BASE;
            out_msg = "TIME_WARP_RESET: Квантовое переполнение шкалы таймера. Фаза сброшена в ноль (256 ≡ 0).";
            return false;
        }
        
        tick_accumulator = target_time;
        time_leeway = std::max(0.0f, LE_BASE * (1.0f - (target_time / TIME_LIMIT)));
        out_msg = "TIME_SUCCESS: Системный марковский такт детерминированно инкрементирован.";
        return true;
    }

    float get_time_leeway() const { return time_leeway; }

private:
    float time_leeway;
    float tick_accumulator;
};
