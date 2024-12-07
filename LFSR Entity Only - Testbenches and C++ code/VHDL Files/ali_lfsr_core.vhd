----------------------------------------------------------------------------------
-- Company: GMU
-- Engineer: Muhammad H. Ali
-- 
-- Create Date: 03/31/2021 02:44:26 PM
-- Design Name: 
-- Module Name: ali_lfsr_core - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments: 8th attempt at a different core design
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

entity ali_lfsr_core is
    Port ( clk : in STD_LOGIC;
           reset : in STD_LOGIC;
           cs : in STD_LOGIC;
           write : in STD_LOGIC;
           read : in STD_LOGIC;
           addr : in std_logic_vector(4 downto 0);
           wr_data : in std_logic_vector(31 downto 0);
           rd_data : out std_logic_vector(31 downto 0));
end ali_lfsr_core;

architecture Behavioral of ali_lfsr_core is
signal inputReg, outputReg1 : std_logic_vector(31 downto 0):= (others => '0');
signal EN : std_logic := '0';
signal Init : std_logic := '1';
begin

    LFSR : Entity work.LFSR(Behavioral)
    PORT MAP(
        CLK  => clk,
        INIT => Init,
        EN   => EN,
        Q    => outputReg1
    );
    
    -- gather inputs from wr_data
    process(clk, reset)
    begin
        if rising_edge(clk) then
            if write = '1' and cs = '1' and addr(1 downto 0) = "10" then
                inputReg <= wr_data;
            end if;
        end if;
    end process;
    
    EN <= '1' when Init = '0' else '0';
    Init <= '1' when inputReg(0) = '1' or reset = '1' else
           '0';
           
    rd_data <= outputReg1 when read = '1' and addr(0) = '0' else
               x"FFFFFFFF"; -- For debugging: Means the wrong address value was used
    

end Behavioral;
