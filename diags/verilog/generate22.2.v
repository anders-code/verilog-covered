/*
 Name:        generate22.2.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        08/16/2009
 Purpose:     Verifies that two race conditions in the two instances of the same module can be
              merged together without error.
*/

module main;

reg clock;
reg a;

foobar #(0) f0( clock,  a );
foobar #(1) f1( clock,  a );
foobar #(0) f2( clock, ~a );

initial begin
`ifdef DUMP
        $dumpfile( "generate22.2.vcd" );
        $dumpvars( 0, main );
`endif
	a = 1'b0;
        #10;
        $finish;
end

initial begin
	clock = 1'b0;
	forever #(2) clock = ~clock;
end

endmodule

module foobar(
  input wire clock,
  input wire a
);

parameter X = 0;

reg b;

generate
  if( X == 0 ) begin
    always @(posedge clock) b =  a;
  end else begin
    always @(posedge clock) b = ~a;
  end
endgenerate

endmodule
