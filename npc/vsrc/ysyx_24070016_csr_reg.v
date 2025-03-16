module ysyx_24070016_csr_reg(
  input clk, rst,
  input [1:0] csr_op,
  input [31:0] pc,
  input [31:0] csr_addr,
  input [31:0] csr_wdata,

  output [31:0] csr_rdata,
  output [31:0] csr_mtvec,
  output [31:0] csr_mepc
);

/*csr_op == 01  r/w
  csr_op == 10  ecall*/
wire is_mstatus = (csr_addr == 32'h00000300);
wire is_mtvec   = (csr_addr == 32'h00000305);
wire is_mepc    = (csr_addr == 32'h00000341);
wire is_mcause  = (csr_addr == 32'h00000342);

wire mstatus_wen = (csr_op[0] & is_mstatus);
wire mtvec_wen   = (csr_op[0] & is_mtvec);
wire mepc_wen    = (csr_op[0] & is_mepc)
                 | (csr_op[1]);
wire mcause_wen  = (csr_op[0] & is_mcause)
                 | (csr_op[1]);

//input
wire [31:0] din         = csr_wdata;
wire [31:0] mepc_din    = csr_op[1] ? pc            : din;
wire [31:0] mcause_din  = csr_op[1] ? 32'h0000000b  : din;


//output
reg [31:0] mstatus_dout;
reg [31:0] mtvec_dout;
reg [31:0] mepc_dout;
reg [31:0] mcause_dout;

assign csr_rdata = ({32{is_mstatus}} & mstatus_dout)
                 | ({32{is_mtvec}}   & mtvec_dout)
                 | ({32{is_mepc}}    & mepc_dout)
                 | ({32{is_mcause}}  & mcause_dout);

assign csr_mepc = mepc_dout;
assign csr_mtvec = mtvec_dout;

//dpic
import "DPI-C" function void set_csr_ptr(input logic [31:0] mstatus[], 
                                         input logic [31:0] mtvec[], 
                                         input logic [31:0] mepc[],
                                         input logic [31:0] mcause[]);
initial begin
  set_csr_ptr(mstatus_dout, mtvec_dout, mtvec_dout, mcause_dout);
end

/* mstatus */
ysyx_24070016_Reg #(32, 32'h00000180)u_ysyx_24070016_mstatus(
  .clk  (clk          ),
  .rst  (rst          ),
  .din  (din          ),
  .dout (mstatus_dout ),
  .wen  (mstatus_wen  )
);

/* mtvec */
ysyx_24070016_Reg #(32, 32'h00000000)u_ysyx_24070016_mtvec(
  .clk  (clk        ),
  .rst  (rst        ),
  .din  (din        ),
  .dout (mtvec_dout ),
  .wen  (mtvec_wen  )
);

/* mepc */
ysyx_24070016_Reg #(32, 32'h00000000)u_ysyx_24070016_mepc(
  .clk  (clk        ),
  .rst  (rst        ),
  .din  (mepc_din   ),
  .dout (mepc_dout  ),
  .wen  (mepc_wen   )
);

/* mcause */
ysyx_24070016_Reg #(32, 32'h00000000)u_ysyx_24070016_mcaus(
  .clk  (clk         ),
  .rst  (rst         ),
  .din  (mcause_din  ),
  .dout (mcause_dout ),
  .wen  (mcause_wen  )
);

endmodule
