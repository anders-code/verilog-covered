/*
 Name:        generate18.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        05/20/2009
 Purpose:     
*/

module main;

reg clock;
reg b;

generate
  genvar i, j;
  for( i=0; i<4; i++ ) begin : vc
    for( j=0; j<12; j++ ) begin : dir
      reg  a;
      wire c;
      always @(posedge clock)
        casex( {b, c} )
          2'b01  :  a <= 1'b1;
          2'b10  :  a <= 1'b0;
          default:  a <= a;
        endcase
      assign c = ~a;
    end
  end
endgenerate

initial begin
`ifdef DUMP
        $dumpfile( "generate18.vcd" );
        $dumpvars( 0, main );
`endif
	b = 1'b0;
        #10;
        $finish;
end

initial begin
	clock = 1'b0;
	forever #(2) clock = ~clock;
end

endmodule
