/*
 Name:        mem7.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        03/26/2010
 Purpose:     Creates a memory which is much larger in width than the machine long size.
              Exercises vector code that is not covered otherwise.
*/

module main;

reg [127:0] mem[0:15];
reg [127:0] a;

initial begin
	a = 128'h0;
	mem[1] = 128'h0;
	mem[2] = 128'h0123456789abcdef_fedcba9876543210;
	#5;
	a = mem[2];
end

initial begin
`ifdef DUMP
        $dumpfile( "mem7.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
