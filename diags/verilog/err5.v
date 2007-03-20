/*
 Name:     err5.v
 Author:   Trevor Williams  (trevorw@charter.net)
 Date:     03/12/2007
 Purpose:  Verifies that specifying a directory as an output file causes an appropriate
           error message.
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
        $dumpfile( "err5.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
