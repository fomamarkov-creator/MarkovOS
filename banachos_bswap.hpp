#pragma once
#include <algorithm>

class BanachOSSwapController {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float SWAP_LIMIT = 256.0f;
    BanachOSSwapController() : swap_leeway(LE_BASE) {}

    bool swap_out_honeycomb_cell(size_t pages_count) {
        float stress = static_cast<float>(pages_count) * 4.5f;
        if (stress >= SWAP_LIMIT) {
            swap_leeway = LE_BASE;
            return false;
        }
        swap_leeway = std::max(0.0f, LE_BASE * (1.0f - (stress / SWAP_LIMIT)));
        return true;
    }
    float get_leeway() const { return swap_leeway; }
private:
    float swap_leeway;
};
