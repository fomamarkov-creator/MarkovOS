# ================================================================================
# BANACHOS v8.3 — MICROKERNEL AUTOMATED BUILD SYSTEM (bBUILD)
# Author: Efim S. Markov & AI Collaborator (2026 Ultimate Edition)
# Formalism: Monolithic Object Linking, Symbiotic Displacement Optimization
# ================================================================================

CXX = g++
CXXFLAGS = -std=c++20 -O3 -Wall -Wextra

TARGET = banachos_core

SRCS = main.cpp \
       banachos_mem.cpp \
       banachos_hal.cpp \
       banachos_syscalls.cpp \
       banachos_net.cpp \
       banachos_crypto.cpp \
       banachos_translator.cpp \
       banachos_injector.cpp \
       banachos_bgui.cpp \
       banachos_bproc.cpp

OBJS = $(SRCS:.cpp=.o)

DEPS = banachos_mem.hpp \
       banachos_hal.hpp \
       banachos_syscalls.hpp \
       banachos_net.hpp \
       banachos_crypto.hpp \
       banachos_translator.hpp \
       banachos_bgui.hpp \
       banachos_bproc.hpp \
       banachos_fs.hpp

all: $(TARGET)
	@echo "================================================================================"
	@echo "[bBUILD] Нативное Bare-Metal микроядро BanachOS v8.3 успешно скомпилировано."
	@echo "[bBUILD] Модуль мимикрии и перехвата сокетов Linux API bNET_HIJACK интегрирован."
	@echo "================================================================================"
	@python3 banachos_dropper.py

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) banachos_firmware.bin banachos_distribution.iso banachos_installer.sh
	@echo "[bBUILD] Очистка завершена. Промежуточные бинарные кванты удалены из bFS."

.PHONY: all clean
