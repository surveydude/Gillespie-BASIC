# Gillespie-BASIC

Gillespie Basic for Windows is a new (2018) spinoff from the original 1990's Chipmunk Basic 1.0 by Dave Gillespie.  The main story behind the original Chipmunk Basic is that it was written by Dave Gillespie who also later used it to test his P2C (Pascal to C) translator. That is to say, the original Chipmunk Basic 1.0 was programmed in Pascal and converted to "C" for an old HP Unix system known colloquially as Chipmunk -- hence the name.  

Over the years, several people have repurposed the Chipmunk Basic 1.0 source code, porting and modifying it in unique ways for various platforms including Apple, Atari, Linux, and others.  I decided to name my project "Gillespie Basic" to pay respect to the original programmer and to more easily distinguish this Windows project from the others.

Improving Gillespie Basic is a part-time hobby for me and one that I think others will enjoy fiddling with. Whereas Gillespie Basic retains 99% of the original Chipmunk functionality, and fewer bugs, I've spent many hours adding more functionality to it, including a BASIC file I/O system and many new built-in functions and commands. I'm also trying to make the "C" source code easier to understand.  It's complicated but getting better.  

My development PC is running Windows 10 Pro 64-bit but I'm compiling for 32-bit.  I use Mingw32, Pelles-C, and Lcc-Win32 compilers to
check simple C99 compatibility. Other compilers should certainly work with little or no fuss. Gillespie Basic makes many calls to the Win32 API, so porting this source code to other platforms will require some effort.  Having said all that, I think the average user, with little or no programming experience, will find Gillespie Basic fun and useful. 

Gillespie Basic supports -dynamic- double precision and string arrays.  Your programs can use up to 2 GB of memory, although it seems unlikely that you'll ever do so.  Gillespie Basic contains many functions not found in the vintage versions of BASIC, making it easier to turn ideas into working programs. You will also find a helpul collection of sample files that were used during testing which demonstrates some of the built-in functionality.  You should browse the Gillespie Basic text file for a complete list of functions, commands, and capabilities of this BASIC interpreter.

Gillespie Basic is easy to re-compile - just one header file and one C99 compatible source file producing aa small 32-bit / 64-bit Windows executable.  
The executable (Basic.exe) found on Github was compiled using the latest version of the Microsoft Visual C++.  It also builds using Pelles C, Mingw64,
and Clang.

Have fun!



