' ============================================================================
' ELSEIF Test Suite for Gillespie BASIC
' Each test prints what it expects, making failures easy to spot
' ============================================================================

PRINT "========================================"
PRINT "       ELSEIF TEST SUITE"
PRINT "========================================"
PRINT ""

' ============================================================================
' TEST 1: Basic ELSEIF - First condition true
' ============================================================================
PRINT "TEST 1: First IF condition true"
PRINT "Expected: ONE"
x = 1
IF x = 1 THEN
    PRINT "ONE"
ELSEIF x = 2 THEN
    PRINT "TWO"
ELSEIF x = 3 THEN
    PRINT "THREE"
ELSE
    PRINT "OTHER"
END IF
PRINT ""

' ============================================================================
' TEST 2: Basic ELSEIF - Second condition true (first ELSEIF)
' ============================================================================
PRINT "TEST 2: First ELSEIF condition true"
PRINT "Expected: TWO"
x = 2
IF x = 1 THEN
    PRINT "ONE"
ELSEIF x = 2 THEN
    PRINT "TWO"
ELSEIF x = 3 THEN
    PRINT "THREE"
ELSE
    PRINT "OTHER"
END IF
PRINT ""

' ============================================================================
' TEST 3: Basic ELSEIF - Third condition true (second ELSEIF)
' ============================================================================
PRINT "TEST 3: Second ELSEIF condition true"
PRINT "Expected: THREE"
x = 3
IF x = 1 THEN
    PRINT "ONE"
ELSEIF x = 2 THEN
    PRINT "TWO"
ELSEIF x = 3 THEN
    PRINT "THREE"
ELSE
    PRINT "OTHER"
END IF
PRINT ""

' ============================================================================
' TEST 4: Basic ELSEIF - No conditions true, ELSE executes
' ============================================================================
PRINT "TEST 4: No conditions true - ELSE branch"
PRINT "Expected: OTHER"
x = 99
IF x = 1 THEN
    PRINT "ONE"
ELSEIF x = 2 THEN
    PRINT "TWO"
ELSEIF x = 3 THEN
    PRINT "THREE"
ELSE
    PRINT "OTHER"
END IF
PRINT ""

' ============================================================================
' TEST 5: ELSEIF without ELSE clause - condition true
' ============================================================================
PRINT "TEST 5: ELSEIF without ELSE - match found"
PRINT "Expected: FOUND-B"
x = 2
IF x = 1 THEN
    PRINT "FOUND-A"
ELSEIF x = 2 THEN
    PRINT "FOUND-B"
ELSEIF x = 3 THEN
    PRINT "FOUND-C"
END IF
PRINT ""

' ============================================================================
' TEST 6: ELSEIF without ELSE clause - no match
' ============================================================================
PRINT "TEST 6: ELSEIF without ELSE - no match (nothing printed)"
PRINT "Expected: (blank line follows)"
x = 99
IF x = 1 THEN
    PRINT "FOUND-A"
ELSEIF x = 2 THEN
    PRINT "FOUND-B"
ELSEIF x = 3 THEN
    PRINT "FOUND-C"
END IF
PRINT ""

' ============================================================================
' TEST 7: Single ELSEIF (no chain)
' ============================================================================
PRINT "TEST 7: Single ELSEIF - first true"
PRINT "Expected: ALPHA"
x = 1
IF x = 1 THEN
    PRINT "ALPHA"
ELSEIF x = 2 THEN
    PRINT "BETA"
END IF
PRINT ""

PRINT "TEST 8: Single ELSEIF - second true"
PRINT "Expected: BETA"
x = 2
IF x = 1 THEN
    PRINT "ALPHA"
ELSEIF x = 2 THEN
    PRINT "BETA"
END IF
PRINT ""

' ============================================================================
' TEST 9: Multiple statements in each branch
' ============================================================================
PRINT "TEST 9: Multiple statements in ELSEIF branch"
PRINT "Expected: START-MIDDLE-END"
x = 2
IF x = 1 THEN
    PRINT "WRONG1"
    PRINT "WRONG2"
ELSEIF x = 2 THEN
    PRINT "START-";
    PRINT "MIDDLE-";
    PRINT "END"
ELSEIF x = 3 THEN
    PRINT "WRONG3"
    PRINT "WRONG4"
END IF
PRINT ""

' ============================================================================
' TEST 10: Nested IF inside ELSEIF
' ============================================================================
PRINT "TEST 10: Nested IF inside ELSEIF"
PRINT "Expected: NESTED-INNER"
x = 2
y = 1
IF x = 1 THEN
    PRINT "OUTER-IF"
ELSEIF x = 2 THEN
    IF y = 1 THEN
        PRINT "NESTED-INNER"
    ELSE
        PRINT "NESTED-ELSE"
    END IF
ELSE
    PRINT "OUTER-ELSE"
END IF
PRINT ""

' ============================================================================
' TEST 11: Nested IF inside IF, with outer ELSEIF
' ============================================================================
PRINT "TEST 11: Nested IF in true IF branch, then ELSEIF skipped"
PRINT "Expected: INNER-MATCH"
x = 1
y = 2
IF x = 1 THEN
    IF y = 1 THEN
        PRINT "Y-IS-ONE"
    ELSEIF y = 2 THEN
        PRINT "INNER-MATCH"
    ELSE
        PRINT "Y-OTHER"
    END IF
ELSEIF x = 2 THEN
    PRINT "SHOULD-NOT-SEE-THIS"
END IF
PRINT ""

' ============================================================================
' TEST 12: Deeply nested ELSEIF chains
' ============================================================================
PRINT "TEST 12: Deeply nested ELSEIF"
PRINT "Expected: DEEP-HIT"
a = 2
b = 3
c = 1
IF a = 1 THEN
    PRINT "A1"
ELSEIF a = 2 THEN
    IF b = 1 THEN
        PRINT "B1"
    ELSEIF b = 2 THEN
        PRINT "B2"
    ELSEIF b = 3 THEN
        IF c = 1 THEN
            PRINT "DEEP-HIT"
        ELSEIF c = 2 THEN
            PRINT "C2"
        END IF
    END IF
ELSEIF a = 3 THEN
    PRINT "A3"
END IF
PRINT ""

' ============================================================================
' TEST 13: ELSEIF with complex expressions
' ============================================================================
PRINT "TEST 13: Complex ELSEIF expressions"
PRINT "Expected: COMPLEX-MATCH"
x = 5
y = 10
IF x + y = 10 THEN
    PRINT "SUM-10"
ELSEIF x * y = 50 THEN
    PRINT "COMPLEX-MATCH"
ELSEIF x - y = 5 THEN
    PRINT "DIFF-5"
END IF
PRINT ""

' ============================================================================
' TEST 14: ELSEIF with string comparisons
' ============================================================================
PRINT "TEST 14: ELSEIF with strings"
PRINT "Expected: HELLO-MATCH"
s$ = "hello"
IF s$ = "world" THEN
    PRINT "WORLD-MATCH"
ELSEIF s$ = "hello" THEN
    PRINT "HELLO-MATCH"
ELSEIF s$ = "foo" THEN
    PRINT "FOO-MATCH"
END IF
PRINT ""

' ============================================================================
' TEST 15: ELSEIF with AND/OR logic
' ============================================================================
PRINT "TEST 15: ELSEIF with AND logic"
PRINT "Expected: BOTH-TRUE"
x = 5
y = 10
IF x = 1 AND y = 1 THEN
    PRINT "FIRST"
ELSEIF x = 5 AND y = 10 THEN
    PRINT "BOTH-TRUE"
ELSEIF x = 5 OR y = 20 THEN
    PRINT "OR-MATCH"
END IF
PRINT ""

' ============================================================================
' TEST 16: Many ELSEIF branches (stress test)
' ============================================================================
PRINT "TEST 16: Many ELSEIF branches - match #7"
PRINT "Expected: SEVEN"
n = 7
IF n = 1 THEN
    PRINT "ONE"
ELSEIF n = 2 THEN
    PRINT "TWO"
ELSEIF n = 3 THEN
    PRINT "THREE"
ELSEIF n = 4 THEN
    PRINT "FOUR"
ELSEIF n = 5 THEN
    PRINT "FIVE"
ELSEIF n = 6 THEN
    PRINT "SIX"
ELSEIF n = 7 THEN
    PRINT "SEVEN"
ELSEIF n = 8 THEN
    PRINT "EIGHT"
ELSEIF n = 9 THEN
    PRINT "NINE"
ELSEIF n = 10 THEN
    PRINT "TEN"
ELSE
    PRINT "OTHER"
END IF
PRINT ""

' ============================================================================
' TEST 17: Variable modification in branches
' ============================================================================
PRINT "TEST 17: Variable modification in ELSEIF"
PRINT "Expected: RESULT=200"
x = 2
result = 0
IF x = 1 THEN
    result = 100
ELSEIF x = 2 THEN
    result = 200
ELSEIF x = 3 THEN
    result = 300
END IF
PRINT "RESULT="; result
PRINT ""

' ============================================================================
' TEST 18: ELSEIF after code in IF block
' ============================================================================
PRINT "TEST 18: Code before ELSEIF is skipped"
PRINT "Expected: SECOND-BRANCH"
x = 2
IF x = 1 THEN
    PRINT "FIRST-A"
    PRINT "FIRST-B"
    PRINT "FIRST-C"
ELSEIF x = 2 THEN
    PRINT "SECOND-BRANCH"
END IF
PRINT ""

' ============================================================================
' TEST 19: Ensure only ONE branch executes
' ============================================================================
PRINT "TEST 19: Only first matching branch executes"
PRINT "Expected: FIRST-MATCH (only)"
x = 5
IF x > 0 THEN
    PRINT "FIRST-MATCH"
ELSEIF x > 3 THEN
    PRINT "SECOND-MATCH-WRONG"
ELSEIF x > 4 THEN
    PRINT "THIRD-MATCH-WRONG"
ELSE
    PRINT "ELSE-WRONG"
END IF
PRINT ""

' ============================================================================
' TEST 20: ELSEIF in a SUB
' ============================================================================
PRINT "TEST 20: ELSEIF inside SUB"
PRINT "Expected: SUB-ELSEIF-HIT"
CALL TestSub(2)
PRINT ""

' ============================================================================
' TEST 21: ELSEIF in a FUNCTION
' ============================================================================
PRINT "TEST 21: ELSEIF inside FUNCTION"
PRINT "Expected: 200"
PRINT GetValue(2)
PRINT ""

' ============================================================================
' TEST 22: Sequential IF-ELSEIF blocks (independent)
' ============================================================================
PRINT "TEST 22: Two independent IF-ELSEIF blocks"
PRINT "Expected: BLOCK1-B then BLOCK2-A"
x = 2
IF x = 1 THEN
    PRINT "BLOCK1-A"
ELSEIF x = 2 THEN
    PRINT "BLOCK1-B"
END IF

y = 1
IF y = 1 THEN
    PRINT "BLOCK2-A"
ELSEIF y = 2 THEN
    PRINT "BLOCK2-B"
END IF
PRINT ""

' ============================================================================
' TEST 23: ELSEIF with GOSUB in branch
' ============================================================================
PRINT "TEST 23: GOSUB inside ELSEIF"
PRINT "Expected: BEFORE-SUBROUTINE-AFTER"
x = 2
PRINT "BEFORE-";
IF x = 1 THEN
    PRINT "WRONG"
ELSEIF x = 2 THEN
    GOSUB MySub
ELSE
    PRINT "ALSO-WRONG"
END IF
PRINT "-AFTER"
GOTO SkipSub

MySub:
PRINT "SUBROUTINE";
RETURN

SkipSub:
PRINT ""

' ============================================================================
' TEST 24: Comparison operators in ELSEIF
' ============================================================================
PRINT "TEST 24: Various comparison operators"
PRINT "Expected: GREATER"
x = 10
IF x < 5 THEN
    PRINT "LESS"
ELSEIF x = 5 THEN
    PRINT "EQUAL"
ELSEIF x > 5 THEN
    PRINT "GREATER"
END IF
PRINT ""

' ============================================================================
' TEST 25: Negative numbers in conditions
' ============================================================================
PRINT "TEST 25: Negative number conditions"
PRINT "Expected: NEGATIVE"
x = -5
IF x > 0 THEN
    PRINT "POSITIVE"
ELSEIF x = 0 THEN
    PRINT "ZERO"
ELSEIF x < 0 THEN
    PRINT "NEGATIVE"
END IF
PRINT ""

PRINT "========================================"
PRINT "       ALL TESTS COMPLETE"
PRINT "========================================"
PRINT ""
PRINT "If all Expected values match, ELSEIF is working!"

END

' ============================================================================
' SUB and FUNCTION definitions
' ============================================================================

SUB TestSub(value)
    IF value = 1 THEN
        PRINT "SUB-IF-HIT"
    ELSEIF value = 2 THEN
        PRINT "SUB-ELSEIF-HIT"
    ELSE
        PRINT "SUB-ELSE-HIT"
    END IF
END SUB

FUNCTION GetValue(n)
    IF n = 1 THEN
        GetValue = 100
    ELSEIF n = 2 THEN
        GetValue = 200
    ELSEIF n = 3 THEN
        GetValue = 300
    ELSE
        GetValue = 0
    END IF
END FUNCTION
