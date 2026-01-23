100 Block$ = SPACE$(10)
110 X = 1: Y = 1
120 COLOR 1,0
130 CLS
140 COLOR 0,14
150 GOTOXY X,Y
160 PRINT Block$
170 SETCURSOR 0
180 DO 
190   COLOR 1,0
200   K = KEYPRESS
210    ' -----------------------------------------
220   IF K = 27 THEN 400 '  Escape Key = QUIT
230    ' -----------------------------------------
240   COLOR 0,0
250   GOTOXY X,Y
260   PRINT Block$
270   IF K = -80 THEN Y = Y+1
280   IF K = -72 THEN Y = Y-1
290   IF K = -75 THEN X = X-5
300   IF K = -77 THEN X = X+5
310   IF Y < 1 THEN Y = 1
320   IF Y > 43 THEN Y = 43
330   IF X < 1 THEN X = 1
340   IF X > 70 THEN X = 70
350   COLOR 0,14
360   GOTOXY X,Y
370   PRINT Block$
380   SETCURSOR 0
390 LOOP 
400 COLOR 10,0: CLS: SETCURSOR 1: END
