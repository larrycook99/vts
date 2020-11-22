# Microsoft Developer Studio Project File - Name="nalservnt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=nalservnt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "nalservnt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "nalservnt.mak" CFG="nalservnt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nalservnt - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nalservnt - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "nalservnt - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G3 /Zp1 /MT /W3 /GX /Od /I "..\..\Includent" /I "..\..\ptp" /I "..\..\mstp" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /fo"Release/nalserv32.res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Release/nalserv32.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib libcmt.lib oldnames.lib ..\..\bacprim32\release\bacprim32.lib "..\..\packet32 for NT\release\packet32.lib" ..\..\ptp\release\ptp.lib ..\..\mstp\release\mstp.lib /nologo /subsystem:windows /dll /incremental:yes /machine:I386 /nodefaultlib /out:"Release/nalserv32.dll" /libpath:"..\packet32 for NT\release"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "nalservnt - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G3 /Zp1 /MTd /W3 /Gm /GX /Zi /Od /I "..\..\Includent" /I "..\..\ptp" /I "..\..\mstp" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FAcs /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /fo"Debug/nalserv32.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/nalserv32.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib libcmt.lib oldnames.lib ..\..\bacprim32\debug\bacprim32.lib "..\..\packet32 for NT\debug\packet32.lib" ..\..\ptp\debug\ptp.lib ..\..\mstp\debug\mstp.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib /out:"Debug/nalserv32.dll" /pdbtype:sept /libpath:"..\packet32 for NT\debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "nalservnt - Win32 Release"
# Name "nalservnt - Win32 Debug"
# Begin Source File

SOURCE=.\Greenled.bmp
# End Source File
# Begin Source File

SOURCE=..\..\includent\Macp32.h
# End Source File
# Begin Source File

SOURCE=".\Nals-res.h"
# End Source File
# Begin Source File

SOURCE=..\..\includent\Nalsapi.h
# End Source File
# Begin Source File

SOURCE=.\Nalserv.ico
# End Source File
# Begin Source File

SOURCE=.\nalservnt.c
# End Source File
# Begin Source File

SOURCE=.\nalservnt.def
# End Source File
# Begin Source File

SOURCE=.\nalservnt.rc
# End Source File
# Begin Source File

SOURCE=..\..\includent\Packet32.h
# End Source File
# Begin Source File

SOURCE=.\Redled.bmp
# End Source File
# End Target
# End Project
