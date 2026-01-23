10 OPEN "demo.htm" FOR OUTPUT AS 1
20 FPRINT 1,"<html><body>"
30 FOR I=1 TO 1000
40   FPRINT 1,"This is line number " ,I,"<br>"
50 NEXT
60 FPRINT 1,"</body></html>"
70 CLOSE 1
80 SHELL "demo.htm"
90 SLEEP(3000)
100 KILL "demo.htm"
