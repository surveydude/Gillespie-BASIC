100   '*************************************************************
110   'GET and PUT operations are limited to 2047 bytes
120   '*************************************************************
130   PRINT "The next line should be: 56789"
140   IF EXIST("Test.Bin") THEN KILL"Test.Bin"
150   OPEN "Test.Bin" FOR BINARY AS 1' Create A New File
160   PUT 1,"0123456789",10' Write Some Data To It
170   REWIND 1' Go To Beginning Of File
180   SEEK 1,5'Advance to file position number 6
190   GET 1,A$,5' Read The Data Into A Variable
200   CLOSE 1' Close The File
210   PRINT A$' Print The Data
220   KILL"Test.Bin"' Delete The Test File
