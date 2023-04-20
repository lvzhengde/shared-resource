`define X(s,n) s=n

module sample ;
    wire out;
    assign `X(out,(1));

endmodule

