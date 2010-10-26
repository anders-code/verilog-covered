/*
 Name:        realtime1.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        10/26/2010
 Purpose:     Verifies the $realtime expression.
*/

`timescale 1 ns / 1 ns

module main;

reg a;

initial begin
	a = 1'b0;
	#(0.499999999);
	if( $realtime > 0.4 )
	  a = 1'b1;
end

initial begin
`ifdef DUMP
        $dumpfile( "realtime1.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
