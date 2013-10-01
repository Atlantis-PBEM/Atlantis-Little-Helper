# Microsoft Developer Studio Project File - Name="ah" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ah - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ah.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ah.mak" CFG="ah - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ah - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ah - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "ah - Win32 Release No Python" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ah - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "$(WX2)/include" /I "$(WX2)/lib/msw" /I "$(PY24)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "HAVE_PYTHON" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /i "$(WX2)/include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 advapi32.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib winmm.lib wxmsw.lib png.lib zlib.lib jpeg.lib tiff.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"$(WX2)/lib" /libpath:"$(PY24)/libs"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "ah - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(WX2)/include" /I "$(WX2)/lib/msw" /I "$(PY24)/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D DEBUG=1 /D "__WXDEBUG__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "HAVE_PYTHON" /FR /YX"stdhdr.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /i "$(WX2)/include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 advapi32.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib winmm.lib wxmswd.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libcid.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"$(WX2)/lib" /libpath:"$(PY24)/libs"

!ELSEIF  "$(CFG)" == "ah - Win32 Release No Python"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ah___Win32_Release_No_Python"
# PROP BASE Intermediate_Dir "ah___Win32_Release_No_Python"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_No_Py"
# PROP Intermediate_Dir "Release_No_Py"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "$(WX2)/include" /I "$(WX2)/lib/msw" /I "$(PY23)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /D "HAVE_PYTHON" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /O1 /Ob2 /I "$(WX2)/include" /I "$(WX2)/lib/msw" /I "$(PY24)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x809 /i "$(WX2)/include" /d "NDEBUG"
# ADD RSC /l 0x809 /i "$(WX2)/include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib winmm.lib wxmsw.lib png.lib zlib.lib jpeg.lib tiff.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"$(WX2)/lib" /libpath:"$(PY23)/libs"
# SUBTRACT BASE LINK32 /map
# ADD LINK32 advapi32.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib winmm.lib wxmsw.lib png.lib zlib.lib jpeg.lib tiff.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrtd.lib" /libpath:"$(WX2)/lib" /libpath:"$(PY24)/libs"
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "ah - Win32 Release"
# Name "ah - Win32 Debug"
# Name "ah - Win32 Release No Python"
# Begin Group "doc"

# PROP Default_Filter "*.txt"
# Begin Source File

SOURCE=.\doc\_bugs.txt
# End Source File
# Begin Source File

SOURCE=.\doc\_todo.txt
# End Source File
# Begin Source File

SOURCE=.\doc\history.txt
# End Source File
# Begin Source File

SOURCE=.\doc\install.txt
# End Source File
# Begin Source File

SOURCE=.\doc\readme.txt
# End Source File
# End Group
# Begin Group "bitmaps"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bitmaps\centerout.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\centerout.xpm
# End Source File
# Begin Source File

SOURCE=.\bitmaps\icon_logo.ico
# End Source File
# Begin Source File

SOURCE=.\bitmaps\icon_logo.xpm
# End Source File
# Begin Source File

SOURCE=.\bitmaps\lastturn.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\lastturn.xpm
# End Source File
# Begin Source File

SOURCE=.\bitmaps\lastvisitturn.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\lastvisitturn.xpm
# End Source File
# Begin Source File

SOURCE=.\bitmaps\nextturn.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\nextturn.xpm
# End Source File
# Begin Source File

SOURCE=.\bitmaps\open.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\open.xpm
# End Source File
# Begin Source File

SOURCE=.\bitmaps\planedwn.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\planedwn.xpm
# End Source File
# Begin Source File

SOURCE=.\bitmaps\planeup.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\planeup.xpm
# End Source File
# Begin Source File

SOURCE=.\bitmaps\prevturn.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\prevturn.xpm
# End Source File
# Begin Source File

SOURCE=.\bitmaps\prevzoom.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\prevzoom.xpm
# End Source File
# Begin Source File

SOURCE=.\bitmaps\showcoord.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\showcoord.xpm
# End Source File
# Begin Source File

SOURCE=.\bitmaps\zoomin.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\zoomin.xpm
# End Source File
# Begin Source File

SOURCE=.\bitmaps\zoomout.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\zoomout.xpm
# End Source File
# End Group
# Begin Group "source"

# PROP Default_Filter "*.cpp, *.h"
# Begin Source File

SOURCE=.\ah.rc
# End Source File
# Begin Source File

SOURCE=.\ahapp.cpp
# End Source File
# Begin Source File

SOURCE=.\ahapp.h
# End Source File
# Begin Source File

SOURCE=.\ahframe.cpp
# End Source File
# Begin Source File

SOURCE=.\ahframe.h
# End Source File
# Begin Source File

SOURCE=.\atlaparser.cpp
# End Source File
# Begin Source File

SOURCE=.\atlaparser.h
# End Source File
# Begin Source File

SOURCE=.\bool.h
# End Source File
# Begin Source File

SOURCE=.\cfgfile.cpp
# End Source File
# Begin Source File

SOURCE=.\cfgfile.h
# End Source File
# Begin Source File

SOURCE=.\collection.cpp
# End Source File
# Begin Source File

SOURCE=.\collection.h
# End Source File
# Begin Source File

SOURCE=.\compat.h
# End Source File
# Begin Source File

SOURCE=.\consts.h
# End Source File
# Begin Source File

SOURCE=.\consts_ah.cpp
# End Source File
# Begin Source File

SOURCE=.\consts_ah.h
# End Source File
# Begin Source File

SOURCE=.\cstr.cpp
# End Source File
# Begin Source File

SOURCE=.\cstr.h
# End Source File
# Begin Source File

SOURCE=.\data.cpp
# End Source File
# Begin Source File

SOURCE=.\data.h
# End Source File
# Begin Source File

SOURCE=.\editpane.cpp
# End Source File
# Begin Source File

SOURCE=.\editpane.h
# End Source File
# Begin Source File

SOURCE=.\editsframe.cpp
# End Source File
# Begin Source File

SOURCE=.\editsframe.h
# End Source File
# Begin Source File

SOURCE=.\errs.cpp
# End Source File
# Begin Source File

SOURCE=.\errs.h
# End Source File
# Begin Source File

SOURCE=.\extend.cpp
# End Source File
# Begin Source File

SOURCE=.\extend.h
# End Source File
# Begin Source File

SOURCE=.\extend_no.cpp
# End Source File
# Begin Source File

SOURCE=.\files.cpp
# End Source File
# Begin Source File

SOURCE=.\files.h
# End Source File
# Begin Source File

SOURCE=.\flagsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\flagsdlg.h
# End Source File
# Begin Source File

SOURCE=.\hash.cpp
# End Source File
# Begin Source File

SOURCE=.\hash.h
# End Source File
# Begin Source File

SOURCE=.\hexfilterdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\listcoledit.cpp
# End Source File
# Begin Source File

SOURCE=.\listcoledit.h
# End Source File
# Begin Source File

SOURCE=.\listpane.cpp
# End Source File
# Begin Source File

SOURCE=.\listpane.h
# End Source File
# Begin Source File

SOURCE=.\mapframe.cpp
# End Source File
# Begin Source File

SOURCE=.\mapframe.h
# End Source File
# Begin Source File

SOURCE=.\mappane.cpp
# End Source File
# Begin Source File

SOURCE=.\mappane.h
# End Source File
# Begin Source File

SOURCE=.\msgframe.cpp
# End Source File
# Begin Source File

SOURCE=.\msgframe.h
# End Source File
# Begin Source File

SOURCE=.\objs.cpp
# End Source File
# Begin Source File

SOURCE=.\objs.h
# End Source File
# Begin Source File

SOURCE=.\optionsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\optionsdlg.h
# End Source File
# Begin Source File

SOURCE=.\stdhdr.h
# End Source File
# Begin Source File

SOURCE=.\unitfilterdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\unitfilterdlg.h
# End Source File
# Begin Source File

SOURCE=.\unitframe.cpp
# End Source File
# Begin Source File

SOURCE=.\unitframe.h
# End Source File
# Begin Source File

SOURCE=.\unitframefltr.cpp
# End Source File
# Begin Source File

SOURCE=.\unitframefltr.h
# End Source File
# Begin Source File

SOURCE=.\unitpane.cpp
# End Source File
# Begin Source File

SOURCE=.\unitpane.h
# End Source File
# Begin Source File

SOURCE=.\unitpanefltr.cpp
# End Source File
# Begin Source File

SOURCE=.\unitpanefltr.h
# End Source File
# Begin Source File

SOURCE=.\unitsplitdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\unitsplitdlg.h
# End Source File
# Begin Source File

SOURCE=.\utildlgs.cpp
# End Source File
# Begin Source File

SOURCE=.\utildlgs.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\hexfilterdlg.h
# End Source File
# End Target
# End Project
