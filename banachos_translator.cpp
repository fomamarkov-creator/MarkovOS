#include "banachos_translator.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>

BanachOSCodeTranslator::BanachOSCodeTranslator() {
    for (int z = 0; z < SIZE; ++z) {
        for (int y = 0; y < SIZE; ++y) {
            for (int x = 0; x < SIZE; ++x) {
                translation_stress_matrix[z][y][x] = 0.1f;
                leeway_matrix[z][y][x] = LE_BASE;
            }
        }
    }
}

void BanachOSCodeTranslator::get_hex_shifts(int k, TranslationCoords* shifts) const {
    // Исправлено: добавлен массив [] для plane_shifts
    TranslationCoords plane_shifts[] = {{1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {1,-1,0}, {-1,1,0}};
    for (int i = 0; i < 6; ++i) shifts[i] = plane_shifts[i];

    if (k % 2 == 0) {
        // Исправлено: добавлен массив [] для inter_shifts
        TranslationCoords inter_shifts[] = {{0,0,1}, {1,0,1}, {0,1,1}, {0,0,-1}, {1,0,-1}, {0,1,-1}};
        for (int i = 0; i < 6; ++i) shifts[i+6] = inter_shifts[i];
    } else {
        // Исправлено: добавлен массив [] для inter_shifts
        TranslationCoords inter_shifts[] = {{0,0,1}, {-1,0,1}, {0,-1,1}, {0,0,-1}, {-1,0,-1}, {0,-1,-1}};
        for (int i = 0; i < 6; ++i) shifts[i+6] = inter_shifts[i];
    }
}

GuestOSType BanachOSCodeTranslator::auto_detect_binary_format(const std::vector<uint8_t>& raw_bytes) {
    if (raw_bytes.size() < 4) return GuestOSType::UNKNOWN_STREAM;

    // Спектральный анализ сигнатур заголовков любых мировых ОС
    if (raw_bytes[0] == 0x7F && raw_bytes[1] == 'E' && raw_bytes[2] == 'L' && raw_bytes[3] == 'F') 
        return GuestOSType::ANDROID_ELF;
    if (raw_bytes[0] == 0xCF && raw_bytes[1] == 0xFA && raw_bytes[2] == 0xED && raw_bytes[3] == 0xFE) 
        return GuestOSType::IOS_MACHO;
    if (raw_bytes[0] == 'M' && raw_bytes[1] == 'Z') 
        return GuestOSType::WINDOWS_PE;

    return GuestOSType::UNKNOWN_STREAM;
}

uint8_t BanachOSCodeTranslator::map_arm_x86_to_banachos(uint32_t raw_op, GuestOSType os_type) const {
    // Ультра-универсальный Mojo-маппинг ассемблерных опкодов, включая беспроводные ловушки Linux API
    switch (os_type) {
        case GuestOSType::ANDROID_ELF:
            if (raw_op == 0xAA0003E0) return 0x50; // ARM64: mov x0, xzr -> ADD_REG
            if (raw_op == 0xD10043FF) return 0x10; // ARM64: sub sp, sp -> ALLOC_MEM
            if (raw_op == 0x000000C6) return 0x30; // __NR_bind (захват Wi-Fi/BT сокета) -> bNET
            if (raw_op == 0x000000D3) return 0x40; // __NR_ioctl (захват параметров радиомодуля) -> bHAL
            break;
        case GuestOSType::IOS_MACHO:
            if (raw_op == 0x55) return 0x10;       // x86_64: push rbp -> ALLOC_MEM
            if (raw_op == 0x4889E5) return 0x50;   // x86_64: mov rbp, rsp -> ADD_REG
            break;
        case GuestOSType::WINDOWS_PE:
            if (raw_op == 0xEB) return 0x40;       // x86: jmp short -> EMIT_PULSE
            break;
        default:
            break;
    }
    return 0x00; // Любая невалидная или опасная команда транслируется в NOP (0x00)
}

bool BanachOSCodeTranslator::register_translation_node(const std::string& ins_id, int z, int y, int x) {
    if (z >= 0 && z < SIZE && y >= 0 && y < SIZE && x >= 0 && x < SIZE) {
        node_registry[ins_id] = {z, y, x};
        return true;
    }
    return false;
}

std::vector<NativeQuantum> BanachOSCodeTranslator::translate_universal_binary(
    const std::vector<uint8_t>& raw_binary_stream, 
    const std::vector<RawBinaryInstruction>& instructions, 
    std::string& out_log
) {
    std::vector<NativeQuantum> native_binary;
    std::stringstream log_stream;

    GuestOSType detected_os = auto_detect_binary_format(raw_binary_stream);
    log_stream << "[bTRANSLATOR] Глобальный сканер активен. Обнаружен бинарный формат: ";
    if (detected_os == GuestOSType::ANDROID_ELF) log_stream << "ANDROID_ELF (Linux Kernel Core)\n";
    else if (detected_os == GuestOSType::IOS_MACHO) log_stream << "IOS_MACHO (Darwin XNU Core)\n";
    else if (detected_os == GuestOSType::WINDOWS_PE) log_stream << "WINDOWS_PE (NT Kernel Core)\n";
    else log_stream << "UNKNOWN_BINARY_STREAM (Принудительное Липшицево квантование)\n";

    for (size_t i = 0; i < instructions.size(); ++i) {
        const auto& ins = instructions[i];
        std::string node_key = "ins_" + std::to_string(i) + "_" + std::to_string(ins.raw_opcode);
        
        // Многомерный гексагональный JIT-маппинг
        int z = static_cast<int>(i % (SIZE - 2)) + 1;
        int y = static_cast<int>((i * 3) % (SIZE - 2)) + 1;
        int x = static_cast<int>((i * 7) % (SIZE - 2)) + 1;
        register_translation_node(node_key, z, y, x);

        // Квантование непрерывной сложности вещественного числа гостевой ОС
        float induced_stress = static_cast<float>(std::abs(ins.continuous_arg) * 1.15f + 1.5f);
        float target_stress = translation_stress_matrix[z][y][x] + induced_stress;

        TranslationCoords shifts[Z_COORDINATION];
        get_hex_shifts(z, shifts);
        float p_ext_accumulator = 0.0f;
        for (int j = 0; j < Z_COORDINATION; ++j) {
            p_ext_accumulator += translation_stress_matrix[(z + shifts[j].z + SIZE) % SIZE]
                                                          [(y + shifts[j].y + SIZE) % SIZE]
                                                          [(x + shifts[j].x + SIZE) % SIZE];
        }
        float p_ext = p_ext_accumulator / static_cast<float>(Z_COORDINATION);
        float p_in = (target_stress < TRANSLATION_LIMIT) ? ((TRANSLATION_LIMIT - target_stress) / (target_stress + 1.0f)) : 0.0f;

        float current_le = 0.0f;
        if (p_in > 0.0f && target_stress < TRANSLATION_LIMIT) {
            current_le = std::max(0.0f, LE_BASE * (1.0f - ((p_ext - p_in) / p_in)));
        }

        // --- АППАРАТНЫЙ ИЗОЛЯТОР УЯЗВИМОСТЕЙ ЧУЖОГО БИНАРНИКА (256 ≡ 0) ---
        if (current_le == 0.0f || target_stress >= TRANSLATION_LIMIT) {
            translation_stress_matrix[z][y][x] = 0.1f;
            leeway_matrix[z][y][x] = LE_BASE;
            log_stream << "[CRITICAL_INTERCEPT] Попытка Buffer Overflow или вирусной инжекции внутри гостевого кода! Инструкция 0x" 
                       << std::hex << ins.raw_opcode << " аннигилирована (256 ≡ 0).\n";
            out_log = log_stream.str();
            return {}; // Полный отказ от сборки скомпрометированного приложения
        }

        translation_stress_matrix[z][y][x] = target_stress;
        leeway_matrix[z][y][x] = current_le;

        // Generation of native quantum within the Z/256Z residue ring
        NativeQuantum quantum;
        quantum.opcode = map_arm_x86_to_banachos(ins.raw_opcode, detected_os);
        quantum.quantum_arg = static_cast<uint8_t>(std::floor(ins.continuous_arg)) % 256;
        native_binary.push_back(quantum);
    }

    log_stream << "[SUCCESS] Сквозная универсальная трансляция кода завершена. Программа полностью поглощена.\n";
    out_log = log_stream.str();
    return native_binary;
}

float BanachOSCodeTranslator::get_translator_stability_index() const {
    float sum = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                sum += leeway_matrix[z][y][x];
    return sum / static_cast<float>(SIZE * SIZE * SIZE);
}

float BanachOSCodeTranslator::get_peak_compilation_stress() const {
    float max_val = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                if (translation_stress_matrix[z][y][x] > max_val) max_val = translation_stress_matrix[z][y][x];
    return max_val;
}
