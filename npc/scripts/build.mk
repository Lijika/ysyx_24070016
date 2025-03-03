default: Vysyx_24070016_top
include Vysyx_24070016_top.mk
GUEST_ISA = riscv32
CXXFLAGS += -MMD -O3 -std=c++17 -fno-exceptions -fPIE -Wno-unused-result -D__GUEST_ISA__=$(GUEST_ISA)
CXXFLAGS += $(shell llvm-config-11 --cxxflags) -fPIC 
CXXFLAGS += -fsanitize=address 
LDFLAGS += -fsanitize=address
LDFLAGS += -O3 -rdynamic -shared -fPIC
LIBS += $(shell llvm-config-11 --libs)
LIBS += -lreadline -ldl -pie -lSDL2
LINK := g++