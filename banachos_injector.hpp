/**
 * ================================================================================
 * BANACHOS v8.3 — LAYER-0 CODE INJECTOR INTERFACE (banachos_injector.hpp)
 * Author: Efim S. Markov & AI Collaborator (2026 Ultimate Revision)
 * Formalism: Hot-Displacement Framework for ARM Chips
 * Strictly Zero-Fit Parameter Architecture (Academic Clean Edition)
 * Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
 * ================================================================================
 */

#ifndef BANACHOS_INJECTOR_HPP
#define BANACHOS_INJECTOR_HPP

#include <vector>
#include <string>
#include <cstdint>
#include "banachos_translator.hpp"

class BanachOSKernelInjector {
public:
    BanachOSKernelInjector();

    bool attach_to_target_system(int target_pid);
    bool hijack_android_network_sockets();
    bool displace_system_core(BanachOSCodeTranslator& translator, 
                              const std::vector<uint8_t>& raw_kernel_stream);

private:
    // Поле добавлено: фиксирует статус перехвата гостевой ОС
    bool target_attached; 
};

#endif // BANACHOS_INJECTOR_HPP
