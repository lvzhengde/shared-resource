/*
    Created by Sister.
*/
module test_enum(
    out,
    in);

    reg proc_state_000000;
    reg[63:0] x;
    reg v0;
    reg[5:0] b;
    reg[5:0] a;
    output out;
    reg out;
    input in;

    always@( in) begin
        case(proc_state_000000)
        0 : begin
            x<=10;
            b<=41;
            a<=30;
            v0<=1;
            proc_state_000000<=1;
        end
        1 : begin
            out<=a;
            proc_state_000000<=0;
        end
        endcase
    end

endmodule

