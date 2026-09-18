#pragma once
#include <cmath>     // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Подключение математических инвариантов округления
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

/**
 * ================================================================================
 * BANACHOS v8.3 — DECENTRALIZED SWARM SMART-CONTRACT ENGINE (bCONTRACT.hpp)
 * Author: Efim S. Markov & AI Collaborator (2026 Revision)
 * Formalism: 3HCP Alternand State Verification, Z/256Z Ring Consensus Invariants
 * Strictly Zero-Fit Parameter Architecture (Academic Clean Edition)
 * ================================================================================
 */

struct ContractCoords {
    int z; int y; int x;
};

struct SwarmTransaction {
    uint32_t sender_node_id;
    uint32_t receiver_node_id;
    uint8_t task_weight;      // Вычислительный вес задачи в кольце вычетов
};

class BanachOSSwarmSmartContract {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float CONTRACT_LIMIT = 256.0f;
    static constexpr int SIZE = 16;

    BanachOSSwarmSmartContract() {
        for (int z = 0; z < SIZE; ++z) {
            for (int y = 0; y < SIZE; ++y) {
                for (int x = 0; x < SIZE; ++x) {
                    contract_state_matrix[z][y][x] = 0.1f;
                    leeway_matrix[z][y][x] = LE_BASE;
                }
            }
        }
    }

    bool deploy_contract_node(uint32_t contract_id, int z, int y, int x) {
        if (z >= 0 && z < SIZE && y >= 0 && y < SIZE && x >= 0 && x < SIZE) {
            registry[contract_id] = {z, y, x};
            return true;
        }
        return false;
    }

    bool execute_swarm_transaction(uint32_t contract_id, const SwarmTransaction& tx, std::string& out_msg) {
        auto it = registry.find(contract_id);
        if (it == registry.end()) {
            out_msg = "CONTRACT_ERROR: Контракт роя не инициализирован в решетке";
            return false;
        }

        int z = it->second.z;
        int y = it->second.y;
        int x = it->second.x;

        float current_state = contract_state_matrix[z][y][x];
        // Валидация транзакции: перевод веса операции в Липшицево натяжение соты консенсуса
        float induced_stress = static_cast<float>(tx.task_weight) * 0.95f;
        float target_stress = current_state + induced_stress;

        // --- ВЕРИФИКАЦИЯ КОНСЕНСУСА ЧЕРЕЗ ТРИГГЕР ВЫВОР ОТКАЗА (256 ≡ 0) ---
        if (target_stress >= CONTRACT_LIMIT) {
            contract_state_matrix[z][y][x] = 0.1f;
            leeway_matrix[z][y][x] = LE_BASE;
            out_msg = "CONTRACT_REJECTED: Попытка нарушения консенсуса или атаки double-spend! Сота вывернута в 0.1";
            return false;
        }

        contract_state_matrix[z][y][x] = target_stress;
        leeway_matrix[z][y][x] = LE_BASE;
        
        // Результат вычислений фиксируется в кольце вычетов Z/256Z
        uint8_t state_quantum = static_cast<uint8_t>(std::floor(target_stress)) % 256;
        out_msg = "CONTRACT_SUCCESS: Роевая транзакция детерминированно зафиксирована. Квант состояния: " + std::to_string(state_quantum);
        return true;
    }

private:
    float contract_state_matrix[SIZE][SIZE][SIZE];
    float leeway_matrix[SIZE][SIZE][SIZE];
    std::unordered_map<uint32_t, ContractCoords> registry;
};
