
--comment begin
entity sample is
    port(
        A: in  bit;
        B: in  bit;
        C: out bit);
end sample;


architecture sample_body of sample is
begin
    bl: block 
    begin 
    --    D <= guarded A and B;
    end block;

end sample_body;

--comment end

