/*
 Name:        generate22.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        08/16/2009
 Purpose:     Verifies that two expressions in the two instances of the same module can be
              merged together without error.
*/

module main;

reg a;

foobar #(0) f0( a );
foobar #(1) f1( a );

initial begin
`ifdef DUMP
        $dumpfile( "generate22.vcd" );
        $dumpvars( 0, main );
`endif
	a = 1'b0;
	#5;
	a = 1'b1;
        #10;
        $finish;
end

endmodule

module foobar(
  input wire a
);

parameter X = 0;

generate
  if( X == 0 ) begin
    assign b = 1'b0;
  end else begin
    assign b = a;
  end
endgenerate

endmodule
