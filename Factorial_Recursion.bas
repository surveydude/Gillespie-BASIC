
PRINT Factorial(10)
END

FUNCTION Factorial(N) '  recursive
  LOCAL Result
  IF N <= 1 THEN 
    Factorial = 1
   ELSE 
    Factorial = N*Factorial(N-1)
  END IF 
END FUNCTION 
