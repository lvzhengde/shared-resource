/*
    Created by Sister.
*/
module test_function(
    out,
    in);

    reg[2:0] proc_state_000000;
    reg[63:0] a__000002;
    reg[63:0] b__000002;
    reg[63:0] a__000001;
    reg[63:0] b__000001;
    reg[63:0] a;
    reg[63:0] b;
    reg[63:0] o;
    reg[63:0] m;
    reg[63:0] n;
    reg[63:0] y;
    reg[63:0] x;
    output[7:0] out;
    reg[7:0] out;
    input[7:0] in;
    reg[64:0] __tmp_000000;

    always@( in) begin
        case(proc_state_000000)
        0 : begin
            y<=20;
            x<=10;
            proc_state_000000<=1;
        end
        1 : begin
            a__000002<=x;
            b__000002<=y;
            a__000001<=x;
            b__000001<=y;
            a<=x;
            b<=y;
            proc_state_000000<=2;
        end
        2 : begin
            o<=(a__000002*b__000002);
            n<=(a__000001-b__000001);
            m<=(a+b);
            proc_state_000000<=3;
        end
        3 : begin
            __tmp_000000<=(n+m);
            proc_state_000000<=4;
        end
        4 : begin
            out<=(__tmp_000000+o);
            proc_state_000000<=0;
        end
        endcase
    end

endmodule

