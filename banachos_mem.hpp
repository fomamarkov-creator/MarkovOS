#pragma once
#include <cstdint>
#include <string>
#include <map>

/**
 * ================================================================================
 * BANACHOS v8.3 — NATIVE LIPSCHITZ MEMORY ALLOCATOR (bMEM.hpp)
 * Formalism: Volumetric Cell Allocation, 3HCP Topology, Bare-Metal C++ Edition
 * ================================================================================
 */

struct HexCoords {
    int z;
    int y;
    int x;
};

class BanachOSMemoryAllocator {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float REGISTER_LIMIT = 256.0f;
    static constexpr int Z_COORDINATION = 12;
    static constexpr int SIZE = 16;

    BanachOSMemoryAllocator();
    ~BanachOSMemoryAllocator() = default;

    bool allocate_cell(int z, int y, int x, float payload_bytes, std::string& out_message);
    void free_cell(int z, int y, int x);
    
    int get_total_allocated_cells() const;
    float get_mean_address_leeway() const;
    float get_max_cell_load() const;

private:
    // Прямые 3D-массивы в памяти вместо numpy. Плотное плоское размещение в куче
    float memory_matrix[SIZE][SIZE][SIZE];
    float leeway_tensor[SIZE][SIZE][SIZE];
    bool allocation_mask[SIZE][SIZE][SIZE];

    void get_hcp_address_shifts(int k, HexCoords shifts[12]) const;
};
