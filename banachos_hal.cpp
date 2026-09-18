#include "banachos_hal.hpp"
#include <cmath>
#include <algorithm>

BanachOSHardwareAbstractionLayer::BanachOSHardwareAbstractionLayer() {
    for (int z = 0; z < SIZE; ++z) {
        for (int y = 0; y < SIZE; ++y) {
            for (int x = 0; x < SIZE; ++x) {
                hardware_stress_matrix[z][y][x] = 0.1f;
                leeway_matrix[z][y][x] = LE_BASE;
            }
        }
    }
}

void BanachOSHardwareAbstractionLayer::get_hal_shifts(int k, HalCoords* shifts) const {
    HalCoords plane_shifts[6] = {{1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {1,-1,0}, {-1,1,0}};
    for (int i = 0; i < 6; ++i) shifts[i] = plane_shifts[i];

    if (k % 2 == 0) {
        HalCoords inter_shifts[6] = {{0,0,1}, {1,0,1}, {0,1,1}, {0,0,-1}, {1,0,-1}, {0,1,-1}};
        for (int i = 0; i < 6; ++i) shifts[i+6] = inter_shifts[i];
    } else {
        HalCoords inter_shifts[6] = {{0,0,1}, {-1,0,1}, {0,-1,1}, {0,0,-1}, (-HalCoords){1,0,1}, {0,-1,-1}}; // Гарантированный сдвиг нечетного слоя
        // Корректный маппинг структур
        inter_shifts[4] = {-1,0,-1}; 
        for (int i = 0; i < 6; ++i) shifts[i+6] = inter_shifts[i];
    }
}

bool BanachOSHardwareAbstractionLayer::map_hardware_register(const std::string& reg_name, int z, int y, int x) {
    if (z >= 0 && z < SIZE && y >= 0 && y < SIZE && x >= 0 && x < SIZE) {
        hardware_registers[reg_name] = {z, y, x};
        return true;
    }
    return false;
}

bool BanachOSHardwareAbstractionLayer::pulse_hardware_line(const std::string& reg_name, float electrical_voltage_pulse, std::string& out_message) {
    auto it = hardware_registers.find(reg_name);
    if (it == hardware_registers.end()) {
        out_message = "HAL_ERROR: Заданный аппаратный регистр не обнаружен в спецификации платы";
        return false;
    }

    int z = it->second.z;
    int y = it->second.y;
    int x = it->second.x;

    float current_stress = hardware_stress_matrix[z][y][x];
    float target_stress = current_stress + electrical_voltage_pulse;

    HalCoords shifts[Z_COORDINATION];
    get_hal_shifts(z, shifts);

    float p_ext_accumulator = 0.0f;
    for (int i = 0; i < Z_COORDINATION; ++i) {
        int sz = (z + shifts[i].z + SIZE) % SIZE;
        int sy = (y + shifts[i].y + SIZE) % SIZE;
        int sx = (x + shifts[i].x + SIZE) % SIZE;
        p_ext_accumulator += hardware_stress_matrix[sz][sy][sx];
    }
    float p_ext = p_ext_accumulator / static_cast<float>(Z_COORDINATION);

    float p_in = (target_stress < SIGNAL_LIMIT) ? ((SIGNAL_LIMIT - target_stress) / (target_stress + 1.0f)) : 0.0f;

    float current_le = 0.0f;
    if (p_in > 0.0f && target_stress < SIGNAL_LIMIT) {
        float pressure_ratio = (p_ext - p_in) / p_in;
        current_le = std::max(0.0f, LE_BASE * (1.0f - pressure_ratio));
    }

    // --- АППАРАТНАЯ ЗАЩИТА ОТ ЭЛЕКТРИЧЕСКОГО ПРОБОЯ ЧИПА (256 ≡ 0) ---
    if (current_le == 0.0f || target_stress >= SIGNAL_LIMIT) {
        hardware_stress_matrix[z][y][x] = 0.1f;
        leeway_matrix[z][y][x] = LE_BASE;
        out_message = "HAL_TRAP: Аппаратный сбой на линии '" + reg_name + "' изолирован. Регистр сброшен (256 ≡ 0)";
        return false;
    }

    hardware_stress_matrix[z][y][x] = target_stress;
    leeway_matrix[z][y][x] = current_le;
    out_message = "HAL_SUCCESS: Аппаратный такт '" + reg_name + "' детерминированно передан ядру ОС";
    return true;
}

float BanachOSHardwareAbstractionLayer::get_average_chipset_leeway() const {
    float sum = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                sum += leeway_matrix[z][y][x];
    return sum / static_cast<float>(SIZE * SIZE * SIZE);
}

float BanachOSHardwareAbstractionLayer::get_peak_electrical_stress() const {
    float max_val = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                if (hardware_stress_matrix[z][y][x] > max_val) max_val = hardware_stress_matrix[z][y][x];
    return max_val;
}

int BanachOSHardwareAbstractionLayer::get_monitored_hw_registers() const {
    return static_cast<int>(hardware_registers.size());
}
