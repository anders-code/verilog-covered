module main;

reg     a;
integer i;

initial begin
        i = 0;
 	a = 1'b0;
	#5;
	if( i++ == 0 ) a = 1'b1;
end

initial begin
`ifdef DUMP
        $dumpfile( "inc3.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
