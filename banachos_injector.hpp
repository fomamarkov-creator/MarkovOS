#ifndef BANACHOS_INJECTOR_HPP
#define BANACHOS_INJECTOR_HPP

#include <vector>
#include <cstdint>
#include <string>
#include "banachos_translator.hpp"

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

void run_injector_test();

#endif // BANACHOS_INJECTOR_HPP
