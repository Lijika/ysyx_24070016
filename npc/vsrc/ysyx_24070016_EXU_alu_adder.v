module ysyx_24070016_EXU_alu_adder(
  input [31:0] adder_a,
  input [31:0] adder_b,
  input Cin,

  output [31:0] adder_result,
  output adder_carry, adder_overflow
);
/* verilator lint_off WIDTHEXPAND */
wire [31:0] t_no_Cin = {32{ Cin }}^adder_b;
assign {adder_carry,adder_result} = adder_a + t_no_Cin + Cin;
assign adder_overflow = (adder_a[31] == t_no_Cin[31]) && (adder_result [31] != adder_a[31]);
/* verilator lint_off WIDTHEXPAND */
endmodule
