/*
    Created by Sister.
*/
module test_function(
    out,
    in);

    reg[2:0] proc_state_000000;
    reg[63:0] b__000009;
    reg[63:0] b__000008;
    reg[63:0] b__000007;
    reg[63:0] b__000006;
    reg[63:0] b__000005;
    reg[63:0] b__000004;
    reg[63:0] b__000003;
    reg[63:0] b__000002;
    reg[63:0] b__000001;
    reg[63:0] b;
    reg[63:0] w[0:1];
    reg[63:0] v[0:1];
    reg[63:0] z;
    reg[63:0] y;
    reg[63:0] x;
    output[7:0] out;
    reg[7:0] out;
    input[7:0] in;

    always@( in) begin
        case(proc_state_000000)
        0 : begin
            w[0][63:0]<=5;
            v[0]<=4;
            z[63:0]<=3;
            y<=2;
            x<=1;
            proc_state_000000<=1;
        end
        1 : begin
            b__000009<=y;
            b__000008<=y;
            b__000007<=y;
            b__000006<=y;
            b__000005<=y;
            b__000004<=y;
            b__000003<=y;
            b__000002<=y;
            b__000001<=y;
            b<=y;
            proc_state_000000<=2;
        end
        2 : begin
            w[0][63:0]<=(w[0][63:0]+b__000008);
            v[0]<=(v[0]+b__000006);
            z[63:0]<=(z[63:0]+b__000004);
            x<=(x+b);
            proc_state_000000<=3;
        end
        3 : begin
            w[0][63:0]<=(w[0][63:0]+b__000009);
            v[0]<=(v[0]+b__000007);
            z[63:0]<=(z[63:0]+b__000005);
            x<=(x+b__000001);
            proc_state_000000<=4;
        end
        4 : begin
            x<=(x+b__000002);
            proc_state_000000<=5;
        end
        5 : begin
            x<=(x+b__000003);
            proc_state_000000<=6;
        end
        6 : begin
            out<=x;
            proc_state_000000<=0;
        end
        endcase
    end

endmodule

