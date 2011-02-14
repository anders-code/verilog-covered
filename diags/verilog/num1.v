/*
 Name:        num1.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        02/14/2011
 Purpose:     Verify that an unsigned decimal unknown value can
              be parsed and used properly.
*/

module main;

reg [15:0] a;
reg        b;

initial begin
	a = 'dx;
	b = 1'b0;
	#5;
	b = (a === 16'bx);
end

initial begin
`ifdef DUMP
        $dumpfile( "num1.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
