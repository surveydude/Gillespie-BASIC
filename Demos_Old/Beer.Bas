100 '****************************************
110 '        99 Bottles of Beer Demo
120 '****************************************
130 CLS
140 PRINT "99 Bottles of Beer ... "  : PRINT  : PRINT 
150 INPUT "How many bottles of beer? " ,Num : PRINT 
160 PRINT Num;" bottles of beer on the wall" 
170 PRINT Num;" bottles of beer" 
180 PRINT "take one down pass it around" 
190 PRINT Num-1;" bottles of beer on the wall" 
200 PRINT 
210 LET Num=Num-1
220 IF Num>0 THEN GOTO 160
230 PRINT "No more beer!  :-(" 
