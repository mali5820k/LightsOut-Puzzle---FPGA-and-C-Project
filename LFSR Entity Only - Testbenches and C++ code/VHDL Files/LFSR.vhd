library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

entity LFSR is
    Port ( CLK : in std_logic;
           INIT: in std_logic;
           EN : in std_logic;
           Q : out std_logic_vector(31 downto 0));
           
end LFSR;

architecture Behavioral of LFSR is

signal d1, q1, q2, q3, q4, q5, q6, q7, q8, q9 : std_logic := '0';

begin
       
    Q <= x"00000" & "000" & q1 & q2 & q3 & q4 & q5 & q6 & q7 & q8 & q9;

    d1 <= q4 XOR q9;
    shift1 : Entity work.ShiftRegister(Behavioral)
    PORT MAP(
        CLK => CLK,
        EN => EN,
        RST => INIT,
        D => d1,
        Q => q1
    );
    
    shift2 : Entity work.ShiftRegister_Set(Behavioral)
    PORT MAP(
        CLK => CLK,
        EN => EN,
        SET => INIT,
        D => q1,
        Q => q2
    );
    
    shift3 : Entity work.ShiftRegister(Behavioral)
    PORT MAP(
        CLK => CLK,
        EN => EN,
        RST => INIT,
        D => q2,
        Q => q3
    );
    
    shift4 : Entity work.ShiftRegister_Set(Behavioral)
    PORT MAP(
        CLK => CLK,
        EN => EN,
        SET => INIT,
        D => q3,
        Q => q4
    );
    
    shift5 : Entity work.ShiftRegister(Behavioral)
    PORT MAP(
        CLK => CLK,
        EN => EN,
        RST => INIT,
        D => q4,
        Q => q5
    );
    
    shift6 : Entity work.ShiftRegister_Set(Behavioral)
    PORT MAP(
        CLK => CLK,
        EN => EN,
        SET => INIT,
        D => q5,
        Q => q6
    );
    
    shift7 : Entity work.ShiftRegister(Behavioral)
    PORT MAP(
        CLK => CLK,
        EN => EN,
        RST => INIT,
        D => q6,
        Q => q7
    );
    
    shift8 : Entity work.ShiftRegister_Set(Behavioral)
    PORT MAP(
        CLK => CLK,
        EN => EN,
        SET => INIT,
        D => q7,
        Q => q8
    );
    
    shift9 : Entity work.ShiftRegister(Behavioral)
    PORT MAP(
        CLK => CLK,
        EN => EN,
        RST => INIT,
        D => q8,
        Q => q9
    );
    
end Behavioral;
