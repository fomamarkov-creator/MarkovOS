#pragma once
#include <algorithm>

class BanachOSDiagnosticProbe {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float PROBE_LIMIT = 256.0f;
    BanachOSDiagnosticProbe() : probe_leeway(LE_BASE) {}

    bool verify_internal_lattice_strains(float structural_strain) {
        if (structural_strain >= PROBE_LIMIT) {
            probe_leeway = LE_BASE;
            return false;
        }
        probe_leeway = std::max(0.0f, LE_BASE * (1.0f - (structural_strain / PROBE_LIMIT)));
        return true;
    }
    float get_leeway() const { return probe_leeway; }
private:
    float probe_leeway;
};
