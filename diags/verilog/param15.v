module main;

parameter foo = 40'hffffffffff;
parameter bar = 2'h2;

reg [39:0] a;

initial begin
	a = foo;
	#5;
	a = bar;
end

initial begin
        $dumpfile( "param15.vcd" );
        $dumpvars( 0, main );
        #10;
        $finish;
end

endmodule
