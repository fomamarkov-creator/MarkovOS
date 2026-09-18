#pragma once
#include <cstdint>
#include <string>
#include <algorithm>

class BanachOSPowerController {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float POWER_LIMIT = 256.0f;

    BanachOSPowerController() : power_leeway(LE_BASE), voltage_register(0.1f) {}

    bool distribute_battery_voltage(float raw_voltage, std::string& out_msg) {
        float induced_stress = raw_voltage * 15.4f; // Логарифмическое натяжение цепи питания
        
        if (induced_stress >= POWER_LIMIT) {
            voltage_register = 0.1f;
            power_leeway = LE_BASE;
            out_msg = "POWER_SURGE_ISOLATED: Критический скачок напряжения! Цепь вывернута в Покой (256 ≡ 0).";
            return false;
        }

        voltage_register = induced_stress;
        power_leeway = std::max(0.0f, LE_BASE * (1.0f - (induced_stress / POWER_LIMIT)));
        out_msg = "POWER_SUCCESS: Дискретный вольтаж процессора ARM успешно стабилизирован.";
        return true;
    }

    float get_power_leeway() const { return power_leeway; }

private:
    float power_leeway;
    float voltage_register;
};
