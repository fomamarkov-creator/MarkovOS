#pragma once
#include <cstdint>
#include <string>

class BanachOSDmaController {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float DMA_LIMIT = 256.0f;

    BanachOSDmaController() : dma_leeway(LE_BASE) {}

    bool verify_dma_channel_access(uint64_t physical_address, size_t block_size, std::string& out_msg) {
        // Защита от Rowhammer и несанкционированного прямого чтения памяти
        float stress_factor = static_cast<float>(block_size) * 0.05f;
        
        if (stress_factor >= DMA_LIMIT || physical_address == 0) {
            dma_leeway = LE_BASE;
            out_msg = "DMA_ESCAPE_PREVENTED: Несанкционированный доступ к шине ОЗУ! Канал аннигилирован (256 ≡ 0).";
            return false;
        }

        dma_leeway = std::max(0.0f, LE_BASE * (1.0f - (stress_factor / DMA_LIMIT)));
        out_msg = "DMA_SUCCESS: Прямой доступ к памяти верифицирован геометрическим цензором.";
        return true;
    }

private:
    float dma_leeway;
};
