#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
================================================================================
BANACHOS v8.3 — INTEGRATION RUNTIME RUNNER (bLang Execution)
Author: Efim S. Markov & AI Collaborator (2026)
Formalism: End-to-End Compilation & Protected Gateway Invocation
Strictly Zero-Fit Parameter Architecture (Academic Clean Edition)
================================================================================
"""

import sys
from banachos_hll_compiler import BanachOSHLLCompiler
from banachos_syscalls import BanachOSSyscallGateway

def main():
    print("================================================================================")
    print(" BANACHOS v8.3 — ЗАПУСК ПЕРВОЙ ВЫСОКОУРОВНЕВОЙ ПРОГРАММЫ НА ЯЗЫКЕ bLang")
    print("================================================================================\n")

    # Инициализация компилятора и шлюза системных вызовов
    compiler = BanachOSHLLCompiler(token_space_size=16)
    gateway = BanachOSSyscallGateway(api_matrix_capacity=16)

    # Регистрация адресного пространства исполняемого процесса (PID: 777)
    app_pid = 777
    gateway.register_application_space(pid=app_pid, coords=(5, 5, 5))
    print(f"[bPROC]: Создано пространство пользователя для процесса PID {app_pid}.")

    # Исходный код первой легитимной программы bLang
    source_program = """ALLOC_MEM 10.0;
WRITE_DISK 5.0;
SEND_PACKET 2.0;
"""
    print("\n[bCOMPILER]: Исходный код программы bLang для сборки:")
    print("--------------------------------------------------")
    print(source_program.strip())
    print("--------------------------------------------------")

    # 1. Сборка программы компилятором bLang
    success, bytecode, msg = compiler.compile_source_code(source_program)
    print(f"\n[bCOMPILER]: {msg}")
    if not success:
        sys.exit(1)

    print(f"[bCOMPILER]: Сгенерирован байт-код: {bytecode}")

    # 2. Исполнение байт-кода процессом через геометрический API-шлюз bSYS
    print("\n[bSYS]: Запуск пошагового выполнения программы...")
    print("--------------------------------------------------")
    for op_code, intensity in bytecode:
        api_success, api_msg = gateway.invoke_syscall(pid=app_pid, syscall_vector=op_code, payload_intensity=intensity)
        print(f" -> Вызов {hex(op_code)} с интенсивностью {intensity}: {api_msg}")
        if not api_success:
            print("[bSYS]: Выполнение программы аварийно прервано защитой ядра.")
            break
    print("--------------------------------------------------")

    # 3. Имитация атаки переполнения буфера (Шторм вызовов)
    print("\n[bSYS]: Тестирование аппаратного иммунитета ядра (Инжекция вредоносного флуда)...")
    print("--------------------------------------------------")
    malicious_program = "ALLOC_MEM 300.0;\n"
    print(f"[bCOMPILER]: Попытка собрать эксплойт: {malicious_program.strip()}")
    
    # Наш компилятор bLang имеет встроенную защиту и заблокирует опасную сборку еще до запуска
    comp_success, _, comp_msg = compiler.compile_source_code(malicious_program)
    print(f"[bCOMPILER]: {comp_msg}")
    
    if not comp_success:
        print("\n[БЕЗОПАСНОСТЬ]: Атака успешно заблокирована компилятором на этапе статического анализа!")
    else:
        # Если бы код прошел, шлюз bSYS аппаратно уничтожил бы зависший поток
        print("\n[bSYS]: Передача сырого импульса напрямую в шлюз...")
        api_success, api_msg = gateway.invoke_syscall(pid=app_pid, syscall_vector=0x10, payload_intensity=300.0)
        print(f" -> {api_msg}")

    print("\n================================================================================")
    print(" ТЕЛЕМЕТРИЯ СТАБИЛЬНОСТИ СИСТЕМЫ ПОСЛЕ ВЫПОЛНЕНИЯ:")
    print("================================================================================")
    print(f" Статус компилятора bLang:  {compiler.get_hll_status()['compiler_safety_status']}")
    print(f" Статус API-шлюза ядра bSYS: {gateway.get_gateway_status()['syscalls_security_status']}")
    print("================================================================================")

if __name__ == "__main__":
    main()
