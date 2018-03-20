100 '***********************************************
110 '  Based on Conway's Life (cellular automata)
120 '************************************************
130 DIM A$(20),T$(20)
140 CLS
150 PRINT  : PRINT "Press any key to begin" : PRINT 
160 PRINT  : PRINT "Press ESCAPE to stop"
170 '
180 I = KEYPRESS
190 CLS
200 '
210 FOR I = 1 TO 20
220   A$(I) = SPACE$(80)
230   T$(I) = SPACE$(80)
240 NEXT 
250 '
260 'Glider
270 A$(10) = SPACE$(7)+"***"+SPACE$(70)
280 A$(11) = SPACE$(7)+"*  "+SPACE$(70)
290 A$(12) = SPACE$(7)+" * "+SPACE$(70)
300 'Diehard
310 A$(15) = SPACE$(25)+"**** "+SPACE$(50)
320 A$(16) = SPACE$(25)+" ****"+SPACE$(50)
330 '
340 FOR J = 1 TO 100
350   FOR Y = 1 TO 20
360     FOR X = 1 TO 80
370       C0 = X-1
380       IF C0 = 0 THEN C0 = 80
390       C1 = X
400       C2 = X+1
410       IF C2 = 81 THEN C2 = 1
420       R0 = Y-1
430       IF R0 = 0 THEN R0 = 20
440       R1 = Y
450       R2 = Y+1
460       IF R2 = 21 THEN R2 = 1
470       '
480       N = 0
490       '
500       IF MID$(A$(R0),C0,1) = "*" THEN N = N+1
510       IF MID$(A$(R0),C1,1) = "*" THEN N = N+1
520       IF MID$(A$(R0),C2,1) = "*" THEN N = N+1
530       IF MID$(A$(R1),C0,1) = "*" THEN N = N+1
540       IF MID$(A$(R1),C2,1) = "*" THEN N = N+1
550       IF MID$(A$(R2),C0,1) = "*" THEN N = N+1
560       IF MID$(A$(R2),C1,1) = "*" THEN N = N+1
570       IF MID$(A$(R2),C2,1) = "*" THEN N = N+1
580       IF MID$(A$(Y),X,1) = "*" AND N < 2 THEN T$(Y) = LEFT$(T$(Y),X-1)+" "+RIGHT$(T$(Y),80-X)
590       IF MID$(A$(Y),X,1) = "*" AND N > 3 THEN T$(Y) = LEFT$(T$(Y),X-1)+" "+RIGHT$(T$(Y),80-X)
600       IF MID$(A$(Y),X,1) = " " AND N = 3 THEN T$(Y) = LEFT$(T$(Y),X-1)+"*"+RIGHT$(T$(Y),80-X)
610       IF MID$(A$(Y),X,1) = "*" AND N = 2 THEN T$(Y) = LEFT$(T$(Y),X-1)+"*"+RIGHT$(T$(Y),80-X)
620       IF MID$(A$(Y),X,1) = "*" AND N = 3 THEN T$(Y) = LEFT$(T$(Y),X-1)+"*"+RIGHT$(T$(Y),80-X)
630       '
640     NEXT 
650   NEXT 
660   '
670   FOR I = 1 TO 20
680     A$(I) = T$(I)
690     T$(I) = SPACE$(80)
700   NEXT 
710   '
720   LOCATE 1,1
730   SETCURSOR 0
740   '
750   FOR I = 1 TO 20
760     PRINT A$(I)
770   NEXT 
780 NEXT 
