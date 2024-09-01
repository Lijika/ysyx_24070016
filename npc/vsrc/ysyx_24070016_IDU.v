module ysyx_24070016_IDU (
	input [31:0] inst,
	//data
	output [4:0] dec_rs1,
	output [4:0] dec_rs2,
	output [4:0] dec_rd,
	output [31:0] dec_imm,
	//control
	output rf_wen,
	output sel_rs2Isimm
);

assign dec_rs1 = inst[19:15];
assign dec_rs2 = inst[24:20];
assign dec_rd = inst[11:7];

wire [6:0] opcode;
wire [2:0] func3;
wire [6:0] func7;
assign opcode = inst[6:0];
assign func3 = inst[14:12];
assign func7 = inst[31:25];

wire opcode_1_0_11 = (opcode[1:0] == 2'b11);

wire opcode_3_2_00 = (opcode[3:2] == 2'b00);
wire opcode_3_2_01 = (opcode[3:2] == 2'b01);
wire opcode_3_2_10 = (opcode[3:2] == 2'b10);
wire opcode_3_2_11 = (opcode[3:2] == 2'b11);

wire opcode_5_4_00 = (opcode[5:4] == 2'b00);
wire opcode_5_4_01 = (opcode[5:4] == 2'b01);
wire opcode_5_4_10 = (opcode[5:4] == 2'b10);
wire opcode_5_4_11 = (opcode[5:4] == 2'b11);

wire opcode_6_0 = (opcode[6] == 1'b0);
wire opcode_6_1 = (opcode[6] == 1'b1);

wire func3_000 = (func3 == 3'b000);
wire func3_001 = (func3 == 3'b001);
wire func3_010 = (func3 == 3'b010);
wire func3_011 = (func3 == 3'b011);
wire func3_100 = (func3 == 3'b100);
wire func3_101 = (func3 == 3'b101);
wire func3_110 = (func3 == 3'b110);
wire func3_111 = (func3 == 3'b111);

//opcode
wire op_imm = opcode_6_0 & opcode_5_4_01 & opcode_3_2_00 & opcode_1_0_11;

//decode instruction
wire inst_addi = op_imm & func3_000;

//instruction type
wire [4:0] num_type;
wire I_type = op_imm;
wire S_type = 1'b0;
wire B_type = 1'b0;
wire U_type = 1'b0;
wire J_type = 1'b0;
assign num_type = {I_type, S_type, B_type, U_type, J_type};

//imm
wire [31:0] imm;
ysyx_24070016_idu_immgen u_ysyx_24070016_idu_immgen(
	.num_type (num_type ),	//i
	.inst     (inst     ),
	.imm      (imm      )	//o
	);
assign dec_imm = imm;

//control sig
assign rf_wen = inst_addi;
assign sel_rs2Isimm = inst_addi;

endmodule
