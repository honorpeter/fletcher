-- Copyright 2018 Delft University of Technology
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.

-- This file was automatically generated by FletchGen. Modify this file
-- at your own risk.

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_misc.all;

library work;
use work.Arrow.all;
use work.Columns.all;
use work.Interconnect.all;
use work.Wrapper.all;

entity fletcher_wrapper is
  generic(
    PRIM_WIDTH                                 : natural := 64;
    PRIM_EPC                                   : natural := 8;
    BUS_ADDR_WIDTH                             : natural;
    BUS_DATA_WIDTH                             : natural;
    BUS_STROBE_WIDTH                           : natural;
    BUS_LEN_WIDTH                              : natural;
    BUS_BURST_STEP_LEN                         : natural;
    BUS_BURST_MAX_LEN                          : natural;
    ---------------------------------------------------------------------------
    INDEX_WIDTH                                : natural;
    ---------------------------------------------------------------------------
    NUM_ARROW_BUFFERS                          : natural;
    NUM_REGS                                   : natural;
    NUM_USER_REGS                              : natural;
    REG_WIDTH                                  : natural;
    ---------------------------------------------------------------------------
    TAG_WIDTH                                  : natural
  );
  port(
    acc_reset                                  : in std_logic;
    bus_clk                                    : in std_logic;
    bus_reset                                  : in std_logic;
    acc_clk                                    : in std_logic;
    ---------------------------------------------------------------------------
    mst_rreq_valid                             : out std_logic;
    mst_rreq_ready                             : in std_logic;
    mst_rreq_addr                              : out std_logic_vector(BUS_ADDR_WIDTH-1 downto 0);
    mst_rreq_len                               : out std_logic_vector(BUS_LEN_WIDTH-1 downto 0);
    ---------------------------------------------------------------------------
    mst_rdat_valid                             : in std_logic;
    mst_rdat_ready                             : out std_logic;
    mst_rdat_data                              : in std_logic_vector(BUS_DATA_WIDTH-1 downto 0);
    mst_rdat_last                              : in std_logic;
    ---------------------------------------------------------------------------
    mst_wreq_valid                             : out std_logic;
    mst_wreq_len                               : out std_logic_vector(BUS_LEN_WIDTH-1 downto 0);
    mst_wreq_addr                              : out std_logic_vector(BUS_ADDR_WIDTH-1 downto 0);
    mst_wreq_ready                             : in std_logic;
    ---------------------------------------------------------------------------
    mst_wdat_valid                             : out std_logic;
    mst_wdat_ready                             : in std_logic;
    mst_wdat_data                              : out std_logic_vector(BUS_DATA_WIDTH-1 downto 0);
    mst_wdat_strobe                            : out std_logic_vector(BUS_STROBE_WIDTH-1 downto 0);
    mst_wdat_last                              : out std_logic;
    ---------------------------------------------------------------------------
    regs_in                                    : in std_logic_vector(NUM_REGS*REG_WIDTH-1 downto 0);
    regs_out                                   : out std_logic_vector(NUM_REGS*REG_WIDTH-1 downto 0);
    regs_out_en                                : out std_logic_vector(NUM_REGS-1 downto 0)
  );
end fletcher_wrapper;

architecture Implementation of fletcher_wrapper is

  -----------------------------------------------------------------------------
  -- Hardware Accelerated Function component.
  -- This component should be implemented by the user.
  component sum is
    generic(
      PRIM_WIDTH                                 : natural;
      PRIM_EPC                                   : natural;
      TAG_WIDTH                                  : natural;
      BUS_ADDR_WIDTH                             : natural;
      INDEX_WIDTH                                : natural;
      REG_WIDTH                                  : natural
    );
    port(
      weight_cmd_weight_values_addr              : out std_logic_vector(BUS_ADDR_WIDTH-1 downto 0);
      weight_cmd_tag                             : out std_logic_vector(TAG_WIDTH-1 downto 0);
      weight_cmd_lastIdx                         : out std_logic_vector(INDEX_WIDTH-1 downto 0);
      weight_cmd_firstIdx                        : out std_logic_vector(INDEX_WIDTH-1 downto 0);
      weight_cmd_ready                           : in std_logic;
      weight_cmd_valid                           : out std_logic;
      weight_out_data                            : in std_logic_vector(PRIM_WIDTH*PRIM_EPC-1 downto 0);
      weight_out_valid                           : in std_logic;
      weight_out_ready                           : out std_logic;
      weight_out_last                            : in std_logic;
      -------------------------------------------------------------------------
      acc_reset                                  : in std_logic;
      acc_clk                                    : in std_logic;
      -------------------------------------------------------------------------
      ctrl_done                                  : out std_logic;
      ctrl_busy                                  : out std_logic;
      ctrl_idle                                  : out std_logic;
      ctrl_reset                                 : in std_logic;
      ctrl_stop                                  : in std_logic;
      ctrl_start                                 : in std_logic;
      -------------------------------------------------------------------------
      idx_first                                  : in std_logic_vector(REG_WIDTH-1 downto 0);
      idx_last                                   : in std_logic_vector(REG_WIDTH-1 downto 0);
      reg_return0                                : out std_logic_vector(REG_WIDTH-1 downto 0);
      reg_return1                                : out std_logic_vector(REG_WIDTH-1 downto 0);
      -------------------------------------------------------------------------
      reg_weight_values_addr                     : in std_logic_vector(BUS_ADDR_WIDTH-1 downto 0)
    );
  end component;
  -----------------------------------------------------------------------------

  signal s_weight_bus_rdat_data                : std_logic_vector(BUS_DATA_WIDTH-1 downto 0);
  signal uctrl_done                            : std_logic;
  signal uctrl_busy                            : std_logic;
  signal uctrl_idle                            : std_logic;
  signal uctrl_reset                           : std_logic;
  signal uctrl_stop                            : std_logic;
  signal uctrl_start                           : std_logic;
  signal uctrl_control                         : std_logic_vector(REG_WIDTH-1 downto 0);
  signal uctrl_status                          : std_logic_vector(REG_WIDTH-1 downto 0);
  signal s_weight_bus_rdat_last                : std_logic;
  signal s_weight_cmd_valid                    : std_logic;
  signal s_weight_bus_rdat_ready               : std_logic;
  signal s_weight_bus_rdat_valid               : std_logic;
  signal s_weight_bus_rreq_len                 : std_logic_vector(BUS_LEN_WIDTH-1 downto 0);
  signal s_weight_bus_rreq_ready               : std_logic;
  signal s_weight_cmd_ready                    : std_logic;
  signal s_weight_cmd_firstIdx                 : std_logic_vector(INDEX_WIDTH-1 downto 0);
  signal s_weight_cmd_lastIdx                  : std_logic_vector(INDEX_WIDTH-1 downto 0);
  signal s_weight_cmd_ctrl                     : std_logic_vector(BUS_ADDR_WIDTH-1 downto 0);
  signal s_weight_cmd_tag                      : std_logic_vector(TAG_WIDTH-1 downto 0);
  signal s_weight_out_valid                    : std_logic_vector(0 downto 0);
  signal s_weight_out_ready                    : std_logic_vector(0 downto 0);
  signal s_weight_out_last                     : std_logic_vector(0 downto 0);
  signal s_weight_out_data                     : std_logic_vector(PRIM_WIDTH*PRIM_EPC-1 downto 0);
  signal s_weight_out_dvalid                   : std_logic_vector(0 downto 0);
  signal s_weight_bus_rreq_valid               : std_logic;
  signal s_weight_bus_rreq_addr                : std_logic_vector(BUS_ADDR_WIDTH-1 downto 0);
  -----------------------------------------------------------------------------
  signal s_bsv_rreq_len                        : std_logic_vector(BUS_LEN_WIDTH-1 downto 0);
  signal s_bsv_rreq_addr                       : std_logic_vector(BUS_ADDR_WIDTH-1 downto 0);
  signal s_bsv_rreq_ready                      : std_logic_vector(0 downto 0);
  signal s_bsv_rreq_valid                      : std_logic_vector(0 downto 0);
  -----------------------------------------------------------------------------
  signal s_bsv_rdat_valid                      : std_logic_vector(0 downto 0);
  signal s_bsv_rdat_ready                      : std_logic_vector(0 downto 0);
  signal s_bsv_rdat_data                       : std_logic_vector(BUS_DATA_WIDTH-1 downto 0);
  signal s_bsv_rdat_last                       : std_logic_vector(0 downto 0);
begin
  -- ColumnReader instance generated from Arrow schema field:
  -- weight: int64 not null
  weight_read_inst: ColumnReader
    generic map (
      CFG                                      => "prim(" & integer'image(PRIM_WIDTH * PRIM_EPC) & ")",
      BUS_ADDR_WIDTH                           => BUS_ADDR_WIDTH,
      BUS_LEN_WIDTH                            => BUS_LEN_WIDTH,
      BUS_DATA_WIDTH                           => BUS_DATA_WIDTH,
      BUS_BURST_STEP_LEN                       => BUS_BURST_STEP_LEN,
      BUS_BURST_MAX_LEN                        => BUS_BURST_MAX_LEN,
      INDEX_WIDTH                              => INDEX_WIDTH
    )
    port map (
      bus_clk                                  => bus_clk,
      bus_reset                                => bus_reset,
      acc_clk                                  => acc_clk,
      acc_reset                                => acc_reset,
      cmd_valid                                => s_weight_cmd_valid,
      cmd_ready                                => s_weight_cmd_ready,
      cmd_firstIdx                             => s_weight_cmd_firstIdx,
      cmd_lastIdx                              => s_weight_cmd_lastIdx,
      cmd_ctrl                                 => s_weight_cmd_ctrl,
      cmd_tag                                  => s_weight_cmd_tag,
      out_valid                                => s_weight_out_valid,
      out_ready                                => s_weight_out_ready,
      out_last                                 => s_weight_out_last,
      out_data                                 => s_weight_out_data,
      out_dvalid                               => s_weight_out_dvalid,
      bus_rreq_valid                           => s_weight_bus_rreq_valid,
      bus_rreq_ready                           => s_weight_bus_rreq_ready,
      bus_rreq_addr                            => s_weight_bus_rreq_addr,
      bus_rreq_len                             => s_weight_bus_rreq_len,
      bus_rdat_valid                           => s_weight_bus_rdat_valid,
      bus_rdat_ready                           => s_weight_bus_rdat_ready,
      bus_rdat_data                            => s_weight_bus_rdat_data,
      bus_rdat_last                            => s_weight_bus_rdat_last
    );

  -- Controller instance.
  UserCoreController_inst: UserCoreController
    generic map (
      REG_WIDTH                                => REG_WIDTH
    )
    port map (
      acc_clk                                  => acc_clk,
      acc_reset                                => acc_reset,
      bus_clk                                  => bus_clk,
      bus_reset                                => bus_reset,
      status                                   => regs_out(2*REG_WIDTH-1 downto REG_WIDTH),
      control                                  => regs_in(REG_WIDTH-1 downto 0),
      start                                    => uctrl_start,
      stop                                     => uctrl_stop,
      reset                                    => uctrl_reset,
      idle                                     => uctrl_idle,
      busy                                     => uctrl_busy,
      done                                     => uctrl_done
    );

  -- Hardware Accelerated Function instance.
  sum_inst: sum
    generic map (
      PRIM_WIDTH                               => PRIM_WIDTH,
      PRIM_EPC                                 => PRIM_EPC,
      TAG_WIDTH                                => TAG_WIDTH,
      BUS_ADDR_WIDTH                           => BUS_ADDR_WIDTH,
      INDEX_WIDTH                              => INDEX_WIDTH,
      REG_WIDTH                                => REG_WIDTH
    )
    port map (
      acc_clk                                  => acc_clk,
      acc_reset                                => acc_reset,
      ctrl_start                               => uctrl_start,
      ctrl_stop                                => uctrl_stop,
      ctrl_reset                               => uctrl_reset,
      ctrl_idle                                => uctrl_idle,
      ctrl_busy                                => uctrl_busy,
      ctrl_done                                => uctrl_done,
      weight_out_valid                         => s_weight_out_valid(0),
      weight_out_ready                         => s_weight_out_ready(0),
      weight_out_last                          => s_weight_out_last(0),
      weight_out_data                          => s_weight_out_data(PRIM_WIDTH*PRIM_EPC-1 downto 0),
      weight_cmd_valid                         => s_weight_cmd_valid,
      weight_cmd_ready                         => s_weight_cmd_ready,
      weight_cmd_firstIdx                      => s_weight_cmd_firstIdx(INDEX_WIDTH-1 downto 0),
      weight_cmd_lastIdx                       => s_weight_cmd_lastIdx(INDEX_WIDTH-1 downto 0),
      weight_cmd_tag                           => s_weight_cmd_tag(TAG_WIDTH-1 downto 0),
      weight_cmd_weight_values_addr            => s_weight_cmd_ctrl(BUS_ADDR_WIDTH-1 downto 0),
      idx_first                                => regs_in(5*REG_WIDTH-1 downto 4*REG_WIDTH),
      idx_last                                 => regs_in(6*REG_WIDTH-1 downto 5*REG_WIDTH),
      reg_return0                              => regs_out(3*REG_WIDTH-1 downto 2*REG_WIDTH),
      reg_return1                              => regs_out(4*REG_WIDTH-1 downto 3*REG_WIDTH),
      reg_weight_values_addr                   => regs_in(8*REG_WIDTH-1 downto 6*REG_WIDTH)
    );

  -- Arbiter instance generated to serve 1 column readers.
  BusReadArbiterVec_inst: BusReadArbiterVec
    generic map (
      BUS_ADDR_WIDTH                           => BUS_ADDR_WIDTH,
      BUS_LEN_WIDTH                            => BUS_LEN_WIDTH,
      BUS_DATA_WIDTH                           => BUS_DATA_WIDTH,
      NUM_SLAVE_PORTS                          => 1
    )
    port map (
      bus_clk                                  => bus_clk,
      bus_reset                                => bus_reset,
      bsv_rreq_valid                           => s_bsv_rreq_valid,
      bsv_rreq_ready                           => s_bsv_rreq_ready,
      bsv_rreq_addr                            => s_bsv_rreq_addr,
      bsv_rreq_len                             => s_bsv_rreq_len,
      bsv_rdat_valid                           => s_bsv_rdat_valid,
      bsv_rdat_ready                           => s_bsv_rdat_ready,
      bsv_rdat_data                            => s_bsv_rdat_data,
      bsv_rdat_last                            => s_bsv_rdat_last,
      mst_rreq_valid                           => mst_rreq_valid,
      mst_rreq_ready                           => mst_rreq_ready,
      mst_rreq_addr                            => mst_rreq_addr,
      mst_rreq_len                             => mst_rreq_len,
      mst_rdat_valid                           => mst_rdat_valid,
      mst_rdat_ready                           => mst_rdat_ready,
      mst_rdat_data                            => mst_rdat_data,
      mst_rdat_last                            => mst_rdat_last
    );


  s_bsv_rreq_addr(BUS_ADDR_WIDTH-1 downto 0)   <= s_weight_bus_rreq_addr;
  s_bsv_rreq_len(BUS_LEN_WIDTH-1 downto 0)     <= s_weight_bus_rreq_len;
  s_weight_bus_rreq_ready                      <= s_bsv_rreq_ready(0);
  s_bsv_rreq_valid(0)                          <= s_weight_bus_rreq_valid;
  -----------------------------------------------------------------------------
  s_weight_bus_rdat_data                       <= s_bsv_rdat_data(BUS_DATA_WIDTH-1 downto 0);
  s_weight_bus_rdat_last                       <= s_bsv_rdat_last(0);
  s_bsv_rdat_ready(0)                          <= s_weight_bus_rdat_ready;
  s_weight_bus_rdat_valid                      <= s_bsv_rdat_valid(0);
  -----------------------------------------------------------------------------
  mst_wdat_valid                               <='0';
  mst_wreq_valid                               <='0';
  regs_out_en(0)                               <='0';
  regs_out_en(1)                               <='1';
  regs_out_en(2)                               <='1';
  regs_out_en(3)                               <='1';

end architecture;

