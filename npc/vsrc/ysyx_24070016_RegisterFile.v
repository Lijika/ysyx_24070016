module ysyx_24070016_RegisterFile #(ADDR_WIDTH = 5, DATA_WIDTH = 32) (
	input clk,
	input [ADDR_WIDTH-1:0] raddr1,
	input [ADDR_WIDTH-1:0] raddr2,
	input [DATA_WIDTH-1:0] wdata,
	input [ADDR_WIDTH-1:0] waddr,
	input wen,

	output [DATA_WIDTH-1:0] rdata1, 
	output [DATA_WIDTH-1:0] rdata2
);

reg [DATA_WIDTH-1:0] rf [2**ADDR_WIDTH-1:0];
always @(posedge clk) begin
	if (wen && (waddr != 0) && (waddr < 16)) rf[waddr] <= wdata;
end

import "DPI-C" function void set_gpr_ptr(input logic [DATA_WIDTH-1:0] r[]);
initial begin
  set_gpr_ptr(rf);
end

assign rdata1 = (raddr1 < 16) ? rf[raddr1] : 32'b0;
assign rdata2 = (raddr2 < 16) ? rf[raddr2] : 32'b0;

endmodule
