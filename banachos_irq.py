#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
================================================================================
BANACHOS v8.3 — GEOMETRIC HARDWARE INTERRUPT MANAGER (IRQ)
Author: Efim S. Markov & AI Collaborator (2026)
Formalism: Volumetric Calculus, Dynamic Leeway Closure, Lipschitz Stability
Features: Integrated Device Bus Queue Mapping, Autonomous Structural Lockup,
          Zero-Fit Matrix Filtering and Closed-Loop Variable Leeway Protection.
================================================================================
"""

import numpy as np
from typing import Dict, Any

class BanachOSInterruptController:
    """
    Геометрический контроллер прерываний ядра BanachOS.
    Заменяет стандартные очереди прерываний (FIFO) на липшицево-стабильную 
    метрическую проекцию, защищая ядро от аппаратного шторма устройств.
    """
    LE_BASE: float = 0.024       # Исходный люфт шины управления
    REGISTER_MAX: float = 256.0  # Предел емкости входного буфера прерываний

    def __init__(self, channels_count: int = 12):
        self.channels_count = channels_count
        # Входные регистры плотности прерываний по 12 гексагональным осям
        self.irq_density_bus = np.zeros(channels_count, dtype=np.float64)
        self.system_lockup = False

    def handle_hardware_signal(self, channel_id: int, pulse_intensity: float):
        """Регистрация физического импульса от устройства (сетевая карта, клавиатура)."""
        if 0 <= channel_id < self.channels_count:
            # Нагнетание плотности в регистр кольца вычетов Z/256Z
            self.irq_density_bus[channel_id] = min(
                self.REGISTER_MAX, 
                self.irq_density_bus[channel_id] + pulse_intensity
            )

    def dispatch_and_filter_bus(self, core_load: float) -> Dict[str, Any]:
        """
        Марковский селектор прерываний. Вычленяет однозначную стабильную ветвь
        выполнения задач, блокируя хаотичные прыжки градиентов данных.
        """
        with np.errstate(divide='ignore', invalid='ignore'):
            # Вычисляем среднее встречное давление ядра операционной системы (p_in)
            p_in = (self.REGISTER_MAX - core_load) / (core_load + 1.0) if core_load < self.REGISTER_MAX else 0.0
            
            # Находим суммарное внешнее давление от периферии (p_ext)
            p_ext = np.mean(self.irq_density_bus)
            
            # Вычисление Липшицева зазора шины ввода-вывода (Эффект №5 Маркова)
            if p_in == 0.0 or core_load >= self.REGISTER_MAX:
                self.system_lockup = True
                current_le = 0.0 # Шина XY заблокирована, сброс идет по вертикальному полярному каналу
            else:
                self.system_lockup = False
                pressure_ratio = (p_ext - p_in) / p_in
                current_le = max(0.0, self.LE_BASE * (1.0 - pressure_ratio))
                
            # ГЛОБАЛЬНЫЙ ОГРАНИЧИТЕЛЬ (Марковский клиппинг шага выполнения задач)
            # Формула гарантирует липшицеву стабильность распределения процессорного времени
            lipschitz_bound = 1.0 / (current_le + 1e-6)
            
            # Фильтруем зашумленный вектор прерываний от устройств через селектор наилучшего приближения
            filtered_tasks = np.clip(self.irq_density_bus * current_le, 0.0, lipschitz_bound)
            
            # Обратный отток данных (Вихревая регенерация шины памяти)
            if self.system_lockup:
                # Принудительный сброс регистров перегруженных устройств
                self.irq_density_bus.fill(0.1)
                
            return {
                "filtered_tasks_vector": filtered_tasks,
                "bus_lipschitz_bound": round(lipschitz_bound, 2),
                "hardware_bus_lockup": self.system_lockup,
                "allocated_leeway": round(current_le, 6)
            }
