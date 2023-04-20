/*
    Created by Sister.
*/
module test_struct(
    out,
    in);

    reg[2:0] proc_state_000000;
    reg[214:0] x;
    reg[214:0] y;
    reg[214:0] c;
    reg[214:0] b[0:1];
    reg[214:0] a;
    output[214:0] out;
    reg[214:0] out;
    input in;

    always@( in) begin
        case(proc_state_000000)
        0 : begin
            y<=b[0];
            a[214:207]<=25;
            a[206:199]<=24;
            a[198:191]<=23;
            a[190:183]<=22;
            a[182:175]<=21;
            a[174:167]<=20;
            a[166:159]<=19;
            a[158:151]<=18;
            a[150:143]<=17;
            a[142:135]<=16;
            a[134:127]<=15;
            a[126:119]<=14;
            a[118:111]<=13;
            a[110:103]<=12;
            a[102:95]<=11;
            a[94:87]<=10;
            a[86:79]<=9;
            a[78:71]<=8;
            a[70:63]<=7;
            a[62:55]<=6;
            a[54:47]<=5;
            a[46:39]<=4;
            a[38:31]<=3;
            a[30:23]<=2;
            a[22:15]<=1;
            a[14:0]<=0;
            proc_state_000000<=1;
        end
        1 : begin
            x<=a;
            proc_state_000000<=2;
        end
        2 : begin
            x[36:33]<=27;
            x[99:96]<=26;
            proc_state_000000<=3;
        end
        3 : begin
            c<=x;
            y<=x;
            proc_state_000000<=4;
        end
        4 : begin
            out<=c;
            proc_state_000000<=0;
        end
        endcase
    end

endmodule

