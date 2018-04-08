100 CLS
110 PRINT "**********************************************************"
120 PRINT " Here is a sampling of Environ variables on Windows 7/8/10"
121 PRINT " that are specific to this computer at this moment in time"
130 PRINT "**********************************************************"
140 PRINT 
150 PRINT "ALLUSERSPROFILE:         ",ENVIRON$("ALLUSERSPROFILE")
160 PRINT "APPDATA:                 ",ENVIRON$("APPDATA")
170 PRINT "COMMONPROGRAMFILES:      ",ENVIRON$("COMMONPROGRAMFILES")
180 PRINT "COMMONPROGRAMFILES(x86): ",ENVIRON$("COMMONPROGRAMFILES(x86)")
190 PRINT "COMSPEC:                 ",ENVIRON$("COMSPEC")
200 PRINT "HOMEPATH:                ",ENVIRON$("HOMEPATH")
210 PRINT "LOCALAPPDATA:            ",ENVIRON$("LOCALAPPDATA")
220 PRINT "PROGRAMDATA:             ",ENVIRON$("PROGRAMDATA")
230 PRINT "PROGRAMFILES:            ",ENVIRON$("PROGRAMFILES")
240 PRINT "PROGRAMFILES(X86):       ",ENVIRON$("PROGRAMFILES(X86)")
250 PRINT "PUBLIC:                  ",ENVIRON$("PUBLIC")
260 PRINT "SystemDrive:             ",ENVIRON$("SystemDrive")
270 PRINT "SystemRoot:              ",ENVIRON$("SystemRoot")
280 PRINT "TEMP:                    ",ENVIRON$("TEMP")
290 PRINT "USERPROFILE:             ",ENVIRON$("USERPROFILE")
300 PRINT "WINDIR:                  ",ENVIRON$("WINDIR")
