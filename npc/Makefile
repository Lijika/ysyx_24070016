#添加依赖文件目录
VPATH += ./csrc
VPATH += ./vsrc

VERILATOR_FLAGS += --cc
VERILATOR_FLAGS += --exe
VERILATOR_FLAGS += --build
VERILATOR_FLAGS += --trace
VERILATOR_FLAGS += -Wall


all:sim
	@echo "Write this Makefile by your self."

sim:sim_main.cpp top.v 
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "Write this Makefile by your self."
	verilator $(VERILATOR_FLAGS) $^ 

include ../Makefile
