This folder contains files for testing USB communication with the microcontroller.
We were having problems with our modified usb definitions in our virtualenvironments.c
file in the hapticPaddle folder, and didn't want to mess with that code too much while
hunting down the problem. It ended up being a wrong line in the SCONS file. All the 
code in this folder is now obsolete- current development is in the hapticPaddle folder.