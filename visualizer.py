#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
================================================================================
BANACHOS v8.3 — 3HCP LATTICE CORE REAL-TIME 3D VISUALIZER (bDASHBOARD)
Author: Efim S. Markov & AI Collaborator (2026 Revision)
Formalism: Live Core Telemetry Binding, Volumetric 3HCP Grid Rendering, 256 ≡ 0 Tracer
================================================================================
"""

import pygame
import numpy as np
import sys
from banachos_mem import BanachOSMemoryAllocator
from banachos_net import BanachOSNetworkStack

# Аппаратная инициализация графической подсистемы bGUI / bMUX
pygame.init()
WIDTH, HEIGHT = 1024, 768
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("BanachOS v8.3 — 3HCP Core Architecture Visualizer")
clock = pygame.time.Clock()

# Развертывание живых ресурсных слоев ядра операционной системы
GRID_SIZE = 16
memory_allocator = BanachOSMemoryAllocator(cube_size=GRID_SIZE)
network_stack = BanachOSNetworkStack(routing_table_size=GRID_SIZE)

# Параметры 3D-ортографической проекции пространственной решетки 3HCP
SCALE = 22
ANGLE_X = 0.5
ANGLE_Y = 0.6

def project_3d(x, y, z, cx, cy, cz):
    """Перевод трехмерных координат гексагональной решетки на 2D-плоскость экрана."""
    dx = x - cx
    dy = y - cy
    dz = z - cz
    
    # Вращение фазового пространства
    rx = dx * np.cos(ANGLE_X) - dy * np.sin(ANGLE_X)
    ry = dx * np.sin(ANGLE_X) + dy * np.cos(ANGLE_X)
    
    rx_final = rx
    ry_final = ry * np.cos(ANGLE_Y) - dz * np.sin(ANGLE_Y)
    
    screen_x = int(WIDTH / 2 + rx_final * SCALE)
    screen_y = int(HEIGHT / 2 + ry_final * SCALE)
    return screen_x, screen_y

# Регистрация демонстрационных системных узлов в физической матрице
target_pid = 777
target_coords = (GRID_SIZE // 2, GRID_SIZE // 2, GRID_SIZE // 2) # Центр кристалла (8, 8, 8)
memory_allocator.allocation_mask[target_coords] = True  # Резервируем центральный сектор ОЗУ
network_stack.open_socket(socket_id=target_pid, coords=target_coords)

# Настройка шрифтов Марковской консоли bTTY
font = pygame.font.SysFont("Courier", 18, bold=True)

# Базовые параметры эмуляции нагрузки
current_input_load = 10.0
inversion_active = False
inversion_timer = 0

running = True
while running:
    screen.fill((10, 15, 24))  # Глубокий вакуумный цвет подложки ядра (bDEBUG-комфорт)
    
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # Считывание кнопок ввода bHID для нагнетания искусственной перегрузки/атаки
    keys = pygame.key.get_pressed()
    if keys[pygame.K_UP]:
        current_input_load = min(300.0, current_input_load + 4.0)  # Нагнетаем волновое давление
    if keys[pygame.K_DOWN]:
        current_input_load = max(0.1, current_input_load - 4.0)   # Сбрасываем нагрузку в Покой

    cz, cy, cx = target_coords

    if not inversion_active:
        # Штатная подача импульсов нагрузки в живые модули bMEM и bNET
        mem_ok, mem_msg = memory_allocator.allocate_cell(z=cz, y=cy, x=cx, payload_bytes=current_input_load)
        net_ok, net_msg = network_stack.ingest_packet_stream(socket_id=target_pid, incoming_packets=current_input_load)
        
        # Если Липшицев зазор схлопнулся — фиксируем триггер выворота регистра 256 ≡ 0
        if not mem_ok or not net_ok:
            inversion_active = True
            inversion_timer = 30  # Замораживаем вспышку выворота на 30 кадров (1 секунда)
    else:
        inversion_timer -= 1
        if inversion_timer <= 0:
            inversion_active = False
            current_input_load = 10.0  # Автоматический детерминированный возврат в Покой

    # Извлекаем живую телеметрию из тензоров модулей ядра
    mem_metrics = memory_allocator.get_bus_metrics()
    net_metrics = network_stack.get_net_telemetry()

    # --- ОТРИСОВКА УЗЛОВ ГЕКСАГОНАЛЬНОЙ РЕШЕТКИ 3HCP ЧЕРЕЗ ЖИВУЮ ОПТИКУ ---
    # Отрисовываем объемные слои для демонстрации Keplerean-упаковки
    for z in range(2, GRID_SIZE - 2, 2):
        for y in range(2, GRID_SIZE - 2, 2):
            for x in range(2, GRID_SIZE - 2, 2):
                sx, sy = project_3d(x, y, z, cx, cy, cz)
                
                if (z, y, x) == target_coords:
                    # Центральный силовой узел (Место атаки / Выворота)
                    color = (255, 50, 50) if inversion_active else (230, 190, 40)
                    radius = 8
                else:
                    # Окружающая стабильная подложка ОЗУ. Цветовой градиент зависит от люфта соты
                    local_le = memory_allocator.leeway_tensor[z, y, x]
                    # Чем меньше люфт (выше нагрузка), тем сильнее сота уходит в оранжевый спектр
                    color_b = int(max(0, min(255, 220 * (local_le / 0.024))))
                    color_r = int(max(0, min(255, 50 + 205 * (1.0 - (local_le / 0.024)))))
                    color = (color_r, 120, color_b)
                    radius = 4

                pygame.draw.circle(screen, color, (sx, sy), radius)

    # --- ВЫВОД ЖИВОЙ ТЕЛЕМЕТРИИ С ШИН bMEM / bNET НА ПАНЕЛЬ ОПЕРАТОРА ---
    t_title = font.render("BANACHOS v8.3 — LIVE CORE HARDWARE INSTRUMENT PANEL", True, (140, 160, 190))
    t_load = font.render(f"INJECTED SIGNAL STRESS   : {current_input_load:.1f} / 256.0", True, (255, 255, 255))
    t_mem_load = font.render(f"bMEM MAX CELL SATURATION  : {mem_metrics['max_cell_load_bytes']:.4f} Bytes", True, (50, 180, 255))
    t_net_load = font.render(f"bNET PEAK PORT CONGESTION : {net_metrics['peak_port_congestion']:.4f} Pkt/t", True, (255, 100, 255))
    t_leeway = font.render(f"MEAN SYSTEM LEEWAY (LE)  : {mem_metrics['mean_address_leeway']:.6f} / 0.024", True, (50, 255, 150))
    t_status = font.render(f"EQUATORIAL REGISTER LOCK : {'INVERTED (256 ≡ 0) - RESET ACTIVE' if inversion_active else 'STABLE (LIPSCHITZ_BOUNDED)'}", True, (255, 255, 50) if inversion_active else (100, 255, 100))
    
    t_hint = font.render("bHID Control: [ARROW_UP] - Inject Data Storm | [ARROW_DOWN] - Settle Load", True, (110, 120, 140))

    screen.blit(t_title, (20, 20))
    pygame.draw.line(screen, (40, 60, 90), (20, 45), (600, 45), 2)
    screen.blit(t_load, (20, 60))
    screen.blit(t_mem_load, (20, 85))
    screen.blit(t_net_load, (20, 110))
    screen.blit(t_leeway, (20, 135))
    screen.blit(t_status, (20, 160))
    screen.blit(t_hint, (20, HEIGHT - 40))

    pygame.display.flip()
    clock.tick(30)

pygame.quit()
sys.exit()
