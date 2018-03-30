100 DIM Myarray(5)
110 CLS
120 PRINT "Bubble sort demo"
130 PRINT "Enter 5 numbers, one at a time"
140 FOR I = 1 TO 5
150   INPUT N
160   Myarray(I) = N
170 NEXT
180 X = 5
190 FOR I = 1 TO X
200   FOR J = 1 TO (X-I)
210     IF Myarray(J) > Myarray(J+1) THEN SWAP Myarray(J),Myarray(J+1)
220   NEXT
230 NEXT
240 PRINT "----------------------------------"
250 PRINT "Here are the sorted numbers"
260 PRINT "----------------------------------"
270 FOR I = 1 TO 5
280   PRINT Myarray(I)
290 NEXT
