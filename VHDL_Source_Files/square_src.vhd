----------------------------------------------------------------------------------
-- Company: GMU?
-- Engineer: Muhammad H. Ali
-- 
-- Create Date: 05/07/2021 11:24:00 PM
-- Design Name: 
-- Module Name: square_src - Behavioral
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
entity square_src is
   generic(
      CD        : integer                       := 12;
      ADDR      : integer                       := 10;
      KEY_COLOR : std_logic_vector(11 downto 0) := (others => '0')
   );                            
   port(
      clk       : std_logic;
      -- frame counter input
      x, y      : in  std_logic_vector(10 downto 0);
      -- origin of sprite 
      x0, y0    : in  std_logic_vector(10 downto 0);
      -- sprite ram write 
      we        : in  std_logic;
      addr_w    : in  std_logic_vector(9 downto 0);
      pixel_in  : in  std_logic_vector(CD - 1 downto 0);
      -- pixel output
      square_rgb : out std_logic_vector(CD - 1 downto 0)
   );
end square_src;

architecture arch of square_src is
   -- sprite size
   constant H_SIZE       : integer := 32; -- horizontal size of sprite
   constant V_SIZE       : integer := 32; -- vertical size of sprite
   signal xr             : signed(11 downto 0); -- relative x position
   signal yr             : signed(11 downto 0); -- relative y position
   signal in_region      : std_logic;
   signal addr_r         : std_logic_vector(9 downto 0);
   signal full_rgb       : std_logic_vector(CD - 1 downto 0);
   signal out_rgb        : std_logic_vector(CD - 1 downto 0);
   signal out_rgb_d1_reg : std_logic_vector(CD - 1 downto 0);
begin
   -- instantiate sprite RAM
   slot_ram_unit : entity work.square_ram
      generic map(
         ADDR_WIDTH => ADDR,
         DATA_WIDTH => CD
      )
      port map(
         clk    => clk,
         we     => we,
         addr_w => addr_w,
         din    => pixel_in,
         addr_r => addr_r,
         dout   => full_rgb
      );
   addr_r <= std_logic_vector(yr(4 downto 0) & xr(4 downto 0));
   -- relative coordinate calculation
   xr <= signed('0' & x) - signed('0' & x0);
   yr <= signed('0' & y) - signed('0' & y0);
   -- in-region comparison and multiplexing 
   in_region <= '1' when (0 <= xr) and (xr < H_SIZE) and 
                         (0 <= yr) and (yr < V_SIZE) else 
                '0';
   out_rgb   <= full_rgb when in_region = '1' else KEY_COLOR;
   -- delay line (one clock) 
   process(clk)
   begin
      if (clk'event and clk = '1') then
         out_rgb_d1_reg <= out_rgb;
      end if;
   end process;
   square_rgb <= out_rgb_d1_reg;
end arch;
  
