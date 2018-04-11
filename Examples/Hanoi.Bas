100 '---------------------------------------------------------
110 '   The Towers Of Hanoi
120 '   Copyright (C) 1998 Amit Singh. All Rights Reserved.
130 '   http://hanoi.kernelthread.com
140 '   Animation and minor changes by Kevin Diggins (2018)
150 '---------------------------------------------------------
160 CLS
170 LOCATE 15,5 : PRINT "       THE TOWERS OF HANOI ANIMATION USING 5 DISKS"
180 LOCATE 16,5 : PRINT "-------------------------------------------------------------"
190 LOCATE 17,5 : PRINT "Rule 1 - Only one disk can be moved at a time."
200 LOCATE 18,5 : PRINT "Rule 2 - Each move consists of taking the upper disk from one"
210 LOCATE 19,5 : PRINT "         of the stacks and placing it on top of another stack."
220 LOCATE 20,5 : PRINT "Rule 3 - No disk may be placed on top of a smaller disk."
230 SETCURSOR 0
240 GOSUB 1490
250 DIM S(4096)' --- S is the stack
260 B = 1' --- b is the stack pointer
270 T = 2' --- _sfTO
280 F = 0' --- _sfFR
290 U = 1' --- _sfUS
300 N = 3' --- _sfNU
310 E = 0' --- _sfEN
320 M = 0' --- remain
330 ' --- the stack variable
340 'INPUT "Number of disks (0 < disks < 10)",V
350 V = 5
360 Peg1 = V
370 IF V > 0 AND V < 11 THEN  GOTO 410 ELSE 380
380 PRINT "Illegal number of disks"
390  GOTO 340
400 ' --- PUSH N (=v here)
410 GOSUB 910
420 V = 1
430 GOSUB 910
440 V = 3
450 GOSUB 910
460 V = 0
470 GOSUB 910
480 WHILE B > 1
490   GOSUB 950
500   E = V
510   GOSUB 950
520   T = V
530   GOSUB 950
540   F = V
550   GOSUB 950
560   N = V
570   M = 6-F-T
580   IF E <> 0 THEN  GOTO 590 ELSE 690
590   GOSUB 1000
600   V = N-1
610   GOSUB 910
620   V = M
630   GOSUB 910
640   V = T
650   GOSUB 910
660   V = 0
670   GOSUB 910
680    GOTO 880
690   IF N <> 1 THEN  GOTO 700 ELSE 870
700   V = N
710   GOSUB 910
720   V = F
730   GOSUB 910
740   V = T
750   GOSUB 910
760   V = 1
770   GOSUB 910
780   V = N-1
790   GOSUB 910
800   V = F
810   GOSUB 910
820   V = M
830   GOSUB 910
840   V = 0
850   GOSUB 910
860    GOTO 880
870   GOSUB 1000
880 WEND 
890 END
900 ' --- procedure push(v)
910 S(B) = V
920 B = B+1
930 RETURN
940 ' --- procedure pop()
950 IF B = 0 THEN  GOTO 980 ELSE 960
960 B = B-1
970 V = S(B)
980 RETURN
990 ' --- procedure movedisk(from, to)
1000 'PRINT "moving from ";F;" to ";T;
1010 '
1020 IF F = 1 THEN Peg1 = Peg1-1
1030 IF F = 2 THEN Peg2 = Peg2-1
1040 IF F = 3 THEN Peg3 = Peg3-1
1050 IF T = 1 THEN Peg1 = Peg1+1
1060 IF T = 2 THEN Peg2 = Peg2+1
1070 IF T = 3 THEN Peg3 = Peg3+1
1080 IF Peg1 < 0 THEN Peg1 = 0
1090 IF Peg2 < 0 THEN Peg2 = 0
1100 IF Peg3 < 0 THEN Peg3 = 0
1110 '
1120 ' redraw peg 1
1130 '
1140 FOR Yy = 1 TO 10
1150   LOCATE Yy,10
1160   PRINT SPACE$(20)
1170 NEXT 
1180 '
1190 FOR Yy = 0 TO Peg1-1
1200   LOCATE Yy+1,10
1210   PRINT ("========")
1220 NEXT 
1230 SLEEP(200)
1240 '
1250 ' redraw peg 2
1260 '
1270 FOR Yy = 1 TO 10
1280   LOCATE Yy,30
1290   PRINT SPACE$(20)
1300 NEXT 
1310 FOR Yy = 0 TO Peg2-1
1320   LOCATE Yy+1,30
1330   PRINT ("========")
1340 NEXT 
1350 SLEEP(200)
1360 '
1370 ' redraw peg 3
1380 '
1390 FOR Yy = 1 TO 10
1400   LOCATE Yy,50
1410   PRINT SPACE$(20)
1420 NEXT 
1430 FOR Yy = 0 TO Peg3-1
1440   LOCATE Yy+1,50
1450   PRINT ("========")
1460 NEXT 
1470 SLEEP(200)
1480 RETURN
1490 LOCATE 5,20 : PRINT "Demonstration starting in 3 SECONDS" : SLEEP(1000)
1500 LOCATE 5,20 : PRINT "Demonstration starting in 2 SECONDS" : SLEEP(1000)
1510 LOCATE 5,20 : PRINT "Demonstration starting in 1 SECONDS" : SLEEP(1000)
1520 LOCATE 5,30 : PRINT SPACE$(40)
1530 RETURN
