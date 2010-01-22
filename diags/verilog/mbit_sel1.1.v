/*
 Name:        mbit_sel1.1.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        01/21/2010
 Purpose:     Verifies interesting RHS multi-bit scenarios.
*/

module main;

reg [135:64] y;
reg [71:0]   z;
reg [15:0]   a, b;
reg [64:0]   c, d;

initial begin
  y = 72'h10__0123_4567_89ab_cdef;
  z = 72'hef__fedc_ba98_7654_3210;
  a = 16'h0;
  b = 16'h0;
  c = 64'h0;
  d = 64'h0;
  #5;
  a = z[80:65];
  b = y[70:55];
  c = z[80:16];
  d = y[80:16];
end

initial begin
`ifdef DUMP
        $dumpfile( "mbit_sel1.1.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
