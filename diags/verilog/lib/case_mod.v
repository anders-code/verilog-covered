module case_mod
  #(parameter SIZE=1) (
  input  wire [1:0]        a,
  output reg  [(SIZE-1):0] b
);

always @*
  case( a )
    2'b00 :  b = {SIZE{1'b1}};
    2'b01 :  b = {SIZE{1'b0}};
    2'b10 :  b = {SIZE{1'b0}};
    2'b11 :  b = {SIZE{1'b1}};
  endcase

endmodule
