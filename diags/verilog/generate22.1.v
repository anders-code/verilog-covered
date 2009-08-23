/*
 Name:        generate22.1.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        08/22/2009
 Purpose:     Verifies that two signals in the two instances of the same module can be
              merged together without error.
*/

module main;

reg a;

foobar #(0) f0(  a );
foobar #(1) f1(  a );
foobar #(1) f2( ~a );

initial begin
`ifdef DUMP
        $dumpfile( "generate22.1.vcd" );
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
    wire c = a;
  end else begin
    wire d = a;
  end
endgenerate

endmodule
