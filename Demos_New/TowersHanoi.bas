MAXDISKS = 7

DIM Peg(3, MAXDISKS)
DIM PegCount(3)

CLS
LOCATE 15, 5: PRINT "             THE TOWERS OF HANOI - Gillespie BASIC"
LOCATE 16, 5: PRINT "--------------------------------------------------------------"
LOCATE 18, 5: PRINT "         Rule 1 - Only one disk can be moved at a time."
LOCATE 19, 5: PRINT "         Rule 2 - Smaller disks must stay on top."
SETCURSOR 0

CALL Countdown

' ---------- Initialize pegs ----------
PegCount(1) = MAXDISKS
PegCount(2) = 0
PegCount(3) = 0

FOR I = 1 TO MAXDISKS
    Peg(1, I) = MAXDISKS - I + 1
NEXT

CALL DrawPegs

' ---------- Solve ----------
CALL Hanoi(MAXDISKS, 1, 3, 2)

LOCATE 22, 30
PRINT "All Done!" 
END

' =====================================================
'             Recursive Hanoi solver
' =====================================================

SUB Hanoi (N, FromPeg, ToPeg, AuxPeg)
    IF N = 0 THEN EXIT SUB
    SLEEP (16)
    CALL Hanoi(N - 1, FromPeg, AuxPeg, ToPeg)
    SLEEP (16)
    CALL MoveDisk(FromPeg, ToPeg)
    SLEEP (16)
    CALL Hanoi(N - 1, AuxPeg, ToPeg, FromPeg)
END SUB

' =====================================================
' Move disk
' =====================================================

SUB MoveDisk (FromPeg, ToPeg)

    Disk = Peg(FromPeg, PegCount(FromPeg))
    PegCount(FromPeg) = PegCount(FromPeg) - 1

    PegCount(ToPeg) = PegCount(ToPeg) + 1
    Peg(ToPeg, PegCount(ToPeg)) = Disk

    CALL DrawPegs
    SLEEP 1
END SUB

' =====================================================
' Draw pegs and disks
' =====================================================

SUB DrawPegs
    FOR P = 1 TO 3
        X = 7 + (P - 1) * 20
        FOR Y = 1 TO 12
            LOCATE Y, X
            PRINT SPACE$(20)
        NEXT

        FOR I = 1 TO PegCount(P)
            Disk = Peg(P, I)
            W = Disk * 2 + 1
            LOCATE 12 - I, X + (10 - W / 2)
            PRINT REPEAT$("=", W)
        NEXT
    NEXT
END SUB

' =====================================================
' Countdown
' =====================================================

SUB Countdown
    LOCATE 5, 30: PRINT "Starting in 3..."
    SLEEP 500
    LOCATE 5, 30: PRINT "Starting in 2..."
    SLEEP 500
    LOCATE 5, 30: PRINT "Starting in 1..."
    SLEEP 500
    LOCATE 5, 30: PRINT SPACE$(20)
END SUB
