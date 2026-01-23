' Test pass-by-reference
CLS
PRINT "Testing pass-by-reference"
PRINT "========================="

' Test 1: Simple variable modification
A = 10
PRINT "Before: A = "; A
CALL SetTo99(A)
PRINT "After SetTo99(A): A = "; A
PRINT "Expected: 99"
PRINT

' Test 2: Pass expression (should be by value, no change)
B = 10
PRINT "Before: B = "; B
CALL SetTo99(B + 0)
PRINT "After SetTo99(B + 0): B = "; B
PRINT "Expected: 10 (unchanged, passed by value)"
PRINT

' Test 3: Two by-ref parameters (no DIM inside)
X = 1
Y = 2
PRINT "Before: X = "; X; " Y = "; Y
CALL AddToBoth(X, Y)
PRINT "After AddToBoth: X = "; X; " Y = "; Y
PRINT "Expected: X = 11, Y = 12"
PRINT

' Test 4: Stack-like push/pop
DIM S(10)
SP = 1
PRINT "Push 5, 10, 15 onto stack"
CALL Push(5)
CALL Push(10)
CALL Push(15)
PRINT "SP after pushes: "; SP
PRINT "Expected: 4"

V = 0
CALL Pop(V)
PRINT "Pop: V = "; V; " (expected 15)"
CALL Pop(V)
PRINT "Pop: V = "; V; " (expected 10)"
CALL Pop(V)
PRINT "Pop: V = "; V; " (expected 5)"
PRINT "SP after pops: "; SP
PRINT "Expected: 1"

PRINT
PRINT "All tests complete!"
END

SUB SetTo99(num)
    num = 99
END SUB

SUB AddToBoth(a, b)
    a = a + 10
    b = b + 10
END SUB

SUB Push(value)
    S(SP) = value
    SP = SP + 1
END SUB

SUB Pop(result)
    SP = SP - 1
    result = S(SP)
END SUB