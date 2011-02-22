/*
 Name:        slist6.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        05/12/2009
 Purpose:     
*/

module main;

reg [1:0] a;
reg [1:0] b[0:3];
reg       c, d, e, f;

wire x = d;
wire y = d;

always @* begin
  if( a[0] )
    c = 1'b1;
  else
    c = 1'b0;
end

always @* begin
  a = 2'b00;
  if( d & f ) begin
    a = b[0];
    if( c ) begin
      e = 1'b1;
    end
  end
end

initial begin
	b[0] = 2'b00;
	#1938;
	b[0] = 2'b01;
end

initial begin
`ifdef DUMP
        $dumpfile( "slist6.vcd" );
        $dumpvars( 0, main );
`endif
	d = 1'b0;
        f = 1'b0;
	fork
	  begin
	    #1934;
            d = 1'b1;
	    #6;       // 1940
            d = 1'b0;
	    #666;     // 2606
            d = 1'b1;
	    #6;       // 2612
            d = 1'b0;
	    #666;     // 3278
	    d = 1'b1;
            #6;       // 3284
            d = 1'b0;
	    #666;     // 3950
            d = 1'b1;
            #6;       // 3956
            d = 1'b0;
          end
          begin
            #1938;
            f = 1'b1;
            #4;       // 1942
            f = 1'b0;
            #68;      // 2610
            f = 1'b1;
            #4;       // 2614
            f = 1'b0;
            #668;     // 3282
            f = 1'b1;
            #4;       // 3286
            f = 1'b0;
            #668;     // 3954
            f = 1'b1;
            #4;       // 3958
            f = 1'b0;
          end
	join
        #10;
        $finish;
end

endmodule
