DIM A(10)
PRINT UBOUND(A)           ' Prints 10 (indices 0-10)

DIM B(5, 20)
PRINT UBOUND(B)           ' Prints 5 (first dimension)
PRINT UBOUND(B, 1)        ' Prints 5 (first dimension)
PRINT UBOUND(B, 2)        ' Prints 20 (second dimension)

PRINT UBOUND(X)           ' Prints -1 (X is not an array)
