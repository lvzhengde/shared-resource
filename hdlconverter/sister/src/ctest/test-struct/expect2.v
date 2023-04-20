/*
    Created by Sister.
*/
module sample(
    out,
    in2,
    in1,
    sw2,
    sw,
    rst,
    clk);

    reg[1:0] proc_state_000000;
    reg[63:0] i;
    reg[15:0] x;
    output[15:0] out;
    reg[15:0] out;
    input[15:0] in2;
    input[15:0] in1;
    input[5:0] sw2;
    input sw;
    input rst;
    input clk;

    always@(posedge clk or negedge rst) begin
        case(proc_state_000000)
        0 : begin
            x<=10;
            proc_state_000000<=1;
        end
        1 : begin
            i<=0;
            proc_state_000000<=2;
        end
        2 : begin
            if((((i<100)&&sw)&&(sw2==0))) begin
                out<=in1;
                proc_state_000000<=0;
            end
            else if((((i<100)&&sw)&&(sw2==1))) begin
                out<=in2;
                proc_state_000000<=0;
            end
            else if((((i<100)&&sw)&&(sw2==2))) begin
                out<=in1;
                proc_state_000000<=0;
            end
            else if(((i<100)&&sw)) begin
                out<=in2;
                proc_state_000000<=0;
            end
            else if((i<100)) begin
                i<=i+1;
                out<=in2;
                proc_state_000000<=2;
            end
            else proc_state_000000<=1;
        end
        endcase
    end

endmodule

