/*
 * Serial_user.c
 *
 *  Created on: 25JUN2023
 *      Author: VIn D'Agostino, D'Agostino Industries Group, Inc.
 */

/***********************************************************************************************************************
 * Copyright [2020-2023] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * This file is used with the contents of the "Serial_ASCII" module which is supplied for Renesas use by
 * D'Agostino Industries Group, Inc. It is part of a simple serial interface management system. "<module>_user" files
 * (.c and .h) are intended to be modified on an application by application basis, and <module> files (.c and .h) are
 * intended to be used as provided. <module_user.h is the only file required to be included in referencing files.
 **********************************************************************************************************************/

#include "Project.h"
#include "Serial.h"
#include "Serial_user.h"
#include "ASCII_numbers.h"
#include <string.h>
#include <stdio.h>
#include "SensorFunctions.h"
#include "r_tau_pwm.h"
#include "r_timer_api.h"



// packet management
uint8_t packetBuffer[16];
uint8_t inPacket = false;
uint8_t nextPacketChar = 0;
uint8_t processPacket = false;

uint8_t processBinaryPacket = false;
int16_t packetLength = -1;
uint8_t escapeDetected = false;

extern volatile uint8_t rxBuffer[RX_BUFFER_SIZE];

extern volatile uint8_t flashEnabled;
extern volatile uint16_t flashDelaySeed;

extern sensor_data_int32_t pressureSensor1;
extern sensor_data_int32_t pressureSensor2;
extern sensor_data_int32_t pressureSensor3;
extern se_sensor_data_t pressureSensorBarometer;
extern sensor_data_t temperatureSensor;
extern sensor_data_t humiditySensor;
extern uint8_t nozzleNo;
extern uint8_t buttonPushed;
extern uint16_t blowerPWM;
extern tau_pwm_instance_ctrl_t g_timer0_ctrl;


//Comm Control variables
uint8_t streaming = false;
uint8_t streamReport = 0;
uint16_t streamIntervalDelay = 1000;  // default is 1 Sec (1000mS)

// Process Variables from other modules/code



// function to process the input buffer with an ASCII-based protocol
// to be placed in main loop and called of the buffer contains received characters
// not yet processed
// arguments: none
// returns: error code: 0 = no error (default)
uint8_t ProcessReceiveBuffer(void)
{
//  SendString((char const *)&rxBuffer[nextSerialRx2Proc], 1, StripZeros, NoAddCRLF);
  if (rxBuffer[nextSerialRx2Proc] == '$') {
    inPacket = true;
    packetBuffer[0] = rxBuffer[nextSerialRx2Proc];
    nextPacketChar = 1;
  }
  else {
    if (inPacket == true) {
        packetBuffer[nextPacketChar++] = rxBuffer[nextSerialRx2Proc];

        if (rxBuffer[nextSerialRx2Proc] == '\n') {
          processPacket = true;
          inPacket = false;
        }
    }
  }
  
  if (++nextSerialRx2Proc >= RX_BUFFER_SIZE) {
    nextSerialRx2Proc = 0;
  }

  return 0;
}


// Packet processing function. packet form: "$<cmd>[<parameter>]\n"
// for simplicity, commands are single characters.
// Each command is a case in a switch statement. Upper or lower case characters are valid.
uint8_t ProcessPacket(void)
{
    uint8_t errorCode = 0;
    uint16_t tempValue;
    timer_info_t info;
    char message[50];


    switch (packetBuffer[COMMAND_LOCATION]) {
    // list of commands
    // each command has intentional fall-thru for upper/lower case
    case 's':     // s = turn streaming on/off for a report
    case 'S':
        if (packetBuffer[PARAMETER_START_LOCATION] == '1') {
            streaming = true;
            streamReport = packetBuffer[PARAMETER_START_LOCATION + 1];// - '0';
        }
        else {
            streaming = false;
        }
        break;
    case 'i': // Streaming interval in mS from 200-2000
    case 'I':
        errorCode = ConvertASCII2UINT16((char*)&packetBuffer[PARAMETER_START_LOCATION], 5, '\n', &tempValue);
        if ((tempValue <= 2000) && (tempValue >= 200)) {
            streamIntervalDelay = tempValue;
            sprintf(message, "$s%d\n", streamIntervalDelay);
            SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
        }
        break;
    case 'p':
    case 'P':
        // units for blower PWM (blowerPWM) are 0.1%, so, 400 = 40%
        errorCode = ConvertASCII2UINT16((char*)&packetBuffer[PARAMETER_START_LOCATION], 5, '\n', &tempValue);
        if ((tempValue >= 400) && (tempValue <= 980)) {
            blowerPWM = tempValue;

           // change the value of the PWM
           /* Get the current period setting. */
           R_TAU_PWM_InfoGet(&g_timer0_ctrl, &info);
           uint32_t current_period_counts = info.period_counts;
           /* Calculate the desired duty cycle based on the current period. */
           uint16_t duty_cycle_counts = (uint16_t) ((current_period_counts * blowerPWM) / 1000);
           /* Set the calculated duty cycle. */
           R_TAU_PWM_DutyCycleSet(&g_timer0_ctrl, duty_cycle_counts, TAU_PWM_IO_PIN_CHANNEL_5);
        }
       sprintf(message, "$p%d\n", blowerPWM);
       SendString(message, (uint16_t)strlen(message), StripZeros, NoAddCRLF);
        break;
    case 'r':
    case 'R':
        switch (packetBuffer[PARAMETER_START_LOCATION]) {
            case '0':    // report 0 gives the average pressure, average temperature, and average humidity
                sprintf(message, "$r0%d:%d:%d\n", pressureSensor1.data.average, temperatureSensor.data.average, humiditySensor.data.average);
                AddChecksum(message, (uint16_t)strlen(message), sizeof(message));
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '1':    // report 1 gives the oldest pressure, average temperature, and nozzle number
                sprintf(message, "$r1%d:%d:%d\n", pressureSensor1.data.rawData[pressureSensor1.data.nextValue], temperatureSensor.data.average, nozzleNo);
                AddChecksum(message, (uint16_t)strlen(message), sizeof(message));
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '2':    // report 1 gives the oldest pressure, average temperature, and nozzle number
                sprintf(message, "$r2%d:%d:%d:%d\n", pressureSensor1.data.rawData[pressureSensor1.data.nextValue], temperatureSensor.data.average, humiditySensor.data.average, pressureSensorBarometer.data.average);
                AddChecksum(message, (uint16_t)strlen(message), sizeof(message));
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '3':    // report 3 gives the average pressure1, average temperature, average humidity, and Average Barometer
                sprintf(message, "$r3%d:%d:%d:%d\n", pressureSensor1.data.average, temperatureSensor.data.average, humiditySensor.data.average, pressureSensorBarometer.data.average);
                AddChecksum(message, (uint16_t)strlen(message), sizeof(message));
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '4':    // report 4 gives the average pressure1-2-3, average temperature, average humidity, and Average Barometer
                sprintf(message, "$r4%d:%d:%d:%d:%d:%d\n", pressureSensor1.data.average, pressureSensor2.data.average, pressureSensor3.data.average, temperatureSensor.data.average, humiditySensor.data.average, pressureSensorBarometer.data.average);
                AddChecksum(message, (uint16_t)strlen(message), sizeof(message));
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
        }
        break;
    case 'b':     // b = check switch (button) press
    case 'B':
        if (buttonPushed == true) {
            SendString("Pushed", 6, StripZeros, AddCRLF);
            SendString("$B1\n", 6, StripZeros, AddCRLF);
            buttonPushed = false;
        }
        else {
            SendString("Not Pushed", 10, StripZeros, AddCRLF);
            SendString("$B0\n", 6, StripZeros, AddCRLF);
       }
        break;
    case 'v':
    case 'V': // individual sensor reports
        switch (packetBuffer[PARAMETER_START_LOCATION]) {
            case '0':
                sprintf(message, "$v0%d\n", pressureSensorBarometer.data.average);
                AddChecksum(message, (uint16_t)strlen(message), sizeof(message));
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '1':
                sprintf(message, "$v1%d\n", pressureSensor1.data.average);
                AddChecksum(message, (uint16_t)strlen(message), sizeof(message));
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '2':
                sprintf(message, "$v1%d\n", pressureSensor2.data.average);
                AddChecksum(message, (uint16_t)strlen(message), sizeof(message));
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '3':
                break;
            case '4':
                sprintf(message, "$v4%d\n", temperatureSensor.data.average);
                AddChecksum(message, (uint16_t)strlen(message), sizeof(message));
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '5':
                sprintf(message, "$v5%d\n", humiditySensor.data.average);
                AddChecksum(message, (uint16_t)strlen(message), sizeof(message));
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
        }
        break;
    case 'l':
    case 'L': // individual sensor reports - last read value
        switch (packetBuffer[PARAMETER_START_LOCATION]) {
            case '0':
                sprintf(message, "$l0%d\n", pressureSensorBarometer.data.rawData[pressureSensorBarometer.data.lastValue]);
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '1':
                sprintf(message, "$1%d\n", pressureSensor1.data.rawData[pressureSensor1.data.lastValue]);
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '2':
                sprintf(message, "$1%d\n", pressureSensor2.data.rawData[pressureSensor1.data.lastValue]);
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '3':
                break;
            case '4':
                sprintf(message, "$l4%d\n", temperatureSensor.data.rawData[temperatureSensor.data.lastValue]);
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '5':
                sprintf(message, "$l5%d\n", humiditySensor.data.rawData[humiditySensor.data.lastValue]);
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
        }
        break;
    case 'a':
    case 'A': // individual sensor reports - average
        switch (packetBuffer[PARAMETER_START_LOCATION]) {
            case '0':
                sprintf(message, "$a0%d\n", pressureSensorBarometer.data.average);
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '1':
                sprintf(message, "$a%d\n", pressureSensor1.data.average);
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '2':
                sprintf(message, "$a%d\n", pressureSensor2.data.average);
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '3':
                break;
            case '4':
                sprintf(message, "$a4%d\n", temperatureSensor.data.average);
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '5':
                sprintf(message, "$a5%d\n", humiditySensor.data.average);
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
        }
        break;
    case 'f':
        SendString(FW_REV_PACKET_BUILT, (uint16_t)strlen(FW_REV_PACKET_BUILT), StripZeros, NoAddCRLF);
        break;
    }

    processPacket = false;

    return errorCode;
}


uint8_t AddChecksum(char * _msg, uint16_t _len, uint16_t _arr_size)
{
    uint8_t status = 0;
    uint8_t i = 0;
    uint8_t chk = 0;
    char chk_str[3] = "";

    if (_msg != 0) {
        while ( (_msg[i] != '\n') && (_msg[i] != 0) && (i < _len) ) {
        chk ^= _msg[i];
        i++;
        }
        if ( (_msg[i] != 0) && (i < _arr_size - 4) ) {
            sprintf(chk_str, "%X", chk);
            _msg[i++] = '!';
            _msg[i++] = chk_str[0];
            _msg[i++] = chk_str[1];
            _msg[i] = '\n';
        }
        else {
            status = 1;
        }
    }
    else {
        status = 2;
    }

    return status;
}



uint8_t CheckChecksum(char * _msg, uint16_t _len, uint16_t _arr_size)
{
    uint8_t status = 0;
    uint8_t i = 0;
    uint8_t chk = 0;
    char chk_str[3] = "";

    if (_msg != 0) {
        while ( (_msg[i] != '\n') && (_msg[i] != 0) && (_msg[i] != '!') && (i < _arr_size) ) {
        chk ^= _msg[i];
        i++;
        }

        if ( (_msg[i] == '!') && (i < _arr_size - 2) ) {
            sprintf(chk_str, "%X", chk);
            if ( (_msg[i+1] != chk_str[0]) || (_msg[i+2] != chk_str[1]) ) {
                status = 1;;
            }
        }
    }
    else {
        status = 2;
    }

    return status;
}
