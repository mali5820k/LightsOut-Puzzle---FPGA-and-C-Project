library ieee;
use ieee.std_logic_1164.all;
use work.chu_io_map.all;

entity video_sys_daisy is
   generic(
      CD              : integer := 12;  -- color depth
      VRAM_DATA_WIDTH : integer := 9    -- frame buffer data width
   );
   port(
      clk_sys       : in  std_logic;
      clk_25M       : in  std_logic;
      reset_sys     : in  std_logic;
      -- generic bus interface
      video_cs      : in  std_logic;
      video_wr      : in  std_logic;
      video_addr    : in  std_logic_vector(20 downto 0);
      video_wr_data : in  std_logic_vector(31 downto 0);
      -- to vga monitor  
      vsync, hsync  : out std_logic;
      rgb           : out std_logic_vector(11 downto 0)
   );
end video_sys_daisy;

architecture arch of video_sys_daisy is
   constant KEY_COLOR : std_logic_vector(CD - 1 downto 0) := (others => '0');
   -- video data stream
   signal frame_rgb8          : std_logic_vector(CD - 1 downto 0);
   signal bar_rgb7            : std_logic_vector(CD - 1 downto 0);
   signal gray_rgb6           : std_logic_vector(CD - 1 downto 0);
   signal user5_rgb5          : std_logic_vector(CD - 1 downto 0);
   signal user4_rgb4          : std_logic_vector(CD - 1 downto 0);
   signal ghost_rgb3          : std_logic_vector(CD - 1 downto 0);
   signal osd_rgb2            : std_logic_vector(CD - 1 downto 0);
   signal mouse_rgb1          : std_logic_vector(CD - 1 downto 0);
   signal line_data_in        : std_logic_vector(CD downto 0);
   -- frame counter
   signal inc                 : std_logic;
   signal x, y                : std_logic_vector(10 downto 0);
   signal frame_start         : std_logic;
   -- delay line
   signal frame_start_d1_reg  : std_logic;
   signal frame_start_d2_reg  : std_logic;
   signal inc_d1_reg          : std_logic;
   signal inc_d2_reg          : std_logic;
   -- frame interface
   signal frame_wr, frame_cs  : std_logic;
   signal frame_addr          : std_logic_vector(19 downto 0);
   signal frame_wr_data       : std_logic_vector(31 downto 0);
   -- video core slot interface 
   signal slot_cs_array       : std_logic_vector(7 downto 0);
   signal slot_mem_wr_array   : std_logic_vector(7 downto 0);
   signal slot_reg_addr_array : slot_2d_video_reg_type;
   signal slot_wr_data_array  : slot_2d_video_data_type;
begin
   --******************************************************************
   -- 2-stage delay line for start signal
   --******************************************************************
   process(clk_sys)
   begin
      if (clk_sys'event and clk_sys = '1') then
         frame_start_d1_reg <= frame_start;
         frame_start_d2_reg <= frame_start_d1_reg;
         inc_d1_reg         <= inc;
         inc_d2_reg         <= inc_d1_reg;
      end if;
   end process;
   --******************************************************************
   -- instantiate global frame counter
   --******************************************************************
   counter_unit : entity work.frame_counter
      generic map(
         HMAX => 640,
         VMAX => 480)
      port map(
         clk         => clk_sys,
         reset       => reset_sys,
         inc         => inc,
         sync_clr    => '0',
         hcount      => x,
         vcount      => y,
         frame_start => frame_start,
         frame_end   => open);
   --******************************************************************
   -- instantiate video decoding circuit 
   --******************************************************************
   ctrl_unit : entity work.chu_video_controller
      port map(
         video_cs            => video_cs,
         video_wr            => video_wr,
         video_addr          => video_addr,
         video_wr_data       => video_wr_data,
         frame_cs            => frame_cs,
         frame_wr            => frame_wr,
         frame_addr          => frame_addr,
         frame_wr_data       => frame_wr_data,
         slot_cs_array       => slot_cs_array,
         slot_mem_wr_array   => slot_mem_wr_array,
         slot_reg_addr_array => slot_reg_addr_array,
         slot_wr_data_array  => slot_wr_data_array
      );
   --******************************************************************
   -- frame buffer and video cores
   --******************************************************************
   -- instantiate frame buffer
   frame_unit : entity work.chu_frame_buffer_core
      generic map(
         CD => CD,
         DW => VRAM_DATA_WIDTH
      )
      port map(
         clk     => clk_sys,
         reset   => reset_sys,
         x       => x,
         y       => y,
         cs      => frame_cs,
         write   => frame_wr,
         addr    => frame_addr,
         wr_data => video_wr_data,
         si_rgb  => x"008",        -- blue screen
         so_rgb  => frame_rgb8
      );
   -- instantiate bar generator 
   v7_bar_unit : entity work.chu_vga_bar_core
      port map(
         clk     => clk_sys,
         reset   => reset_sys,
         x       => x,
         y       => y,
         cs      => slot_cs_array(V7_BAR),
         write   => slot_mem_wr_array(V7_BAR),
         addr    => slot_reg_addr_array(V7_BAR),
         wr_data => slot_wr_data_array(V7_BAR),
         si_rgb  => frame_rgb8,
         so_rgb  => bar_rgb7);
   -- instantiate rgb-to-gray generator 
   v6_gray_unit : entity work.chu_rgb2gray_core
      port map(
         clk     => clk_sys,
         reset   => reset_sys,
         cs      => slot_cs_array(V6_GRAY),
         write   => slot_mem_wr_array(V6_GRAY),
         addr    => slot_reg_addr_array(V6_GRAY),
         wr_data => slot_wr_data_array(V6_GRAY),
         si_rgb  => bar_rgb7,
         so_rgb  => gray_rgb6);
   -- instantiate dummy unit (place holder for 1st user defined unit)  
   --v5_user_unit : entity work.chu_vga_dummy_core
   v5_user_unit : entity work.square_vga_slot_core
      generic map(
         CD         => CD,
         ADDR_WIDTH => 10,
         KEY_COLOR  => KEY_COLOR
      )
      port map(
         clk     => clk_sys,
         reset   => reset_sys,
         x       => x,
         y       => y,
         cs      => slot_cs_array(V5_USER5),
         write   => slot_mem_wr_array(V5_USER5),
         addr    => slot_reg_addr_array(V5_USER5),
         wr_data => slot_wr_data_array(V5_USER5),
         si_rgb  => gray_rgb6,
         so_rgb  => user5_rgb5
      );
   -- instantiate dummy unit (place holder for 2nd user defined unit)  
   --v4_user_unit : entity work.chu_vga_dummy_core
   v4_user_unit : entity work.chu_vga_dummy_core
       port map(
             clk     => clk_sys,
             reset   => reset_sys,
             cs      => slot_cs_array(V4_USER4),
             write   => slot_mem_wr_array(V4_USER4),
             addr    => slot_reg_addr_array(V4_USER4),
             wr_data => slot_wr_data_array(V4_USER4),
             si_rgb  => user5_rgb5,
             so_rgb  => user4_rgb4
       );
--      generic map(
--         CD         => CD,
--         ADDR_WIDTH => 10,
--         KEY_COLOR  => KEY_COLOR
--      )
--      port map(
--         clk     => clk_sys,
--         reset   => reset_sys,
--         x       => x,
--         y       => y,
--         cs      => slot_cs_array(V5_USER5),
--         write   => slot_mem_wr_array(V5_USER5),
--         addr    => slot_reg_addr_array(V5_USER5),
--         wr_data => slot_wr_data_array(V5_USER5),
--         si_rgb  => gray_rgb6,
--         so_rgb  => user5_rgb5
--      );
   -- instantiate ghost sprite
   v3_ghost_unit : entity work.chu_vga_slot_ghost_core
      generic map(
         CD         => CD,
         ADDR_WIDTH => 10,
         KEY_COLOR  => KEY_COLOR
      )
      port map(
         clk     => clk_sys,
         reset   => reset_sys,
         x       => x,
         y       => y,
         cs      => slot_cs_array(V3_GHOST),
         write   => slot_mem_wr_array(V3_GHOST),
         addr    => slot_reg_addr_array(V3_GHOST),
         wr_data => slot_wr_data_array(V3_GHOST),
         si_rgb  => user4_rgb4,
         so_rgb  => ghost_rgb3
      );
   -- instantiate osd 
   v2_osd_unit : entity work.chu_vga_osd_core
      generic map(
         CD        => CD,
         KEY_COLOR => KEY_COLOR
      )
      port map(
         clk     => clk_sys,
         reset   => reset_sys,
         x       => x,
         y       => y,
         cs      => slot_cs_array(V2_OSD),
         write   => slot_mem_wr_array(V2_OSD),
         addr    => slot_reg_addr_array(V2_OSD),
         wr_data => slot_wr_data_array(V2_OSD),
         si_rgb  => ghost_rgb3,         --bar_rgb,
         so_rgb  => osd_rgb2
      );
   -- instantiate mouse sprite
   v1_mouse_unit : entity work.chu_vga_slot_mouse_core
      generic map(
         CD         => CD,
         ADDR_WIDTH => 10,
         KEY_COLOR  => KEY_COLOR
      )
      port map(
         clk     => clk_sys,
         reset   => reset_sys,
         x       => x,
         y       => y,
         cs      => slot_cs_array(V1_MOUSE),
         write   => slot_mem_wr_array(V1_MOUSE),
         addr    => slot_reg_addr_array(V1_MOUSE),
         wr_data => slot_wr_data_array(V1_MOUSE),
         si_rgb  => osd_rgb2,
         so_rgb  => mouse_rgb1
      );
   -- merge start bit to rgb data stream
   line_data_in <= mouse_rgb1 & frame_start_d2_reg;
   -- instantiate sync_core
   v0_vga_sync_unit : entity work.chu_vga_sync_core
      generic map(CD => CD)
      port map(
         clk_sys  => clk_sys,
         reset    => reset_sys,
         clk_25M  => clk_25M,
         cs       => slot_cs_array(V0_SYNC),
         write    => slot_mem_wr_array(V0_SYNC),
         addr     => slot_reg_addr_array(V0_SYNC),
         wr_data  => slot_wr_data_array(V0_SYNC),
         si_data  => line_data_in,
         si_valid => inc_d2_reg,
         si_ready => inc,
         hsync    => hsync,
         vsync    => vsync,
         rgb      => rgb
      );
end arch;

