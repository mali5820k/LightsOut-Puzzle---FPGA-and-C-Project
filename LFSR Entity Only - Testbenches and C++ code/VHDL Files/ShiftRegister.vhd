library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity ShiftRegister is
    Port ( CLK : in STD_LOGIC;
           EN : in STD_LOGIC;
           RST : in STD_LOGIC;
           D : in STD_LOGIC;
           Q : out STD_LOGIC);
end ShiftRegister;

architecture Behavioral of ShiftRegister is

begin
    process(CLK)
    begin
        if rising_edge(clk) then
            if rst = '1' then
                Q <= '0';
            elsif(EN = '1') then
                Q <= D;
            end if;
        end if;
        
    end process;

end Behavioral;
