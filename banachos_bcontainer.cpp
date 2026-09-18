#include "banachos_bcontainer.hpp"
#include <cmath>
#include <algorithm>

BanachOSContainerManager::BanachOSContainerManager() {
    for (int z = 0; z < SIZE; ++z) {
        for (int y = 0; y < SIZE; ++y) {
            for (int x = 0; x < SIZE; ++x) {
                container_bus_matrix[z][y][x] = 0.1f;
                leeway_matrix[z][y][x] = LE_BASE;
            }
        }
    }
}

void BanachOSContainerManager::get_container_shifts(int k, ContainerCoords* shifts) const {
    // Исправлено: добавлен массив [] для plane_shifts
    ContainerCoords plane_shifts[] = {{1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {1,-1,0}, {-1,1,0}};
    for (int i = 0; i < 6; ++i) shifts[i] = plane_shifts[i];

    if (k % 2 == 0) {
        // Исправлено: добавлен массив [] для inter_shifts
        ContainerCoords inter_shifts[] = {{0,0,1}, {1,0,1}, {0,1,1}, {0,0,-1}, {1,0,-1}, {0,1,-1}};
        for (int i = 0; i < 6; ++i) shifts[i+6] = inter_shifts[i];
    } else {
        // Исправлено: добавлен массив [] для inter_shifts
        ContainerCoords inter_shifts[] = {{0,0,1}, {-1,0,1}, {0,-1,1}, {0,0,-1}, {-1,0,-1}, {0,-1,-1}};
        for (int i = 0; i < 6; ++i) shifts[i+6] = inter_shifts[i];
    }
}

bool BanachOSContainerManager::spawn_container_sandbox(int container_id, int z, int y, int x) {
    if (z >= 0 && z < SIZE && y >= 0 && y < SIZE && x >= 0 && x < SIZE) {
        active_containers[container_id] = {z, y, x};
        return true;
    }
    return false;
}

bool BanachOSContainerManager::cross_border_request(int container_id, float request_intensity, std::string& out_msg) {
    auto it = active_containers.find(container_id);
    if (it == active_containers.end()) {
        out_msg = "CONTAINER_ERROR: Заданный идентификатор песочницы отсутствует в реестре";
        return false;
    }

    int z = it->second.z;
    int y = it->second.y;
    int x = it->second.x;

    float current_stress = container_bus_matrix[z][y][x];
    float target_stress = current_stress + request_intensity;

    ContainerCoords shifts[Z_COORDINATION];
    get_container_shifts(z, shifts);

    float p_ext_accumulator = 0.0f;
    for (int i = 0; i < Z_COORDINATION; ++i) {
        int sz = (z + shifts[i].z + SIZE) % SIZE;
        int sy = (y + shifts[i].y + SIZE) % SIZE;
        int sx = (x + shifts[i].x + SIZE) % SIZE;
        p_ext_accumulator += container_bus_matrix[sz][sy][sx];
    }
    float p_ext = p_ext_accumulator / static_cast<float>(Z_COORDINATION);

    float p_in = (target_stress < STRESS_LIMIT) ? ((STRESS_LIMIT - target_stress) / (target_stress + 1.0f)) : 0.0f;

    float current_le = 0.0f;
    if (p_in > 0.0f && target_stress < STRESS_LIMIT) {
        float pressure_ratio = (p_ext - p_in) / p_in;
        current_le = std::max(0.0f, LE_BASE * (1.0f - pressure_ratio));
    }

    // --- АППАРАТНАЯ ИЗОЛЯЦИЯ ПРИ ПОПЫТКЕ ПОБЕГА (Триггер 256 ≡ 0) ---
    if (current_le == 0.0f || target_stress >= STRESS_LIMIT) {
        container_bus_matrix[z][y][x] = 0.1f;
        leeway_matrix[z][y][x] = LE_BASE;
        out_msg = "CONTAINER_ESCAPE_PREVENTED: Нарушение Липшицевых границ! Контейнер аннигилирован (256 ≡ 0)";
        return false;
    }

    container_bus_matrix[z][y][x] = target_stress;
    leeway_matrix[z][y][x] = current_le;
    out_msg = "CONTAINER_SUCCESS: Запрос ресурсов детерминированно выполнен в рамках Липшицевой нормы";
    return true;
}

int BanachOSContainerManager::get_active_sandboxes_count() const {
    return static_cast<int>(active_containers.size());
}

float BanachOSContainerManager::get_average_isolation_leeway() const {
    float sum = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                sum += leeway_matrix[z][y][x];
    return sum / static_cast<float>(SIZE * SIZE * SIZE);
}

float BanachOSContainerManager::get_peak_cross_border_stress() const {
    float max_val = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                if (container_bus_matrix[z][y][x] > max_val) max_val = container_bus_matrix[z][y][x];
    return max_val;
}
