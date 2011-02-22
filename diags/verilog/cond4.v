/*
 Name:        cond4.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        06/27/2009
 Purpose:     Verifies that conditional subexpressions are evaluated correctly for coverage.
*/

module main;

reg  [3:0] y, z;
reg  [1:0] b, c, d;
wire [1:0] a;

assign a = (y < z) ? b : (y > z) ? c : d;

initial begin
`ifdef DUMP
        $dumpfile( "cond4.vcd" );
        $dumpvars( 0, main );
`endif
	b = 2'h0;
	c = 2'h1;
	d = 2'h2;
	y = 4'h3;
	z = 4'h5;
	#5;
	y = 4'h5;
        #10;
        $finish;
end

endmodule
