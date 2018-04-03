# Gillespie-BASIC

Gillespie Basic for Windows is a spinoff from the original 1990's Chipmunk Basic 1.0 by Dave Gillespie.  The main story behind the original Chipmunk Basic is that Dave Gillespie used it to test his P2C (Pascal to C) translator. That is to say, the original Chipmunk Basic 1.0 was programmed in Pascal and converted to "C" for an old HP Unix system known colloquially as Chipmunk -- hence the name.  

Over the years, several people have repurposed the Chipmunk Basic 1.0 source code, porting and modifying it in unique ways for various platforms including Apple, Atari, Linux, and others.  I decided naming this software "Gillespie Basic" would pay respect to the original programmer and more easily distinguish this Windows project from the others.

Improving Gillespie Basic is a part-time  hobby for me but one that I think other folks will enjoy playing around with. Whereas Gillespie Basic retains 99% of the original Chipmunk 1.0 functionality, and fewer bugs, I've spent many hours adding more functionality to it, including a BASIC file I/O system and many new BASIC functions. I've tried to make the "C" source code easier to understand and work with.  

My development PC is running Windows 10 Pro 64-bit but I'm compiling for 32-bit.  I use Mingw32, Pelles-C, and Lcc-Win32 compilers to
check simple compatibility. Other compilers will certainly work with little or no fuss. But understand that Gillespie Basic makes many
calls to the Win32 API, so porting this source code to other platforms will require some effort.  Having said all that, I think the
average people, with little or no programming experience, will find Gillespie Basic fun and useful. 

There is (1) header file and (1) "C" source file which produces (1) very small (~120KB) standalone 32-bit Windows executable.  
You will also find a collection of sample files that were used during testing and which demonstrates some of the built-in functionality.  You should browse the Gillespie Basic text file for a complete list of functions, commands, and capabilities of this BASIC interpreter.

Have fun!
