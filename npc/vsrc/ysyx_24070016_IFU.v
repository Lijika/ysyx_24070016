module ysyx_24070016_IFU (
	input [31:0] pc,
	output [31:0] inst
);

import "DPI-C" function int pmem_read_if(input int pc);

assign inst = pmem_read_if(pc);

endmodule
