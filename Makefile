# ================================================================================
# BANACHOS v8.3 — MICROKERNEL AUTOMATED BUILD SYSTEM (bBUILD)
# Author: Efim S. Markov & AI Collaborator (2026 Revision)
# Formalism: Monolithic Object Linking, Strict Compiler Flag Optimization
# ================================================================================

# Настройка компилятора под стандарт C++20
CXX = g++
CXXFLAGS = -std=c++20 -O3 -Wall -Wextra

# Имя итогового нативного бинарника операционной системы
TARGET = banachos_core

# Список всех исходных файлов реализации
SRCS = main.cpp \
       banachos_mem.cpp \
       banachos_hal.cpp \
       banachos_syscalls.cpp \
       banachos_net.cpp \
       banachos_crypto.cpp \
       banachos_translator.cpp

# Автоматическое определение объектных файлов (.o)
OBJS = $(SRCS:.cpp=.o)

# Заголовки для отслеживания изменений и автокомпиляции
DEPS = banachos_mem.hpp \
       banachos_hal.hpp \
       banachos_syscalls.hpp \
       banachos_net.hpp \
       banachos_crypto.hpp \
       banachos_translator.hpp

# Главная цель сборки операционной системы
all: $(TARGET)
	@echo "================================================================================"
	@echo "[bBUILD] Операционная система BanachOS v8.3 успешно собрана в монолит."
	@echo "[bBUILD] Для запуска нативного ядра выполните команду: ./$(TARGET)"
	@echo "================================================================================"

# Линковка объектных файлов в финальный исполняемый файл
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Компиляция каждого .cpp файла в объектный модуль .o
%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка репозитория от временных файлов компиляции
clean:
	rm -f $(OBJS) $(TARGET) banachos_firmware.bin banachos_distribution.iso
	@echo "[bBUILD] Временные бинарные блоки и кэш успешно удалены из bFS."

.PHONY: all clean
