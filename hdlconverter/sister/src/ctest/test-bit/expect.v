/*
    Created by Sister.
*/
module sample(
    out,
    in,
    clk,
    rst);

    reg proc_state_000000;
    output[9:0] out;
    reg[9:0] out;
    input[23:0] in;
    input clk;
    input rst;
    reg[8:0] __tmp_000000;

    always@(posedge clk or negedge rst) begin
        case(proc_state_000000)
        0 : begin
            __tmp_000000<=(in[23:16]+in[15:8]);
            proc_state_000000<=1;
        end
        1 : begin
            out<=(__tmp_000000+in[7:0]);
            proc_state_000000<=0;
        end
        endcase
    end

endmodule

