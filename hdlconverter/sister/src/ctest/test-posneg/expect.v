/*
    Created by Sister.
*/
module sample(
    out,
    in,
    clk,
    rst);

    output[7:0] out;
    reg[7:0] out;
    input[7:0] in;
    input clk;
    input rst;

    always@(posedge clk or negedge rst) begin
        out<=in;
    end

endmodule

