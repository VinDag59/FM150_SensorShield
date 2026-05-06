/*
 * Serial_user.h
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


#ifndef _SERIAL_USER_
#define _SERIAL_USER_

#include "Project.h"
#include "Serial.h"


// Configuration
#define ECHO_BACK false

//Public #defines
#define TX_BUFFER_SIZE 50
#define RX_BUFFER_SIZE 50

// Packet Definition
#define SYNC_CHAR '$'
#define TERM_CHAR '\n'
#define PARAMATER_SEPARATOR ':'

#define COMMAND_LOCATION 1
#define PARAMETER_START_LOCATION 2

// Com port definitions
//#define UART_CFG vcp_0_cfg
//#define UART_CTRL vcp_0_ctrl
//#define UART_ISR_CB vcp_0_cb

#define UART_CFG vcp_0_cf
#define UART_CTRL g_uart0_ctrl
#define UART_ISR_CB vcp_0_cb


// public variables
extern uint8_t processPacket;
//extern uint8_t flashEnable;
//extern volatile uint16_t flashDelaySeed;
extern uint8_t streaming;
extern uint8_t streamReport;
extern uint16_t streamIntervalDelay;


// prototypes

uint8_t ProcessReceiveBuffer(void);
uint8_t ProcessPacket(void);

#endif
