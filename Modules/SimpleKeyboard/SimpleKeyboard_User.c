
#include <stdint.h>
#include "SimpleKeyboard_User.h"
#include "Serial.h"
#include "ASCII_numbers.h"



uint8_t keyPress = false;

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
     uint8_t tempVal = 0;
     timer_info_t info;
     volatile fsp_err_t err;
#endif
    char msg[20] = "$K3\n";
  switch (_keys) {
      case 0x00: // sw1 & 2
          strcpy(msg, "$K0\n");
          break;
      case 0x02: // sw1
          strcpy(msg, "$K2\n");
          SendString(msg, (uint16_t)strlen(msg), StripZeros, NoAddCRLF);

          tempVal = blowerPWM + 2;
          if (tempVal <= 90) {
              blowerPWM = tempVal;

             // change the value of the PWM
             /* Get the current period setting. */
             R_TAU_PWM_InfoGet(&g_timer0_ctrl, &info);
             uint32_t current_period_counts = info.period_counts;
             /* Calculate the desired duty cycle based on the current period. */
             uint16_t duty_cycle_counts = (uint16_t) ((current_period_counts * blowerPWM) / 100);
             /* Set the calculated duty cycle. */
             err = R_TAU_PWM_DutyCycleSet(&g_timer0_ctrl, duty_cycle_counts, TAU_PWM_IO_PIN_CHANNEL_5);

//             sprintf(msg, "$P%d\n", blowerPWM);
//             SendString(msg, (uint16_t)strlen(msg), StripZeros, NoAddCRLF);
          }
          break;
      case 0x01: // sw2
          strcpy(msg, "$K1\n");
          SendString(msg, (uint16_t)strlen(msg), StripZeros, NoAddCRLF);

          tempVal = blowerPWM - 2;
          if (tempVal >= 40) {
              blowerPWM = tempVal;

             // change the value of the PWM
             //status = R_TAU_PWM_DutyCycleSet(&g_timer0_ctrl, duty_cycle_counts, pin);
              /* Get the current period setting. */

             R_TAU_PWM_InfoGet(&g_timer0_ctrl, &info);
             uint32_t current_period_counts = info.period_counts;
              /* Calculate the desired duty cycle based on the current period. */
             uint16_t duty_cycle_counts = (uint16_t) ((current_period_counts * blowerPWM) / 100);
              /* Set the calculated duty cycle. */
             //err = R_TAU_PWM_Stop(&g_timer0_ctrl);
             err = R_TAU_PWM_DutyCycleSet(&g_timer0_ctrl, duty_cycle_counts, TAU_PWM_IO_PIN_CHANNEL_5);
             //         assert(FSP_SUCCESS == err)
             //err = R_TAU_PWM_Start(&g_timer0_ctrl);


//             sprintf(msg, "$P%d\n", blowerPWM);
//             SendString(msg, (uint16_t)strlen(msg), StripZeros, NoAddCRLF);
          }
          break;
      default:
          SendString(msg, (uint16_t)strlen(msg), StripZeros, NoAddCRLF);
          break;
      }

  SendString(msg, (uint16_t)strlen(msg), StripZeros, NoAddCRLF);
  if (tempVal == blowerPWM) {
      sprintf(msg, "$P%d\n", blowerPWM);
      SendString(msg, (uint16_t)strlen(msg), StripZeros, NoAddCRLF);
  }
  return returnValue;
}

