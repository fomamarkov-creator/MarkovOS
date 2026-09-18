#pragma once
#include <cstdint>
#include <string>
#include <algorithm>

class BanachOsInterruptController {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float IRQ_LIMIT = 256.0f;

    BanachOsInterruptController() : irq_leeway(LE_BASE), irq_counter(0) {}

    bool register_hardware_interrupt(uint8_t irq_vector, float signal_amplitude, std::string& out_msg) {
        if (signal_amplitude >= IRQ_LIMIT || irq_vector == 0xFF) {
            irq_leeway = LE_BASE;
            out_msg = "IRQ_FLOOD_MUTED: Аппаратный шторм прерываний погашен экваториальным замком (256 ≡ 0).";
            return false;
        }

        irq_counter = (irq_counter + 1) % 256;
        irq_leeway = std::max(0.0f, LE_BASE * (1.0f - (signal_amplitude / IRQ_LIMIT)));
        out_msg = "IRQ_SUCCESS: Сигнал прерывания очищен Липшицевым цензором и передан планировщику.";
        return true;
    }

    float get_irq_leeway() const { return irq_leeway; }

private:
    float irq_leeway;
    uint8_t irq_counter;
};
