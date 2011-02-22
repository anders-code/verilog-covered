/*
 Name:        func6.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        05/04/2009
 Purpose:     Verify that a function call made within an always block works properly.
*/

module main;

reg [1:0] a, c;
reg [3:0] d;
reg       b, e;

always @(a, b) begin
  c    = a;
  d[a] = b;
  e    = foo( a );
end

initial begin
`ifdef DUMP
        $dumpfile( "func6.vcd" );
        $dumpvars( 0, main );
`endif
	a = 2'b0;
	b = 1'b0;
	#5;
	b = 1'b1;
        #10;
        $finish;
end

function foo;
  input [1:0] a;
  begin
    case( a )
      2'b00,
      2'b10 : foo = 1'b0;
      2'b01,
      2'b11 : foo = 1'b1;
    endcase
  end
endfunction

endmodule
