----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 05/07/2021 11:32:00 PM
-- Design Name: 
-- Module Name: square_vga_slot_core - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
entity square_vga_slot_core is
   generic(
      CD         : integer                       := 12;
      ADDR_WIDTH : integer                       := 10;
      KEY_COLOR  : std_logic_vector(11 downto 0) := (others => '0')
   );

   port(
      clk     : in  std_logic;
      reset   : in  std_logic;
      -- frame counter
      x       : in  std_logic_vector(10 downto 0);
      y       : in  std_logic_vector(10 downto 0);
      -- video slot interface
      cs      : in  std_logic;
      write   : in  std_logic;
      addr    : in  std_logic_vector(13 downto 0);
      wr_data : in  std_logic_vector(31 downto 0);
      -- stream interface
      si_rgb  : in  std_logic_vector(CD - 1 downto 0);
      so_rgb  : out std_logic_vector(CD - 1 downto 0)
   );
end square_vga_slot_core;

architecture arch of square_vga_slot_core is
   signal wr_en      : std_logic;
   signal wr_ram     : std_logic;
   signal wr_reg     : std_logic;
   signal wr_bypass  : std_logic;
   signal wr_x0      : std_logic;
   signal wr_y0      : std_logic;
   signal x0_reg     : std_logic_vector(10 downto 0);
   signal y0_reg     : std_logic_vector(10 downto 0);
   signal bypass_reg : std_logic;
   signal square_rgb  : std_logic_vector(CD - 1 downto 0);
   signal chrom_rgb  : std_logic_vector(CD - 1 downto 0);
begin
   -- instantiate sprite generator
   slot_unit : entity work.square_src
      generic map(
         CD        => 12,
         KEY_COLOR => (others => '0')
      )
      port map(
         clk       => clk,
         x         => x,
         y         => y,
         x0        => x0_reg,
         y0        => y0_reg,
         we        => wr_ram,
         addr_w    => addr(ADDR_WIDTH - 1 downto 0),
         pixel_in  => wr_data(CD - 1 downto 0),
         square_rgb => square_rgb
      );
   -- registers and decoding 
   process(clk, reset)
   begin
      if reset = '1' then
         x0_reg <= (others => '0');
         y0_reg <= (others => '0');
         bypass_reg <= '0';
      elsif (clk'event and clk = '1') then
         if wr_x0 = '1' then
            x0_reg <= wr_data(10 downto 0);
         end if;
         if wr_y0 = '1' then
            y0_reg <= wr_data(10 downto 0);
         end if;
         if wr_bypass = '1' then
            bypass_reg <= wr_data(0);
         end if;
      end if;
   end process;
   wr_en     <= '1' when write = '1' and cs = '1' else '0';
   wr_ram    <= '1' when addr(13) = '0' and wr_en = '1' else '0';
   wr_reg    <= '1' when addr(13) = '1' and wr_en = '1' else '0';
   wr_bypass <= '1' when wr_reg='1' and addr(1 downto 0)="00" else '0';
   wr_x0     <= '1' when wr_reg='1' and addr(1 downto 0)="01" else '0';
   wr_y0     <= '1' when wr_reg='1' and addr(1 downto 0)="10" else '0';
   -- chroma-key blending and multiplexing
   chrom_rgb <= square_rgb when square_rgb /= KEY_COLOR else si_rgb;
   so_rgb    <= si_rgb when bypass_reg = '1' else chrom_rgb;
   --so_rgb <= x"0f0";
end arch;

