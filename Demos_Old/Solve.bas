
CLS
PRINT
PRINT "Solve for X"
PRINT
PRINT "X + X^2 + X^3 = 42"
PRINT
PRINT "Working ..."

A=TIMER

WHILE X + X^2 + X^3 < 42
  X = X + 0.0001
WEND

WHILE X + X^2 + X^3 > 42
  X = X - 0.000001
WEND

PRINT
PRINT "*********"
PRINT "Solution:"
PRINT "*********"
PRINT "X   =  " ; X
PRINT "X^2 =  " ; X^2
PRINT "X^3 = "  ; X^3
PRINT
PRINT
PRINT "Elapsed time = " ,TIMER-A," seconds"
