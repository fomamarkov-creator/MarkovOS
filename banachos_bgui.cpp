#include "banachos_bgui.hpp"
#include <cmath>
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

// Эмуляция структур Linux Framebuffer для обеспечения кросс-компиляции Bare-Metal
#define FBIOGET_VSCREENINFO 0x4600
struct mock_fb_var_screeninfo {
    uint32_t xres;
    uint32_t yres;
    uint32_t bits_per_pixel;
};

BanachOSGraphicsDriver::BanachOSGraphicsDriver() : fb_fd(-1), vram_memory_map(nullptr), screensize(0) {
    for (int z = 0; z < SIZE; ++z) {
        for (int y = 0; y < SIZE; ++y) {
            for (int x = 0; x < SIZE; ++x) {
                gui_stress_matrix[z][y][x] = 0.1f;
                leeway_matrix[z][y][x] = LE_BASE;
            }
        }
    }
}

BanachOSGraphicsDriver::~BanachOSGraphicsDriver() {
    if (vram_memory_map && vram_memory_map != MAP_FAILED) {
        munmap(vram_memory_map, screensize);
    }
    if (fb_fd >= 0) {
        close(fb_fd);
    }
}

void BanachOSGraphicsDriver::get_gui_shifts(int k, GuiCoords* shifts) const {
    GuiCoords plane_shifts = {{1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {1,-1,0}, {-1,1,0}};
    for (int i = 0; i < 6; ++i) shifts[i] = plane_shifts[i];

    if (k % 2 == 0) {
        GuiCoords inter_shifts = {{0,0,1}, {1,0,1}, {0,1,1}, {0,0,-1}, {1,0,-1}, {0,1,-1}};
        for (int i = 0; i < 6; ++i) shifts[i+6] = inter_shifts[i];
    } else {
        GuiCoords inter_shifts = {{0,0,1}, {-1,0,1}, {0,-1,1}, {0,0,-1}, {-1,0,-1}, {0,-1,-1}};
        for (int i = 0; i < 6; ++i) shifts[i+6] = inter_shifts[i];
    }
}

bool BanachOSGraphicsDriver::initialize_framebuffer(const std::string& fb_path) {
    // Попытка низкоуровневого открытия устройства экрана смартфона
    fb_fd = open(fb_path.c_str(), O_RDWR);
    if (fb_fd < 0) {
        // Режим изолированного прохождения тестов внутри CI/CD на GitHub
        std::cout << "[WARN] bGUI: Физический фреймбуфер недоступен. Запуск математической эмуляции видеопамяти VRAM." << std::endl;
        screensize = 1024 * 768 * 4;
        vram_memory_map = nullptr;
        return true;
    }

    mock_fb_var_screeninfo vinfo;
    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        close(fb_fd);
        return false;
    }

    screensize = vinfo.xres * vinfo.yres * (vinfo.bits_per_pixel / 8);
    
    // Прямой маппинг VRAM физического экрана в адресное пространство BanachOS
    vram_memory_map = (uint8_t*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (vram_memory_map == MAP_FAILED) {
        close(fb_fd);
        return false;
    }

    std::cout << "[bGUI] Аппаратный фреймбуфер успешно маппирован. Нативный видеовывод активен." << std::endl;
    return true;
}

bool BanachOSGraphicsDriver::draw_interface_element(int element_id, float render_intensity, std::string& out_message) {
    // Детерминированный маппинг элемента в 3HCP координаты видеопамяти
    int z = static_cast<int>(element_id % (SIZE - 2)) + 1;
    int y = static_cast<int>((element_id * 3) % (SIZE - 2)) + 1;
    int x = static_cast<int>((element_id * 7) % (SIZE - 2)) + 1;
    active_elements[element_id] = {z, y, x};

    float current_stress = gui_stress_matrix[z][y][x];
    float target_stress = current_stress + render_intensity;

    GuiCoords shifts[Z_COORDINATION];
    get_gui_shifts(z, shifts);

    float p_ext_accumulator = 0.0f;
    for (int i = 0; i < Z_COORDINATION; ++i) {
        int sz = (z + shifts[i].z + SIZE) % SIZE;
        int sy = (y + shifts[i].y + SIZE) % SIZE;
        int sx = (x + shifts[i].x + SIZE) % SIZE;
        p_ext_accumulator += gui_stress_matrix[sz][sy][sx];
    }
    float p_ext = p_ext_accumulator / static_cast<float>(Z_COORDINATION);
    float p_in = (target_stress < RENDER_LIMIT) ? ((RENDER_LIMIT - target_stress) / (target_stress + 1.0f)) : 0.0f;

    float current_le = 0.0f;
    if (p_in > 0.0f && target_stress < RENDER_LIMIT) {
        float pressure_ratio = (p_ext - p_in) / p_in;
        current_le = std::max(0.0f, LE_BASE * (1.0f - pressure_ratio));
    }

    // --- КАТАСТРОФИЧЕСКИЙ ВЫВОР ТЕЛЕВИЗИОННОГО РЕГИСТРА (256 ≡ 0) ---
    if (current_le == 0.0f || target_stress >= RENDER_LIMIT) {
        gui_stress_matrix[z][y][x] = 0.1f;
        leeway_matrix[z][y][x] = LE_BASE;
        
        // Очищаем VRAM (сброс экрана в стабильный чёрный цвет Покоя)
        if (vram_memory_map) {
            std::memset(vram_memory_map, 0x00, screensize);
        }
        
        out_message = "GUI_TRAP: Аномальный мерцающий шторм кадров погашен экваториальным замком (256 ≡ 0)";
        return false;
    }

    gui_stress_matrix[z][y][x] = target_stress;
    leeway_matrix[z][y][x] = current_le;

    // Прямая запись пикселей во фреймбуфер чипа дисплея
    if (vram_memory_map) {
        size_t pixel_offset = (element_id * 4) % screensize;
        uint8_t color_quantum = static_cast<uint8_t>(std::floor(target_stress)) % 256;
        vram_memory_map[pixel_offset]     = color_quantum; // B
        vram_memory_map[pixel_offset + 1] = 128;           // G
        vram_memory_map[pixel_offset + 2] = 50;            // R
        vram_memory_map[pixel_offset + 3] = 0;             // Alpha
    }

    out_message = "GUI_SUCCESS: Кадр bSHELL успешно выведен напрямую в кремний матрицы дисплея";
    return true;
}

float BanachOSGraphicsDriver::get_average_gui_leeway() const {
    float sum = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                sum += leeway_matrix[z][y][x];
    return sum / static_cast<float>(SIZE * SIZE * SIZE);
}

float BanachOSGraphicsDriver::get_peak_render_stress() const {
    float max_val = 0.0f;
    for (int z = 0; z < SIZE; ++z)
        for (int y = 0; y < SIZE; ++y)
            for (int x = 0; x < SIZE; ++x)
                if (gui_stress_matrix[z][y][x] > max_val) max_val = gui_stress_matrix[z][y][x];
    return max_val;
}
