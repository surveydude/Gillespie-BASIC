100 '**********************************************
110 ' "sieve.bas" , a prime number sieve benchmark
120 '**********************************************
130 CLS
140 T=TIMER
150 DIM F(8194)
160 FOR I=0 TO 8191
170   F(I)=1
180 NEXT 
190 S=8191
200 FOR I=0 TO S
210   IF F(I)=0 THEN GOTO 270
220   P=I+I+3
230   FOR K=I+P TO S STEP P
240     F(K)=0
250   NEXT 
260   C=C+1
270 NEXT 
280 PRINT C;" primes found in " ;
290 T=TIMER-T
300 PRINT USING$ ("##.###" ,T)," seconds" 
310 END
