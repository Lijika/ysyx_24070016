#添加依赖文件目录
VPATH += ./csrc
VPATH += ./vsrc

VERILATOR_FLAGS += --cc
VERILATOR_FLAGS += --exe
VERILATOR_FLAGS += --build
VERILATOR_FLAGS += --trace
VERILATOR_FLAGS += -Wall

#文件存放目录


all:sim
	@echo "Write this Makefile by your self."

sim:main.cpp example.v 
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "Write this Makefile by your self."
	verilator $(VERILATOR_FLAGS) main.cpp example.v

include ../Makefile
