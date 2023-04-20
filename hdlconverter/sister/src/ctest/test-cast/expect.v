/*
    Created by Sister.
*/
module sample(
    out,
    in,
    clk,
    rst);

    reg[63:0] i[0:2];
    output[7:0] out;
    reg[7:0] out;
    input[7:0] in;
    input clk;
    input rst;

    always@(posedge clk or negedge rst) begin
        i[in]<=10;
        out<=in;
    end

endmodule

