#include <stdint.h>
#include <stdint.h>
#include "Project.h"
#include "SimpleKeyboard_User.h"


// Module scope variables
#if (USE_KBD_ENUM)
keyboard_enum_t lastKeyCode = NO_KEY_PRESSED;
#else
uint8_t lastKeyCode = NO_KEY_PRESSED;
#endif
uint8_t keyCodeProcessed = false;



#if (USE_KBD_ENUM)
keyboard_enum_t DebounceKeyboard(keyboard_enum_t _keyCode)
#else
uint8_t DebounceKeyboard(uint8_t _keyCode)
#endif
{
#if (USE_KBD_ENUM)
    keyboard_enum_t debounceStatus = NO_KEY_PRESSED;
#else
    uint8_t debounceStatus = NO_KEY_PRESSED;        
#endif

    if (_keyCode != NO_KEY_PRESSED) {               // Check to see if a button is pressed
        if (lastKeyCode == _keyCode) {              // is it the same as the last keycode?
          if (keyCodeProcessed == false) {          // have we processed this keycode already?
#if (PROCESS_KEY_INLINE)
            ProcessKeyCode(_keyCode);               // we might want to process it inline here
#else
            debounceStatus = _keyCode;              // if not, tell the calling function to process it
                                                    // by returning the keycode rather than NO_KEY_PRESSED
#endif
            keyCodeProcessed = true;
          }
        }
        else {
          lastKeyCode = _keyCode;                   // if the last and current keycodes are not the same, make them the same
        }
    }
    else {
        lastKeyCode = NO_KEY_PRESSED;               // if no key is pressed, reset everything to the default
        keyCodeProcessed = false;
    }
    return debounceStatus;
}
