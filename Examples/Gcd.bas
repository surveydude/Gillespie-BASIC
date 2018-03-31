100 CLS
110 '--------------------------------------
120 'Calculate Greatest Common Denominator
130 '        Using Euclid's Method
140 '--------------------------------------
150 A = 561
160 B = 633
170 '------------------------
180 GOSUB 270
190 '------------------------
200 PRINT "The GCD of 561 & 633 is: ",C
210 PRINT 
220 PRINT C,"* ",A/C," = ",A
230 PRINT C,"* ",B/C," = ",B
240 END
250 '---------------------
260 '    Euclid's Method
270 '---------------------
280 Z = 1
290 X = A : Y = B
300 WHILE Z > 0
310   Z = X MOD Y
320   X = Y
330   Y = Z
340 WEND 
350 C = X
360 RETURN
