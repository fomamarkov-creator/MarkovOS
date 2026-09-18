#include "banachos_mem.hpp"
#include <cmath>
#include <algorithm>

BanachOSMemoryAllocator::BanachOSMemoryAllocator() {
    // Атомарная инициализация физических регистров ОЗУ в состояние Покоя
    for (int z = 0; z < SIZE; ++z) {
        for (int y = 0; y < SIZE; ++y) {
            for (int x = 0; x < SIZE; ++x) {
                memory_matrix[z][y][x] = 0.1f;
                leeway_tensor[z][y][x] = LE_BASE;
                allocation_mask[z][y][x] = false;
            }
        }
    }
}

void BanachOSMemoryAllocator::get_hcp_address_shifts(int k, HexCoords shifts[12]) const {
    // Нативная реализация 12 гексагональных осей по четности слоя Z
    HexCoords plane_shifts[6] = {{1,0,0}, {-1,0,0}, {0,1,0}, (HexCoords){0,-1,0}, {1,-1,0}, {-1,1,0}};
    for(int i = 0; i < 6; ++i) shifts[i] = plane_shifts[i];

    if (k % 2 == 0) {
        HexCoords inter[6] = {{0,0,1}, {1,0,1}, {0,1,1}, {0,0,-1}, {1,0,-1}, {0,1,-1}};
        for(int i = 0; i < 6; ++i) shifts[i+6] = inter[i];
    } else {
        HexCoords inter[6] = {{0,0,1}, {-1,0,1}, {0,-1,1}, {0,0,-1}, {-1,0,-1}, {0,-1,-1}};
        for(int i = 0; i < 6; ++i) shifts[i+6] = inter[i];
    }
}

bool BanachOSMemoryAllocator::allocate_cell(int z, int y, int x, float payload_bytes, std::string& out_message) {
    // Жесткая проверка границ физической адресации кристалла памяти
    if (z <= 0 || z >= SIZE - 1 || y <= 0 || y >= SIZE - 1 || x <= 0 || x >= SIZE - 1) {
        out_message = "REJECTED: Нарушение аппаратных границ адресации 3HCP";
        return false;
    }

    if (allocation_mask[z][y][x]) {
        out_message = "REJECTED: Ячейка ОЗУ монополизирована другим процессом";
        return false;
    }

    float core_rho = memory_matrix[z][y, x] + payload_bytes;

    // Сбор гидростатического давления от 12 соседей
    HexCoords shifts[12];
    get_hcp_address_shifts(z, shifts);
    float p_ext_accumulator = 0.0f;
    for (int i = 0; i < Z_COORDINATION; ++i) {
        p_ext_accumulator += memory_matrix[z + shifts[i].z][y + shifts[i].y][x + shifts[i].x];
    }
    float p_ext = p_ext_accumulator / static_cast<float>(Z_COORDINATION);

    // Внутреннее давление
    float p_in = (core_rho < REGISTER_LIMIT) ? ((REGISTER_LIMIT - core_rho) / (core_rho + 1.0f)) : 0.0f;

    float current_le = 0.0f;
    if (p_in > 0.0f && core_rho < REGISTER_LIMIT) {
        float pressure_ratio = (p_ext - p_in) / p_in;
        current_le = std::max(0.0f, LE_BASE * (1.0f - pressure_ratio));
    }

    // --- НАЛЕТУ: ЭФФЕКТ ВЫВОРОТА РЕГИСТРА НА КРЕМНИИ (256 ≡ 0) ---
    if (current_le == 0.0f || core_rho >= REGISTER_LIMIT) {
        memory_matrix[z][y][x] = 0.1f;
        leeway_tensor[z][y][x] = LE_BASE;
        allocation_mask[z][y][x] = false;
        out_message = "CRITICAL_LOCKUP: Аппаратный сброс буфера ячейки (256 ≡ 0). Атака Rowhammer подавлена.";
        return false;
    }

    // Фиксация данных в сотовой структуре кремния
    memory_matrix[z][y][x] = core_rho;
    leeway_tensor[z][y][x] = current_le
    allocation_mask[z][y][x] = true;
    out_message = "ALLOCATED: Квант данных успешно запечатан в физической соте";
    return true;
}

void BanachOSMemoryAllocator::free_cell(int z, int y, int x) {
    if (z >= 0 && z < SIZE && y >= 0 && y < SIZE && x >= 0 && x < SIZE) {
        memory_matrix[z][y][x] = 0.1f;
        leeway_tensor[z][y][x] = LE_BASE;
        allocation_mask[z][y][x] = false;
    }
}

int BanachOSMemoryAllocator::get_total_allocated_cells() const {
    int count = 0;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                if (allocation_mask[z][y][x]) count++;
    return count;
}

float BanachOSMemoryAllocator::get_mean_address_leeway() const {
    float sum = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                sum += leeway_tensor[z][y][x];
    return sum / static_cast<float>(SIZE * SIZE * SIZE);
}

float BanachOSMemoryAllocator::get_max_cell_load() const {
    float max_val = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                if (memory_matrix[z][y][x] > max_val) max_val = memory_matrix[z][y][x];
    return max_val;
}
