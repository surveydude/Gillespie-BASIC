100 CLS
110 PRINT "This demonstrates 3 built-in popups."
120 PRINT 
130 '----------------------------------------------
140 A$ = INPUTBOX$("This is your title","This is your special message","This is your Default Value")
150 PRINT A$
160 '----------------------------------------------
170 A = YN_CANCEL("My Cool Title","1=Yes, 2=No, 3=Cancel")
180 PRINT A
190 '----------------------------------------------
200 '
210 OK_CANCEL "A Boring Title","A Boring Message"
