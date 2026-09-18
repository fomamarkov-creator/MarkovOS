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
    TranslationCoords plane_shifts = {{1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {1,-1,0}, {-1,1,0}};
    for (int i = 0; i < 6; ++i) shifts[i] = plane_shifts[i];

    if (k % 2 == 0) {
        TranslationCoords inter_shifts = {{0,0,1}, {1,0,1}, {0,1,1}, {0,0,-1}, {1,0,-1}, {0,1,-1}};
        for (int i = 0; i < 6; ++i) shifts[i+6] = inter_shifts[i];
    } else {
        TranslationCoords inter_shifts = {{0,0,1}, {-1,0,1}, {0,-1,1}, {0,0,-1}, {-1,0,-1}, {0,-1,-1}};
        for (int i = 0; i < 6; ++i) shifts[i+6] = inter_shifts[i];
    }
}

uint8_t BanachOSCodeTranslator::map_to_opcode(const std::string& foreign_op) const {
    // Mojo-подобная нативная подмена стандартных POSIX вызовов на опкоды BanachOS
    if (foreign_op == "MALLOC" || foreign_op == "MMAP") return 0x10;      // ALLOC_MEM
    if (foreign_op == "FWRITE" || foreign_op == "WRITE") return 0x20;     // WRITE_DISK
    if (foreign_op == "SEND" || foreign_op == "SOCKET_WRITE") return 0x30; // SEND_PACKET
    if (foreign_op == "FLOATING_ADD" || foreign_op == "ADD") return 0x50;  // ADD_REG
    return 0x00; // Неизвестная инструкция принудительно глушится в NOP
}

bool BanachOSCodeTranslator::register_translation_node(const std::string& ins_id, int z, int y, int x) {
    if (z >= 0 && z < SIZE && y >= 0 && y < SIZE && x >= 0 && x < SIZE) {
        node_registry[ins_id] = {z, y, x};
        return true;
    }
    return false;
}

std::vector<NativeQuantum> BanachOSCodeTranslator::translate_foreign_block(const std::vector<ForeignInstruction>& foreign_code, std::string& out_log) {
    std::vector<NativeQuantum> native_binary;
    std::stringstream log_stream;
    
    log_stream << "[bTRANSLATOR] Анализ Mojo-слоя. Обнаружено внешних инструкций: " << foreign_code.size() << "\n";

    for (size_t i = 0; i < foreign_code.size(); ++i) {
        const auto& ins = foreign_code[i];
        std::string node_key = "ins_" + std::to_string(i) + "_" + ins.operation;
        
        // Детерминированный маппинг в 3HCP координаты транслятора
        int z = static_cast<int>(i % (SIZE - 2)) + 1;
        int y = static_cast<int>((i * 3) % (SIZE - 2)) + 1;
        int x = static_cast<int>((i * 7) % (SIZE - 2)) + 1;
        register_translation_node(node_key, z, y, x);

        // Оценка натяжения: преобразуем непрерывный аргумент в весовую сложность
        float induced_stress = static_cast<float>(std::abs(ins.raw_value) * 1.2f + 1.0f);
        float current_stress = translation_stress_matrix[z][y][x];
        float target_stress = current_stress + induced_stress;

        // Контроль интерференции по 12 смежным сотам трансляции
        TranslationCoords shifts[Z_COORDINATION];
        get_hex_shifts(z, shifts);
        float p_ext_accumulator = 0.0f;
        for (int j = 0; i < Z_COORDINATION; ++j) {
            p_ext_accumulator += translation_stress_matrix[(z + shifts[j].z + SIZE) % SIZE]
                                                          [(y + shifts[j].y + SIZE) % SIZE]
                                                          [(x + shifts[j].x + SIZE) % SIZE];
        }
        float p_ext = p_ext_accumulator / static_cast<float>(Z_COORDINATION);
        float p_in = (target_stress < TRANSLATION_LIMIT) ? ((TRANSLATION_LIMIT - target_stress) / (target_stress + 1.0f)) : 0.0f;

        float current_le = 0.0f;
        if (p_in > 0.0f && target_stress < TRANSLATION_LIMIT) {
            float pressure_ratio = (p_ext - p_in) / p_in;
            current_le = std::max(0.0f, LE_BASE * (1.0f - pressure_ratio));
        }

        // --- ВЕРИФИКАЦИЯ ПРИ ПЕРЕВОДЕ: КРАХ СТАБИЛЬНОСТИ ОПЕРАТОРА (256 ≡ 0) ---
        if (current_le == 0.0f || target_stress >= TRANSLATION_LIMIT) {
            translation_stress_matrix[z][y][x] = 0.1f;
            leeway_matrix[z][y][x] = LE_BASE;
            log_stream << "[CRITICAL] Ошибка перевода! Инструкция " << ins.operation << " содержит скрытую аномалию. Сектор заблокирован.\n";
            out_log = log_stream.str();
            return {}; // Полный отказ от сборки небезопасного бинарника
        }

        translation_stress_matrix[z][y][x] = target_stress;
        leeway_matrix[z][y][x] = current_le;

        // --- ЧЕСТНЫЙ ВЫВОР КВАНТА: Укладываем дробный float в кольцо Z/256Z ---
        NativeQuantum quantum;
        quantum.opcode = map_to_opcode(ins.operation);
        quantum.quantum_arg = static_cast<uint8_t>(std::floor(ins.raw_value)) % 256;
        
        native_binary.push_back(quantum);
    }

    log_stream << "[SUCCESS] Mojo-трансляция завершена успешно. Липшицев барьер удержан.\n";
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
