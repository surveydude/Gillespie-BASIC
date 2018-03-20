100 '***********************************************
110 '  Based on Conway's Life (cellular automata)
120 '************************************************
130 DIM A$(20),T$(20)
140 CLS
150 PRINT  : PRINT "Press any key to begin" : PRINT 
160 PRINT  : PRINT "Press ESCAPE to stop"
170 '
180 I=KEYPRESS
190 '
200 FOR I=1 TO 20
210   A$(I)=SPACE$(80)
220   T$(I)=SPACE$(80)
230 NEXT 
240 '
250 'Glider
260 A$(10)="       "+"***"+SPACE$(70)
270 A$(11)="       "+"*  "+SPACE$(70)
280 A$(12)="       "+" * "+SPACE$(70)
290 'Diehard
300 A$(15)=SPACE$(25)+"**** "+SPACE$(50)
310 A$(16)=SPACE$(25)+" ****"+SPACE$(50)
320 '
330 FOR J=1 TO 100
340   FOR Y=1 TO 20
350     FOR X=1 TO 80
360       C0=X-1
370       IF C0=0 THEN C0=80
380       C1=X
390       C2=X+1
400       IF C2=81 THEN C2=1
410       R0=Y-1
420       IF R0=0 THEN R0=20
430       R1=Y
440       R2=Y+1
450       IF R2=21 THEN R2=1
460       '
470       N=0
480       '
490       IF MID$(A$(R0),C0,1)="*" THEN N=N+1
500       IF MID$(A$(R0),C1,1)="*" THEN N=N+1
510       IF MID$(A$(R0),C2,1)="*" THEN N=N+1
520       IF MID$(A$(R1),C0,1)="*" THEN N=N+1
530       IF MID$(A$(R1),C2,1)="*" THEN N=N+1
540       IF MID$(A$(R2),C0,1)="*" THEN N=N+1
550       IF MID$(A$(R2),C1,1)="*" THEN N=N+1
560       IF MID$(A$(R2),C2,1)="*" THEN N=N+1
570       IF MID$(A$(Y),X,1)="*" AND N<2 THEN T$(Y)=LEFT$(T$(Y),X-1)+" "+RIGHT$(T$(Y),80-X)
580       IF MID$(A$(Y),X,1)="*" AND N>3 THEN T$(Y)=LEFT$(T$(Y),X-1)+" "+RIGHT$(T$(Y),80-X)
590       IF MID$(A$(Y),X,1)=" " AND N=3 THEN T$(Y)=LEFT$(T$(Y),X-1)+"*"+RIGHT$(T$(Y),80-X)
600       IF MID$(A$(Y),X,1)="*" AND N=2 THEN T$(Y)=LEFT$(T$(Y),X-1)+"*"+RIGHT$(T$(Y),80-X)
610       IF MID$(A$(Y),X,1)="*" AND N=3 THEN T$(Y)=LEFT$(T$(Y),X-1)+"*"+RIGHT$(T$(Y),80-X)
620       '
630     NEXT 
640   NEXT 
650   '
660   FOR I=1 TO 20
670     A$(I)=T$(I)
680     T$(I)=SPACE$(80)
690   NEXT 
700   '
710   LOCATE 1,1
720   SETCURSOR 0
730   '
740   FOR I=1 TO 20
750     PRINT A$(I)
760   NEXT 
770 NEXT 
