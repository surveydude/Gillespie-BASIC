A$ = "global"
CALL Mysub
PRINT "Outside: "; A$


SUB Mysub
  DIM A$
  A$ = "In Sub"
  PRINT "Inside sub: "; A$
END SUB