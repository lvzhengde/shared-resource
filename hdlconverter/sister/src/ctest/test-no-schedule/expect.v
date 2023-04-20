/*
    Created by Sister.
*/
module sample(
    out,
    in3,
    in2,
    in1,
    sel);

    reg[15:0] y;
    reg[7:0] x;
    output[15:0] out;
    reg[15:0] out;
    input[7:0] in3;
    input[7:0] in2;
    input[7:0] in1;
    input[2:0] sel;
    reg[8:0] __tmp_000005;
    reg[8:0] __tmp_000004;
    reg[8:0] __tmp_000003;
    reg[15:0] __tmp_000002;
    reg[15:0] __tmp_000001;
    reg[8:0] __tmp_000000;

    always@( in3 or  in2 or  in1) begin
      x=10;
      if((!sel)) begin
        __tmp_000000=(in1+in2);
        y=(__tmp_000000+in3);
      end
      else begin
        if((sel==1)) begin
          __tmp_000001=(in2*in3);
          y=(in1+__tmp_000001);
        end
        else begin
          if((sel==2)) begin
            __tmp_000002=(in1*in2);
            y=(__tmp_000002+in3);
          end
          else begin
            if((sel==3)) begin
              __tmp_000003=(in1-in2);
              y=(__tmp_000003-in3);
            end
            else begin
              if((sel==4)) begin
                __tmp_000004=(in1-in2);
                y=(__tmp_000004+in3);
              end
              else begin
                __tmp_000005=(in1+in2);
                y=(__tmp_000005-in3);
              end
            end
          end
        end
      end
      out<=(x+y);
    end

endmodule

