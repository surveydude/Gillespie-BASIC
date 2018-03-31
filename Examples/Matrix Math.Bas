100 '***************************************
110 '     Simulate matrix addition
120 '***************************************
130 DIM A(5,3),B(5,3),C(5,3)
140 CLS
150 PRINT "a array contents:"  : PRINT 
160 FOR X=1 TO 5
170   FOR Y=1 TO 3
180     READ A(X,Y)
190     PRINT A(X,Y),
200   NEXT Y
210   PRINT 
220 NEXT X
230 PRINT  : PRINT 
240 PRINT "b array contents:"  : PRINT 
250 FOR X=1 TO 5
260   FOR Y=1 TO 3
270     READ B(X,Y)
280     PRINT B(X,Y),
290   NEXT Y
300   PRINT 
310 NEXT X
320 PRINT  : PRINT 
330 PRINT "RESULTS: c array contents:"  : PRINT 
340 FOR X=1 TO 5
350   FOR Y=1 TO 3
360     C(X,Y)=A(X,Y)+B(X,Y)
370     PRINT C(X,Y),
380   NEXT Y
390   PRINT 
400 NEXT X
410 PRINT  : PRINT 
420 DATA 7,8,9,3,4,1,6,8,2,1,4,1,8,-10,2
430 DATA 3,5,8,2,4,7,1,3,1,8,4,9,6,6,6
