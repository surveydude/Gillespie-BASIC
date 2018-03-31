10 INPUT "How Large a String Array do you want to create? " ,A
20 DIM Buffer$(A)
30 FOR I=1 TO A
40   Buffer$(I)="This is line number " +STR$(I)
50 NEXT 
60 CLS
70 FOR I=1 TO A
80   PRINT Buffer$(I)
90 NEXT 
