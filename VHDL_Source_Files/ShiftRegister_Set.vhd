library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity ShiftRegister_Set is
    Port ( CLK : in STD_LOGIC;
           EN : in STD_LOGIC;
           SET : in STD_LOGIC;
           D : in STD_LOGIC;
           Q : out STD_LOGIC);
end ShiftRegister_Set;

architecture Behavioral of ShiftRegister_Set is
begin 
    
    process(CLK)
    begin
        if rising_edge(clk) then
            if SET = '1' then
                Q <= '1';
            elsif(EN = '1') then
                Q <= D;
            end if;
        end if;
        
    end process;

end Behavioral;
