module ysyx_24070016_IFU (
	input clk,
  input rst,
  input [1:0] branch_intr,
  input [2:0] sel_branch,
  input zero, less,
  input [31:0] reg_rs1,
  input [31:0] dec_imm,
  input [31:0] csr_mepc,
  input [31:0] csr_mtvec,

  output [31:0] pc,
  output [31:0] sim_dnpc,
  output [31:0] inst
);

wire [31:0] nextpc = (branch_intr == 2'b0) ? pc_adder : pc_intr;
assign sim_dnpc = nextpc;
ysyx_24070016_Reg #(32, 32'h80000000)u_ysyx_24070016_PCreg(
	.clk	(clk     ),
	.rst	(rst     ),
	.din	(nextpc  ),
	.dout	(pc      ),
	.wen	(1'b1    )
);


/************************
* Next PC gen
************************/
//branch cond
wire branch_001_jumppc    = (sel_branch == 3'b001);
wire branch_010_jumpreg   = (sel_branch == 3'b010);
wire branch_100_equal     = (sel_branch == 3'b100);
wire branch_101_notequal  = (sel_branch == 3'b101);
wire branch_110_less      = (sel_branch == 3'b110);
wire branch_111_bigequal  = (sel_branch == 3'b111);

wire sel_pcsrc1 = branch_001_jumppc
                | branch_010_jumpreg
                | (branch_100_equal     & (zero == 1'b1))
                | (branch_101_notequal  & (zero == 1'b0))
                | (branch_110_less      & (less == 1'b1))
                | (branch_111_bigequal  & (less == 1'b0));
wire sel_pcsrc2 = branch_010_jumpreg;

//pc adder
wire [31:0] pcsrc1, pcsrc2;
ysyx_24070016_MuxKey #(2, 1, 32) u_ysyx_24070016_pcsrc1_mux(
  .out (pcsrc1     ),
  .key (sel_pcsrc1 ),
  .lut ({
        1'b0, 32'h00000004,
        1'b1, dec_imm
  } )
);
ysyx_24070016_MuxKey #(2, 1, 32) u_ysyx_24070016_pcsrc2_mux(
  .out (pcsrc2     ),
  .key (sel_pcsrc2 ),
  .lut ({
        1'b0, pc,
        1'b1, reg_rs1
  } )
);
wire [31:0] pc_adder = pcsrc1 + pcsrc2;

//branch intr
wire [31:0] pc_intr = ({32{branch_intr[0]}} & csr_mtvec)
                    | ({32{branch_intr[1]}} & csr_mepc);

//dpic sdb fetch inst
import "DPI-C" function void fetch_instruction(input int unsigned addr, output int unsigned rword);
reg [31:0] t_inst;
always@(*) begin
  if(!rst) fetch_instruction(pc, t_inst);
  else t_inst = 32'b0;
end

assign inst = t_inst;

endmodule
