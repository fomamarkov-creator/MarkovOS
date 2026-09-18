#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

/**
 * ================================================================================
 * BANACHOS v8.3 — HARDWARE ABSTRACTION LAYER & BOOT REGISTERS (bHAL.hpp)
 * Formalism: 3HCP Physical Frame Mapping, ARMv8-A Peripheral Signal Hardening
 * Strictly Zero-Fit Parameter Architecture (Academic Clean Edition)
 * ================================================================================
 */

struct HalCoords {
    int z;
    int y;
    int x;
};

class BanachOSHardwareAbstractionLayer {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float SIGNAL_LIMIT = 256.0f;
    static constexpr int Z_COORDINATION = 12;
    static constexpr int SIZE = 16;

    BanachOSHardwareAbstractionLayer();
    ~BanachOSHardwareAbstractionLayer() = default;

    bool map_hardware_register(const std::string& reg_name, int z, int y, int x);
    bool pulse_hardware_line(const std::string& reg_name, float electrical_voltage_pulse, std::string& out_message);
    
    float get_average_chipset_leeway() const;
    float get_peak_electrical_stress() const;
    int get_monitored_hw_registers() const;

private:
    float hardware_stress_matrix[SIZE][SIZE][SIZE];
    float leeway_matrix[SIZE][SIZE][SIZE];
    
    // Реестр зарегистрированных физических портов
    std::unordered_map<std::string, HalCoords> hardware_registers;

    void get_hal_shifts(int k, HalCoords* shifts) const;
};
