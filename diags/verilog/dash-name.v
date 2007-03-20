/*
 Name:     dash-name.v
 Author:   Trevor Williams  (trevorw@charter.net)
 Date:     03/12/2007
 Purpose:  Verifies that Covered can handle names with a '-' in them.
*/

module main;

reg a;

initial begin
	a = 1'b0;
	#5;
	a = 1'b1;
end

initial begin
`ifndef VPI
        $dumpfile( "dash-name.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
