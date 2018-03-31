100 CLS
110 PRINT 
120 PRINT "The VERIFY function returns TRUE(1) if each letter in the" 
130 PRINT "first string is present in the second string, in any position" 
140 PRINT  : PRINT  : PRINT 
150 A=VERIFY("FTW" ,"For The Win!" )
160 PRINT "First test produced: " ,A
170 PRINT 
180 A=VERIFY("FTW" ,"For The Foobar!" )
190 PRINT "Second test produced: " ,A
200 PRINT  : PRINT 
