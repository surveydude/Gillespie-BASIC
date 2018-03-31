100 CLS
110 A$="0123456789ABCDEFGHIJKLNOPQRSTUVWXYZ" 
120 B$="!@#$%^&*()_+-={}[]|\;:/?>.,<~`" 
130 Y=ATN(1)*4
140 Z=99999
150 PRINT "------------------------------------------" 
160 PRINT "Before SWAP - Non-array strings and numbers" 
170 PRINT "------------------------------------------" 
180 PRINT "A$ = " ,A$
190 PRINT "B$ = " ,B$
200 PRINT "Y = " ,Y
210 PRINT "Z = " ,Z
220 SWAP A$,B$
230 SWAP Y,Z
240 PRINT "------------------------------------------" 
250 PRINT "After SWAP - Non-array strings and numbers" 
260 PRINT "------------------------------------------" 
270 PRINT "A$ = " ,A$
280 PRINT "B$ = " ,B$
290 PRINT "Y = " ,Y
300 PRINT "Z = " ,Z
310 DIM B(10)
320 A(9)=99999
330 B(5)=55555
340 PRINT "------------------------------------------" 
350 PRINT "Before SWAP - An array of numbers" 
360 PRINT "------------------------------------------" 
370 PRINT "A(9)=" ,A(9)
380 PRINT "B(5)=" ,B(5)
390 SWAP A(9),B(5)
400 PRINT "------------------------------------------" 
410 PRINT "After SWAP - An array of numbers" 
420 PRINT "------------------------------------------" 
430 PRINT "A(9)=" ,A(9)
440 PRINT "B(5)=" ,B(5)
450 PRINT "------------------------------------------" 
460 PRINT "Before SWAP - An array of strings" 
470 PRINT "------------------------------------------" 
480 DIM Animals$(10)
490 Animals$(1)="Lions" 
500 Animals$(2)="Tigers" 
510 PRINT "Animals$(1)=" ,Animals$(1)
520 PRINT "Animals$(2)=" ,Animals$(2)
530 SWAP Animals$(1),Animals$(2)
540 PRINT "------------------------------------------" 
550 PRINT "After SWAP - An array of strings" 
560 PRINT "------------------------------------------" 
570 PRINT "Animals$(1)=" ,Animals$(1)
580 PRINT "Animals$(2)=" ,Animals$(2)
