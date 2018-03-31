10 Lines=17
20 FOR I=1 TO Lines/2+1
30   FOR J=1 TO (Lines+1)/2-I+1
40     PRINT " " ;
50   NEXT 
60   FOR J=1 TO I*2-1
70     PRINT "*" ;
80   NEXT 
90   PRINT 
100 NEXT I
110 FOR I=1 TO Lines/2
120   FOR J=1 TO I+1
130     PRINT " " ;
140   NEXT 
150   FOR J=1 TO ((Lines+1)/2-I)*2-1
160     PRINT "*" ;
170   NEXT 
180   PRINT 
190 NEXT I
