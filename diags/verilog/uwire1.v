/*
 Name:        uwire1.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        07/19/2010
 Purpose:     Verifies that uwire types are parsed properly.
*/

module main;

uwire a;
reg   b;

assign a = b;

initial begin
`ifdef DUMP
        $dumpfile( "uwire1.vcd" );
        $dumpvars( 0, main );
`endif
	b = 1'b0;
	#5;
	b = 1'b1;
        #10;
        $finish;
end

endmodule
