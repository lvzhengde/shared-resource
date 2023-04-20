/*
    Created by Sister.
*/
module rgby(
    y__ready,
    y,
    rgb__ready,
    rgb,
    rst,
    clk);

    reg sw;
    reg[30:0] t1_2;
    reg[27:0] t3;
    reg[28:0] t4;
    reg[25:0] t5;
    reg[23:0] inbuf;
    reg[3:0] state;
    output y__ready;
    reg y__ready;
    output[15:0] y;
    reg[15:0] y;
    input rgb__ready;
    input[23:0] rgb;
    input rst;
    input clk;

    always@(posedge clk or negedge rst) begin
      if((!rst)) begin
        y<=0;
        y__ready<=0;
        state<=0;
      end
      else begin
        if((state==0)) begin
          if((!rgb__ready)) begin
            state<=0;
          end
          else begin
            inbuf<=rgb;
            state<=1;
          end
        end
        else if((state==1)) begin
          if(rgb__ready) begin
            state<=1;
          end
          else begin
            state<=2;
          end
        end
        else if((state==2)) begin
          sw<=1;
          state<=3;
        end
        else if((state==3)) begin
          sw<=0;
          state<=4;
        end
        else if((state==4)) begin
          state<=5;
        end
        else if((state==5)) begin
          y__ready<=1;
          y<=t1_2;
          state<=6;
        end
        else begin
          y__ready<=0;
          state<=0;
        end
      end
    end

    always@(posedge clk or negedge rst) begin
      if((!rst)) begin
        t1_2<=0;
        t3<=0;
        t4<=0;
        t5<=0;
      end
      else begin
        t5<=(inbuf[7:0]<<2);
        t4<=(inbuf[15:8]*19);
        t3<=(inbuf[23:16]*10);
        if(sw) begin
          t1_2<=(t3+t4);
        end
        else begin
          t1_2<=(t1_2+t5);
        end
      end
    end

endmodule

