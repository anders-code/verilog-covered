/*
 Name:        param18.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        10/13/2010
 Purpose:     Verifies that parameter integer and parameter real are parsed
              and handled properly.
*/

module main;

parameter integer a = 15;
parameter real    b = 123.456;

reg [1:0] c;

initial begin
	c = 2'b0;
	#5;
	if( a == 15 )
          c[0] = 1'b1;
        if( (b > 123) && (b < 124) )
          c[1] = 1'b1;
end

initial begin
`ifdef DUMP
        $dumpfile( "param18.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
