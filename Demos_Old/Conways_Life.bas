10  ' *************************************************************
20  '  Conway's Game of Life
30  '  Chipmunk version by Kevin Diggins 2020
40  '  Derived from an old ABC BASIC Packets demo
50  ' *************************************************************
60 Boardx = 82
70 Boardy = 25
80 Alive = 2
90 Dead = 32
100 DIM B(Boardy,Boardx)
110 DIM C(Boardy,Boardx)
120 DIM K(Boardy,Boardx)
130 Neighbors = 0
140 X = 0
150 Y = 0
160 RANDOMIZE TIMER
170  '  start me up
180 CLS
190 FOR X = 1 TO Boardx-2
200   FOR Y = 1 TO Boardy-2
210     IF (RND*10) > 5 THEN 
220       GOSUB 580
230      ELSE 
240       GOSUB 620
250     END IF 
260     COLOR K(Y,X),0
270     LOCATE Y,X
280     SETCURSOR 0
290     PRINT CHR$(B(Y,X))
300   NEXT 
310 NEXT 
320 DO 
330   FOR X = 1 TO Boardx-2
340     FOR Y = 1 TO Boardy-2
350       Neighbors = 0
360       IF C(Y-1,X) = Alive THEN Neighbors = Neighbors+1
370       IF C(Y-1,X-1) = Alive THEN Neighbors = Neighbors+1
380       IF C(Y,X-1) = Alive THEN Neighbors = Neighbors+1
390       IF C(Y+1,X-1) = Alive THEN Neighbors = Neighbors+1
400       IF C(Y+1,X) = Alive THEN Neighbors = Neighbors+1
410       IF C(Y+1,X+1) = Alive THEN Neighbors = Neighbors+1
420       IF C(Y,X+1) = Alive THEN Neighbors = Neighbors+1
430       IF C(Y-1,X+1) = Alive THEN Neighbors = Neighbors+1
440       IF Neighbors < 2 OR Neighbors > 3 THEN B(Y,X) = Dead
450       IF Neighbors = 3 THEN B(Y,X) = Alive
460     NEXT 
470   NEXT 
480   FOR X = 1 TO Boardx-2
490     FOR Y = 1 TO Boardy-2
500       LOCATE Y,X
510       COLOR K(Y,X),0
520       SETCURSOR 0
530       PRINT CHR$(B(Y,X))
540       C(Y,X) = B(Y,X)
550     NEXT 
560   NEXT 
570 LOOP 
580 B(Y,X) = Alive
590 C(Y,X) = Alive
600 K(Y,X) = RND*15
610 RETURN
620 B(Y,X) = Dead
630 C(Y,X) = Dead
640 K(Y,X) = RND*15
650 RETURN
