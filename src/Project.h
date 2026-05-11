/*
 * Project.h
 *
 *  Created on: Jul 9, 2023
 *      Author: VincentD'Agostino
 */

#ifndef PROJECT_H_
#define PROJECT_H_

#include <stdbool.h>

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif


#define FW_REV_HI "00"
#define FW_REV_LOW "03"
#define FW_REV_REL "10"

#define FW_REV__BUILT "FW v" FW_REV_HI "." FW_REV_LOW "." FW_REV_REL "\r\n"
#define FW_REV_PACKET_BUILT "$f" FW_REV_HI "." FW_REV_LOW "." FW_REV_REL "\n\r\n"

#define DEFAULT_ON_TIME 40
#define DEFAULT_OFF_TIME 40

#define MAX_TIME 80
#define MIN_TIME 4

#define BLUE_LED_INDEX 0
#define SHIELD_MOUNTED_LED1 R_PORT2->PODR_b.PODR1 // D5
#define SHIELD_MOUNTED_LED2 R_PORT2->PODR_b.PODR1 // D6
#define SHIELD_MOUNTED_LED3 R_PORT1->PODR_b.PODR2 // D7
#define SHIELD_MOUNTED_HEARTBEAT_LED R_PORT0->PODR_b.PODR11

#define  BOARD_MOUNTED_SWITCH R_PORT2->PIDR_b.PIDR0
#define SHIELD_MOUNTED_SW1
#define SHIELD_MOUNTED_SW2

#define DEFAULT_SER_NO "SNxxxxxx"
#define DEFAULT_MODEL_NAME "APC Sensor Shield\r\n"
#define DEFAULT_REF_DATA "SNxxxxxx;Acme Comms 2000"

#define SER_NO_LENGTH 12
#define MODEL_NAME_LENGTH 25
#define REF_DATA_LENGTH 32

enum VEPROM_IDS {
    ON_TIME_ID = 1,
    OFF_TIME_ID,
    BLUE_LED_FEATURE_ID
};

#endif /* PROJECT_H_ */
