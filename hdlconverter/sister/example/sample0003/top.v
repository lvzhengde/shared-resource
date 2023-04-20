//
//this is a test bench module
//

module top();
    //parameters
    parameter RAM_SIZE=256;
    
    //input
    reg clk,rst;
    
    //ram
    reg[7:0] iram[0:RAM_SIZE-1];
    reg[7:0] dram[0:RAM_SIZE-1];
    integer i;

    //clock creation
    always #1 clk<=~clk;
    
    //module
    ppu u0(.clk(clk),.rst(rst));

    //to start simulation
    initial begin
    $readmemh("sample.o",u0.IRAM_0.ram);

    for(i=0;i<RAM_SIZE;i=i+1) u0.DRAM_0.ram[i]=0;
    clk=0;
    rst=0;
    #2
    rst=1;
    #2048
    
    for(i=0;i<10;i=i+1)
        $display("0%x",u0.DRAM_0.ram[i]);
    $finish;
    end

endmodule

