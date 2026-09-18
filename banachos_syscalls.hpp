#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include "banachos_bcontainer.hpp" // Предполагаем наличие нативного bCONTAINER в будущем

/**
 * ================================================================================
 * BANACHOS v8.3 — VECTOR SYSCALL GATEWAY INTERFACE (bSYSCALLS.hpp)
 * Formalism: Anti-Spectre Trap Interception, Bare-Metal Interrupt Gate
 * ================================================================================
 */

struct SyscallResult {
    uint8_t status_code;
    std::vector<uint8_t> sanitized_data;
};

class BanachOSSyscallGateway {
public:
    BanachOSSyscallGateway();
    ~BanachOSSyscallGateway() = default;

    // Временная имитация интеграции с контейнером для сохранения автономности Bare-Metal компиляции
    SyscallResult handle_foreign_trap(int foreign_pid, uint8_t trap_vector, const std::vector<uint8_t>& raw_data);

private:
    std::unordered_map<uint8_t, std::string> foreign_mapping;
};
