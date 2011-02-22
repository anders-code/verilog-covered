module main;
  localparam f_wid = 6;
  localparam a_wid = $clog2(f_wid);
  reg pass;
  reg load;
  reg [a_wid-1:0] addr;
  wire [f_wid-1:0] out;
  integer lp;

  initial begin
    $dumpfile("test.vcd");
    $dumpvars(0, main);
    pass = 1'b1;

    // Run the address space, but keep load low.
    load = 1'b0;
    for (lp=0; lp<f_wid; lp=lp+1) begin
      addr = lp;
      #1 if (out !== 'b0) begin
        $display("Failed to block address %d, got %b", addr, out);
        pass = 1'b0;
      end
    end

    // Run the address space, but pass the value.
    load = 1'b1;
    for (lp=0; lp<f_wid; lp=lp+1) begin
      addr = lp;
      #1 if (out !== 1'b1<<addr) begin
        $display("Failed to pass address %d, got %b", addr, out);
        pass = 1'b0;
      end
    end

    load = 1'b0;
    addr = 'd0;
    #1 if (out !== 'b0) begin
      $display("Failed to block address 0, got %b", out);
      pass = 1'b0;
    end
    if (pass) $display("PASSED");
  end

  example dut(out, addr, load);
endmodule

module example
  #(parameter f_wid = 1,
    parameter a_wid = $clog2(f_wid))
   (output wire [f_wid-1:0] out,
    input wire [a_wid:0] addr,
    input wire load);

  assign #0.1 out = {f_wid{load}} & (2**addr);
endmodule
