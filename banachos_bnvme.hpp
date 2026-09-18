#pragma once
#include <cstdint>
#include <string>
#include <algorithm>

class BanachOSNvmeController {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float NVME_LIMIT = 256.0f;

    BanachOSNvmeController() : nvme_leeway(LE_BASE) {}

    bool read_nvme_sector(uint64_t sector_lba, float read_intensity, std::string& out_msg) {
        if (read_intensity >= NVME_LIMIT || sector_lba == 0xFFFFFFFFFFFFFFFF) {
            nvme_leeway = LE_BASE;
            out_msg = "NVME_BLOCK_SHIELD: Сбой чтения сектора накопителя! Контроллер аппаратно сброшен (256 ≡ 0).";
            return false;
        }
        nvme_leeway = std::max(0.0f, LE_BASE * (1.0f - (read_intensity / NVME_LIMIT)));
        out_msg = "NVME_SUCCESS: Высокоскоростной блок данных детерминированно считан из флеш-памяти.";
        return true;
    }

    float get_nvme_leeway() const { return nvme_leeway; }

private:
    float nvme_leeway;
};
