
#ifndef _SIMPLE_KEYBOARD_USER_H_
#define _SIMPLE_KEYBOARD_USER_H_


#include "bsp_api.h"
#include "SimpleKeyboard.h"



#define USE_KBD_ENUM 0
#define PROCESS_KEY_INLINE 0


#if (USE_KBD_ENUM)
typedef enum _key_names_ {
    Key1,
    key2,
    key3,
    key4,
    NO_KEY_PRESSED = 0x0F;
} keyboard_enum_t;
#else
#define NO_KEY_PRESSED  0x03
#endif


#if (USE_KBD_ENUM)
keyboard_enum_t ScanKeyboard(void);
keyboard_enum_t ProcessKeyCode(keyboard_enum_t _keys);
#else
uint8_t ScanKeyboard(void);
uint8_t ProcessKeyCode(uint8_t _keys);
#endif


#endif
