/* generated HAL source file - do not edit */
#include "hal_data.h"
const tau_pwm_channel_cfg_t g_timer_channel_cfg5 =
{ .channel = 5,
#define OPERATION_TIMER_MODE_ONE_SHOT (0xFFFFFFFF)
#if (OPERATION_TIMER_MODE_ONE_SHOT == OPERATION_TIMER_MODE_PWM)
    /* Actual pulse width: 1.04856 seconds. */  .duty_cycle_counts = (uint16_t) 0xffff,
#else
  /* Actual duty cycle percent: 40 %. */.duty_cycle_counts = (uint16_t) 0x32,
#endif
#undef OPERATION_TIMER_MODE_ONE_SHOT
  .output_level = TAU_PWM_OUTPUT_LEVEL_LOW,
  .output_polarity = TAU_PWM_OUTPUT_POLARITY_ACTIVE_HIGH, .cycle_end_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_TAU0_TMI05)
    .cycle_end_irq       = VECTOR_NUMBER_TAU0_TMI05,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
tau_pwm_instance_ctrl_t g_timer0_ctrl;

const tau_pwm_extended_cfg_t g_timer0_extend =
{ .operation_clock = TAU_PWM_OPERATION_CLOCK_CK00,
#define TRIGGER_TAU_PWM_SOURCE_PIN_INPUT (0xFFFFFFFF)
#if (TRIGGER_TAU_PWM_SOURCE_PIN_INPUT == TRIGGER_TAU_PWM_SOURCE_PIN_INPUT)
  .trigger_source = TAU_PWM_SOURCE_PIN_INPUT,
#else
    .trigger_source                  = TAU_PWM_SOURCE_ELC_EVENT,
#endif
#undef TRIGGER_TAU_PWM_SOURCE_PIN_INPUT
  .detect_edge = TAU_PWM_DETECT_EDGE_FALLING,
  .p_slave_channel_cfgs =
  {
#define RA_NOT_DEFINED (0xFFFFFFFF)
#if (RA_NOT_DEFINED != 5)
    &g_timer_channel_cfg5,
#endif
#if (RA_NOT_DEFINED != RA_NOT_DEFINED)
    &g_timer_channel_cfgRA_NOT_DEFINED,
#endif

#if (RA_NOT_DEFINED != RA_NOT_DEFINED)
    &g_timer_channel_cfgRA_NOT_DEFINED,
#endif

#if (RA_NOT_DEFINED != RA_NOT_DEFINED)
    &g_timer_channel_cfgRA_NOT_DEFINED,
#endif

#if (RA_NOT_DEFINED != RA_NOT_DEFINED)
    &g_timer_channel_cfgRA_NOT_DEFINED,
#endif

#if (RA_NOT_DEFINED != RA_NOT_DEFINED)
    &g_timer_channel_cfgRA_NOT_DEFINED,
#endif

#if (RA_NOT_DEFINED != RA_NOT_DEFINED)
    &g_timer_channel_cfgRA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
          } };
const timer_cfg_t g_timer0_cfg =
{ .mode = TIMER_MODE_PWM,
/* Actual pulse period: 0.002 seconds. */.period_counts = (uint32_t) 0x7d,
  .source_div = (timer_source_div_t) BSP_CFG_TAU_CK00, .channel = 0, .p_callback = NULL,
  /** If NULL then do not add & */
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = (void*) &NULL,
#endif
  .p_extend = &g_timer0_extend,
  .cycle_end_ipl = (2),
#if defined(VECTOR_NUMBER_TAU0_TMI00)
    .cycle_end_irq       = VECTOR_NUMBER_TAU0_TMI00,
#else
  .cycle_end_irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const timer_instance_t g_timer0 =
{ .p_ctrl = &g_timer0_ctrl, .p_cfg = &g_timer0_cfg, .p_api = &g_timer_on_tau_pwm };
#include "r_sau_spi_cfg.h"
sau_spi_instance_ctrl_t g_spi0_ctrl;
#if SAU_SPI_CFG_DTC_SUPPORT_ENABLE
transfer_info_t RA_NOT_DEFINED_info[2] =
{
    { .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
      .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
      .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
      .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_EACH,
      .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
      .transfer_settings_word_b.size = TRANSFER_SIZE_1_BYTE,
      .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
      .p_dest = (void*) NULL,
      .p_src = (void const*) NULL,
      .num_blocks = 0,
      .length = 0, },
    { .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
      .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
      .transfer_settings_word_b.irq = TRANSFER_IRQ_END,
      .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
      .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
      .transfer_settings_word_b.size = TRANSFER_SIZE_1_BYTE,
      .transfer_settings_word_b.mode = TRANSFER_MODE_NORMAL,
      .p_dest = (void*) NULL,
      .p_src = (void const*) NULL,
      .num_blocks = 0,
      .length = 0, }
};
const transfer_cfg_t RA_NOT_DEFINED_cfg_sau_spi =
{
  .p_info              = RA_NOT_DEFINED_info,
  .p_extend = &RA_NOT_DEFINED_cfg_extend, };

/* Instance structure to use this module. */
const transfer_instance_t RA_NOT_DEFINED_sau_spi =
{
    .p_ctrl        = &RA_NOT_DEFINED_ctrl,
    .p_cfg         = &RA_NOT_DEFINED_cfg_sau_spi,
    .p_api         = &g_transfer_on_dtc
};

#endif
/** SPI extended configuration */
const sau_spi_extended_cfg_t g_spi0_cfg_extend =
{ .clk_div =
{
/* Actual calculated bitrate: 500000 */
.stclk = 15,
  .operation_clock = SAU_SPI_OPERATION_CLOCK_CK0, },
  .transfer_mode = SAU_SPI_TRANSFER_MODE_SINGLE, .data_phase = SAU_SPI_DATA_PHASE_START, .clock_phase =
          SAU_SPI_CLOCK_PHASE_REVERSE,
  .sau_unit = 0,
#if defined(PIN_SCK11)
    .sck_pin_settings.pin = PIN_SCK11,
#else
  .sck_pin_settings.pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
#if defined(CFG_SCK11)
    .sck_pin_settings.cfg = CFG_SCK11,
#else
  .sck_pin_settings.cfg = (uint32_t) IOPORT_CFG_PORT_DIRECTION_INPUT,
#endif
#if defined(PIN_SO11)
    .so_pin_settings.pin = PIN_SO11,
#else
  .so_pin_settings.pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
#if defined(CFG_SO11)
    .so_pin_settings.cfg = CFG_SO11,
#else
  .so_pin_settings.cfg = (uint32_t) IOPORT_CFG_PORT_DIRECTION_INPUT,
#endif
        };

const spi_cfg_t g_spi0_cfg =
{ .channel = 3,
  .operating_mode = SPI_MODE_MASTER,
  .bit_order = SPI_BIT_ORDER_MSB_FIRST,
  .p_callback = sau_spi_callback,
  .p_context = NULL,
#if defined(VECTOR_NUMBER_SAU0_SPI_TXRXI11)
    .tei_irq         = VECTOR_NUMBER_SAU0_SPI_TXRXI11,
#else
  .tei_irq = FSP_INVALID_VECTOR,
#endif
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_tx = NULL,
#else
    .p_transfer_tx   = &RA_NOT_DEFINED_sau_spi,
#endif
#undef RA_NOT_DEFINED
  .tei_ipl = (2),
  .p_extend = &g_spi0_cfg_extend, };
/* Instance structure to use this module. */
const spi_instance_t g_spi0 =
{ .p_ctrl = &g_spi0_ctrl, .p_cfg = &g_spi0_cfg, .p_api = &g_spi_on_sau };
iica_master_instance_ctrl_t g_iica_master0_ctrl;
const iica_master_extended_cfg_t g_iica_master0_extend =
{ .clock_settings.digital_filter = 0,
/* Actual calculated bitrate: 99379. IICWH0 duty cycle: 53. IICWL0 duty cycle: 47. */.clock_settings.iicwl_value = 76,
  .clock_settings.iicwh_value = 85, .clock_settings.cks_value = 1,
#if defined(PIN_SDAA0)
    .sda_pin_settings.pin = PIN_SDAA0,
#else
  .sda_pin_settings.pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
#if defined(CFG_SDAA0)
    .sda_pin_settings.cfg = CFG_SDAA0 | (uint32_t) IOPORT_PERIPHERAL_PIN_IO,
#else
  .sda_pin_settings.cfg = (uint32_t) IOPORT_CFG_PORT_DIRECTION_INPUT,
#endif
#if defined(PIN_SCLA0)
    .scl_pin_settings.pin = PIN_SCLA0,
#else
  .scl_pin_settings.pin = (bsp_io_port_pin_t) UINT16_MAX,
#endif
#if defined(CFG_SCLA0)
    .scl_pin_settings.cfg = CFG_SCLA0 | (uint32_t) IOPORT_PERIPHERAL_PIN_IO,
#else
  .scl_pin_settings.cfg = (uint32_t) IOPORT_CFG_PORT_DIRECTION_INPUT,
#endif
        };
const i2c_master_cfg_t g_iica_master0_cfg =
{ .channel = 0, .rate = I2C_MASTER_RATE_STANDARD, .slave = 0x00, .addr_mode = I2C_MASTER_ADDR_MODE_7BIT, .p_callback =
          iica_master_callback,
  .p_context = NULL,
#if defined(VECTOR_NUMBER_IICA0_TXRXI)
    .tei_irq             = VECTOR_NUMBER_IICA0_TXRXI,
#else
  .tei_irq = FSP_INVALID_VECTOR,
#endif
  .ipl = (2),
  .p_extend = &g_iica_master0_extend, };
/* Instance structure to use this module. */
const i2c_master_instance_t g_iica_master0 =
{ .p_ctrl = &g_iica_master0_ctrl, .p_cfg = &g_iica_master0_cfg, .p_api = &g_iica_master_on_iica };
uarta_instance_ctrl_t g_uart0_ctrl;

uarta_baud_setting_t g_uart0_baud_setting =
{
#if (BSP_CFG_UARTA0_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_MAIN_OSC)

  /* Baud rate calculated with Acutal_Error 0.22%. */
  /* The permissible baud rate error range during reception: -4.71% ~ 5.20% */
  .utanck_clock_b.utasel = UARTA_CLOCK_SOURCE_MOSC,
  .utanck_clock_b.utanck = UARTA_CLOCK_DIV_1, .brgca = 87, .delay_time = 1
#elif (BSP_CFG_UARTA0_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_HOCO)

/* Baud rate calculated with Acutal_Error 0.08%. */
/* The permissible baud rate error range during reception: -4.73% ~ 5.22% */
  .utanck_clock_b.utasel = UARTA_CLOCK_SOURCE_HOCO
, .utanck_clock_b.utanck = UARTA_CLOCK_DIV_1
, .brgca = 139
, .delay_time = 1
 #elif (BSP_CFG_UARTA0_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_MOCO)

/* Baud rate calculated with Acutal_Error 2.12%. */ 
/* The permissible baud rate error range during reception: -4.49% ~ 4.94% */
  .utanck_clock_b.utasel = UARTA_CLOCK_SOURCE_MOCO
, .utanck_clock_b.utanck = UARTA_CLOCK_DIV_1
, .brgca = 17
, .delay_time = 1
 #elif ((BSP_CFG_UARTA0_CLOCK_SOURCE == BSP_CFG_FSXP_SOURCE) || (BSP_CFG_UARTA0_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_LOCO))

/* Baud rate calculated with Acutal_Error 100%. */
/* The permissible baud rate error range during reception: Invalid Range Error */
  .utanck_clock_b.utasel = 0
, .utanck_clock_b.utanck = 0
, .brgca = 0
, .delay_time = 31
 #elif (BSP_CFG_UARTA0_CLOCK_SOURCE == BSP_CLOCKS_SOURCE_CLOCK_SUBCLOCK)

/* Baud rate calculated with Acutal_Error 100%. */
/* The permissible baud rate error range during reception: Invalid Range Error */
  .utanck_clock_b.utasel = 0
, .utanck_clock_b.utanck = 0
, .brgca = 0
, .delay_time = 31
 #endif
        };

/** UART extended configuration for UART on UARTA HAL driver */
const uarta_extended_cfg_t g_uart0_cfg_extend =
{ .transfer_dir = UARTA_DIR_BIT_LSB_FIRST, .transfer_level = UARTA_ALV_BIT_POSITIVE_LOGIC, .clock_output =
          UARTA_CLOCK_OUTPUT_DISABLED,
  .p_baud_setting = &g_uart0_baud_setting, };

/** UART interface configuration */
const uart_cfg_t g_uart0_cfg =
{ .channel = 0, .data_bits = UART_DATA_BITS_8, .parity = UART_PARITY_OFF, .stop_bits = UART_STOP_BITS_1, .p_callback =
          UartA_CB,
  .p_context = NULL, .p_extend = &g_uart0_cfg_extend,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_tx = NULL,
#else
                .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
  .p_transfer_rx = NULL,
#else
                .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
  .rxi_ipl = (2),
  .txi_ipl = (2), .eri_ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_UARTA0_RXI)
                .rxi_irq             = VECTOR_NUMBER_UARTA0_RXI,
#else
  .rxi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_UARTA0_TXI)
                .txi_irq             = VECTOR_NUMBER_UARTA0_TXI,
#else
  .txi_irq = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_UARTA0_ERRI)
                .eri_irq             = VECTOR_NUMBER_UARTA0_ERRI,
#else
  .eri_irq = FSP_INVALID_VECTOR,
#endif
        };

/* Instance structure to use this module. */
const uart_instance_t g_uart0 =
{ .p_ctrl = &g_uart0_ctrl, .p_cfg = &g_uart0_cfg, .p_api = &g_uart_on_uarta };
void g_hal_init(void)
{
    g_common_init ();
}
