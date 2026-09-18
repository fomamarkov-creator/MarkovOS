#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
================================================================================
BANACHOS v8.3 - 3HCP LATTICE REAL-TIME 3D VISUALIZER
Visualizes the geometric lockup and Polar Jet emission.
================================================================================
"""

import pygame
import numpy as np
import sys
from umm_core import UMMv83UltimateEngine

# Инициализация графического движка
pygame.init()
WIDTH, HEIGHT = 1024, 768
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("BanachOS v8.3 — 3HCP Lattice Grid Visualizer")
clock = pygame.time.Clock()

# Запуск физического резонатора Маркова
GRID_SIZE = 16
engine = UMMv83UltimateEngine(grid_size=GRID_SIZE)

# Параметры 3D-ортографической проекции
SCALE = 22
ANGLE_X = 0.5
ANGLE_Y = 0.6

def project_3d(x, y, z, cx, cy, cz):
    """Перевод трехмерных координат гексагональной решетки на 2D-экран."""
    dx = x - cx
    dy = y - cy
    dz = z - cz
    
    # Матрица поворота вокруг осей
    rx = dx * np.cos(ANGLE_X) - dy * np.sin(ANGLE_X)
    ry = dx * np.sin(ANGLE_X) + dy * np.cos(ANGLE_X)
    
    rx_final = rx
    ry_final = ry * np.cos(ANGLE_Y) - dz * np.sin(ANGLE_Y)
    
    screen_x = int(WIDTH / 2 + rx_final * SCALE)
    screen_y = int(HEIGHT / 2 + ry_final * SCALE)
    return screen_x, screen_y

# Интерактивные параметры
current_core_rho = 100.0
font = pygame.font.SysFont("Courier", 18, bold=True)

running = True
while running:
    screen.fill((10, 15, 24)) # Глубокий космический цвет ядра
    
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            
    # Считываем зажатые клавиши для симуляции системной нагрузки
    keys = pygame.key.get_pressed()
    if keys[pygame.K_UP]:
        current_core_rho = min(256.0, current_core_rho + 2.5) # Нагнетаем давление
    if keys[pygame.K_DOWN]:
        current_core_rho = max(1.0, current_core_rho - 2.5)  # Сбрасываем нагрузку
        
    # Считаем текущий квант физического состояния решетки
    state = engine.calculate_state(m_baryon=4e10, core_rho=current_core_rho, steps=32)
    le_array = state["current_le_array"] # Динамический зазор
    
    # Отрисовка узлов гексагональной решетки
    # Для скорости выводим центральный срез
    cz, cy, cx = GRID_SIZE // 2, GRID_SIZE // 2, GRID_SIZE // 2
    for z in range(1, GRID_SIZE - 1, 2):
        for y in range(1, GRID_SIZE - 1, 2):
            for x in range(1, GRID_SIZE - 1, 2):
                sx, sy = project_3d(x, y, z, cx, cy, cz)
                
                # Рассчитываем цвет узла в зависимости от его деформации
                if x == cx and y == cy:
                    # Центральный силовой полярный канал
                    color = (255, 60, 60) if state["inversion_triggered"] else (230, 200, 50)
                    radius = 6
                else:
                    # Вакуумная подложка
                    color = (50, 120, 220)
                    radius = 3
                    
                pygame.draw.circle(screen, color, (sx, sy), radius)
                
    # Вывод телеметрии ядра на экран
    t_rho = font.render(f"SYSTEM IRQ PRESSURE (rho_c): {current_core_rho:.1f} / 256.0", True, (255, 255, 255))
    t_shumann = font.render(f"SHUMANN CLOCK FREQUENCY  : {state['shumann_hz']:.1f} Hz", True, (50, 255, 150))
    t_jet = font.render(f"POLAR JET IMPULSE (Vz)   : {state['polar_jet_velocity']:.2f} km/s", True, (255, 100, 100))
    t_lock = font.render(f"EQUATORIAL BUS LOCKUP    : {'LOCKED (256 ≡ 0)' if state['inversion_triggered'] else 'STABLE'}", True, (255, 255, 50))
    t_hint = font.render("Управление: [СТРЕЛКА ВВЕРХ] - Нагнетание прерываний, [СТРЕЛКА ВНИЗ] - Сброс", True, (150, 160, 180))
    
    screen.blit(t_rho, (20, 20))
    screen.blit(t_shumann, (20, 45))
    screen.blit(t_jet, (20, 70))
    screen.blit(t_lock, (20, 95))
    screen.blit(t_hint, (20, HEIGHT - 40))
    
    pygame.display.flip()
    clock.tick(30)

pygame.quit()
sys.exit()
