module ysyx_24070016_EXU (
	//data
	input [31:0] pc,
	input [31:0] exu_src1,
	input [31:0] exu_src2,
	input [31:0] exu_imm,
	//control
	input sel_alusrc1,
  input [1:0] sel_alusrc2,
  input [3:0] sel_aluop,
	input ebreak,

	output [31:0] exu_result,
  output zero, less
);
/* verilator lint_off UNUSEDSIGNAL */

//DPI-C ebreak
import "DPI-C" function void ebreak_detected(input bit ebreak);

always @(*) begin
	if (ebreak) begin
		ebreak_detected(ebreak);
	end
end
//alu
wire [31:0] alu_src1;
wire [31:0] alu_src2;
wire [31:0] alu_result;
ysyx_24070016_MuxKey #(2, 1, 32)u_ysyx_24070016_alu_src1_mux(
  .out (alu_src1    ),
  .key (sel_alusrc1 ),
  .lut ({
        1'b0, exu_src1,
        1'b1, pc
  } )
);
ysyx_24070016_MuxKey #(3, 2, 32)u_ysyx_24070016_MuxKey(
  .out (alu_src2    ),
  .key (sel_alusrc2 ),
  .lut ({
        2'b00, exu_src2,
        2'b01, exu_imm,
        2'b10, 32'h00000004
  } )
);
ysyx_24070016_EXU_alu u_ysyx_24070016_exu_alu(
  .sel_aluop  (sel_aluop  ),
	.alu_src1   (alu_src1   ),
	.alu_src2   (alu_src2   ),
	.alu_result (alu_result ),
  .zero       (zero       ),
  .less       (less       )
);
assign exu_result = alu_result;

/* verilator lint_off UNUSEDSIGNAL */
endmodule //ysyx_24070016_EXU 

