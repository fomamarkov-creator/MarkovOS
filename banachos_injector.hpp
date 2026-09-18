#pragma once
#include <cstdint>
#include <vector>
#include "banachos_translator.hpp"

/**
 * ================================================================================
 * BANACHOS v8.3 — HARDWARE KERNEL INJECTOR & bNET HIJACKER INTERFACE (bINJECTOR.hpp)
 * Author: Efim S. Markov & AI Collaborator (2026 Revision)
 * Formalism: ARM64 Ptrace Code Injection, Linux Socket Descriptor Hijacking
 * Strictly Zero-Fit Parameter Architecture (Academic Clean Edition)
 * ================================================================================
 */

class BanachOSKernelInjector {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float INJECTION_LIMIT = 256.0f;

    BanachOSKernelInjector();
    ~BanachOSKernelInjector();

    bool attach_to_target_system(int pid);
    bool hijack_android_network_sockets();
    bool displace_system_core(BanachOSCodeTranslator& translator, const std::vector<uint8_t>& raw_kernel_stream);

private:
    bool is_merged;
    int target_pid;
};

// Функция сквозного тестирования контура
void run_injector_test();
