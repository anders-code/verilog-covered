/*
 Name:     exclude7.v
 Author:   Trevor Williams  (trevorw@charter.net)
 Date:     08/22/2007
 Purpose:  Verifies that a functional unit with embedded scope
           can be safely excluded from coverage.
*/

module main;

reg a, b;

initial begin
	a = 1'b0;
	#5;
	a = foo_func( 1'b0 );
end

initial begin
	b = 1'b1;
	#5;
	b = 1'b0;
end

initial begin
`ifndef VPI
        $dumpfile( "exclude7.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

function foo_func;
  input a;
  begin : foo_nb
    foo_func = ~a;
  end
endfunction

endmodule
