library ieee;
use ieee.std_logic_1164.all;
use work.chu_io_map.all;
entity mmio_sys_vanilla is
   generic(
     N_LED: integer;
     N_SW: integer
	);  
   port(
      -- FPro bus
      clk          : in  std_logic;
      reset        : in  std_logic;
      mmio_cs      : in  std_logic;
      mmio_wr      : in  std_logic;
      mmio_rd      : in  std_logic;
      mmio_addr    : in  std_logic_vector(20 downto 0); -- only 11 LSBs used
      mmio_wr_data : in  std_logic_vector(31 downto 0);
      mmio_rd_data : out std_logic_vector(31 downto 0);
      -- switches and LEDs
      sw           : in  std_logic_vector(N_SW-1 downto 0);
      led          : out std_logic_vector(N_LED-1 downto 0);
      -- uart
      rx           : in  std_logic;
      tx           : out std_logic;
      -- btn
      btn          : in    std_logic_vector(4 downto 0)
   );
end mmio_sys_vanilla;

architecture arch of mmio_sys_vanilla is
   signal cs_array       : std_logic_vector(63 downto 0);
   signal reg_addr_array : slot_2d_reg_type;
   signal mem_rd_array   : std_logic_vector(63 downto 0);
   signal mem_wr_array   : std_logic_vector(63 downto 0);
   signal rd_data_array  : slot_2d_data_type;
   signal wr_data_array  : slot_2d_data_type;
begin
   --******************************************************************
   --  MMIO controller instantiation  
   --******************************************************************
   ctrl_unit : entity work.chu_mmio_controller
      port map(
         -- FPro bus interface
         mmio_cs             => mmio_cs,
         mmio_wr             => mmio_wr,
         mmio_rd             => mmio_rd,
         mmio_addr           => mmio_addr,
         mmio_wr_data        => mmio_wr_data,
         mmio_rd_data        => mmio_rd_data,
         -- 64 slot interface
         slot_cs_array       => cs_array,
         slot_reg_addr_array => reg_addr_array,
         slot_mem_rd_array   => mem_rd_array,
         slot_mem_wr_array   => mem_wr_array,
         slot_rd_data_array  => rd_data_array,
         slot_wr_data_array  => wr_data_array
      );

   --******************************************************************
   -- IO slots instantiations
   --******************************************************************
   -- slot 0: system timer 
   timer_slot0 : entity work.chu_timer
      port map(
         clk           => clk,
         reset         => reset,
         cs            => cs_array(S0_SYS_TIMER),
         read          => mem_rd_array(S0_SYS_TIMER),
         write         => mem_wr_array(S0_SYS_TIMER),
         addr          => reg_addr_array(S0_SYS_TIMER),
         rd_data       => rd_data_array(S0_SYS_TIMER),
         wr_data       => wr_data_array(S0_SYS_TIMER)
      );
   -- slot 1: uart1     
   uart1_slot1 : entity work.chu_uart
      generic map(FIFO_DEPTH_BIT => 6)
      port map(
         clk     => clk,
         reset   => reset,
         cs      => cs_array(S1_UART1),
         read    => mem_rd_array(S1_UART1),
         write   => mem_wr_array(S1_UART1),
         addr    => reg_addr_array(S1_UART1),
         rd_data => rd_data_array(S1_UART1),
         wr_data => wr_data_array(S1_UART1),
         -- external signals
         tx      => tx,
         rx      => rx
      );
   -- slot 2: GPO for LEDs
   gpo_slot2 : entity work.chu_gpo
      generic map(W => N_LED)
      port map(
         clk     => clk,
         reset   => reset,
         cs      => cs_array(S2_LED),
         read    => mem_rd_array(S2_LED),
         write   => mem_wr_array(S2_LED),
         addr    => reg_addr_array(S2_LED),
         rd_data => rd_data_array(S2_LED),
         wr_data => wr_data_array(S2_LED),
         -- external signal
         dout    => led
      );
   -- slot 3: input port for switches     
   gpi_slot3 : entity work.chu_gpi
      generic map(W => N_SW)
      port map(
         clk     => clk,
         reset   => reset,
         cs      => cs_array(S3_SW),
         read    => mem_rd_array(S3_SW),
         write   => mem_wr_array(S3_SW),
         addr    => reg_addr_array(S3_SW),
         rd_data => rd_data_array(S3_SW),
         wr_data => wr_data_array(S3_SW),
         -- external signal
         din     => sw
      );
   lfsr_slot4 : entity work.ali_lfsr_core(Behavioral)
    port map(
       clk      => clk,
       reset    => reset,
       cs       => cs_array(S4_USER),
       read     => mem_rd_array(S4_USER),
       write    => mem_wr_array(S4_USER),
       addr     => reg_addr_array(S4_USER),
       rd_data  => rd_data_array(S4_USER),
       wr_data  => wr_data_array(S4_USER)
      );
   --rd_data_array(4) <= (others => '0');
   -- slot 7: push button     
   debounce_slot7 : entity work.chu_debounce_core
      generic map(
         W => 5,       
         N => 20  
      )
      port map(
         clk     => clk,
         reset   => reset,
         cs      => cs_array(S7_BTN),
         read    => mem_rd_array(S7_BTN),
         write   => mem_wr_array(S7_BTN),
         addr    => reg_addr_array(S7_BTN),
         rd_data => rd_data_array(S7_BTN),
         wr_data => wr_data_array(S7_BTN),
         -- external interface
         din     => btn
      );
   -- assign 0's to all unused slot rd_data signals 
   rd_data_array(5) <= (others => '0');
   rd_data_array(6) <= (others => '0');
   gen_unused_slot : for i in 8 to 63 generate
      rd_data_array(i) <= (others => '0');
   end generate gen_unused_slot;
end arch;
