/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "hal_data.h"
#include "Project.h"
#include <string.h>
#include <stdio.h>
#include "Serial_user.h"
#include "ASCII_numbers.h"
#include "Scheduler.h"
#include "SensorFunctions.h"
#include "SimpleKeyboard_User.h"


void R_BSP_WarmStart(bsp_warm_start_event_t event);
void i2c_master_callback(i2c_master_callback_args_t *p_args);
void sau_spi_callback(spi_callback_args_t *p_args);


//-----------------------------
// General #defines
#define SENSOR_1_SS R_PORT1->PODR_b.PODR10
#define SENSOR_2_SS R_PORT1->PODR_b.PODR12
#define SENSOR_3_SS R_PORT1->PODR_b.PODR9
#define SENSOR_3_EEPROM_SEL R_PORT1->PODR_b.PODR3
#define SENSOR_BAROMETER_SS R_PORT0->PODR_b.PODR8

#define SS_ASSERTED 0
#define SS_DEASSERTED 1



//-----------------------------
    // *** Variable Definitions ***
    // File Scope Variables
    bsp_io_level_t pin_level = BSP_IO_LEVEL_LOW; /* Holds level to set for pins */

    //sensor_data_t pressureSensor;
    sensor_data_int32_t pressureSensor1;
    sensor_data_int32_t pressureSensor2;
    sensor_data_int32_t pressureSensor3;
    se_sensor_data_t pressureSensorBarometer;

    sensor_data_t temperatureSensor;
    sensor_data_t humiditySensor;
    uint8_t nozzleNo = 12;

//    sensor_obj_t pressureSensorObj = {.pSensorData = &pressureSensor, .sineOffset = 10};
//    sensor_obj_t temperatureSensorObj = {.pSensorData = &temperatureSensor, .sineOffset = 50};
//    sensor_obj_t humiditySensorObj = {.pSensorData = &humiditySensor, .sineOffset = 90};

    const sensor_data_t initValues1 = {.byteAccess ={1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0,  8,0,0,0, 1,0, 0}};
    const sensor_data_t initValues2 = {.byteAccess ={2,0, 2,0, 2,0, 2,0, 2,0, 2,0, 2,0, 2,0, 16,0,0,0, 2,0, 0}};
    const sensor_data_t initValues3 = {.byteAccess ={3,0, 3,0, 3,0, 3,0, 3,0, 3,0, 3,0, 3,0, 24,0,0,0, 3,0, 0}};
    const sensor_data_t initValues0 = {.byteAccess ={0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,0,0, 0,0, 0}};
    const sensor_data_int32_t initValues0Int32 = {.byteAccess ={0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,0,0, 0,0,0,0, 0}};
    const se_sensor_data_t initSeValues0 = {.byteAccess ={0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,0,0, 0,0, 0}};

    uint8_t sensorStateMachine = 0;

    uint8_t testPacket[] = "$R1123:456:789:135:248:369\n";
    uint16_t testUINT16Array[6];

    uint8_t volatile flashEnabled = true;
    uint16_t volatile flashDelaySeed = 10;
    uint16_t flashCounter = 10;

    uint8_t buttonPushed = false;
    uint8_t volatile keyCode = NO_KEY_PRESSED;

    fsp_err_t status = FSP_SUCCESS;



    // variables for I2C
    fsp_err_t err     = FSP_SUCCESS;
    // Sensor
    static volatile i2c_master_event_t i2c_event = I2C_MASTER_EVENT_ABORTED;
    #define SENSOR_I2C_BUS_ADDRESS 0x44
    uint8_t sensorRegisters[6];
    const uint8_t cmdRead[2] = {0x24,0x16}; // read temp/hum in Low repeatibility and no clock stretching
    //const uint8_t cmdRead[2] = {0x24,0x0B}; // read temp/hum in Med repeatibility and no clock stretching
    uint8_t getTempHum = false;
    uint8_t getTempHumState = 0;
    uint8_t currentTempF = 40;
    uint8_t currentHum = 10;
    // EEProm
    #define EEPROM_I2C_BUS_ADDRESS 0x50
    uint8_t readEEPROM = false;
    uint8_t writeEEPROM = false;
    uint8_t readEEpromValues[8] = {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};
    uint8_t writeEEpromValues[8];
    uint8_t readWriteStartingRegister[2];
    uint8_t eepromReadWriteState = 0;
    uint8_t noBytestoRW = 4;
    volatile uint8_t i2cBusy = false;

    // SPI stuff
    // DLHR Sensors
    uint8_t spiCmdStartSingle[3] = {0xAA, 0x00, 0x00};
    uint8_t spiCmdReadValue[7] = {0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    volatile uint8_t sampleData_DLHR_1 = false;
    volatile uint8_t sampleData_DLHR_2 = false;
    volatile uint8_t sampleData_DLHR_3 = false;
    spi_cfg_t dlhr_spi0_cfg;
    sau_spi_extended_cfg_t dlhr_spi0_ext_cfg;
    // Barometric Sensor
    union {
        uint8_t byteAccess[2];
        uint16_t wordAccess;
    } bar_convert;
    uint8_t spiCmdGetDeviceID[2] = {0xE0, 0x00};
    uint8_t spiCmdGetBarometricPressure[2] = {0x20, 0x00};
    uint8_t spiCmdGetTemp[2] = {0x40, 0x00};
    volatile uint8_t sampleData_Barometer = false;
    spi_cfg_t bar_spi0_cfg;
    sau_spi_extended_cfg_t bar_spi0_ext_cfg;
    // Honeywell Sensor
    uint8_t hw_readEEPROM[2] = {0x03, 0x00};
    uint8_t hw_readEEPROM_ADC_Comp[2] = {0x03, 0x3C};
    uint8_t hw_reset_ADC[1] = {0x06};
    uint8_t hw_setupADC[2] ={0x44, 0x04};
    uint8_t hw_startADC[1] = {0x08};
    uint8_t hw_readADC[4] = {0x10, 0x00, 0x00, 0x00};
    uint8_t hw_ADC_value[4];
    uint8_t hw_EEPROM_values[16] ="";
    uint8_t hw_EEPROM_pushOut[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    uint8_t hw_ADC_COMP[5];
    uint8_t init_hw_sensor = true;
    uint8_t sampleData_HW = false;
    spi_cfg_t hw_spi0_cfg;
    sau_spi_extended_cfg_t hw_spi0_ext_cfg;
    spi_cfg_t hw_ee_spi0_cfg;
    sau_spi_extended_cfg_t hw_ee_spi0_ext_cfg;
    // General
    uint8_t spiReadSensorData[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t sensorSampleState = 0;
    volatile uint8_t busBusy_SPI = false;
    uint8_t sensorSelectState = 0;
    volatile uint8_t transferNotComplete;
    volatile uint8_t i;
    extern sau_spi_extended_cfg_t g_spi0_cfg_extend;
    volatile uint16_t fred;

    // UART Stuff
    char msg_str[50] = "";

    // Blower Stuff
    uint16_t blowerPWM = 40;


/*******************************************************************************************************************//**
 * @brief  Blinky example application
 *
 * Blinks all leds at a rate of 1 second using the software delay function provided by the BSP.
 *
 **********************************************************************************************************************/
void hal_entry (void)
{
    /* Define the units to be used with the software delay function */
    const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;

    /* Set the blink frequency (must be <= bsp_delay_units / 2) */
//    const uint32_t freq_in_hz = 1;

    /* Calculate the delay in terms of bsp_delay_units */
//    const uint32_t delay = bsp_delay_units / (freq_in_hz * 2);

    R_BSP_SoftwareDelay(2, bsp_delay_units);

    // -----------------------------------------
    // Set up peripherals
    // Give me a SysTick every 1mS
    SysTick_Config (SystemCoreClock / 1000);


    // Start the UART
    R_UARTA_Open(&g_uart0_ctrl, &g_uart0_cfg);

    // Start I2C
    err = R_IICA_MASTER_Open(&g_iica_master0_ctrl, &g_iica_master0_cfg);

    // start PWM timer
    status = R_TAU_PWM_Open(&g_timer0_ctrl, &g_timer0_cfg);
    status = R_TAU_PWM_Start(&g_timer0_ctrl);


    // Set up & Start SPI
    //status = R_SAU_SPI_Open(&g_spi0_ctrl, &g_spi0_cfg);
    #define SPI_CLK_IDLE_HIGH 0
    #define SPI_CLK_IDLE_LOW 1
    #define SPI_DATA_SAMPLE_ODD_EDGE 0
    #define SPI_DATA_SAMPLE_EVEN_EDGE 1
    memcpy(&bar_spi0_cfg, &g_spi0_cfg, sizeof(g_spi0_cfg));
    bar_spi0_cfg.clk_phase = SPI_CLK_PHASE_EDGE_ODD;
    bar_spi0_cfg.clk_polarity = SPI_CLK_POLARITY_LOW;
    memcpy(&bar_spi0_ext_cfg, &g_spi0_cfg_extend, sizeof(g_spi0_cfg_extend));
    bar_spi0_cfg.p_extend = &bar_spi0_ext_cfg;
    bar_spi0_ext_cfg.clock_phase = SPI_CLK_IDLE_LOW;
    bar_spi0_ext_cfg.data_phase = SPI_DATA_SAMPLE_ODD_EDGE;

    memcpy(&dlhr_spi0_cfg, &g_spi0_cfg, sizeof(g_spi0_cfg));
    dlhr_spi0_cfg.clk_phase = SPI_CLK_PHASE_EDGE_ODD;
    dlhr_spi0_cfg.clk_polarity = SPI_CLK_POLARITY_LOW;
    memcpy(&dlhr_spi0_ext_cfg, &g_spi0_cfg_extend, sizeof(g_spi0_cfg_extend));
    dlhr_spi0_cfg.p_extend = &dlhr_spi0_ext_cfg;
    dlhr_spi0_ext_cfg.clock_phase = SPI_CLK_IDLE_LOW; //1;
    dlhr_spi0_ext_cfg.data_phase = SPI_DATA_SAMPLE_ODD_EDGE; //0;

    memcpy(&hw_spi0_cfg, &g_spi0_cfg, sizeof(g_spi0_cfg));
    hw_spi0_cfg.clk_phase = SPI_CLK_PHASE_EDGE_EVEN;
    hw_spi0_cfg.clk_polarity = SPI_CLK_POLARITY_LOW;
    memcpy(&hw_spi0_ext_cfg, &g_spi0_cfg_extend, sizeof(g_spi0_cfg_extend));
    hw_spi0_cfg.p_extend = &hw_spi0_ext_cfg;
    hw_spi0_ext_cfg.clock_phase = SPI_CLK_IDLE_LOW; //1;
    hw_spi0_ext_cfg.data_phase = SPI_DATA_SAMPLE_EVEN_EDGE; //0;

    memcpy(&hw_ee_spi0_cfg, &g_spi0_cfg, sizeof(g_spi0_cfg));
    hw_ee_spi0_cfg.clk_phase = SPI_CLK_PHASE_EDGE_ODD;
    hw_ee_spi0_cfg.clk_polarity = SPI_CLK_POLARITY_LOW;
    memcpy(&hw_ee_spi0_ext_cfg, &g_spi0_cfg_extend, sizeof(g_spi0_cfg_extend));
    hw_ee_spi0_cfg.p_extend = &hw_ee_spi0_ext_cfg;
    hw_ee_spi0_ext_cfg.clock_phase = SPI_CLK_IDLE_LOW; //1;
    hw_ee_spi0_ext_cfg.data_phase = SPI_DATA_SAMPLE_ODD_EDGE; //0;



    // UART Hello World, so to speak
    SendString(DEFAULT_MODEL_NAME, (uint16_t)strlen(DEFAULT_MODEL_NAME), StripZeros, NoAddCRLF);
//    SendString(FW_REV_PACKET_BUILT, (uint16_t)strlen(FW_REV_PACKET_BUILT), StripZeros, NoAddCRLF);
    SendString(FW_REV__BUILT, (uint16_t)strlen(FW_REV__BUILT), StripZeros, NoAddCRLF);


    InitSensorInt32(&pressureSensor1, &initValues0Int32);
    InitSensorInt32(&pressureSensor2, &initValues0Int32);
    InitSensor(&temperatureSensor, &initValues0);
    InitSensor(&humiditySensor, &initValues0);
    InitSeSensor(&pressureSensorBarometer, &initSeValues0);

    ParseParamsToUINT16(&testPacket[3], testUINT16Array, 6);

//    // Read Honeywell EEPROM
//    while (init_hw_sensor == true){
//        if (ten_mS_Flag == true) {
//            ten_mS_Flag = false;
//
//            switch (sensorSampleState) {
//                case 0:
//                    SENSOR_3_EEPROM_SEL = SS_ASSERTED;
//                    busBusy_SPI = true;
//                    sensorSampleState++;
//                    break;
//                case 1:
//                    hw_readEEPROM[0] = 0;
//                    status = R_SAU_SPI_Write(&g_spi0_ctrl, hw_readEEPROM, 2, SPI_BIT_WIDTH_8_BITS);
//                    sensorSampleState++;
//                    break;
//                case 2:
//                    status = R_SAU_SPI_WriteRead(&g_spi0_ctrl, hw_EEPROM_pushOut, hw_EEPROM_values, 16, SPI_BIT_WIDTH_8_BITS);
//                    sensorSampleState++;
//                    break;
//                case 3:
//                    SENSOR_1_SS = SS_DEASSERTED;
//                    sensorSampleState++;
//                    break;
//                case 4:
//                    SENSOR_3_EEPROM_SEL = true;
//                    sensorSampleState = 0;
//                    busBusy_SPI = false;
//                    sensorSampleState = 0;
//                    init_hw_sensor = false;
//                    break;
//                default:
//                    break;
//            }
//        }
//    }

    status = R_SAU_SPI_Close(&g_spi0_ctrl);
    status = R_SAU_SPI_Open(&g_spi0_ctrl, &hw_ee_spi0_cfg);

    SENSOR_3_EEPROM_SEL = SS_ASSERTED;
    R_BSP_SoftwareDelay(1, bsp_delay_units);
    status = R_SAU_SPI_Write(&g_spi0_ctrl, hw_readEEPROM_ADC_Comp, 2, SPI_BIT_WIDTH_8_BITS);
    R_BSP_SoftwareDelay(1, bsp_delay_units);
    status = R_SAU_SPI_WriteRead(&g_spi0_ctrl, hw_EEPROM_pushOut, hw_EEPROM_values, 8, SPI_BIT_WIDTH_8_BITS);
    R_BSP_SoftwareDelay(1, bsp_delay_units);
    SENSOR_3_EEPROM_SEL = SS_DEASSERTED;
    R_BSP_SoftwareDelay(1, bsp_delay_units);
#if (1)
    status = R_SAU_SPI_Close(&g_spi0_ctrl);
    status = R_SAU_SPI_Open(&g_spi0_ctrl, &hw_spi0_cfg);

    SENSOR_3_SS = SS_ASSERTED;
    R_BSP_SoftwareDelay(1, bsp_delay_units);
    status = R_SAU_SPI_Write(&g_spi0_ctrl, hw_reset_ADC, 1, SPI_BIT_WIDTH_8_BITS);
    R_BSP_SoftwareDelay(1, bsp_delay_units);
    SENSOR_3_SS = SS_DEASSERTED;
    R_BSP_SoftwareDelay(5, bsp_delay_units);

    hw_ADC_COMP[0] = 0x43;
    hw_ADC_COMP[1] = hw_EEPROM_values[1];
    hw_ADC_COMP[2] = hw_EEPROM_values[3];
    hw_ADC_COMP[3] = hw_EEPROM_values[5];
    hw_ADC_COMP[4] = hw_EEPROM_values[7];

    SENSOR_3_SS = SS_ASSERTED;
    R_BSP_SoftwareDelay(1, bsp_delay_units);
    status = R_SAU_SPI_Write(&g_spi0_ctrl, hw_ADC_COMP, 5, SPI_BIT_WIDTH_8_BITS);
    R_BSP_SoftwareDelay(1, bsp_delay_units);
    SENSOR_3_SS = SS_DEASSERTED;

    status = R_SAU_SPI_Close(&g_spi0_ctrl);
    status = R_SAU_SPI_Open(&g_spi0_ctrl, &g_spi0_cfg);
#endif

    while (1)
    {
        //---------------------------------
        // 10mS Tasks
        if (ten_mS_Flag) {
          ten_mS_Flag = false;


          if (readEEPROM == true) {
              switch(eepromReadWriteState++) {
                  case 0:
                      g_iica_master0_ctrl.slave = EEPROM_I2C_BUS_ADDRESS;
                      err = R_IICA_MASTER_Write(&g_iica_master0_ctrl, (uint8_t *)&readWriteStartingRegister[0], 2, true);
                      break;
                  case 1:
                      err = R_IICA_MASTER_Read(&g_iica_master0_ctrl, &readEEpromValues[0], noBytestoRW, false);
                      break;
                  case 2:
                      readEEPROM = false;
                      eepromReadWriteState = 0;
                      break;
              }
          }


          if (writeEEPROM == true) {
              g_iica_master0_ctrl.slave = EEPROM_I2C_BUS_ADDRESS;
              err = R_IICA_MASTER_Write(&g_iica_master0_ctrl, (uint8_t *)&writeEEpromValues[0], noBytestoRW + 2, false);
              writeEEPROM = false;
          }


          if (getTempHum == true) {
              switch (getTempHumState++) {
                  case 0:
                      g_iica_master0_ctrl.slave = SENSOR_I2C_BUS_ADDRESS;
                      err = R_IICA_MASTER_Write(&g_iica_master0_ctrl, (uint8_t *)&cmdRead[0], 2, false);
                      break;
                  case 1:
                      err = R_IICA_MASTER_Read(&g_iica_master0_ctrl, &sensorRegisters[0], 6, false);
                      break;
                  case 2:
                      currentTempF = (uint8_t)(
                              (uint32_t)(
                              (uint32_t)(
                              (uint32_t)(((uint32_t)sensorRegisters[0] << 8) + sensorRegisters[1])
                              * 315)
                              / 0xFFFF)
                              - 49);

                      currentHum = (uint8_t)(
                              (uint32_t)(
                              (uint32_t)(
                              (uint32_t)(((uint32_t)sensorRegisters[3] << 8) + sensorRegisters[4])
                              *100)
                              /0xFFFF));
                      break;
                  case 3:
                      AddSensorInt16Value(&temperatureSensor, currentTempF * 10);
                      AddSensorInt16Value(&humiditySensor, currentHum * 10);
                      getTempHum = false;
                      getTempHumState = 0;
                      break;
              }
          }


          if (sampleData_DLHR_1 == true) {
              switch (sensorSampleState) {
                  case 0:
                      status = R_SAU_SPI_Close(&g_spi0_ctrl);
                      //if ((status != FSP_SUCCESS) && (status != FSP_ERR_NOT_OPEN)) while(1);
                      status = R_SAU_SPI_Open(&g_spi0_ctrl, &dlhr_spi0_cfg);
                      //if (status != FSP_SUCCESS) while(1);
                      sensorSampleState++;
                      break;
                  case 1:
                      SENSOR_1_SS = SS_ASSERTED;
                      busBusy_SPI = true;
                      sensorSampleState++;
                      break;
                  case 2:
                      status = R_SAU_SPI_Write(&g_spi0_ctrl, spiCmdStartSingle, 3, SPI_BIT_WIDTH_8_BITS);
                      sensorSampleState++;
                      break;
                  case 3:
                      SENSOR_1_SS = SS_DEASSERTED;
                      sensorSampleState++;
                      break;
                  case 4:
                      SENSOR_1_SS = SS_ASSERTED;
                      sensorSampleState++;
                      break;
                  case 5:
                      status = R_SAU_SPI_WriteRead(&g_spi0_ctrl, spiCmdReadValue, spiReadSensorData, 7, SPI_BIT_WIDTH_8_BITS);
                      sensorSampleState++;
                      break;
                  case 6:
                      SENSOR_1_SS = SS_DEASSERTED;
                      sensorSampleState++;
                      break;
                  case 7:
                      AddSensorInt32Value(&pressureSensor1, (spiReadSensorData[1] << 16) + (spiReadSensorData[2] << 8) + spiReadSensorData[3]);
                      sensorSampleState++;
                      break;
                  case 8:
                      status = R_SAU_SPI_Close(&g_spi0_ctrl);
                      //if ((status != FSP_SUCCESS) && (status != FSP_ERR_NOT_OPEN)) while(1);
                      status = R_SAU_SPI_Open(&g_spi0_ctrl, &g_spi0_cfg);
                      //if (status != FSP_SUCCESS) while(1);
                      sampleData_DLHR_1 = false;
                      busBusy_SPI = false;
                      sensorSampleState = 0;
                      break;
                  default:
                      break;
              }
          }


          if (sampleData_DLHR_2 == true) {
              switch (sensorSampleState) {
                  case 0:
                      status = R_SAU_SPI_Close(&g_spi0_ctrl);
                      if (status != FSP_SUCCESS) while(1);
                      status = R_SAU_SPI_Open(&g_spi0_ctrl, &dlhr_spi0_cfg);
                      if (status != FSP_SUCCESS) while(1);
                      sensorSampleState++;
                      break;
                  case 1:
                      SENSOR_2_SS = SS_ASSERTED;
                      busBusy_SPI = true;
                      sensorSampleState++;
                      break;
                  case 2:
                      status = R_SAU_SPI_Write(&g_spi0_ctrl, spiCmdStartSingle, 3, SPI_BIT_WIDTH_8_BITS);
                      sensorSampleState++;
                      break;
                  case 3:
                      SENSOR_2_SS = SS_DEASSERTED;
                      sensorSampleState++;
                      break;
                  case 4:
                      SENSOR_2_SS = SS_ASSERTED;
                      sensorSampleState++;
                      break;
                  case 5:
                      status = R_SAU_SPI_WriteRead(&g_spi0_ctrl, spiCmdReadValue, spiReadSensorData, 7, SPI_BIT_WIDTH_8_BITS);
                      sensorSampleState++;
                      break;
                  case 6:
                      SENSOR_2_SS = SS_DEASSERTED;
                      sensorSampleState++;
                      break;
                  case 7:
                      AddSensorInt32Value(&pressureSensor2, (spiReadSensorData[1] << 16) + (spiReadSensorData[2] << 8) + spiReadSensorData[3]);
                      sensorSampleState++;
                      break;
                  case 8:
                      status = R_SAU_SPI_Close(&g_spi0_ctrl);
                      if (status != FSP_SUCCESS) while(1);
                      status = R_SAU_SPI_Open(&g_spi0_ctrl, &g_spi0_cfg);
                      if (status != FSP_SUCCESS) while(1);
                      sampleData_DLHR_2 = false;
                      sensorSampleState = 0;
                      busBusy_SPI = false;
                      sensorSampleState = 0;
                      break;
                  default:
                      break;
              }
          }


          if (sampleData_DLHR_3 == true) {
              switch (sensorSampleState) {
                  case 0:
                      status = R_SAU_SPI_Close(&g_spi0_ctrl);
                      //if ((status != FSP_SUCCESS) && (status != FSP_ERR_NOT_OPEN)) while(1);
                      status = R_SAU_SPI_Open(&g_spi0_ctrl, &dlhr_spi0_cfg);
                      //if (status != FSP_SUCCESS) while(1);
                      sensorSampleState++;
                      break;
                  case 1:
                      SENSOR_3_SS = SS_ASSERTED;
                      busBusy_SPI = true;
                      sensorSampleState++;
                      break;
                  case 2:
                      status = R_SAU_SPI_Write(&g_spi0_ctrl, spiCmdStartSingle, 3, SPI_BIT_WIDTH_8_BITS);
                      sensorSampleState++;
                      break;
                  case 3:
                      SENSOR_3_SS = SS_DEASSERTED;
                      sensorSampleState++;
                      break;
                  case 4:
                      SENSOR_3_SS = SS_ASSERTED;
                      sensorSampleState++;
                      break;
                  case 5:
                      status = R_SAU_SPI_WriteRead(&g_spi0_ctrl, spiCmdReadValue, spiReadSensorData, 7, SPI_BIT_WIDTH_8_BITS);
                      sensorSampleState++;
                      break;
                  case 6:
                      SENSOR_3_SS = SS_DEASSERTED;
                      sensorSampleState++;
                      break;
                  case 7:
                      AddSensorInt32Value(&pressureSensor3, (spiReadSensorData[1] << 16) + (spiReadSensorData[2] << 8) + spiReadSensorData[3]);
                      sensorSampleState++;
                      break;
                  case 8:
                      status = R_SAU_SPI_Close(&g_spi0_ctrl);
                      //if ((status != FSP_SUCCESS) && (status != FSP_ERR_NOT_OPEN)) while(1);
                      status = R_SAU_SPI_Open(&g_spi0_ctrl, &g_spi0_cfg);
                      //if (status != FSP_SUCCESS) while(1);
                      sampleData_DLHR_3 = false;
                      busBusy_SPI = false;
                      sensorSampleState = 0;
                      break;
                  default:
                      break;
              }
          }


          if (sampleData_Barometer == true) {
              switch (sensorSampleState) {
                  case 0:
                      status = R_SAU_SPI_Close(&g_spi0_ctrl);
                      status = R_SAU_SPI_Open(&g_spi0_ctrl, &bar_spi0_cfg);
                      busBusy_SPI = true;
                      sensorSampleState = 1;
                       break;
                  case 1:
                      SENSOR_BAROMETER_SS = SS_ASSERTED;
                      sensorSampleState = 2;
                      break;
                  case 2:
                      status = R_SAU_SPI_WriteRead(&g_spi0_ctrl, spiCmdGetBarometricPressure, spiReadSensorData, 2, SPI_BIT_WIDTH_8_BITS);
                      sensorSampleState = 3;
                      break;
                  case 3:
                      SENSOR_BAROMETER_SS = SS_DEASSERTED;
                      sensorSampleState = 4;
                      break;
                  case 4:
                      status = R_SAU_SPI_Close(&g_spi0_ctrl);
                      //if (status != FSP_SUCCESS) while(1);
                      status = R_SAU_SPI_Open(&g_spi0_ctrl, &g_spi0_cfg);
                      //if (status != FSP_SUCCESS) while(1);
                      sensorSampleState = 5;
                      break;
                  case 5:
                      bar_convert.byteAccess[0] = spiReadSensorData[1];
                      bar_convert.byteAccess[1] = spiReadSensorData[0];
                      AddSensorUint16Value(&pressureSensorBarometer, ((bar_convert.wordAccess >> 1) & 0x0FFF));
                      sensorSampleState = 6;
                      break;
                  case 6:
                      sampleData_Barometer = false;
                      sensorSampleState = 0;
                      busBusy_SPI = false;
                      break;
                  default:
                      break;
              }
          }


//          if (sampleData_HW == true) {
//              switch (sensorSampleState) {
//                  case 0:
//                      busBusy_SPI = true;
//                      status = R_SAU_SPI_Close(&g_spi0_ctrl);
//                      status = R_SAU_SPI_Open(&g_spi0_ctrl, &hw_spi0_cfg);
//                      sensorSampleState++;
//                      break;
//                  case 1:
//                      SENSOR_3_SS = SS_ASSERTED;
//                      sensorSampleState++;
//                      break;
//                  case 2:
//                      status = R_SAU_SPI_Write(&g_spi0_ctrl, hw_setupADC, 2, SPI_BIT_WIDTH_8_BITS);
//                      sensorSampleState++;
//                      break;
//                  case 3: SENSOR_3_SS = SS_DEASSERTED;
//                      sensorSampleState++;
//                      break;
//                  case 4:
//                  case 5: // intentional fall through - 60mS delay
//                  case 6:
//                  case 7:
//                      sensorSampleState++;
//                      break;
//                  case 8:
//                      SENSOR_3_SS = SS_ASSERTED;
//                      sensorSampleState++;
//                      break;
//                  case 9:
//                      status = R_SAU_SPI_Write(&g_spi0_ctrl, hw_startADC, 1, SPI_BIT_WIDTH_8_BITS);
//                      sensorSampleState++;
//                      break;
//                  case 10:
//                      SENSOR_3_SS = SS_DEASSERTED;
//                      sensorSampleState++;
//                      break;
//                  case 11:
//                  case 12:
//                  case 13:
//                  case 14:
//                      sensorSampleState++;
//                      break;
//                  case 15:
//                      status = R_SAU_SPI_WriteRead(&g_spi0_ctrl, spiCmdGetBarometricPressure, spiReadSensorData, 4, SPI_BIT_WIDTH_8_BITS);
//                      sensorSampleState++;
//                      break;
//                  case 16: // put the data somewhere.
//                      sensorSampleState++;
//                      break;
//                  case 17:
//                      status = R_SAU_SPI_Close(&g_spi0_ctrl);
//                      status = R_SAU_SPI_Open(&g_spi0_ctrl, &g_spi0_cfg);
//                      sensorSampleState = 0;
//                      sampleData_HW = false;
//                      busBusy_SPI = false;
//                      break;
//                  default:
//                      break;
//              }
//          }

        }  // end of 10mS Tasks
        //---------------------------------


        //---------------------------------
        // 25mS Tasks
        if (twentyfive_mS_Flag) {
          twentyfive_mS_Flag = false;

          keyCode = ScanKeyboard();

          if (keyCode != NO_KEY_PRESSED) {
              if (keyPress != true) {
                  keyPress = true;
                  ProcessKeyCode(keyCode);
              }
          }
          else {
                  keyPress = false;
          }

        }  // end of 25mS Tasks
        //---------------------------------


        //---------------------------------
        // 100mS Tasks
        if (hundred_mS_Flag) {
          hundred_mS_Flag = false;

          if (busBusy_SPI == false) {
              //SendString("*", (uint16_t)1, StripZeros, AddCRLF);
              switch (sensorSelectState) {
                  case 0:  // Sensor 1
                      sampleData_DLHR_1 = true;
                      sensorSelectState = 1;
                      //SendString("0", (uint16_t)1, StripZeros, AddCRLF);
                      break;
                  case 1:  // Sensor 2
                      sampleData_DLHR_2 = true;
                      sensorSelectState = 2;
                      //SendString("1", (uint16_t)1, StripZeros, AddCRLF);
                      break;
                  case 2:  // Sensor 3
                      sampleData_DLHR_3 = true;
                      //sampleData_HW = true;
                      sensorSelectState = 3;
                      //SendString("2", (uint16_t)1, StripZeros, AddCRLF);
                      break;
                  case 3: // Barometric Sensor
                      sampleData_Barometer = true;
                      sensorSelectState = 0;
                      //SendString("3", (uint16_t)1, StripZeros, AddCRLF);
                       break;
                  default:
                      break;
              }
          }

        }  // end of 100mS Tasks
        //---------------------------------


        //---------------------------------
        // 1 Sec Tasks
        if (one_S_Flag) {
          one_S_Flag = false;

          SHIELD_MOUNTED_HEARTBEAT_LED = pin_level;

//          SHIELD_MOUNTED_LED1 = pin_level;
//          SHIELD_MOUNTED_LED2 = pin_level;
//          SHIELD_MOUNTED_LED3 = pin_level;



          pin_level ^= true;

          getTempHum = true;

        } // end of 1Sec Tasks
        //---------------------------------


        //---------------------------------
        // Every time through the loop
        // Check to see if a packet has been received
        if (processPacket == true) {
            processPacket = false;
            ProcessPacket();
        }

        // If the buffer is not empty, process a byte
        if (!RxBufferEmpty()) {
            ProcessReceiveBuffer();
        }


        if (streaming == true) {
            if (sendReport_Flag == true) {
                sendReport_Flag = false;

                switch (streamReport) {
                    case '0':    // report 0 gives the average pressure, average temperature, and average humidity
                        sprintf(msg_str, "$r0%d:%d:%d\n", pressureSensor1.data.average, temperatureSensor.data.average, humiditySensor.data.average);
                        SendString(msg_str, (uint16_t)strlen(msg_str), NoStripZeros, NoAddCRLF);
                        break;
                    case '1':    // report 1 gives the oldest pressure, average temperature, and nozzle number
                        sprintf(msg_str, "$r1%d:%d:%d\n", pressureSensor1.data.rawData[pressureSensor1.data.nextValue], temperatureSensor.data.average, nozzleNo);
                        SendString(msg_str, (uint16_t)strlen(msg_str), NoStripZeros, NoAddCRLF);
                        break;
                    case '3':    // Stream report 3 gives the average pressure1, average temperature, average humidity, and Average Barometer
                        sprintf(msg_str, "$r3%d:%d:%d:%d\n", pressureSensor1.data.average, temperatureSensor.data.average, humiditySensor.data.average, pressureSensorBarometer.data.average);
                        AddChecksum(msg_str, (uint16_t)strlen(msg_str), sizeof(msg_str));
                        SendString(msg_str, (uint16_t)strlen(msg_str), NoStripZeros, NoAddCRLF);
                        break;
                    case '4':    // report 4 gives the average pressure1-2-3, average temperature, average humidity, and Average Barometer
                        sprintf(msg_str, "$r4%d:%d:%d:%d:%d:%d\n", pressureSensor1.data.average, pressureSensor2.data.average, pressureSensor3.data.average, temperatureSensor.data.average, humiditySensor.data.average, pressureSensorBarometer.data.average);
                        AddChecksum(msg_str, (uint16_t)strlen(msg_str), sizeof(msg_str));
                        SendString(msg_str, (uint16_t)strlen(msg_str), NoStripZeros, NoAddCRLF);
                        break;
                }
            }
        }

        // end Every time through the loop
        //---------------------------------

    }
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart (bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&IOPORT_CFG_CTRL, &IOPORT_CFG_NAME);
    }
}


/*******************************************************************************************************************//**
 *  @brief      User callback function
 *  @param[in]  p_args
 *  @retval None
 **********************************************************************************************************************/
void iica_master_callback(i2c_master_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        /* capture callback event for validating the i2c transfer event*/
        i2c_event = p_args->event;
        if (i2c_event == I2C_MASTER_EVENT_TX_COMPLETE) {
            i2cBusy = false;
        }
    }
}


void sau_spi_callback(spi_callback_args_t *p_args)
{
    transferNotComplete = 0;
}


