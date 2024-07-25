TOPNAME = top
NXDC_FILES = constr/top.nxdc

VERILATOR = verilator
VERILATOR_CFLAGS += -MMD --build -cc --trace -Wall

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

$(shell mkdir -p $(BUILD_DIR))

#约束文件
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

#项目源文件
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
CSRCS += $(SRC_AUTO_BIND)

#nvboard
include $(NVBOARD_HOME)/scripts/nvboard.mk

#verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
CFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""
LDFLAGS += -lSDL2 -lSDL2_image

all: sim
	@echo "Write this Makefile by your self."

sim: $(CSRCS) $(VSRCS) $(NVBOARD_ARCHIVE)
#	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	verilator $(VERILATOR_CFLAGS) \
		-top $(TOPNAME) \
		$^ \
		$(addprefix -CFLAGS , $(CFLAGS)) \
		$(addprefix -LDFLAGS , $(LDFLAGS)) \
		-Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

.PHONY: all sim