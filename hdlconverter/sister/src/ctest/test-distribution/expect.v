/*
    Created by Sister.
*/
module sample(
    out,
    in3,
    in2,
    in1);

    reg[7:0] z;
    reg[7:0] y;
    reg[7:0] x;
    reg[15:0] tmp;
    output[15:0] out;
    reg[15:0] out;
    input[7:0] in3;
    input[7:0] in2;
    input[7:0] in1;
    reg[8:0] __tmp_000006;
    reg[7:0] __tmp_000005;
    reg[7:0] __tmp_000004;
    reg[8:0] __tmp_000003;
    reg[8:0] __tmp_000002;
    reg[8:0] __tmp_000001;
    reg[8:0] __tmp_000000;

    always@( in3 or  in2 or  in1) begin
      x=in1;
      y=in2;
      z=in3;
      __tmp_000000=(x+y);
      tmp=(__tmp_000000*z);
      __tmp_000001=(x-y);
      tmp=(__tmp_000001*z);
      __tmp_000002=(x-y);
      tmp=(__tmp_000002*z);
      __tmp_000003=(x-y);
      tmp=(__tmp_000003*z);
      __tmp_000004=(z/x);
      __tmp_000005=(y/z);
      tmp=(__tmp_000004-__tmp_000005);
      __tmp_000006=(x-y);
      tmp=(__tmp_000006/z);
      out<=tmp;
    end

endmodule

