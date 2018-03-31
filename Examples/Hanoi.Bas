100 '---------------------------------------------------------
110 ' * The Towers Of Hanoi
120 ' * Copyright (C) 1998 Amit Singh. All Rights Reserved.
130 ' * http://hanoi.kernelthread.com
140 '---------------------------------------------------------
150 DIM S(4096)' --- S is the stack
160 B=1' --- b is the stack pointer
170 T=2' --- _sfTO
180 F=0' --- _sfFR
190 U=1' --- _sfUS
200 N=3' --- _sfNU
210 E=0' --- _sfEN
220 M=0' --- remain
230 ' --- the stack variable
240 INPUT "Number of disks (0 < disks < 10)" ,V
250 IF V>0 AND V<10 THEN GOTO 290 ELSE 260
260 PRINT "Illegal number of disks" 
270 GOTO 240
280 ' --- PUSH N (=v here)
290 GOSUB 790
300 V=1
310 GOSUB 790
320 V=3
330 GOSUB 790
340 V=0
350 GOSUB 790
360 WHILE B>1
370   GOSUB 830
380   E=V
390   GOSUB 830
400   T=V
410   GOSUB 830
420   F=V
430   GOSUB 830
440   N=V
450   M=6-F-T
460   IF E<>0 THEN GOTO 470 ELSE 570
470   GOSUB 880
480   V=N-1
490   GOSUB 790
500   V=M
510   GOSUB 790
520   V=T
530   GOSUB 790
540   V=0
550   GOSUB 790
560   GOTO 760
570   IF N<>1 THEN GOTO 580 ELSE 750
580   V=N
590   GOSUB 790
600   V=F
610   GOSUB 790
620   V=T
630   GOSUB 790
640   V=1
650   GOSUB 790
660   V=N-1
670   GOSUB 790
680   V=F
690   GOSUB 790
700   V=M
710   GOSUB 790
720   V=0
730   GOSUB 790
740   GOTO 760
750   GOSUB 880
760 WEND 
770 END
780 ' --- procedure push(v)
790 S(B)=V
800 B=B+1
810 RETURN
820 ' --- procedure pop()
830 IF B=0 THEN GOTO 860 ELSE 840
840 B=B-1
850 V=S(B)
860 RETURN
870 ' --- procedure movedisk(from, to)
880 PRINT "moving from " ;F;" to " ;T;
890 PRINT "" 
900 RETURN
