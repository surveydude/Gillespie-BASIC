100 DIM Myarray(10)
110 X = 10
120 CLS
130 PRINT "Bubble sort demo"
140 PRINT 
150 PRINT "----------------------------------"
160 PRINT "Here are the unsorted numbers"
170 PRINT "----------------------------------"
180 FOR I = 1 TO X
190   READ N
200   PRINT N
210   Myarray(I) = N
220 NEXT 
230 FOR I = 1 TO X
240   FOR J = 1 TO (X-I)
250     IF Myarray(J) > Myarray(J+1) THEN SWAP Myarray(J),Myarray(J+1)
260   NEXT 
270 NEXT 
280 PRINT "----------------------------------"
290 PRINT "Here are the sorted numbers"
300 PRINT "----------------------------------"
310 FOR I = 1 TO X
320   PRINT Myarray(I)
330 NEXT 
340 DATA 17,33,47,13,2,99,73,82,27,66
