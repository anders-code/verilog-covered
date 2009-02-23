/*
 Name:        version2.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        02/23/2009
 Purpose:     Verifies that if version information is specified and modules are excluded that
              no error occurs (bug 2617354).
 $Id$
*/

module main;

foo f( 1'b0 );

initial begin
`ifdef DUMP
        $dumpfile( "version2.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

endmodule
