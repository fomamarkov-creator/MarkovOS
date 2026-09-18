#include "banachos_bproc.hpp"
#include <cmath>
#include <algorithm>

BanachOSProcessManager::BanachOSProcessManager() {
    for (int z = 0; z < SIZE; ++z) {
        for (int y = 0; y < SIZE; ++y) {
            for (int x = 0; x < SIZE; ++x) {
                proc_stress_matrix[z][y][x] = 0.1f;
                leeway_matrix[z][y][x] = LE_BASE;
            }
        }
    }
}

void BanachOSProcessManager::get_proc_shifts(int k, ProcCoords* shifts) const {
    ProcCoords plane_shifts = {{1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {1,-1,0}, {-1,1,0}};
    for (int i = 0; i < 6; ++i) shifts[i] = plane_shifts[i];

    if (k % 2 == 0) {
        ProcCoords inter_shifts = {{0,0,1}, {1,0,1}, {0,1,1}, {0,0,-1}, {1,0,-1}, {0,1,-1}};
        for (int i = 0; i < 6; ++i) shifts[i+6] = inter_shifts[i];
    } else {
        ProcCoords inter_shifts = {{0,0,1}, {-1,0,1}, {0,-1,1}, {0,0,-1}, {-1,0,-1}, {0,-1,-1}};
        for (int i = 0; i < 6; ++i) shifts[i+6] = inter_shifts[i];
    }
}

bool BanachOSProcessManager::spawn_process_node(int pid, int z, int y, int x) {
    if (z >= 0 && z < SIZE && y >= 0 && y < SIZE && x >= 0 && x < SIZE) {
        process_table[pid] = {z, y, x};
        return true;
    }
    return false;
}

bool BanachOSProcessManager::update_process_state(int pid, float task_execution_stress, std::string& out_message) {
    auto it = process_table.find(pid);
    if (it == process_table.end()) {
        out_message = "PROC_ERROR: Заданный PID отсутствует в системной таблице процессов";
        return false;
    }

    int z = it->second.z;
    int y = it->second.y;
    int x = it->second.x;

    float current_stress = proc_stress_matrix[z][y][x];
    float target_stress = current_stress + task_execution_stress;

    ProcCoords shifts[Z_COORDINATION];
    get_proc_shifts(z, shifts);

    float p_ext_accumulator = 0.0f;
    for (int i = 0; i < Z_COORDINATION; ++i) {
        int sz = (z + shifts[i].z + SIZE) % SIZE;
        int sy = (y + shifts[i].y + SIZE) % SIZE;
        int sx = (x + shifts[i].x + SIZE) % SIZE;
        p_ext_accumulator += proc_stress_matrix[sz][sy][sx];
    }
    float p_ext = p_ext_accumulator / static_cast<float>(Z_COORDINATION);

    float p_in = (target_stress < LOAD_LIMIT) ? ((LOAD_LIMIT - target_stress) / (target_stress + 1.0f)) : 0.0f;

    float current_le = 0.0f;
    if (p_in > 0.0f && target_stress < LOAD_LIMIT) {
        float pressure_ratio = (p_ext - p_in) / p_in;
        current_le = std::max(0.0f, LE_BASE * (1.0f - pressure_ratio));
    }

    // --- АППАРАТНАЯ ИЗОЛЯЦИЯ И СБРОС ЗАВИСШЕГО ПРОЦЕССА (Триггер 256 ≡ 0) ---
    if (current_le == 0.0f || target_stress >= LOAD_LIMIT) {
        proc_stress_matrix[z][y][x] = 0.1f;
        leeway_matrix[z][y][x] = LE_BASE;
        out_message = "PROC_TERMINATED: Обнаружен Deadlock! Поток принудительно терминирован (256 ≡ 0)";
        return false;
    }

    proc_stress_matrix[z][y][x] = target_stress;
    leeway_matrix[z][y][x] = current_le;
    out_message = "PROC_SUCCESS: Квант времени ЦП детерминированно выделен задаче";
    return true;
}

float BanachOSProcessManager::get_average_scheduler_leeway() const {
    float sum = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                sum += leeway_matrix[z][y][x];
    return sum / static_cast<float>(SIZE * SIZE * SIZE);
}

float BanachOSProcessManager::get_peak_process_stress_load() const {
    float max_val = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                if (proc_stress_matrix[z][y][x] > max_val) max_val = proc_stress_matrix[z][y][x];
    return max_val;
}

int BanachOSProcessManager::get_active_pids_count() const {
    return static_cast<int>(process_table.size());
}
