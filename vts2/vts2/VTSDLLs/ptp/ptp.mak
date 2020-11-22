# Microsoft Developer Studio Generated NMAKE File, Based on ptp.dsp
!IF "$(CFG)" == ""
CFG=ptp - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ptp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ptp - Win32 Release" && "$(CFG)" != "ptp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ptp.mak" CFG="ptp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ptp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ptp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ptp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ptp.dll"

!ELSE 

ALL : "$(OUTDIR)\ptp.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ptp.obj"
	-@erase "$(INTDIR)\ptp.res"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\ptp.dll"
	-@erase "$(OUTDIR)\ptp.exp"
	-@erase "$(OUTDIR)\ptp.lib"
	-@erase "$(OUTDIR)\ptp.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /Zp1 /MT /W3 /GX /O2 /I "..\Include" /D "WIN32" /D\
 "NDEBUG" /D "_WINDOWS" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\ptp.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ptp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ptp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib shell32.lib sjack32.lib /nologo\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\ptp.pdb"\
 /map:"$(INTDIR)\ptp.map" /machine:I386 /def:".\ptp.def"\
 /out:"$(OUTDIR)\ptp.dll" /implib:"$(OUTDIR)\ptp.lib" /libpath:"Release" 
DEF_FILE= \
	".\ptp.def"
LINK32_OBJS= \
	"$(INTDIR)\ptp.obj" \
	"$(INTDIR)\ptp.res"

"$(OUTDIR)\ptp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ptp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ptp.dll"

!ELSE 

ALL : "$(OUTDIR)\ptp.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\ptp.obj"
	-@erase "$(INTDIR)\ptp.res"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\ptp.dll"
	-@erase "$(OUTDIR)\ptp.exp"
	-@erase "$(OUTDIR)\ptp.ilk"
	-@erase "$(OUTDIR)\ptp.lib"
	-@erase "$(OUTDIR)\ptp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /Zp1 /MTd /W3 /Gm /GX /Zi /Od /I "..\Include" /D "WIN32"\
 /D "_DEBUG" /D "_WINDOWS" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\ptp.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ptp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ptp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib shell32.lib sjack32.lib /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ptp.pdb" /debug\
 /machine:I386 /def:".\ptp.def" /out:"$(OUTDIR)\ptp.dll"\
 /implib:"$(OUTDIR)\ptp.lib" /pdbtype:sept /libpath:"Debug" 
DEF_FILE= \
	".\ptp.def"
LINK32_OBJS= \
	"$(INTDIR)\ptp.obj" \
	"$(INTDIR)\ptp.res"

"$(OUTDIR)\ptp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "ptp - Win32 Release" || "$(CFG)" == "ptp - Win32 Debug"
SOURCE=.\ptp.rc
DEP_RSC_PTP_R=\
	".\GREENLED.BMP"\
	".\handshak.bmp"\
	".\noshake.bmp"\
	".\ptp.ICO"\
	".\ptptray.ico"\
	".\REDLED.BMP"\
	

"$(INTDIR)\ptp.res" : $(SOURCE) $(DEP_RSC_PTP_R) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\ptp.c

!IF  "$(CFG)" == "ptp - Win32 Release"

DEP_CPP_PTP_C=\
	"..\Include\Macp32.h"\
	".\ptp.h"\
	".\sjack32.h"\
	

"$(INTDIR)\ptp.obj" : $(SOURCE) $(DEP_CPP_PTP_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ptp - Win32 Debug"

DEP_CPP_PTP_C=\
	"..\Include\Macp32.h"\
	".\ptp.h"\
	".\sjack32.h"\
	

"$(INTDIR)\ptp.obj" : $(SOURCE) $(DEP_CPP_PTP_C) "$(INTDIR)"


!ENDIF 


!ENDIF 

