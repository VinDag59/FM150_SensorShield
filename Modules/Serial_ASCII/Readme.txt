This version of the serial port manager uses inline functions for the interrupt - functions that are not called, 
but put inline by the compiler. This makes the scope much simpler, and the execution as fast as the macros.

*** This is assuming an ASCII Protocol, so it includes the ASCII -> Binary Conversion fuctions in 

It requres that the .h file be included in "main.c" and "*_it.c" files in ASCII_NUMBERS.c and .h which also need to be included in the project.

To incorporate it, you simply need to: 

1) Copy this folder into the modules folder of the project
2) Include the SingleSerial_user file in main.c and _it.c, and any file that may call a public function for UART
3) Place the function calls into the UART handler in _it.c
4) Add SingeSerial.c, SingleSerial_User.c, ASCII_Numbers.c to the project in the modules folder
5) Add the module folder to the search path in "project | options | C/C++ compiler | Preprocessor"
6) Add the appropriate code to the main() funcion to test for buffer empty test, process a char, and process a packet
7) Select the UART (#define UART2USE_A UART1)
8) Add   "LL_USART_EnableIT_RXNE_RXFNE(UART2USE_A);" in user code area 2 of main.c
9) Add the device-appropriate version of "stm32g0xx_ll_usart.h" to the top of main.c



-------------------------------------------------------
NOTE:
ensure that true and false are defined in main.h:

#define true 1
#define false 0

To use this for more than one UART, you must have a second copy with all the function names changed (I suggest adding a _B, _C, etc to
folder, file, and function names. you can change the "_A" in "UART_2_USE_A" also to _B, _C, etc.)

But... the MultiSerial module is a better choice for multiple serial ports: one set of code and multiple port definitions.




