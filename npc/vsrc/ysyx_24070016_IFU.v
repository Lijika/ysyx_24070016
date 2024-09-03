module ysyx_24070016_IFU (
	input [31:0] pc,
	output [31:0] inst
);

import "DPI-C" function uint32_t pmem_read_if (input uint32_t pc);

assign inst = pmem_read_if(pc);

endmodule
