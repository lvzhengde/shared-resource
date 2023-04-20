/*
    Created by Sister.
*/
module sub1(
    in,
    clk);

    reg[7:0] x;
    input[7:0] in;
    input clk;

    always@(posedge clk) begin
        x<=in;
    end

endmodule

module sub2(
    out,
    clk);

    output[7:0] out;
    reg[7:0] out;
    input clk;

    always@(posedge clk) begin
        out<=1;
    end

endmodule

module sample(
    rst,
    clk);

    wire[7:0] sig;
    input rst;
    input clk;

    sub2 u1(
        .clk(clk),
        .out(sig));
    sub1 u0(
        .clk(clk),
        .in(sig));

    always@(posedge clk or negedge rst) begin
    end

endmodule

