10 CLS
20 PRINT "Demonstrates the KEYPRESS function" 
30 PRINT 
40 PRINT "Press ESCAPE to quit" 
50 DO 
60   A=KEYPRESS
70   PRINT A
80 LOOP UNTIL A=27
