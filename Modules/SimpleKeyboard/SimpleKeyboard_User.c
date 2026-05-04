
#include <stdint.h>
#include "SimpleKeyboard_User.h"
#include "Serial.h"


#if (USE_KBD_ENUM)
keyboard_enum_t ScanKeyboard(void)
#else
uint8_t ScanKeyboard(void)
#endif
{
    uint8_t _keyCode = NO_KEY_PRESSED;
//    volatile uint8_t keypins[4] = {0xFF, 0xFF, 0xFF, 0xFF};

//    keypins[0] = R_PORT2->PIDR_b.PIDR1;
//    keypins[1] = R_PORT1->PIDR_b.PIDR0;
//    keypins[2] = R_PORT1->PIDR_b.PIDR1;
//    keypins[3] = R_PORT1->PIDR_b.PIDR2;

    _keyCode = // insert your buttons here, bitmapped into a byte
            (uint8_t)((R_PORT2->PIDR_b.PIDR0 << 1) |
                      (R_PORT1->PIDR_b.PIDR0 << 0));

    return _keyCode;
}


#if (USE_KBD_ENUM)
keyboard_enum_t ProcessKeyCode(keyboard_enum_t _keys)
#else
uint8_t ProcessKeyCode(uint8_t _keys)
#endif
{
#if (USE_KBD_ENUM)
    keyboard_enum_t returnValue =  = NO_KEY_PRESSED;
#else
    uint8_t returnValue = NO_KEY_PRESSED;
#endif
    char msg[20] = "Invalid Combo\r\n";
  switch (_keys) {
  case 0x0E: // sw1
      strcpy(msg, "SW1\r\n");
    break;
  case 0x0D: // sw2
      strcpy(msg, "SW2\r\n");
    break;
  case 0x0B: // sw3
      strcpy(msg, "SW3\r\n");
    break;
  case 0x07: // sw4
      strcpy(msg, "SW4\r\n");
    break;
  case 0x05:
      strcpy(msg, "SW2 & SW4\r\n");
  }
  SendString(msg, (uint16_t)strlen(msg), StripZeros, NoAddCRLF);
  return returnValue;
}

