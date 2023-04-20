
--comment begin
entity sample is
    port(
        A: in  bit;
        B: in  bit;
        C: out bit);
end sample;


architecture sample_body of sample is
component COMP1
    port(
        A: in  bit;
        B: in  bit;
        C: out bit);
end component;
begin
    A;
end sample_body;

--comment end

