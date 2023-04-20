
--comment begin
entity sample is
    port(
        A: in  bit;
        B: in  bit;
        C: out bit);
end sample;


architecture sample_body of sample is
constant C1 , C2 : bit :="0100";

begin
    null ;
end sample_body;

--comment end

