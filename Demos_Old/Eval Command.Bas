10 CLS
20 A$ = "FOR i=1 TO 5:Tmp$=Tmp$+STR$(i)+SPACE$(1):NEXT:A$=Tmp$"
30 EVAL A$
40 PRINT 
50 PRINT "  A$ =: ",A$
60 PRINT "Tmp$ =: ",Tmp$'Tmp$ was created inside A$ in Line 20
