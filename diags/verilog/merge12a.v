/*
 Name:        merge12a.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        02/22/2010
 Purpose:     
*/

module main;

reg  [1:0] x;
wire [1:0] y;

case_mod #(2) a( x, y );

initial begin
`ifdef DUMP
        $dumpfile( "merge12a.vcd" );
        $dumpvars( 0, main );
`endif
	x = 2'b00;
	#5;
	x = 2'b10;
        #10;
        $finish;
end

endmodule
