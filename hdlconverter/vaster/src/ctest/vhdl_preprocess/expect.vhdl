--sample.vhdl 1

entity sample is
    port(
        A: in  bit;
        B: in  bit;
        C: out bit);
end sample;


architecture sample_body of sample is
begin
    C <= A and B;
end sample_body;




