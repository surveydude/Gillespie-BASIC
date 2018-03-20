# Gillespie-BASIC

Gillespie Basic for Windows is a spinoff from the original 1990's Chipmunk Basic 1.0 by Dave Gillespie.  The main story behind the original Chipmunk Basic is that Dave Gillespie wrote it to test his P2C (Pascal to C) translator. That is to say, the original Chipmunk Basic 1.0 was created in Pascal and converted to "C" for an old HP Unix system known colloquially as Chipmunk -- hence the name.  

Over the years, several programmers have used the Chipmunk 1.0 source code, porting and modifying it in unique ways for various computers including Apple, Atari, Linux, and others.  I decided naming this software "Gillespie Basic" would pay respect to the original programmer and more easily distinguish this Windows project from the others.

Improving Gillespie Basic is a recreational hobby for me that I think other folks will enjoy. Whereas Gillespie Basic retains most of the original Chipmunk 1.0 functionality, and fewer bugs, I've spent many hours adding more functionality to it, including a BASIC file I/O system and many new BASIC functions. I've tried to make the "C" source code easier to understand and work with.  Much of the code behind my enhancements comes from one of my earlier projects, BCX (The Basic To C Translator). 

I enjoy using Pelles C as my compiler system - certainly other compilers will work with minimal fuss.  But please understand that Gillespie Basic makes many calls to the Win32 API, so porting this source code to other platforms will require more effort.  All that being said, I feel Gillespie Basic strikes a nice blend of cool vintage look and feel while enjoying some benefits of running natively on a 32-bit or 64-bit version of Windows.  My development PC is running Windows 10 Pro 64-bit but I'm compiling for 32-bit.

There is (1) header file, (1) "C" source file, and (1) small (117kb) executable.  I've also uploaded a bunch of sample files that were used during testing that will be instructional.  Also read the Gillespie Basic text file for the new functions and keywords that I've added to the interpreter.

FUTURE IMPROVEMENTS ???

I'd like to add: 

    * multi-line IF-THEN-ELSE-END IF
    * text labels ( for goto / gosub )
    * user defined FUNCTIONS/SUBS
    * SELECT CASE
    
If there is any interest, I may post updates.
