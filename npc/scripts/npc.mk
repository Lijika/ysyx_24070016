ARGS ?=
IMG ?=
DIFF_REF_SO = $(NEMUISO)
ARGS_DIFF = --diff=$(DIFF_REF_SO)

override ARGS += --log=$(BUILD_DIR)/npc-log.txt $(ARGS_DIFF)
NPC_EXEC := $(VBIN) $(ARGS) $(IMG)

CINCLUDES = $(addprefix -CFLAGS , $(CINC_PATH))

$(VBIN): $(CSRC) $(VSRC)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "[VERILATE] $(notdir $(OBJ_DIR))/V$(TOPNAME)"
	@verilator $(VFLAGS) $(CSRC) $(CINCLUDES) $(VERILOGTOP)
	@make -s -C $(OBJ_DIR) -f $(BUILDMK)

sim: $(VBIN)


run: sim
	$(NPC_EXEC)

gdb: $(VBIN) $(NEMUISO) $(IMG)
	@echo "[GDB IMG]" $(notdir $(IMG))
	@gdb -s $(VBIN) --args $(NPC_EXEC)

clean:
	@echo rm -rf OBJ_DIR *vcd
	@rm -rf $(OBJ_DIR)
	@rm -rf *.vcd

# clean-all:
# 	@echo rm -rf OBJ_DIR *vcd NEMU_DIFF
# 	@rm -rf $(OBJ_DIR)
# 	@rm -rf *.vcd
# 	@make -s -C $(NEMU_DIR) clean

.PHONY: clean