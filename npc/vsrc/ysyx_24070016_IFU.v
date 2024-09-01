module ysyx_24070016_IFU(
	input [31:0] pc,
	input [31:0] inst_mem_rdata,
	output [31:0] inst,
	output [31:0] inst_mem_addr,
	output [31:0] inst_mem_wdata,
	output inst_mem_wen
);

assign inst = inst_mem_rdata;
assign inst_mem_addr = pc;
assign inst_mem_wdata = 32'h0;
assign inst_mem_wen = 1'b0;

endmodule 
