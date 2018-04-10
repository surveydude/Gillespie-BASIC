100 CLS
110 PRINT "******************************************************************"
120 PRINT "A simple demonstration showing a usage of LOG and LOG10 functions."
130 PRINT "-LOGS- are simply another way of representing numbers.  Before"
140 PRINT "calculators and computers were invented, LOGS were used to make"
150 PRINT "multiplying and dividing numbers much easier. LOG BOOKS still exist"
160 PRINT "containing tables of LOGS indexed by their decimal equivalents."
170 PRINT 
180 PRINT "To multiply two numbers together using LOGS, one first looks up"
190 PRINT "the LOGS of the two numbers and then ADDS the LOGS together."
200 PRINT "The final step is to raise that sum to the base of the LOG that"
210 PRINT "you are using - sometimes referred to as finding the anti-log or"
220 PRINT "more simply, converting it to a normal decimal number.  If you "
230 PRINT "are using Base-10 (decimal), your LOG BOOK (or computer function)"
240 PRINT "would need to provide you with LOGS derived using decimal(base 10)"
250 PRINT 
260 PRINT "On the other hand, NATURAL LOGS use Euler's constant (e) for their"
270 PRINT "base. Euler's constant, like PI, is a very long irrational number"
280 PRINT "found throughout nature. Euler's constant is approximately"
290 PRINT "2.718281828459045 and which we will use below in this example."
300 PRINT "******************************************************************"
310 PRINT "One final point, just as adding 2 LOGS results in multiplication,"
320 PRINT "subtracting one LOG from another results in division."
330 PRINT "Anyone who has ever done long division will quickly appreciate"
340 PRINT "how much easier that operation is using LOGS"
350 PRINT "******************************************************************"
360 PRINT '   Our first demonstration squares a number using Natural Logs
370 '           The variable "A" will be the number that we square
380 A = 7
390 Euler = 2.718281828459045
400 B = LOG(A)'         Obtain the natural log of our number
410 C = B+B'     Remember, to multiply 2 numbers, simply add their logs
420 '      To convert our result to back to decimal, we raise it by its base
430 PRINT "7x7 solved using base-e (Natural) LOGS: ",Euler^C
440 PRINT 
450 PRINT "******************************************************************"
460 PRINT "Now the same idea but this time using the LOG10 function"
470 PRINT "******************************************************************"
480 A = 7
490 B = LOG10(A)
500 C = B+B
510 'This time we use base 10 instead of Euler's constant
520 PRINT "7x7 solved using base-10 LOGS: ",10^C
