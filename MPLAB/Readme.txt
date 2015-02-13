MPLAB elecanisms tutorial, written by Kyle 2/13/2015/I

This course was clearly designed to be run using Linux, and NOT using an IDE . . . *Sigh*

If you still want to use an IDE (or, idk, want to use the official program supported and distributed by microchip??), then 
this is for you.

create a library project, and add all the .c files from Brad's lib folder. see this website for a quick tutorial on creating librarys
http://microchip.wikidot.com/mplabx:libraries#exproj

Note that the header files do not go in the library project (which is slightly frustrating). I'd suggest making a folder
with all the header files in it.

you can compile the library project if you want, but it doesn't save all that much time- its a small library.

now, when you make new projects, add the library project under 'librarys'. COPY all the .h files into the new project directory.
MPLAB has some struggles with linking to files not in the project directory- its not impossible, but with a stable library,
its much easier to just copy them in. (this is why it's slightly frustrating- why don't the .h files belong in the library
project??). This will have to happen for every project you make. also note: you don't actually have to add the .h files
to the project- if you just copy them into the folder, you can then import them into any other code you want.