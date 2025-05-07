module ysyx_24070016_top(
	input clk,
	input rst,
  output [31:0] cur_pc,
  output [31:0] sim_dnpc
	//Mem port
	// output [31:0] inst_mem_addr,
	// output inst_mem_wen,
	// output [31:0] inst_mem_wdata,
	// input [31:0] inst_mem_rdata

	
	// output [31:0] data_mem_addr,
	// output data_mem_wen,
	// output [31:0] data_mem_wdata,
	// input [31:0] data_mem_rdata
);
wire [31:0] pc;
assign cur_pc = pc;
// assign inst_mem_wdata = 32'b0;
// assign inst_mem_wen = 1'b0;
// assign inst_mem_addr = pc;
wire [31:0] inst;

wire zero, less;
wire [1:0] branch_intr;
wire [31:0] reg_rs1 = rf_rdata1;
ysyx_24070016_IFU u_ysyx_24070016_IFU(
  .clk         (clk         ),
  .rst         (rst         ),
  .branch_intr (branch_intr ),
  .sel_branch  (sel_branch  ),
  .zero        (zero        ),
  .less        (less        ),
  .reg_rs1     (reg_rs1     ),
  .dec_imm     (dec_imm     ),
  .csr_mepc    (csr_mepc    ),
  .csr_mtvec   (csr_mtvec   ),
  .pc          (pc          ),
  .sim_dnpc    (sim_dnpc    ),
  .inst        (inst        )
);

wire [4:0] dec_rs1;
wire [4:0] dec_rs2;
wire [4:0] dec_rd;
wire [31:0] dec_imm;
//control sign
wire ebreak, rf_wen, sel_alusrc1;
wire [1:0] sel_alusrc2;
wire [3:0] sel_aluop;
wire [2:0] sel_branch;
ysyx_24070016_IDU u_ysyx_24070016_IDU(
  .inst            (inst            ),
  .dec_rs1         (dec_rs1         ),
  .dec_rs2         (dec_rs2         ),
  .dec_rd          (dec_rd          ),
  .dec_imm         (dec_imm         ),
  .rf_wen          (rf_wen          ),
  .sel_alusrc1     (sel_alusrc1     ),
  .sel_alusrc2     (sel_alusrc2     ),
  .sel_aluop       (sel_aluop       ),
  .sel_branch      (sel_branch      ),
  .sel_memop       (sel_memop       ),
  .sel_wb_to_reg   (sel_wb_to_reg   ),
  .sel_csrt_is_rs1 (sel_csrt_is_rs1 ),
  .csr_op          (csr_op          ),
  .branch_intr     (branch_intr     ),
  .mem_valid       (mem_valid       ),
  .mem_wren        (mem_wren        ),
  .ebreak          (ebreak          )
);


wire [4:0] rf_raddr1 = dec_rs1;
wire [4:0] rf_raddr2 = dec_rs2;
wire [4:0] rf_waddr = dec_rd;
wire reg_write = rf_wen;
wire [31:0] rf_rdata1;
wire [31:0] rf_rdata2;
wire [31:0] rf_wdata;

wire [1:0] sel_wb_to_reg;
wire [31:0] wrback_result = ({32{sel_wb_to_reg == 2'b00}} & exu_result)
                          | ({32{sel_wb_to_reg == 2'b01}} & mem_rdata)
                          | ({32{sel_wb_to_reg == 2'b10}} & csr_rdata);
assign rf_wdata = wrback_result;
ysyx_24070016_RegisterFile #(
	.ADDR_WIDTH (5),
	.DATA_WIDTH (32)
	) u_ysyx_24070016_RegisterFile(
	.clk    (clk       ),
	.raddr1 (rf_raddr1 ),
	.raddr2 (rf_raddr2 ),
	.wdata  (rf_wdata  ),
	.waddr  (rf_waddr  ),
	.wen    (reg_write ),
	.rdata1 (rf_rdata1 ),
	.rdata2 (rf_rdata2 )
);

wire sel_csrt_is_rs1;
wire [1:0] csr_op;
wire [31:0] csr_rdata;
wire [31:0] csr_addr = dec_imm;
wire [31:0] csr_wdata = sel_csrt_is_rs1 ? rf_rdata1 : exu_result;
wire [31:0] csr_mtvec, csr_mepc;
ysyx_24070016_csr_reg u_ysyx_24070016_csr_reg(
  .clk       (clk       ),
  .rst       (rst       ),
  .csr_op    (csr_op    ),
  .pc        (pc        ),
  .csr_addr  (csr_addr  ),
  .csr_wdata (csr_wdata ),
  .csr_rdata (csr_rdata ),
  .csr_mtvec (csr_mtvec ),
  .csr_mepc  (csr_mepc  )
);


wire [31:0] exu_src1 = rf_rdata1;
wire [31:0] exu_src2 = rf_rdata2;
wire [31:0] exu_imm = dec_imm;
wire [31:0] exu_result;
ysyx_24070016_EXU u_ysyx_24070016_EXU(
  .pc            (pc            ),
  .exu_src1      (exu_src1      ),
  .exu_src2      (exu_src2      ),
  .exu_imm       (exu_imm       ),
  .sel_alusrc1   (sel_alusrc1   ),
  .sel_alusrc2   (sel_alusrc2   ),
  .sel_aluop     (sel_aluop     ),
  .ebreak        (ebreak        ),
  .exu_result    (exu_result    ),
  .zero          (zero          ),
  .less          (less          )
);

wire mem_valid, mem_wren;
wire [2:0] sel_memop;
wire [31:0] mem_rdata;
wire [31:0] mem_wdata = rf_rdata2;
wire [31:0] mem_addr = exu_result;
ysyx_24070016_Mem u_ysyx_24070016_Mem(
  .clk       (clk       ),
  .mem_valid (mem_valid ),
  .mem_wren  (mem_wren  ),
  .mem_op    (sel_memop ),
  .mem_addr  (mem_addr  ),
  .mem_wdata (mem_wdata ),
  .mem_rdata (mem_rdata )
);

endmodule
