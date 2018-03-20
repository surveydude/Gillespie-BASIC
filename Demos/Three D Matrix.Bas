10 DIM  A(100,100,100) : REM This Allocates (100^3) * 8 Bytes = approx 8MB
20 FOR  i =1 TO 99
30   A( i, i, i) = i
40 NEXT
50 FOR  i =1 TO 99
60   PRINT  A( i, i, i)
70 NEXT
