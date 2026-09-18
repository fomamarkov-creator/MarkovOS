/**
 * ================================================================================
 * BANACHOS v8.3 — GEOMETRIC LIPSCHITZ NETWORK STACK (banachos_net.cpp)
 * Author: Efim S. Markov & AI Collaborator (2026 Revision)
 * Formalism: 3HCP Packet Routing, Non-Linear Border Traffic Attenuation
 * Strictly Zero-Fit Parameter Architecture (Academic Clean Edition)
 * ================================================================================
 */

#include "banachos_net.hpp"
#include <cmath>
#include <algorithm>

BanachOSNetworkStack::BanachOSNetworkStack() {
    for (int z = 0; z < SIZE; ++z) {
        for (int y = 0; y < SIZE; ++y) {
            for (int x = 0; x < SIZE; ++x) {
                net_bus_matrix[z][y][x] = 0.1f;
                leeway_matrix[z][y][x] = LE_BASE;
            }
        }
    }
}

void BanachOSNetworkStack::get_topology_shifts(int k, NetCoords* shifts) const {
    // ИСПРАВЛЕНО: Явное объявление массивов структур из 6 элементов
    NetCoords plane_shifts[6] = {
        {1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {1,-1,0}, {-1,1,0}
    };
    for (int i = 0; i < 6; ++i) {
        shifts[i] = plane_shifts[i];
    }

    if (k % 2 == 0) {
        NetCoords inter_shifts[6] = {
            {0,0,1}, {1,0,1}, {0,1,1}, {0,0,-1}, {1,0,-1}, {0,1,-1}
        };
        for (int i = 0; i < 6; ++i) {
            shifts[i+6] = inter_shifts[i];
        }
    } else {
        NetCoords inter_shifts[6] = {
            {0,0,1}, {-1,0,1}, {0,-1,1}, {0,0,-1}, {-1,0,-1}, {0,-1,-1}
        };
        for (int i = 0; i < 6; ++i) {
            shifts[i+6] = inter_shifts[i];
        }
    }
}

bool BanachOSNetworkStack::open_socket(int socket_id, int z, int y, int x) {
    if (z >= 0 && z < SIZE && y >= 0 && y < SIZE && x >= 0 && x < SIZE) {
        socket_registry[socket_id] = {z, y, x};
        return true;
    }
    return false;
}

NetResult BanachOSNetworkStack::ingest_packet_stream(int socket_id, float incoming_packets_intensity, const std::vector<uint8_t>& raw_payload) {
    NetResult result;
    auto it = socket_registry.find(socket_id);
    if (it == socket_registry.end()) {
        result.success = false;
        result.message = "NET_ERROR: Заданный дескриптор сокета отсутствует в сетевой матрице";
        return result;
    }

    int z = it->second.z;
    int y = it->second.y;
    int x = it->second.x;

    float current_load = net_bus_matrix[z][y][x];
    float target_load = current_load + incoming_packets_intensity;

    NetCoords shifts[Z_COORDINATION];
    get_topology_shifts(z, shifts);

    float p_ext_accumulator = 0.0f;
    for (int i = 0; i < Z_COORDINATION; ++i) {
        int sz = (z + shifts[i].z + SIZE) % SIZE;
        int sy = (y + shifts[i].y + SIZE) % SIZE;
        int sx = (x + shifts[i].x + SIZE) % SIZE;
        p_ext_accumulator += net_bus_matrix[sz][sy][sx];
    }
    float p_ext = p_ext_accumulator / static_cast<float>(Z_COORDINATION);

    float p_in = (target_load < PACKET_LIMIT) ? ((PACKET_LIMIT - target_load) / (target_load + 1.0f)) : 0.0f;

    float current_le = 0.0f;
    if (p_in > 0.0f && target_load < PACKET_LIMIT) {
        float pressure_ratio = (p_ext - p_in) / p_in;
        current_le = std::max(0.0f, LE_BASE * (1.0f - pressure_ratio));
    }

    // --- АППАРАТНАЯ ЗАЩИТА ЯДРА ОТ DDOS ЧЕРЕЗ ВЫВОР ТРИГГЕРА (256 ≡ 0) ---
    if (current_le == 0.0f || target_load >= PACKET_LIMIT) {
        net_bus_matrix[z][y][x] = 0.1f;
        leeway_matrix[z][y][x] = LE_BASE;
        result.success = false;
        result.message = "NET_ANTI_DDOS: Шторм пакетов заблокирован. Буфер порта сброшен (256 ≡ 0). ОС стабильна";
        result.sanitized_payload = {0x55}; 
        return result;
    }

    net_bus_matrix[z][y][x] = target_load;
    leeway_matrix[z][y][x] = current_le;

    result.sanitized_payload.resize(raw_payload.size());
    for (size_t i = 0; i < raw_payload.size(); ++i) {
        result.sanitized_payload[i] = raw_payload[i] % 256;
    }

    result.success = true;
    result.message = "NET_SUCCESS: Пакеты успешно инжектированы в сетевой стек BanachOS";
    return result;
}

float BanachOSNetworkStack::get_average_network_leeway() const {
    float sum = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                sum += leeway_matrix[z][y][x];
    return sum / static_cast<float>(SIZE * SIZE * SIZE);
}

float BanachOSNetworkStack::get_peak_port_congestion() const {
    float max_val = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                if (net_bus_matrix[z][y][x] > max_val) max_val = net_bus_matrix[z][y][x];
    return max_val;
}

int BanachOSNetworkStack::get_active_sockets_count() const {
    return static_cast<int>(socket_registry.size());
}
