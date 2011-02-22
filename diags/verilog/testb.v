/*
 Name:        testb.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        04/26/2009
 Purpose:     
*/

module main;

reg a;

psys dut( a );

initial begin
`ifdef DUMP
        $dumpfile( "testb.vcd" );
        $dumpvars( 0, main );
`endif
	a = 1'b1;
        #10;
        $finish;
end

endmodule


module psys(
  input a
);

wire c = ~a;

endmodule
