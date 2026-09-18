#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
================================================================================
BANACHOS v8.3 — 3HCP METAMACRO SWARM & CORE VISUALIZER (bSWARM_DASHBOARD)
Author: Efim S. Markov & AI Collaborator (2026 Revision)
Formalism: Live Core & Swarm Mesh Telemetry Binding, Volumetric 3HCP Cluster Rendering
================================================================================
"""

import pygame
import numpy as np
import sys
from banachos_mem import BanachOSMemoryAllocator
from banachos_net import BanachOSNetworkStack
from banachos_mesh import BanachOSSwarmMeshProtocol  # Живой импорт вашего роевого стека

# Аппаратная инициализация графической подсистемы bGUI / bMUX
pygame.init()
WIDTH, HEIGHT = 1024, 768
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("BanachOS v8.3 — 3HCP Swarm & Core Dashboard")
clock = pygame.time.Clock()

# Развертывание живых ресурсных слоев ядра операционной системы
GRID_SIZE = 16
memory_allocator = BanachOSMemoryAllocator(cube_size=GRID_SIZE)
network_stack = BanachOSNetworkStack(routing_table_size=GRID_SIZE)
swarm_mesh = BanachOSSwarmMeshProtocol(swarm_grid_capacity=GRID_SIZE)

# Параметры 3D-ортографической проекции пространственной решетки 3HCP
SCALE = 22
ANGLE_X = 0.5
ANGLE_Y = 0.6

def project_3d(x, y, z, cx, cy, cz):
    """Перевод трехмерных координат гексагональной решетки на 2D-плоскость экрана."""
    dx = x - cx
    dy = y - cy
    dz = z - cz
    
    rx = dx * np.cos(ANGLE_X) - dy * np.sin(ANGLE_X)
    ry = dx * np.sin(ANGLE_X) + dy * np.cos(ANGLE_X)
    
    rx_final = rx
    ry_final = ry * np.cos(ANGLE_Y) - dz * np.sin(ANGLE_Y)
    
    screen_x = int(WIDTH / 2 + rx_final * SCALE)
    screen_y = int(HEIGHT / 2 + ry_final * SCALE)
    return screen_x, screen_y

# Интеграция тестовых узлов роя в мета-сеть
target_pid = 777
center_coords = (GRID_SIZE // 2, GRID_SIZE // 2, GRID_SIZE // 2) # (8, 8, 8)
memory_allocator.allocation_mask[center_coords] = True
network_stack.open_socket(socket_id=target_pid, coords=center_coords)

# Регистрируем 3 внешних роевых узла (например, дроны сопровождения) вокруг центра
swarm_mesh.integrate_swarm_node(node_uuid="DRONE_ALPHA", coords=(6, 8, 8))
swarm_mesh.integrate_swarm_node(node_uuid="DRONE_BRAVO", coords=(10, 8, 8))
swarm_mesh.integrate_swarm_node(node_uuid="PHONE_SYMBIOTE", coords=(8, 10, 8))

font = pygame.font.SysFont("Courier", 18, bold=True)

current_input_load = 10.0
inversion_active = False
inversion_timer = 0

running = True
while running:
    screen.fill((10, 15, 24))  # Глубокий вакуумный цвет ядра
    
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # Считывание кнопок bHID для нагнетания искусственной перегрузки или DDoS-атаки
    keys = pygame.key.get_pressed()
    if keys[pygame.K_UP]:
        current_input_load = min(300.0, current_input_load + 4.0)
    if keys[pygame.K_DOWN]:
        current_input_load = max(0.1, current_input_load - 4.0)

    cz, cy, cx = center_coords

    if not inversion_active:
        # Подача импульсов в локальное ОЗУ и сеть
        mem_ok, _ = memory_allocator.allocate_cell(z=cz, y=cy, x=cx, payload_bytes=current_input_load)
        net_ok, _ = network_stack.ingest_packet_stream(socket_id=target_pid, incoming_packets=current_input_load)
        
        # Одновременно пускаем вычислительную волну по каналам связи всего роя bCELL-MESH
        mesh_ok, _ = swarm_mesh.synchronize_cluster_wave(node_uuid="DRONE_ALPHA", wave_amplitude=current_input_load * 0.5)
        
        # Если Липшицев барьер пробит хотя бы на одном уровне — активируем выворот регистра
        if not mem_ok or not net_ok or not mesh_ok:
            inversion_active = True
            inversion_timer = 30
    else:
        inversion_timer -= 1
        if inversion_timer <= 0:
            inversion_active = False
            current_input_load = 10.0

    # Сбор живой телеметрии со всех трех подсистем
    mem_metrics = memory_allocator.get_bus_metrics()
    net_metrics = network_stack.get_net_telemetry()
    mesh_metrics = swarm_mesh.get_mesh_telemetry()

    # --- ОТРИСОВКА КРИСТАЛЛА РОЯ И ЛОКАЛЬНЫХ СОТ ---
    for z in range(2, GRID_SIZE - 2, 2):
        for y in range(2, GRID_SIZE - 2, 2):
            for x in range(2, GRID_SIZE - 2, 2):
                sx, sy = project_3d(x, y, z, cx, cy, cz)
                
                # Подсвечиваем координаты, где физически закреплены внешние узлы роя
                is_swarm_node = False
                for node_id, node_coords in swarm_mesh.swarm_nodes.items():
                    if (z, y, x) == node_coords:
                        is_swarm_node = True
                        break

                if (z, y, x) == center_coords:
                    color = (255, 50, 50) if inversion_active else (230, 190, 40)
                    radius = 9
                elif is_swarm_node:
                    # Внешние активные узлы сети bCELL-MESH горят неоново-зеленым
                    color = (50, 255, 255) if not inversion_active else (255, 100, 50)
                    radius = 6
                else:
                    # Градиент упругости берется из живой матрицы люфтов распределенного роя
                    local_le = swarm_mesh.leeway_matrix[z, y, x]
                    color_b = int(max(0, min(255, 220 * (local_le / 0.024))))
                    color_r = int(max(0, min(255, 50 + 205 * (1.0 - (local_le / 0.024)))))
                    color = (color_r, 90, color_b)
                    radius = 4

                pygame.draw.circle(screen, color, (sx, sy), radius)

    # --- ПАНЕЛЬ ТЕЛЕМЕТРИИ ОПЕРАТОРА МЕТА-ПЛАТФОРМЫ ---
    t_title = font.render("BANACHOS v8.3 — LIVE DISTRIBUTED SWARM INSTRUMENT PANEL", True, (140, 160, 190))
    t_load = font.render(f"SWARM WAVE AMPLITUDE   : {current_input_load:.1f} / 256.0", True, (255, 255, 255))
    t_mem_load = font.render(f"bMEM MAX CELL CAPACITY  : {mem_metrics['max_cell_load_bytes']:.2f} B", True, (50, 180, 255))
    t_net_load = font.render(f"bNET PORT CONGESTION    : {net_metrics['peak_port_congestion']:.2f} Pkt/t", True, (255, 100, 255))
    t_mesh_nodes = font.render(f"bCELL ACTIVE MESH NODES : {mesh_metrics['connected_swarm_nodes_count']} Devices", True, (50, 255, 255))
    t_leeway = font.render(f"SWARM INTER-DEVICE LE   : {mesh_metrics['average_inter_device_leeway']:.6f} / 0.024", True, (50, 255, 150))
    t_status = font.render(f"METAMACRO MATRIX SYSTEM : {'CASCADING INVERSION (256 ≡ 0)' if inversion_active else 'KEPLER_SWARM_STABILIZED'}", True, (255, 255, 50) if inversion_active else (100, 255, 100))
    
    t_hint = font.render("Controls: [ARROW_UP] - Escalate Swarm Stress | [ARROW_DOWN] - Settle Swarm", True, (110, 120, 140))

    screen.blit(t_title, (20, 20))
    pygame.draw.line(screen, (40, 60, 90), (20, 45), (650, 45), 2)
    screen.blit(t_load, (20, 60))
    screen.blit(t_mem_load, (20, 85))
    screen.blit(t_net_load, (20, 110))
    screen.blit(t_mesh_nodes, (20, 135))
    screen.blit(t_leeway, (20, 160))
    screen.blit(t_status, (20, 185))
    screen.blit(t_hint, (20, HEIGHT - 40))

    pygame.display.flip()
    clock.tick(30)

pygame.quit()
sys.exit()
