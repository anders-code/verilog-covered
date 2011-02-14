/*
 Name:        task7.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        02/13/2011
 Purpose:     
*/

module main;

reg       x;
reg [7:0] y;
reg [3:0] z;

initial begin
	x = 1'b0;
	y = 8'h10;
	foobar( x, y, z );
	#5;
	x = 1'b1;
	foobar( x, y, z );
end

task foobar (
  input       a,
  input [7:0] b,
  output[3:0] c
);
  begin
    c = a ? b[7:4] : b[3:0];
  end
endtask

initial begin
`ifdef DUMP
        $dumpfile( "task7.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
