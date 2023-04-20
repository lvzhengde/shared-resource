module sample(clk,rst,in,out) ;
    input clk,rst,in;
    output out;
    reg out;

    reg state;
    
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

endmodule

