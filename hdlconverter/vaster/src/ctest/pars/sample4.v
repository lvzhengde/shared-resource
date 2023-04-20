module sample ;
    reg clk,rst,in;
    output out;
    reg out;

    reg state;
    
    integer i;
    
    always #1 clk=~clk;

    always@(posedge clk or negedge rst) begin
        if(!rst) begin 
            out<=0;
            state<=0;
        end
        else begin
            case(state)
                1'b0: state<=1'b1;
                1'b1:  begin
                    state<=1'b0;
                    out<=in;
                end
                default:state<=1'b0;
            endcase
        end
    end

    initial begin
        in=1;
        rst=0;
        clk=0;
        #2
        rst=1;
        for(i=0;i<10;i=i+1) #2;
        i=0;
        while(i<10) begin
            #2
            i=i+1;
        end
    end
endmodule

