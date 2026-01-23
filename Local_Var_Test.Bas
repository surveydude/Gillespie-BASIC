PRINT Test(5)
PRINT I '          This is a global numeric variable
PRINT Sum '        This is a global numeric variable
PRINT Temp$ '      This is a global string variable
PRINT Values(5) '  This is a global numeric array
END

FUNCTION Test(X)
    LOCAL   I, Sum, Temp$, Value(10) '  These are distinct from the global variables
    LOCAL   Values(10)
    Sum = 0
    FOR I = 0 TO X
        Values(I) = I*2
        Sum = Sum+Values(I)
    NEXT
    Temp$ = "Result: "
    Test = Sum
END FUNCTION

' Expected Output:
'
' Ready: run
' 30
' 0
' 0
'
' 0
'
' Ready:
