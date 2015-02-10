master
======

Master repository for ENGR3199 includes C and Python source code and supporting files


Instructions for building and programming the PIC with a simple blink program.
1. Modify/ Write the blink program to operate as desired
2. build a .hex file. See two options below (SCons vs. MPLab)
3. Program the board.

1. We altered the blink file to have all of the leds blink at different rates. To do this, we stopped LED2 and LED3 from listening to the buttons, and added a counter that counted how many times the timer overflowed. We then blinked the LEDs when this counter reached various values. This enables us to multiplex multiple LEDs while saving the other hardware timers for other functions.

2.A. Generate a hex file with SCons
update the PATH in the SConstruct file to point to the location of the MPLAB compilier on your computer.
open command line or terminal and navigate to the directory containing blink.c.
type 'scons'.

2.B. Generate a hex file with MPLab
create a new MPLab project
Add all the required files to the project (right click on source file, add existing item)
move the .h files to the header files folder (not required, nice for organization)
build the project (right click on the project, build)
find the .hex file in dist/... foler chain.
NOTE: another method involves building a library project from all of the lib files. This reduces the number of files that need to be added to the project, and also elimates the need to make copies of lib files as new projects are started. It requires writing a header file for the library project. I am still exploring this option.

3. Programming the board:
plug the board into the computer via the USB to USB micro cable.
install drivers for board. Manual driver installation is necessary. See bootloader readme for instructions.
run bootloadergui.py
place board into programming mode by holding rst and sw1, then releasing rst, followed by sw1.
load the hex file (file/Import Hex)
click 'write' to write the hex file to the PIC
press the reset button to enter normal operation mode.
Verify that the lights are blinking as expected.


Unexpected pitfalls/bugs:
board drivers had to be reinstalled after board was disconnected and reconnected. This was not tried a second time. (possibly due to windows)
bootloadergui.py had to be reloaded after each write. attempting a second write (even if the board is put in programming mode) causes the gui to freeze.
