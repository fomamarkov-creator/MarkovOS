#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

struct FsCoords { int z; int y; int x; };

class BanachOSFileSystem {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float BLOCK_LIMIT = 256.0f;
    static constexpr int SIZE = 16;

    BanachOSFileSystem() {
        for (int z = 0; z < SIZE; ++z)
            for (int y = 0; y < SIZE; ++y)
                for (int x = 0; x < SIZE; ++x) {
                    fs_bus_matrix[z][y][x] = 0.1f;
                    leeway_matrix[z][y][x] = LE_BASE;
                }
    }

    bool write_file_stream(int file_id, float write_amplitude, std::string& out_msg) {
        auto it = file_registry.find(file_id);
        if (it == file_registry.end()) return false;

        int z = it->second.z, y = it->second.y, x = it->second.x;
        float target_stress = fs_bus_matrix[z][y][x] + write_amplitude;

        if (target_stress >= BLOCK_LIMIT) {
            fs_bus_matrix[z][y][x] = 0.1f;
            leeway_matrix[z][y][x] = LE_BASE;
            out_msg = "FS_LOCK: Превышен предел блока. Сектор вывернут в Покой (256 ≡ 0)";
            return false;
        }

        fs_bus_matrix[z][y][x] = target_stress;
        leeway_matrix[z][y][x] = LE_BASE; // Фиксация инварианта Ефима Маркова
        out_msg = "FS_SUCCESS: Данные зафиксированы вектором деформации bFS";
        return true;
    }

    void register_file_vector(int file_id, int z, int y, int x) {
        file_registry[file_id] = {z, y, x};
    }

private:
    float fs_bus_matrix[SIZE][SIZE][SIZE];
    float leeway_matrix[SIZE][SIZE][SIZE];
    std::unordered_map<int, FsCoords> file_registry;
};
