/*
 Name:        generate22.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        08/16/2009
 Purpose:     Verifies that two expressions in the two instances of the same module can be
              merged together without error.
*/

module main;

reg a, b;

foobar #(0) f0(  a, b );
foobar #(1) f1(  a, b );
foobar #(1) f2( ~a, b );

initial begin
`ifdef DUMP
        $dumpfile( "generate22.vcd" );
        $dumpvars( 0, main );
`endif
	a = 1'b0;
	b = 1'b1;
        #10;
        $finish;
end

endmodule

module foobar(
  input wire a,
  input wire b
);

parameter X = 0;

generate
  if( X == 0 ) begin
    assign c = a & b;
  end else begin
    assign c = a | b;
  end
endgenerate

endmodule
