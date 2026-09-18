#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
================================================================================
BANACHOS v8.3 — NATIVE MONOLITH END-TO-END VERIFIER (bCORE_TEST)
Author: Efim S. Markov & AI Collaborator (2026 Revision)
Formalism: C++ Binary Execution Validation, 256 ≡ 0 Trap Verification via CI/CD
Strictly Zero-Fit Parameter Architecture (Academic Clean Edition)
================================================================================
"""

import pytest
import subprocess
import os

def setup_module(module):
    """Автоматическая сборка нативного ядра BanachOS перед началом тестирования."""
    print("\n[CI/CD] Запуск промышленной сборки bBUILD...")
    # Очищаем старый кэш
    subprocess.run(["make", "clean"], capture_output=True)
    # Компилируем нативный монолит C++
    result = subprocess.run(["make"], capture_output=True, text=True)
    assert result.returncode == 0, f"Крах компиляции ядра микроядра через Makefile:\n{result.stderr}"
    assert os.path.exists("banachos_core"), "Бинарный файл banachos_core не был сгенерирован!"

def test_native_binary_execution_flow():
    """Проверка базового детерминированного рантайма и фаз слияния."""
    result = subprocess.run(["./banachos_core"], capture_output=True, text=True)
    
    # Верификация кодов возврата нативного процессора
    assert result.returncode == 0, "Нативное ядро завершилось с критической ошибкой!"
    
    output = result.stdout
    # Проверяем, что все фазы мимикрии отработали успешно
    assert "Нативная архитектура BanachOS успешно развернута" in output
    
    # Проверка работы Mojo-транслятора Wi-Fi и Bluetooth
    assert "Обнаружен бинарный формат: ANDROID_ELF" in output
    assert "Сквозная универсальная трансляция кода завершена" in output

def test_equatorial_register_lockup():
    """Тест на автоматическое отражение хакерской атаки брутфорса (256 ≡ 0)."""
    result = subprocess.run(["./banachos_core"], capture_output=True, text=True)
    output = result.stdout
    
    # Проверяем активацию защиты и аннигиляцию скомпрометированной соты контракта
    assert "Попытка нарушения консенсуса или атаки double-spend" in output
    assert "Сота вывернута в 0.1" in output
    assert "Вытеснение успешно инициировано и выведено на экран" in output

def test_installer_symbiote_generation():
    """Проверка, что banachos_dropper.py успешно запекает монолитный имплант."""
    # При вызове 'make' дроппер должен автоматически создать скрипт-упаковщик
    assert os.path.exists("banachos_installer.sh"), "Полиморфный контекстный дроппер не собрался!"
    
    with open("banachos_installer.sh", "r", encoding="utf-8") as f:
        content = f.read()
        
    assert "#!/system/bin/sh" in content, "Неверный шебанг мобильного скрипта прошивки!"
    assert "base64 -d" in content, "В монолите отсутствует декодер Base64-матрицы ОЗУ!"

def teardown_module(module):
    """Финальная очистка временных бинарных блоков из файловой системы bFS."""
    subprocess.run(["make", "clean"], capture_output=True)
