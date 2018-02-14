-- File: fsm.vhd
-- Date: 05.05.2016
-- Author: Jan Gula, xgulaj00
-- Project: INC-PROJ1
-- Description: Access terminal
--

library ieee;
use ieee.std_logic_1164.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------

entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;
   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;
   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;
-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------

architecture behavioral of fsm is
   type t_state is (TEST1, TEST2, TEST3, TEST_1_3, TEST_1_4, TEST_1_5, TEST_1_6, TEST_1_7,
      TEST_1_8, TEST_1_9, TEST_1_10, TEST_2_3, TEST_2_4, TEST_2_5, TEST_2_6, TEST_2_7, TEST_2_8,
      TEST_2_9, TEST_2_10, ERROR, PRINT_CORRECT,
      PRINT_INVALID, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST1 =>
      next_state <= TEST1;
      if (KEY(4) = '1') then
         next_state <= TEST2;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST2 =>
      next_state <= TEST2;
      if (KEY(6) = '1') then
         next_state <= TEST3;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST3 =>
      next_state <= TEST3;
      if (KEY(0) = '1') then
         next_state <= TEST_1_3;
      elsif (KEY(8) = '1') then
         next_state <= TEST_2_3;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_1_3 =>
      next_state <= TEST_1_3;
      if (KEY(2) = '1') then
         next_state <= TEST_1_4;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_1_4 =>
      next_state <= TEST_1_4;
      if (KEY(7) = '1') then
         next_state <= TEST_1_5;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_1_5 =>
      next_state <= TEST_1_5;
      if (KEY(2) = '1') then
         next_state <= TEST_1_6;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_1_6 =>
      next_state <= TEST_1_6;
      if (KEY(9) = '1') then
         next_state <= TEST_1_7;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_1_7 =>
      next_state <= TEST_1_7;
      if (KEY(4) = '1') then
         next_state <= TEST_1_8;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_1_8 =>
      next_state <= TEST_1_8;
      if (KEY(2) = '1') then
         next_state <= TEST_1_9;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_1_9 =>
      next_state <= TEST_1_9;
      if (KEY(4) = '1') then
         next_state <= TEST_1_10;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_1_10 =>
      next_state <= TEST_1_10;
      if (KEY(15) = '1') then
         next_state <= PRINT_CORRECT;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when TEST_2_3 =>
      next_state <= TEST_2_3;
		if (KEY(0) = '1') then
         next_state <= TEST_2_4;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_2_4 =>
      next_state <= TEST_2_4;
      if (KEY(8) = '1') then
         next_state <= TEST_2_5;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_2_5 =>
      next_state <= TEST_2_5;
      if (KEY(1) = '1') then
         next_state <= TEST_2_6;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_2_6 =>
      next_state <= TEST_2_6;
      if (KEY(8) = '1') then
         next_state <= TEST_2_7;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_2_7 =>
      next_state <= TEST_2_7;
      if (KEY(8) = '1') then
         next_state <= TEST_2_8;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_2_8 =>
      next_state <= TEST_2_8;
      if (KEY(2) = '1') then
         next_state <= TEST_2_9;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_2_9 =>
      next_state <= TEST_2_9;
		if (KEY(7) = '1') then
         next_state <= TEST_2_10;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	 when TEST_2_10 =>
      next_state <= TEST_2_10;
      if (KEY(15) = '1') then
         next_state <= PRINT_CORRECT;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ERROR =>
      next_state <= ERROR;
      if (KEY(15) = '1') then
         next_state <= PRINT_INVALID;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
         next_state <= ERROR;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_CORRECT =>
      next_state <= PRINT_CORRECT;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_INVALID =>
      next_state <= PRINT_INVALID;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= TEST1;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= TEST1;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_INVALID =>
      FSM_MX_MEM     <= '0';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_CORRECT =>
      FSM_MX_MEM     <= '1';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;