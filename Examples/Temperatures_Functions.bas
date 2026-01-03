10 FUNCTION Ftoc(F) = (F-32)*5/9
20 FUNCTION Ctof(C) = C*9/5+32
30 INPUT "Fahrenheit ? ",F
40 PRINT F," = ",Ftoc(F)," C"
50 INPUT "Celsius ? ",C
60 PRINT C," = ",Ctof(C)," F"