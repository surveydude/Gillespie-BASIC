 '  ********************************************************
 '           QuickBasic 4.5 compatible code
 '  ********************************************************
 '                       Expected
 '    2  3  5  7  11  13  17  19  23  29  31  37  41  43  47
 '  ********************************************************

FOR A = 1 TO 50
  IF Isprime(A) THEN PRINT A;
NEXT A



14 FUNCTION Isprime(Num)
  IF Num < 2 THEN 
    Isprime = 0
  ELSEIF Num = 2 THEN 
    Isprime = -1
  ELSEIF Num MOD 2 = 0 THEN 
    Isprime = 0
   ELSE 
    Isprime = -1
    FOR X = 3 TO SQR(Num) STEP 2
      IF Num MOD X = 0 THEN 
        Isprime = 0
        EXIT FUNCTION 
      END IF 
    NEXT 
  END IF 
END FUNCTION 



