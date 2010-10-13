/*
 Name:        real9.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        10/13/2010
 Purpose:     Verify that constant assignments to reals works properly.
*/

module main;

real a = 123.456;
reg  b;

initial begin
	b = 1'b0;
	#5;
	if( a > 100 )
	  b = 1'b1;
end

initial begin
`ifdef DUMP
        $dumpfile( "real9.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
