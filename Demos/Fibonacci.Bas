100 '********************************************
110 '            The Fibonacci Series
111 '********************************************
120 CLS
130 A=1
140 B=1
150 FOR Z=0 TO 40' Seems like a good place to stop
160   C=A+B
170   PRINT USING$ ("#" ,A)
180   A=B
190   B=C
200 NEXT Z
