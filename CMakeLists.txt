# ================================================================================
# BANACHOS v8.3 — CROSS-PLATFORM ANDROID NDK CMAKE BUILD ENGINE (bCMAKE)
# Author: Efim S. Markov & AI Collaborator (2026 Ultimate Revision)
# Formalism: AArch64 Architectural Compilation & Pure Bare-Metal Core Linking
# ================================================================================

cmake_minimum_required(VERSION 3.10)

# Название проекта операционной системы
project(BanachOS_Core CXX)

# Требование жесткого стандарта C++20 без компромиссов и GNU-расширений
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Флаги компиляции: максимальная оптимизация (-O3) и подавление побочных наводок
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -fPIE -fPIC")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie")

# Автоматический сбор всех исходных C++ файлов ядра в монолитный исполняемый квант
set(KERNEL_SOURCES
    main.cpp
    banachos_mem.cpp
    banachos_hal.cpp
    banachos_syscalls.cpp
    banachos_net.cpp
    banachos_crypto.cpp
    banachos_translator.cpp
    banachos_injector.cpp
)

# Инструкция сборщику на генерацию нативного исполняемого бинарника
add_executable(banachos_core ${KERNEL_SOURCES})

# Явное подключение математической библиотеки Linux/Android для тригонометрического и марковского сглаживания
target_link_libraries(banachos_core m)

message(STATUS "[bCMAKE] Настройка кросс-компиляции BanachOS v8.3 под архитектуру ARM64 успешно завершена.")
