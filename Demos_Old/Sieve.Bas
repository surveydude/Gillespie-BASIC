10 '**********************************************
20 ' "sieve.bas" , a prime number sieve benchmark
30 '**********************************************
40 T=TIMER
50 DIM F(8194)
60 FOR I=0 TO 8191
70   F(I)=1
80 NEXT 
90 S=8191
100 FOR I=0 TO S
110   IF F(I)=0 THEN GOTO 170
120   P=I+I+3
130   FOR K=I+P TO S STEP P
140     F(K)=0
150   NEXT 
160   C=C+1
170 NEXT 
180 PRINT C;" primes found in ";
190 T=TIMER-T
200 PRINT USING$"##.###",T;" seconds"
210 END
