/**
 * ================================================================================
 * BANACHOS v8.3 — NATIVE HIGH-LEVEL LANGUAGE COMPILER & EMITTER (banachos_hll_compiler.hpp)
 * Author: Efim S. Markov & AI Collaborator (2026 Revision)
 * Formalism: bLang Lexical Parsing, Z/256Z Machine Code Generation & AST Verification
 * Strictly Zero-Fit Parameter Architecture (Academic Clean Edition)
 * Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
 * ================================================================================
 */

#ifndef BANACHOS_HLL_COMPILER_HPP
#define BANACHOS_HLL_COMPILER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "banachos_compiler.hpp"

class BanachOSHllCompiler {
public:
    BanachOSHllCompiler(BanachOSCompiler& static_verifier) : verifier(static_verifier) {
        // СИНХРОНИЗИРОВАННАЯ КАРТА ОПКОДОВ НАШЕЙ РАСШИРЕННОЙ АРХИТЕКТУРЫ ЯДРА
        opcodes["ALLOC_MEM"] = 0x10;  // Выделение гексагональной соты ОЗУ bMEM
        opcodes["WAVE_PROP"] = 0x20;  // Волновое продольное смещение через ZETA
        opcodes["LIP_CHECK"] = 0x30;  // Липшицев цензор градиента
        opcodes["CELL_EXP"]  = 0x40;  // Экспансия и раздувание объема электроатома
        opcodes["ADD_REG"]   = 0x50;  // Нативное сложение регистров (mod 256)
        opcodes["SUB_REG"]   = 0x60;  // Нативное вычитание регистров (mod 256)
        opcodes["CHEM_EX"]   = 0x70;  // Квантовый фазовый перенос заряда по Маркову
    }

    /**
     * Компилирует текстовые строки bLang в исполняемый бинарный образ (std::vector<uint8_t>).
     */
    std::tuple<bool, std::vector<uint8_t>, std::string> compile_source_code(const std::vector<std::string>& source_lines) {
        compiled_binary.clear();

        std::cout << "[bCOMPILER] Накапливается исходный код программы... Строк обнаружено: " << source_lines.size() << std::endl;

        for (size_t line_idx = 0; line_idx < source_lines.size(); ++line_idx) {
            std::string line = source_lines[line_idx];
            
            // Исправлено: парсинг С++ комментариев // вместо легаси #
            size_t comment_pos = line.find("//");
            if (comment_pos != std::string::npos) {
                line = line.substr(0, comment_pos);
            }

            std::stringstream ss(line);
            std::string command;
            ss >> command;

            if (command.empty()) {
                continue;
            }

            if (opcodes.find(command) == opcodes.end()) {
                return {false, {}, "COMPILE_FAIL: Синтаксическая ошибка на строке " + std::to_string(line_idx + 1) + ": Неизвестный оператор '" + command + "'"};
            }

            float argument_val = 0.0f;
            if (ss >> argument_val) {
                if (ss.fail()) {
                    return {false, {}, "COMPILE_FAIL: Ошибка парсинга аргумента на строке " + std::to_string(line_idx + 1)};
                }
            }

            std::string token_name = "line_" + std::to_string(line_idx) + "_" + command;
            
            size_t token_hash = std::hash<std::string>{}(token_name);
            int z = static_cast<int>(token_hash % BanachOSCompiler::SIZE);
            int y = static_cast<int>((line_idx * 3) % BanachOSCompiler::SIZE);
            int x = static_cast<int>((line_idx * 7) % BanachOSCompiler::SIZE);
            
            verifier.register_code_token(token_name, z, y, x);
            
            float logical_complexity = static_cast<float>(std::abs(argument_val) * 1.8f + 2.0f);
            auto [verified, msg] = verifier.compile_operator(token_name, logical_complexity);
            
            if (!verified) {
                return {false, {}, "COMPILE_FAIL: Сборка заблокирована цензором! " + msg};
            }

            uint8_t opcode = opcodes[command];
            int floored_arg = static_cast<int>(std::floor(argument_val));
            uint8_t byte_arg = static_cast<uint8_t>((floored_arg % 256 + 256) % 256);
            
            compiled_binary.push_back(opcode);
            compiled_binary.push_back(byte_arg);
        }

        return {true, compiled_binary, "COMPILE_SUCCESS: Бинарный образ успешно собран и верифицирован по Липшицу"};
    }

    std::vector<std::string> get_binary_disassembly(const std::vector<uint8_t>& binary_image) {
        std::vector<std::string> disassembly;
        std::map<uint8_t, std::string> opcode_reverse;
        for (const auto& [k, v] : opcodes) {
            opcode_reverse[v] = k;
        }
        
        for (size_t i = 0; i < binary_image.size(); i += 2) {
            uint8_t opcode = binary_image[i];
            uint8_t arg = (i + 1 < binary_image.size()) ? binary_image[i + 1] : static_cast<uint8_t>(0);
            
            std::string cmd_name = (opcode_reverse.find(opcode) != opcode_reverse.end()) ? 
                                   opcode_reverse[opcode] : "UNKNOWN_OP_0x";
            
            std::stringstream line_stream;
            if (cmd_name == "UNKNOWN_OP_0x") {
                line_stream << "0x" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << i 
                            << ": " << cmd_name << std::setw(2) << static_cast<int>(opcode) 
                            << " (mod 256 argument: " << std::dec << static_cast<int>(arg) << ")";
            } else {
                line_stream << "0x" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << i 
                            << ": " << cmd_name << " (mod 256 argument: " << std::dec << static_cast<int>(arg) << ")";
            }
            disassembly.push_back(line_stream.str());
        }
        return disassembly;
    }

private:
    BanachOSCompiler& verifier;
    std::map<std::string, uint8_t> opcodes;
    std::vector<uint8_t> compiled_binary;
};

#endif // BANACHOS_HLL_COMPILER_HPP
