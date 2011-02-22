/*
 Name:        negate3.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        03/26/2010
 Purpose:     Performs a negate operation on a vector that is much larger than the machine
              long width.
*/

module main;

reg signed [127:0] a;

initial begin
	a = 128'h0;
	#5;
	a = -a;
end

initial begin
`ifdef DUMP
        $dumpfile( "negate3.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
