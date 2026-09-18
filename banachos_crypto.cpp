#include "banachos_crypto.hpp"
#include <cmath>
#include <algorithm>

BanachOSCryptoEngine::BanachOSCryptoEngine() {
    for (int z = 0; z < SIZE; ++z) {
        for (int y = 0; y < SIZE; ++y) {
            for (int x = 0; x < SIZE; ++x) {
                crypto_matrix[z][y][x] = 0.1f;
                leeway_matrix[z][y][x] = LE_BASE;
            }
        }
    }
}

void BanachOSCryptoEngine::get_crypto_shifts(int k, CryptoCoords* shifts) const {
    CryptoCoords plane_shifts = {{1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {1,-1,0}, {-1,1,0}};
    for (int i = 0; i < 6; ++i) shifts[i] = plane_shifts[i];

    if (k % 2 == 0) {
        CryptoCoords inter_shifts = {{0,0,1}, {1,0,1}, {0,1,1}, {0,0,-1}, {1,0,-1}, {0,1,-1}};
        for (int i = 0; i < 6; ++i) shifts[i+6] = inter_shifts[i];
    } else {
        CryptoCoords inter_shifts = {{0,0,1}, {-1,0,1}, {0,-1,1}, {0,0,-1}, {-1,0,-1}, {0,-1,-1}};
        for (int i = 0; i < 6; ++i) shifts[i+6] = inter_shifts[i];
    }
}

bool BanachOSCryptoEngine::register_alternand_key(int key_id, int z, int y, int x) {
    if (z >= 0 && z < SIZE && y >= 0 && y < SIZE && x >= 0 && x < SIZE) {
        key_registry[key_id] = {z, y, x};
        return true;
    }
    return false;
}

bool BanachOSCryptoEngine::encrypt_system_data(int key_id, float raw_data_byte, std::string& out_message) {
    auto it = key_registry.find(key_id);
    if (it == key_registry.end()) {
        out_message = "CRYPTO_ERROR: Заданный крипто-ключ отсутствует в реестре альтернанса";
        return false;
    }

    int z = it->second.z;
    int y = it->second.y;
    int x = it->second.x;

    float current_state = crypto_matrix[z][y][x];
    
    // Лавинное геометрическое преобразование входного байта с винтовым шагом ZETA
    float transformed_byte = std::fmod(raw_data_byte * ZETA, REGISTER_MAX);
    float target_state = current_state + transformed_byte;

    CryptoCoords shifts[Z_COORDINATION];
    get_crypto_shifts(z, shifts);

    float p_ext_accumulator = 0.0f;
    for (int i = 0; i < Z_COORDINATION; ++i) {
        int sz = (z + shifts[i].z + SIZE) % SIZE;
        int sy = (y + shifts[i].y + SIZE) % SIZE;
        int sx = (x + shifts[i].x + SIZE) % SIZE;
        p_ext_accumulator += crypto_matrix[sz][sy][sx];
    }
    float p_ext = p_ext_accumulator / static_cast<float>(Z_COORDINATION);

    float p_in = (target_state < REGISTER_MAX) ? ((REGISTER_MAX - target_state) / (target_state + 1.0f)) : 0.0f;

    float current_le = 0.0f;
    if (p_in > 0.0f && target_state < REGISTER_MAX) {
        float pressure_ratio = (p_ext - p_in) / p_in;
        current_le = std::max(0.0f, LE_BASE * (1.0f - pressure_ratio));
    }

    // --- АППАРАТНАЯ БЛОКИРОВКА СЕССИИ ПРИ ПОПЫТКЕ ВЗЛОМА (256 ≡ 0) ---
    if (current_le == 0.0f || target_state >= REGISTER_MAX) {
        crypto_matrix[z][y][x] = 0.1f;
        leeway_matrix[z][y][x] = LE_BASE;
        out_message = "CRYPTO_TAMPER_DETECTED: Аномалия диффузии! Ячейка стерта по формуле 256 ≡ 0";
        return false;
    }

    crypto_matrix[z][y][x] = target_state;
    leeway_matrix[z][y][x] = current_le;
    out_message = "CRYPTO_SUCCESS: Байт данных успешно зашифрован в 3HCP-контексте";
    return true;
}

float BanachOSCryptoEngine::get_average_diffusion_leeway() const {
    float sum = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                sum += leeway_matrix[z][y][x];
    return sum / static_cast<float>(SIZE * SIZE * SIZE);
}

float BanachOSCryptoEngine::get_peak_entropy_density() const {
    float max_val = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                if (crypto_matrix[z][y][x] > max_val) max_val = crypto_matrix[z][y][x];
    return max_val;
}

int BanachOSCryptoEngine::get_active_secure_keys() const {
    return static_cast<int>(key_registry.size());
}
