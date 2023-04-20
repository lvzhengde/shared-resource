module sample(clk,rst,in,out) ;
    input clk,rst,in;
    output out;
    reg out;
    
    always@(posedge clk or negedge rst) begin
        if(!rst) out<=0;
        else out<=in;
    end

endmodule

