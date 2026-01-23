' *************************************************
' Conway's Game of Life in Gillespie BASIC (2026)
' by Kevin Diggins (2026 Update)
' Derived from an old ABC BASIC Packets demo
' *************************************************
Boardx = 80
Boardy = 25
Alive = 2
Dead = 32

DIM B(Boardy, Boardx)
DIM C(Boardy, Boardx)
DIM K(Boardy, Boardx)

Neighbors = 0
X = 0
Y = 0

CALL SetupField
DO
    CALL Cycle
    CALL Display
LOOP


SUB Life(Y, X, K)
    B(Y, X) = Alive
    C(Y, X) = Alive
    K(Y, X) = 1   ' QBCOLOR Blue = 1
END SUB


SUB Death(Y, X, K)
    B(Y, X) = Dead
    C(Y, X) = Dead
    K(Y, X) = 0   ' QBCOLOR Black = 0
END SUB


SUB Cycle
    LOCAL X, Y             ' NOW FIXED: local variables work correctly
    FOR X = 1 TO Boardx-2
        FOR Y = 1 TO Boardy-2
            Neighbors = 0
            IF C(Y-1, X) = Alive THEN Neighbors = Neighbors+1
            IF C(Y-1, X-1) = Alive THEN Neighbors = Neighbors+1
            IF C(Y, X-1) = Alive THEN Neighbors = Neighbors+1
            IF C(Y+1, X-1) = Alive THEN Neighbors = Neighbors+1
            IF C(Y+1, X) = Alive THEN Neighbors = Neighbors+1
            IF C(Y+1, X+1) = Alive THEN Neighbors = Neighbors+1
            IF C(Y, X+1) = Alive THEN Neighbors = Neighbors+1
            IF C(Y-1, X+1) = Alive THEN Neighbors = Neighbors+1
            IF Neighbors < 2 OR Neighbors > 3 THEN B(Y, X) = Dead
            IF Neighbors = 3 THEN B(Y, X) = Alive
        NEXT
    NEXT
END SUB


SUB Display
    LOCAL X, Y             ' NOW FIXED: local variables work correctly
    FOR X = 1 TO Boardx-2
        FOR Y = 1 TO Boardy-2
            LOCATE Y, X
            COLOR K(Y, X), 0

            PRINT CHR$(B(Y, X))
            C(Y, X) = B(Y, X)
        NEXT
    NEXT
END SUB



SUB SetupField
    LOCAL X, Y             ' NOW FIXED: local variables work correctly
    RANDOMIZE TIMER
    CLS
    SETCURSOR 0
    FOR X = 1 TO Boardx-2
        FOR Y = 1 TO Boardy-2

            IF (RND*10) > 5 THEN
                CALL Life(Y, X, K)
            ELSE
                CALL Death(Y, X, K)
            END IF

            COLOR K(Y, X), 0
            LOCATE Y, X
            SETCURSOR 0
            PRINT CHR$(B(Y, X))
        NEXT
    NEXT
END SUB
