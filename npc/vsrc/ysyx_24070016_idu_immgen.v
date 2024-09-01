module ysyx_24070016_idu_immgen (
	input [4:0] num_type,
	input [31:0] inst,
	output [31:0] imm
);

wire [31:0] imm_I = {{21{inst[31]}}, inst[30:25], inst[24:21], inst[20]};
wire [31:0] imm_S = {{21{inst[31]}}, inst[30:25], inst[11:8], inst[7]};
wire [31:0] imm_B = {{20{inst[31]}}, inst[7], inst[30:25], inst[11:8], 1'b0};
wire [31:0] imm_U = {inst[31], inst[30:20], inst[19:12], 12'b0};
wire [31:0] imm_J = {{12{inst[31]}}, inst[19:12], inst[20], inst[30:25], inst[24:21], 1'b0};

assign imm = ({32{num_type[4]}} & imm_I) 
			| ({32{num_type[3]}} & imm_S)
			| ({32{num_type[2]}} & imm_B)
			| ({32{num_type[1]}} & imm_U)
			| ({32{num_type[0]}} & imm_J);

endmodule