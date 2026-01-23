' ============================================================================
' SELECT CASE Test Suite for Gillespie BASIC
' ============================================================================

PRINT "========================================"
PRINT "     SELECT CASE TEST SUITE"
PRINT "========================================"
PRINT ""

' ============================================================================
' TEST 1: Basic SELECT CASE - first case matches
' ============================================================================
PRINT "TEST 1: First CASE matches"
PRINT "Expected: ONE"
x = 1
SELECT CASE x
  CASE 1
    PRINT "ONE"
  CASE 2
    PRINT "TWO"
  CASE 3
    PRINT "THREE"
END SELECT
PRINT ""

' ============================================================================
' TEST 2: Basic SELECT CASE - second case matches
' ============================================================================
PRINT "TEST 2: Second CASE matches"
PRINT "Expected: TWO"
x = 2
SELECT CASE x
  CASE 1
    PRINT "ONE"
  CASE 2
    PRINT "TWO"
  CASE 3
    PRINT "THREE"
END SELECT
PRINT ""

' ============================================================================
' TEST 3: Basic SELECT CASE - third case matches
' ============================================================================
PRINT "TEST 3: Third CASE matches"
PRINT "Expected: THREE"
x = 3
SELECT CASE x
  CASE 1
    PRINT "ONE"
  CASE 2
    PRINT "TWO"
  CASE 3
    PRINT "THREE"
END SELECT
PRINT ""

' ============================================================================
' TEST 4: CASE ELSE - no match
' ============================================================================
PRINT "TEST 4: CASE ELSE (no match)"
PRINT "Expected: OTHER"
x = 99
SELECT CASE x
  CASE 1
    PRINT "ONE"
  CASE 2
    PRINT "TWO"
  CASE ELSE
    PRINT "OTHER"
END SELECT
PRINT ""

' ============================================================================
' TEST 5: String SELECT CASE
' ============================================================================
PRINT "TEST 5: String SELECT CASE"
PRINT "Expected: HELLO-MATCH"
s$ = "hello"
SELECT CASE s$
  CASE "world"
    PRINT "WORLD-MATCH"
  CASE "hello"
    PRINT "HELLO-MATCH"
  CASE "foo"
    PRINT "FOO-MATCH"
END SELECT
PRINT ""

' ============================================================================
' TEST 6: String CASE ELSE
' ============================================================================
PRINT "TEST 6: String CASE ELSE"
PRINT "Expected: NO-MATCH"
s$ = "bar"
SELECT CASE s$
  CASE "hello"
    PRINT "HELLO"
  CASE "world"
    PRINT "WORLD"
  CASE ELSE
    PRINT "NO-MATCH"
END SELECT
PRINT ""

' ============================================================================
' TEST 7: Multiple statements in CASE
' ============================================================================
PRINT "TEST 7: Multiple statements in CASE"
PRINT "Expected: A then B then C"
x = 2
SELECT CASE x
  CASE 1
    PRINT "WRONG"
  CASE 2
    PRINT "A"
    PRINT "B"
    PRINT "C"
  CASE 3
    PRINT "ALSO WRONG"
END SELECT
PRINT ""

' ============================================================================
' TEST 8: Only first matching CASE executes
' ============================================================================
PRINT "TEST 8: Only first match executes"
PRINT "Expected: FIRST"
x = 5
SELECT CASE x
  CASE 5
    PRINT "FIRST"
  CASE 5
    PRINT "SECOND-WRONG"
  CASE ELSE
    PRINT "ELSE-WRONG"
END SELECT
PRINT ""

' ============================================================================
' TEST 9: Expression in SELECT CASE
' ============================================================================
PRINT "TEST 9: Expression in SELECT"
PRINT "Expected: TEN"
a = 3
b = 7
SELECT CASE a + b
  CASE 5
    PRINT "FIVE"
  CASE 10
    PRINT "TEN"
  CASE 15
    PRINT "FIFTEEN"
END SELECT
PRINT ""

' ============================================================================
' TEST 10: Variable modification in CASE
' ============================================================================
PRINT "TEST 10: Variable modification"
PRINT "Expected: RESULT=200"
x = 2
result = 0
SELECT CASE x
  CASE 1
    result = 100
  CASE 2
    result = 200
  CASE 3
    result = 300
END SELECT
PRINT "RESULT="; result
PRINT ""

' ============================================================================
' TEST 11: EXIT SELECT
' ============================================================================
PRINT "TEST 11: EXIT SELECT"
PRINT "Expected: BEFORE then AFTER (no SHOULD-NOT-SEE)"
x = 2
SELECT CASE x
  CASE 1
    PRINT "WRONG"
  CASE 2
    PRINT "BEFORE"
    EXIT SELECT
    PRINT "SHOULD-NOT-SEE"
  CASE ELSE
    PRINT "ALSO WRONG"
END SELECT
PRINT "AFTER"
PRINT ""

' ============================================================================
' TEST 12: SELECT CASE in SUB
' ============================================================================
PRINT "TEST 12: SELECT CASE in SUB"
PRINT "Expected: SUB-CASE-2"
CALL TestSelectSub(2)
PRINT ""

' ============================================================================
' TEST 13: SELECT CASE in FUNCTION
' ============================================================================
PRINT "TEST 13: SELECT CASE in FUNCTION"
PRINT "Expected: 200"
PRINT GetSelectValue(2)
PRINT ""

' ============================================================================
' TEST 14: Nested SELECT CASE
' ============================================================================
PRINT "TEST 14: Nested SELECT CASE"
PRINT "Expected: INNER-B"
x = 2
y = 2
SELECT CASE x
  CASE 1
    PRINT "OUTER-1"
  CASE 2
    SELECT CASE y
      CASE 1
        PRINT "INNER-A"
      CASE 2
        PRINT "INNER-B"
      CASE ELSE
        PRINT "INNER-ELSE"
    END SELECT
  CASE ELSE
    PRINT "OUTER-ELSE"
END SELECT
PRINT ""

' ============================================================================
' TEST 15: SELECT CASE with no match and no ELSE
' ============================================================================
PRINT "TEST 15: No match, no ELSE (nothing printed)"
PRINT "Expected: (blank line follows)"
x = 99
SELECT CASE x
  CASE 1
    PRINT "ONE"
  CASE 2
    PRINT "TWO"
END SELECT
PRINT ""

' ============================================================================
' TEST 16: Negative numbers
' ============================================================================
PRINT "TEST 16: Negative numbers"
PRINT "Expected: NEGATIVE"
x = -5
SELECT CASE x
  CASE 5
    PRINT "POSITIVE"
  CASE 0
    PRINT "ZERO"
  CASE -5
    PRINT "NEGATIVE"
END SELECT
PRINT ""

' ============================================================================
' TEST 17: Sequential SELECT blocks
' ============================================================================
PRINT "TEST 17: Sequential SELECT blocks"
PRINT "Expected: BLOCK1-B then BLOCK2-A"
x = 2
SELECT CASE x
  CASE 1
    PRINT "BLOCK1-A"
  CASE 2
    PRINT "BLOCK1-B"
END SELECT

y = 1
SELECT CASE y
  CASE 1
    PRINT "BLOCK2-A"
  CASE 2
    PRINT "BLOCK2-B"
END SELECT
PRINT ""


' ============================================================================
' TEST 18: Multiple values in CASE - numeric match
' ============================================================================
PRINT "TEST 18: Multiple values in CASE (numeric)"
PRINT "Expected: ONE-THRU-FIVE"
A = 3
SELECT CASE A
  CASE 1, 2, 3, 4, 5
    PRINT "ONE-THRU-FIVE"
  CASE 6, 7, 8
    PRINT "SIX-THRU-EIGHT"
  CASE ELSE
    PRINT "OTHER"
END SELECT
PRINT ""

' ============================================================================
' TEST 19: Multiple values in CASE - string match
' ============================================================================
PRINT "TEST 19: Multiple values in CASE (string)"
PRINT "Expected: GREETING"
S$ = "hello"
SELECT CASE S$
  CASE "hi", "hello", "hey", "howdy"
    PRINT "GREETING"
  CASE "bye", "goodbye", "later"
    PRINT "FAREWELL"
  CASE ELSE
    PRINT "UNKNOWN"
END SELECT
PRINT ""

' ============================================================================
' TEST 20: Multiple values - last value matches
' ============================================================================
PRINT "TEST 20: Last value in list matches"
PRINT "Expected: FOUND-IN-LAST"
X = 99
SELECT CASE X
  CASE 1, 2, 3
    PRINT "LOW"
  CASE 10, 20, 30, 40, 50, 60, 70, 80, 90, 99
    PRINT "FOUND-IN-LAST"
  CASE ELSE
    PRINT "NOT-FOUND"
END SELECT
PRINT ""

PRINT "========================================"
PRINT "     ALL TESTS COMPLETE"
PRINT "========================================"
PRINT ""
PRINT "If all Expected values match, SELECT CASE is working!"

END

' ============================================================================
' SUB and FUNCTION definitions
' ============================================================================

SUB TestSelectSub(Value)
  SELECT CASE Value
    CASE 1
      PRINT "SUB-CASE-1"
    CASE 2
      PRINT "SUB-CASE-2"
    CASE ELSE
      PRINT "SUB-CASE-ELSE"
  END SELECT
END SUB

FUNCTION GetSelectValue(n)
  SELECT CASE n
    CASE 1
      GetSelectValue = 100
    CASE 2
      GetSelectValue = 200
    CASE 3
      GetSelectValue = 300
    CASE ELSE
      GetSelectValue = 0
  END SELECT
END FUNCTION
