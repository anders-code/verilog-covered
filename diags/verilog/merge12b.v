/*
 Name:        merge12b.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        02/22/2010
 Purpose:     
*/

module main;

reg  [1:0] a;
wire [2:0] b;

dut #(3) d( a, b );

initial begin
`ifdef DUMP
        $dumpfile( "merge12b.vcd" );
        $dumpvars( 0, main );
`endif
	a = 2'b00;
	#5;
	a = 2'b01;
        #100;
        $finish;
end

endmodule

//-------------------------------------------

module dut #(parameter SIZE=1) (
  input  wire [1:0]        a,
  output wire [(SIZE-1):0] b
);

logic_mod #(SIZE) l( a, b );

endmodule

//-------------------------------------------

module logic_mod #(parameter SIZE=1) (
  input  wire [1:0]        a,
  output wire [(SIZE-1):0] b
);

case_mod #(SIZE) c( a, b );

endmodule
