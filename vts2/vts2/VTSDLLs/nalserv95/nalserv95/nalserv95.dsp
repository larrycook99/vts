# Microsoft Developer Studio Project File - Name="nalserv95" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=nalserv95 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "nalserv95.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "nalserv95.mak" CFG="nalserv95 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nalserv95 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nalserv95 - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "nalserv95 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release95"
# PROP Intermediate_Dir "Release95"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G3 /Zp1 /MT /W3 /GX /O2 /I "..\..\Include95" /I "..\..\ptp" /I "..\..\mstp" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FAcs /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /fo"Release/Nalserv32.res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Release/nalserv32.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib libcmt.lib oldnames.lib "..\..\packet32 for 95\release95\packet32.lib" ..\..\bacprim32\release95\bacprim32.lib ..\..\ptp\release95\ptp.lib ..\..\mstp\release95\mstp.lib /nologo /subsystem:windows /dll /map /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib /out:"Release95/nalserv32.dll" /libpath:"..\..\..\..\bacdoor\bacdoor win95\packet32\release95"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "nalserv95 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug95"
# PROP Intermediate_Dir "Debug95"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G3 /Zp1 /MTd /W3 /Gm /GX /Zi /Od /I "..\..\Include95" /I "..\..\ptp" /I "..\..\mstp" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FAcs /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo"Debug/Nalserv32.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/nalserv32.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib libcmt.lib oldnames.lib "..\..\packet32 for 95\debug95\packet32.lib" ..\..\bacprim32\debug95\bacprim32.lib ..\..\ptp\debug95\ptp.lib ..\..\mstp\debug95\mstp.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib /out:"Debug95/nalserv32.dll" /pdbtype:sept /libpath:"..\..\..\..\bacdoor\bacdoor win95\packet32\debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "nalserv95 - Win32 Release"
# Name "nalserv95 - Win32 Debug"
# Begin Group "Resources"

# PROP Default_Filter "ico;bmp;rc"
# Begin Source File

SOURCE=.\Greenled.bmp
# End Source File
# Begin Source File

SOURCE=.\Nalserv.ico
# End Source File
# Begin Source File

SOURCE=.\Nalserv95.rc
# End Source File
# Begin Source File

SOURCE=.\Redled.bmp
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\..\Include\Bacprim.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Macp32.h
# End Source File
# Begin Source File

SOURCE=".\Nals-res.h"
# End Source File
# Begin Source File

SOURCE=..\..\Include\Nalsapi.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\Packet32.h
# End Source File
# Begin Source File

SOURCE=..\..\ptp\ptp.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Nalserv95.c
# End Source File
# Begin Source File

SOURCE=.\Nalserv95.def
# End Source File
# End Target
# End Project
