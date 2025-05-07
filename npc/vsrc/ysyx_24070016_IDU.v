module ysyx_24070016_IDU (
	input [31:0] inst,
	//data
	output [4:0] dec_rs1,
	output [4:0] dec_rs2,
	output [4:0] dec_rd,
	output [31:0] dec_imm,
	//control
	output rf_wen,
	output sel_alusrc1,
  output [1:0] sel_alusrc2,
  output [3:0] sel_aluop, 
  output [2:0] sel_branch,
  output [1:0] branch_intr,
  output [2:0] sel_memop,
  output [1:0] sel_wb_to_reg,
  output sel_csrt_is_rs1,
  output [1:0] csr_op,
  output mem_valid,
  output mem_wren,
	output ebreak
);
/* verilator lint_off UNUSEDSIGNAL */
assign dec_rs1 = inst[19:15];
assign dec_rs2 = inst[24:20];
assign dec_rd = inst[11:7];

wire [6:0] opcode;
wire [2:0] func3;
wire [6:0] func7;
assign opcode = inst[6:0];
assign func3 = inst[14:12];
assign func7 = inst[31:25];

wire EBREAK = (dec_imm == 32'b1);

//opcode
wire op_imm    = (opcode == 7'b0010011);
wire op_auipc  = (opcode == 7'b0010111);
wire op_lui    = (opcode == 7'b0110111);
wire op_jal    = (opcode == 7'b1101111);
wire op_jalr   = (opcode == 7'b1100111);
wire op_op     = (opcode == 7'b0110011);
wire op_branch = (opcode == 7'b1100011);
wire op_store  = (opcode == 7'b0100011);
wire op_laod   = (opcode == 7'b0000011);

wire op_SYSTEM = (opcode == 7'b1110011);

//func3
wire func3_000 = (func3 == 3'b000);
wire func3_001 = (func3 == 3'b001);
wire func3_010 = (func3 == 3'b010);
wire func3_011 = (func3 == 3'b011);
wire func3_100 = (func3 == 3'b100);
wire func3_101 = (func3 == 3'b101);
wire func3_110 = (func3 == 3'b110);
wire func3_111 = (func3 == 3'b111);

//func7
wire func7_0000000 = (func7 == 7'b0000000);
wire func7_0100000 = (func7 == 7'b0100000);
wire func7_0011000 = (func7 == 7'b0011000);

//func12
wire func12_000000000000 = (inst[31:20] == 12'b0);
wire func12_000000000001 = (inst[31:20] == 12'b1);

//inst type
wire [4:0] num_type;
wire I_type = op_imm | op_jalr | op_laod | op_SYSTEM;
wire S_type = op_store;
wire B_type = op_branch;
wire U_type = op_auipc | op_lui;
wire J_type = op_jal;


/************************
* Instruction
************************/
//r type
wire inst_add   = func7_0000000 & func3_000 & op_op;
wire inst_slt   = func7_0000000 & func3_010 & op_op;
wire inst_sltu  = func7_0000000 & func3_011 & op_op;
wire inst_sub   = func7_0100000 & func3_000 & op_op;
wire inst_xor   = func7_0000000 & func3_100 & op_op;
wire inst_or    = func7_0000000 & func3_110 & op_op;
wire inst_and   = func7_0000000 & func3_111 & op_op;
wire inst_sll   = func7_0000000 & func3_001 & op_op;
wire inst_srl   = func7_0000000 & func3_101 & op_op;
wire inst_sra   = func7_0100000 & func3_101 & op_op;
wire inst_mret  = func7_0011000 & func3_000 & op_SYSTEM;
//i type
wire inst_addi  =                 func3_000 & op_imm;
wire inst_jalr  =                 func3_000 & op_jalr;
wire inst_slti  =                 func3_010 & op_imm;
wire inst_sltiu =                 func3_011 & op_imm;
wire inst_xori  =                 func3_100 & op_imm;
wire inst_ori   =                 func3_110 & op_imm;
wire inst_andi  =                 func3_111 & op_imm;
wire inst_slli  = func7_0000000 & func3_001 & op_imm;
wire inst_srli  = func7_0000000 & func3_101 & op_imm;
wire inst_srai  = func7_0100000 & func3_101 & op_imm;
wire inst_lb    =                 func3_000 & op_laod;
wire inst_lh    =                 func3_001 & op_laod;
wire inst_lw    =                 func3_010 & op_laod;
wire inst_lbu   =                 func3_100 & op_laod;
wire inst_lhu   =                 func3_101 & op_laod;
wire inst_csrrw =                 func3_001 & op_SYSTEM;
wire inst_csrrs =                 func3_010 & op_SYSTEM;
wire inst_ecall = func12_000000000000 & (dec_rs2 == 5'b0) &func3_000 & (dec_rd == 5'b0) & op_SYSTEM;
//s type
wire inst_sb    =                 func3_000 & op_store;
wire inst_sh    =                 func3_001 & op_store;
wire inst_sw    =                 func3_010 & op_store;
//b type
wire inst_beq   =                 func3_000 & op_branch;
wire inst_bne   =                 func3_001 & op_branch;
wire inst_blt   =                 func3_100 & op_branch;
wire inst_bge   =                 func3_101 & op_branch;
wire inst_bltu  =                 func3_110 & op_branch;
wire inst_bgeu  =                 func3_111 & op_branch;
//u type
wire inst_auipc =                             op_auipc;
wire inst_lui   =                             op_lui;
//j type
wire inst_jal   =                             op_jal;

// wire inst_ebreak = func12_000000000001 & (dec_rs2 == 5'b0) &func3_000 & (dec_rd == 5'b0) & op_SYSTEM;
wire inst_ebreak = op_SYSTEM & func3_000 & EBREAK & (dec_rs1 == 5'b0) & (dec_rd == 5'b0);

/************************
* Imm generate
************************/
wire [31:0] imm;
assign num_type = {I_type, S_type, B_type, U_type, J_type};
ysyx_24070016_idu_immgen u_ysyx_24070016_idu_immgen(
	.num_type (num_type ),	//i
	.inst     (inst     ),
	.imm      (imm      )	//o
	);
assign dec_imm = imm;

/************************
* Control sign generate
************************/
assign ebreak = inst_ebreak;
assign rf_wen = op_imm | op_op | op_laod
              | inst_auipc | inst_lui | inst_jal | inst_jalr | inst_csrrw | inst_csrrs;

//alusrc
wire alusrc2_00 = inst_beq | inst_bne | op_op | op_branch;
wire alusrc2_01 = inst_auipc | inst_lui | op_imm | op_laod | op_store;
wire alusrc2_10 = inst_jal | inst_jalr;
wire alusrc2_11 = inst_csrrw | inst_csrrs;

assign sel_alusrc1 = inst_auipc | inst_jal | inst_jalr;
assign sel_alusrc2 = ({2{alusrc2_00}} & 2'b00)
                   | ({2{alusrc2_01}} & 2'b01)
                   | ({2{alusrc2_10}} & 2'b10)
                   | ({2{alusrc2_11}} & 2'b11);

//aluop
wire aluop_0000 = inst_addi | inst_auipc | inst_jal | inst_jalr | inst_add | op_laod | op_store;
wire aluop_1000 = inst_sub;
wire aluop_0011 = inst_lui;
wire aluop_0010 = inst_beq | inst_bne | inst_slti | inst_slt | inst_blt | inst_bge;
wire aluop_1010 = inst_sltiu | inst_sltu | inst_bltu | inst_bgeu;
wire aluop_0100 = inst_xori | inst_xor;
wire aluop_0110 = inst_ori | inst_or | inst_csrrs;
wire aluop_0111 = inst_andi | inst_and;
wire aluop_0001 = inst_slli | inst_sll;
wire aluop_0101 = inst_srli | inst_srl;
wire aluop_1101 = inst_srai | inst_sra;
assign sel_aluop = ({4{aluop_0000}} & 4'b0000)
                 | ({4{aluop_1000}} & 4'b1000)
                 | ({4{aluop_0010}} & 4'b0010)
                 | ({4{aluop_0011}} & 4'b0011)
                 | ({4{aluop_1010}} & 4'b1010)
                 | ({4{aluop_0100}} & 4'b0100)
                 | ({4{aluop_0110}} & 4'b0110)
                 | ({4{aluop_0111}} & 4'b0111)
                 | ({4{aluop_0001}} & 4'b0001)
                 | ({4{aluop_0101}} & 4'b0101)
                 | ({4{aluop_1101}} & 4'b1101);

//branch
// wire branch_000 = inst_addi | inst_auipc | inst_lui | inst_add | inst_slti;
assign branch_intr = {inst_mret, inst_ecall};
wire branch_001 = inst_jal;
wire branch_010 = inst_jalr;
wire branch_100 = inst_beq;
wire branch_101 = inst_bne;
wire branch_110 = inst_blt | inst_bltu;
wire branch_111 = inst_bge | inst_bgeu;
assign sel_branch = ({3{branch_001}} & 3'b001)
                  | ({3{branch_010}} & 3'b010)
                  | ({3{branch_100}} & 3'b100)
                  | ({3{branch_101}} & 3'b101)
                  | ({3{branch_110}} & 3'b110)
                  | ({3{branch_111}} & 3'b111);

//mem
assign mem_valid = op_laod | op_store;
assign mem_wren = op_store;
wire memop_000 = inst_lb | inst_sb;
wire memop_001 = inst_lh | inst_sh;
wire memop_100 = inst_lbu;
wire memop_101 = inst_lhu;
wire memop_010 = inst_lw |inst_sw;
assign sel_memop = ({3{memop_000}} & 3'b000)
                 | ({3{memop_001}} & 3'b001)
                 | ({3{memop_100}} & 3'b100)
                 | ({3{memop_101}} & 3'b101)
                 | ({3{memop_010}} & 3'b010);

//csr
assign sel_csrt_is_rs1 = inst_csrrw;
assign csr_op[0] = inst_csrrs | inst_csrrw;
assign csr_op[1] = inst_ecall;

//write back sel
wire wb_to_reg_01 = op_laod;
wire wb_to_reg_10 = inst_csrrw | inst_csrrs;
assign sel_wb_to_reg = ({2{wb_to_reg_01}} & 2'b01)
                     | ({2{wb_to_reg_10}} & 2'b10);




/* verilator lint_off UNUSEDSIGNAL */
endmodule
