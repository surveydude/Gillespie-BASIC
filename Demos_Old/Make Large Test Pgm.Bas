100 OPEN "test.bas" FOR OUTPUT AS 1
110 FOR I=1 TO 10000
120   A$=ENC$("Here I present before you line number " +STR$(I))
130   FPRINT 1,I,"print " ,A$
140 NEXT 
150 CLOSE 1
160 LOAD "test" 
