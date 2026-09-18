#pragma once
#include <cstdint>
#include <string>
#include <algorithm>

class BanachOSUsbDriver {
public:
    static constexpr float LE_BASE = 0.024f;
    static constexpr float USB_LIMIT = 256.0f;

    BanachOSUsbDriver() : usb_leeway(LE_BASE) {}

    bool handle_usb_device_pulse(int port_id, float peripheral_stress, std::string& out_msg) {
        if (peripheral_stress >= USB_LIMIT || port_id < 0) {
            usb_leeway = LE_BASE;
            out_msg = "USB_HARDWARE_TRAP: Попытка атаки BadUSB или скачка питания! Порт изолирован (256 ≡ 0).";
            return false;
        }
        usb_leeway = std::max(0.0f, LE_BASE * (1.0f - (peripheral_stress / USB_LIMIT)));
        out_msg = "USB_SUCCESS: Внешнее USB-устройство успешно синхронизировано с HAL.";
        return true;
    }

    float get_usb_leeway() const { return usb_leeway; }

private:
    float usb_leeway;
};
