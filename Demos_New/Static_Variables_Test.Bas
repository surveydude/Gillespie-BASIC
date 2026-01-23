FOR i = 1 TO 3
    CALL TestStatic
NEXT i



SUB TestStatic
    STATIC counter, message$
    STATIC foo(5)
    
    counter = counter + 1

    foo(3) = foo(3) + counter

    IF counter = 1 THEN
        message$ = "First call"
    ELSE
        message$ = "Called " + STR$(counter) + " times"
    END IF

    PRINT
    PRINT "Counter = "; counter; " Message: "; message$
    PRINT "Foo(3) = Foo(3) + Counter = ", Foo(3)
    PRINT "Foo(3) =", Foo(3)
END SUB
