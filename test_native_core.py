#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
================================================================================
BANACHOS v8.3 — NATIVE CORE & ARCHITECTURAL INTELLIGENT VERIFIER (bCORE_TEST)
Author: Efim S. Markov & AI Collaborator (2026 Revision)
Formalism: C++ Binary Validation, Automatic Missing Component Scan via CI/CD
Strictly Zero-Fit Parameter Architecture (Academic Clean Edition)
================================================================================
"""

import pytest
import subprocess
import os
import glob

def setup_module(module):
    """Автоматическая сборка нативного ядра BanachOS перед началом тестирования."""
    print("\n[CI/CD] Запуск промышленной сборки bBUILD...")
    subprocess.run(["make", "clean"], capture_output=True)
    result = subprocess.run(["make"], capture_output=True, text=True)
    assert result.returncode == 0, f"Крах компиляции ядра микроядра через Makefile:\n{result.stderr}"
    assert os.path.exists("banachos_core"), "Бинарный файл banachos_core не был сгенерирован!"

def test_check_for_forgotten_native_modules():
    """Интеллектуальный скан: проверяет, все ли старые Python-драйверы переведены на C++."""
    print("\n[SCANNER] Проверка архитектурной целостности репозитория...")
    
    # Находим все старые Python-макеты подсистем
    python_drivers = glob.glob("banachos_*.py")
    missing_cpp_headers = []

    for py_file in python_drivers:
        base_name = os.path.basename(py_file).replace(".py", "")
        # Исключаем служебные файлы тестов и отладки
        if base_name in ["banachos_debug", "banachos_crypto_symbiote"]:
            continue
            
        hpp_analog = f"{base_name}.hpp"
        # Если для старого .py файла нет .hpp аналога в репозитории — фиксируем пропуск
        if not os.path.exists(hpp_analog):
            missing_cpp_headers.append(hpp_analog)

    if missing_cpp_headers:
        error_msg = f"КРИТИЧЕСКИЙ РАЗРЫВ: Забыли перевести на C++ модули:\n" + "\n".join([f" - {m}" for m in missing_cpp_headers])
        # Принудительно вызываем сбой теста, чтобы бот вывел этот список в README.md
        assert False, error_msg

def test_native_binary_execution_flow():
    """Проверка базового детерминированного рантайма и фаз слияния."""
    result = subprocess.run(["./banachos_core"], capture_output=True, text=True)
    assert result.returncode == 0, "Нативное ядро завершилось с критической ошибкой!"
    output = result.stdout
    assert "Нативная архитектура BanachOS успешно развернута" in output
    assert "Обнаружен бинарный формат: ANDROID_ELF" in output
    assert "Сквозная универсальная трансляция кода завершена" in output

def test_equatorial_register_lockup():
    """Тест на автоматическое отражение хакерской атаки брутфорса (256 ≡ 0)."""
    result = subprocess.run(["./banachos_core"], capture_output=True, text=True)
    output = result.stdout
    assert "Попытка нарушения консенсуса или атаки double-spend" in output
    assert "Сота вывернута in 0.1" in output or "Сота вывернута в 0.1" in output
    assert "Вытеснение успешно инициировано и выведено на экран" in output

def test_installer_symbiote_generation():
    """Проверка, что banachos_dropper.py успешно запекает монолитный имплант."""
    assert os.path.exists("banachos_installer.sh"), "Полиморфный контекстный дроппер не собрался!"
    with open("banachos_installer.sh", "r", encoding="utf-8") as f:
        content = f.read()
    assert "#!/system/bin/sh" in content
    assert "base64 -d" in content

def teardown_module(module):
    """Финальная очистка временных бинарных блоков из файловой системы bFS."""
    subprocess.run(["make", "clean"], capture_output=True)
