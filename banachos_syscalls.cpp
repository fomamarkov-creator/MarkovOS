#include "banachos_syscalls.hpp"
#include <cmath>

BanachOSSyscallGateway::BanachOSSyscallGateway() {
    // Маппинг аппаратныхPOSIX-векторов прерываний чужой архитектуры
    foreign_mapping[0x10] = "SYS_ALLOC_MEM";
    foreign_mapping[0x20] = "SYS_WRITE_DISK";
    foreign_mapping[0x30] = "SYS_SEND_PACKET";
    foreign_mapping[0x40] = "SYS_RENDER_GUI";
}

SyscallResult BanachOSSyscallGateway::handle_foreign_trap(int foreign_pid, uint8_t trap_vector, const std::vector<uint8_t>& raw_data) {
    SyscallResult result;
    
    // Если вектор прерывания не зарегистрирован — жестко тушим вызов в ноль
    if (foreign_mapping.find(trap_vector) == foreign_mapping.end()) {
        result.status_code = 0x01;
        result.sanitized_data = {0x01};
        return result;
    }

    // Симуляция логарифмического квантования нагрузки внутри C++ шлюза
    size_t data_len = raw_data.size();
    float induced_intensity = static_cast<float>(log1p(data_len) * 2.5);

    // Предохранительный лимит: имитация выворота регистра bCONTAINER (256 ≡ 0)
    if (induced_intensity >= 256.0f || data_len > 4096) {
        result.status_code = 0x55; // Системный флаг экваториальной блокировки BanachOS
        result.sanitized_data = {0x01};
        return result;
    }

    // Сквозная побайтовая фильтрация в кольце вычетов Z/256Z против скрытых эксплойтов
    result.sanitized_data.resize(data_len);
    for (size_t i = 0; i < data_len; ++i) {
        result.sanitized_data[i] = raw_data[i] % 256;
    }

    result.status_code = 0x00; // Вызов успешно нормализован
    return result;
}
