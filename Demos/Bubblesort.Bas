100 DIM A(5)
110 CLS
120 PRINT "Bubble sort demo" 
130 PRINT "Enter 5 numbers, one at a time" 
140 FOR I=1 TO 5
150   INPUT N
160   A(I)=N
170 NEXT 
180 X=5
190 FOR I=1 TO X
200   FOR J=1 TO (X-I)
210     IF A(J)>A(J+1) THEN GOSUB 320
220   NEXT J
230 NEXT I
240 PRINT "----------------------------------" 
250 PRINT "Here are the sorted numbers" 
260 PRINT "----------------------------------" 
270 FOR I=1 TO 5
280   PRINT A(I)
290 NEXT 
300 END
310 ' Swap routine 
320 Temp=A(J)
330 A(J)=A(J+1)
340 A(J+1)=Temp
350 RETURN
