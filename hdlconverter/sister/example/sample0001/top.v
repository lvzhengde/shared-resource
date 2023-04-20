//
//this is a test bench module
//

module top();
    //parameters
    parameter DATA_SIZE=1024;
    
    //input
    reg clk,rst;
    reg w;
    reg[7:0] in;
    
    //output
    wire r;
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
        .w(w), .r(r), .in(in), .out(out));

    //to start simulation
    initial begin
    $readmemh("in.data",ram);
    clk=0;
    rst=0;
    u0.r=0;
    u0.out=0;
    for(i=0;i<4;i=i+1) u0.bf[i]=0;
    #2
    
    w=1;
    rst=1;
    for(i=0;i<DATA_SIZE;i=i+1) begin
        while(r) #2;
        $display("0%h",out);
        in=ram[i];
        #2;
    end
    
    $finish;
    end

endmodule

