100 '-------------------------
110 '     Sieve benchmark
120 '-------------------------
130 Loops=1
140 T=TIMER
150 DIM F(8194)
160 FOR J=1 TO Loops
170   C=0
180   S=8191
190   FOR I=0 TO S
200     F(I)=1
210   NEXT 
220   FOR I=0 TO S
230     IF F(I)=0 THEN GOTO 300
240     P=I+I+3
250     IF I+P>S THEN 290
260     FOR K=I+P TO S STEP P
270       F(K)=0
280     NEXT 
290     C=C+1
300   NEXT I
310 NEXT J
320 PRINT C;"primes found " ;
330 T=TIMER-T
340 PRINT Loops;"times in " ;
350 PRINT USING$ ("##.##" ,T)," seconds" 
