#pragma once
#include <cstdint>
#include <string>
#include <algorithm>

class BanachOSBusController {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float BUS_LIMIT = 256.0f;

    BanachOSBusController() : bus_leeway(LE_BASE), traffic_density(0.1f) {}

    bool transmit_interchip_command(uint32_t command_id, float amplitude, std::string& out_msg) {
        float target_stress = traffic_density + amplitude;
        if (target_stress >= BUS_LIMIT || command_id == 0xFFFFFFFF) {
            traffic_density = 0.1f;
            bus_leeway = LE_BASE;
            out_msg = "BUS_COLLISION_MUTED: Аномальный резонанс шины локализован. Регистр вывернут в 0.1 (256 ≡ 0).";
            return false;
        }
        traffic_density = target_stress;
        bus_leeway = std::max(0.0f, LE_BASE * (1.0f - (target_stress / BUS_LIMIT)));
        out_msg = "BUS_SUCCESS: Системная команда успешно транслирована по межкристальной шине bBUS.";
        return true;
    }

    float get_bus_leeway() const { return bus_leeway; }

private:
    float bus_leeway;
    float traffic_density;
};
