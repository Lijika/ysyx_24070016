TOPNAME = ysyx_24070016_top
GUEST_ISA = riscv32

BUILD_DIR = ./build
$(shell mkdir -p $(BUILD_DIR))
OBJ_DIR = $(BUILD_DIR)/obj_dir
CSRC_DIR = $(abspath ./csrc)
VSRC_DIR = $(abspath ./vsrc)

NEMUISO = $(NEMU_HOME)/build/riscv32-nemu-interpreter-so
VERILOGTOP = $(VSRC_DIR)/$(TOPNAME).v
VFLAGS = --trace --cc --exe -O3 --threads-dpi all -I$(VSRC_DIR) -Mdir $(OBJ_DIR)
VBIN =  $(OBJ_DIR)/$(addprefix V, $(TOPNAME))
VSRC = $(shell find $(VSRC_DIR) -name "*.v")
CSRC = $(shell find $(CSRC_DIR) -name "*.cc" -or -name "*.cpp")
CINC_PATH = -I$(NPC_HOME)
BUILDMK = $(abspath ./scripts/build.mk)

include ./scripts/npc.mk
include ../Makefile

# Include rules for menuconfig
include $(NPC_HOME)/scripts/config.mk