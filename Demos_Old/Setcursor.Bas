10 CLS
20 PRINT "The blinking cursor should not be visible" 
30 PRINT 
40 PRINT "PRESS ANY KEY to restore the blinking cursor" 
41 '------------------------------------------
50 SETCURSOR 0' Hide the blinking cursor
60 A=KEYPRESS
70 SETCURSOR 1' Show the blinking cursor
71 '------------------------------------------
80 PRINT  : PRINT  : PRINT "TADA!" 
