/*
 Name:     task6.v
 Author:   Trevor Williams  (trevorw@charter.net)
 Date:     12/18/2006
 Purpose:  Calls a task recursively without re-entrant behavior.
*/

module main;

reg bar;

initial begin
	bar = 1'b1;
	#5;
	foo( 1'b1 );
end

initial begin
`ifndef VPI
        $dumpfile( "task6.vcd" );
        $dumpvars( 0, main );
`endif
        #10;
        $finish;
end

task foo;
  input a;
  begin
   if( a )
     foo( ~a );
   bar = a;
  end
endtask

endmodule
