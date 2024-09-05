module ysyx_24070016_EXU (
	//data
	input [31:0] exu_src1,
	input [31:0] exu_src2,
	input [31:0] exu_imm,
	//control
	input sel_rs2Isimm,
	input ebreak,

	output [31:0] wrback_result
);

wire [31:0] alu_src1;
wire [31:0] alu_src2;
assign alu_src1 = exu_src1;
assign alu_src2 = sel_rs2Isimm ? exu_imm : exu_src2;
wire [31:0] alu_result;
ysyx_24070016_exu_alu u_ysyx_24070016_exu_alu(
	.alu_src1   (alu_src1   ),
	.alu_src2   (alu_src2   ),
	.alu_result (alu_result )
);

assign wrback_result = alu_result;

import "DPI-C" function void ebreak_detected(input bit ebreak);

endmodule //ysyx_24070016_EXU 

