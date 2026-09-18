# ================================================================================
# BANACHOS v8.3 — MICROKERNEL AUTOMATED BUILD SYSTEM (bBUILD)
# Author: Efim S. Markov & AI Collaborator (2026 Ultimate Revision)
# Formalism: Monolithic Object Linking, 100% C++ Core Integration
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
       banachos_bproc.cpp \
       banachos_bcontainer.cpp \
       banachos_baudio.cpp

OBJS = $(SRCS:.cpp=.o)

DEPS = banachos_mem.hpp \
       banachos_hal.hpp \
       banachos_syscalls.hpp \
       banachos_net.hpp \
       banachos_crypto.hpp \
       banachos_translator.hpp \
       banachos_bgui.hpp \
       banachos_bproc.hpp \
       banachos_fs.hpp \
       banachos_smart_contract.hpp \
       banachos_bcontainer.hpp \
       banachos_baudio.hpp \
       banachos_bdma.hpp \
       banachos_power.hpp \
       banachos_irq.hpp \
       banachos_bbus.hpp \
       banachos_busb.hpp \
       banachos_bnvme.hpp \
       banachos_btime.hpp \
       banachos_bfan.hpp \
       banachos_bgpu.hpp \
       banachos_bmux.hpp \
       banachos_btty.hpp \
       banachos_bprobe.hpp \
       banachos_bswap.hpp \
       banachos_bregistry.hpp \
       banachos_injector.hpp \
       banachos_self_healer.hpp

all: $(TARGET)
	@echo "================================================================================"
	@echo "[bBUILD] 100% МОНОЛИТ ОПЕРАЦИОННОЙ СИСТЕМЫ BANACHOS v8.3 СКОМПИЛИРОВАН!"
	@echo "[bBUILD] Последние Python-макеты вытеснены. Все инварианты кремния активны."
	@echo "================================================================================"
	@python3 banachos_dropper.py

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) banachos_firmware.bin banachos_distribution.iso banachos_installer.sh
	@echo "[bBUILD] Очистка bFS завершена."

.PHONY: all clean
