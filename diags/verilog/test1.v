module main;

reg  b, c, d;

wire a = b & c & d;
wire e = 1'b0 & b;

initial begin
	$dumpfile( "test1.vcd" );
	$dumpvars( 0, main );
	b = 1'b0;
	c = 1'b0;
	d = 1'b0;
	#10;
	b = 1'b1;
	c = 1'b1;
	d = 1'b1;
	#10;
	$finish;
end

endmodule
