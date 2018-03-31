100 CLS
110 OPEN "filename.txt" FOR OUTPUT  AS 1
120 FOR I = 1 TO 1000
130   FPRINT 1, MCASE$( "this is line number" ),I
140 NEXT
150 CLOSE 1
160 OPEN "filename.txt" FOR  INPUT  AS 1
170 WHILE  NOT EOF(1)
180   LINE  INPUT  1,A$
190   PRINT  A$
200 WEND
210 CLOSE 1
220 KILL "filename.txt" 
