PRINT " A Prime Number Sieve Benchmark, Please Wait.."

MAXTEST = 10000

DIM f(MAXTEST)

t = TIMER

FOR i  = 0 TO MAXTEST
    f(i) = 1
NEXT i

s = MAXTEST
c = 0 ' Initialize counter

FOR i = 0 TO s
    IF f(i) = 0 THEN GOTO NextIter

    ' This logic calculates the actual prime
    p = (2*i) + 3

    FOR k  = i + p TO s STEP p
        f(k) = 0
    NEXT k

    c = c + 1

    NextIter:
NEXT
t = (TIMER - t)
PRINT
PRINT c; "Primes Found in "; USING$("#,", MAXTEST); " numbers in "; USING$("#.#", t); " seconds"

