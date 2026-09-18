#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
================================================================================
BANACHOS v8.3 — NATIVE HIGH-LEVEL LANGUAGE COMPILER & EMITTER (bLANG_COMPILER)
Author: Efim S. Markov & AI Collaborator (2026 Revision)
Formalism: bLang Lexical Parsing, Z/256Z Machine Code Generation & AST Verification
Strictly Zero-Fit Parameter Architecture (Academic Clean Edition)
================================================================================
"""

import numpy as np
from typing import Dict, List, Tuple, Any
from banachos_compiler import BanachOSCompiler

class BanachOSHllCompiler:
    """
    Двухуровневый компилятор языка bLang для операционной системы BanachOS.
    Парсит исходный текст программ, валидирует через геометрический цензор bCOMPILER
    и генерирует нативный машинный байт-код для целочисленных регистров Z/256Z.
    """
    # Карта кодов операций (Opcommands) нативной архитектуры процессора BanachOS
    OPCODES: Dict[str, int] = {
        "ALLOC_MEM":   0x10,  # Выделение гексагональной соты ОЗУ bMEM
        "WRITE_DISK":  0x20,  # Запись вектора деформации в хранилище bFS
        "SEND_PACKET": 0x30,  # Передача пакетного импульса в стек bNET
        "EMIT_PULSE":  0x40,  # Межпроцессный импульс в шину bIPC
        "ADD_REG":     0x50,  # Нативное сложение регистров в кольце вычетов
        "SUB_REG":     0x60   # Нативное вычитание регистров в кольце вычетов
    }

    def __init__(self, static_verifier: BanachOSCompiler):
        self.verifier = static_verifier
        self.compiled_binary: List[int] = []  # Массив байт готового бинарного файла ОС

    def compile_source_code(self, source_lines: List[str]) -> Tuple[bool, bytes, str]:
        """
        Компилирует текстовые строки bLang в исполняемый бинарный образ (.bin).
        Осуществляет жесткую сквозную верификацию Липшицева предела.
        """
        self.compiled_binary.clear()
        np.random.seed(1024) # Детерминированная укладка токенов в решетку

        print(f"[bCOMPILER] Накапливается исходный код программы... Строк обнаружено: {len(source_lines)}")

        for line_idx, line in enumerate(source_lines):
            # Очистка строки от комментариев и пробелов
            clean_line = line.split("#")[0].strip()
            if not clean_line:
                continue

            parts = clean_line.split()
            command = parts[0]
            
            if command not in self.OPCODES:
                return False, b"", f"COMPILE_FAIL: Синтаксическая ошибка на строке {line_idx + 1}: Неизвестный оператор '{command}'"

            # Вычисление логической сложности и аргумента
            try:
                argument_val = float(parts[1]) if len(parts) > 1 else 0.0
            except ValueError:
                return False, b"", f"COMPILE_FAIL: Ошибка парсинга аргумента на строке {line_idx + 1}"

            token_name = f"line_{line_idx}_{command}"
            
            # Детерминированно выделяем соту в AST-решетке для статического цензора
            z = int(np.abs(hash(token_name)) % self.verifier.size)
            y = int((line_idx * 3) % self.verifier.size)
            x = int((line_idx * 7) % self.verifier.size)
            
            self.verifier.register_code_token(token_name=token_name, coords=(z, y, x))
            
            # Рассчитываем вносимое оператором Липшицево натяжение
            logical_complexity = float(np.abs(argument_val) * 1.8 + 2.0)
            
            # Пропускаем оператор через геометрический Липшицев барьер стабильности
            verified, msg = self.verifier.compile_operator(token_name=token_name, logical_complexity=logical_complexity)
            
            if not verified:
                return False, b"", f"COMPILE_FAIL: Сборка заблокирована цензором! {msg}"

            # --- ЭМИССИЯ НАЛЕТУ: ГЕНЕРАЦИЯ БАЙТ-КОДА В КОЛЬЦЕ Z/256Z ---
            opcode = self.OPCODES[command]
            byte_arg = int(np.floor(argument_val)) % 256
            
            self.compiled_binary.append(opcode)
            self.compiled_binary.append(byte_arg)

        binary_image = bytes(self.compiled_binary)
        return True, binary_image, "COMPILE_SUCCESS: Бинарный образ успешно собран и верифицирован по Липшицу"

    def get_binary_disassembly(self, binary_image: bytes) -> List[str]:
        """Дизассемблирует нативный образ BanachOS обратно в инструкции."""
        disassembly = []
        opcode_reverse = {v: k for k, v in self.OPCODES.items()}
        
        for i in range(0, len(binary_image), 2):
            opcode = binary_image[i]
            arg = binary_image[i+1] if i+1 < len(binary_image) else 0
            cmd_name = opcode_reverse.get(opcode, f"UNKNOWN_OP_0x{opcode:02X}")
            disassembly.append(f"0x{i:04X}: {cmd_name} (mod 256 argument: {arg})")
            
        return disassembly
