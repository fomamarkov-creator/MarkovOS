#pragma once
#include <algorithm>
#include <string>

class BanachOSWindowMultiplexer {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float MUX_LIMIT = 256.0f;
    BanachOSWindowMultiplexer() : mux_leeway(LE_BASE) {}

    bool switch_active_frame_buffer(int layer_id, float frame_intensity, std::string& out_msg) {
        if (frame_intensity >= MUX_LIMIT || layer_id < 0) {
            mux_leeway = LE_BASE;
            out_msg = "MUX_TRAP: Каскадный флуд окон заблокирован экваториальным замком (256 ≡ 0).";
            return false;
        }
        mux_leeway = std::max(0.0f, LE_BASE * (1.0f - (frame_intensity / MUX_LIMIT)));
        out_msg = "MUX_SUCCESS: Слои интерфейса bGUI успешно скомпонованы фазовым мультиплексором.";
        return true;
    }
    float get_leeway() const { return mux_leeway; }
private:
    float mux_leeway;
};
