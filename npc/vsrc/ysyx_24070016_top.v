module ysyx_24070016_top(
	input clk,
	input rst,
	//Mem port
	output [31:0] inst_mem_addr,
	output inst_mem_wen,
	output [31:0] inst_mem_wdata,
	input [31:0] inst_mem_rdata,
	
	output [31:0] data_mem_addr,
	output data_mem_wen,
	output [31:0] data_mem_wdata,
	input [31:0] data_mem_rdata
);

wire [31:0] pc;
wire [31:0] nextpc;
ysyx_24070016_Reg #(
	.WIDTH		(32            ),
	.RESET_VAL	(32'h80000000  )
	)u_ysyx_24070016_PCreg(
	.clk	(clk     ),
	.rst	(rst     ),
	.din	(nextpc  ),
	.dout	(pc      ),
	.wen	(1'b1    )
	);

wire [31:0] inst;
ysyx_24070016_IFU u_ysyx_24070016_IFU(
	.pc             (pc             ),	//i
	.inst_mem_rdata (inst_mem_rdata ),
	.inst           (inst           ),	//o
	.inst_mem_addr  (inst_mem_addr  ),
	.inst_mem_wdata (inst_mem_wdata ),
	.inst_mem_wen   (inst_mem_wen   )
	);

wire [4:0] dec_rs1;
wire [4:0] dec_rs2;
wire [4:0] dec_rd;
wire [31:0] dec_imm;
wire rf_wen, sel_rs2Isimm;
ysyx_24070016_IDU u_ysyx_24070016_IDU(
	.inst         (inst         ),	//i
	.dec_rs1      (dec_rs1      ),	//o
	.dec_rs2      (dec_rs2      ),
	.dec_rd       (dec_rd       ),
	.dec_imm      (dec_imm      ),
	.rf_wen       (rf_wen       ),
	.sel_rs2Isimm (sel_rs2Isimm )
);

wire [4:0] rf_raddr1 = dec_rs1;
wire [4:0] rf_raddr2 = dec_rs2;
wire [4:0] rf_waddr = dec_rd;
wire gr_wen = (rf_waddr == 5'b0) ? 0 : rf_wen;
wire [31:0] rf_wdata;
wire [31:0] rf_rdata1;
wire [31:0] rf_rdata2;
ysyx_24070016_RegisterFile #(
	.ADDR_WIDTH (5),
	.DATA_WIDTH (32)
	) u_ysyx_24070016_RegisterFile(
	.clk    (clk    ),
	.raddr1 (rf_raddr1 ),
	.raddr2 (rf_raddr2 ),
	.wdata  (rf_wdata  ),
	.waddr  (rf_waddr  ),
	.wen    (gr_wen    ),
	.rdata1 (rf_rdata1 ),
	.rdata2 (rf_rdata2 )
);

wire [31:0] exu_src1 = rf_rdata1;
wire [31:0] exu_src2 = rf_rdata2;
wire [31:0] exu_imm = dec_imm;
wire [31:0] wrback_result;
assign rf_wdata = wrback_result;
ysyx_24070016_EXU u_ysyx_24070016_EXU(
	.exu_src1      (exu_src1      ),
	.exu_src2      (exu_src2      ),
	.exu_imm       (exu_imm       ),
	.sel_rs2Isimm  (sel_rs2Isimm  ),
	.wrback_result (wrback_result )
);

endmodule
