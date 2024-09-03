TOPNAME = ysyx_24070016_top
# NXDC_FILES = constr/ysyx_24070016_top.nxdc

VERILATOR = verilator
VERILATOR_CFLAGS += -MMD --build -cc --trace -Wall

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

$(shell mkdir -p $(BUILD_DIR))

#项目源文件
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")

#nvboard
# include $(NVBOARD_HOME)/scripts/nvboard.mk
# INCFLAGS = $(addprefix -I, $(INC_PATH))
# CFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""
# LDFLAGS += -lSDL2 -lSDL2_image
# CSRCS += $(SRC_AUTO_BIND)
##约束文件
# SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
# $(SRC_AUTO_BIND): $(NXDC_FILES)
# 	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

all: sim
	@echo "Write this Makefile by your self."

# sim_nvbd: $(CSRCS) $(VSRCS) $(NVBOARD_ARCHIVE)
# 	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
# 	verilator $(VERILATOR_CFLAGS) \
# 		-top $(TOPNAME) \
# 		$^ \
# 		$(addprefix -CFLAGS , $(CFLAGS)) \
# 		$(addprefix -LDFLAGS , $(LDFLAGS)) \
# 		-Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

sim: $(CSRCS) $(VSRCS)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	verilator $(VERILATOR_CFLAGS) \
		-top $(TOPNAME) \
		$^ \
		-Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

.PHONY: all sim

include ../Makefile