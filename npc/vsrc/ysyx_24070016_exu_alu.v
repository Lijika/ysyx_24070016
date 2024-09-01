module ysyx_24070016_exu_alu(
	input [31:0] alu_src1,
	input [31:0] alu_src2,
	// input  aluop,
	output [31:0] alu_result
);

wire [31:0] adder_a = alu_src1;
wire [31:0] adder_b = alu_src2;

wire [31:0] addi_result = adder_a + adder_b;

assign alu_result = addi_result;

endmodule