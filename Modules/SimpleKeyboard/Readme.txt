



This simple keyboard handling module works with up to seven 
bit-mapped keys allowing for "chords" of 2-8 keys. If there is 
no need for chords (or only a few defined chords) then up to 
254 keys, or key combinations, are allowed.

In the case of all chord combinations, the keys (up to 8) will be 
arranged as a bitmap - shifting each input to one of columns 
zero-seven in the 8-bit keyCode variable.

The value of all used bits high is reserved for NO_KEY_PRESSED. (Note
that you will need to define this (e.g.. if three buttons are used,
then the three lowest bits in keyCode will be high/low based on 
button presses, so the value would be b.0000 0111, or 0x07).

In the case of more single keys and/or limited chords, each key
or chord is assigned a value from 0 to 254, and the value of 
255 (0xFF) is reserved for NO_KEY_PRESSED.

Note that the return value from ProcessKeyCode() is either NO_KEY_PRESSED
or the keyCode value. This is valuable for key processes that take 
longer than a few mS to process - a state machine can be built and
the function re-entrant to process each state before it returns
NO_KEY_PRESSED - the last state of the state machine must set the return
value to NO_KEY_PRESSED.

Ideally, an enum type is created for both of these cases, and 
the variable returned from the key scanning and key debouncing 
routines is that type (keyboard_enum_t).

This is controlled with a #define switch (USE_KBD_ENUM) in 
SimpleKeyboard_User.h.


Here is an outline of how the module code is used:

--------------------------------------------
keyboard_enum_t myKeys = NO_KEY_PRESSED;

myKeys = ScanKeyboard();
myKeys = DebounceKeyboard(myKeys);

if (NO_KEY_PRESSED != myKeys) {
    myKeys = ProcessKeyCode(myKeys);
}
--------------------------------------------

Note that the construct of returning a keycode from ProcessKeyboard() 
allows for the possibility of a reentrant function which would 
be re-called the next time through a scheduler loop, or the 
containing RTOS thread running.

The two function calls:

myKeys = ScanKeyboard();
myKeys = DebounceKeyboard(myKeys);

should be called in the 25mS time slot of the scheduler or a thread 
that runs every 25mS. It is a simple "see it twice" algorthm so the 
keys are debounced for 26-50mS (depending on where in the time 
slice the button was pushed) so, on average, it is debounced for 38mS.

The keyboard processing can be done anywhere that makes sense: either 
in that 25mS time-slot, or "every time through the loop." Note that it 
should not be launched in a time slot that is less frequent than the scan, 
or you risk missing a key press. If there is a need to do that, then a 
key queue ring buffer must be created.

(but, suddenly it's not a simple keyboard routine anymore :-)

If your key processing is all short, and simple, then you can avoid adding
the code:

if (NO_KEY_PRESSED != myKeys) {
    myKeys = ProcessKeyCode(myKeys);
}

by turning on the PROCESS_KEY_INLINE #define switch, which will call 
ProcessKeyCode() before it exits, and then it will always return NO_KEY_PRESSED.



