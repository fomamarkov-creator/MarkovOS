# ================================================================================
# BANACHOS v8.3 — MICROKERNEL AUTOMATED BUILD SYSTEM (bBUILD)
# Author: Efim S. Markov & AI Collaborator (2026 Ultimate Revision)
# Formalism: Monolithic Object Linking, Symbiotic Displacement Flag Optimization
# ================================================================================

# Настройка компилятора под стандарт C++20 с максимальной оптимизацией скорости
CXX = g++
CXXFLAGS = -std=c++20 -O3 -Wall -Wextra

# Имя готового нативного бинарного файла мета-платформы
TARGET = banachos_core

# Список всех исходных модулей C++ реализации ядра
SRCS = main.cpp \
       banachos_mem.cpp \
       banachos_hal.cpp \
       banachos_syscalls.cpp \
       banachos_net.cpp \
       banachos_crypto.cpp \
       banachos_translator.cpp

# Автоматическая генерация объектных блоков (.o)
OBJS = $(SRCS:.cpp=.o)

# Заголовки для отслеживания зависимостей и сквозной компиляции
DEPS = banachos_mem.hpp \
       banachos_hal.hpp \
       banachos_syscalls.hpp \
       banachos_net.hpp \
       banachos_crypto.hpp \
       banachos_translator.hpp

# Главное правило сборки всей нативной операционной системы
all: $(TARGET)
	@echo "================================================================================"
	@echo "[bBUILD] Нативное Bare-Metal микроядро BanachOS v8.3 успешно скомпилировано."
	@echo "[bBUILD] Модуль мимикрии и вытеснения bHIJACKER интегрирован в бинарный образ."
	@echo "[bBUILD] Для эмуляции такт-цикла на хосте выполните: ./$(TARGET)"
	@echo "================================================================================"

# Линковка объектных блоков в единый монолитный бинарник
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Компиляция каждого отдельного .cpp файла в объектный модуль .o
%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка файловой системы репозитория от кэша сборки
clean:
	rm -f $(OBJS) $(TARGET) banachos_firmware.bin banachos_distribution.iso
	@echo "[bBUILD] Очистка завершена. Промежуточные бинарные кванты удалены из bFS."

.PHONY: all clean
