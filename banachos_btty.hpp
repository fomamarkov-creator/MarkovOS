#pragma once
#include <algorithm>
#include <string>

class BanachOSTtyConsole {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float TTY_LIMIT = 256.0f;
    BanachOSTtyConsole() : tty_leeway(LE_BASE) {}

    bool inject_char_stream(size_t buffer_len, std::string& out_msg) {
        float stress = static_cast<float>(buffer_len) * 2.0f;
        if (stress >= TTY_LIMIT) {
            tty_leeway = LE_BASE;
            out_msg = "TTY_RESET: Буфер ввода консоли Маркова очищен для предотвращения атаки (256 ≡ 0).";
            return false;
        }
        tty_leeway = std::max(0.0f, LE_BASE * (1.0f - (stress / TTY_LIMIT)));
        out_msg = "TTY_SUCCESS: Символьный поток bTTY успешно передан планировщику.";
        return true;
    }
    float get_leeway() const { return tty_leeway; }
private:
    float tty_leeway;
};
