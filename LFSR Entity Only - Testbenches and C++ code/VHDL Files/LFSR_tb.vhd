library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

entity LFSR_tb is
--  Port ( );
end LFSR_tb;

architecture Behavioral of LFSR_tb is
signal clk, rst, cs, write, read : std_logic := '0';
signal addr : std_logic_vector(4 downto 0) := "00000";
signal rd_data, wr_data : std_logic_vector(31 downto 0) := x"00000000";

Component ali_lfsr_core port( 
           clk : in STD_LOGIC;
           reset : in STD_LOGIC;
           cs : in STD_LOGIC;
           write : in STD_LOGIC;
           read : in STD_LOGIC;
           addr : in std_logic_vector(4 downto 0);
           wr_data : in std_logic_vector(31 downto 0);
           rd_data : out std_logic_vector(31 downto 0));
end component;

Constant clkPeriod : Time := 10ns;

begin

    UUT: ali_lfsr_core port map(
        clk => clk,
        reset => rst,
        cs => cs,
        write => write,
        read => read,
        addr => addr,
        rd_data => rd_data,
        wr_data => wr_data
    );

        
    
    clk <= not(clk) after clkPeriod/2;
    rst <= '0';
    read <= '1';
    addr <= "00010";
    cs <= '1';
    process
    begin
        write <= '1';
        -- Initial half period just so that inputs can be assigned on falling clock edges:
        wr_data <= x"00000001"; -- Init to default value;
        wait for clkPeriod;
        wait for clkPeriod;
        write <= '0';
        wait for clkPeriod;
        
        write <= '1';
        wr_data <= x"00000000"; -- Start generating randomized outputs
        wait for clkPeriod;
        wait for clkPeriod;
        write <= '0';
        wait for clkPeriod;
        wait for clkPeriod;
        
         write <= '1';
        -- Initial half period just so that inputs can be assigned on falling clock edges:
        wr_data <= x"00000001"; -- Init to default value;
        wait for clkPeriod;
        wait for clkPeriod;
        write <= '0';
        wait for clkPeriod;
        wait for clkPeriod;
        wait for clkPeriod;
        wait for clkPeriod;
        wait for clkPeriod;
        wait for clkPeriod;
    end process;

end Behavioral;
