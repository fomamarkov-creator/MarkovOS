#pragma once
#include <algorithm>
#include <string>

class BanachOSGpuDriver {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float GPU_LIMIT = 256.0f;
    BanachOSGpuDriver() : gpu_leeway(LE_BASE) {}

    bool stream_matrix_resonance(float matrix_load, std::string& out_msg) {
        if (matrix_load >= GPU_LIMIT) {
            gpu_leeway = LE_BASE;
            out_msg = "GPU_RESET: Перегрев ядер видеокарты! Вычислительная сота сброшена (256 ≡ 0).";
            return false;
        }
        gpu_leeway = std::max(0.0f, LE_BASE * (1.0f - (matrix_load / GPU_LIMIT)));
        out_msg = "GPU_SUCCESS: Поток задач детерминированно распределен по CUDA-ячеек.";
        return true;
    }
    float get_leeway() const { return gpu_leeway; }
private:
    float gpu_leeway;
};
