# Gillespie-BASIC

Gillespie Basic for Windows is a spinoff from the original 1990's Chipmunk Basic 1.0 by Dave Gillespie.  The main story behind the original Chipmunk Basic is that Dave Gillespie used it to test his P2C (Pascal to C) translator. That is to say, the original Chipmunk Basic 1.0 was programmed in Pascal and converted to "C" for an old HP Unix system known colloquially as Chipmunk -- hence the name.  

Over the years, several people have repurposed the Chipmunk Basic 1.0 source code, porting and modifying it in unique ways for various platforms including Apple, Atari, Linux, and others.  I decided naming this software "Gillespie Basic" would pay respect to the original programmer and more easily distinguish this Windows project from the others.

Improving Gillespie Basic is a part-time recreational hobby for me but one that I think other folks will enjoy playing around with. Whereas Gillespie Basic retains 99% of the original Chipmunk 1.0 functionality, and fewer bugs, I've spent many hours adding more functionality to it, including a BASIC file I/O system and many new BASIC functions. I've tried to make the "C" source code easier to understand and work with.  Much of the code behind my enhancements comes from one of my earlier projects, BCX (The Basic To C Translator). 

I enjoy using Pelles C as my compiler system but certainly other compilers will work with minimal fuss.  Please understand that Gillespie Basic makes many calls to the Win32 API, so porting this source code to other platforms will require some effort.  All that being said, I feel Gillespie Basic strikes a cool blend of vintage look and feel while enjoying some key benefits (speed, memory, WinAPI) of running natively on a 32-bit or 64-bit version of Windows  and more.  My development PC is running Windows 10 Pro 64-bit but I'm compiling for 32-bit.

There is (1) header file and (1) "C" source file which produced (1) very small (~120KB) executable.  I've also uploaded a bunch of sample files that were used during testing and that show off some of the new functions and commands.  You should browse the Gillespie Basic text file for the list of functions, commands, and capabilities of this BASIC interpreter.

Gillespie BASIC is both usable and stable, as is.  Enjoy!
