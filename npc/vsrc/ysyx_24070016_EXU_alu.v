module ysyx_24070016_EXU_alu(
  input [3:0] sel_aluop,
	input [31:0] alu_src1,
	input [31:0] alu_src2,
	// input  aluop,
	output [31:0] alu_result,
  output zero, less
);

wire op_add  = (sel_aluop == 4'b0000);
wire op_lui  = (sel_aluop == 4'b0011);
wire op_sub  = (sel_aluop == 4'b1000);
wire op_slt  = (sel_aluop == 4'b0010);
wire op_sltu = (sel_aluop == 4'b1010);
wire op_xor  = (sel_aluop == 4'b0100);
wire op_or   = (sel_aluop == 4'b0110);
wire op_and  = (sel_aluop == 4'b0111);
wire op_sll  = (sel_aluop == 4'b0001);
wire op_srl  = (sel_aluop == 4'b0101);
wire op_sra  = (sel_aluop == 4'b1101);

wire Cin = (op_sub | op_slt | op_sltu) ? 1'b1 : 1'b0;

wire [31:0] xor_result = alu_src1 ^ alu_src2;
wire [31:0] or_result = alu_src1 | alu_src2;
wire [31:0] and_result = alu_src1 & alu_src2;
wire [31:0] lui_result = alu_src2;

wire [31:0] adder_result;
wire [31:0] adder_a = alu_src1;
wire [31:0] adder_b = alu_src2;
wire carry, overflow;
ysyx_24070016_EXU_alu_adder u_ysyx_24070016_exu_alu_adder(
  .adder_a        (adder_a      ),
  .adder_b        (adder_b      ),
  .Cin            (Cin          ),
  .adder_result   (adder_result ),
  .adder_carry    (carry        ),
  .adder_overflow (overflow     )
);

wire [31:0] slt_result;
assign slt_result[31:1] = 31'b0;
assign slt_result[0]  = (overflow ^ adder_result[31]);

wire [31:0] sltu_result;
assign sltu_result[31:1] = 31'b0;
assign sltu_result[0] = (carry    ^ Cin             );

wire [31:0] sll_result = alu_src1 << alu_src2[4:0];
wire [31:0] srl_result = alu_src1 >> alu_src2[4:0];
wire [31:0] sra_result = ($signed(alu_src1)) >>> alu_src2[4:0];

//alu output result
assign alu_result = ({32{op_add | op_sub}} & adder_result)
                  | ({32{op_lui}}  & lui_result)
                  | ({32{op_slt}}  & slt_result)
                  | ({32{op_sltu}} & sltu_result)
                  | ({32{op_xor}}  & xor_result)
                  | ({32{op_or}}   & or_result)
                  | ({32{op_and}}  & and_result)
                  | ({32{op_sll}}  & sll_result)
                  | ({32{op_srl}}  & srl_result)
                  | ({32{op_sra}}  & sra_result);

assign zero = (adder_result == 32'b0);

assign less = op_slt ? slt_result[0] : sltu_result[0];

endmodule
