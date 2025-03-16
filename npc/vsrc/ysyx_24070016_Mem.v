module ysyx_24070016_Mem(
  input clk,
  input mem_valid,
  input mem_wren,
  input [2:0] mem_op,
  input [31:0] mem_addr,
  input [31:0] mem_wdata,

  output [31:0] mem_rdata
);

wire mem_op_1_s = (mem_op == 3'b000);
wire mem_op_2_s = (mem_op == 3'b001);
wire mem_op_1_u = (mem_op == 3'b100);
wire mem_op_2_u = (mem_op == 3'b101);
wire mem_op_4_s = (mem_op == 3'b010);

wire [7:0] mask;
assign mask[7:3] = 5'b0;
assign mask[0] = mem_op_1_s | mem_op_1_u;
assign mask[1] = mem_op_2_s | mem_op_2_u;
assign mask[2] = mem_op_4_s;

reg [31:0] rdata;
import "DPI-C" function int npc_datamem_read(input int raddr, input byte rmask);
import "DPI-C" function void npc_datapmem_write(input int waddr, input int wdata, input byte wmask);
always @(posedge clk) begin
  if (mem_valid) begin // 有读写请求时
    if (!mem_wren) begin // 有写请求时
      rdata = npc_datamem_read(mem_addr, mask);
    end else begin
      npc_datapmem_write(mem_addr, mem_wdata, mask);
    end 
  end else begin
    rdata = 0;
  end
end

wire [31:0] sign_ext_rdata = ({32{mem_op_1_s}} & {{(32 - 1 * 8){rdata[7]}}, rdata[7:0]})
                           | ({32{mem_op_2_s}} & {{(32 - 2 * 8){rdata[15]}}, rdata[15:0]})
                           | ({32{mem_op_4_s}} & rdata);

wire [31:0] zero_ext_rdata = ({32{mem_op_1_u}} & {{(32 - 1 * 8){1'b0}}, rdata[7:0]})
                           | ({32{mem_op_2_u}} & {{(32 - 2 * 8){1'b0}}, rdata[15:0]});

wire zero_ext = mem_op_1_u | mem_op_2_u;
assign mem_rdata = zero_ext ? zero_ext_rdata : sign_ext_rdata;

//device addr to c
import "DPI-C" function void detecte_device_addr(input int addr);
always @(*) begin
	if (mem_valid) detecte_device_addr(mem_addr);
end

endmodule
