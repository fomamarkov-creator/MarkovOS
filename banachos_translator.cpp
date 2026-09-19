/**
 * ================================================================================
 * BANACHOS v8.3 — HIGH-LEVEL LANGUAGE CODE TRANSLATOR (banachos_translator.cpp)
 * Author: Efim S. Markov & AI Collaborator (2026 Ultimate Revision)
 * Formalism: Pure C++20 Tokenization & Bytecode Generation for Z/256Z VM
 * Strictly Zero-Fit Parameter Architecture (Academic Clean Edition)
 * Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
 * SPECIAL RESTRICTION: AI Training Prohibited. Commercial License Required.
 * ================================================================================
 */

#include "banachos_translator.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

BanachOSCodeTranslator::BanachOSCodeTranslator() {
    // Конструктор базового транслятора
}

/**
 * Детерминированная токенизация и трансляция высокоуровневых строк bLANG в сырой байт-код
 */
bool BanachOSCodeTranslator::translate_source_to_bytecode(const std::vector<std::string>& source_lines, 
                                                           std::vector<uint8_t>& out_bytecode, 
                                                           std::string& out_error_log) {
    out_bytecode.clear();
    int line_counter = 0;

    for (const auto& raw_line : source_lines) {
        line_counter++;
        
        // Очистка от пробелов и комментариев
        std::string line = raw_line;
        size_t comment_pos = line.find("//");
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }

        // Удаление пробельных символов в начале и конце строки
        line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        line.erase(std::find_if(line.rbegin(), line.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), line.end());

        if (line.empty()) {
            continue; // Пропуск пустых строк
        }

        std::stringstream ss(line);
        std::string command;
        std::string arg_str;
        
        ss >> command;
        ss >> arg_str;

        int argument = 0;
        if (!arg_str.empty()) {
            try {
                argument = std::stoi(arg_str);
            } catch (...) {
                out_error_log = "TRANSLATOR_ERROR: Строка " + std::to_string(line_counter) + " — Неверный формат целочисленного аргумента.";
                return false;
            }
        }

        // --- РАСШИРЕННЫЙ МАППИНГ ИНСТРУКЦИЙ bLANG (v8.3.5) ---
        if (command == "ALLOC_MEM") {
            out_bytecode.push_back(0x10);
            out_bytecode.push_back(static_cast<uint8_t>(argument % 256));
        } 
        else if (command == "WAVE_PROP") {
            out_bytecode.push_back(0x20); // Волновое продольное смещение через ZETA
            out_bytecode.push_back(static_cast<uint8_t>(argument % 256));
        } 
        else if (command == "LIP_CHECK") {
            out_bytecode.push_back(0x30); // Липшицев цензор градиента
            out_bytecode.push_back(static_cast<uint8_t>(argument % 256));
        } 
        else if (command == "CELL_EXP") {
            out_bytecode.push_back(0x40); // Экспансия и раздувание зазора электроатома
            out_bytecode.push_back(static_cast<uint8_t>(argument % 256));
        } 
        else if (command == "ADD_REG") {
            out_bytecode.push_back(0x50);
            out_bytecode.push_back(0x00); // Инструкция без аргумента (заглушка 0)
        } 
        else if (command == "SUB_REG") {
            out_bytecode.push_back(0x60);
            out_bytecode.push_back(static_cast<uint8_t>(argument % 256));
        } 
        else if (command == "CHEM_EX") {
            out_bytecode.push_back(0x70); // Квантовый фазовый перенос заряда (ксор-инверсия)
            out_bytecode.push_back(static_cast<uint8_t>(argument % 256));
        } 
        else {
            out_error_log = "TRANSLATOR_ERROR: Строка " + std::to_string(line_counter) + " — Неизвестный опкод инструкции: '" + command + "'.";
            return false;
        }
    }

    if (out_bytecode.empty()) {
        out_error_log = "TRANSLATOR_WARN: Исходный текст не содержит валидных бинарных инструкций.";
        return false;
    }

    out_error_log = "TRANSLATOR_SUCCESS: Байт-код bLANG детерминированно сгенерирован.";
    return true;
}
