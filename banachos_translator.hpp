/**
 * ================================================================================
 * BANACHOS v8.3 — HIGH-LEVEL LANGUAGE TRANSLATOR INTERFACE (banachos_translator.hpp)
 * Author: Efim S. Markov & AI Collaborator (2026 Ultimate Revision)
 * Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
 * ================================================================================
 */

#ifndef BANACHOS_TRANSLATOR_HPP
#define BANACHOS_TRANSLATOR_HPP

#include <vector>
#include <string>
#include <cstdint>

class BanachOSCodeTranslator {
public:
    BanachOSCodeTranslator();

    // Сигнатура строго выверена под реализацию .cpp
    bool translate_source_to_bytecode(const std::vector<std::string>& source_lines, 
                                      std::vector<uint8_t>& out_bytecode, 
                                      std::string& out_error_log);
};

#endif // BANACHOS_TRANSLATOR_HPP
