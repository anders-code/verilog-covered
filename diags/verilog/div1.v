/*
 Name:        div1.v
 Author:      Trevor Williams  (phase1geo@gmail.com)
 Date:        05/24/2008
 Purpose:     Verify that a divide by zero error is reported when the situation
              arises during simulation.
 Simulators:  IV CVER VERIWELL VCS
 Modes:       VCD LXT VPI
*/

module main;

integer a, b;

initial begin
	a = 0;
	b = 32 / a;
	#5;
	$finish;
end

endmodule
