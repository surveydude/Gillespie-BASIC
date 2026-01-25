PRINT " A Prime Number Sieve Benchmark, Please Wait.."
Maxtest = 10000
Limit = (Maxtest - 3) / 2
DIM F(Limit)
T = TIMER
FOR I = 0 TO Limit
  F(I) = 1
NEXT I
C = 1 '    Start at 1 to count 2 as prime
FOR I = 0 TO Limit
  IF F(I) = 0 THEN CONTINUE
  P = (2*I) + 3
  IF P*P > Maxtest THEN CONTINUE
  FOR K = (P*P - 3)/2 TO Limit STEP P
    F(K) = 0
  NEXT K
NEXT
FOR I = 0 TO Limit
  IF F(I) = 1 THEN C = C + 1
NEXT
T = (TIMER - T)
PRINT
PRINT C;"Primes Found up to ";USING$("#,", Maxtest);" in ";USING$("#.#", T);" seconds"