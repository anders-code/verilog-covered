/*
 Name:        generate22.3.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        08/16/2009
 Purpose:     Verifies that two FSMs in the two instances of the same module can be
              merged together without error.
*/

module main;

reg a;

foobar #(0) f0(  a );
foobar #(1) f1(  a );
foobar #(1) f2( ~a );

initial begin
`ifdef DUMP
        $dumpfile( "generate22.3.vcd" );
        $dumpvars( 0, main );
`endif
	a = 1'b0;
        #10;
        $finish;
end

endmodule

module foobar(
  input wire a
);

parameter X = 0;

reg b;

generate
  if( X == 0 ) begin
    (* covered_fsm, foobar, is="a", os="b" *)
    always @(a)
      case( a )
        1'b0 :  b = 1'b0;
        1'b1 :  b = 1'b1;
      endcase
  end else begin
    (* covered_fsm, barfoo, is="a", os="b" *)
    always @(a)
      case( a )
        1'b0 :  b = 1'b1;
        1'b1 :  b = 1'b0;
      endcase
  end
endgenerate

endmodule
