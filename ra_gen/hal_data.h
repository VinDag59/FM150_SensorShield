/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_sau_spi.h"
#include "r_spi_api.h"
#include "r_iica_master.h"
#include "r_i2c_master_api.h"
#include "r_uarta.h"
#include "r_uart_api.h"
FSP_HEADER
/** SPI on SAU Instance. */
extern const spi_instance_t g_spi0;

/** Access the SAU_SPI instance using these structures when calling API functions directly (::p_api is not used). */
extern sau_spi_instance_ctrl_t g_spi0_ctrl;
extern const spi_cfg_t g_spi0_cfg;

/** Called by the driver when a transfer has completed or an error has occurred (Must be implemented by the user). */
#ifndef sau_spi_callback
void sau_spi_callback(spi_callback_args_t *p_args);
#endif
/* IICA Master on IICA Instance. */
extern const i2c_master_instance_t g_iica_master0;

/** Access the IICA Master instance using these structures when calling API functions directly (::p_api is not used). */
extern iica_master_instance_ctrl_t g_iica_master0_ctrl;
extern const i2c_master_cfg_t g_iica_master0_cfg;

#ifndef iica_master_callback
void iica_master_callback(i2c_master_callback_args_t *p_args);
#endif
/** UART on UARTA Instance. */
extern const uart_instance_t g_uart0;

/** Access the UARTA instance using these structures when calling API functions directly (::p_api is not used). */
extern uarta_instance_ctrl_t g_uart0_ctrl;
extern const uart_cfg_t g_uart0_cfg;
extern const uarta_extended_cfg_t g_uart0_cfg_extend;

#ifndef NULL
void NULL(uart_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
