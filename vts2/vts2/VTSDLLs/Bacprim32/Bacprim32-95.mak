# Microsoft Developer Studio Generated NMAKE File, Based on Bacprim32-95.dsp
!IF "$(CFG)" == ""
CFG=Bacprim32 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Bacprim32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Bacprim32 - Win32 Release" && "$(CFG)" !=\
 "Bacprim32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Bacprim32-95.mak" CFG="Bacprim32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Bacprim32 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Bacprim32 - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Bacprim32 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Bacprim32-95.dll" "$(OUTDIR)\Bacprim32-95.bsc"

!ELSE 

ALL : "$(OUTDIR)\Bacprim32-95.dll" "$(OUTDIR)\Bacprim32-95.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Bacprim32.obj"
	-@erase "$(INTDIR)\Bacprim32.res"
	-@erase "$(INTDIR)\Bacprim32.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\Bacprim32-95.bsc"
	-@erase "$(OUTDIR)\Bacprim32-95.dll"
	-@erase "$(OUTDIR)\Bacprim32-95.exp"
	-@erase "$(OUTDIR)\Bacprim32-95.ilk"
	-@erase "$(OUTDIR)\Bacprim32-95.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /MT /W3 /GX /Od /I "..\includent" /D "NDEBUG" /D "WIN32"\
 /D "_WINDOWS" /FAcs /Fa"$(INTDIR)\\" /FR"$(INTDIR)\\"\
 /Fp"$(INTDIR)\Bacprim32-95.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\Release/

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Bacprim32.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Bacprim32-95.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Bacprim32.sbr"

"$(OUTDIR)\Bacprim32-95.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib vtsapi32.lib /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\Bacprim32-95.pdb"\
 /machine:I386 /def:".\Bacprim32.def" /out:"$(OUTDIR)\Bacprim32-95.dll"\
 /implib:"$(OUTDIR)\Bacprim32-95.lib" /libpath:"..\vtsapi32\release" 
DEF_FILE= \
	".\Bacprim32.def"
LINK32_OBJS= \
	"$(INTDIR)\Bacprim32.obj" \
	"$(INTDIR)\Bacprim32.res"

"$(OUTDIR)\Bacprim32-95.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Bacprim32 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Bacprim32-95.dll" "$(OUTDIR)\Bacprim32-95.bsc"

!ELSE 

ALL : "$(OUTDIR)\Bacprim32-95.dll" "$(OUTDIR)\Bacprim32-95.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\Bacprim32.obj"
	-@erase "$(INTDIR)\Bacprim32.res"
	-@erase "$(INTDIR)\Bacprim32.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\Bacprim32-95.bsc"
	-@erase "$(OUTDIR)\Bacprim32-95.dll"
	-@erase "$(OUTDIR)\Bacprim32-95.exp"
	-@erase "$(OUTDIR)\Bacprim32-95.ilk"
	-@erase "$(OUTDIR)\Bacprim32-95.lib"
	-@erase "$(OUTDIR)\Bacprim32-95.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp1 /MTd /W3 /Gm /GX /Zi /Od /I "..\includent" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /FAcs /Fa"$(INTDIR)\\" /FR"$(INTDIR)\\"\
 /Fp"$(INTDIR)\Bacprim32-95.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Bacprim32.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Bacprim32-95.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Bacprim32.sbr"

"$(OUTDIR)\Bacprim32-95.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib vtsapi32.lib /nologo\
 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\Bacprim32-95.pdb"\
 /debug /machine:I386 /def:".\Bacprim32.def" /out:"$(OUTDIR)\Bacprim32-95.dll"\
 /implib:"$(OUTDIR)\Bacprim32-95.lib" /pdbtype:sept /libpath:"..\vtsapi32\debug"\
 
DEF_FILE= \
	".\Bacprim32.def"
LINK32_OBJS= \
	"$(INTDIR)\Bacprim32.obj" \
	"$(INTDIR)\Bacprim32.res"

"$(OUTDIR)\Bacprim32-95.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "Bacprim32 - Win32 Release" || "$(CFG)" ==\
 "Bacprim32 - Win32 Debug"
SOURCE=.\Bacprim32.c

!IF  "$(CFG)" == "Bacprim32 - Win32 Release"

DEP_CPP_BACPR=\
	"..\includent\Bacprim.h"\
	"..\includent\Db.h"\
	"..\includent\PROPS.H"\
	"..\includent\SERVICE.H"\
	"..\includent\Stdobj.h"\
	"..\includent\Vtsapi.h"\
	

"$(INTDIR)\Bacprim32.obj"	"$(INTDIR)\Bacprim32.sbr" : $(SOURCE)\
 $(DEP_CPP_BACPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Bacprim32 - Win32 Debug"

DEP_CPP_BACPR=\
	"..\includent\Bacprim.h"\
	"..\includent\Db.h"\
	"..\includent\PROPS.H"\
	"..\includent\SERVICE.H"\
	"..\includent\Stdobj.h"\
	"..\includent\Vtsapi.h"\
	

"$(INTDIR)\Bacprim32.obj"	"$(INTDIR)\Bacprim32.sbr" : $(SOURCE)\
 $(DEP_CPP_BACPR) "$(INTDIR)"


!ENDIF 

SOURCE=.\Bacprim32.rc

"$(INTDIR)\Bacprim32.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

