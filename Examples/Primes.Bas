10 CLS
20 FOR N = 1 TO 150
30   C = 0
40   FOR I = 1 TO N
50     IF  N MOD I = 0 THEN C = C + 1
60   NEXT I
70   IF  C = 2 THEN PRINT  N, " is prime" 
80 NEXT
