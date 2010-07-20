/*
 Name:        wirereal1.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        07/20/2010
 Purpose:     
*/

module main;

wire real a;
reg       b;

assign a = b ? 0.1 : 0.2;

initial begin
`ifdef DUMP
        $dumpfile( "wirereal1.vcd" );
        $dumpvars( 0, main );
`endif
        b = 1'b0;
        #5;
        $finish;
end

endmodule
