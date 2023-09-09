#添加依赖文件目录
VPATH += ./csrc : ./vsrc


all:sim
	@echo "Write this Makefile by your self."

sim:main.cpp example.v 
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "Write this Makefile by your self."
	verilator --cc --exe --build -trace -Wall main.cpp example.v

include ../Makefile
