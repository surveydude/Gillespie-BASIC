100 '------------------------------
110 'Parse a comma separated string
120 '------------------------------
130 CLS
140 PRINT 
150 Z$="1,2,3,4,5,666,777,88888,99999,101010101010101010" 
160 PRINT Z$ : PRINT  : PRINT 
170 '------------
180 GOTO 250
190 '------------
200 PRINT Z$ : PRINT 
210 END
220 '
230 '
240 '------------------
250 ' Parse (Z$)
260 '------------------
270 Sep=0
280 B$="" 
290 Sep=INSTR(Z$,"," )
300 IF Sep>0 THEN GOTO 330
310 GOTO 200
320 '------------------
330 B$=LEFT$(Z$,Sep-1)
340 PRINT B$
350 Z$=MID$(Z$,Sep+1,256)
360 GOTO 250' --- Simulates a Recursive Call ---
