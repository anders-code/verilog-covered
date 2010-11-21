/*
 Name:        mbit_sel10.1.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        10/13/2010
 Purpose:     Verifies LHS assignments to variable multi-bit part selects
              work properly.
*/

module main;

integer   a;
reg [3:0] b;

initial begin
	a = 5;
	b = 4'h0;
	#5;
	b[a-:2] = 2'b01;
	a = 4;
	#5;
	b[a-:2] = 2'b01;
	a = 3;
	#5;
	b[a-:2] = 2'b01;
	a = 2;
	#5;
	b[a-:2] = 2'b01;
	a = 1;
	#5;
	b[a-:2] = 2'b01;
	a = 0;
	#5;
	b[a-:2] = 2'b01;
end

initial begin
`ifdef DUMP
        $dumpfile( "mbit_sel10.1.vcd" );
        $dumpvars( 0, main );
`endif
        #50;
        $finish;
end

endmodule
