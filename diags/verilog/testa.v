/*
 Name:        testa.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        04/26/2009
 Purpose:     
*/

module main;

reg a;

ldut dut( a );

initial begin
`ifdef DUMP
        $dumpfile( "testa.vcd" );
        $dumpvars( 0, main );
`endif
	a = 1'b0;
        #10;
        $finish;
end

endmodule


module ldut(
  input a
);

wire b = ~a;

psys p( b );

endmodule


module psys(
  input a
);

wire c = ~a;

endmodule
