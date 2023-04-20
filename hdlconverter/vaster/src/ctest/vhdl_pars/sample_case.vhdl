
--comment begin
entity sample is
    port(
        A: in  bit;
        B: in  bit;
        C: out bit);
end sample;


architecture sample_body of sample is
begin
    process(A) begin
        case A is
        when '1' =>    A;
        when others => B;
        end case;
    end process;
end sample_body;

--comment end

