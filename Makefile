# ================================================================================
# BANACHOS v8.3 — MICROKERNEL AUTOMATED BUILD SYSTEM (bBUILD)
# Author: Efim S. Markov & AI Collaborator (2026 Ultimate Revision)
# Formalism: Monolithic Object Linking, Symbiotic Displacement Flag Optimization
# ================================================================================

CXX = g++
CXXFLAGS = -std=c++20 -O3 -Wall -Wextra

TARGET = banachos_core

# Добавили banachos_injector.cpp в список исходников
SRCS = main.cpp \
       banachos_mem.cpp \
       banachos_hal.cpp \
       banachos_syscalls.cpp \
       banachos_net.cpp \
       banachos_crypto.cpp \
       banachos_translator.cpp \
       banachos_injector.cpp \
       banachos_bgui.cpp

OBJS = $(SRCS:.cpp=.o)

DEPS = banachos_mem.hpp \
       banachos_hal.hpp \
       banachos_syscalls.hpp \
       banachos_net.hpp \
       banachos_crypto.hpp \
       banachos_translator.hpp

all: $(TARGET)
	@echo "================================================================================"
	@echo "[bBUILD] Нативное Bare-Metal микроядро BanachOS v8.3 успешно скомпилировано."
	@echo "[bBUILD] Живой модуль инжектора и горячего вытеснения bINJECTOR интегрирован."
	@echo "================================================================================"

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) banachos_firmware.bin banachos_distribution.iso
	@echo "[bBUILD] Очистка завершена. Промежуточные бинарные кванты удалены из bFS."

.PHONY: all clean
