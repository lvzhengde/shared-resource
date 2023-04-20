
`define  X 1
//`define  Y 1

//aaa bbb ccc


`ifdef X

module sample1 ;
    wire out;
    assign out=1;
endmodule

`ifdef Y
module sample3 ;
    wire out;
    assign out=1;
endmodule

`endif
`endif

/*
a b c d
*/
module sample2 ;
    wire out;
    assign out=1;
endmodule


