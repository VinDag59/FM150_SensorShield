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

extern sensor_data_t pressureSensor;
extern sensor_data_t temperatureSensor;
extern sensor_data_t humiditySensor;
extern uint8_t nozzleNo;
extern uint8_t buttonPushed;

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
    char message[50];


    switch (packetBuffer[COMMAND_LOCATION]) {
    // list of commands
    // each command has intentional fall-thru for upper/lower case
    case 'f':     // r = turn on LED
    case 'F':
        flashEnabled = (packetBuffer[PARAMETER_START_LOCATION] == '1') ? true : false;
        SendString("$f\n", strlen("$f\n"), NoStripZeros, NoAddCRLF);
        break;
    case 's':     // s = adjust the flash speed
    case 'S':
        errorCode = ConvertASCII2UINT16((char*)&packetBuffer[PARAMETER_START_LOCATION], 5, '\n', &tempValue);
        if ((tempValue <= MAX_TIME) && (tempValue >= MIN_TIME)) {
            flashDelaySeed = tempValue;
            sprintf(message, "$s%d\n", flashDelaySeed);
            SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
        }
        break;
    case 'n':     // n<sensor> = new sensor value
    case 'N':
        switch (packetBuffer[PARAMETER_START_LOCATION]) {
        case '0': // pressure sensor
            errorCode = ConvertASCII2UINT16((char*)&packetBuffer[PARAMETER_START_LOCATION +1], 5, '\n', &tempValue);
            if (errorCode == 0) {
               AddSensorUint16Value(&pressureSensor, tempValue);
               sprintf(message, "$n0%d\n", tempValue);
               SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
            }
            break;
        case '1': // temperature sensor
            errorCode = ConvertASCII2UINT16((char*)&packetBuffer[PARAMETER_START_LOCATION +1], 5, '\n', &tempValue);
            if (errorCode == 0) {
               AddSensorUint16Value(&temperatureSensor, tempValue);
               sprintf(message, "$n0%d\n", tempValue);
               SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
            }
            break;
        case '3': // humidity sensor
            errorCode = ConvertASCII2UINT16((char*)&packetBuffer[PARAMETER_START_LOCATION +1], 5, '\n', &tempValue);
            if (errorCode == 0) {
               AddSensorUint16Value(&humiditySensor, tempValue);
               sprintf(message, "$n0%d\n", tempValue);
               SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
            }
            break;
        }
        break;
    case 'r':
    case 'R':
        switch (packetBuffer[PARAMETER_START_LOCATION]) {
            case '0':    // report 0 gives the average pressure, average temperature, and average humidity
                sprintf(message, "$r0%d:%d:%d\n", pressureSensor.data.average, temperatureSensor.data.average, humiditySensor.data.average);
                SendString(message, (uint16_t)strlen(message), NoStripZeros, NoAddCRLF);
                break;
            case '1':    // report 1 gives the oldest pressure, average temperature, and nozzle number
                sprintf(message, "$r1%d:%d:%d\n", pressureSensor.data.rawData[pressureSensor.data.nextValue], temperatureSensor.data.average, nozzleNo);
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
    }

    processPacket = false;

    return errorCode;
}

