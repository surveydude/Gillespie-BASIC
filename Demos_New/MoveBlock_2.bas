CALL Initialize

DO
    K = KEYPRESS
    IF K = 27 THEN EXIT DO ' Escape Key = QUIT
    CALL MoveBlock
LOOP
 
COLOR 10, 0
CLS
SETCURSOR 1
END

SUB Initialize
    Block$ = SPACE$(10)
    X = 1
    Y = 1
    COLOR 1, 0 : CLS : COLOR 0, 14
    GOTOXY X, Y
    PRINT Block$
    SETCURSOR 0
END SUB

SUB MoveBlock
    COLOR 0, 0
    GOTOXY X, Y
    PRINT Block$
    IF K = -80 THEN Y = Y+1
    IF K = -72 THEN Y = Y-1
    IF K = -75 THEN X = X-5
    IF K = -77 THEN X = X+5
    IF Y < 1 THEN Y = 1
    IF Y > 43 THEN Y = 43
    IF X < 1 THEN X = 1
    IF X > 70 THEN X = 70
    COLOR 0, 14
    GOTOXY X, Y
    PRINT Block$
    SETCURSOR 0
END SUB
