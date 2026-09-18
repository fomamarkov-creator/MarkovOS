#pragma once
#include <algorithm>
#include <string>

class BanachOSFanController {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float THERMAL_LIMIT = 256.0f;
    BanachOSFanController() : fan_leeway(LE_BASE) {}

    bool compute_pwm_rotation(float cauchy_stress, std::string& out_msg) {
        if (cauchy_stress >= THERMAL_LIMIT) {
            fan_leeway = LE_BASE;
            out_msg = "FAN_CRITICAL: Тепловой шторм кремния! Кулер запущен на 100% мощности (256 ≡ 0).";
            return false;
        }
        fan_leeway = std::max(0.0f, LE_BASE * (1.0f - (cauchy_stress / THERMAL_LIMIT)));
        out_msg = "FAN_SUCCESS: ШИМ-такт кулера сбалансирован по тензору Коши-Рыбникова.";
        return true;
    }
    float get_leeway() const { return fan_leeway; }
private:
    float fan_leeway;
};
