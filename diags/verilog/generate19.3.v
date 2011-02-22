/*
 Name:        generate19.3.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        06/18/2009
 Purpose:     
*/

module main;

initial begin
`ifdef DUMP
        $dumpfile( "generate19.3.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule

//---------------------------------------------------

module foobar(
  input  wire clock,
  input  wire a,
  output reg b
);

endmodule
