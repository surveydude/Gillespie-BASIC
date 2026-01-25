PRINT "========================================="
PRINT "    CONTINUE COMMAND STRESS TEST"
PRINT "========================================="
PRINT
Errors = 0

' =========================================
' TEST 1: Basic CONTINUE in FOR loop
' =========================================
PRINT "TEST 1: Basic CONTINUE in FOR loop"
PRINT "  Expected: 1 2 3 4 6 7 8 9 10"
PRINT "  Got:      ";
FOR I = 1 TO 10
    IF I = 5 THEN CONTINUE
    PRINT I;
NEXT
PRINT
PRINT

' =========================================
' TEST 2: CONTINUE FOR (explicit)
' =========================================
PRINT "TEST 2: CONTINUE FOR (explicit)"
PRINT "  Expected: 2 4 6 8 10"
PRINT "  Got:      ";
FOR I = 1 TO 10
    IF I MOD 2 = 1 THEN CONTINUE FOR
    PRINT I;
NEXT
PRINT
PRINT

' =========================================
' TEST 3: CONTINUE in WHILE loop
' =========================================
PRINT "TEST 3: CONTINUE in WHILE loop"
PRINT "  Expected: 1 2 4 5"
PRINT "  Got:      ";
X = 0
WHILE X < 5
    X = X + 1
    IF X = 3 THEN CONTINUE
    PRINT X;
WEND
PRINT
PRINT

' =========================================
' TEST 4: CONTINUE WHILE (explicit)
' =========================================
PRINT "TEST 4: CONTINUE WHILE (explicit)"
PRINT "  Expected: 10 20 40 50"
PRINT "  Got:      ";
Y = 0
WHILE Y < 50
    Y = Y + 10
    IF Y = 30 THEN CONTINUE WHILE
    PRINT Y;
WEND
PRINT
PRINT

' =========================================
' TEST 5: CONTINUE in DO loop
' =========================================
PRINT "TEST 5: CONTINUE in DO loop"
PRINT "  Expected: 1 2 4 5"
PRINT "  Got:      ";
Z = 0
DO WHILE Z < 5
    Z = Z + 1
    IF Z = 3 THEN CONTINUE
    PRINT Z;
LOOP
PRINT
PRINT

' =========================================
' TEST 6: CONTINUE DO (explicit)
' =========================================
PRINT "TEST 6: CONTINUE DO (explicit)"
PRINT "  Expected: A B D E"
PRINT "  Got:      ";
N = 0
DO WHILE N < 5
    N = N + 1
    IF N = 3 THEN CONTINUE DO
    PRINT CHR$(64 + N);
LOOP
PRINT
PRINT

' =========================================
' TEST 7: Nested FOR loops with CONTINUE
' =========================================
PRINT "TEST 7: Nested FOR loops - inner CONTINUE"
PRINT "  Expected: [1,1] [1,2] [2,1] [2,2] [3,1] [3,2]"
PRINT "  Got:      ";
FOR I = 1 TO 3
    FOR J = 1 TO 3
        IF J = 3 THEN CONTINUE
        PRINT "["; I; ","; J; "]";
    NEXT J
NEXT I
PRINT
PRINT

' =========================================
' TEST 8: Nested FOR with outer CONTINUE FOR
' =========================================
PRINT "TEST 8: Nested FOR - outer CONTINUE FOR"
PRINT "  Expected: [1,1] [1,2] [1,3] [3,1] [3,2] [3,3]"
PRINT "  Got:      ";
FOR I = 1 TO 3
    IF I = 2 THEN CONTINUE FOR
    FOR J = 1 TO 3
        PRINT "["; I; ","; J; "]";
    NEXT J
NEXT I
PRINT
PRINT

' =========================================
' TEST 9: CONTINUE in FUNCTION
' =========================================
PRINT "TEST 9: CONTINUE in FUNCTION"
PRINT "  Expected: 25 (sum of 1+2+4+5+6+7 skipping 3)"
R = TestFuncContinue()
PRINT "  Got:      "; R
IF R <> 25 THEN Errors = Errors + 1
PRINT

' =========================================
' TEST 10: CONTINUE FOR in FUNCTION
' =========================================
PRINT "TEST 10: CONTINUE FOR in FUNCTION"
PRINT "  Expected: 30 (sum of 2+4+6+8+10)"
R = TestFuncContinueFor()
PRINT "  Got:      "; R
IF R <> 30 THEN Errors = Errors + 1
PRINT

' =========================================
' TEST 11: CONTINUE in SUB
' =========================================
PRINT "TEST 11: CONTINUE in SUB"
PRINT "  Expected: 1 2 4 5"
PRINT "  Got:      ";
CALL TestSubContinue()
PRINT
PRINT

' =========================================
' TEST 12: Multiple CONTINUEs in one loop
' =========================================
PRINT "TEST 12: Multiple CONTINUEs in one loop"
PRINT "  Expected: 1 5 7 11 13"
PRINT "  Got:      ";
FOR I = 1 TO 15
    IF I MOD 2 = 0 THEN CONTINUE
    IF I MOD 3 = 0 THEN CONTINUE
    PRINT I;
NEXT
PRINT
PRINT

' =========================================
' TEST 13: CONTINUE with STEP
' =========================================
PRINT "TEST 13: CONTINUE with STEP"
PRINT "  Expected: 2 4 8 10"
PRINT "  Got:      ";
FOR I = 2 TO 10 STEP 2
    IF I = 6 THEN CONTINUE
    PRINT I;
NEXT
PRINT
PRINT

' =========================================
' TEST 14: CONTINUE with negative STEP
' =========================================
PRINT "TEST 14: CONTINUE with negative STEP"
PRINT "  Expected: 10 8 4 2"
PRINT "  Got:      ";
FOR I = 10 TO 2 STEP -2
    IF I = 6 THEN CONTINUE
    PRINT I;
NEXT
PRINT
PRINT

' =========================================
' TEST 15: Mixed nested loops
' =========================================
PRINT "TEST 15: Mixed nested loops (FOR/WHILE/DO)"
PRINT "  Expected count: 18"
Count = 0
FOR I = 1 TO 3
    J = 0
    WHILE J < 3
        J = J + 1
        K = 0
        DO WHILE K < 3
            K = K + 1
            IF K = 2 THEN CONTINUE DO
            Count = Count + 1
        LOOP
    WEND
NEXT
PRINT "  Got count: "; Count
IF Count <> 18 THEN Errors = Errors + 1
PRINT

' =========================================
' RESULTS
' =========================================
PRINT "========================================="
PRINT "         TEST COMPLETE"
PRINT "========================================="
IF Errors = 0 THEN
    PRINT "  All numeric tests PASSED!"
ELSE
    PRINT "  ERRORS detected: "; Errors
END IF
PRINT "  Please verify printed output above"
PRINT "========================================="
END

' =========================================
' TEST FUNCTIONS AND SUBS
' =========================================

FUNCTION TestFuncContinue()
    DIM Total
    Total = 0
    FOR I = 1 TO 7
        IF I = 3 THEN CONTINUE
        Total = Total + I
    NEXT
    FUNCTION = Total
END FUNCTION

FUNCTION TestFuncContinueFor()
    DIM Sum
    Sum = 0
    FOR I = 1 TO 10
        IF I MOD 2 = 1 THEN CONTINUE FOR
        Sum = Sum + I
    NEXT
    FUNCTION = Sum
END FUNCTION

SUB TestSubContinue()
    FOR I = 1 TO 5
        IF I = 3 THEN CONTINUE
        PRINT I;
    NEXT
END SUB
