//
//this is a test bench module
//

module top();
    //parameters
    parameter DATA_SIZE=128*128*3;
    
    //input
    reg clk,rst;
    reg we;
    reg[7:0] r,g,b;
    
    //output
    wire re;
    wire[7:0] out;
    
    //ram
    reg[7:0] ram[0:DATA_SIZE-1];
    integer i;

    //clock creation
    always #1 begin
    clk<=~clk;
    end
    
    //module
    sample u0(.clk(clk),.rst(rst), 
        .we(we), .re(re), .r(r), .g(g), .b(b), .out(out));

    //to start simulation
    initial begin
    $readmemh("in.hex",ram);
    clk=0;
    rst=0;
    #2
    we=1;
    rst=1;

    for(i=0;i<DATA_SIZE;i=i+1) begin
        r=ram[i];
        i=i+1;
        g=ram[i];
        i=i+1;
        b=ram[i];
        #2;
        while(re) #2;
        $display("0%h",out);
        $display("0%h",out);
        $display("0%h",out);
    end
    
    $finish;
    end

endmodule

