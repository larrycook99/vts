# Microsoft Developer Studio Generated NMAKE File, Based on nalservnt.dsp
!IF "$(CFG)" == ""
CFG=nalservnt - Win32 Debug
!MESSAGE No configuration specified. Defaulting to nalservnt - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "nalservnt - Win32 Release" && "$(CFG)" !=\
 "nalservnt - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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

!IF  "$(CFG)" == "nalservnt - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\nalserv32.dll"

!ELSE 

ALL : "$(OUTDIR)\nalserv32.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\nalserv32.res"
	-@erase "$(INTDIR)\nalservnt.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\nalserv32.dll"
	-@erase "$(OUTDIR)\nalserv32.exp"
	-@erase "$(OUTDIR)\nalserv32.ilk"
	-@erase "$(OUTDIR)\nalserv32.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /Zp1 /MT /W3 /GX /Od /I "p:\VTS\VTSDLLs\Includent" /I\
 "..\..\ptp" /I "..\..\mstp" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\nalservnt.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\nalserv32.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\nalserv32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib\
 libcmt.lib oldnames.lib p:\Vts\VTSDLLs\Libs\bacprim32.lib packet32.lib /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\nalserv32.pdb"\
 /machine:I386 /nodefaultlib /def:".\nalservnt.def"\
 /out:"$(OUTDIR)\nalserv32.dll" /implib:"$(OUTDIR)\nalserv32.lib"\
 /libpath:"p:\bacdoor\bacdoor nt4\packet32\release" /libpath:"..\..\ptp\Release"\
 /libpath:"..\..\mstp\Release" 
DEF_FILE= \
	".\nalservnt.def"
LINK32_OBJS= \
	"$(INTDIR)\nalserv32.res" \
	"$(INTDIR)\nalservnt.obj"

"$(OUTDIR)\nalserv32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "nalservnt - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\nalserv32.dll"

!ELSE 

ALL : "$(OUTDIR)\nalserv32.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\nalserv32.res"
	-@erase "$(INTDIR)\nalservnt.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\nalserv32.dll"
	-@erase "$(OUTDIR)\nalserv32.exp"
	-@erase "$(OUTDIR)\nalserv32.ilk"
	-@erase "$(OUTDIR)\nalserv32.lib"
	-@erase "$(OUTDIR)\nalserv32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /Zp1 /MTd /W3 /Gm /GX /Zi /Od /I\
 "p:\VTS\VTSDLLs\Includent" /I "..\..\ptp" /I "..\..\mstp" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /FAcs /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\nalservnt.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\nalserv32.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\nalserv32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib\
 libcmt.lib oldnames.lib p:\Vts\VTSDLLs\Libs\bacprim32.lib packet32.lib ptp.lib\
 mstp.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\nalserv32.pdb" /debug /machine:I386 /nodefaultlib:"libcmt"\
 /nodefaultlib /def:".\nalservnt.def" /out:"$(OUTDIR)\nalserv32.dll"\
 /implib:"$(OUTDIR)\nalserv32.lib" /pdbtype:sept\
 /libpath:"p:\bacdoor\bacdoor nt4\packet32\debug" /libpath:"..\..\ptp\Debug"\
 /libpath:"..\..\mstp\Debug" 
DEF_FILE= \
	".\nalservnt.def"
LINK32_OBJS= \
	"$(INTDIR)\nalserv32.res" \
	"$(INTDIR)\nalservnt.obj"

"$(OUTDIR)\nalserv32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "nalservnt - Win32 Release" || "$(CFG)" ==\
 "nalservnt - Win32 Debug"
SOURCE=.\nalservnt.c

!IF  "$(CFG)" == "nalservnt - Win32 Release"

DEP_CPP_NALSE=\
	"..\..\includent\bacprim.h"\
	"..\..\includent\db.h"\
	"..\..\includent\Macp32.h"\
	"..\..\includent\Nalsapi.h"\
	"..\..\includent\ntddndis.h"\
	"..\..\includent\Packet32.h"\
	"..\..\includent\service.h"\
	"..\..\includent\stdobj.h"\
	

"$(INTDIR)\nalservnt.obj" : $(SOURCE) $(DEP_CPP_NALSE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "nalservnt - Win32 Debug"

DEP_CPP_NALSE=\
	"..\..\includent\bacprim.h"\
	"..\..\includent\db.h"\
	"..\..\includent\Macp32.h"\
	"..\..\includent\Nalsapi.h"\
	"..\..\includent\ntddndis.h"\
	"..\..\includent\Packet32.h"\
	"..\..\includent\service.h"\
	"..\..\includent\stdobj.h"\
	

"$(INTDIR)\nalservnt.obj" : $(SOURCE) $(DEP_CPP_NALSE) "$(INTDIR)"


!ENDIF 

SOURCE=.\nalservnt.rc
DEP_RSC_NALSER=\
	".\Greenled.bmp"\
	".\Nalserv.ico"\
	".\Redled.bmp"\
	

"$(INTDIR)\nalserv32.res" : $(SOURCE) $(DEP_RSC_NALSER) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

