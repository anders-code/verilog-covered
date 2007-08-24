/*
 Name:     concat7.v
 Author:   Trevor Williams  (trevorw@charter.net)
 Date:     04/18/2007
 Purpose:  Verifies that concatenation operators are sized correctly when
	   a function call exists in its list.
*/

module main;

reg [6:0] a;
reg       b;
reg [5:0] c;

initial begin
	a = 7'h0;
	#1;
	a = {b, foo( c )};
end

initial begin
`ifndef VPI
        $dumpfile( "concat7.vcd" );
        $dumpvars( 0, main );
`endif
	b = 1'b1;
	c = 6'h15;
        #10;
        $finish;
end

function [5:0] foo;
  input [5:0] a;
  begin
   foo = ~a;
  end
endfunction

endmodule
